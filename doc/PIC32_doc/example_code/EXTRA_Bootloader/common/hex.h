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
