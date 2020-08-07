/*
 * leds.c
 *
 *  Created on: Jul 24, 2020
 *      Author: jambox
 */

#include "leds.h"
//for debug
#include "uart.h"
#include <stdio.h>
static uint8_t info_leds[4] = {IOID_6, IOID_7, IOID_15, IOID_28};

void setup_leds()
{
    int i;
    for (i = 0; i < 5; i++)
    {
        SetGPIOPin(info_leds[i]);
    }
}

//static char buffer[50];
//void manage_leds()
//{
//
//    uint8_t info = get_target_flag() << 2 | get_policy();
//    int i;
//    for (i = 0; i < NUM_INFO_LEDS; i++)
//    {
//        GPIO_writeDio(info_leds[i], (info & (0b1 << i)) >> i);
//    }
//    return;
//}



