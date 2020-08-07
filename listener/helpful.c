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

uint32_t ticsToMicrosec(uint32_t tics)
{
    return (tics / 48.0);
}

//uint32_t microsecToTic(float usec)
//{
//
//}


void EnableTimer()
{
    //enable peripheral
   PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER2);
   PRCMLoadSet();
   while ( !PRCMLoadGet() );
   TimerDisable(GPT2_BASE,TIMER_A);
   TimerConfigure(GPT2_BASE, TIMER_CFG_PERIODIC_UP);
   TimerEnable(GPT2_BASE ,TIMER_A);
}

void SetTimerState(int timer_state_command)
{
    if(timer_state_command)
    {
        TimerDisable(GPT2_BASE,TIMER_A);
    }
    else
    {
        TimerEnable(GPT2_BASE,TIMER_A);
    }

}

uint32_t GetTime()
{
    return TimerValueGet(GPT2_BASE ,TIMER_A);
}

void print_array(uint16_t * array, uint8_t num_els)
{
    int i;
    char buff[300];
    for (i = 0; i < num_els; i++)
    {
        sprintf(buff, "%u ", array[i]);
        WriteUART0(buff);
    }
    WriteUART0("\r\n");
}

void test_leds(){
  while(1)
  {
      GPIO_toggleDio(BLED0);
      GPIO_toggleDio(BLED1);
      GPIO_toggleDio(BLED2);
      GPIO_toggleDio(BLED3);
      delay(.1);
  }
}

