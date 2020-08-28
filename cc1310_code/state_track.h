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

//enum States{ret_0=0b101, ret_1=0b110, for_0=0b001, for_1=0b010};


#define MAX_NUM_BRANCHES 2
struct StateTrack{
    //policy and target bits
    uint8_t policy : 5;
    uint8_t return_policy : 5;
    uint8_t target : 1;

    //state management bits
    uint8_t bb_idx : 2;
    uint8_t bb : 3;
    uint8_t bbs[MAX_NUM_BRANCHES];
    uint8_t ret : 1;
    uint8_t mask;
    uint8_t NUM_BRANCHES : 4;


    uint8_t xc_state : 4;
    uint8_t prev_xc_state : 4;

    uint8_t detect_flag : 1; //raised when grey detected
    uint8_t intersection_flag : 1; //rasied when white intersection detected

    uint8_t neighbor_target_policy : 5; //this is the policy heard from another bot that has found target
    uint8_t neighbor_target_flag : 1; //flag that indicates another bot found target

    uint8_t new_policy : 5; //if we want to load a new policy
    uint8_t new_policy_flag : 1;//flag indicates new pol received

    uint8_t on_line_flag : 1; // raised if robot on line
    uint8_t actuation_flag : 1; //raised if: xc_state != prev_xc_state || ret_flag != prev_ret_flag
                                   // in execute_policy() in zumo_moves

    uint8_t actuation_pre_ret_flag : 1;
    uint8_t num_loops;
    uint8_t ignore_pol_flag : 1;

    uint8_t dist_flag : 1;
    uint8_t enabled: 1;
};

void init_state(uint8_t xc_state, uint8_t NUM_BRANCHES, uint8_t * branch_bit_shifts, uint8_t curr_branch_bit,
                uint8_t curr_branch_bit_idx, uint8_t curr_return_flag, uint8_t curr_mask);

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
bool get_return_flag();
void set_prev_return_flag(uint8_t flag);
uint8_t get_prev_return_flag();
void toggle_return_flag();


void set_detect_flag(uint8_t flag);
uint8_t get_detect_flag();

void set_intersection_flag(uint8_t flag);
uint8_t get_intersection_flag();

void set_on_line_flag(uint8_t flag);
uint8_t get_on_line_flag();

void set_neighbor_target_policy(uint8_t policy);
uint8_t get_neighbor_target_policy();

uint8_t get_neighbor_target_flag();
void set_neighbor_target_flag(uint8_t flag);

void set_new_policy(uint8_t policy);
uint8_t get_new_policy();

uint8_t get_new_policy_flag();
void set_new_policy_flag(uint8_t flag);

void set_enable_flag(uint8_t flag);
uint8_t get_enable_flag();

void set_dist_flag(uint8_t flag);
uint8_t get_dist_flag();

void set_num_loops(uint8_t num);
uint8_t get_num_loops();

void set_ignore_pol_flag(uint8_t flag);
uint8_t get_ignore_pol_flag();

void set_actuation_pre_ret_flag(uint8_t flag);
uint8_t get_actuation_pre_ret_flag();

void evaluate_state();
void inc_state();

//gets the mask for the mask, only used internally
uint8_t get_mask_mask(uint8_t bb);
//gives current bit mask for policy to other files
uint8_t get_mask();
uint8_t get_bb_idx();
#endif /* STATE_TRACK_H_ */
