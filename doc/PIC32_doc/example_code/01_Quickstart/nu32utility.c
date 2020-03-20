#ifndef FRAME_H
#define FRAME_H
// Functions and data types for the bootloader protocol
// data is sent between the bootloader and the utility as a series of frames
// see Microchip Application Note AN1388 Appendix B for details
// This code is shared by the bootloader on the PIC and the utility
#include<stddef.h>
#include<stdint.h>

// special control characters
typedef enum { Frame_soh = 0x01, // start of heading (a.k.a start of frame)
               Frame_eot = 0x04, // end of transmission (a.k.a end of frame)
               Frame_dle = 0x10  // data link escape (a.k.a escape character)
             } frame_control_t; 

// commands to send to the bootloader
typedef enum { Frame_version = 0x01, // bootloader version command
               Frame_erase   = 0x02, // erase flash command
               Frame_program = 0x03, // program flash command
               Frame_crc     = 0x04, // CRC checksum command
               Frame_jump    = 0x05  // jump to application command
             } frame_command_t;

// get the nth byte of a multi byte value. 0 is the least significant byte
#define FRAME_BYTE(byte, n) ((uint8_t) (byte) >> ((n) * 8))

// the max size of the data part of a frame, excluding escape and control characters
#define FRAME_DATA_SIZE 512

// calculate the CRC16 checksum of the data in an array
uint16_t frame_crc16(const uint8_t * data, size_t length); 

// send a data frame over the serial port, return -1 on serial write error. 
// the data buffer here is just the raw data for the frame and len is the length
// of that data.  the SOH, EOT, CHECKSUM and any escaping are performed by frame_write
// len should be less than FRAME_DATA_SIZE
int frame_write(const uint8_t * data, size_t len);

#define FRAME_READ_TIMEOUT -2 // timeout when reading part of a frame
#define FRAME_READ_ERROR -1   // data received when reading a frame is erroneous
// read a frame from the serial port, return FRAME_READ_TIMEOUT on timeout, 
// return FRAME_READ_ERROR on error, otherwise return the length of the frame
// if the read times out, subsequent calls to frame_read will resume 
// reading into the data array data where the last call started
// len should be less than frame data size
int frame_read(uint8_t * data, size_t len);

#endif
#ifndef UART_H
#define UART_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
// generic interface to a uart, implemented with different uart_<platform>.c files

// read at most len bytes from the uart and return the number of bytes read 
// or -1 if no data is available
int uart_read(uint8_t * data, size_t len);

// write the data to uart and return the number of bytes written or -1 on error
int uart_write(const uint8_t * data, size_t len);

// open the uart, any failure will exit(1)
void uart_open(const char port_name[]); 

// close the serial port. only needed as a workaround in posix, since the ports
// are closed when the program exits
void uart_close();

// sample name for a com port on the given platform
extern const char uart_sample_name[];
#endif
#ifndef HEX_FILE_H
#define HEX_FILE_H
// file for handling intel HEX32 records
// for details see  PIC32 Flash Programming Specification
#include <stdint.h>
#include <stdbool.h>

#define HEX_MAX_DATA 255 // maximum length of the data field

#define HEX_MAX_RECORD (HEX_MAX_DATA+5) // maximum size, in bytes, of a hex record

// the maximum length (in characters) of an intel hexfile line. 
// 2 per data byte + ':' '\r' '\n' '\0' characters
#define HEX_MAX_LINE (HEX_MAX_RECORD*2) + 4

// convert a PA to a KSEG0 VA (redundant with sys/kmem.h, but this is for PC use)
#define HEX_PA_TO_VA(x) ((x) | 0x80000000)

typedef enum {    // the type of hex record
  HEX_DAT = 0x00, // 
  HEX_EOF = 0x01, // end of file
  HEX_ESA = 0x02, // extended segment address
  HEX_SSA = 0x03, // start segment address (not used by PIC32)
  HEX_ELA = 0x04, // extended linear address
  HEX_SLA = 0x05, // start linear address (not used by PIC32)
} Hex_record_t;

typedef struct {
  uint8_t checksum;           // checksum for data integrity
  uint8_t byte_count;         // the number of bytes in the data field
  uint16_t address;           // the address field
  Hex_record_t type;          // the type of the data
  uint8_t data[HEX_MAX_DATA]; // the data field;
} Hex_record;

