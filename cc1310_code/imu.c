#include "imu.h"
#include "gpio.h"
#include "CC1310_LAUNCHXL.h"
#include <stdio.h>
#include <math.h>
/*values*/
float accels[3] = {0,0,0};
float teslas[3]= {0,0,0};
float ang_vels[3];
float heading;

/*callibration vars*/
int callib_flag = 0;
float calib_data[9] = {0,0,0,0,0,0,0,0,0};
//enum data {x,y,z};

/*IO*/
static char buffer[200];

void start()
{
    InitUART0();
    WriteUART0("uart init\n\r");
    InitI2C0();
    WriteUART0("i2c init\n\r");
    acc_setup();
//    gyro_setup();

}

float x_prev, y_prev, z_prev;
void read_imu()
{
    uint32_t z_h, z_l, y_l, y_h, x_l, x_h, my_l, my_h, mz_l, mz_h;
    float x, y, z;
    short mz, my;
    I2CReceive(ACC_ADDR, ACC_OUT_X_L_A, &x_l,1);
    I2CReceive(ACC_ADDR, ACC_OUT_X_L_A + 1, &x_h,1);
    x = ((short) (x_h << 8) | x_l) * 0.000061;
    if (fabs(x) < 0.05){
        x = 0;
    } else if (fabs(x) > 2) {
        x = x_prev;
    } else {
        x_prev = x;
    }
    I2CReceive(ACC_ADDR, ACC_OUT_Y_L_A, &y_l,1);
    I2CReceive(ACC_ADDR, ACC_OUT_Y_L_A + 1, &y_h,1);
    y = ((short) (y_h << 8) | y_l) * 0.000061;
    if (fabs(y) < 0.05){
         y = 0;
     }else if (fabs(y) > 2) {
         y = y_prev;
     } else {
         y_prev = y;
     }
    I2CReceive(ACC_ADDR, ACC_OUT_Z_L_A, &z_l,1);
    I2CReceive(ACC_ADDR, ACC_OUT_Z_L_A + 1, &z_h,1);
    z = ((short) (z_h << 8) | z_l) * 0.000061;
    if (fabs(z) < 0.05){
         x = 0;
     }else if (fabs(z) > 2) {
         z = z_prev;
     } else {
         z_prev = z;
     }
    I2CReceive(ACC_ADDR, ACC_OUT_Y_L_M, &my_l,1);
    I2CReceive(ACC_ADDR, ACC_OUT_Y_L_M + 1, &my_h,1);
    my = ((short) (my_h << 8) | my_l);// / 16384.0;
    I2CReceive(ACC_ADDR, ACC_OUT_Z_L_M, &mz_l,1);
    I2CReceive(ACC_ADDR, ACC_OUT_Z_L_M + 1, &mz_h,1);
    mz = ((short) (mz_h << 8) | mz_l);/// 16384.0;

    heading = atan2(my,mz)  * 180.0 / PI;

//    read_accels(accels, teslas);
//    read_ang_vels(ang_vels);
//    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);

    if (!callib_flag){
        sprintf(buffer,"ax: %10.5f \r\nay: %10.5f \r\naz: %10.5f\r\n", x, y, z);
        WriteUART0(buffer);
//        sprintf(buffer,"mx: %10.5f \r\nmy: %10.5f \r\nmz: %10.5f\r\n", teslas[x], teslas[y], teslas[z]);
//        WriteUART0(buffer);
        sprintf(buffer, "heading: %f\r\n", heading);
        WriteUART0(buffer);
//        sprintf(buffer, "gx: %10.5f\r\ngy: %10.5f\r\ngz: %10.5f\r\n", ang_vels[x], ang_vels[y], ang_vels[z]);
//        WriteUART0(buffer);
    }
//    GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);
}


void callibrate()
{
//    //take 1000 measurements to callibrate
////    GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);
//    int i = 0;
//    float num_samples = 1000;
//    for (;i < num_samples; i++){
////        WriteUART0("pre read\n");
//        read_imu();
////        WriteUART0("post read\n");
//        calib_data[0] += accels[x]/num_samples;
//        calib_data[1] += accels[y]/num_samples;
//        calib_data[2] += accels[z]/num_samples;
//
////        calib_data[3] += ang_vels[x]/num_samples;
////        calib_data[4] += ang_vels[y]/num_samples;
////        calib_data[5] += ang_vels[z]/num_samples;
//    }
////    GPIO_clearDio(CC1310_LAUNCHXL_PIN_RLED);
////    set_gyro_offsets(calib_data[3], calib_data[4], calib_data[5]);
////    sprintf(buffer,"calibration complete:\n\rax: %f \tay: %f\taz: %f\n\rgx: %f \tgy: %f\tgz: %f\n\r",
////            calib_data[x], calib_data[y], calib_data[z], calib_data[(3 + x)], calib_data[(3 + y)], calib_data[(3 + z)]);
//    sprintf(buffer,"calibration complete:\n\rax: %f \tay: %f\taz: %f\n\r",
//            calib_data[x], calib_data[y], calib_data[z]);
//    WriteUART0(buffer);
//    callib_flag = 1;
}

//// void update_filter()
//// {
////  	/*update for embedded*/
////   // get current time in nanoseconds
////   // timespec_get(&te,TIME_UTC);
////   // time_curr=te.tv_nsec;
////   // //compute time since last execution
////   // float imu_diff=time_curr-time_prev;
//
////   // //check for rollover
////   // if(imu_diff<=0)
////   // {
////   //   imu_diff+=1000000000;
////   // }
////   // //convert to seconds
////   // imu_diff=imu_diff/1000000000;
////   // time_prev=time_curr;
//
////   // //comp. filter for roll, pitch here:
////   // /*Roll*/
////   // roll_accel = roll_angle;
////   // roll_gyro_delta = imu_data[1] * imu_diff;
////   // roll_sum += roll_gyro_delta;
////   // // printf("accel: %f\n", roll_accel);
////   // roll_next = roll_accel * A + (1-A) * (roll_gyro_delta + roll_curr);
////   // roll_curr = roll_next;
//
////   // /*Pitch*/
////   // pitch_accel = pitch_angle;
////   // pitch_gyro_delta = imu_data[0] * imu_diff;
////   // pitch_sum += pitch_gyro_delta;
////   // // printf("accel: %f\n", pitch_accel);
////   // pitch_next = pitch_accel * B + (1-B) * (pitch_gyro_delta + pitch_curr);
////   // pitch_curr = pitch_next;
//
////   // pitch_smooth = pitch_curr;
////   // roll_smooth = roll_curr;
//
//// }
