#ifndef ACCEL__H__
#define ACCEL__H__
// Basic interface with an LSM303D accelerometer/compass.
// Used for both i2c and spi examples, but with different implementation (.c) files

                        // register addresses
#define CTRL1 0x20      // control register 1
#define CTRL5 0x24      // control register 5
#define CTRL7 0x26      // control register 7

#define OUT_X_L_A 0x28  // LSB of x-axis acceleration register.
                        // accel. registers are contiguous, this is the lowest address
#define OUT_X_L_M 0x08  // LSB of x-axis of magnetometer register

#define TEMP_OUT_L 0x05 // temperature sensor register

// read len bytes from the specified register into data[]
void acc_read_register(unsigned char reg, unsigned char data[], unsigned int len); 

// write to the register
void acc_write_register(unsigned char reg, unsigned char data);                    

// initialize the accelerometer
void acc_setup();                                                                  
#endif
