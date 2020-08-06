/*
 * print_info.h
 *
 *  Created on: Aug 5, 2020
 *      Author: jambox
 */

#ifndef PRINT_INFO_H_
#define PRINT_INFO_H_

#include <stdint.h>

#define MACH_MASK 0x1F800
#define POLICY_MASK 0x003E0
#define TFLAG_MASK 0x00400
#define BBI_MASK 0x000010
#define STATE_MASK 0x00000F

#define MACH_SHIFT 11
#define POL_SHIFT 5
#define TFLAG_SHIFT 10
#define BBI_SHIFT 4
#define STATE_SHIFT 0


void print_info(uint32_t data);


#endif /* PRINT_INFO_H_ */
