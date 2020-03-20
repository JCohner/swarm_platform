#include "NU32.h"          // constants, funcs for startup and UART

int main(void) {
  
  if(RCONbits.WDTO && RCONbits.SLEEP) { // reset due to WDT waking PIC32 from sleep?
      __asm__ __volatile__ ("iret");    // if so, resume where we left off. 
  } 
  
  NU32_Startup();      // cache on, interrupts on, LED/button init, UART init            

  if(RCONbits.WDTO) { // WDT caused the reset, but it was not from sleep mode
    RCONbits.WDTO = 0;// clear WDT reset.  Subsequent resets due to the reset button 
                      // won't be interpreted as a WDT reset
     NU32_WriteUART3("\r\nReset after a WDT timeout in infinite loop.\r\n");
  }

  char letters[2] = "a";  // second char is the string terminator
  int pressed = 0;        // true if button pressed during the delay

  OSCCONbits.SLPEN = 1;   // enters sleep (not idle) when 'wait' instruction issued

  // print instructions
  NU32_WriteUART3("Press USER button before 'j' to go to sleep; after to enter a\r\n");
  NU32_WriteUART3("faulty infinite loop. If sleep, the WDT will wake the PIC32.\r\n");
  NU32_WriteUART3("If infinite loop, the WDT will reset the PIC32 and start over.\r\n");
  
  WDTCONbits.ON = 1;            // turn on the WDT (rollover of 4.096s in DEVCFG1)

  while(1) {
    if(!NU32_USER || pressed) { // USER button pushed (NU32_USER is low if USER pressed)
      if(letters[0] < 'j') {    // if button pushed early in program, go to sleep
        NU32_WriteUART3(" Going to sleep ... ");
        __asm__ __volatile__ ("wait");  // until the WDT rolls over and wakes from sleep
        NU32_WriteUART3(" Waking up. ");
      } else {                  // if button pushed late, get stuck in infinite loop
        NU32_WriteUART3(" Getting stuck in infinite loop.\r\n");
        while(1) {
          _nop();               // fortunately the WDT will reset the PIC32
        }
      }
    }
    NU32_WriteUART3(letters);
    ++letters[0];
    pressed = 0;
    _CP0_SET_COUNT(0);
    while(_CP0_GET_COUNT() < 40000000) {
      pressed |= !NU32_USER;    // delay for ~1 second, still poll the user button
    }
    WDTCONbits.WDTCLR = 1;      // clear the watchdog timer
  }
  return 0;
}

