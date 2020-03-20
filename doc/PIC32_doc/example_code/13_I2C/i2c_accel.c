#include "accel.h"
#include "i2c_master_int.h"
#include <stdlib.h>

#define I2C_ADDR 0x1D // the I2C slave address

// Wire GND to GND, VDD to 3.3V, SDA to SDA2 (RA3) and SCL to SCL2 (RA2)

// read data from the accelerometer, given the starting register address.
// return the data in data
void acc_read_register(unsigned char reg, unsigned char data[], unsigned int len)
{
  unsigned char write_cmd[1] = {};
  if(len > 1) { // want to read more than 1 byte and we are reading from the accelerometer
    write_cmd[0] = reg | 0x80; // make the MSB of addr 1 to enable auto increment
  }
  else {
    write_cmd[0] = reg;
  }
  i2c_write_read(I2C_ADDR,write_cmd, 1, data,len);
}

void acc_write_register(unsigned char reg, unsigned char data)
{
  unsigned char write_cmd[2];
  write_cmd[0] = reg;   // write the register
  write_cmd[1] = data;  // write the actual data
  i2c_write_read(I2C_ADDR, write_cmd, 2, NULL, 0);
}

void acc_setup() {                 // set up the accelerometer, using I2C 2
  i2c_master_setup();
  acc_write_register(CTRL1, 0xAF); // set accelerometer data rate to 1600 Hz. 
                                   // Don't update until we read values
  acc_write_register(CTRL5, 0xF0); // 50 Hz magnetometer, high resolution, temp sensor on
  acc_write_register(CTRL7, 0x0);  // enable continuous reading of the magnetometer
}
