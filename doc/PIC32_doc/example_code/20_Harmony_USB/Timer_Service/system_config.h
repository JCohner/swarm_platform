#ifndef SYSTEM_CONFIG_H__
#define SYSTEM_CONFIG_H__

// avoid superfluous warnings
#define _PLIB_UNSUPPORTED

// system clock settings.

// the system clock frequency, 8MHz (this is the oscillator frequency not
// the final frequency)
#define  SYS_CLK_CONFIG_PRIMARY_XTAL 8000000L

// define this to stop complaints, but secondary oscillator is disabled
#define SYS_CLK_CONFIG_SECONDARY_XTAL 0

// allowed frequency error, in Hz
#define SYS_CLK_CONFIG_FREQ_ERROR_LIMIT 10

// timer driver settings

// there are 5 hardware timers
#define DRV_TMR_INSTANCES_NUMBER 5

// do not use interrupt mode
#define DRV_TMR_INTERRUPT_MODE false

// system timer configuration
// max number of timer objects that can be created
#define SYS_TMR_MAX_CLIENT_OBJECTS 5 

// frequency of the clock that the system service uses, 10 kHz.
// other frequencies are derived from this 10 kHz clock.
#define SYS_TMR_UNIT_RESOLUTION 10000

// tolerance for the resulting client frequency
#define SYS_TMR_CLIENT_TOLERANCE 10

// tolerance for the system 
#define SYS_TMR_FREQUENCY_TOLERANCE 10

// some definitions for the NU32 board
#define NU32_LED_CHANNEL PORT_CHANNEL_F  // port channel for the NU32 LEDs
#define NU32_LED1_POS PORTS_BIT_POS_0    
#define NU32_LED2_POS PORTS_BIT_POS_1

#endif
