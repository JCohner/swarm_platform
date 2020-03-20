#include "NU32.h" // constants, funcs for startup and UART
#include "bldc.h"

// Open-loop control of the Pittman ELCOM SL 4443S013 BLDC operating at no load and 9 V.
// Pins E0, E1, E2 correspond to the connected state of
// phase A, B, C.  When low, the respective phase is floating,
// and when high, the voltage on the phase is determined by
// the value of pins D0 (OC1), D1 (OC2), and D2 (OC3), respectively.

// Some values below have been tuned experimentally for the Pittman motor.

#define MAX_RPM 1800 // the max speed of the motor (no load speed), in RPM, for 9 V 
#define MIN_PWM 3    // the min PWM required to overcome friction, as a percentage
#define SLOPE (MAX_RPM/(100 - MIN_PWM)) // slope of the RPM vs PWM curve
#define OFFSET (-SLOPE*MIN_PWM) // RPM where the RPM vs PWM curve intersects the RPM axis
                           // NOTE: RPM = SLOPE * PWM + OFFSET
#define TICKS_MIN 1200000  // number of 20 kHz (50 us) timer ticks in a minute
#define EMREV 2     // number of electrical revs per mechanical revs (erev/mrev)
#define PHASE_REV 6 // the number of phases per electrical revolution (phase/erev)

// convert minutes/mechanical revolution into ticks/phase (divide TP_PER_MPR by the rpm)
#define TP_PER_MPR (TICKS_MIN/(PHASE_REV *EMREV))

#define ACCEL_PERIOD 200000 // Time in 40 MHz ticks to wait before accelerating to next 
                            // PWM level (i.e., the higher this value, the slower
                            // the acceleration).  When doing open-loop control, you
                            // must accel/decel slowly enough, otherwise you lose sync.
                            // The PWM is adjusted by 1 percent in each accel period,
                            // but the deadband where the motor does not move is skipped.

static volatile int pwm = 0;    // current PWM as a percentage
static volatile int period = 0; // commutation period, in 50 us (1/20 kHz) ticks 

void __ISR(_TIMER_2_VECTOR, IPL6SRS) timer2_ISR(void) { // entered every 50 us (1/20 kHz)
  // the states, in the order of rotation through the phases (what we'd expect to read)
  static unsigned int state_table[] = {0b101,0b001,0b011,0b010,0b110,0b100};
  
  static int phase = 0;
  static int count = 0; // used to commutate when necessary
  if(count >= period) {
    count = 0;
    if(pwm > MIN_PWM) {
      ++phase;
    } else if (pwm < -MIN_PWM){
      --phase;
    }
    if(phase == 6) {
      phase = 0;
    } else if (phase == -1) {
      phase = 5;
    }
    bldc_commutate(pwm, state_table[phase]);
  } else {
    ++count;
  }
  IFS0bits.T2IF = 0;
}

// return true if the PWM percentage is in the deadband region
int in_deadband(int pwm) {
  return -MIN_PWM <= pwm && pwm <= MIN_PWM;
}

int main(void) {
  char msg[100];
  
  NU32_Startup(); // cache on, interrupts on, LED/button init, UART init
  bldc_setup();
  
  // Set up Timer2 interrupts (the bldc already uses Timer2 for the PWM).
  // We just reuse it for our timer here.
  IPC2bits.T2IP = 6;
  IFS0bits.T2IF = 0;
  IEC0bits.T2IE = 1;

  while(1) {
    int newpwm = bldc_get_pwm();  // get new PWM from user
    if(in_deadband(newpwm)) {     // if PWM is in deadband where motor doesn't move, pwm=0
      __builtin_disable_interrupts();
      period = 0;
      pwm = 0;
      __builtin_enable_interrupts();
    } else {
      // newpwm is not in the deadband
      int curr_pwm = pwm;
      _CP0_SET_COUNT(0);

      while(curr_pwm != newpwm) {  // ramp the PWM up or down, respecting accel limits
        int comm_period;
 
        if(curr_pwm > newpwm) {
          --curr_pwm;
          // skip the deadband
          if(in_deadband(curr_pwm)) {
            curr_pwm = - MIN_PWM - 1;
          }
        } else if(curr_pwm < newpwm) {
          ++curr_pwm;
          if(in_deadband(curr_pwm)) {
            curr_pwm = MIN_PWM + 1;
          }
        }
        // divide T_PER_MPR by the RPM to get the commutation period
        // We compute the RPM based on the RPM vs pwm curve RPM = SLOPE pwm + OFFSET
        comm_period =  (TP_PER_MPR/(SLOPE*abs(curr_pwm)+ OFFSET));
        while(_CP0_GET_COUNT() < ACCEL_PERIOD) { ; } // delay until accel period over
        __builtin_disable_interrupts();
        period = comm_period;
        pwm = curr_pwm;
        __builtin_enable_interrupts();
        _CP0_SET_COUNT(0); // we just moved to a new pwm, reset the acceleration period
      }
      sprintf(msg,"PWM Percent: %d, PERIOD: %d\r\n",pwm,period);
      NU32_WriteUART3(msg);
    }
  }
}
