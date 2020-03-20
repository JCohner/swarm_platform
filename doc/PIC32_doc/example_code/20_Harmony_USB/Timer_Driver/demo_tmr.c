// demonstrates the timer driver

#include "system_config.h"
#include "system_definitions.h"

void invert_leds_callback(uintptr_t context, uint32_t alarmCount) {
  // context is data passed by the user that can then be used in the callback.
  // alarmCount tracks how many times the callback has occurred.
  PLIB_PORTS_PinToggle(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED1_POS); // toggle led 1
  PLIB_PORTS_PinToggle(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED2_POS); // toggle led 2
}

const static DRV_TMR_INIT init =   // used to configure timer; const so stored in flash
{
  .moduleInit = {SYS_MODULE_POWER_RUN_FULL},   // no power saving
  .tmrId = TMR_ID_1,                           // use timer 1
  .clockSource = DRV_TMR_CLKSOURCE_INTERNAL,   // use pbclk
  .prescale = TMR_PRESCALE_VALUE_256,          // use a 1:256 prescaler value
  .interruptSource = INT_SOURCE_TIMER_1,       // ignored since system_config has set 
                                               // interrupt mode to false
  .mode = DRV_TMR_OPERATION_MODE_16_BIT,       // use 16 bit mode
  .asyncWriteEnable = false                    // no asynchronous write
};

int main(void) {
  SYS_MODULE_OBJ timer_handle;     // handle to the timer driver
  SYS_MODULE_OBJ devcon_handle;    // device configuration handle
  DRV_HANDLE timer1;               // handle to the timer
 
  SYS_CLK_Initialize(NULL);        // initialize the clock, but tell it to use 
                                   // configuration bit 
                                   // settings that were set with the bootloader
                                  
  // initialize the DEVCON system service, default init settings are fine.
  devcon_handle = SYS_DEVCON_Initialize(SYS_DEVCON_INDEX_0, NULL); 

  // initialize the LED pins as outputs
  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED1_POS); 
  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED2_POS); 
  PLIB_PORTS_PinClear(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED1_POS); // turn on LED1 
  PLIB_PORTS_PinSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED2_POS);   // turn off LED2 
 
  // initialize the timer driver
  timer_handle = DRV_TMR_Initialize(DRV_TMR_INDEX_0, (SYS_MODULE_INIT*)&init);

  // open the timer, this is the only client (only place where DRV_TMR_Open is called)
  timer1 = DRV_TMR_Open(DRV_TMR_INDEX_0, DRV_IO_INTENT_EXCLUSIVE); 

  // the timer driver will call invert_leds_callback at 5 Hz. This is a "periodic alarm."
  DRV_TMR_Alarm16BitRegister(timer1, 62499, true, 0, invert_leds_callback);
  
  // start the timer
  DRV_TMR_Start(timer1);
  
  while (1) {
    // update device configuration. Does nothing in Harmony v1.06 but may in the future
    SYS_DEVCON_Tasks(devcon_handle); 
    // update the timer driver state machine
    DRV_TMR_Tasks(timer_handle);
  }
  return 0;
}
