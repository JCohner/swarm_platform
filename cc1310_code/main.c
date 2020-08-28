

/**
 * @file main.c
 * @brief Main file of the project
 *
 * main's primary use is to perform all necessary initializations.
 * the "main" while loop is also useful for debugging; however, all critical
 * code is executed in timer interrupts.
 */

//CC1310 includes
#include <ccfg.c>
#include <color_track.h>
#include "CC1310_LAUNCHXL.h"
#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/sys_ctrl.h>
//developed includes
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
#include "interrupt_timer.h"
#include "comm_packet.h"
#include "dist_sense.h"
#include "leds.h"
#include "rando.h"

static char buffer[60];

uint8_t pol;
uint8_t pol_lookup()
{

    uint16_t mach_id = get_mach_id();
    switch(mach_id)
    {
    case 0xC464:
        pol = 0;
        break;
    case 0xC219:
        pol = 0;
        break;
    case 0xA171:
        pol = 2;
        break;
    case 0xC683:
        pol = 2;
        break;
    case 0x20CE:
        pol = 3;
        break;
    case 0xB5A8:
        pol = 3;
        break;
    case 0xC262:
        pol = 1;
        break;
    case 0xA3EB:
        pol = 1;
        break;
    case 0xC718:
        pol = 0;
        break;
    default:
        sprintf(buffer, "no match for %X\r\n", mach_id);
        WriteUART0(buffer);
        while(1);
    }

    return pol;
}



uint32_t adc_vals[8];
uint32_t adc_ave_vals[8];
int main(void)
{
    //CC1310 Requires you specifically turn on any peripheral you are going to use
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
    //initialize True Random Number Gen periph
    RandoConfig();
    //initializes polict and sets machine ID to first 2 bytes of MAC address



    set_mach_id(HWREG(FCFG1_BASE +0x2F0) & 0xFFFF);
    //configures rf driver, configures application specific packages, makes initial chirp call
    rf_setup();
    //explicitly starts motors in off state
    setMotor(M2, 0, 0);
    setMotor(M1, 0, 0);
    //usefull delay on startup (also seems needed for random number gen??)
    delay(1);

    //IF WE WANT RAND POLICY
//    set_policy(get_random_num(4));

    set_policy(pol_lookup());

//    sprintf(buffer, "rand: %u\r\n" ,get_policy());
//    WriteUART0(buffer);
//    while(1);
    //start up neccesarries for ir sensing (GPT2A full width set up)
    IR_SenseSetup();
    //configures GPIOs associated with LEDs
    setup_leds();

    //sets open loop control characteristics
    set_on_time(400);
    set_offset_time(500);

    /*
     * State Track Initialization for Circulatory Map
     * exclusively comment this in for state machine
     * to be in circulatory map configuration
     * */
//    uint8_t bbs[2] = {3,4};
//    init_state(0b1100, 2, bbs, 4, 1, 0, 0x7);

    /*
     * State Track Initialization for 5 node map
     * exclusively comment this in for state machine
     * to be in 5 node map configuration
     * */
      uint8_t bbs[1] = {3};
      init_state(0b110, 1, bbs, 3, 0, 0, 0x3);

      //Enable timer based interrupts:
      //    GPT1A - Openloop motor control
      //    GPT1B - Information sensing and decision making
      //    GPT3A - RF Communication
      InterTimerEnable();

      while(1)
      {
//          uint32_t forw = ReadDistForward();
//          uint32_t side = ReadDist45();
//          sprintf(buffer, "%u %u\r\n", forw, side);
//          WriteUART0(buffer);



          ReadIR(adc_vals);
////          WriteRF(adc_vals);
                    sprintf(buffer,"%u, %u, %u, %u, %u, %u\r\n", adc_vals[5], adc_vals[3], adc_vals[1],
                                      adc_vals[0], adc_vals[2], adc_vals[4]);
                    WriteUART0(buffer);


//          sprintf(buffer,"dist val %u\r\n", ReadDist());
//          WriteUART0(buffer);
//          GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
//          read_imu();
      }


	return 0;
}
