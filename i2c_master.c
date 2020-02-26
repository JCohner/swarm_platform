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
void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...)
{
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

    //stores list of variable number of arguments
    va_list vargs;

    //specifies the va_list to "open" and the last fixed argument
    //so vargs knows where to start looking
    va_start(vargs, num_of_args);

    //put data to be sent into FIFO
    I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint8_t));

    //if there is only one argument, we only need to use the
    //single send I2C function
    if(num_of_args == 1)
    {
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_RLED);
        //Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));

        //"close" variable argument list
        va_end(vargs);
    }

    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));

        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        uint8_t i;
        for(i = 1; i < (num_of_args - 1); i++)
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
            //send next data that was just placed into FIFO
            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

            // Wait until MCU is done transferring.
            while(I2CMasterBusy(I2C0_BASE));
        }

        //put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
        //send next data that was just placed into FIFO
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));

        //"close" variable args list
        va_end(vargs);
    }
}

//read specified register on slave device
uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg)
{
    //specify that we are writing (a register address) to the
    //slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

    //specify register to be read
    I2CMasterDataPut(I2C0_BASE, reg);

    //send control byte and register address byte to slave device
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND); //switch back to burst send

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true);

    //send control byte and read from the register we
    //specified
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //return data pulled from the specified register
    return I2CMasterDataGet(I2C0_BASE);
}
