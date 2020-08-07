/*
 * gpio.h
 *
 *  Created on: Feb 21, 2020
 *      Author: jambox
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <devices/cc13x0/driverlib/ioc.h>
#include <devices/cc13x0/driverlib/gpio.h>
#include <devices/cc13x0/driverlib/prcm.h>
#include "CC1310_LAUNCHXL.h"

void InitGPIO(void);

void SetAndWritePinHigh(uint32_t pin);

void ClearPin(uint32_t pin);

void SetGPIOPin(uint32_t pin);

#endif /* GPIO_H_ */
