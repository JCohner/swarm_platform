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
}

uint8_t get_policy()
{
    return state_track.policy;
}

void set_flags(uint8_t flags)
{
    state_track.flags = flags & 0x03;
}

uint8_t get_flags()
{
    return state_track.flags;
}

void set_prev_flags(uint8_t flags)
{
    state_track.prev_flags = flags & 0x03;
}

uint8_t get_prev_flags()
{
    return state_track.prev_flags;
}

void set_target(uint8_t target)
{
    state_track.target = target & 0x01;
}

uint8_t get_target()
{
    return state_track.target;
}
