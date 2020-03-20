#include "system_config.h"
#include "system_definitions.h"
#include "app.h"

APP_DATA appdata;

void invert_led_callback(uintptr_t context, uint32_t alarmCount) {
  // context is data passed by the user that can then be used in the callback.
  // alarm count tracks how many times the callback has occured
  PLIB_PORTS_PinToggle(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED1_POS); // toggle led 1
  PLIB_PORTS_PinToggle(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED2_POS); // toggle led 2
}

// initialize the application state
void APP_Initialize(void) {
  appdata.state = APP_STATE_INIT;
}

void APP_Tasks(void) {
  switch(appdata.state) {
    case APP_STATE_INIT:
      // turn on LED1 by clearing A5
      PLIB_PORTS_PinClear(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED1_POS); 
      // turn off LED2 by setting A5
      PLIB_PORTS_PinSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED2_POS);   

      // only one client at a time, open the timer
      appdata.handleTmr = DRV_TMR_Open(DRV_TMR_INDEX_0, DRV_IO_INTENT_EXCLUSIVE); 
     
      // timer driver calls invert_led_callback at 5 Hz.  Register this "periodic alarm."
      DRV_TMR_Alarm16BitRegister(appdata.handleTmr, 62499, true, 0, invert_led_callback);
      DRV_TMR_Start(appdata.handleTmr);
      appdata.state = APP_STATE_WAIT;
      break;
    case APP_STATE_WAIT:
      break; // we need not do anything here
  }
}
