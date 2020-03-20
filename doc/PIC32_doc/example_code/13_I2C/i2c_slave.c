#include "NU32.h"   // constants, funcs for startup and UART
// Implements a I2C slave on I2C5 using pins SDA5 (F4) and SCL5 (F5)
// The slave returns the last two bytes the master writes

void __ISR(_I2C_5_VECTOR, IPL1SOFT) I2C5SlaveInterrupt(void) {
  static unsigned char bytes[2];    // store two received bytes 
  static int rw = 0;                // index of the bytes read/written
  if(rw == 2) {                     // reset the data index after every two bytes
    rw = 0;
  }
  // We have to check why the interrupt occurred.  Some possible causes:
  // (1) slave received its address with RW bit = 1:  read address & send data to master
  // (2) slave received its address with RW bit = 0:  read address (data will come next)
  // (3) slave received an ACK in RW = 1 mode:        send data to master
  // (4) slave received a data byte in RW = 0 mode:   store this data sent by master

  if(I2C5STATbits.D_A) {       // received data/ACK, so Case (3) or (4)
    if(I2C5STATbits.R_W) {     // Case (3):  send data to master
      I2C5TRN = bytes[rw];     // load slave's previously received data to send to master
      I2C5CONbits.SCLREL = 1;  // release the clock, allowing master to clock in data
    } else {                   // Case (4):  we have received data from the master
      bytes[rw] = I2C5RCV;     // store the received data byte
    }
    ++rw;                           
  } else {                     // the byte is an address byte, so Case (1) or (2)
    I2C5RCV;                   // read to clear I2C5RCV (we don't need our own address)
    if(I2C5STATbits.R_W) {     // Case (1):  send data to master
      I2C5TRN = bytes[rw];     // load slave's previously received data to send to master
      ++rw;
      I2C5CONbits.SCLREL = 1;  // release the clock, allowing master to clock in data
    }                          // Case (2):  do nothing more, wait for data to come
  }
  IFS1bits.I2C5SIF = 0;
}

// I2C5 slave setup (disable interrupts before calling)
void i2c_slave_setup(unsigned char addr) { 
  I2C5ADD = addr;                   // the address of the slave
  IPC8bits.I2C5IP  = 1;             // slave has interrupt priority 1
  IEC1bits.I2C5SIE = 1;             // slave interrupt is enabled
  IFS1bits.I2C5SIF = 0;             // clear the interrupt flag
  I2C5CONbits.ON   = 1;             // turn on i2c2
}
