#include "NU32.h" // constants, funcs for startup and UART
#include "accel.h"
// accelerometer/magnetometer example.  Prints the results from the sensor to the UART

int main() {
  char buffer[200];
  NU32_Startup(); // cache on, interrupts on, LED/button init, UART init
  acc_setup();

  short accels[3];   // accelerations for the 3 axes
  short mags[3];     // magnetometer readings for the 3 axes
  short temp;        // temperature reading
  while(1) {
    // read the accelerometer from all three axes 
    // the accelerometer and the pic32 are both little endian by default 
    // (the lowest address has the LSB)
    // the accelerations are 16-bit twos complement numbers, the same as a short
    acc_read_register(OUT_X_L_A, (unsigned char *)accels,6); 

    // NOTE: the accelerometer is influenced by gravity,
    // meaning that, on earth, when stationary it measures gravity as a 1g acceleration
    // You could use this information to calibrate the readings into actual units
    sprintf(buffer,"x: %d y: %d z: %d\r\n",accels[0], accels[1], accels[2]);
    NU32_WriteUART3(buffer);

    // need to read all 6 bytes in one transaction to get an update. 
    acc_read_register(OUT_X_L_M, (unsigned char *)mags, 6); 
    
    sprintf(buffer, "xmag: %d ymag: %d zmag: %d \r\n",mags[0], mags[1], mags[2]);
    NU32_WriteUART3(buffer);

    // read the temperature data.  It's a right-justified 12-bit two's complement number
    acc_read_register(TEMP_OUT_L,(unsigned char *)&temp,2);
    sprintf(buffer,"temp: %d\r\n",temp);
    NU32_WriteUART3(buffer);

    //delay
   _CP0_SET_COUNT(0);
   while(_CP0_GET_COUNT() < 40000000) { ; }
  }
}
