/*
 * zumo.c
 *
 *  Created on: Mar 1, 2020
 *      Author: jambox
 */

#include "zumo.h"
#include "uart.h"

char buffer[50];

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



int read_line(uint32_t * vals)
{
    float on_line = (vals[0] + vals[1])/2;

    float rhs = ((1000.0 - (vals[0]))/1000.0) * on_line/1000.0 + 0.5;
    float lhs = ((1000.0 - (vals[1]))/1000.0) * on_line/1000.0 + 0.5;


//    float rhs = (on_line - (vals[0] + vals[2])/2.0)/on_line;
//    float lhs = (on_line - (vals[1] + vals[3])/2.0)/on_line;

    int m1_val = MOTOR_ON * lhs;
    int m2_val = MOTOR_ON * rhs;
    sprintf(buffer, "left: %f, %d, right: %f, %d\r\n",  lhs, m1_val, rhs, m2_val);
    WriteUART0(buffer);

//    setMotor(M1, 0, m1_val);
//    setMotor(M2, 0, m2_val);

    return;
}
