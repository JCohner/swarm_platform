#include <i2c_gyro.h>
#include <stdlib.h>

#define LSB_TO_G_PM2G 0.000061  // FS +/- 2G
#define LSB_TO_G_PM4G 0.000122  // FS +/- 4G
#define LSB_TO_G_PM6G 0.000183  // FS +/- 6G
#define LSB_TO_G_PM8G 0.000244  // FS +/- 8G
#define LSB_TO_G_PM16G 0.000732 // FS +/- 16G

float gx_offset,gy_offset,gz_offset;

uint32_t GYRO_WhoAmI(void) {
    uint32_t who_am_i;
    I2CReceive(GYRO_ADDR, GYRO_WHOAMI, &who_am_i,1);
    return who_am_i;
}

void set_gyro_offsets(float gx, float gy, float gz)
{
    gx_offset = gx;
    gy_offset = gy;
    gz_offset = gz;
}

uint32_t gyro_ReadReg(uint8_t reg)
{
    uint32_t gyroData[2];
    I2CReceive(GYRO_ADDR, reg, gyroData,2);


    return ((gyroData[1] << 8) | gyroData[0]);
}

//description: get floating point value for specific reading type of specific axis
//params - type: Magentometer [M] or Accel [A]
//params - axis:'x', 'y', or 'z'
float gyro_get(char axis)
{
    char reg = GYRO_OUT_X_L_G;

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

    float value = gyro_ReadReg(reg) * LSB_TO_G_PM2G;
    return value;
}

void read_ang_vels(float * ang_vels)
{
    ang_vels[0] = gyro_get('x') + gx_offset;
    ang_vels[1] = gyro_get('y') + gy_offset;
    ang_vels[2] = gyro_get('z') + gz_offset;
}


void gyro_setup()
{
    // set up the gyro, using I2C1
    I2CSend(GYRO_ADDR, GYRO_CTRL1, 0xEF);
    I2CSend(GYRO_ADDR, GYRO_CTRL4, 0x90);

}
