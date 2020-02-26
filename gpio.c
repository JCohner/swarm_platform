/*
 * gpio.c
 *
 *  Created on: Feb 21, 2020
 *      Author: jambox
 */

#include "gpio.h"

void InitGPIO(void)
{
    /* GPIO power */
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMLoadSet();
    while (!PRCMLoadGet());
    IOCPortConfigureSet(CC1310_LAUNCHXL_PIN_GLED,IOC_PORT_GPIO, IOC_STD_OUTPUT);
    IOCPortConfigureSet(CC1310_LAUNCHXL_PIN_RLED,IOC_PORT_GPIO, IOC_STD_OUTPUT);
    GPIO_setOutputEnableDio(CC1310_LAUNCHXL_PIN_GLED, GPIO_OUTPUT_ENABLE);
    GPIO_setOutputEnableDio(CC1310_LAUNCHXL_PIN_RLED, GPIO_OUTPUT_ENABLE);
}
