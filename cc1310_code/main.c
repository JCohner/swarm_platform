

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
#include "zumo_moves.h"

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
    ////pause until character entered
    //data = UARTCharGet(UART0_BASE);



    //light green LED to show setup is done
    GPIO_setDio(CC1310_LAUNCHXL_PIN_GLED);

//
//      callibrate();
//      GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);

      //returns frequency of system clock: 48 MHz rn
      uint32_t freq = SysCtrlClockGet();
      sprintf(buffer, "freq: %u\r\n", freq);
      WriteUART0(buffer);
//      while(1);

      int counter = 0;
//      test_leds();

      //sets open loop control characteristics
      set_total_count(8);
      set_offset(22);

      //sets inital policy pursued by robot
      set_policy(0b10);

      while(1)
      {

//          read_imu();
          rf_main();

          ReadIR(adc_vals);

          //ODD IS LEFT
//          sprintf(buffer,"adc: %u, %u, %u, %u, %u, %u\r\n", adc_vals[5], adc_vals[3], adc_vals[1],
//                            adc_vals[0], adc_vals[2], adc_vals[4]);
//          WriteUART0(buffer);

          float IR_val = read_line(adc_vals);
//          sprintf(buffer, "IR VAL: %f\r\n", IR_val);
//          WriteUART0(buffer);
          drive_line(IR_val, adc_vals);
          detect_poi(adc_vals);
          evaluate_state();
          execute_policy();
          openloop_turn();

//          delay(.05); //WILL MESS UP COLOR DETECT THRESHOLDING
//          WriteUART0("hi you \r\n");
          ++counter;
      }


	return 0;
}
