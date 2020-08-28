/*
 * rando.c
 *
 *  Created on: Aug 18, 2020
 *      Author: jambox
 */

#include "rando.h"
#include "uart.h"

void RandoConfig()
{
    PRCMPeripheralRunEnable(PRCM_PERIPH_TRNG);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    TRNGConfigure(197, 4194304, 4);
    TRNGEnable();
}
char buffer[50];
uint32_t get_random_num(uint8_t num)
{


    while(!TRNGStatusGet())
    {
//        sprintf(buffer, "suh: %u\r\n",TRNGStatusGet());
//        WriteUART0(buffer);
    }
//    WriteUART0("gut num\r\n");
    uint32_t rando = TRNGNumberGet(TRNG_HI_WORD);
//    TRNGDisable();
    return rando % num;

}

