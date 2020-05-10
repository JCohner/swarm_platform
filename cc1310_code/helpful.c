/*
 * helpful.c
 *
 *  Created on: Apr 26, 2020
 *      Author: jambox
 */

#include "helpful.h"
#include "uart.h"

char buffer[50];

void delay(float sec)
{
    uint32_t num_ticks = (uint32_t) (sec * 4800000); //weird behavior: seems to work when clk freq is 4.8 * 10^6 not 48 * 10^6

//    sprintf(buffer, "num ticks: %u\r\n", num_ticks);
//    WriteUART0(buffer);


    uint32_t curr_time = 0;

    while ((curr_time) < num_ticks){
        ++curr_time;
    }

}

void EnableTimer()
{
    //enable peripheral
   PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER1);
   PRCMLoadSet();
   while ( !PRCMLoadGet() );
   TimerDisable(GPT1_BASE,TIMER_A);
   TimerConfigure(GPT1_BASE, TIMER_CFG_PERIODIC_UP);
   TimerEnable(GPT1_BASE ,TIMER_A);
}

void SetTimerState(int timer_state_command)
{
    if(timer_state_command)
    {
        TimerDisable(GPT1_BASE,TIMER_A);
    }
    else
    {
        TimerEnable(GPT1_BASE,TIMER_A);
    }

}

uint32_t GetTime()
{
    return TimerValueGet(GPT1_BASE ,TIMER_A);
}
