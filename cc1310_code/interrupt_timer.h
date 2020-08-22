/**
 * @file interupt_timer.h
 * @brief Interrupt Timer intiializations for 3 time critical functions:
 * openloop control, sensing and decision making, rf communication
 */

#ifndef INTERRUPT_TIMER_H_
#define INTERRUPT_TIMER_H_

#include <devices/cc13x0/driverlib/timer.h>
#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/interrupt.h>

#include <stdint.h>

void InterTimerEnable();

#endif /* INTERRUPT_TIMER_H_ */
