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
    int left_right_bias = vals[0] - vals[1];
    int thresh = 128;

//    sprintf(buffer, "hi fuck this: %d\r\n", left_right_bias);
//    WriteUART0(buffer);

    //TODO stop being shitty and make this a proportional controller w threshold sir
    if ((vals[0] < 300) && (vals[1] < 300))
    {
        //stahp
        setMotor(M2, 0, MOTOR_OFF);
        setMotor(M1, 0, MOTOR_OFF);
    }
    else if (left_right_bias > thresh)
    {
        //turn right
        setMotor(M1, 0, MOTOR_ON);
        setMotor(M2, 1, MOTOR_TURN);
    }
    else if (left_right_bias < -thresh)
    {
        //turn left
        setMotor(M1, 1, MOTOR_TURN);
        setMotor(M2, 0, MOTOR_OFF);
    }
    else if (vals[0] > 300)
    {
        //go forward
        setMotor(M1, 1, MOTOR_ON);
        setMotor(M2, 1, MOTOR_ON);
    }
}