// convert the hex record to an array of bytes, and return the length
// if out = NULL this will just return the required number of bytes in the buffer.  
// The maximum number of bytes that a record can be is HEX_MAX_RECORD
int hex_serialize(uint8_t * out, const Hex_record * in);

// convert a byte array to a hex record.  out is the record to output
// data contains 1 or more full hex records, and len is the length of the data
// return the number of bytes processed to find the next hex record, or -1 on error
int hex_deserialize(Hex_record * out, uint8_t data[], unsigned int len);

// gets the physical address for the next hex record, assuming the records
// are processed sequentially. an EOF record resets the current address
uint32_t hex_address(const Hex_record * rec);

// check if the physical address is in boot flash on a PIC32MX
bool hex_pa_in_boot_flash(uint32_t pa);

#endif
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

#ifndef FRAME_H // when we are in single-file mode, FRAME_H will be defined earlier
#include "frame.h" // so we can avoid including this header
#include "uart.h"
#endif

#include <stdbool.h>
#include <stdlib.h>

// maximum size of a data frame, including control and escape characters
#define FRAME_MAX_SIZE (FRAME_DATA_SIZE * 2)

uint16_t frame_crc16(const uint8_t * data, size_t length) {
  const uint16_t crc_table[16] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
  };
  
  uint16_t crc = 0, reg = 0;
  size_t i = 0;
  for(i = 0; i != length ; ++i) {
    reg = (crc >> 12) ^ (data[i] >> 4);
    crc = crc_table[reg & 0x0F] ^ (crc << 4);
    reg = (crc >> 12) ^ data[i];
    crc = crc_table[reg & 0x0F] ^ (crc << 4);
  }
  return crc;
}

// return true if the data is a control character
static bool is_control_char(uint8_t data) {
  return data == Frame_soh || data == Frame_eot|| data == Frame_dle;
}

// write a uint8_t to the frame and escape it if it is a special character.
// return the number of uint8_ts written
// frame - data frame we are constructing
// data - the uint8_t to write.
// pos - posiition at which to write the data, pos must not go out of bounds from data
// esc - if true escape the character if it is a control character
static size_t frame_byte(uint8_t frame[], uint8_t data, size_t pos, bool esc)
{
  bool escaped = !esc;
  do
  {
    // the data is a control character and needs to be escaped
    if(!escaped && is_control_char(data)) {
      frame[pos] = Frame_dle;
      escaped = true;
    } else {
      frame[pos] = data;
      escaped = false;
    }
    ++pos;
  } while(escaped);

  return pos;
}


int frame_write(const uint8_t * data, size_t len) {
  uint8_t frame[FRAME_MAX_SIZE] = {0};
  frame[0] = Frame_soh;
  
  size_t findex = 1, dindex = 0; // frame and data indices
  for(dindex = 0; dindex != len; ++dindex) {
    // write the uint8_t to the next part of the frame, and escape when appropriate
    findex = frame_byte(frame, data[dindex], findex, true);
  }

  // compute the CRC checksum on the original data
  unsigned int crc = frame_crc16(data, len); 
 
  // write the LSB of the crc checksum
  findex = frame_byte(frame, (uint8_t)crc, findex, true);
 
  // write the FSB of the crc checksum
  findex = frame_byte(frame, (uint8_t)(crc >> 8), findex, true);

  // don't escape the last EOT
  findex = frame_byte(frame, Frame_eot, findex, false); 
  return uart_write(frame, findex);
}

