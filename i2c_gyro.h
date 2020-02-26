#ifndef GYRO__H__
#define GYRO__H__
// Basic interface with an L3GD20H gyro.
// Used for both i2c and spi examples, but with different implementation (.c) files
#include "i2c_master.h"

#define GYRO_WHOAMI 0x0F

#define GYRO_ADDR 0x6B                       // i2c addresses
#define GYRO_CTRL1 0x20      // control register 1 0xEF
#define GYRO_CTRL4 0x23      // control register 4 0x90
// #define CTRL7 0x26      // control register 7

#define GYRO_OUT_X_L_G 0x28  // LSB of x-axis angular rate register.
#define GYRO_OUT_Y_L_G 0x2A
#define GYRO_OUT_Z_L_G 0x2C

uint32_t GYRO_WhoAmI(void);
void read_ang_vels(float * ang_vels);
void set_gyro_offsets(float gx, float gy, float gz);
uint32_t gyro_ReadReg(uint8_t reg);
//uint16_t gyro_ReadReg(uint8_t reg);
float gyro_get(char axis);


// initialize the accelerometer
void gyro_setup();                                                                  
#endif
