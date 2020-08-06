/*
 * uart.h
 *
 *  Created on: Feb 16, 2020
 *      Author: jambox
 */

#ifndef UART_H_
#define UART_H_

#include <devices/cc13x0/driverlib/uart.h>
#include <devices/cc13x0/inc/hw_memmap.h>
#include <devices/cc13x0/driverlib/sys_ctrl.h>
#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/ioc.h>
#include <devices/cc13x0/inc/hw_types.h>
#include <devices/cc13x0/inc/hw_gpio.h>
#include <devices/cc13x0/driverlib/ioc.h>

#include <stdio.h>

void InitUART0();
void WriteUART0(const char * string);
void ReadUART0(char * message, int maxLength);
uint8_t ReadUART0_AS(char * message, int maxLength);
void READUART0_AS_ENABLE();
#endif /* UART_H_ */