// read a frame from the serial port (up to len uint8_ts) and store the data portion
// in data. the checksum will be verified. 
// return the length of the data read 
// return FRAME_READ_ERROR if there is an error reading the frame
// return FRAME_READ_TIMEOUT if the read operation times out, in which case
// calling this function again will cause the read to resume from the current spot
// in the frame
int frame_read(uint8_t * data, size_t len) {
  // a frame this large ensures we will always read a complete frame
  static uint8_t checksum[2] = {0,0}; // store the checksum
  static bool escaped = false;
  static size_t dindex = 0; // frame data index
  static size_t cindex = 0; // checksum index
  static bool midframe = false;  // true when we are in the middle of parsing a frame
  uint8_t frame[FRAME_MAX_SIZE] = {0}; 

  if(!midframe) // reset the state of the function
  {
    checksum[0] = 0;
    checksum[1] = 0;
    escaped = false;
    dindex = 0;
    cindex = 0;
  }

  int nread = uart_read(frame, sizeof(frame)); 
  if(nread < 1) { // error (possibly a timeout) reading from the port
    return FRAME_READ_TIMEOUT;
  }
  int i = 0;
  for(i = 0; i != nread; ++i)
  {
    uint8_t nextb = frame[i];
    if(!midframe) { // we have read the first byte of a frame
      if(nextb != Frame_soh) {
        midframe = false;
        return FRAME_READ_ERROR;// the byte was not the start byte, which is an error
      }
      midframe = true; // we are now parsing a frame
    } else {
      if(!escaped && is_control_char(nextb)) { // handle the control character
        switch(nextb) {
          case Frame_soh: // start of frame, already read this so finding it here is an error
            midframe = false;
            return FRAME_READ_ERROR;
            break;
          case Frame_eot: // end of the frame
            midframe = false; // no matter what, the frame is over
            if(cindex < 2) { // did not read a full checksum, an error
              return FRAME_READ_ERROR;
            }
            // verify the checksum
            uint16_t crc = checksum[1];
            crc = (crc << 8) | checksum[0];
            if(frame_crc16(data, dindex) != crc) { // checksum is incorrect
              return FRAME_READ_ERROR;
            }
            return dindex;
            break;
          case Frame_dle: // escape character
            escaped = true;  // escape the next character
            break;
          default: // should never get here
            midframe = false;
            return FRAME_READ_ERROR;
        }
      } else { // normal character. data is read into a checksum buffer and then shifted
        // into the data array. When EOT is encountered, checksum contains the CRC 
        if(cindex < 2) { // have not filled the checksum buffer yet
          checksum[cindex] = nextb;
          ++cindex;
        } else { // shift data from checksum buffer
          if(dindex >= len) { // there is not enough room in the data buffer
            midframe = false;
            return FRAME_READ_ERROR;
          }
          data[dindex] = checksum[0];
          checksum[0] = checksum[1];
          checksum[1] = nextb;
          ++dindex;
        }
        escaped = false;
      }
    }
  }
  return FRAME_READ_TIMEOUT; // if we got here we did not complete the read
}

#ifndef  HEX_FILE_H
#include "hex.h"
#endif

#include <string.h>

#define BOOT_FLASH_START_PA 0x1FC00000                   // start of boot flash
#define BOOT_FLASH_END_PA (BOOT_FLASH_START_PA + 0x3000) // 12kb boot flash


int hex_serialize(uint8_t * out, const Hex_record * in) {
  int len = in->byte_count + 5;
  if(!out) { // return the length of the buffer needed to store the record
    return len;
  }
  out[0] = in->byte_count;
  out[1] = in->address >> 8; // address is stored big endian and goes out that way too
  out[2] = (uint8_t)in->address;
  out[3] = in->type;
  memcpy(&out[4], in->data, in->byte_count); // copy the rest of the data
  out[len - 1] = in->checksum;
  return len;
}

int hex_deserialize(Hex_record * out, uint8_t data[], unsigned int len) {
  if(len < 1) {
    return -1;
  }
  out->byte_count = data[0];
  // total length is the number of data bytes (given by HEX_MAX_RECORD - HEX_MAX_DATA)
  // plus the byte count.
  unsigned short total_len = (HEX_MAX_RECORD - HEX_MAX_DATA) + out->byte_count;
  if(len < total_len ) { // the length of the record is incorrect
    return -1;
  }

  // hex records are big endian
  out->address = (uint16_t)(data[1] << 8) | (uint16_t)data[2]; 
  out->type = data[3]; // 3rd byte is the data type
  memcpy(out->data, &data[4], out->byte_count);
  out->checksum = data[total_len - 1]; // the last byte in the stream is the checksum

  // data is is read in, now some more validation based on record type
  switch(out->type) {
    case HEX_DAT:
      break; // any data is good (not going to validate machine code!)
    case HEX_EOF:
      if(out->address != 0 || out->byte_count != 0) {
        return -1;
      }
      break;
    case HEX_ESA:
      if(out->address != 0 || out->byte_count != 2) {
        return -1;
      }
      break;
    case HEX_SSA:
        return -1; // not used by the PIC32
        break;
    case HEX_ELA:
      if(out->address != 0 || out->byte_count != 2) {
        return -1;
      }
      break;
    case HEX_SLA:
        return -1; // not used by the PIC32 
        break;
    default:
        return -1;
        break;
  }
  // validate the checksum
  unsigned int i = 0;
  uint8_t sum = 0;
  for(i = 0; i != total_len; ++i) {
    sum += data[i];
  }
  return sum == 0 ? total_len : -1; // sum should be zero if checksum is correct
}

