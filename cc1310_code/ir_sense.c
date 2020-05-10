/*
 * ir_sense.c
 *
 *  Created on: May 10, 2020
 *      Author: jambox
 */
#include "ir_sense.h"

char buffer[50];

void IR_SenseSetup(void)
{
    EnableTimer();
}


void emmitters_on(void)
{
    int i;
    for (i = 0; i < NUM_SENSORS; i++)
    {
        SetAndWritePinHigh(adc_outputs[i]);
    }

    delay(200 * 10e-6);
}

void ReadIR(uint32_t * vals)
{
    //initialize all values to zero
    memset(vals, 0, NUM_SENSORS * 4);
    uint32_t temp_vals[4] = {0,0,0,0};
    int i, j;
    emmitters_on();
    SetTimerState(TIMER_ON);
    uint32_t startTime = GetTime();
//    sprintf(buffer, "start time: %u\r\n", startTime);
//    WriteUART0(buffer);
    uint32_t IRTime;
    for (j = 0; j < NUM_SAMPLES; j++)
    {
        ReadADC(temp_vals);
        for (i = 0; i < NUM_SENSORS; i++)
        {
//            IRTime = GetTime() - startTime;
//            if (temp_vals[i] < 2400)
//            {
//
//            }
            vals[i] += temp_vals[i] / NUM_SAMPLES; //should divide each value by 4
        }
    }
    SetTimerState(TIMER_OFF);
}

