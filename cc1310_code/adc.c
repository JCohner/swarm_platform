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
    AONWUCAuxWakeupEvent(AONWUC_AUX_WAKEUP);
    while(!(AONWUCPowerStatusGet() & AONWUC_AUX_POWER_ON));

    AUXWUCPowerCtrl(AUX_WUC_POWER_ACTIVE);
    //eneable interface and adc clock
    AUXWUCClockEnable(AUX_WUC_ADC_CLOCK | AUX_WUC_ANAIF_CLOCK | AUX_WUC_ADI_CLOCK);
    //wait for request ack
    while (AUXWUCClockStatus(AUX_WUC_ADC_CLOCK) != AUX_WUC_CLOCK_READY);

    AUXADCSelectInput(ADC_COMPB_IN_AUXIO7); //CORRESPONDS TO DIO PIN 23
    WriteUART0("sizzurp\r\n");
    //set input pins
//    IOCPinTypeAux(IOID_25); //TODO: figure out how to map pins better
    AUXADCDisable();
    WriteUART0("rock chalk?\r\n");
    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US, AUXADC_TRIGGER_MANUAL);
    AUXADCGenManualTrigger();
    WriteUART0("killed jayhawk?\r\n");
    uint16_t sing_samp = AUXADCReadFifo();
    sprintf(buffer, "adc val: %u\r\n", sing_samp);
    WriteUART0(buffer);
//    //configure ADC registers
    AUXADCDisable();
//    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_5P3_US, AUXADC_TRIGGER_MANUAL);
    WriteUART0("am i hancocking here?\r\n");
}

uint32_t ReadADC()
{
    AUXADCDisable();
    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US, AUXADC_TRIGGER_MANUAL);
    AUXADCGenManualTrigger();
    uint16_t sing_samp = AUXADCReadFifo();
    AUXADCDisable();
    return AUXADCReadFifo();
}
