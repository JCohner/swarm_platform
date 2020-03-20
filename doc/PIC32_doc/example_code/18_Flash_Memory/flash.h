#ifndef FLASH__H__
#define FLASH__H__
// the flash module allocates a page of flash and provides read/write accesss

#define PAGE_SIZE 4096                                 // size of a page, in bytes
#define PAGE_WORDS (PAGE_SIZE/4)                       // size of a page, in 4-byte words

// erases the flash page by setting all bits to 1's
void flash_erase(void);            

// writes the 0's of a 4-byte word 
void flash_write_word(unsigned int index, unsigned int data); 

// reads a word from flash
unsigned int flash_read_word(unsigned int index);             

#endif
