#ifndef SYSTEM_CONFIG_H__
#define SYSTEM_CONFIG_H__

// Suppresses warnings from parts of Harmony that are not yet fully
// implemented by Microchip.
#define _PLIB_UNSUPPORTED

// system clock settings

// The NU32 system clock oscillator frequency, 8 MHz.  (This is the oscillator frequency,
// not the final SYSCLK frequency.)
#define  SYS_CLK_CONFIG_PRIMARY_XTAL 8000000L

// The secondary oscillator frequency.  There is no secondary oscillator on the NU32.
#define SYS_CLK_CONFIG_SECONDARY_XTAL 0

// If we were asking Harmony to automatically determine clock multipliers and divisors
// to achieve our 80 MHz SYSCLK from the 8 MHz external oscillator, this tolerance 
// would be the largest acceptable error.  
#define SYS_CLK_CONFIG_FREQ_ERROR_LIMIT 10

// timer driver settings

// The PIC32 has five hardware timers.  This example uses only one, so we could set
// this number to one to save some RAM (driver instances are stored in a statically
// allocated array, so the more instances, the more RAM used).
#define DRV_TMR_INSTANCES_NUMBER 5

// Set this to true to enable interrupts.  We do not use interrupts in this example.
#define DRV_TMR_INTERRUPT_MODE false

// Some definitions for the NU32 board.
#define NU32_LED_CHANNEL PORT_CHANNEL_F  // port channel for the NU32 LEDs
#define NU32_LED1_POS    PORTS_BIT_POS_0    
#define NU32_LED2_POS    PORTS_BIT_POS_1

#endif
