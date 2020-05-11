/*
 * zumo.h
 *
 *  Created on: Mar 1, 2020
 *      Author: jambox
 */

#ifndef ZUMO_H_
#define ZUMO_H_

#include "gpio.h"
#include "pwm.h"

#define M1 1
#define M2 2
#define M2_PWM IOID_12//pin 9 on zumo
#define M2_DIR IOID_6//pin 7 on zumo  //THIS SWITCHES TO IOID_19 for future revisions!!!
#define M1_PWM IOID_11//pin 10 on zumo
#define M1_DIR IOID_20//pin 8 on zumo

#define MR_Sense IOID_25
#define IR_Sense IOID_23
#define IL_Sense IOID_24
#define ML_Sense IOID_26
#define ER_Sense IOID_27
#define EL_Sense IOID_28

#define LED_Sense IOID_19 //connected to pin 2 on zum (with jumper on IR breakout set there)

#define MOTOR_ON 256//256
#define MOTOR_OFF 0
#define MOTOR_TURN 64//64

void setMotor(int motor, int dir, int value);
void driver(uint32_t * vals);

#endif /* ZUMO_H_ */
