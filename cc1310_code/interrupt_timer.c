/*
 * interupt_timer.c
 *
 *  Created on: Jul 5, 2020
 *      Author: jambox
 */

#include <interrupt_timer.h>

#include "gpio.h"

void TimerInt()
{
    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
    //documentation recomends this isn't the very last function call
    TimerIntClear(GPT1_BASE, TIMER_TIMA_TIMEOUT);
}

void InterTimerEnable()
{

    //enable peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER1);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    TimerDisable(GPT1_BASE,TIMER_A);

    TimerConfigure(GPT1_BASE, TIMER_CFG_A_PERIODIC);

    TimerPrescaleSet(GPT1_BASE, TIMER_A, 0xFF);
    TimerPrescaleMatchSet(GPT1_BASE, TIMER_A, 0xFF);
    TimerLoadSet(GPT1_BASE, TIMER_A, 0xFFFF); // Period = 255 * 65535 / freq = 0.25 sec
    TimerMatchSet(GPT1_BASE, TIMER_A, 0xFFFF / 2); // PulseWidth = Period / 2 = 0.125 sec

    IntMasterEnable();
    TimerIntRegister(GPT1_BASE, TIMER_A, TimerInt);
    TimerIntEnable(GPT1_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_GPT1A);
    TimerLoadSet(GPT1_BASE,TIMER_A,  48e6);

    TimerEnable(GPT1_BASE,TIMER_A);

}


