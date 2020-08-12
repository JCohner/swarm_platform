/*
 * print_info.h
 *
 *  Created on: Aug 5, 2020
 *      Author: jambox
 */

#ifndef PRINT_INFO_H_
#define PRINT_INFO_H_

#include <stdint.h>

#define MACH_MASK 0xFFFF0000
#define POLICY_MASK 0x0003E0
#define TFLAG_MASK 0x000400
#define BBI_MASK 0x0000010
#define STATE_MASK 0x000000F

#define MACH_SHIFT 16
#define POL_SHIFT 5
#define TFLAG_SHIFT 10
#define BBI_SHIFT 4
#define STATE_SHIFT 0


void print_info(uint32_t data);


#endif /* PRINT_INFO_H_ */
