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

    setMotor(M1, dir, MOTOR_ON);
    setMotor(M2, !dir, MOTOR_ON);
}


void openloop_turn()
{
    uint8_t flag = get_flags();
    uint8_t policy= get_policy();
    uint8_t ret_policy = get_return_policy();

    uint8_t dir;
    if (!get_return_flag())
    {
        dir = flag & policy;
    }
    else
    {
        dir = flag & ret_policy;
    }

    sprintf(buffer, "state: %u\r\n", state);
    WriteUART0(buffer);
    sprintf(buffer, "count: %u\r\n", counter);
    WriteUART0(buffer);
    if (state && counter < total_count + timer_offset)
    {
        if (counter > timer_offset)
        {
            rotate(dir);
        }
        counter++;
    }
    else if (counter >= total_count + timer_offset && state)
    {
        end_openloop();
        GPIO_toggleDio(BLED0);
        setMotor(M1, dir, MOTOR_OFF);
        setMotor(M2, !dir, MOTOR_OFF);

        set_actuation_flag(0);
        state = 0;
    }

    return;
}

void execute_policy()
{
    uint8_t flags = get_flags();
    uint8_t policy = get_policy();
    uint8_t prev_flags =  get_prev_flags();

    if (flags == DETECT_0 && (prev_flags == NO_DETECT || prev_flags == DETECT_1) && !get_actuation_flag())
    {
        init_openloop();
        GPIO_toggleDio(BLED0);
    }
    else if (flags == DETECT_1 && prev_flags == DETECT_0 && !get_actuation_flag())
    {
        init_openloop();
        GPIO_toggleDio(BLED2);
    }
}


