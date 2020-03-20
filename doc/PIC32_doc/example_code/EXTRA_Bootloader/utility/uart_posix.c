#if defined(__APPLE__) || defined(__linux__) || defined(__unix__)

// posix systems include linux and os x
#include <termios.h>  // for serial
#include <fcntl.h>    // for serial constants
#include <poll.h>
#include <unistd.h>   // for STDOUT_FILENO
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __APPLE__
  const char uart_sample_name[] = "/dev/tty.usbserial-DJ00DV5V"; 
#else
  const char uart_sample_name[] = "/dev/ttyUSB0"; // sample COM port on linux
#endif

#define BAUD B230400

static int port = -1; // serial port file descriptor

static struct termios old_tio; // the termios structure when the program starts

void uart_close() {
  if(port != -1) {
    // wait for all pending writes to finish, or there is a timeout
    struct pollfd events;
    memset(&events, 0, sizeof(events));
    events.fd = port;
    events.revents |= POLLOUT;

    // wait for writes to finish or for a timeout. we must wait for writes to finish
    // to reliably jump to the application, since the bootloader does not respond
    int pval = poll(&events, 1, 200);
    if(pval < 0) {
      perror("Error (on exit): could not poll for finished writes");
    } else if(pval == 0) {
      // this was a timeout.  
      struct termios tio;
      // get the current settings
      if( tcgetattr(port, &tio) != 0) {
        perror("Error (on exit): could not get port attributes");
      }
      // disable flow control
      tio.c_cflag &= ~CRTSCTS;
      if(tcsetattr(port, TCSANOW, &tio) != 0) {
        perror("Error (on exit): could not disable flow control");
      }

      // flush the buffers
      if(tcflush(port, TCIOFLUSH) != 0) {
        perror("Error (on exit): could not flush serial buffers");
      }
    }

    // restore settings to state when the program was open
    if(tcsetattr(port, TCSADRAIN, &old_tio) != 0) {
      perror("Error (on exit): could not restore serial attributes");
    }

    if(close(port) != 0) {
      perror("Error (on exit): could not close port");
    }
    port = -1;
  }
}

void uart_open(const char port_name[])  // open a serial port (using POSIX calls)
{
  // open serial port for non-blocking reads 
  port = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK); 

  if (port == -1) {
    perror("Error: could not open serial port");
    exit(EXIT_FAILURE);
  }

  if (tcgetattr(port, &old_tio) != 0) {
    perror("Error: could not get port attributes");
    exit(EXIT_FAILURE);
  }

  if(atexit(uart_close) != 0) {
    perror("Error: could not set exit function");
    exit(EXIT_FAILURE);
  }

  struct termios tio;
  memcpy(&tio, &old_tio, sizeof(tio)); // start from the original settings

  tio.c_cflag |= CS8 | CREAD | CLOCAL | CRTSCTS; // 8n1, see termios.h 

  // set raw input mode
  tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // set raw output mode
  tio.c_oflag &= ~OPOST;

  // set the baud rate
  if(cfsetispeed(&tio, BAUD) != 0) {
    perror("Error: could not set input baud");
    exit(EXIT_FAILURE);
  }

  if(cfsetospeed(&tio, BAUD) != 0) {
    perror("Error: could not set output baud");
    exit(EXIT_FAILURE);
  }

  // flush serial buffers
  if(tcflush(port, TCIOFLUSH) != 0) {
    perror("Error: could not flush serial buffers");
    exit(EXIT_FAILURE);
  }

  // set serial port options
  if(tcsetattr(port, TCSANOW, &tio) != 0) {
    perror("Error: could not set serial options");
    exit(EXIT_FAILURE);
  }

}

int uart_read(uint8_t * data, size_t length) {
  return read(port, data, length);
}

int uart_write(const uint8_t * data, size_t length) {
  return write(port, data, length);
}

#endif // end platform-specific code
