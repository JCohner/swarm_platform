#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include <sys/kmem.h>      // used to convert between physical and virtual addresses
// simulates a traffic light that can be controlled via can_cop
// LED1 on,  LED2 on  = GREEN
// LED1 on,  LED2 off = YELLOW
// LED1 off, LED2 off = RED

// connect AC1TX to TXD on the transceiver, AC1RX to RXD on the transceiver
// The CANH and CANL transceiver pins should be connected to the same wires as
// the PIC running can_cop.c
//
// if your transceiver chip is the Microchip MCP2562 then connect 
// Vss and STBY to GND, VDD to 5V VIO to 3.3V
#define FIFO_0_SIZE 4 // size of FIFO 0, in number of message buffers
#define FIFO_1_SIZE 4
#define MB_SIZE 4     // number of 4-byte integers in a message buffer

#define LIGHT1_SID 1  // SID of the first traffic light

volatile unsigned int fifos[(FIFO_0_SIZE + FIFO_1_SIZE)* MB_SIZE]; // buffer for CAN FIFOs

int main(void) {
  unsigned int * addr = NULL;     // used to store fifo address

  NU32_Startup();                 // cache on, interrupts on, LED/button init, UART init
  
  C1CONbits.ON = 1;                    // turn on the CAN module
  C1CONbits.REQOP = 4;                 // request configure mode
  while(C1CONbits.OPMOD != 4) { ; }    // wait to enter config mode

  C1FIFOCON0bits.FSIZE = FIFO_0_SIZE-1;// set fifo 0 size
  C1FIFOCON0bits.TXEN = 0;             // fifo 0 is an RX fifo

  C1FIFOCON1bits.FSIZE = FIFO_1_SIZE-1;// set fifo 1 size
  C1FIFOCON1bits.TXEN = 1;             // fifo 1 is a TX fifo
  C1FIFOBA = KVA_TO_PA(fifos);         // tell CAN where the fifos are

  C1RXM0bits.SID = 0x7FF;              // mask 0 requires all SID bits to match

  C1FLTCON0bits.FSEL0 = 0;             // filter 0 uses FIFO 0
  C1FLTCON0bits.MSEL0 = 0;             // filter 0 uses mask 0
  C1RXF0bits.SID = LIGHT1_SID;         // filter 0 matches against SID
  C1FLTCON0bits.FLTEN0 = 1;            // enable filter 0

  C1CFGbits.BRP = 16;                  // copy the bit timing info for can_cop.c;
  C1CFGbits.PRSEG =  3;                // see comments in can_cop.c
  C1CFGbits.SEG1PH = 3;         
  C1CFGbits.SJW = 0;            
 
  C1CONbits.REQOP = 0;                 // request normal mode 
  while(C1CONbits.OPMOD != 0) { ; }    // wait for normal mode 
  NU32_LED1 = 1;                       // turn both LEDs off
  NU32_LED2 = 1;
  while(1) {
    if(C1FIFOINT0bits.RXNEMPTYIF) {    // we have received data
      addr = PA_TO_KVA1(C1FIFOUA0);    // get VA of the RX fifo 0 current message
      switch(addr[2]) {
        case 'r':                      // switch to red
          NU32_LED1 = 1;
          NU32_LED2 = 1;
          break;
      case 'y':                        // switch to yellow
          NU32_LED1 = 0;
          NU32_LED2 = 1;
          break;
      case 'g':                        // switch to green
          NU32_LED1 = 0;            
          NU32_LED2 = 0;
          break;
        default:
          ;                            // error! do something here
      }
      C1FIFOCON0SET = 0x2000;          // setting the UINC bit increments RX FIFO pointer
    }
  }
  return 0;
}
