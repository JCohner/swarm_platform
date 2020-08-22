/*
 * dist_sense.c
 *
 *  Created on: Jul 23, 2020
 *      Author: jambox
 */

#include "dist_sense.h"
#include "uart.h"
#include <stdio.h>


static char buffer[50];
uint32_t ReadDistForward()
{
    uint32_t analog_sig = Read1ADC(7);

//    sprintf(buffer, "sig: %u\r\n", analog_sig);
//    WriteUART0(buffer);


    return analog_sig;
}

uint32_t ReadDist45()
{
    uint32_t analog_sig = Read1ADC(6);

//    sprintf(buffer, "sig: %u\r\n", analog_sig);
//    WriteUART0(buffer);


    return analog_sig;
}
