/*
 * zumo_moves.h
 *
 *  Created on: Jun 21, 2020
 *      Author: jambox
 */

#ifndef ZUMO_MOVES_H_
#define ZUMO_MOVES_H_

#include <stdint.h>
//#include "zumo.h"
#include "uart.h"
#include "helpful.h"
#include "pwm.h"

void setMotor(int motor, int dir, int value);
void init_openloop(void);
void end_openloop(void);
void set_total_count(uint32_t tot_count);
void openloop_turn(uint8_t flag, uint8_t policy);
#endif /* ZUMO_MOVES_H_ */
