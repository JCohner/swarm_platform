#include <i2c_accel.h>
#include <stdlib.h>

#define LSB_TO_G_PM2G 0.000061  // FS +/- 2G
#define LSB_TO_G_PM4G 0.000122  // FS +/- 4G
#define LSB_TO_G_PM6G 0.000183  // FS +/- 6G
#define LSB_TO_G_PM8G 0.000244  // FS +/- 8G
#define LSB_TO_G_PM16G 0.000732 // FS +/- 16G


uint32_t ACC_WhoAmI(void) {
    uint32_t who_am_i;
    I2CReceive(ACC_ADDR, ACC_WHOAMI, &who_am_i,1);
    return who_am_i;
}

uint32_t accel_ReadReg(uint8_t reg)
{
    uint32_t accelData[2];
    I2CReceive(ACC_ADDR, reg, accelData,1);
    I2CReceive(ACC_ADDR, reg, accelData + 1,1);

    return ((accelData[1] << 8) | accelData[0]);
}

//description: get floating point value for specific reading type of specific axis
//params - type: Magentometer [M] or Accel [A]
//params - axis:'x', 'y', or 'z'
float accel_get(char type, char axis)
{
    char reg;
    switch(type)
    {
    case 'M':
        reg = ACC_OUT_X_L_M;// | 0x80;
        break;
    case 'A':
        reg = ACC_OUT_X_L_A ;//| 0x80; //enables continous reading
        break;
    default:
        //TODO: UART Error Log
        ;
    }

    switch(axis)
    {
    case 'x':
        //already pointing to x reg
        break;
    case 'y':
        reg += 2; //sets to y reg
        break;
    case 'z':
        reg  += 4;
        break;
    default:
        //TODO: error log
        ;
    }

    float value = accel_ReadReg(reg) * 0.000061;
    return value;
}

void read_accels(float * accels, float * teslas)
{
    accels[0] = accel_get('A', 'x');
//    accels[1] = accel_get('A', 'y');
//    accels[2] = accel_get('A', 'z');
//    teslas[0] = accel_get('M', 'x');
//    teslas[1] = accel_get('M', 'y');
//    teslas[2] = accel_get('M', 'z');
}

void acc_setup() {
  // set up the accelerometer, using I2C 2
  I2CSend(ACC_ADDR, ACC_CTRL1, 0xAF);
  I2CSend(ACC_ADDR, ACC_CTRL5, 0xF0);
  I2CSend(ACC_ADDR, ACC_CTRL7, 0x00);

  //add WhoAmI call and uart log here
}
