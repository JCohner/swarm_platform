#include "NU32.h"     // constants, funcs for startup and UART

// Use IC1 (D8) to measure the PWM duty cycle of OC1 (D0).
// Connect D8 to D0 for this to work.

#define TMR3_ROLLOVER 0xFFFF   // defines rollover count for IC1's 16-bit Timer3

static volatile int icperiod = -1;    // measured period, in Timer3 counts
static volatile int ichigh = -1;      // measured high duration, in Timer3 counts

void __ISR(_INPUT_CAPTURE_1_VECTOR, IPL3SOFT) InputCapture1() {
  int rise1, fall1, rise2, fall2;
  rise1 = IC1BUF;              // time of first rising edge
  fall1 = IC1BUF;              // time of first falling edge
  rise2 = IC1BUF;              // time of second rising edge
  fall2 = IC1BUF;              // time of second falling edge; not used below
  if (fall1 < rise1) {         // handle Timer3 rollover between rise1 and fall1
    fall1 = fall1 + TMR3_ROLLOVER+1;
    rise2 = rise2 + TMR3_ROLLOVER+1;  
  }
  else if (rise2 < fall1) {    // handle Timer3 rollover between fall1 and rise2
    rise2 = rise2 + TMR3_ROLLOVER+1;
  }
  icperiod = rise2 - rise1;    // calculate period, time between rising edges
  ichigh = fall1 - rise1;      // calculate high duration, between 1st rise and 1st fall
  IFS0bits.IC1IF = 0;          // clear interrupt flag
}

int main() {
  char buffer[100] = "";
  int val = 0, pd = 0, hi = 0; // desired pwm value, period, and high duration
  int i = 0;                   // loop counter

  NU32_Startup();  // cache on, interrupts on, LED/button init, UART init
  __builtin_disable_interrupts();
  
  // set up PWM signal using OC1 using Timer2
  T2CONbits.TCKPS = 0x3;    // Timer2 1:8 prescaler; ticks at 10 MHz (each tick is 100ns)
  PR2 = 9999;               // roll over after 10,000 ticks, or 1 ms (1 kHz)
  TMR2 = 0;           
  OC1CONbits.OCM = 0b110;   // PWM mode without fault pin; other OC1CON are defaults 
                            // (use TMR2) 
  T2CONbits.ON = 1;         // turn on Timer2
  OC1CONbits.ON = 1;        // turn on OC1

  // set up IC1 to use Timer3. IC1 could also use Timer2 (sharing with OC1) in this case 
  // since we set both timers to the same frequency, but we'd need to incorporate the
  // different rollover period in the ISR
  T3CONbits.TCKPS = 0x3;    // Timer3 1:8 prescaler; ticks at 10 MHz (each tick is 100ns)
  PR3 = TMR3_ROLLOVER;      // rollover value is also used in ISR to handle 
                            // timer rollovers.
  TMR3 = 0; 
  IC1CONbits.ICTMR = 0;     // IC1 uses Timer3
  IC1CONbits.ICM = 6;       // capture every edge
  IC1CONbits.FEDGE = 1;     // capture rising edge first
  IC1CONbits.ICI = 3;       // interrupt every 4th edge
  IFS0bits.IC1IF = 0;       // clear interrupt flag 
  IPC1bits.IC1IP = 3;       // interrupt priority 3
  IEC0bits.IC1IE = 1;       // enable IC1 interrupt
  T3CONbits.ON = 1;         // turn on Timer3
  IC1CONbits.ON = 1;        // turn on IC1
  
  __builtin_enable_interrupts();
  
  while(1) {
    NU32_WriteUART3("PWM period = 10,000 ticks of 10 MHz clock = 1 ms (1 kHz PWM).\r\n");
    NU32_WriteUART3("Enter high portion in 10 MHz (100 ns) ticks, in range 5-9995.\r\n");
    NU32_ReadUART3(buffer,sizeof(buffer));
    sscanf(buffer,"%d",&val);
    if (val < 5) {
      val = 5;               // try removing these limits and understanding the 
    } else if (val > 9995) { // behavior when val is close to 0 or 10,000
      val = 9995; 
    }
    sprintf(buffer,"You entered %d.\r\n",val);
    NU32_WriteUART3(buffer);
    OC1RS = val;                    // change the PWM duty cycle
    for (i=0; i<1000000; i++) {     // a short delay as PWM updates and 
      _nop();                       // IC1 measures the signal
    }
    __builtin_disable_interrupts(); // disable ints briefly to copy vars shared with ISR
    pd = icperiod;
    hi = ichigh;
    __builtin_enable_interrupts();  // re-enable the interrupts
    sprintf(buffer,"Measured period is %d clock cycles, high for %d cycles, \r\n",pd,hi);
    NU32_WriteUART3(buffer);
    sprintf(buffer,"  for a duty cycle of %5.2f percent.\r\n\n",
            100.0*((double) hi/(double) pd));
    NU32_WriteUART3(buffer);
  }
  return 0;
}