uint32_t hex_address(const Hex_record * rec) {
  static uint32_t curr_addr = 0;
  switch(rec->type) {
    case HEX_EOF:
      curr_addr = 0;
      break;
    case HEX_ESA:
      // data[0] = B2 and data[1] = B1
      curr_addr = ((uint32_t)rec->data[0]  << 16) | ((uint32_t)rec->data[1] << 8); 
      break;
    case HEX_ELA:
      // data[0] = B3 and data[1] = B2
      curr_addr = ((uint32_t)rec->data[0] << 24) | ((uint32_t)rec->data[1] << 16);
      break;
    case HEX_DAT: // curr_addr is not updated, but the full address is known
      return curr_addr + rec->address;
      break;
    default:
      ; // unhandled hex record don't update the address
  }
  return curr_addr;
}

bool hex_pa_in_boot_flash(uint32_t pa) {
  return pa >= BOOT_FLASH_START_PA && pa <= BOOT_FLASH_END_PA;
}
// uart code that will only be compiled on windows
#if (defined(_WIN32)) || (defined(_WIN64)) // we are on windows

#include <windows.h>                     // for serial and time functions
#include <stdio.h>

const char uart_sample_name[] = "COM5";  // sample COM port on windows

static HANDLE port = INVALID_HANDLE_VALUE;

void uart_open(const char port_name[])  // open a serial port (using the windows API)
{
  port = CreateFile(port_name,
                    GENERIC_READ | GENERIC_WRITE, // read and write access
                    0,         // other processes cannot access the port
                    NULL,          // no security settings
                    OPEN_EXISTING, // open only if it exists
                    FILE_ATTRIBUTE_NORMAL, // non overlapped
                    NULL);

  if (port == INVALID_HANDLE_VALUE) { // the function failed
    printf("Invalid serial port.\n");
    exit(EXIT_FAILURE);
  }

  // set NU32 serial port parameters - 230400 baud, 8-1-n, RTS and CTS
  DCB dcbSerialParams = {0}; // parameters are zero by default
  dcbSerialParams.DCBlength = sizeof(DCB);

  // get the current (default) configuration
  if (!GetCommState(port, &dcbSerialParams)) {
    printf("Could not get serial port state.\n");
    exit(EXIT_FAILURE);
  }

  dcbSerialParams.BaudRate = 230400;      // set the baud rate
  dcbSerialParams.fBinary = TRUE;         // must be true
  dcbSerialParams.fParity = FALSE;        // no parity checking
  dcbSerialParams.ByteSize = 8;           // 8 bits in a byte
  dcbSerialParams.StopBits = ONESTOPBIT;  // one stop bit
  dcbSerialParams.Parity = NOPARITY;      // no parity
  dcbSerialParams.fRtsControl = RTS_CONTROL_HANDSHAKE; // use RTS flow control
  dcbSerialParams.fOutxCtsFlow = TRUE;                 // use CTS flow control
  if(!SetCommState(port, &dcbSerialParams)){
    printf("Error configuring serial port.\n");
    exit(EXIT_FAILURE);
  }
  // play with these if you get errors (numbers too small)
  // or if performance is too slow (numbers too big)
  COMMTIMEOUTS timeouts={0};
  timeouts.ReadIntervalTimeout = MAXDWORD; // return immediately with bytes available
  timeouts.ReadTotalTimeoutMultiplier = 0; 
  timeouts.ReadTotalTimeoutConstant = 0; 
  timeouts.WriteTotalTimeoutMultiplier = 2; // 2 ms per byte
  timeouts.WriteTotalTimeoutConstant = 100; // an additional 100ms total
  if(!SetCommTimeouts(port, &timeouts)) {
    printf("Error setting serial timeouts.\n");
    exit(EXIT_FAILURE);
  }
}

