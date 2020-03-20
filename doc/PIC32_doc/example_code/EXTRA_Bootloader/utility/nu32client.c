// NU32 Serial Bootloader Utility 
// The boot protocol follows Microchip Application Note AN1388 PIC32 Bootloader
// Hex files are in the INTEL HEX32 format, see
//  www.microsym.com/editor/assets/intehex.pdf (accessed on 3/31/2016)
//
// Combined Windows, Mac, and Linux version
// Compile with
// > gcc nu32utility.c -o nu32utility.
// 
// To use, enter from command line: 
// > nu32utility COMPORT HEXFILE
//
// On Windows:
// > mode
// lists serial ports
// Example: 
// > nu32utility COM5 myhex.hex
//
// On Mac:
// serial port is named something like /dev/tty.usbserial-DJ00DV5V
// > nu32utility /dev/tty.usbserial-DJ00DV5V myhex.hex
//
// On Linux:
// serial port is something like /dev/ttyUSB0
// > nu32utility /dev/ttyUSB0 myhex.hex
#include <stdio.h>  // printf
#include <stdlib.h> // for strtol
#include <stdint.h>
#include <time.h>   // for time functions
#include <string.h> // string functions
#include <errno.h>

#ifndef UART_H      // uart.h and frame.h were already prepended, so we are compiling
                    // as a single file, don't include external dependencies
#include "uart.h"   // uart interface
#include "frame.h"  // bootloader frame protocol
#include "hex.h"
#endif 

// the version of the client
#define MAJOR_VERSION 5
#define MINOR_VERSION 2


#define VERSION_WAIT 1      // time to wait for bootloader version in seconds
#define ERASE_WAIT 5        // time to wait for a flash erase, in seconds
                            // could be shorter for 5.2, but must be this long for 5.0
#define PROGRAM_WAIT 2      // time to write a hex line, in seconds
#define CHECK_WAIT 2        // time to wait for a crc check on the pic32, in seconds

// get a hex record from a line in a hex file, return false on parsing error
// and out contains the record  line must be a null terminated string
// logically this would be part of common/hex.c, but it is not used by the PIC32
// and would make the bootloader not fit in program memory
static bool hex_parse(Hex_record * out, const char * hex_line) {
    size_t hex_len = strlen(hex_line); // length of the hex line

    // the line lacks the proper ending
    if(hex_line[hex_len - 1] != '\n' && hex_line[hex_len - 1] != '\r') {
      return false;
    }

    // the line is too long to be a hex file line
    if(strlen(hex_line) >= HEX_MAX_LINE) {
      return false;
    }

    // every hex line starts with a : and has a minimum length
    if(hex_len <  3 || hex_line[0] != ':') {
      return false;
    }
    // build the message, translate two character ASCII hex into hex bytes

    char hex_num[3] = {0}; // two characters + null, stores the current hex number
    size_t ldex = 1;       // index into the line, skipping the ':'
    size_t byte_loc = 0;   // which byte we are parsing
    
    uint8_t binhex[HEX_MAX_RECORD] = {0}; // the hex line, with ascii converted to binary

    // we know the line ends with '\r' or '\n' 
    while(hex_line[ldex] != '\r' && hex_line[ldex] != '\n') {
      hex_num[(ldex - 1) % 2] = hex_line[ldex];
      if(ldex % 2 == 0) { // we have a full hex number
        errno = 0;       // use errno to check for error when converting
        long hex_byte = strtol(hex_num, NULL, 16); // convert the ascii to a byte
        if(errno != 0 || hex_byte < 0 || hex_byte > 0xFF) {
          return false; // the two chars from the file is not a valid hex number
        }
        binhex[byte_loc] = hex_byte;
        ++byte_loc;
      }
      ++ldex;
    }
    if(ldex % 2 != 1) { // there were not an even number of characters
      return false;
    }

    // convert the string of bytes into a hex record
    return hex_deserialize(out, binhex, byte_loc) != -1;
} 

// attempt to read  a frame for delay seconds. if data is not read by delay seconds
// return an error -1, otherwise return the number of bytes in data
static int await_response(uint8_t * data, size_t len, unsigned int timeout) {
  time_t start_time = time(NULL);
  int rlen = FRAME_READ_TIMEOUT;
  while (rlen == FRAME_READ_TIMEOUT &&  difftime(time(NULL), start_time) < timeout) {
    rlen = frame_read(data, len);
  }
  return rlen < 1 ? -1 : rlen;
}

