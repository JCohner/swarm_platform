/*
 * pwm.h
 *
 *  Created on: Feb 28, 2020
 *      Author: jambox
 */

#ifndef PWM_H_
#define PWM_H_

#include <devices/cc13x0/driverlib/timer.h>
#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/ioc.h>
#include "zumo.h"
void PWMEnable();
void PWMSet(int motor, int value);
#endif /* PWM_H_ */
