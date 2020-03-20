#ifndef FLASH_H
#define FLASH_H
#include<stdbool.h>
#include<stdint.h>
// Flash memory driver for the NU32 bootloader

// physical address that is the start of program flash
#define FLASH_PF_START_PA 0x1D000000 
#define FLASH_TOTAL_ROWS 1024   // the total number of flash rows

typedef uint32_t flash_word_t; // a type the same size as the bytes in a flash word

// number of bytes in a flash word
#define FLASH_BYTES_PER_WORD sizeof(flash_word_t)  

#define FLASH_WORDS_PER_ROW 128 // words per row 
#define FLASH_BYTES_PER_ROW (FLASH_WORDS_PER_ROW * FLASH_BYTES_PER_WORD)


// determine if a physical address is flash word aligned and in the program flash
bool flash_valid_pf_pa(uint32_t pa);

// clear flash error registers, return false on error
bool flash_clear_error(); 

// erase all of program memory, setting it to all ones, return false on error
bool flash_erase_all();

// write a word to the program flash. pa is the physical address (as a uint32_t value),
// and data is the word to write
bool flash_write_word(uint32_t pa, flash_word_t data);  

// write a row of flash. a row is 512 flash words (4 bytes) which is 512*4 bytes and 
// must be on a 512 byte boundary.  so va % 512 == 0 and data is 512 words long
// pa is the physical address of the row,
// data is the virtual address of the data to write
bool flash_write_row(uint32_t pa, flash_word_t data[FLASH_WORDS_PER_ROW]);

#endif
