/*
 * state_track.c
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#include "state_track.h"

#include "uart.h"
#include <stdio.h>

//static enum States state;

static struct StateTrack state_track =
                                {.xc_state = 0b1100, .prev_xc_state=0b1100,
                                 .actuation_flag = 0,
                                 .bbs = {3,4}, .bb = 4, .bb_idx = 1,
                                 .ret = 0, .mask = 0x7};


uint8_t reverse_bits(uint8_t bits_in, uint8_t len)
{
    uint8_t bits_out = 0;
    int i;
    for (i = len; i > 0; i--)
    {
        bits_out = bits_out | ((bits_in & (0b1 << (i - 1))) >> (i -1))  << (len - i) ;
    }
    return bits_out;

}

uint8_t mask_mask(uint8_t bb)
{

    uint8_t mask_mask = 0;
    //avoiding using bitwise not due to alignment issues
    int i;
    for (i = bb - 1; i >= 0; i--)
    {
        mask_mask |= 0b1 << i;
    }

    return mask_mask;
}


uint8_t get_mask_mask(uint8_t bb)
{
    //mask mask takes care of alignment issues for bb = 3
    uint8_t mask = ~(((0b1 << (bb - 1)))) & mask_mask(bb);
    return mask;
}

/*!
 *  \brief examines flags, determines if state transition is needed
 *
 *  If the actuation flag is high the state will not be evaluated
 *  If not actuating, will set the current xc_state to the previous
 *  If the detect flag is high a new poi has been detected, will increment the
 *      xc_state variable that tracks which intersection the robot believes it is at
 *
 */
static char buffer[50];
//after an intersection has been dealt with we can increment the state
void inc_state()
{
    uint8_t xcs = state_track.xc_state;
    uint8_t prev_xcs = state_track.prev_xc_state;
    uint8_t ret = state_track.ret;

    //ensures we actually detect transition
    if (get_intersection_flag() && !get_actuation_flag() && get_detect_flag() && get_on_line_flag())
    {
        //OHE encoding of all but MSB
        xcs = (xcs & state_track.mask) << 1;
        //Wrap all but MSB
        xcs = xcs % state_track.mask;
        //Add MSB back in to reflect if one return path
        xcs = xcs | ret << (state_track.bb - 1);
        //Tracks bit before MSB, this XORd with current MSB (ie ret) allows correct trans
        ret = (((xcs & (0b1 << (state_track.bb - 2)))) >> (state_track.bb - 2)) ^ ret;


        sprintf(buffer, "xcs: %X, prev: %X, bb: %X, ret: %X\r\n",
                xcs, prev_xcs, state_track.bb, state_track.ret);
        WriteUART0(buffer);
        //Deals with switching between branches of different bb
        if (xcs < prev_xcs)
        {
            state_track.bb_idx = (state_track.bb_idx + 1) % NUM_BRANCHES;
            state_track.bb = state_track.bbs[state_track.bb_idx];
            state_track.mask = get_mask_mask(state_track.bb);
        }

        if(prev_xcs == 0b0010 && get_bb_idx() && ((get_policy() & 0b11) == 0b11))
        {
            //fast forward for path that has lack of
            xcs = 0x9;
            ret = 1; //this may break;
        }


        state_track.xc_state = xcs;
        state_track.ret = ret;
//        state_track.prev_xc_state = xcs;

        //set_intersection_flag(0); //going to set this low when managed by manage_intersection()
        set_detect_flag(0);
        set_on_line_flag(0);
    }
}

void set_policy(uint8_t policy)
{
    state_track.policy = policy;
    //TODO: make implementation that just accesses bb params of state
    uint8_t ret_pol = 0;
    ret_pol = ret_pol | reverse_bits((policy & 0x1C) >> 2, 3)  << 2;
    ret_pol = ret_pol | reverse_bits(policy & 0x03, 2);
    state_track.return_policy = ret_pol;
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
    state_track.xc_state = state;
}


uint8_t get_xc_state()
{
    return state_track.xc_state;
}

void set_prev_xc_state(uint8_t state)
{
    state_track.prev_xc_state = state;
}

uint8_t get_prev_xc_state()
{
    return state_track.prev_xc_state;
}

void set_target_flag(uint8_t target)
{
    state_track.target = target & 0x01;
}

uint8_t get_target_flag()
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

bool get_return_flag()
{
    return (state_track.xc_state & (0b1 << (state_track.bb - 1)));
}

void set_detect_flag(uint8_t flag)
{
    state_track.detect_flag = flag;
}

uint8_t get_detect_flag()
{
    return state_track.detect_flag;
}

void set_intersection_flag(uint8_t flag)
{
    state_track.intersection_flag = flag;
}
uint8_t get_intersection_flag()
{
    return state_track.intersection_flag;
}

void set_on_line_flag(uint8_t flag)
{
    state_track.on_line_flag = flag;
}
uint8_t get_on_line_flag()
{
    return state_track.on_line_flag;
}

void set_neighbor_target_policy(uint8_t policy)
{
    state_track.neighbor_target_policy = policy;
}

uint8_t get_neighbor_target_policy()
{
    return state_track.neighbor_target_policy;
}

uint8_t get_neighbor_target_flag()
{
    return state_track.neighbor_target_flag;
}
void set_neighbor_target_flag(uint8_t flag)
{
    state_track.neighbor_target_flag = flag;
}

uint8_t get_mask()
{
    return state_track.mask;
}

uint8_t get_bb_idx()
{
    return state_track.bb_idx;
}



