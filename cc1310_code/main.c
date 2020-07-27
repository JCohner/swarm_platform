

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
//#include "rtc.h"
#include "interrupt_timer.h"
#include "comm_packet.h"
#include "dist_sense.h"
#include "leds.h"

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

    setup_leds();

    //    char data;
    ////pause until character entered
    //data = UARTCharGet(UART0_BASE);

    //light green LED to show setup is done
//    GPIO_setDio(CC1310_LAUNCHXL_PIN_GLED);
//      callibrate();
//      test_leds();

      //sets open loop control characteristics
      set_on_time(300);
      set_offset_time(500);

      //sets inital policy pursued by robot
      set_policy(0b01);
      set_mach_id(1);

//
      set_policy(0b10);
      set_mach_id(2);

//      InitRTC();
      InterTimerEnable();
//      struct RTCVal time_prev = GetRTCVal();
      while(1)
      {
//          GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED); //pin6
//          read_imu();
          rf_main();

          ReadIR(adc_vals);

          //ODD IS LEFT
//          sprintf(buffer,"%u, %u, %u, %u, %u, %u\r\n", adc_vals[5], adc_vals[3], adc_vals[1],
//                            adc_vals[0], adc_vals[2], adc_vals[4]);
//          WriteUART0(buffer);
//          sprintf(buffer, "int: %u\r\ndet: %u\r\n, line:%u\r\n", )

//          WriteRF(buffer);

//          WriteRFState(get_return_flag() << 2 | get_xc_state());
          WriteRFPacket(get_packet());

          float IR_val = read_line(adc_vals);
          uint32_t Dist_val = ReadDist();
//          sprintf(buffer, "DIST VAL: %f\r\n", Dist_val);
//          WriteUART0(buffer);
          drive_line(IR_val, Dist_val, adc_vals);

          detect_poi(adc_vals);
          detect_xc(adc_vals);
          inc_state();
          manage_intersection();

          manage_leds();
      }


	return 0;
}
