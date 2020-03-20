#include "i2c_slave.h"
#include "NU32.h"

int main() {
  NU32_Startup();
  i2c_slave_setup(0x32); // enable the slave w/ address 0x32

  while(1) {             // the slave is handled in an interrupt in i2c_slave.c
    _nop();              // so we do nothing.
  }
  return 0;
}
