#ifndef ACCEL__H__
#define ACCEL__H__
// Basic interface with an LSM303D accelerometer/compass.
// Used for both i2c and spi examples, but with different implementation (.c) files
#include "i2c_master.h"

#define ACC_WHOAMI 0x0F

#define ACC_ADDR 0x1D               // i2c addresses
#define ACC_CTRL1 0x20      // control register 1
#define ACC_CTRL5 0x24      // control register 5
#define ACC_CTRL7 0x26      // control register 7

#define ACC_OUT_X_L_A 0x28  // LSB of x-axis acceleration register.
#define ACC_OUT_Y_L_A 0x2A
#define ACC_OUT_Z_L_A 0x2C
#define ACC_OUT_X_L_M 0x08
#define ACC_OUT_Y_L_M 0x0A
#define ACC_OUT_Z_L_M 0x0C

#define ACC_TEMP_OUT_L 0x05 // temperature sensor register


uint32_t ACC_WhoAmI(void);
void read_accels(float * accels, float * teslas);
uint32_t accel_ReadReg(uint8_t reg);
float accel_get(char type, char axis);
// initialize the accelerometer
void acc_setup();
#endif
