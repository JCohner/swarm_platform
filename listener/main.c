

/**
 * main.c
 */
#include <ccfg.c>
#include "CC1310_LAUNCHXL.h"

#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/uart.h>
#include <devices/cc13x0/driverlib/sys_ctrl.h>
#include <rf_listener.h>
#include "gpio.h"
#include "uart.h"
static char buffer[50];
int main(void)
{

    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL | PRCM_DOMAIN_RFCORE);
    while (PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL | PRCM_DOMAIN_RFCORE)
    != PRCM_DOMAIN_POWER_ON);

    InitGPIO();
    GPIO_setDio(CC1310_LAUNCHXL_PIN_GLED);

    InitUART0();
    rf_setup();

      while(1)
      {
//          GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);
          //proceed on space
//          char data = UARTCharGet(UART0_BASE);

          rf_main();
      }


	return 0;
}
