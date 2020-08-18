/*
 * radno.h
 *
 *  Created on: Aug 18, 2020
 *      Author: jambox
 */

#ifndef RANDO_H_
#define RANDO_H_
#include <devices/cc13x0/driverlib/trng.h>
#include <stdint.h>
#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/sys_ctrl.h>
void RandoConfig();
uint32_t get_random_num(uint8_t num);



#endif /* RANDO_H_ */
