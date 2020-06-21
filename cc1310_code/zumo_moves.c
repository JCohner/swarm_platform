/*
 * zumo_moves.c
 *
 *  Created on: Jun 21, 2020
 *      Author: jambox
 */
#include "zumo_moves.h"

static uint16_t total_count = 0;
static uint16_t counter = 0;
static uint8_t state = 0;

void begin_openloop(void)
{
    state = 1;
    counter = 0;
}

void end_openloop(void)
{
    state = 0;
}

void set_total_count(uint16_t tot_count)
{
    total_count = tot_count;
}

void openloop_turn(uint8_t dir)
{
    if (state && counter < total_count)
    {
        setMotor(M1, dir, MOTOR_ON * 0.5);
        setMotor(M2, !dir, MOTOR_ON * 0.5);
        counter++;
    }
    else if (counter >= total_count)
    {
        end_openloop();
    }
    return;
}