// send a command and wait timeout seconds for a response. on error, exit the program
// message - command to send
// mlen - length of message
// timeout - time to wait for response
// errmsg - what to print on error
// return the data associated with the message (if any).  all the commands that return
// data only return two bytes. data is little endian  
static uint16_t command_response(
    const uint8_t message[], size_t mlen, unsigned int timeout, const char * errmsg) {
  // send the command
  if(frame_write(message, mlen) < 0) {
    puts(errmsg);
    exit(EXIT_FAILURE);
  }

  uint8_t response[3] = {0}; // stores the response received from the PIC
  int response_length = await_response(response, sizeof(response), timeout);
  if(message[0] == Frame_version || message[0] == Frame_crc) {
    // these commands get data as a response
    if(response_length != 3 || response[0] != message[0]) {
      puts(errmsg);
      exit(EXIT_FAILURE);
    }
    // convert the lsb and msb into a uint_16
    return ((uint16_t)response[1]) | (((uint16_t)response[2]) << 8);
  }
  else {
    // all other commands merely get an acknowledgment
    if(response_length != 1 || response[0] != message[0]) {
      puts(errmsg);
      exit(EXIT_FAILURE);
    }
    return 0;
  }
}

// read the next hex record from the file, returning it in out.
// if out is null the line will be skipped.  If EOF is reached return false,
// and rewind the file to the beginning. exit the program on error
static bool next_hex_record(FILE * file, Hex_record * out) {
  char hex_line[HEX_MAX_LINE] = {0}; // a line in the hex file
  char * result = fgets(hex_line, sizeof(hex_line), file);
  if(!result) {
    // there was an error reading from the hex file
    if(ferror(file)) {
      printf("Error: could not read hex file!\n");
      exit(EXIT_FAILURE);
    }
    
    // seek to the start of the file
    if(fseek(file, 0, SEEK_SET) != 0) {
      printf("Error: could not read hex file!\n");
      exit(EXIT_FAILURE);
    }
    return false;
  } else if(out) {
      if(!hex_parse(out, hex_line)) { // the hex file is not valid
        printf("Error: invalid hex file!\n");
        exit(EXIT_FAILURE);
      }
  }
  return true;
}

// print a status bar
static void status_bar_update(unsigned int curr, unsigned int total) {
  static unsigned int percent = 0;
  // update the status bar
  if( (100*curr)/total > percent) {
    printf("*");
    fflush(stdout);
    percent += 10;
  }
  if(curr == total - 1) {
    percent = 0;
  }
}

