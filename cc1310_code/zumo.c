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
    int DIR, PWM;
    if (motor == M1)
    {
       DIR = M1_DIR;
       PWM = M1_PWM;
    }
    else if (motor == M2)
    {
        DIR = M2_DIR;
        PWM = M2_PWM;
    }

    int set_val = 1022 - value;
    if (set_val < 0){
        set_val = 0;
    }

    GPIO_writeDio(DIR, dir);
    PWMSet(PWM, set_val);

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

int read_line(uint32_t * vals)
{
//    uint32_t onLine = (val[0] + val[1]) / 2.0;
//    if (onLine > 800)
//    {
//        setMotor(M1, 0, MOTOR_ON);
//        setMotor(M2, 0, MOTOR_ON);
//    }

    int m1_val = MOTOR_ON * (vals[0] / 1000.0);
    int m2_val = MOTOR_ON * (vals[1] / 1000.0);

    setMotor(M1, 0, m1_val);
    setMotor(M2, 0, m2_val);


}
