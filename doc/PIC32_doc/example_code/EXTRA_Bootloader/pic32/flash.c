#include "flash.h"
#include <xc.h>
#include <sys/kmem.h> // macros for converting between physical and virtual addresses

// based on flash.c example from Chapter 18 Flash Memory, extended for use
// in the NU32 bootloader

// erase operations for NVMCON
typedef enum { 
  OP_NOP           = 0,
  OP_WRITE_WORD    = 1, 
  OP_WRITE_ROW     = 3,
  OP_ERASE_PAGE    = 4,
  OP_ERASE_PROGRAM = 5
} Operation;

// true if there was a flash programming error
static bool flash_error() {
  return NVMCONbits.WRERR != 0 || NVMCONbits.LVDERR != 0; // if an error status bit is
                                                          // is set, return true
}                                                      

// perform a flash operation (op is NVMOP), return false on failure
static bool flash_op(Operation op) { 
  int ie = __builtin_disable_interrupts();

  NVMCONbits.NVMOP = op;  // store the operation
  NVMCONbits.WREN = 1;    // enable writes to the WR bit
  // reference manual says to delay here.  This delay appears only in the data sheet
  // (and errata) for the 3xx/4xx series, not the 7xx series, so we omit it.
  NVMKEY = 0xAA996655;    // unlock step 1
  NVMKEY = 0x556699AA;    // unlock step 2
  NVMCONSET = 0x8000;     // set the WR bit to begin the operation  

  while (NVMCONbits.WR) { // wait for the operation to finish
    ;
  }

  NVMCONbits.WREN = 0;    // disables writes to the WR bit
  
  if (ie & 0x1) {         // re-enable interrupts if they had been disabled
    __builtin_enable_interrupts();
  }
  return !flash_error();
}

// check that the physical asddress is in program flash and is on a flash word boundary
bool flash_valid_pf_pa(uint32_t pa) {
  return pa % FLASH_BYTES_PER_WORD == 0 
    && pa >= FLASH_PF_START_PA 
    && pa <  FLASH_PF_START_PA + BMXPFMSZ; // BMXPFMSZ is the size of program flash
                                               // in bytes
}

bool flash_clear_error() {
  // only attempt to clear if there is an error to clear
  return flash_error() ? flash_op(OP_NOP) : true;
}

bool flash_erase_all() {   // erase program flash
  return flash_op(OP_ERASE_PROGRAM);      
}

bool flash_write_word(uint32_t pa, uint32_t data) { 
  NVMADDR = pa;   // physical address of flash to write to
  NVMDATA = data; // data to write 
  return flash_op(OP_WRITE_WORD);
}

bool flash_write_row(uint32_t pa, uint32_t * data) { 
  NVMADDR = pa;   // physical address of flash to write to
  NVMSRCADDR = KVA_TO_PA(data);
  return flash_op(OP_WRITE_ROW);
}

