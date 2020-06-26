/*
 * state_track.c
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#include "state_track.h"

#include "uart.h"
#include <stdio.h>

static struct StateTrack state_track =
                                {.xc_state = 0b10, .prev_xc_state=0b10,
                                 .return_flag = 0b0, .prev_return_flag = 0b0,
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
void evaluate_state()
{
    //if we are currently actuating, do not overwite prev flag and do not set a new flag
    sprintf(buffer, "xc state: %u\r\nactuation flag: %u\r\n",
            state_track.xc_state, state_track.actuation_flag);
    WriteUART0(buffer);

    if (!state_track.actuation_flag){
        set_prev_xc_state(state_track.xc_state);
    }
    else
    {
        set_detect_flag(0);
    }

    if(state_track.detect_flag)
    {
        inc_xc_state();
//        GPIO_toggleDio(BLED2);
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

void inc_xc_state()
{
    state_track.xc_state = (state_track.xc_state ^ 0b11) &0x03;

    GPIO_toggleDio(BLED2);
    if (state_track.xc_state == 0b01)
    {
        toggle_return_flag();
//        GPIO_toggleDio(BLED2);
    }
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

uint8_t get_prev_return_flag()
{
    return state_track.prev_return_flag;
}

void toggle_return_flag()
{
    state_track.prev_return_flag = state_track.return_flag;
    state_track.return_flag = !state_track.return_flag;
}

void update_prev_return_flag()
{
    state_track.prev_return_flag = state_track.return_flag;
}

void set_detect_flag(uint8_t flag)
{
    state_track.detect_flag = flag;
}

uint8_t get_detect_flag()
{
    return state_track.detect_flag;
}





