/*
 * interupt_timer.c
 *
 *  Created on: Jul 5, 2020
 *      Author: jambox
 */

#include <interrupt_timer.h>

#include "gpio.h"
#include "zumo_moves.h"

void TimerInt()
{
    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
    TimerIntClear(GPT1_BASE, TIMER_TIMB_TIMEOUT);


}

void InterTimerEnable()
{

    //enable peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER1);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    TimerDisable(GPT1_BASE,TIMER_A);
    TimerDisable(GPT1_BASE,TIMER_B);


    TimerConfigure(GPT1_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC)); //
    TimerPrescaleSet(GPT1_BASE, TIMER_A, 0xFF); // this doesnt work because it thinks its in 32 bit mode
    TimerLoadSet(GPT1_BASE, TIMER_A, 0xFFFF / 256);
//    TimerMatchSet(GPT1_BASE, TIMER_A, 0xFFFF / 4);
//
    TimerPrescaleSet(GPT1_BASE, TIMER_B, 0xFF);
    TimerLoadSet(GPT1_BASE, TIMER_B, 0xFFFF / 512);
//    TimerMatchSet(GPT1_BASE, TIMER_B, 0xFFFF / 2);


    IntMasterEnable();
    TimerIntRegister(GPT1_BASE, TIMER_A, openloop_turn); //
    TimerIntEnable(GPT1_BASE, TIMER_TIMA_TIMEOUT);
    IntPrioritySet(INT_GPT1A, INT_PRI_LEVEL3);
    IntEnable(INT_GPT1A);
//
    TimerIntRegister(GPT1_BASE, TIMER_B, TimerInt);
    TimerIntEnable(GPT1_BASE, TIMER_TIMB_TIMEOUT);
    IntPrioritySet(INT_GPT1B, INT_PRI_LEVEL4);
    IntEnable(INT_GPT1B);

//    TimerLoadSet(GPT1_BASE,TIMER_A,  48e6 / 2); //this line is needed think harder bout it

    TimerEnable(GPT1_BASE,TIMER_A);
    TimerEnable(GPT1_BASE,TIMER_B);
}


