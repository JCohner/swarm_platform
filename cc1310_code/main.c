

/**
 * main.c
 */
#include <ccfg.c>
#include "CC1310_LAUNCHXL.h"

#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/sys_ctrl.h>
#include "imu.h"
#include "gpio.h"
#include "i2c_accel.h"
#include "i2c_gyro.h"
#include "pwm.h"
#include "zumo.h"
#include "zumo_rf.h"
#include "adc.h"
#include "helpful.h"
#include "ir_sense.h"

static char buffer[60];
uint32_t adc_vals[8];
uint32_t adc_ave_vals[8];
int main(void)
{

    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL | PRCM_DOMAIN_RFCORE);
    while (PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL | PRCM_DOMAIN_RFCORE)
    != PRCM_DOMAIN_POWER_ON);

    //from imu.c: begins uart and i2c; performs accelerometer and gyro setup
    start();
    //opens digital input output pins, and analog output pins configures motor driver specific pins
    InitGPIO();
    PWMEnable(); //sets timers to finish configuring analog out pins
    //initialize analog input
    InitADC();
    //configures rf driver, configures application specific packages, makes initial chirp call
    rf_setup();

    setMotor(M2, 0, 0);
    setMotor(M1, 0, 0);

    //start up neccesarries for ir sensing
    IR_SenseSetup();
//    char data;
//    while(1){
//        //pause until character entered
//        data = UARTCharGet(UART0_BASE);
//        ReadIR(adc_vals);
//        float IR_val = read_line(adc_vals);
//        drive_line(IR_val, adc_vals);
//        WriteUART0("collecting...\r\n");
//        calibrate_line(10); //not sure of utility yet
//        setMotor(M2, 0, 0);
//        setMotor(M1, 0, 0);
//    }



    //light green LED to show setup is done
    GPIO_setDio(CC1310_LAUNCHXL_PIN_GLED);

//
//      callibrate();
//      GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);

      //returns frequency of system clock: 48 MHz rn
      uint32_t freq = SysCtrlClockGet();
      sprintf(buffer, "freq: %u\r\n", freq);
      WriteUART0(buffer);


      int counter = 0;

      uint32_t since_last=0;

      state_track.policy = 0b10;

//      while(1)
//      {
//          GPIO_toggleDio(BLED0);
//          GPIO_toggleDio(BLED1);
//          GPIO_toggleDio(BLED2);
//          GPIO_toggleDio(BLED3);
//          delay(.1);
//      }



      while(1)
      {

          if(counter % 10 == 0)
          {
              since_last = 1;
          }

//          sprintf(buffer,"%d\r\n", counter);
//          WriteUART0(buffer);
//          GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);

          //pause until character entered
//          char data = UARTCharGet(UART0_BASE);

//          read_imu();
//          rf_main();

          ReadIR(adc_vals);
//          if(since_last)
//          {
//              sprintf(buffer,"adc: %u, %u, %u, %u, %u, %u\r\n", adc_ave_vals[5], adc_ave_vals[3], adc_ave_vals[1],
//                                                                  adc_ave_vals[0], adc_ave_vals[2], adc_ave_vals[4]);
//              WriteUART0(buffer);
//              memset(adc_ave_vals, 0, 6 * 4);
//              since_last = 0;
//          }

          if(1)
          {
              //ODD IS LEFT
              sprintf(buffer,"adc: %u, %u, %u, %u, %u, %u\r\n", adc_vals[5], adc_vals[3], adc_vals[1],
                                                                                adc_vals[0], adc_vals[2], adc_vals[4]);
              WriteUART0(buffer);
          }
//          else
//          {
//              int j;
//              for (j = 0; j < 6; j++)
//              {
//                  adc_ave_vals[j] += .1 * adc_vals[j];
//              }
//          }


//          driver(adc_vals);
          float IR_val = read_line(adc_vals);
          sprintf(buffer, "IR VAL: %f\r\n", IR_val);
          WriteUART0(buffer);
          drive_line(IR_val, adc_vals);
          detect_poi(adc_vals);
          sprintf(buffer, "flag val: %u\r\n", state_track.xc_flags);
          WriteUART0(buffer);
          if (state_track.xc_flags == 0b01)
          {
              if (state_track.policy & 0b01) //mask 1st bit
              {
                  //turn right
              }
              else
              {
                  //turn left
                  WriteUART0("AEJFBIFBIU\r\n");
                  GPIO_toggleDio(BLED1);
              }
          }
          else if (state_track.xc_flags == 0b10)
          {
              if (state_track.policy & 0b10) //mask 2nd bit
              {
                  //turn right
                  GPIO_toggleDio(BLED2);
                  WriteUART0("BDIUBIFDB\r\n");
              }
              else
              {
                  //turn left
              }
              state_track.xc_flags = 0b11 ;
          }

          else if (state_track.xc_flags == 0b11)
          {
              setMotor(M2, 0, 0);
              setMotor(M1, 0, 0);
              while(1);
          }

//          delay(.05);
//          WriteUART0("hi you \r\n");
          ++counter;
      }


	return 0;
}
