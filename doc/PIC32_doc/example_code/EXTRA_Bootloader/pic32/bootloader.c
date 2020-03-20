// NU32 Serial bootloader for Embedded Computing and Mechatronics with the 
// PIC32 Microcontroller, by Lynch, Marchuk, and Elwin
// The boot protocol follows Microchip Application Note AN1388 PIC32 Bootloader
#include "NU32.h"
#include "frame.h"  // the communication protocol between the pic and the utility
#include "hex.h"    // handles hex records
#include "flash.h"  // flash memory driver
#include <stdbool.h>
#include <sys/kmem.h>

// reset address, where the bootloader jumps when it is finished.
// Start of cacheable program flash, plus 0x1000 bytes for the interrupt table
//  (exception_mem in the bootloader script) and
//  0x970 bytes for startup code (kseg_0_boot_mem)
//  We execute startup code from here
#define PROGRAM_RESET_VA (PA_TO_KVA0(FLASH_PF_START_PA) + 0x1000 + 0x970)

#define CORE_TIMER_FREQ (NU32_SYS_FREQ/2) // core timer frequency
#define ERROR_BLINK_RATE 2 // frequency of error flash, in hz
#define LED_BLINK_RATE 5 // rate of LED blinking, in hz


// version info, MAJOR_VERSION.MINOR_VERSION
// note that using certain values will damage
// compatibility with nu32utility 5.0 
// due to how that utility handles escape characters (bootloader won't be detected)
#define MAJOR_VERSION 5
#define MINOR_VERSION 2



// call this function on error. it will flash leds a few times and carry on.
// it will also attempt to clear flash errors.  if the flash error can not be cleared
// it will loop forever, as this indicates an issue with the hardware.
// blinks the specified number of times (very useful for debugging)
static void error(int blinks) {
  int i = 0;
  NU32_LED1 = 0;
  NU32_LED2 = 0;

  do {
    for(i = 0; i != blinks; ++i) {
      unsigned int tmr = _CP0_GET_COUNT();
      while(_CP0_GET_COUNT() - tmr < CORE_TIMER_FREQ/ERROR_BLINK_RATE) { ; } // delay
      NU32_LED1 = !NU32_LED1;
      NU32_LED2 = !NU32_LED2;
    }
  } while(!flash_clear_error());
}


// frame_write with error checking 
static void send_response(const uint8_t * data, int len) {
  if(frame_write(data, len) < len) {
    error(1);
  }
}

// write an array of data to flash, return false on error
static bool write_data(uint32_t address, uint8_t * data, uint8_t len) {
  unsigned int i = 0;
  for(i = 0; i < len; i += 4) {
    uint32_t word = 0xFFFFFFFF; // create a blank flash word (data is 
    // erased to zero
    if(len - i >= 4) {
      memcpy(&word, &data[i], 4);
    } else {
      memcpy(&word, &data[i], i);
    }
    if(!flash_write_word(address + i, word)) {
      return false;
    }
  }
  return true;
}

int main(void)
{
  // to compensate for electrical stability issues on some boards, we must
  // delay prior to illuminating any LEDs. The board works with only a 1ms delay,
  // but we use 10ms to be safe. Version 5.0 had a 100ms delay
  _CP0_SET_COUNT(0);
  while(_CP0_GET_COUNT() < CORE_TIMER_FREQ/100) {;}

  // setup the cache and reclaim pins.  This call here is why calling NU32_Startup()
  // is redundant for bootloaded programs (but it lets them work as standalone too)!
  NU32_Startup();
  NU32_LED2 = 1;
  // Enter firmware upgrade mode if the user button is pressed or the program is missing
	if(!NU32_USER || *(unsigned int *)PROGRAM_RESET_VA == 0xFFFFFFFF) {
    
    unsigned int blink_time = _CP0_GET_COUNT();
    uint8_t frame[FRAME_DATA_SIZE] = {0};
    uint8_t response[3] = {0};  // all responses from the PIC32 are 3 bytes, max
    bool complete = false;
    
    // note on error handling: if there is an error we flash both leds for a little
    // and then continue.  We do not respond to the bootloader
    while (!complete) {
      int frame_len = frame_read(frame, sizeof(frame));

      if(frame_len == FRAME_READ_ERROR) { // the bootloader utility sent a bad frame
        error(1);  // indicate an error
      } else if(frame_len == FRAME_READ_TIMEOUT) { // we did not get a complete frame yet
        // update the blinking LED
        if(_CP0_GET_COUNT() - blink_time >= (CORE_TIMER_FREQ/LED_BLINK_RATE) ) {
          NU32_LED1 = !NU32_LED1;
          blink_time = _CP0_GET_COUNT();
        }
      } else { // handle the frame, depending on the command
        response[0] = frame[0]; // the response command is always the same as 
                                // what we have received from the utility
        // the first byte of the frame is the command
        switch(frame[0]) {
          case Frame_version: // Read boot loader version info.
            // reply with the version info
            response[0] = Frame_version;
            response[1] = MAJOR_VERSION;
            response[2] = MINOR_VERSION;
            send_response(response,3);
            break;
            
          case Frame_erase:
            if(!flash_erase_all()) {
                error(3); // we failed to erase flash, don't send response
            } else {      
              send_response(response, 1);
            }
            break;
          
          case Frame_program:
          {
            // program frame can contain multiple hex records
            Hex_record record = {0};
            int rlen = 0; // length, in bytes, of all records read from current frame
            bool eflag = false; // set if there was an error
            
            while(!eflag && rlen != frame_len - 1) { // while no errors and more records
              rlen += hex_deserialize(&record, &frame[1 + rlen], frame_len - 1 - rlen);
              if(rlen == -1) { // invalid hex record
                eflag = true;
              } else {
                uint32_t address = hex_address(&record); // physical address to write
                if(record.type == HEX_DAT) { // this is a data record
                  if(flash_valid_pf_pa(address)) {
                    // start the process of writing to flash
                    if(!write_data(address, record.data, record.byte_count)) {
                      eflag = true; // error occurred
                    }
                  } // else: it is not an error to send data at an invalid address
                  // for compatibility with utility version 5.0. we simply ignore it  
                } 
              }
            } 

            if(eflag) { // an error occurred
              error(2);
            } else {
              send_response(response, 1); // only respond if there no error
            }
            break;
          } 
          case Frame_crc: // verify checksum of an address
          { 
            if(frame_len == 9 ) {
              // frame[1..4] is the address to check, in little endian, it is a VA
              // frame[5...8] is the length of the data to check, in little endian
              // the PIC32 is little endian as well, so we can just copy data
              uint32_t  address = 0;
              memcpy(&address, &frame[1], sizeof(address));
              // note: the address here is a VA (to make the utility compatible with 5.0)
              if(flash_valid_pf_pa(KVA_TO_PA(address))) { // ensure the address is flash
                                                   // word aligned and in program memory
                uint32_t length = 0;
                memcpy(&length,  &frame[5], sizeof(length));
              
                uint16_t crc = frame_crc16((uint8_t*)address, length);
                memcpy(&response[1], &crc, 2); // respond with the crc
                send_response(response, 3);

              } else {
                error(2); // the utility should have sent only valid VAs
              }
            } else {
              error(2); // incorrect crc request from client
            }
            break;
          }  
          case Frame_jump:
            // we are ready to run the program
            complete = true;
            break;
          default:
            // unexpected command
            error(2);
            break;
        }
      }
    }
	}

	// run the application.  Create a function pointer to the reset address and call it
	void (*run_program)(void) = (void (*)(void))PROGRAM_RESET_VA;
  run_program();
	return 0;
}			



