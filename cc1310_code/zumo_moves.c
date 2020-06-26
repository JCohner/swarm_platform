/*
 * zumo_moves.c
 *
 *  Created on: Jun 21, 2020
 *      Author: jambox
 */
#include "zumo_moves.h"
#include "gpio.h"
#include "uart.h"

static uint32_t total_count = 0;
static uint32_t counter = 0;
static uint8_t state = 0;
static uint16_t timer_offset = 0;

void setMotor(int motor, int dir, int value)
{
    int DIR_pin, PWM_pin;
    if (motor == M1)
    {
       DIR_pin = M1_DIR;
       PWM_pin = M1_PWM;
    }
    else if (motor == M2)
    {
        DIR_pin = M2_DIR;
        PWM_pin = M2_PWM;
    }

    int set_val = 1022 - value;
    if (set_val < 0){
        set_val = 0;
    }

//    if (value < 0)
//    {
//        dir = !dir;
//    }

    GPIO_writeDio(DIR_pin, dir);
    PWMSet(PWM_pin, set_val);
}



void init_openloop(void)
{
    state = 1;
    counter = 0;
    set_actuation_flag(1);
}

void end_openloop(void)
{
    GPIO_toggleDio(BLED0);
    state = 0;
}

void set_total_count(uint32_t tot_count)
{
    total_count = tot_count;
}


void set_offset(uint32_t offset)
{
    timer_offset = offset;
}

char buffer[50];

//0 - CCW (left)
//1 - CW (right)s
void rotate(int dir)
{
    //ensures all non zero inputs normalized to 1
    if (dir)
    {
        dir = dir/dir;
    }

    setMotor(M1, dir, MOTOR_TURN);
    setMotor(M2, !dir, MOTOR_TURN);
}

/*!
 *  \brief Called once per main loop, if state is active will take over controlling
 *      motors and open loop turn the robot. Uses state as a mask to get policy bit OR
 *      return_policy bit relevant to current intersection
 */

void openloop_turn()
{

//    sprintf(buffer, "tot count: %u, offset: %u\r\n", total_count, timer_offset);
//    WriteUART0(buffer);
//    while(1);

    uint8_t xc_state = get_xc_state();
    uint8_t policy= get_policy();
    uint8_t ret_policy = get_return_policy();

    uint8_t dir;
    if (!get_return_flag())
    {
        dir = xc_state & policy;
    }
    else
    {
        dir = xc_state & ret_policy;
    }

    if (state && counter < (total_count + timer_offset))
    {
        if (counter > timer_offset)
        {
            WriteUART0("ACTUATING\r\n");
            rotate(dir);
        }
        counter++;
        sprintf(buffer, "counter %u\r\n", counter);
        WriteUART0(buffer);
    }
    else if (counter >= total_count + timer_offset && state)
    {

//        GPIO_toggleDio(BLED0);
        setMotor(M1, dir, MOTOR_OFF);
        setMotor(M2, !dir, MOTOR_OFF);
        end_openloop();
        set_actuation_flag(0);

    }

    return;
}

/*!
 *  \brief calls flag getters from StateTrack, calls open loop controller to turn
 *      if state change detected
 *
 */
void execute_policy()
{
    uint8_t xc_state = get_xc_state();
    uint8_t prev_xc_state=  get_prev_xc_state();

    uint8_t ret_flag = get_return_flag();
//    uint8_t prev_ret_flag = get_prev_return_flag();

    uint8_t actuation_flag = get_actuation_flag();

    sprintf(buffer, "xc: %u, %u \t ret %u\r\n", xc_state, prev_xc_state, ret_flag);
    WriteUART0(buffer);
    if (xc_state != prev_xc_state && !actuation_flag)
    {
//        GPIO_toggleDio(BLED0);
        init_openloop();
    }

//    update_prev_return_flag();

    return;
}


