

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
static char buffer[200];
int main(void)
{

    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL);
    while (PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH | PRCM_DOMAIN_SERIAL)
    != PRCM_DOMAIN_POWER_ON);

    InitGPIO();
    GPIO_setDio(CC1310_LAUNCHXL_PIN_GLED);

//    InitUART0();
//    InitI2C0();


      start();
      callibrate();
//      GPIO_setDio(CC1310_LAUNCHXL_PIN_RLED);
      uint32_t curr_time = 0;
      uint32_t prev_time = 0;
      uint32_t freq = SysCtrlClockGet();
      int counter = 0;

      while(1)
      {
          sprintf(buffer,"%d\r\n", counter);
          WriteUART0(buffer);
////          sprintf(buffer, "%u, %u\n", (unsigned int)ACC_WhoAmI(),(unsigned int)GYRO_WhoAmI());
////          WriteUART0(buffer);
//////          read_imu();
////             WriteUART0("sup min  dude");
////          I2CSend(0x08, 1, 0x07);
////          sprintf(buffer,"%u\r\n", I2CMasterErr(I2C0_BASE));
////          WriteUART0(buffer);
//          I2CMasterSlaveAddrSet(I2C0_BASE, 0x1D, false);
//          I2CMasterDataPut(I2C0_BASE, (uint8_t)0x0F);
//          I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START); //need to issue repeated start
////          // Wait until MCU is done transferring.
////          WriteUART0("bingo bongo\n");
//          while(I2CMasterBusy(I2C0_BASE));
//          GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);
////          WriteUART0("window wando\n");
//          I2CMasterSlaveAddrSet(I2C0_BASE, 0x1D, true);
//          I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
////          // Wait until MCU is done transferring.
//          while(I2CMasterBusy(I2C0_BASE));
////          I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
////          // Wait until MCU is done transferring.
//          uint32_t data = I2CMasterDataGet(I2C0_BASE);
//          sprintf(buffer,"i heardL %u\r\n", data);
//          WriteUART0(buffer);
//          WriteUART0("shplingo shplongo");
          read_imu();
          while ((curr_time - prev_time) < 1000000){
              ++curr_time;
          }
          curr_time = 0;
          ++counter;
      }


	return 0;
}