// call this program with 2 arguments: 
//      the name of the serial port, and the location of the hex file
int main(int argc, char *argv[]) {
  // check if the right number of args
  printf("NU32 Bootloader Utility Version %d.%d\n", 
      MAJOR_VERSION, MINOR_VERSION);

  // some command line options, set to defaults
  int port_index = -1, hex_file_index = -1;
  bool program = true, verify = false;
  // parse command line arguments (unfortunately getopt is not on windows...)
  { 
    int i = 1; // start parsing after the program's name
    for(i = 1; i != argc; ++i) {
      if(strncmp(argv[i], "-w", 2) == 0) {
        program = true;
        verify = true;
      } else if(strncmp(argv[i], "-c",2) == 0) {
        program = false;
        verify = true;
      }
      else if(port_index == -1) {
        port_index = i;
      } else if(hex_file_index == -1) {
        hex_file_index = i;
      }
    }
  }

  if(port_index == -1 || hex_file_index == -1) { // wrong number of arguments
    printf("Usage: nu32utility [OPTION]... COMPORT HEXFILE\n");
    printf("Write HEXFILE to the PIC32 via COMPORT.\n\n");
    printf("  COMPORT - Serial port to use.\n");
    printf("  HEXFILE - Hex file to load.\n\n");
    printf("Options:\n");
    printf("  -c     Check that the program on the PIC32\n"
           "         matches the HEXFILE. The HEXFILE is\n"
           "         not written to the PIC32.\n\n"); 
    printf("  -w     Write HEXFILE to the PIC32 and     \n"
           "         perform the -c check.\n\n");
    printf("Example:\n nu32utility %s myhex.hex\n", uart_sample_name);
    exit(EXIT_FAILURE);
  }

  // open the serial port (program will exit on error)
  uart_open(argv[port_index]);
  printf("> Using serial port %s.\n",argv[port_index]); 
  

  uint8_t message[FRAME_DATA_SIZE] = {0};  // stores the data to send to the PIC
  
  // send request for bootloader info
  printf("> Requesting bootloader version...\n");
  message[0] = Frame_version; // bootloader request command

  // send command and get version number
  uint16_t version = command_response(message, 1, VERSION_WAIT,
      "Error: could not detect PIC32 bootloader!");
 
  printf("Bootloader detected, version %u.%u\n", 
      (uint8_t)version, (uint8_t)(version >> 8));
  
  printf("> Using hex file %s.\n", argv[hex_file_index]);
  // open the hex file, get number of lines
  FILE * hexfile = fopen(argv[hex_file_index],"r");
  if(!hexfile) {
    printf("Error: could not open hex file %s!\n", argv[hex_file_index]);
    exit(EXIT_FAILURE); 
  }


  unsigned int lines = 0; // count the number of lines in the hex file

  // loop through file, get number of hex lines
  while(next_hex_record(hexfile, NULL)) {
    ++lines;
  }

  printf("Hex file has %d lines.\n",lines);



  if(program) { // if we are only verifying, don't erase the program!
    // send request to erase flash 
    printf("> Requesting flash erase...\n");
    message[0] = Frame_erase;
    
    // note: version 5.0 of the bootloader requires a much longer wait than version 5.1
    // the main consequence is it takes the utility longer to notice if the pic32 is
    // disconnected mid erase
    command_response(message, 1, ERASE_WAIT, "Error: could not erase flash!");
    
    printf("Flash erased!\n");

  
    printf("> Programming...\n");
    printf("||||||||||\n"); // each bar represents 10% complete
    
    unsigned int curr_line = 0 ; // current line & current percent programmed
    unsigned int msg_index = 1;              // where in message[] to write next hex byte
    
    message[0] = Frame_program;              // all messages are Program frames

    Hex_record record = {0};
    while(next_hex_record(hexfile, &record)) { // loop through the hex records
      
      // if we can't fit any more records into this frame, write the frame to the pic32
      if(msg_index + hex_serialize(NULL, &record) > sizeof(message)) {
        command_response(message, msg_index, PROGRAM_WAIT,"Error: could not program!");
        msg_index = 1;
      }
      
      uint32_t pa = hex_address(&record);
      // skip sending any data records that are in the boot flash
      if(! (record.type == HEX_DAT && hex_pa_in_boot_flash(pa)) ) {
        msg_index += hex_serialize(&message[msg_index], &record);
      }
      status_bar_update(curr_line, lines);
      ++curr_line;
    }
    
    // if there is any unwritten frame write it now
    if(msg_index > 1) {
      command_response(message, msg_index, PROGRAM_WAIT, "Error: could not program!");
    }

    printf("\nProgramming complete!\n");
  }

  // verify the hex file contents
  if(verify) {
    unsigned int curr_line = 0;
    printf("> Verifying program...\n");
    printf("||||||||||\n");
    message[0] = Frame_crc;  // checksum command

    uint32_t base_addr = 0;  // current base address 
    Hex_record record = {0}; // the curent hex record
 
    while(next_hex_record(hexfile, &record)) {
      // get the physical address of the data start
      uint32_t pa = hex_address(&record);

      // if this is a data record that is not in boot flash
      // (note, the current program linker script still places some data in
      // bootflash, which the bootloader ignores, so it won't match, for compatibility
      // with bootloader version 5.0 we need to check the address on the client)
      if(record.type == HEX_DAT && !hex_pa_in_boot_flash(pa)) {  
        uint32_t check_address = HEX_PA_TO_VA(pa); // get the kseg0 VA of the data
        uint32_t check_length = record.byte_count;
        // the first part of the message is the address, in little endian
        // code below does not depend on endianess of the machine
        message[1] = (uint8_t)check_address;          // Byte 0 (LSB)
        message[2] = (uint8_t)(check_address >> 8);   // Byte 1
        message[3] = (uint8_t)(check_address >> 16);  // Byte 2
        message[4] = (uint8_t)(check_address >> 24);  // Byte 3 (MSB)
        message[5] = (uint8_t)check_length;           // Byte 0 (LSB)
        message[6] = (uint8_t)(check_length >> 8);    // Byte 1
        message[7] = (uint8_t)(check_length >> 16);   // Byte 2
        message[8] = (uint8_t)(check_length >> 24);   // Byte 3 (MSB)
        uint16_t expected_crc = frame_crc16(record.data, check_length);

        uint16_t actual_crc = 
          command_response(message, 9, CHECK_WAIT, "Error: could not get CRC16!");
        if(actual_crc != expected_crc) {
          printf("Error: hex file does not match!\n");
          exit(EXIT_FAILURE);
        }
  
      }
      status_bar_update(curr_line, lines);
      ++curr_line;
    }
    printf("\nVerification complete!\n");
  }

  // jump to application
  printf("> Jumping to program...\n");

  message[0] = Frame_jump;
  if(frame_write(message, 1) < 0) {
    printf("Error: could not jump to program!\n");
    exit(EXIT_FAILURE);
  }

  printf("Jumped to program!\n");
  
  return EXIT_SUCCESS;
}

