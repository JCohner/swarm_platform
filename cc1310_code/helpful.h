/*
 * helpful.h
 *
 *  Created on: Apr 26, 2020
 *      Author: jambox
 */

#ifndef HELPFUL_H_
#define HELPFUL_H_

#include <stdint.h>
#include <devices/cc13x0/driverlib/timer.h>
#include <devices/cc13x0/driverlib/prcm.h>

#define TIMER_ON 0
#define TIMER_OFF 1


void delay(float sec);
void EnableTimer(void);
void SetTimerState(int timer_state_command);
uint32_t GetTime(void);

uint32_t ticsToMicrosec(uint32_t tics);

#endif /* HELPFUL_H_ */
