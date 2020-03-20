#include "NU32.h"          // constants, funcs for startup and UART
#include "flash.h"         // allocates buffer of PAGE_WORDS (1024) unsigned ints

// Uses flash.{c,h} library to allocate a page in flash and then write 32-bit
// words there, consecutively starting from the zeroth index in the array.  
// LIMITATION:  Words that are all 1's (0xFFFFFFFF) cannot be saved; array
// elements holding this value are considered to be empty (not written since erase).

#define PAGE_IN_USE_PWD    0xdeadbeef // password meaning a valid flash buffer is present
#define PAGE_IN_USE_INDEX  (PAGE_WORDS-1) // the password is at the last index of buffer

int page_exists() {  // valid flash page is allocated if password is at the right index
  return (flash_read_word(PAGE_IN_USE_INDEX) == PAGE_IN_USE_PWD);
}

// returns the index where the next word should be written
unsigned int next_page_index() { 
  unsigned int count = 0;

  while ((flash_read_word(count) != 0xFFFFFFFF)
          && (count < PAGE_IN_USE_INDEX)) {  
    count++;
  }
  return count;
}

// page is full if the next word would be written at the password index 
int page_full() {  
  return (next_page_index() == PAGE_IN_USE_INDEX);
}

// erase page and write the password indicating a flash page is available
void make_page() { 
  flash_erase();
  flash_write_word(PAGE_IN_USE_INDEX,PAGE_IN_USE_PWD);
}

void add_new_word() { // add a four-byte word at the next_page_index if page is not full
  char msg[100];
  unsigned int ind = next_page_index();  // where to write the word in the flash page
  unsigned int val;

  if (page_full()) {
    NU32_WriteUART3("Flash page full; no more data will be stored.\r\n");
  }
  else {
    sprintf(msg,"Enter an unsigned int to store at location %d.\r\n",ind);
    NU32_WriteUART3(msg);
    NU32_ReadUART3(msg,sizeof(msg)); // enter word using 8 hex characters, like f01dab1e 
    sscanf(msg,"%x",&val);
    flash_write_word(ind,val);
    sprintf(msg,"Adding 0x%x at location %d.\r\n",val,ind);
    NU32_WriteUART3(msg);
  }
}

void show_words() {  // print out the currently saved four-byte words in hex
  char msg[100];
  unsigned int i;
  for (i = 0; i < next_page_index(); i++) {
    sprintf(msg,"at index %4d: 0x%x \r\n",i,flash_read_word(i));
    NU32_WriteUART3(msg);
  }
}

int main(void) {
  char msg[100]="";

  NU32_Startup(); // cache on, interrupts on, LED/button init, UART init
  while(1) {
    if (!page_exists()) {  // initialize a flash page if the password is not present
      NU32_WriteUART3("\r\nNo flash memory allocated currently; making a page.");
      make_page();
    }
    sprintf(msg,"\r\nCurrently %d words stored in flash.\r\n",next_page_index());
    NU32_WriteUART3(msg);
    NU32_WriteUART3("(a)dd word, erase & (m)ake new page, (s)how words, (e)rror?\r\n");
    NU32_ReadUART3(msg,sizeof(msg));
    switch (msg[0]) {      // check the first character entered by user
      case 'a': 
        add_new_word();
        break;
      case 'm': 
        make_page();
        break;
      case 's': 
        show_words();
        break;
    case 'e':// shows that if we obliterate the password, then no valid flash page exists
        flash_write_word(PAGE_IN_USE_INDEX,0);
        break;
    default: 
        break;
    }
  }
  return 0;
}
