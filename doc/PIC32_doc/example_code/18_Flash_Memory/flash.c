#include "flash.h"
#include <xc.h>
#include <sys/kmem.h> // macros for converting between physical and virtual addresses

#define OP_ERASE_PAGE 4 // erase page operation, per NVMCONbits.NVMOP specification
#define OP_WRITE_WORD 1 // write word operation, per NVMCONbits.NVMOP specification

// Making the array const and initializing it to 0 ensures that the linker will
// store it in flash memory.
// Since one page is erased at a time, array must be a multiple of PAGE_SIZE bytes long.
// The aligned attribute ensures that the page falls at an address divisible by 4096.

static const unsigned int buf[PAGE_WORDS] __attribute__ ((__aligned__(PAGE_SIZE))) = {0};

static void flash_op(unsigned char op) { // perform a flash operation (op is NVMOP)
  int ie = __builtin_disable_interrupts();

  NVMCONbits.NVMOP = op;  // store the operation
  NVMCONbits.WREN = 1;    // enable writes to the WR bit

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
}

void flash_erase() {         // erase the flash buffer. resets the memory to ones
  NVMADDR = KVA_TO_PA(buf);  // use the physical address of the buffer
  flash_op(OP_ERASE_PAGE);      
}

void flash_write_word(unsigned int index, unsigned int data) { // writes a word to flash  
  NVMADDR = KVA_TO_PA(buf + index);   // physical address of flash to write to
  NVMDATA = data;                     // data to write 
  flash_op(OP_WRITE_WORD);
}

unsigned int flash_read_word(unsigned int index) { // read a word from flash
  return buf[index];
}