// serial read function, mimics the interface to POSIX (e.g., linux/mac) read 
int uart_read(byte * data , size_t length) {
  DWORD nread = 0;
  if(!ReadFile(port, data, length, &nread, NULL)) {
    return -1;
  }
  return nread;
}

  // serial write function, mimics interface to POSIX (e.g., linux/mac) write function
int uart_write(const byte * data, size_t length) {
  DWORD nwritten = 0;
  if(!WriteFile(port, data, length, &nwritten, NULL)){
    return -1;
  }
  return nwritten;
}

void uart_close() {
  CloseHandle(port);
}
#endif
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__)

// posix systems include linux and os x
#include <termios.h>  // for serial
#include <fcntl.h>    // for serial constants
#include <poll.h>
#include <unistd.h>   // for STDOUT_FILENO
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __APPLE__
  const char uart_sample_name[] = "/dev/tty.usbserial-DJ00DV5V"; 
#else
  const char uart_sample_name[] = "/dev/ttyUSB0"; // sample COM port on linux
#endif

#define BAUD B230400

static int port = -1; // serial port file descriptor

static struct termios old_tio; // the termios structure when the program starts

void uart_close() {
  if(port != -1) {
    // wait for all pending writes to finish, or there is a timeout
    struct pollfd events;
    memset(&events, 0, sizeof(events));
    events.fd = port;
    events.revents |= POLLOUT;

    // wait for writes to finish or for a timeout. we must wait for writes to finish
    // to reliably jump to the application, since the bootloader does not respond
    int pval = poll(&events, 1, 200);
    if(pval < 0) {
      perror("Error (on exit): could not poll for finished writes");
    } else if(pval == 0) {
      // this was a timeout.  
      struct termios tio;
      // get the current settings
      if( tcgetattr(port, &tio) != 0) {
        perror("Error (on exit): could not get port attributes");
      }
      // disable flow control
      tio.c_cflag &= ~CRTSCTS;
      if(tcsetattr(port, TCSANOW, &tio) != 0) {
        perror("Error (on exit): could not disable flow control");
      }

      // flush the buffers
      if(tcflush(port, TCIOFLUSH) != 0) {
        perror("Error (on exit): could not flush serial buffers");
      }
    }

    // restore settings to state when the program was open
    if(tcsetattr(port, TCSADRAIN, &old_tio) != 0) {
      perror("Error (on exit): could not restore serial attributes");
    }

    if(close(port) != 0) {
      perror("Error (on exit): could not close port");
    }
    port = -1;
  }
}

void uart_open(const char port_name[])  // open a serial port (using POSIX calls)
{
  // open serial port for non-blocking reads 
  port = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK); 

  if (port == -1) {
    perror("Error: could not open serial port");
    exit(EXIT_FAILURE);
  }

  if (tcgetattr(port, &old_tio) != 0) {
    perror("Error: could not get port attributes");
    exit(EXIT_FAILURE);
  }

  if(atexit(uart_close) != 0) {
    perror("Error: could not set exit function");
    exit(EXIT_FAILURE);
  }

  struct termios tio;
  memcpy(&tio, &old_tio, sizeof(tio)); // start from the original settings

  tio.c_cflag |= CS8 | CREAD | CLOCAL | CRTSCTS; // 8n1, see termios.h 

  // set raw input mode
  tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // set raw output mode
  tio.c_oflag &= ~OPOST;

  // set the baud rate
  if(cfsetispeed(&tio, BAUD) != 0) {
    perror("Error: could not set input baud");
    exit(EXIT_FAILURE);
  }

  if(cfsetospeed(&tio, BAUD) != 0) {
    perror("Error: could not set output baud");
    exit(EXIT_FAILURE);
  }

  // flush serial buffers
  if(tcflush(port, TCIOFLUSH) != 0) {
    perror("Error: could not flush serial buffers");
    exit(EXIT_FAILURE);
  }

  // set serial port options
  if(tcsetattr(port, TCSANOW, &tio) != 0) {
    perror("Error: could not set serial options");
    exit(EXIT_FAILURE);
  }

}

int uart_read(uint8_t * data, size_t length) {
  return read(port, data, length);
}

int uart_write(const uint8_t * data, size_t length) {
  return write(port, data, length);
}

#endif // end platform-specific code
