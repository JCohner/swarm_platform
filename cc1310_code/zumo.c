/*
 * zumo.c
 *
 *  Created on: Mar 1, 2020
 *      Author: jambox
 */

#include "zumo.h"
#include "uart.h"
#include <math.h>

char buffer[50];
char bleds[4] = {BLED0, BLED1, BLED2, BLED3};
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

void driver(uint32_t * vals)
{
    if((vals[0] > 500) && vals[1] > 500)
    {
        setMotor(M1, 1, MOTOR_OFF);
        setMotor(M2, 1, MOTOR_OFF);
    }
    else
    {
        setMotor(M1, 0, MOTOR_ON);
        setMotor(M2, 0, MOTOR_ON);
    }


}

static uint32_t line_calib[8];

void calibrate_line()
{

    uint32_t temp_adc_vals[8];
    int i, j;
    for (i = 0; i < 10; i++)
    {
        ReadIR(temp_adc_vals);
        for (j = 0; j < 6; j++)
        {
            line_calib[j] += 0.1 * temp_adc_vals[j];
        }
    }

    sprintf(buffer, "calib: %u, %u, %u, %u, %u, %u\r\n", line_calib[5], line_calib[3], line_calib[1], line_calib[0], line_calib[2], line_calib[4]);
    WriteUART0(buffer);
//    while(1);
}

float lastValue = 0;
float read_line(uint32_t * vals)
{
    unsigned char on_line = 0;
    uint32_t avg = 0;
    uint32_t sum = 0;
    int i;

//    uint32_t ordered_vals[6] = {vals[5], vals[3], vals[1], vals[0], vals[2], vals[4]};
    uint32_t ordered_vals[4] = {vals[3], vals[1], vals[0], vals[2]};
    for (i = 0; i < 4; ++i)
    {
        int value = ordered_vals[i];
        if (value > 600)
        {
            on_line = 1;
        }

        if (value > 460)
        {
            avg += value * (i * 1000);
            sum += value;
        }
    }

    if (!on_line)
    {
        // If it last read to the left of center, return 0.
        if (lastValue < (4 -  1) * 1000/2.0)
        {
            return 0;
        }
        //otherwise return rightmost
        else
        {
            return (4 - 1);
        }
//        return lastValue/1000.0;
    }

    lastValue = avg/(float)sum;
    return lastValue/1000.0;
}

float error, e, prev_error, d_error;
float dt = 0.05;
void drive_line(float val, uint32_t * vals)
{
    prev_error = error;
    error = 1.5 - val;
    e = (1.5 - val)/1.5;
    d_error = (error-prev_error)/dt;

    sprintf(buffer, "error: %f\r\n", error);
    WriteUART0(buffer);
    float speed_delim = 1 - fabs(error)/1.5;
    float rhs = speed_delim * MOTOR_ON + (e * MOTOR_ON/2.0) + 128;
    float lhs = speed_delim * MOTOR_ON - (e * MOTOR_ON/2.0) + 128;

    sprintf(buffer, "rhs: %f, lhs: %f\r\n", rhs, lhs);
    WriteUART0(buffer);


    setMotor(M2, 0, lhs);
    setMotor(M1, 0, rhs);

    if (fabs(error) == 1.5 || error == 0) {
        if (error < 0){
            setMotor(M1, 1, MOTOR_ON);
            setMotor(M2, 0, MOTOR_ON);
        }
        else if (error > 0){
            setMotor(M1, 0, MOTOR_ON);
            setMotor(M2, 1, MOTOR_ON);
        }
        else
        {
            setMotor(M1, 0, MOTOR_OFF);
            setMotor(M2, 0, MOTOR_OFF);
        }
//        setMotor(M1, 0, MOTOR_OFF);
//        setMotor(M2, 0, MOTOR_OFF);
//        WriteUART0("rough waters\r\n");
    }

    return;

//    int x = val;
//    switch (x)
//    {
//    case 0:
//        GPIO_writeDio(BLED0, 1);
//
//        GPIO_writeDio(BLED1, 0);
//        GPIO_writeDio(BLED2, 0);
//        GPIO_writeDio(BLED3, 0);
//        break;
//    case 1:
//        GPIO_writeDio(BLED1, 1);
//
//        GPIO_writeDio(BLED2, 0);
//        GPIO_writeDio(BLED3, 0);
//        GPIO_writeDio(BLED0, 0);
//        break;
//    case 2:
//        GPIO_writeDio(BLED2, 1);
//
//        GPIO_writeDio(BLED1, 0);
//        GPIO_writeDio(BLED3, 0);
//        GPIO_writeDio(BLED0, 0);
//        break;
//
//    case 3:
//        GPIO_writeDio(BLED3, 1);
//
//        GPIO_writeDio(BLED1, 0);
//        GPIO_writeDio(BLED2, 0);
//        GPIO_writeDio(BLED0, 0);
//        break;
//    }
}
