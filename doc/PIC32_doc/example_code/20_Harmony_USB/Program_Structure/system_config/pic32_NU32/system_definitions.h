#ifndef SYSTEM_DEFINITIONS_H
#define SYSTEM_DEFINITIONS_H

#include <stddef.h>         // some standard C headers with types needed by harmony
                            // defines integer types with fixed sizes, for example
                            // uint32_t is guaranteed to be a 32-bit unsigned integer
                            // uintptr_t is an integer that can be treated as a pointer
                            // (i.e., an unsigned int large enough to hold an address)
#include <stdbool.h>
#include "peripheral/peripheral.h"     // all the peripheral (PLIB) libraries
#include "system/devcon/sys_devcon.h"  // device configuration system service
#include "system/clk/sys_clk.h"        // clock system service
#include "system/common/sys_module.h"  // basic system module
#include "driver/tmr/drv_tmr.h"        // the timer driver

// system object handles
typedef struct {
  SYS_MODULE_OBJ sysDevcon;  // device configuration object
  SYS_MODULE_OBJ drvTmr;     // the timer driver object
} SYSTEM_OBJECTS;

// Declares a global variable that holds the system objects so that all files including
// system_definitions.h can access the handles. Is actually defined and initialized in 
// system_init.c,
extern SYSTEM_OBJECTS sysObj; 
                              
#endif
