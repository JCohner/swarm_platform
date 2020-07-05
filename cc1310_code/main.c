

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
#include "color_track.h"
#include "rtc.h"


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
    delay(1);
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
//      uint32_t freq = SysCtrlClockGet();
//      sprintf(buffer, "freq: %u\r\n", freq);
//      WriteUART0(buffer);
//      while(1);

//      test_leds();

      //sets open loop control characteristics
      set_on_time(9);
      set_offset_time(10);
//      init_openloop();
//      while(1)
//      {
//          openloop_turn();
//      }


      //sets inital policy pursued by robot
      set_policy(0b10);

//      InitRTC();
      struct RTCVal time_prev = GetRTCVal();
      while(1)
      {
          GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED); //pin6
//          read_imu();
          rf_main();

          ReadIR(adc_vals);

          //ODD IS LEFT
//          sprintf(buffer,"%u, %u, %u, %u, %u, %u\r\n", adc_vals[5], adc_vals[3], adc_vals[1],
//                            adc_vals[0], adc_vals[2], adc_vals[4]);
//          WriteUART0(buffer);
//          int offset = sprintf(buffer, "stator: %u\r\n", get_return_flag() << 2 | get_xc_state());
//          offset += sprintf(buffer + offset, "prev_stator: %u\r\n", get_prev_return_flag() << 2 | get_prev_xc_state());
//          offset += sprintf(buffer + offset, "act: %u\r\n", get_actuation_flag());
//          WriteRF(buffer);

          float IR_val = read_line(adc_vals);
//          sprintf(buffer, "IR VAL: %f\r\n", IR_val);
//          WriteUART0(buffer);
          drive_line(IR_val, adc_vals);
//          detect_poi(adc_vals);
          detect_xc(adc_vals);
          detect_poi(adc_vals);
          //evaluate_state();
          inc_state(); //if at intersection
          //execute_policy();
          manage_intersection();
          openloop_turn();

          struct RTCVal time_curr = GetRTCVal();
          struct RTCVal delta_time = GetDeltaTime(time_prev, time_curr);

          sprintf(buffer, "delt_sec: %u\r\ndelt_frac: %u\r\n", delta_time.sec, delta_time.frac);
          WriteUART0(buffer);
          time_prev = time_curr;

      }


	return 0;
}
