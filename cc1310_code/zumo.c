/*
 * zumo.c
 *
 *  Created on: Mar 1, 2020
 *      Author: jambox
 */

#include "zumo.h"

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




