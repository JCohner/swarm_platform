/*
 * adc.c
 *
 *  Created on: Apr 9, 2020
 *      Author: jambox
 */
#include "adc.h"
#include "uart.h"

char buffer[50];

void InitADC()
{
    AUXWUCPowerCtrl(AUX_WUC_POWER_ACTIVE);
    //eneable interface and adc clock
    AUXWUCClockEnable(AUX_WUC_ADC_CLOCK);
    //wait for request ack
    while (AUXWUCClockStatus(AUX_WUC_ADC_CLOCK) != AUX_WUC_CLOCK_READY);

    AUXADCSelectInput(ADC_COMPB_IN_AUXIO2);
    //set input pins
    IOCPinTypeAux(IOID_25); //TODO: figure out how to map pins better


    //TODO HAVAE TO MAKE ENABLE CALL BEFORE I DISABLE
    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_5P3_US, AUXADC_TRIGGER_MANUAL);
    AUXADCGenManualTrigger();
    AUXADCReadFifo();
    //configure ADC registers
    AUXADCDisable();
//    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_5P3_US, AUXADC_TRIGGER_MANUAL);
    WriteUART0("am i hancocking here?\r\n");
}

uint32_t ReadADC()
{
    AUXADCGenManualTrigger();
    return AUXADCReadFifo();
}
