/*
 * adc.c
 *
 *  Created on: Apr 9, 2020
 *      Author: jambox
 */
#include "adc.h"


void InitADC()
{
    //eneable interface and adc clock
    AUXWUCClockEnable(AUX_WUC_ADC_CLOCK);
    //wait for request ack
    while (!AUXWUCClockStatus(AUX_WUC_ADC_CLOCK));

    //set input pins
    IOCPinTypeAux(IOID_25);

    //configure ADC registers
    AUXADCDisable();
    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_5P3_US, AUXADC_TRIGGER_MANUAL);
}

//uint32_t ReadADC()
//{
//    AUXADCGenManualTrigger();
//
//}
