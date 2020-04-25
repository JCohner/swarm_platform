

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

static char buffer[20];
char adc_vals[8];

int main(void)
{

    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL | PRCM_DOMAIN_RFCORE);
    while (PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL | PRCM_DOMAIN_RFCORE)
    != PRCM_DOMAIN_POWER_ON);

    start();
    InitGPIO();
    GPIO_setDio(CC1310_LAUNCHXL_PIN_GLED);
    WriteUART0("making call\r\n");
    InitADC();
    WriteUART0("call returned\r\n");
    PWMEnable();

//    InitUART0();
//    rf_setup();
//      callibrate();
//      GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);
      uint32_t curr_time = 0;
      uint32_t prev_time = 0;
      uint32_t freq = SysCtrlClockGet();
      int counter = 0;
      setMotor(M2, 0, 0);
      setMotor(M1, 0, 0);
      while(1)
      {
          sprintf(buffer,"%d\r\n", counter);
          WriteUART0(buffer);
//          GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);

          //pause until character entered
//          char data = UARTCharGet(UART0_BASE);

//          read_imu();
//          rf_main();
          ReadADC(adc_vals);
          sprintf(buffer,"adc: %u, %u, %u, %u\r\n", adc_vals[0], adc_vals[1], adc_vals[2], adc_vals[3]);
          WriteUART0(buffer);
          while ((curr_time - prev_time) < 1000000){
              ++curr_time;
          }
          curr_time = 0;
          ++counter;
      }


	return 0;
}
