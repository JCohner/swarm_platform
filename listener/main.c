

/**
 * main.c
 */
#include <ccfg.c>
#include "CC1310_LAUNCHXL.h"

#include <devices/cc13x0/driverlib/prcm.h>
#include <devices/cc13x0/driverlib/uart.h>
#include <devices/cc13x0/driverlib/sys_ctrl.h>
#include "rf_listener.h"
#include <stdlib.h>

#include "gpio.h"
#include "uart.h"
#include "print_info.h"
#include "leds.h"
#include "helpful.h"

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
    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);
    WriteUART0("got here\r\n");


    READUART0_AS_ENABLE();

    setup_leds();

      while(1)
      {
//          GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);

          uint8_t flag = ReadUART0_AS(buffer, 16);
          if (flag)
          {
              char * pEnd;
              rf_post_message((uint32_t) strtoul(buffer, &pEnd, 16));
          }
          rf_main();

      }


	return 0;
}
