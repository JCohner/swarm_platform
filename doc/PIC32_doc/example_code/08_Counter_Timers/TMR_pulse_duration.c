#include "NU32.h"          // constants, functions for startup and UART

void __ISR(_TIMER_5_VECTOR, IPL4SOFT) Timer5ISR(void) {  // INT step 1: the ISR
  LATDINV = 0x02;                 // toggle RD1
  IFS0bits.T5IF = 0;              // clear interrupt flag
}

void __ISR(_TIMER_1_VECTOR, IPL3SOFT) Timer1ISR(void) { // INT step 1: the ISR
  char msg[100] = {};
  sprintf(msg,"The count was %u, or %10.8f seconds.\r\n", TMR1, TMR1/312500.0);
  NU32_WriteUART3(msg);
  TMR1 = 0;                       // reset Timer1
  IFS0bits.T1IF = 0;              // clear interrupt flag
}

int main(void) {
  NU32_Startup();                 // cache on, interrupts on, LED/button init, UART init

  __builtin_disable_interrupts(); // INT step 2: disable interrupts

  TRISDbits.TRISD1 = 0;           // make D1 an output. connect D1 to T1CK (C14)

                                  // INT step 3
  T1CONbits.TGATE = 1;            //             Timer1 in gated accumulation mode
  T1CONbits.TCKPS = 3;            //             1:256 prescale ratio
  T1CONbits.TCS = 0;
  PR1 = 0xFFFF;                   //             use the full period of Timer1
  T1CONbits.TON = 1;              //             turn Timer1 on 

  T4CONbits.T32 = 1;              // for T45:    enable 32 bit mode Timer45            
  PR4 = 7999999;                  //             set PR so timer rolls over at 10 Hz
  TMR4 = 0;                       //             initialize count to 0
  T4CONbits.TON = 1;              //             turn Timer45 on

  IPC5bits.T5IP = 4;              // INT step 4: priority for Timer5 (int for Timer45)
  IPC1bits.T1IP = 3;              //             priority for Timer1 
  IFS0bits.T5IF = 0;              // INT step 5: clear interrupt flag for Timer45
  IFS0bits.T1IF = 0;              //             clear interrupt flag for Timer1
  IEC0bits.T5IE = 1;              // INT step 6: enable interrupt for Timer45
  IEC0bits.T1IE = 1;              //             enable interrupt for Timer1
  __builtin_enable_interrupts();  // INT step 7: enable interrupts at the CPU
  
  while (1) {
    ;
  }
  return 0;
}
