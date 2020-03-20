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
