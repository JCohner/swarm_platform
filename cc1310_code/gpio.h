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

#endif /* GPIO_H_ */
