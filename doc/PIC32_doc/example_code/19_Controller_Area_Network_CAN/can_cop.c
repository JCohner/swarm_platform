#include "NU32.h"          // constants, funcs for startup and UART
#include <sys/kmem.h>      // used to convert between physical and virtual addresses
#include <ctype.h>         // function "tolower" makes uppercase chars into lowercase
// The CAN cop is the "police officer" that controls the traffic light
// connect AC1TX to TXD on the transceiver, AC1RX to RXD on the transceiver
// The CANH and CANL transceiver pins should be connected to the same wires as
// the PIC running can_light.c.
//
// if your transceiver chip is the Microchip MCP2562 then connect 
// Vss and STBY to GND, VDD to 5V VIO to 3.3V
#define FIFO_0_SIZE 4      // size of FIFO 0, in number of message buffers
#define FIFO_1_SIZE 4
#define MB_SIZE 4          // number of 4-byte integers in a message buffer

#define LIGHT1_SID 1       // SID of the first traffic light

volatile unsigned int fifos[(FIFO_0_SIZE + FIFO_1_SIZE)* MB_SIZE]; // buffer for CAN FIFOs

int main(void) {
  char buffer[100] = "";
  char cmd = '\0';
  unsigned int * addr = NULL;     // used to store fifo address

  NU32_Startup();                 // cache on, interrupts on, LED/button init, UART init
  
  C1CONbits.ON = 1;                    // turn on the CAN module  
  C1CONbits.REQOP = 4;                 // request configure mode
  while(C1CONbits.OPMOD != 4) { ; }    // wait to enter config mode

  C1FIFOCON0bits.FSIZE = FIFO_0_SIZE-1;// set fifo 0 size in message buffers
  C1FIFOCON0bits.TXEN = 0;             // fifo 0 is an RX fifo

  C1FIFOCON1bits.FSIZE = FIFO_1_SIZE-1;// set fifo 1 size
  C1FIFOCON1bits.TXEN = 1;             // fifo 1 is a TX fifo
  C1FIFOBA = KVA_TO_PA(fifos);         // tell CAN where the fifos are

  C1CFGbits.BRP = 16;   // Tq = (2 x (BRP + 1)) x 12.5 ns = 425 ns
  C1CFGbits.PRSEG =  3; // 4Tq in propagation segment
  C1CFGbits.SEG1PH = 3; // 4Tq in phase 1. Phase 2 is set automatically to be the same.
  // bit duration = 1Tq(sync) + 4Tq(prop) + 4Tq(phase 1) + 4Tq(phase 2) = 13Tq = 5.525 us
  // so baud is 1/5.525 us = 180,995 Hz

  C1CFGbits.SJW = 0;    // up to (SJW+1)*Tq adjustment possible in phase 1 or 2 to resync
 
  C1CONbits.REQOP = 0;                // request normal mode 
  while(C1CONbits.OPMOD != 0) { ; }   // wait for normal mode 
  NU32_LED1 = 0;
  while(1) {
    NU32_WriteUART3("(R)ed, (Y)ellow, or (G)reen?\r\n");
    NU32_ReadUART3(buffer,100);
    sscanf(buffer,"%c", &cmd);
    sprintf(buffer,"Setting %c\r\n", cmd);
    NU32_WriteUART3(buffer);

    if(C1TRECbits.TXWARN) {           // many bad transmissions have occurred,
                                      // print info to help debug bus (no ACKs?)
      sprintf(buffer,"Error: C1TREC 0x%08x\r\n",C1TREC); 
      NU32_WriteUART3(buffer);                           
    }                                 
   
    addr = PA_TO_KVA1(C1FIFOUA1);     // get VA of FIFO 1 (TX) current message buffer 
    addr[0] = LIGHT1_SID;             // specify SID in word 0
    addr[1] = sizeof(cmd);            // specify DLC in word 1 (one byte being sent)
    addr[2] = tolower(cmd);           // the data (make uppercase chars lowercase)
                                      // since only 1 byte, no addr[3] given
    C1FIFOCON1SET = 0x2000;           // setting the UINC bit icrements fifo pointer
    C1FIFOCON1bits.TXREQ = 1;         // request that fifo data be sent on the bus
  }
  return 0;
}
