// Demonstrates the timer service.
// The timer service allows us to use one hardware timer to run tasks
// at different frequencies.

#include "system_config.h"
#include "system_definitions.h"

void invert_leds_callback(uintptr_t context, uint32_t alarmCount) {
  // context is data passed by the user that can then be used in the callback.
  // Here the context is NU32_LED1_POS or NU32_LED2_POS to tell us which LED to toggle.
  PLIB_PORTS_PinToggle(PORTS_ID_0, NU32_LED_CHANNEL, context); 
}

const static DRV_TMR_INIT init =   // used to configure timer; const so stored in flash
{
  .moduleInit = {SYS_MODULE_POWER_RUN_FULL}, // no power saving
  .tmrId = TMR_ID_1,                         // use Timer1
  .clockSource = DRV_TMR_CLKSOURCE_INTERNAL, // use pbclk
  .prescale = TMR_PRESCALE_VALUE_256,        // use a 1:256 prescaler value
  .interruptSource = INT_SOURCE_TIMER_1,     // ignored because system_config.h 
                                             // has set interrupt mode to false
  .mode = DRV_TMR_OPERATION_MODE_16_BIT,     // use 16-bit mode
  .asyncWriteEnable = false                  // no asynchronous write
};

const static SYS_TMR_INIT sys_init =
{
  .moduleInit = {SYS_MODULE_POWER_RUN_FULL}, // no power saving
  .drvIndex = DRV_TMR_INDEX_0,               // use timer driver 0
  .tmrFreq = 1000                  // base frequency of the system service (Hz)
};

// holds the state of the application
typedef enum {APP_STATE_INIT, APP_STATE_RUN} AppState; 

int main(void) {
  SYS_MODULE_OBJ timer_handle;    // handle to the timer driver
  SYS_MODULE_OBJ devcon_handle;   // device configuration handle
  SYS_TMR_HANDLE sys_tmr;
  SYS_CLK_Initialize(NULL);       // initialize the clock, 
                                  // but tell it to use configuration bit settings
                                  // that were set with the bootloader
                                  
  // initialize the device, default init settings are fine
  devcon_handle = SYS_DEVCON_Initialize(SYS_DEVCON_INDEX_0, NULL); 
  
  // initialize the pins for LEDs
  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED1_POS);
  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED2_POS);
  PLIB_PORTS_PinClear(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED1_POS); 
  PLIB_PORTS_PinSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED2_POS);  
 
  // initialize the timer driver
  timer_handle = DRV_TMR_Initialize(DRV_TMR_INDEX_0, (SYS_MODULE_INIT*)&init);

  // initialize the timer system service
  sys_tmr = SYS_TMR_Initialize(SYS_TMR_INDEX_0,(SYS_MODULE_INIT*)&sys_init);
  
  AppState state = APP_STATE_INIT; // initialize the application state 

  while (1) {
    // based on the application state, we may need to initialize timer callbacks
    switch(state) {
      case APP_STATE_INIT: 
        if(SYS_STATUS_READY == SYS_TMR_Status(sys_tmr)) { 
          // If the timer is ready:
          // Register the timer callbacks to invert LED1 at 5 Hz (200 ms period)
          // & LED2 at 1 Hz (1000 ms period). Both tasks use the same callback function
          // and use the context to determine which LED to invert; however, we could
          // have registered different callback functions.  Note that the context type
          // could also be a pointer, if you want more information passed to the callback
          SYS_TMR_CallbackPeriodic(200,NU32_LED1_POS,invert_leds_callback); 
          SYS_TMR_CallbackPeriodic(1000,NU32_LED2_POS,invert_leds_callback);
          state = APP_STATE_RUN;
        } else {
          // the timer is not ready, so do nothing and let the state machines update
        }
        break;
      case APP_STATE_RUN: 
        break; // we are just running
      }

    //update the device configuration
    SYS_DEVCON_Tasks(devcon_handle); 
    SYS_TMR_Tasks(sys_tmr);
    // update the timer driver state machine
    DRV_TMR_Tasks(timer_handle);
  }
  return 0;
}
