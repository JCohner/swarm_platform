#ifndef IMU__H__
#define IMU__H__

#include "i2c_accel.h"
#include "i2c_gyro.h"
#include "i2c_master.h"
#include "uart.h"
#include <math.h>
#include <stdio.h>

#define PI 3.14159

void start(); //initializes i2c, gyro, and accel
void read_imu();
void callibrate();
#endif //IMU__H__
