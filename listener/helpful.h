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
#include <devices/cc13x0/driverlib/gpio.h>
// #include "zumo.h"

#define TIMER_ON 0
#define TIMER_OFF 1
#define BLED0 IOID_9
#define BLED1 IOID_8
#define BLED2 IOID_13
#define BLED3 IOID_14

void delay(float sec);
void EnableTimer(void);
void SetTimerState(int timer_state_command);
uint32_t GetTime(void);

uint32_t ticsToMicrosec(uint32_t tics);

void print_array(uint16_t * array, uint8_t num_els);

void test_leds(void);
#endif /* HELPFUL_H_ */
