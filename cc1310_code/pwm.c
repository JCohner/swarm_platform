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
    // Route the timer pwm output to a physical pin
    // EVENT0: GPT0 A
    // EVENT1: GPT0 B
    // EVENT2: GPT1 A ... //from https://e2e.ti.com/support/wireless-connectivity/sub-1-ghz/f/156/p/515712/1894843#1894843
    IOCIOPortIdSet(M1_PWM, IOC_PORT_MCU_PORT_EVENT0);
    IOCIOPortIdSet(M2_PWM, IOC_PORT_MCU_PORT_EVENT1);
    IOCPortConfigureSet(M2_PWM, IOC_PORT_MCU_PORT_EVENT1, IOC_IOMODE_NORMAL);
    IOCPortConfigureSet(M1_PWM, IOC_PORT_MCU_PORT_EVENT0, IOC_IOMODE_NORMAL);

    //configure timer
    TimerDisable(GPT0_BASE,TIMER_A);
    TimerDisable(GPT0_BASE,TIMER_B);
    TimerConfigure(GPT0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM);
    // Set 2MHz (count until 24-1 before reset)
    TimerLoadSet(GPT0_BASE, TIMER_A, 1023);
    TimerLoadSet(GPT0_BASE, TIMER_B, 1023);
    // Set duty cycle to 50% (12 is not exact, but that's impossible with 23 as load value)
    TimerMatchSet(GPT0_BASE, TIMER_A, 512);
    TimerMatchSet(GPT0_BASE, TIMER_B, 256);
    TimerEnable(GPT0_BASE ,TIMER_A);
    TimerEnable(GPT0_BASE ,TIMER_B);
}

void PWMSet(int motor, int value)
{
    //setting to 0 sets 100%, setting to 1022 sets to ~0%
    if (motor == M1_PWM){
        TimerMatchSet(GPT0_BASE, TIMER_A, (value % 1024));
    } else if (motor == M2_PWM) {
        TimerMatchSet(GPT0_BASE, TIMER_B, (value % 1024));
    }
}

