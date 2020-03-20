#include "NU32.h"   // constants, funcs for startup and UART

// Use the comparator reference voltage as a cheap DAC.
// Voltage is output on CVREFOUT (RB10). You can measure it.
// You will most likely need to buffer this output to use it to 
// drive a low-impedance load.

int main(void) {
  int i;
  NU32_Startup();        // cache on, interrupts on, LED/button init, UART init
  TRISBbits.TRISB10 = 0; // make the CVrefout/RB10 pin an output
  CVRCONbits.CVROE = 1;  // output the voltage on CVrefout
  CVRCONbits.CVRR = 0;   // use the smaller output range, at higher voltages
  CVRCONbits.ON = 1;     // turn the module on
  while(1) {
    for(i = 0; i < 16; ++i) { // step through the voltages, one per second
      CVRCONbits.CVR = i;
      _CP0_SET_COUNT(0);
      while(_CP0_GET_COUNT() < 40000000) {
        ;
      }
    }
  }
  return 0;
}
