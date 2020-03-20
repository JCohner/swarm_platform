#include "system_config.h"
#include "system_definitions.h"
#include "app.h"

// Code to initialize the system.
// For standalone projects (those without a bootloader)
// you would define the configuration values here, e.g.,
// #pragma config FWDTEN = OFF
// See NU32.h for configuration bits.  These are set by the bootloader in this example.

const static DRV_TMR_INIT init = // used to configure the timer; const so stored in flash
{
  .moduleInit = SYS_MODULE_POWER_RUN_FULL,     // no power saving
  .tmrId = TMR_ID_1,                           // use timer 1
  .clockSource = DRV_TMR_CLKSOURCE_INTERNAL,   // use pbclk
  .prescale = TMR_PRESCALE_VALUE_256,          // use a 1:256 prescaler value
  .interruptSource = INT_SOURCE_TIMER_1,       // use timer one interrupts
  .mode = DRV_TMR_OPERATION_MODE_16_BIT,       // use 16-bit mode
  .asyncWriteEnable = false             
};

SYSTEM_OBJECTS sysObj; // handles to harmony modules. Defined in system_definitions.h.

// called from the beginning of main to initialize the harmony components etc.
void SYS_Initialize(void * data) {
  SYS_CLK_Initialize(NULL);  // initialize the clock, but use configuration bit settings
                             // that were set with the bootloader
                                 
                          // Initialize the device, default init settings are fine.
                          // As of harmony 1.06 this call is not needed for our purposes,
                          // but this may change in future versions so we include it.
                          // It is necessary if you want to set the prefetch cache
                          // and wait states using SYS_DEVCON_PerformanceConfig.
                          // However, we need not configure the cache and wait states 
                          // the bootloader has already done this for us.
  sysObj.sysDevcon = SYS_DEVCON_Initialize(SYS_DEVCON_INDEX_0, NULL); 
  
  // initialize the pins for the LEDs
  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED1_POS); 
  PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, NU32_LED_CHANNEL, NU32_LED2_POS); 
 
  // initialize the timer driver
  sysObj.drvTmr = DRV_TMR_Initialize(DRV_TMR_INDEX_0, (SYS_MODULE_INIT*)&init);

  PLIB_INT_MultiVectorSelect(INT_ID_0); // enable multi-vector interrupt mode
 
  // set timer int priority
  PLIB_INT_VectorPrioritySet(INT_ID_0, INT_VECTOR_T1, INT_PRIORITY_LEVEL5); 
  PLIB_INT_Enable(INT_ID_0);            // enable interrupts

  // initialize the apps
  APP_Initialize();
}
