/*
 * adc.c
 *
 *  Created on: Apr 9, 2020
 *      Author: jambox
 */
#include "adc.h"
#include "uart.h"
#include "helpful.h"

//#define NUM_SENSORS 6


char buffer[50];

void InitADC()
{
    AONWUCAuxWakeupEvent(AONWUC_AUX_WAKEUP);
    while(!(AONWUCPowerStatusGet() & AONWUC_AUX_POWER_ON));

    AUXWUCPowerCtrl(AUX_WUC_POWER_ACTIVE);

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
    for (i = 0; i < NUM_SENSORS; i++)
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

uint32_t Read1ADC(int pin)
{
    AUXWUCClockEnable(AUX_WUC_ADC_CLOCK | AUX_WUC_ANAIF_CLOCK | AUX_WUC_ADI_CLOCK);
    while (AUXWUCClockStatus(AUX_WUC_ADC_CLOCK) != AUX_WUC_CLOCK_READY);
    AUXADCDisable();
    AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US, AUXADC_TRIGGER_MANUAL);

    int i = pin;
    IOCPinTypeAux(adc_outputs[i]); //tri-state the pin to switch it off
//        delay(0.000003);
    AUXADCSelectInput(adc_inputs[i]);
    AUXADCGenManualTrigger();
    return AUXADCReadFifo();
}
