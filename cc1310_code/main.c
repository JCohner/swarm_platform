

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
      set_on_time(325);
      set_offset_time(500);

//      //state track for circ
//      uint8_t bbs[2] = {3,4};
//      init_state(0b1100, 2, bbs, 4, 1, 0, 0x7);

      //state track for 5 node
      uint8_t bbs[1] = {3};
      init_state(0b110, 1, bbs, 3, 0, 0, 0x3);


      set_policy(0b00011);
      set_mach_id(HWREG(FCFG1_BASE +0x2F0) & 0xFFFF);

      InterTimerEnable();


      while(1)
      {
//          GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
//          read_imu();
//          rf_main();
//          WriteRFPacket(get_packet());
      }


	return 0;
}
