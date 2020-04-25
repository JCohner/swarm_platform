/*
 * adc.h
 *
 *  Created on: Apr 9, 2020
 *      Author: jambox
 */

#ifndef ADC_H_
#define ADC_H_

#include <devices/cc13x0/driverlib/aux_wuc.h>
#include <devices/cc13x0/driverlib/aux_adc.h>
#include <devices/cc13x0/driverlib/ioc.h>


static char adc_inputs[8] = {ADC_COMPB_IN_AUXIO7, ADC_COMPB_IN_AUXIO6,ADC_COMPB_IN_AUXIO5, ADC_COMPB_IN_AUXIO4, ADC_COMPB_IN_AUXIO3, ADC_COMPB_IN_AUXIO2, ADC_COMPB_IN_AUXIO1, ADC_COMPB_IN_AUXIO0};
void InitADC();
void ReadADC(uint32_t * vals);

#endif /* ADC_H_ */
