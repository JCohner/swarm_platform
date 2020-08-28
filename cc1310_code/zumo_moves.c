/*
 * zumo_moves.c
 *
 *  Created on: Jun 21, 2020
 *      Author: jambox
 */
#include "zumo_moves.h"
#include "gpio.h"
#include "uart.h"

static uint32_t on_time = 0;
static uint16_t reset_time = 0;

static uint16_t offset_time = 0;
static volatile uint32_t counter = 0;
static volatile uint8_t state = 0;

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
    set_actuation_pre_ret_flag(1);
    WriteUART0("loop initialized\r\n");
}

void end_openloop(void)
{
    GPIO_toggleDio(BLED0);
    state = 0;
}

void set_on_time(uint32_t counts)
{
    on_time = counts;
}

void set_counts(uint32_t counts)
{
    counter = counts;
}


void set_offset_time(uint32_t counts)
{
    offset_time = counts;
}

void set_reset_time(uint32_t counts)
{
    reset_time = counts;
}

char buffer[50];

//0 - CCW (left)
//1 - CW (right)
void rotate(int dir)
{
    //ensures all non zero inputs normalized to 1
    if (dir)
    {
        dir = 1;
//        WriteUART0("turning CW\r\n");
    }
    else
    {
//        WriteUART0("turning CounterCW\r\n");
        ;
    }

    setMotor(M1, dir, MOTOR_TURN);
    setMotor(M2, !dir, MOTOR_TURN);
}

//0 - CCW (left)
//1 - CW (right)
void steer(int dir)
{
    if (dir)
    {
        setMotor(M1, 0, MOTOR_ON - 50);
        setMotor(M2, 0, MOTOR_ON + 50);
    }
    else
    {
        setMotor(M1, 0, MOTOR_ON + 50);
        setMotor(M2, 0, MOTOR_ON - 50);
    }
}

//TODO for scalibility can definitely reference (bb - 1) value for number of
//      bits to bitshift
uint8_t mask_policy(uint8_t policy)
{
    uint8_t ret;
    uint8_t idx = get_bb_idx();
    if (idx == 0)
    {
        ret = policy & get_mask();
    }
    else if (idx == 1)
    {
        ret = (policy & (get_mask() << 2)) >> 2;
    }

    return ret;
}

/*!
 *  \brief Called at 1kHz, if state is active will take over controlling
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

    //this gets the single bit rotation value we need
    //this works due to the OHE nature of the xc_state representation
    uint8_t dir;
    if (!get_return_flag())
    {
        dir = xc_state & mask_policy(policy);
    }
    else
    {
        dir = xc_state & mask_policy(ret_policy);
    }
    //it is recommended that clearing the interrupt flag is not the last command
    TimerIntClear(GPT1_BASE, TIMER_TIMA_TIMEOUT);
    if (state && counter < (on_time + offset_time))
    {
        if (counter > offset_time)
        {
//            WriteUART0("ACTUATING\r\n");
            sprintf(buffer, "dir: %u\r\n", dir);
            WriteUART0(buffer);
            rotate(dir);
        }
        else
        {
            //drive forward cautiously
            setMotor(M1, 0, MOTOR_ON);
            setMotor(M2, 0, MOTOR_ON);
        }

        counter++;
//        sprintf(buffer, "counter %u\r\n", counter);
//        WriteUART0(buffer);
    }
    else if (counter >= on_time + offset_time && state)
    {

//        GPIO_toggleDio(BLED0);
        setMotor(M1, dir, MOTOR_OFF);
        setMotor(M2, !dir, MOTOR_OFF);
//        delay(5);
        end_openloop();
        set_actuation_flag(0);
        state = 0;
    }



//    if(state)
//    {
//        WriteUART0("hi\r\n");
//    }

    return;
}

/*!
 *  \brief calls flag getters from StateTrack, calls open loop controller to turn
 *      if state change detected
 *
 */
void manage_intersection()
{
    uint8_t xc_state = get_xc_state();
    uint8_t prev_xc_state=  get_prev_xc_state();

//    sprintf(buffer, "prev: %u, cur: %u\r\n",prev_xc_state, xc_state);
//    WriteUART0(buffer);
    if (xc_state != prev_xc_state)
    {
        GPIO_toggleDio(BLED3);
        init_openloop();
    }

    set_prev_xc_state(xc_state);
    set_intersection_flag(0);
    return;
}
