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
#include "state_track.h"

void setMotor(int motor, int dir, int value);
void init_openloop(void);
void end_openloop(void);
void set_total_count(uint32_t tot_count);
void set_offset(uint32_t offset);
void openloop_turn();
void execute_policy(void);
void rotate(int dir);
#endif /* ZUMO_MOVES_H_ */
