/*
 * state_track.c
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#include "state_track.h"

#include "uart.h"
#include <stdio.h>

static enum States state;

static struct StateTrack state_track =
                                {.xc_state = for_1, .prev_xc_state=for_1,
                                 .actuation_flag = 0};


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
//    uint8_t stator = state_track.xc_state;

//    sprintf(buffer, "stator: %u\r\n",stator);
//    WriteUART0(buffer);

    if (get_intersection_flag() && !get_actuation_flag() && get_detect_flag() && get_on_line_flag())
    {
        switch(state_track.xc_state)
        {
            case for_1: //2
                state_track.xc_state = ret_0;
                break;
            case ret_0: //5
                state_track.xc_state = ret_1;
                break;
            case ret_1: //6
                state_track.xc_state = for_0;

                //noting that a loop is completed now update policy based on neighbor feedback
                if (get_neighbor_target_flag())
                {
                    set_policy(get_neighbor_target_policy());
                }

                break;
            case for_0:
                state_track.xc_state = for_1;
                break;
        }

        //set_intersection_flag(0); //going to set this low when managed by manage_intersection()
        set_detect_flag(0);
        set_on_line_flag(0);
    }
}

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

//void inc_xc_state()
//{
//    state_track.xc_state = (state_track.xc_state ^ 0b11) &0x03;
//
//    GPIO_toggleDio(BLED2);
//    if (state_track.xc_state == 0b01)
//    {
//        toggle_return_flag();
////        GPIO_toggleDio(BLED2);
//    }
//}


void set_xc_state(uint8_t state)
{
    state_track.xc_state = state & 0x07;
}


uint8_t get_xc_state()
{
    return state_track.xc_state;
}

void set_prev_xc_state(uint8_t state)
{
    state_track.prev_xc_state = state & 0x07;
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

//void set_return_flag(uint8_t flag)
//{
//    state_track.return_flag = flag;
//}
//
bool get_return_flag()
{
    return (state_track.xc_state & 0b100);
}
//
//uint8_t get_prev_return_flag()
//{
//    return state_track.prev_return_flag;
//}
//
//void set_prev_return_flag(uint8_t flag)
//{
//    state_track.prev_return_flag = flag;
//}

//void toggle_return_flag()
//{
//    state_track.prev_return_flag = state_track.return_flag;
//    state_track.return_flag = !state_track.return_flag;
//}

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
    state_track.neighbor_target_policy = policy & 0x3;
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

