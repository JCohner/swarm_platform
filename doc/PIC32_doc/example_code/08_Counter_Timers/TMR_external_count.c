#include "NU32.h"          // constants, functions for startup and UART

void __ISR(_TIMER_5_VECTOR, IPL4SOFT) Timer5ISR(void) {  // INT step 1: the ISR
  LATDINV = 0x02;                                        // toggle RD1
  IFS0bits.T5IF = 0;                                     // clear interrupt flag
}                                                        

int main(void) {
  char message[200] = {};
  int i = 0;

  NU32_Startup();                 // cache on, interrupts on, LED/button init, UART init
  __builtin_disable_interrupts(); // INT step 2: disable interrupts

  TRISDbits.TRISD1 = 0;           // make D1 an output. connect D1 to T1CK (C14)!
                                  
                                  // configure Timer1 to count external pulses.  
                                  // The remaining settings are left at their defaults
  T1CONbits.TCS = 1;              // count external pulses
  PR1 = 0xFFFF;                   // enable counting to max value of 2^16 - 1
  TMR1 = 0;                       // set the timer count to zero
  T1CONbits.ON = 1;               // turn Timer1 on and start counting
                                                                
                                  // 1 kHz pulses with 2 kHz interrupt from Timer45
  T4CONbits.T32 = 1;              // INT step 3: set up Timers 4 and 5 as 32-bit Timer45
  PR4 = 39999;                    //             rollover at 40,000; 80MHz/40k = 2 kHz
  TMR4 = 0;                       //             set the timer count to zero 
  T4CONbits.ON = 1;               //             turn the timer on
  IPC5bits.T5IP = 4;              // INT step 4: priority for Timer5 (int goes with T5)
  IFS0bits.T5IF = 0;              // INT step 5: clear interrupt flag
  IEC0bits.T5IE = 1;              // INT step 6: enable interrupt
  __builtin_enable_interrupts();  // INT step 7: enable interrupts at CPU

  while (1) {
                                  // display the elapsed time in ms
    sprintf(message,"Elapsed time:  %u ms\r\n", TMR1); 
    NU32_WriteUART3(message);
                                  
    for(i = 0; i < 10000000; ++i){// loop to delay printing    
      _nop();                     // include nop so loop is not optimized away
    }
  }
  return 0;
}
