/*
 * print_info.c
 *
 *  Created on: Aug 5, 2020
 *      Author: jambox
 */

#include "print_info.h"
#include "uart.h"

static char buffer[50];
void print_info(uint32_t data)
{
    uint16_t mach_id = (data & MACH_MASK) >> MACH_SHIFT; //10-15th bits is mach ID
    uint8_t target_flag = (data & TFLAG_MASK) >> TFLAG_SHIFT; //9th bit is target flag
    uint8_t policy = (data & POLICY_MASK) >> POL_SHIFT; //4-8 bit is poloicy
    uint8_t bb_idx = (data & BBI_MASK) >> BBI_SHIFT;
    uint8_t xc_state = data & STATE_MASK; //0-3 bit is state


    //        sprintf(buffer, "%u\r\n", * (packetDataPointer + 2));
//    sprintf(buffer, "mach: %x\ttarg: %u\tpol: %u\tbbi: %u\tstate: %X\r\n",
//            mach_id, target_flag, policy, bb_idx, xc_state);
    sprintf(buffer, "%X,%u,%u,%u,%u\r\n",
            mach_id, target_flag, policy, bb_idx, xc_state);

    WriteUART0(buffer);
}
