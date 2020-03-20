#ifndef SYSTEM_CONFIG_H__
#define SYSTEM_CONFIG_H__

// Suppresses warnings from parts of Harmony that are not yet fully
// implemented by Microchip.
#define _PLIB_UNSUPPORTED

// The number of UART driver instances needed by the program.  If you wanted
// to use UART1, UART2, and UART3, for example, this value should be 3.
#define DRV_USART_INSTANCES_NUMBER 1

// Multiple clients could concurrently use the driver.  The function
// DRV_USART_Open creates a client that code uses to access the driver.
// To allow the driver to manage concurrent perhipheral access from multiple
// tasks (e.g., mainline code and a timer ISR), you should have each task
// create its own client.  Not all drivers support concurrent access;
// consult the Harmony documentation.  Usually sufficient to set this to 1.
#define DRV_USART_CLIENTS_NUMBER 1

// Can be true or false.  If true, the driver FSMs are updated in interrupts,
// meaning that the various DRV_USART_Tasks should be called from ISRs.  Our
// program does not use interrupts.
#define DRV_USART_INTERRUPT_MODE false

// Use the read/write UART model.
#define DRV_USART_READ_WRITE_MODEL_SUPPORT true

// This is not used by Harmony, so we use APP_ as the prefix (our application).
// This is used in the initialization of the UART so the driver can generate
// the proper baud rate.
#define APP_PBCLK_FREQUENCY 80000000L 

#endif
