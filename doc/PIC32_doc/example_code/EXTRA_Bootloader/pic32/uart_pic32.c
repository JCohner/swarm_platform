#include "uart.h"
#include "NU32.h"
#include <xc.h>

// time to wait for a timeout, in ms
#define WRITE_TIMEOUT 500

// pic32 uart code, compatible with common/uart.h interface
// Note that the uart_open and uart_sample_name are not referenced
// in the bootloader code and so are not implemented here

// read at most len bytes from the uart and return the number of bytes read 
// or -1 if no data is available
int uart_read(uint8_t * data, size_t len) {
  int index = 0; // note: the provided len will never be greater than MAX_INT, so we can
                 // safely cast len to an int
  while(U3STAbits.URXDA && index != (int)len) {
    data[index] = U3RXREG;
    ++index;
  }
  return index == 0 ? -1 : index;
}

// write the data to uart and return the number of bytes written or -1 on error
int uart_write(const uint8_t * data, size_t len) {
  size_t index = 0;
  for(index = 0; index != len; ++index) {
    unsigned int start_time = _CP0_GET_COUNT();
    while(U3STAbits.UTXBF ) {   // wait for tx buffer to be not full. core timer freq is
                                // 2x sys clock
      if(_CP0_GET_COUNT() - start_time > (NU32_SYS_FREQ/2000) * WRITE_TIMEOUT ) {
        return index; // we timed out writing so exit from the function
      }
    }
    U3TXREG = data[index];
  }
  return len;
}

