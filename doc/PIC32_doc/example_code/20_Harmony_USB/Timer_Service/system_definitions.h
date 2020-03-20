#ifndef SYSTEM_DEFINITIONS_H
#define SYSTEM_DEFINITIONS_H

#include<stddef.h>              // some standard C headers with types needed by Harmony
#include<stdbool.h>
#include"peripheral/peripheral.h"     // all the peripheral (plib) libraries
#include"system/devcon/sys_devcon.h"  // device configuration system service
#include"system/clk/sys_clk.h"        // clock system service
#include"system/common/sys_module.h" // basic module system, used by all harmony projects

#include"driver/tmr/drv_tmr.h"   // the timer driver
#include"system/tmr/sys_tmr.h"   // the timer service
#endif
