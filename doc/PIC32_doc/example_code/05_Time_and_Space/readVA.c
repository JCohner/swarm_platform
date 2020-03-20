#include "NU32.h"           
#define MSGLEN 100

// val is an initialized global; you can find it in the memory map with this program
char msg[MSGLEN];
unsigned int *addr;  
unsigned int k = 0, nwords = 0, val = 0xf01dab1e;

int main(void) {
  
  NU32_Startup(); 
  while (1) {
    sprintf(msg, "Enter the start VA (e.g., bd001970) and # of 32-bit words to print: ");
    NU32_WriteUART3(msg);

    NU32_ReadUART3(msg,MSGLEN);
    sscanf(msg,"%x %d",&addr, &nwords);
    
    sprintf(msg,"\r\nListing %d 32-bit words starting from VA %08x.\r\n",nwords,addr);
    NU32_WriteUART3(msg);

    sprintf(msg," ADDRESS  CONTENTS\r\n");
    NU32_WriteUART3(msg);
    
    for (k = 0; k < nwords; k++) {
      sprintf(msg,"%08x  %08x\r\n", addr,*addr); // *addr is the 32 bits starting at addr
      NU32_WriteUART3(msg);
      ++addr;                  // addr is an unsigned int ptr so it increments by 4 bytes
    }
  }
  return 0;
}

// handle cpu exceptions, such as trying to read from a bad memory location
void _general_exception_handler(unsigned cause, unsigned status)
{
  unsigned int exccode = (cause & 0x3C) >> 2; // the exccode is reason for the exception
  // note: see PIC32 Family Reference Manual Section 03 CPU M4K Core for details
  // Look for the Cause register and the Status Register
  NU32_WriteUART3("Reset the PIC32 due to general exception.\r\n");
  sprintf(msg,"cause 0x%08x (EXCCODE = 0x%02x), status 0x%08x\r\n",cause,exccode,status);
  NU32_WriteUART3(msg);
  while(1) {
    ;
  }
}
