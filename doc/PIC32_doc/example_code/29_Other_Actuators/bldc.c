#include "NU32.h" 

void bldc_setup() {
  TRISECLR = 0x7;          // E0, E1, and E2 are outputs
  
  // Set up timer 2 to use as the PWM clock source.
  T2CONbits.TCKPS = 1;     // Timer2 prescaler N=2 (1:2)
  PR2 = 1999;              // period = (PR2+1) * N * 12.5 ns = 50 us, 20 kHz

  // Set up OC1, OC2, and OC3 for PWM mode; use defaults otherwise.
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC2CONbits.OCM = 0b110;
  OC3CONbits.OCM = 0b110;

  T2CONbits.ON = 1;        // turn on Timer2
  OC1CONbits.ON = 1;       // turn on OC1
  OC2CONbits.ON = 1;       // turn on OC2
  OC3CONbits.ON = 1;       // turn on OC3
}

// A convenient new type to use the mnemonic names PHASE_A, etc.  
// PHASE_NONE is not needed, but there for potential error handling.

typedef enum {PHASE_A = 0, PHASE_B = 1, PHASE_C = 2, PHASE_NONE = 3} phase;

// Performs the actual commutation: one phase is PWMed, one is grounded, the third floats.
// The sign of the PWM determines which phase is PWMed and which is low, so the motor can 
// spin in either direction.  

static void phases_set(int pwm, phase p1, phase p2) { 

  // an array of the addresses of the PWM duty cycle-controlling SFRs
  static volatile unsigned int * ocr[] = {&OC1RS, &OC2RS, &OC3RS}; 

  // If p1 and p2 are the energized phases, then floating[p1][p2] gives
  // the floating phase.  Note p1 should not equal p2 (that would be an error), so 
  // the diagonal entries in the 2d matrix are the bogus PHASE_NONE.
  static phase floating[3][3] = {{PHASE_NONE, PHASE_C,    PHASE_B},
                                 {PHASE_C,    PHASE_NONE, PHASE_A},
                                 {PHASE_B,    PHASE_A,    PHASE_NONE}};

  // elow_bits[pfloat] takes the floating phase pfloat (e.g., pfloat could be PHASE_A)
  // and returns a 3-bit value with a zero in the column corresponding to the
  // floating phase (0th column = A, 1st column = B, 2nd column = C).  
  static int elow_bits[3] = {0b110, 0b101, 0b011};

  phase pfloat = floating[p1][p2]; // the floating phase
  phase phigh, plow;               // phigh is the PWMed phase, plow is the grounded phase
  int apwm;                        // magnitude of the pwm count

  // choose the appropriate direction
  if(pwm > 0) {
    phigh = p1;
    plow =  p2;
    apwm = pwm;
  } else {
    phigh = p2;
    plow = p1;
    apwm = -pwm;
  }
  // Pin E0 controls enable for phase A; E1 for B; E2 for C.
  // The pfloat phase should have its pin be 0; other pins should be 1.
  LATE = (LATE & ~0x7) | elow_bits[pfloat];
  
  // set the PWM's appropriately by setting the OCxRS SFRs
  *ocr[pfloat] = 0;   // floating pin has 0 duty cycle
  *ocr[plow] = 0;     // low will always be low, 0 duty cycle
  *ocr[phigh] = apwm; // the high phase gets the actual duty cycle
}

// Given the Hall sensor state, use the mapping between sensor readings and 
// energized phases given in the Pittman ELCOM motor data sheet to determine the
// correct phases to PWM and GND and the phase to leave floating.

void bldc_commutate(int pwm, unsigned int state) {
  pwm = ((int)PR2 * pwm)/100; // convert pwm to ticks
  switch(state) {
    case 0b100:
      phases_set(pwm,PHASE_B, PHASE_A); // if pwm > 0, phase A = GND and B is PWMed
      break;                            // if pwm < 0, phase B = GND and A is PWMed
    case 0b110:
      phases_set(pwm,PHASE_C, PHASE_A);
      break;
    case 0b010:
      phases_set(pwm, PHASE_C, PHASE_B);
      break;
    case 0b011:
      phases_set(pwm, PHASE_A, PHASE_B);
      break;
    case 0b001:
      phases_set(pwm,PHASE_A,PHASE_C);
      break;
    case 0b101:
      phases_set(pwm,PHASE_B, PHASE_C);
      break;
    default:
      NU32_WriteUART3("ERROR: Read the state incorrectly!\r\n"); // ERROR!
  }
}

// Get the signed PWM duty cycle from the user.

int bldc_get_pwm() {
  char msg[100];
  int newpwm;
  NU32_WriteUART3("Enter signed PWM duty cycle (-100 to 100): ");
  NU32_ReadUART3(msg, sizeof(msg));
  NU32_WriteUART3(msg);
  NU32_WriteUART3("\r\n");
  sscanf(msg,"%d", &newpwm);
  return newpwm;
}
