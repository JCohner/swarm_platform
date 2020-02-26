#include "imu.h"
#include "gpio.h"
#include "CC1310_LAUNCHXL.h"
#include <stdio.h>
/*values*/
float accels[3];
float teslas[3];
float ang_vels[3];
float heading;

/*callibration vars*/
int callib_flag = 0;
float calib_data[9] = {0,0,0,0,0,0,0,0,0};
enum data {x,y,z};

/*IO*/
static char buffer[200];

void start()
{
    InitUART0();
    WriteUART0("uart init\n\r");
    InitI2C0();
    WriteUART0("i2c init\n\r");
//    acc_setup();
//    gyro_setup();

}

void read_imu()
{
    heading = atan2(accel_get('M','y'),accel_get('M','z'))  * 180.0 / PI;

    read_accels(accels, teslas);
    read_ang_vels(ang_vels);
    GPIO_clearDio(CC1310_LAUNCHXL_PIN_GLED);

    if (callib_flag){
        sprintf(buffer,"ax: %10.5f \r\nay: %10.5f \r\naz: %10.5f\r\n", accels[x], accels[y], accels[z]);
        WriteUART0(buffer);
        sprintf(buffer,"mx: %10.5f \r\nmy: %10.5f \r\nmz: %10.5f\r\n", teslas[x], teslas[y], teslas[z]);
        WriteUART0(buffer);
        sprintf(buffer, "heading: %f\r\n", heading);
        WriteUART0(buffer);
        sprintf(buffer, "gx: %10.5f\r\ngy: %10.5f\r\ngz: %10.5f\r\n", ang_vels[x], ang_vels[y], ang_vels[z]);
        WriteUART0(buffer);
    }
    GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);
}


void callibrate()
{
    //take 1000 measurements to callibrate
//    GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);
    int i = 0;
    float num_samples = 1000;
    for (;i < num_samples; i++){
//        WriteUART0("pre read\n");
        read_imu();
//        WriteUART0("post read\n");
        calib_data[0] += accels[x]/num_samples;
        calib_data[1] += accels[y]/num_samples;
        calib_data[2] += accels[z]/num_samples;

        calib_data[3] += ang_vels[x]/num_samples;
        calib_data[4] += ang_vels[y]/num_samples;
        calib_data[5] += ang_vels[z]/num_samples;
    }
//    GPIO_clearDio(CC1310_LAUNCHXL_PIN_RLED);
    set_gyro_offsets(calib_data[3], calib_data[4], calib_data[5]);
    sprintf(buffer,"calibration complete:\n\rax: %f \tay: %f\taz: %f\n\rgx: %f \tgy: %f\tgz: %f\n\r",
            calib_data[x], calib_data[y], calib_data[z], calib_data[(3 + x)], calib_data[(3 + y)], calib_data[(3 + z)]);
    WriteUART0(buffer);
    callib_flag = 1;
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
