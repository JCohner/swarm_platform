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

void openloop_turn();
void execute_policy(void);
void manage_intersection();
void rotate(int dir);

void set_on_time(uint32_t counts);
void set_offset_time(uint32_t counts);
void set_reset_time(uint32_t counts);
void set_counts(uint32_t counts);
#endif /* ZUMO_MOVES_H_ */
