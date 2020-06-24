/*
 * state_track.c
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#include "state_track.h"

static struct StateTrack state_track;

void set_policy(uint8_t policy)
{
    state_track.policy = policy & 0x03;

    if (!((state_track.policy >> 1) ^ (state_track.policy & 0b01)))
    {
        // 0b00 or 0b11 policy case
        state_track.return_policy = state_track.policy;
    }
    else
    {
        //for 0b01 and 0b10 cases return policy is opposite
        state_track.return_policy = ~state_track.policy;
    }
}

uint8_t get_policy()
{
    return state_track.policy;
}

uint8_t get_return_policy()
{
    return state_track.return_policy;
}

void set_xc_state(uint8_t state)
{
    state_track.xc_state = state & 0x03;
}

uint8_t get_xc_state()
{
    return state_track.xc_state;
}

void set_prev_xc_state(uint8_t state)
{
    state_track.prev_xc_state = state & 0x03;
}

uint8_t get_prev_xc_state()
{
    return state_track.prev_xc_state;
}

void set_target(uint8_t target)
{
    state_track.target = target & 0x01;
}

uint8_t get_target()
{
    return state_track.target;
}

void set_actuation_flag(uint8_t flag)
{
    state_track.actuation_flag = flag;
}
uint8_t get_actuation_flag()
{
    return state_track.actuation_flag;
}

void set_return_flag(uint8_t flag)
{
    state_track.return_flag = flag;
}

uint8_t get_return_flag()
{
    return state_track.return_flag;
}

void toggle_return_flag()
{
    state_track.return_flag = !state_track.return_flag;
}

void set_detect_flag(uint8_t flag)
{
    state_track.detect_flag = flag;
}

uint8_t get_detect_flag()
{
    return state_track.detect_flag;
}





