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
