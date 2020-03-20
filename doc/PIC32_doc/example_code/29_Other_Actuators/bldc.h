#ifndef COMMUTATION_H__
#define COMMUTATION_H__

// Set up three PWMs and three digital outputs to control a BLDC via
// the STM L6234.  The three PWM channels (OC1 = D0, OC2 = D1, OC3 = D2) control the
// INx pins for phases A, B, and C, respectively.  The digital outputs
// E0, E1, and E2 control the ENx pins for phases A, B, and C, respectively.
// The PWM uses timer 2.
void bldc_setup(void);

// Perform commutation, given the PWM percentage and the current sensor state.
void bldc_commutate(int pwm, unsigned int state);

// Prompt the user for a signed PWM percentage.
int bldc_get_pwm(void);

#endif
