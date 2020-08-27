/*
 * uart.c
 *
 *  Created on: Feb 16, 2020
 *      Author: jambox
 */
#include "uart.h"
#include <stdio.h>
#include <string.h>

void InitUART0()
{

//    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
//    PRCMLoadSet();
//    while ( !PRCMLoadGet() );

    PRCMPeripheralRunEnable(PRCM_PERIPH_UART0);
//    PRCMLoadSet();
//    while ( !PRCMLoadGet() );
//
//    PRCMPeripheralSleepEnable(PRCM_PERIPH_UART0);
////    PRCMLoadSet();
////    while ( !PRCMLoadGet() );
//
//    PRCMPeripheralDeepSleepEnable(PRCM_PERIPH_UART0);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    IOCIOPortIdSet(IOID_3, IOC_PORT_MCU_UART0_TX);
    IOCIOPortIdSet(IOID_2,IOC_PORT_MCU_UART0_RX);
    IOCPortConfigureSet(IOID_3, IOC_PORT_MCU_UART0_TX,IOC_IOMODE_NORMAL); //set TX
    IOCPortConfigureSet(IOID_2, IOC_PORT_MCU_UART0_RX, IOC_IOMODE_NORMAL);
    IOCPinTypeUart(UART0_BASE, IOID_2, IOID_3, IOID_UNUSED, IOID_UNUSED);

    // Initialize the UART. Set the baud rate, number of data bits, turn off
    // parity, number of stop bits, and stick mode.
    //
    UARTConfigSetExpClk(UART0_BASE, SysCtrlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
    //
    // Enable the UART.
    //
    UARTEnable(UART0_BASE);
}

void WriteUART0(const char * string)
{
    //while there are still chars left to send
    while (*string != '\0')
    {
//        wait for space to be available to tx
//        while (UARTSpaceAvail(UART0_BASE)){
//            ;
//        }
        UARTCharPut(UART0_BASE, *string);
        ++string;
    }
}

void ReadUART0(char * message, int maxLength)
{
    char data = 0;
    int complete = 0, num_bytes = 0;
    //loop until \r or \n
    while (!complete)
    {
        while(UARTCharsAvail(UART0_BASE))
        {
            data = UARTCharGetNonBlocking(UART0_BASE);
            if ((data == '\n') || (data == '\r'))
            {
                complete = 1;
            } else {
                message[num_bytes] = data;
                ++num_bytes;
                if (num_bytes >= maxLength){
                    num_bytes = 0;
                }
            }
        }
    }
    message[num_bytes] = '\0';
}




static uint16_t message_idx;
static uint8_t message_complete;
void READUART0_AS_ENABLE()
{
    message_idx = 0;
    message_complete = 0;
}

uint8_t ReadUART0_AS(char * message, int maxLength)
{
    if (message_complete)
    {
        message_complete = 0;
        message_idx = 0;
        memset(message, 0, maxLength);
    }

    char data = 0;
    while(UARTCharsAvail(UART0_BASE))
    {
        data = UARTCharGetNonBlocking(UART0_BASE);
        if ((data == '\n') || (data == '\r'))
        {
            message_complete = 1;
        } else {
            message[message_idx] = data;
            ++message_idx;
            if (message_idx >= maxLength){
                message_idx = 0;
            }
        }
    }

    if (message_complete)
    {
//        WriteUART0(message);
//        WriteUART0("\r\n");
    }
    return message_complete;
}

