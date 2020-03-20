#ifndef SYSTEM_DEFINITIONS_H
#define SYSTEM_DEFINITIONS_H

#include <stddef.h>         // Standard C header defining NULL and types used by Harmony
#include <stdbool.h>        // Standard C header defining type bool (true/false)
#include "peripheral/peripheral.h"  // The Harmony PLIB library
#include "system/devcon/sys_devcon.h" // DEVCON handles cache, other config tasks.
#include "system/clk/sys_clk.h"// Clock header. Control and query oscillator properties.
#include "system/common/sys_module.h"//Basic system module, used by most Harmony projects
#include "driver/tmr/drv_tmr.h"      // The timer driver
#include <sys/attribs.h>    // defines the __ISR macro, needed when we use interrupts
#endif
