/*
 * state_track.h
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#ifndef STATE_TRACK_H_
#define STATE_TRACK_H_

#include <stdint.h>
#include "gpio.h"
#include "helpful.h"
#define NO_DETECT 0b00
#define DETECT_0 0b01 //first intersection marker on left
#define DETECT_1 0b10 //second intersection marker on right

//extern struct ColorTrack;

struct StateTrack{
    //policy and target bits
    uint8_t policy : 2;
    uint8_t return_policy : 2;
    uint8_t target : 1;

    //intersection management bits
    uint8_t xc_state : 2;
    uint8_t prev_xc_state : 2;

    uint8_t actuation_flag : 1; //raised if: xc_state != prev_xc_state || ret_flag != prev_ret_flag
                                   // in execute_policy() in zumo_moves
    uint8_t return_flag : 1; //whether on return path or forward path
    uint8_t prev_return_flag : 1; //prev state of return such that
                                    //state transition can be handled
    uint8_t detect_flag : 2; //if poi detected flag raised
};

void set_policy(uint8_t policy);
uint8_t get_policy();
uint8_t get_return_policy();

void set_xc_state(uint8_t state);
uint8_t get_xc_state();
void inc_xc_state();

void set_prev_xc_state(uint8_t flags);
uint8_t get_prev_xc_state();

void set_target_flag(uint8_t target);
uint8_t get_target_flag();

void set_actuation_flag(uint8_t flag);
uint8_t get_actuation_flag();

void set_return_flag(uint8_t flag);
uint8_t get_return_flag();
uint8_t get_prev_return_flag();
void toggle_return_flag();
void update_prev_return_flag();


void set_detect_flag(uint8_t flag);
uint8_t get_detect_flag();

void evaluate_state();


#endif /* STATE_TRACK_H_ */
