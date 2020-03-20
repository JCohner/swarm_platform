// uart code that will only be compiled on windows
#if (defined(_WIN32)) || (defined(_WIN64)) // we are on windows

#include <windows.h>                     // for serial and time functions
#include <stdio.h>

const char uart_sample_name[] = "COM5";  // sample COM port on windows

static HANDLE port = INVALID_HANDLE_VALUE;

void uart_open(const char port_name[])  // open a serial port (using the windows API)
{
  port = CreateFile(port_name,
                    GENERIC_READ | GENERIC_WRITE, // read and write access
                    0,         // other processes cannot access the port
                    NULL,          // no security settings
                    OPEN_EXISTING, // open only if it exists
                    FILE_ATTRIBUTE_NORMAL, // non overlapped
                    NULL);

  if (port == INVALID_HANDLE_VALUE) { // the function failed
    printf("Invalid serial port.\n");
    exit(EXIT_FAILURE);
  }

  // set NU32 serial port parameters - 230400 baud, 8-1-n, RTS and CTS
  DCB dcbSerialParams = {0}; // parameters are zero by default
  dcbSerialParams.DCBlength = sizeof(DCB);

  // get the current (default) configuration
  if (!GetCommState(port, &dcbSerialParams)) {
    printf("Could not get serial port state.\n");
    exit(EXIT_FAILURE);
  }

  dcbSerialParams.BaudRate = 230400;      // set the baud rate
  dcbSerialParams.fBinary = TRUE;         // must be true
  dcbSerialParams.fParity = FALSE;        // no parity checking
  dcbSerialParams.ByteSize = 8;           // 8 bits in a byte
  dcbSerialParams.StopBits = ONESTOPBIT;  // one stop bit
  dcbSerialParams.Parity = NOPARITY;      // no parity
  dcbSerialParams.fRtsControl = RTS_CONTROL_HANDSHAKE; // use RTS flow control
  dcbSerialParams.fOutxCtsFlow = TRUE;                 // use CTS flow control
  if(!SetCommState(port, &dcbSerialParams)){
    printf("Error configuring serial port.\n");
    exit(EXIT_FAILURE);
  }
  // play with these if you get errors (numbers too small)
  // or if performance is too slow (numbers too big)
  COMMTIMEOUTS timeouts={0};
  timeouts.ReadIntervalTimeout = MAXDWORD; // return immediately with bytes available
  timeouts.ReadTotalTimeoutMultiplier = 0; 
  timeouts.ReadTotalTimeoutConstant = 0; 
  timeouts.WriteTotalTimeoutMultiplier = 2; // 2 ms per byte
  timeouts.WriteTotalTimeoutConstant = 100; // an additional 100ms total
  if(!SetCommTimeouts(port, &timeouts)) {
    printf("Error setting serial timeouts.\n");
    exit(EXIT_FAILURE);
  }
}

// serial read function, mimics the interface to POSIX (e.g., linux/mac) read 
int uart_read(byte * data , size_t length) {
  DWORD nread = 0;
  if(!ReadFile(port, data, length, &nread, NULL)) {
    return -1;
  }
  return nread;
}

  // serial write function, mimics interface to POSIX (e.g., linux/mac) write function
int uart_write(const byte * data, size_t length) {
  DWORD nwritten = 0;
  if(!WriteFile(port, data, length, &nwritten, NULL)){
    return -1;
  }
  return nwritten;
}

#endif
