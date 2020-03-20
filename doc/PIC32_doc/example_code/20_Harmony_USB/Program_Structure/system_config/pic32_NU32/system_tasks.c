#include "system_config.h"
#include "system_definitions.h"
#include "app.h"

// update the state machines
void SYS_Tasks(void) {
  SYS_DEVCON_Tasks(sysObj.sysDevcon);
  // timer tasks are interrupt driven, not polled.
  // for a polled application uncomment below 
  // DRV_TMR_Tasks(sysObj.drvTmr);
  APP_Tasks(); // application specific tasks
}
