#ifndef I2C_MASTER_NO_INT__H__
#define I2C_MASTER_NO_INT__H__
// Header file for i2c_master_no_int.c
// helps implement I2C as a master w/o interrupts
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <devices/cc13x0/inc/hw_i2c.h>
#include <devices/cc13x0/inc/hw_memmap.h>
#include <devices/cc13x0/inc/hw_types.h>
#include <devices/cc13x0/inc/hw_gpio.h>
#include <devices/cc13x0/driverlib/i2c.h>
#include <devices/cc13x0/driverlib/sys_ctrl.h>
#include <devices/cc13x0/driverlib/gpio.h>
#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/ioc.h>

void InitI2C0(void);
void I2CSend(uint8_t slave_addr, uint8_t reg, uint8_t value);
void I2CSendString(uint32_t slave_addr, char array[]);
void I2CReceive(uint32_t slave_addr, uint8_t reg, uint32_t * data, uint8_t num_data);

#endif
