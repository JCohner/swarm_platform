#include "i2c_master.h"
#include "gpio.h"
#include "uart.h"
//#include <devices/cc13x0/inc/hw_i2c.h>
//#include <devices/cc13x0/driverlib/i2c.h>

void InitI2C0()
{
    //    WriteUART0("in this bih\n\r");
        //from documentation, turn on clock and power
    PRCMPeripheralRunEnable(PRCM_PERIPH_I2C0);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );
    //    WriteUART0("periph is heavy\n\r");

    uint32_t ui32IOConfig;
    //IO Init
//    IOCIOPortIdSet(IOID_4, IOC_PORT_MCU_I2C_MSSCL);
//    IOCIOPortIdSet(IOID_5, IOC_PORT_MCU_I2C_MSSDA);
    ui32IOConfig = IOC_CURRENT_2MA | IOC_STRENGTH_AUTO | IOC_IOPULL_UP |
                          IOC_SLEW_DISABLE | IOC_HYST_DISABLE | IOC_NO_EDGE |
                          IOC_INT_DISABLE | IOC_IOMODE_OPEN_DRAIN_NORMAL |
                          IOC_NO_WAKE_UP | IOC_INPUT_ENABLE;
    IOCPortConfigureSet(IOID_4, IOC_PORT_MCU_I2C_MSSCL,ui32IOConfig);
    IOCPortConfigureSet(IOID_5, IOC_PORT_MCU_I2C_MSSDA,ui32IOConfig);
//    IOCPinTypeI2c(I2C0_BASE, IOID_5, IOID_4); I2C runs but runs incorrectly with this line




    //from driver example
    I2CMasterDisable(I2C0_BASE);
    I2CMasterInitExpClk(I2C0_BASE, SysCtrlClockGet(), false);
    I2CMasterIntClear(I2C0_BASE);
    I2CMasterEnable(I2C0_BASE);
//    WriteUART0("HIII\n\r");

}
void I2CSend(uint8_t slave_addr, uint8_t reg, uint8_t value)
{
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
    I2CMasterDataPut(I2C0_BASE, reg);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));
    I2CMasterDataPut(I2C0_BASE, value);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE));
    return;
}

////read specified register on slave device
void I2CReceive(uint32_t slave_addr, uint8_t reg, uint32_t * data, uint8_t num_data)
{
    //specify that we are writing (a register address) to the
    //slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

    if (num_data > 1)
    {
        //specify register to be read
        I2CMasterDataPut(I2C0_BASE, reg | 0x80);
    }
    else
    {
        //specify register to be read
        I2CMasterDataPut(I2C0_BASE, reg);
    }


    //send control byte and register address byte to slave device
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START); //keep repeeated start

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true); // begin reading

    if (num_data == 1)
    {
        //if this doesnt work use continue
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        while(I2CMasterBusy(I2C0_BASE));
        data[0] = I2CMasterDataGet(I2C0_BASE);
    }
    else
    {
        int i;
        for (i = 0; i < num_data; i++)
        {
            if(i == 0){
                I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
            }else if ((num_data - i) == 1)
            {
                I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
            }
            else
            {
                I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            }

            //wait for MCU to finish transaction
            while(I2CMasterBusy(I2C0_BASE));

            data[i] = I2CMasterDataGet(I2C0_BASE);
        }
    }

    //return data pulled from the specified register
    return;
}
