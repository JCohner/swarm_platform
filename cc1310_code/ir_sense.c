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

void emmitter_on(int emmitter_num)
{
    SetAndWritePinHigh(adc_outputs[emmitter_num]);
    delay(200 * 10e-6);
}

uint32_t temp_vals[NUM_SENSORS];
uint32_t sensor_flags[NUM_SENSORS];
//void ReadIR(uint32_t * vals)
//{
//    //initialize all values to zero
//    memset(vals, 0, NUM_SENSORS * 4);
//    memset(temp_vals, 0, NUM_SENSORS * 4);
//    memset(sensor_flags, 0, NUM_SENSORS * 4);
//
//    int i, j;
//    //possibly turn 1 by 1
//    // to see if we can differentiate colors better (based on some weird cross reflection stuff)
//
//    emmitters_on();
//    SetTimerState(TIMER_ON);
//    uint32_t startTime = GetTime();
////    sprintf(buffer, "start time: %u\r\n", startTime);
////    WriteUART0(buffer);
//    uint32_t IRTime = 0;
//    while (IRTime < MAX_SAMPLE_TIME)
//    {
//        //read the voltage value
//        ReadADC(temp_vals);
//        for (j = 0; j < NUM_SAMPLES; j++)
//        {
//            for (i = 0; i < NUM_SENSORS; i++)
//            {
//                IRTime = ticsToMicrosec(GetTime() - startTime);
//                temp_vals[i] += IRTime / NUM_SAMPLES;
//    //            vals[i] += temp_vals[i] / NUM_SAMPLES; //should divide each value by 4
//            }
//        }
//
//
//        for(i = 0; i < NUM_SENSORS; i++)
//        {
//            //if voltage value l
//            if ((temp_vals[i] < 2400) && !(sensor_flags[i]))
//            {
//                vals[i] = IRTime;
//                sensor_flags[i] = 1;
//            }
//        }
//    }
//
//    for (i = 0; i < NUM_SENSORS; i++)
//    {
//        if ((vals[i] == 0) || (vals[i] > MAX_SAMPLE_TIME))
//        {
//            vals[i] = MAX_SAMPLE_TIME;
//        }
//    }
//
//    SetTimerState(TIMER_OFF);
//}


//temp_vals[] hold voltage values
//vals[] holds IRTime values

void ReadIR(uint32_t * vals)
{
    //initialize all values to zero
    memset(vals, 0, NUM_SENSORS * 4); //4 is just sizeof(uint32_t)
    memset(temp_vals, 0, NUM_SENSORS * 4);
    memset(sensor_flags, 0, NUM_SENSORS * 4);

    int i, j;
    //possibly turn 1 by 1
    // to see if we can differentiate colors better (based on some weird cross reflection stuff)
    uint32_t startTime;
    uint32_t IRTime;
    char sensor_flag;
    SetTimerState(TIMER_ON);
    for (i = 0; i < NUM_SAMPLES; i++){
    for (j = 0; j < NUM_SENSORS; j++)
    {
        emmitter_on(j);
        startTime = GetTime();
        IRTime = 0;
        sensor_flag = 0;
        while (IRTime < MAX_SAMPLE_TIME && !sensor_flag)
        {
            temp_vals[j]= Read1ADC(j);
            IRTime = ticsToMicrosec(GetTime() - startTime);

            //if voltage value l
            if ((temp_vals[j] < 2400)) // && !(sensor_flags[j]))
            {
                vals[j] += IRTime / (float) NUM_SAMPLES;
                sensor_flag = 1;
            }
        }

        if ((vals[j] == 0) || (vals[j] > MAX_SAMPLE_TIME))
        {
            vals[j] = MAX_SAMPLE_TIME;
        }
    }
    }
    SetTimerState(TIMER_OFF);
    return;
}

