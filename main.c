

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
static char buffer[200];
int main(void)
{

    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL);
    while (PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL)
    != PRCM_DOMAIN_POWER_ON);

    InitGPIO();
    GPIO_setDio(CC1310_LAUNCHXL_PIN_GLED);

    PWMEnable();
    start();
//      callibrate();
//      GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);
      uint32_t curr_time = 0;
      uint32_t prev_time = 0;
      uint32_t freq = SysCtrlClockGet();
      int counter = 0;

      while(1)
      {
          sprintf(buffer,"%d\r\n", counter);
          WriteUART0(buffer);

          GPIO_setDio(M1_DIR);

//          read_imu();
          while ((curr_time - prev_time) < 1000000){
              ++curr_time;
          }
          curr_time = 0;
          ++counter;
      }


	return 0;
}
