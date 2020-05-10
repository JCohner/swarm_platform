/*
 * adc.c
 *
 *  Created on: Apr 9, 2020
 *      Author: jambox
 */
#include "adc.h"
#include "uart.h"
#include "helpful.h"


char buffer[50];

void InitADC()
{
    AONWUCAuxWakeupEvent(AONWUC_AUX_WAKEUP);
    while(!(AONWUCPowerStatusGet() & AONWUC_AUX_POWER_ON));

    AUXWUCPowerCtrl(AUX_WUC_POWER_ACTIVE);
    //eneable interface and adc clock
//    AUXWUCClockEnable(AUX_WUC_ADC_CLOCK | AUX_WUC_ANAIF_CLOCK | AUX_WUC_ADI_CLOCK);
//    //wait for request ack
//    while (AUXWUCClockStatus(AUX_WUC_ADC_CLOCK) != AUX_WUC_CLOCK_READY);
//
//    AUXADCSelectInput(ADC_COMPB_IN_AUXIO7); //CORRESPONDS TO DIO PIN 23
//    WriteUART0("sizzurp\r\n");
//    //set input pins
////    IOCPinTypeAux(IOID_25); //TODO: figure out how to map pins better
//    AUXADCDisable();
////    WriteUART0("rock chalk?\r\n");
//    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US, AUXADC_TRIGGER_MANUAL);
//    AUXADCGenManualTrigger();
////    WriteUART0("killed jayhawk?\r\n");
//    uint16_t sing_samp = AUXADCReadFifo();
//    sprintf(buffer, "adc val: %u\r\n", sing_samp);
//    WriteUART0(buffer);
////    //configure ADC registers
//    AUXADCDisable();
//    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_5P3_US, AUXADC_TRIGGER_MANUAL);
}

//while this is generically named its function is to know read the refelectance sensor array from pololu
void ReadADC(uint32_t * vals)
{
    //eneable interface and adc clock
    AUXWUCClockEnable(AUX_WUC_ADC_CLOCK | AUX_WUC_ANAIF_CLOCK | AUX_WUC_ADI_CLOCK);
    while (AUXWUCClockStatus(AUX_WUC_ADC_CLOCK) != AUX_WUC_CLOCK_READY);
//    AUXADCSelectInput(ADC_COMPB_IN_AUXIO7);
    AUXADCDisable();
    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US, AUXADC_TRIGGER_MANUAL);
    int i;
    for (i = 0; i < 4; i++)
    {
//        SetAndWritePinHigh(adc_outputs[i]);
        IOCPinTypeAux(adc_outputs[i]); //tri-state the pin to switch it off
//        delay(0.000003);
        AUXADCSelectInput(adc_inputs[i]);
        AUXADCGenManualTrigger();
        vals[i]= AUXADCReadFifo();
    }
    AUXADCDisable();
    return;
}

