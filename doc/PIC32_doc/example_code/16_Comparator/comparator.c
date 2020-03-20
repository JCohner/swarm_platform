#include "NU32.h"  // constants, funcs for startup and UART

// Uses comparator 2 to interrupt on a low voltage condition.
// The + comparator terminal is connected to C2IN+ (B3).
// The - comparator terminal is connected to the internal voltage IVref (1.2 V).
// Both NU32 LEDs illuminate if the + input is > 1.2 V; otherwise, off.
// The comparator output can be viewed on C2OUT (B9) with a voltmeter or measured by ADC.

int main(void) {
  NU32_Startup();        // cache on, interrupts on, LED/button init, UART init
  CM2CONbits.COE = 1;    // comparator output is on the C2OUT pin, so you can measure it 
  CM2CONbits.CCH = 0x3;  // connect - input to IVref; by default + connected to C2IN+
  TRISBbits.TRISB9 = 0;  // configure B9 as an output, which must be set to use C2OUT
  CM2CONbits.ON = 1;

  while(1) {
    // test the comparator output
    if(CMSTATbits.C2OUT) { // if output is high then the input signal > 1.2 V
      NU32_LED1 = 0;
      NU32_LED2 = 0;
    } else {
      NU32_LED1 = 1;
      NU32_LED2 = 1;
    }
  }
  return 0;
}
