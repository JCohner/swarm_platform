#include "NU32.h"                 // constants, functions for startup and UART

// We will set up UART1 at a slow baud rate so you can examine the signal on a scope.
// Connect the UART1 RX and TX pins together so the UART can communicate with itself.

int main(void) {
  char msg[100] = {};
  NU32_Startup();  // cache on, interrupts on, LED/button init, UART init

  // initialize UART1:  100 baud, odd parity, 1 stop bit
  U1MODEbits.PDSEL = 0x2; // odd parity (parity bit set to make the number of 1's odd)
  U1STAbits.UTXEN = 1;    // enable transmit
  U1STAbits.URXEN = 1;    // enable receive

  // U1BRG = Fpb/(M * baud) - 1  (note U1MODEbits.BRGH = 0 by default, so M = 16)
  // setup for 100 baud.  This means 100 bits /sec or 1 bit/ 1/10ms
  U1BRG = 49999;     // 80 M/(16*100) - 1 = 49,999
  U1MODEbits.ON = 1; // turn on the uart

  // scope instructions: 10 ms/div, trigger on falling edge, single capture
  while(1) {
    unsigned char data = 0;
    NU32_WriteUART3("Enter hex byte (lowercase) to send to UART1 (i.e., 0xa1): "); 
    NU32_ReadUART3(msg, sizeof(msg));
    sscanf(msg,"%2x",&data);
    sprintf(msg,"0x%02x\r\n",data);
    NU32_WriteUART3(msg); //echo back

    while(U1STAbits.UTXBF) {  // wait for UART to be ready to transmit
      ;
    }
    U1TXREG = data;           // write twice so we can see the stop bit
    U1TXREG = data;
    while(!U1STAbits.URXDA) { // poll to see if there is data to read in RX FIFO
      ;
    }
    data = U1RXREG;           // data has arrived; read the byte
    while(!U1STAbits.URXDA) { // wait until there is more data to read in RX FIFO
      ;
    }
    data = U1RXREG;           // overwriting data from previous read! could check if same
    sprintf(msg,"Read 0x%x from UART1\r\n",data);
    NU32_WriteUART3(msg);
  }
  return 0;
}
