/*
 * pwm.c
 *
 *  Created on: Feb 28, 2020
 *      Author: jambox
 */
#include "pwm.h"

void PWMEnable()
{
    //enable peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER0);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    //configure our output pwm pin
    IOCIOPortIdSet(IOID_12, IOC_PORT_MCU_PORT_EVENT0);
    IOCPortConfigureSet(IOID_12, IOC_PORT_MCU_PORT_EVENT0, IOC_IOMODE_NORMAL);

    //configure timer
    TimerDisable(GPT0_BASE,TIMER_A);
    TimerConfigure(GPT0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM);
    // Set 2MHz (count until 24-1 before reset)
    TimerLoadSet(GPT0_BASE, TIMER_A, 23);
    // Set duty cycle to 50% (12 is not exact, but that's impossible with 23 as load value)
    TimerMatchSet(GPT0_BASE, TIMER_A, 12);
    TimerEnable(GPT0_BASE ,TIMER_A);
}



