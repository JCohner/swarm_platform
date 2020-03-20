#include "system_config.h"
#include "system_definitions.h"
#include <sys/attribs.h>

// the timer interrupt.  note that this just updates the state of the timer
void __ISR(_TIMER_1_VECTOR, IPL5SOFT) Timer1ISR(void) {  
  DRV_TMR_Tasks_ISR(sysObj.drvTmr); // update the timer state machine
}
