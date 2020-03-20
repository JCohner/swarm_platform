#include "NU32.h"               // constants, functions for startup and UART

#define DECIMATE 3              // only send every 4th sample (counting starts at zero)
#define NSAMPLES 5000           // store 5000 samples

volatile int data_buf[NSAMPLES];// stores the samples
volatile int curr = 0;          // the current index into buffer

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) Timer1ISR(void) {  // Timer1 ISR operates at 5 kHz
  static int count = 0;         // counter used for decimation
  static int i = 0;             // the data returned from the isr
  ++i;                          // generate the data (we just increment it for now)
  if(count == DECIMATE) {       // skip some data
    count = 0;
    if(curr < NSAMPLES) {
      data_buf[curr] = i;       // queue a number for sending over the UART 
      ++curr;
    }
  }
  ++count;
  IFS0bits.T1IF = 0;            // clear interrupt flag
}

int main(void) {
  int i = 0;
  char buffer[100] = {};
  NU32_Startup();                // cache on, interrupts on, LED/button init, UART init

  __builtin_disable_interrupts();// INT step 2: disable interrupts at CPU
  T1CONbits.TCKPS = 0b01;        // PBCLK prescaler value of 1:8
  PR1 = 1999;                    // The frequency is 80 MHz / (8 * (1999 + 1)) = 5 kHz
  TMR1 = 0;       
  IPC1bits.T1IP = 5;             // interrupt priority 5
  IFS0bits.T1IF = 0;             // clear the interrupt flag
  IEC0bits.T1IE = 1;             // enable the interrupt
  T1CONbits.ON  = 1;             // turn the timer on                                 
  __builtin_enable_interrupts(); // INT step 7: enable interrupts at CPU

  NU32_ReadUART3(buffer, sizeof(buffer)); // wait for the user to press enter 
  while(curr !=NSAMPLES) { ; }            // wait for the data to be collected
  
  sprintf(buffer,"%d\r\n",NSAMPLES);      // send the number of samples that will be sent
  NU32_WriteUART3(buffer);

  for(i = 0; i < NSAMPLES; ++i) {
    sprintf(buffer,"%d\r\n",data_buf[i]);  // send the data to the terminal
    NU32_WriteUART3(buffer);
  }
  return 0;
}
