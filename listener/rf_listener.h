/*
 * zumo_rf.h
 *
 *  Created on: Mar 2, 2020
 *      Author: jambox
 */

#ifndef RF_LISTENER_H_
#define RF_LISTENER_H_

void rf_setup();
void rf_main();

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

#endif /* RF_LISTENER_H_ */
