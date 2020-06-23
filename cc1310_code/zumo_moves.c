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
}

void end_openloop(void)
{
    state = 0;
}

void set_total_count(uint32_t tot_count)
{
    total_count = tot_count;
}

char buffer[50];

void openloop_turn(uint8_t dir)
{
//    sprintf(buffer, "count: %u\r\n", counter);
//    WriteUART0(buffer);
    if (state && counter < total_count)
    {
        setMotor(M1, dir, MOTOR_ON);
        setMotor(M2, !dir, MOTOR_ON);
        counter++;
    }
    else if (counter >= total_count && state)
    {
        end_openloop();
        GPIO_toggleDio(BLED0);
        setMotor(M1, dir, MOTOR_OFF);
        setMotor(M2, !dir, MOTOR_OFF);
        state = 0;
    }

    return;
}

