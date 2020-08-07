/*
 * gpio.c
 *
 *  Created on: Feb 21, 2020
 *      Author: jambox
 */

#include "gpio.h"
//#include "zumo.h"

void InitGPIO(void)
{
    /* GPIO power */
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMLoadSet();
    while (!PRCMLoadGet());
}
    //configure on board LEDs
//    IOCPortConfigureSet(CC1310_LAUNCHXL_PIN_GLED,IOC_PORT_GPIO, IOC_STD_OUTPUT);
//    IOCPortConfigureSet(CC1310_LAUNCHXL_PIN_RLED,IOC_PORT_GPIO, IOC_STD_OUTPUT);
//    GPIO_setOutputEnableDio(CC1310_LAUNCHXL_PIN_GLED, GPIO_OUTPUT_ENABLE);
//    GPIO_setOutputEnableDio(CC1310_LAUNCHXL_PIN_RLED, GPIO_OUTPUT_ENABLE);
//
//    //configure drive outputs
//    IOCPortConfigureSet(M1_DIR,IOC_PORT_GPIO, IOC_STD_OUTPUT);
//    IOCPortConfigureSet(M2_DIR,IOC_PORT_GPIO, IOC_STD_OUTPUT);
//    GPIO_setOutputEnableDio(M1_DIR, GPIO_OUTPUT_ENABLE);
//    GPIO_setOutputEnableDio(M2_DIR, GPIO_OUTPUT_ENABLE);
//
//    // defining output leds
//    IOCPortConfigureSet(BLED0, IOC_PORT_GPIO, IOC_STD_OUTPUT);
//    GPIO_setOutputEnableDio(BLED0, GPIO_OUTPUT_ENABLE);
//
//    IOCPortConfigureSet(BLED1, IOC_PORT_GPIO, IOC_STD_OUTPUT);
//    GPIO_setOutputEnableDio(BLED1, GPIO_OUTPUT_ENABLE);
//
//    IOCPortConfigureSet(BLED2, IOC_PORT_GPIO, IOC_STD_OUTPUT);
//    GPIO_setOutputEnableDio(BLED2, GPIO_OUTPUT_ENABLE);
//
//    IOCPortConfigureSet(BLED3, IOC_PORT_GPIO, IOC_STD_OUTPUT);
//    GPIO_setOutputEnableDio(BLED3, GPIO_OUTPUT_ENABLE);
//}

void SetAndWritePinHigh(uint32_t pin)
{
    IOCPortConfigureSet(pin, IOC_PORT_GPIO, IOC_STD_OUTPUT);
    GPIO_setOutputEnableDio(pin, GPIO_OUTPUT_ENABLE);
    GPIO_setDio(pin);
}

void SetGPIOPin(uint32_t pin)
{
    IOCPortConfigureSet(pin, IOC_PORT_GPIO, IOC_STD_OUTPUT);
    GPIO_setOutputEnableDio(pin, GPIO_OUTPUT_ENABLE);
}

void ClearPin(uint32_t pin)
{
    GPIO_clearDio(pin);
}
