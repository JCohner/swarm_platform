#ifndef UART_H
#define UART_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
// generic interface to a uart, implemented with different uart_<platform>.c files

// read at most len bytes from the uart and return the number of bytes read 
// or -1 if no data is available
int uart_read(uint8_t * data, size_t len);

// write the data to uart and return the number of bytes written or -1 on error
int uart_write(const uint8_t * data, size_t len);

// open the uart, any failure will exit(1)
void uart_open(const char port_name[]); 

// close the serial port. only needed as a workaround in posix, since the ports
// are closed when the program exits
void uart_close();

// sample name for a com port on the given platform
extern const char uart_sample_name[];
#endif
