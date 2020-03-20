#include "NU32.h"  // constants, funcs for startup and UART
#include "bldc.h"
// Using Hall sensor feedback to commutate a BLDC.
// Pins E0, E1, E2 correspond to the connected state of
// phase A, B, C.  When low, the respective phase is floating
// and when high, the voltage on the phase is determined by
// the value of pins D0 (OC1), D1 (OC2), and D2 (OC3), respectively.
//
// Pins IC1 (RD8), IC2 (RD9), and IC3 (RD10) read the Hall effect sensor output
// and provide velocity feedback using the input capture peripheral.
// When one of the Hall sensor values changes, an interrupt is generated
// by the input capture, and a call to the bldc library updates the
// commutation.

// read the hall effect sensor state

inline unsigned int state() {
  return (PORTD & 0x700) >> 8;
}

static volatile int pwm = 0;    // current PWM percentage

void __ISR(_INPUT_CAPTURE_1_VECTOR,IPL6SRS) commutation_interrupt1(void) {
  IC1BUF;                       // clear the input capture buffer
  bldc_commutate(pwm,state());  // update the commutation
  IFS0bits.IC1IF = 0;           // clear the interrupt flag
}

void __ISR(_INPUT_CAPTURE_2_VECTOR,IPL6SRS) commutation_interrupt2(void) {
  IC2BUF;                       // clear the input capture buffer
  bldc_commutate(pwm,state());  // update the commutation
  IFS0bits.IC2IF = 0;           // clear the interrupt flag
}

void __ISR(_INPUT_CAPTURE_3_VECTOR,IPL6SRS) commutation_interrupt3(void) {
  IC3BUF;                       // clear the input capture buffer
  bldc_commutate(pwm,state());  // update the commutation
  IFS0bits.IC3IF = 0;           // clear the interrupt flag
}

int main(void) {
  NU32_Startup(); // cache on, interrupts on, LED/button init, UART init
  TRISECLR = 0x7; // E0, E1, and E2 are outputs

  bldc_setup();   // initialize the bldc

  // set up input capture to generate interrupts on Hall sensor changes
  IC1CONbits.ICTMR = 1; // use timer 2
  IC1CONbits.ICM = 1;   // interrupt on every rising or falling edge
  IFS0bits.IC1IF = 0;   // enable interrupt for IC1
  IPC1bits.IC1IP = 6;
  IEC0bits.IC1IE = 1;
  
  IC2CONbits.ICTMR = 1; // use timer 2
  IC2CONbits.ICM = 1;   // interrupt on every rising or falling edge
  IC2CONbits.ON = 1;    // enable interrupt for IC2
  IFS0bits.IC2IF = 0;
  IPC2bits.IC2IP = 6;
  IEC0bits.IC2IE = 1;

  IC3CONbits.ICTMR = 1; // use timer 2
  IC3CONbits.ICM = 1;   // interrupt on every rising or falling edge
  IFS0bits.IC3IF = 0;   // enable interrupt for IC3
  IPC3bits.IC3IP = 6;
  IEC0bits.IC3IE = 1;
  
  IC1CONbits.ON = 1;    // start the input capture modules
  IC2CONbits.ON = 1;    
  IC3CONbits.ON = 1;
  
  while(1) {
    int newpwm = bldc_get_pwm();   // prompt user for PWM duty cycle
    __builtin_disable_interrupts();
    pwm = newpwm;
    __builtin_enable_interrupts();
    bldc_commutate(pwm, state());  // ground, PWM, and float the right phases
  }
}
