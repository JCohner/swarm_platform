/*
 * state_track.h
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#ifndef STATE_TRACK_H_
#define STATE_TRACK_H_

#include <stdint.h>

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
    uint8_t flags : 2;
    uint8_t prev_flags : 2;
    uint8_t actuation_flag : 1;
    uint8_t return_flag : 1;
//    uint8_t first_send : 1;
};

void set_policy(uint8_t policy);
uint8_t get_policy();
uint8_t get_return_policy();

void set_flags(uint8_t flags);
uint8_t get_flags();

void set_prev_flags(uint8_t flags);
uint8_t get_prev_flags();

void set_target(uint8_t target);
uint8_t get_target();

void set_actuation_flag(uint8_t flag);
uint8_t get_actuation_flag();

void set_return_flag(uint8_t flag);
uint8_t get_return_flag();

//void update_state(struct ColorTrack *left, struct ColorTrack *right);

#endif /* STATE_TRACK_H_ */
