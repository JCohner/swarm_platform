#include <peripheral/peripheral.h>    // harmony peripheral library
#include <sys/attribs.h>              // defines the __ISR macro
// Almost a direct translation of TMR_5Hz.c to use the harmony peripheral library.
// The main difference is that it flashes two LEDs out of phase,
// instead of just flashing LED2.

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) Timer1ISR(void) {  
  // toggle LATF0 (LED1) and LATF1 (LED2)
  PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_0); 
  PLIB_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_1); 
  // clear the interrupt flag
  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_TIMER_1); 
}

int main(void) {
  // F0 and F1 are  is output (LED1 and LED2)
  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_0); 
  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_1); 

  // turn LED1 on by clearing F0 to zero
  PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_0); 

  // turn off LED2 by setting F1 to one
  PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_1);   
  
  PLIB_TMR_Period16BitSet(TMR_ID_1, 62499);                   // set up PR1: PR1 = 62499
  PLIB_TMR_Counter16BitSet(TMR_ID_1,0);                       // set up TMR1: TMR1 = 0
  PLIB_TMR_PrescaleSelect(TMR_ID_1, TMR_PRESCALE_VALUE_256);  // 1:256 prescaler

  // set up the timer interrupts
  // clear the interrupt flag
  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_TIMER_1); 
 
  // set the interrupt priority
  PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_T1, INT_PRIORITY_LEVEL5);

  // enable the timer interrupt
  PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_1);

  // start the timer
  PLIB_TMR_Start(TMR_ID_1);
  // enable interrupts
  PLIB_INT_Enable(INT_ID_0);
  while (1) {
      ;       // infinite loop
  }
  return 0;
}
