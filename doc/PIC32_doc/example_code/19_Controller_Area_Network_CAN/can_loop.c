#include "NU32.h"          // constants, funcs for startup and UART
#include <sys/kmem.h>      // used to convert between physical and virtual addresses
// Basic CAN example using loopback mode, so this functions with no external hardware.
// Prompts user to enter numbers to send via CAN.
// Sends the numbers and receives them via loopback, printing the results.

#define MY_SID 0x146  // the sid that this module responds to

#define FIFO_0_SIZE 4 // size of FIFO 0 (RX), in number of message buffers
#define FIFO_1_SIZE 2 // size of FIFO 1 (TX), in number of message buffers
#define MB_SIZE 4     // number of 4-byte integers in a message buffer

// buffer for CAN FIFOs
static volatile unsigned int fifos[(FIFO_0_SIZE + FIFO_1_SIZE) * MB_SIZE]; 

int main() {
  char buffer[100];
  int to_send = 0; 
  unsigned int * addr;          // used for storing fifo addresses

  NU32_Startup();               // cache on, interrupts on, LED/button init, UART init
  
  C1CONbits.ON = 1;                    // turn on the CAN module 
  C1CONbits.REQOP = 4;                 // request configure mode
  while(C1CONbits.OPMOD != 4) { ; }    // wait to enter config mode

  C1FIFOCON0bits.FSIZE = FIFO_0_SIZE-1;// set fifo 0 size.  Actual size is 1 + FSIZE
  C1FIFOCON0bits.TXEN = 0;             // fifo 0 is an RX fifo

  C1FIFOCON1bits.FSIZE = FIFO_1_SIZE-1;// set fifo 1 size. Actual size is 1 + FSIZE
  C1FIFOCON1bits.TXEN = 1;             // fifo 1 is a TX fifo
  C1FIFOBA = KVA_TO_PA(fifos);         // tell CAN where the fifos are
  
  C1RXM0bits.SID = 0x7FF;              // mask 0 requires all SID bits to match

  C1FLTCON0bits.FSEL0 = 0;             // filter 0 is for FIFO 0
  C1FLTCON0bits.MSEL0 = 0;             // filter 0 uses mask 0
  C1RXF0bits.SID = MY_SID;             // filter 0 matches against SID
  C1FLTCON0bits.FLTEN0 = 1;            // enable filter 0
                                      
                                       // skipping baud settings since loopback only
  C1CONbits.REQOP = 2;                 // request loopback mode 
  while(C1CONbits.OPMOD != 2) { ; }    // wait for loopback mode 

  while(1) {
    NU32_WriteUART3("Enter number to send via CAN:\r\n");
    NU32_ReadUART3(buffer,100);
    sscanf(buffer,"%d", &to_send);
    sprintf(buffer,"Sending: %d\r\n",to_send);
    NU32_WriteUART3(buffer);

    addr = PA_TO_KVA1(C1FIFOUA1);      // get FIFO 1 (the TX fifo) current message address
    addr[0] = MY_SID;                  // only the sid must be set for this example
    addr[1] = sizeof(to_send);         // only DLC field must be set, we indicate 4 bytes
    addr[2] = to_send;                 // 4 bytes of actual data
    C1FIFOCON1SET = 0x2000;            // setting UINC bit tells fifo to increment pointer
    C1FIFOCON1bits.TXREQ = 1;          // request that data from the queue be sent

    while(!C1FIFOINT0bits.RXNEMPTYIF) { ; } // wait to receive data
    addr = PA_TO_KVA1(C1FIFOUA0);      // get the VA of current pointer to the RX FIFO
    sprintf(buffer,"Received %d with SID = 0x%x\r\n",addr[2], addr[0] & 0x7FF);
    NU32_WriteUART3(buffer);
    C1FIFOCON0SET = 0x2000;            // setting UINC bit tells fifo to increment pointer
  }
  return 0;
}
