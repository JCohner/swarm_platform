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
#include "ir_sense.h"

//TODO: serious organization decisions need to be made about where these pinds are going to be defined
//      as of rn zumo is a high level abstraction and should not have low level pin decs

#define M1 1
#define M2 2
#define M2_PWM IOID_12//pin 9 on zumo
#define M2_DIR IOID_19//pin 7 on zumo  //THIS SWITCHES TO IOID_19 for future revisions!!!
#define M1_PWM IOID_11//pin 10 on zumo
#define M1_DIR IOID_20//pin 8 on zumo

#define BLED0 IOID_9
#define BLED1 IOID_8
#define BLED2 IOID_13
#define BLED3 IOID_14

#define PURP_LOW 100
#define PURP_HIGH 140

#define GREY_LOW 175
#define GREY_HIGH 220


#define NUM_PREV_VALS 10
struct ColorTrack {
    char curr_state;
    char prev_state;
    uint8_t accum;
    uint8_t stash_val;
    uint16_t prev_vals[NUM_PREV_VALS];
    uint8_t prev_vals_ave;
    uint8_t idx;

    uint16_t high_bound;
    uint16_t low_bound;

};

struct StateTrack{
    //policy and target bits
    uint8_t policy : 2;
    uint8_t target : 1;

    //intersection management bits

    //state dia:
    // 0b00 - no detect
    // 0b01 - detect 1
    // 0b10 - maneuver 1 done
    // 0b11 - detect 2
    // 0b00 - maneuver 2 done
    uint8_t xc_flags : 2;
    uint8_t err_flag : 1;
} state_track;



//#define MR_Sense IOID_25
//#define IR_Sense IOID_23
//#define IL_Sense IOID_24
//#define ML_Sense IOID_26
//#define ER_Sense IOID_27
//#define EL_Sense IOID_28

//WILL HAVE TO ENABLE THIS BEFORE USE in gpio.c
#define LED_Sense IOID_18 //connected to pin 2 on zum (with jumper on IR breakout set there) //

#define MOTOR_ON 256
#define MOTOR_OFF 0
#define MOTOR_TURN 64//64

void setMotor(int motor, int dir, int value);
void driver(uint32_t * vals);
float read_line(uint32_t * vals);
void drive_line(float val, uint32_t * vals);
void calibrate_line(int num_samps);
void detect_poi(uint32_t * vals);
#endif /* ZUMO_H_ */
