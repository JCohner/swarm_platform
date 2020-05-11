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
    uint32_t sensor_flags[4] = {0,0,0,0};
    int i, j;
    emmitters_on();
    SetTimerState(TIMER_ON);
    uint32_t startTime = GetTime();
//    sprintf(buffer, "start time: %u\r\n", startTime);
//    WriteUART0(buffer);
    uint32_t IRTime = 0;
    while (IRTime < MAX_SAMPLE_TIME)
    {
        ReadADC(temp_vals);
        for (j = 0; j < NUM_SAMPLES; j++)
        {
            for (i = 0; i < NUM_SENSORS; i++)
            {
                IRTime = ticsToMicrosec(GetTime() - startTime);
                temp_vals[i] += IRTime / NUM_SAMPLES;
    //            vals[i] += temp_vals[i] / NUM_SAMPLES; //should divide each value by 4
            }
        }


        for(i = 0; i < NUM_SENSORS; i++)
        {
            if ((temp_vals[i] < 2400) && !(sensor_flags[i]))
            {
                vals[i] = IRTime;
                sensor_flags[i] = 1;
            }
        }
    }

    for (i = 0; i < NUM_SENSORS; i++)
    {
        if (vals[i] == 0)
        {
            vals[i] = MAX_SAMPLE_TIME;
        }
    }

    SetTimerState(TIMER_OFF);
}

