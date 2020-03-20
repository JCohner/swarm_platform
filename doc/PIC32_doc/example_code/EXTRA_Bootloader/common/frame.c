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

