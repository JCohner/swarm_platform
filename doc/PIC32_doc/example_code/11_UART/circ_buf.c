#include "NU32.h" // constants, functions for startup and UART
// uses a circular buffer to stream data from an ISR over the UART
// notice that the buffer can be much smaller than the total number of samples sent and
// that data starts streaming immediately unlike with batch.c

#define BUFLEN   1024                             // length of the buffer
#define NSAMPLES 5000                             // number of samples to collect

static volatile int data_buf[BUFLEN];             // array that stores the data
static volatile unsigned int read = 0, write = 0; // circular buf indexes
static volatile int start = 0;                    // set to start recording

int buffer_empty() {    // return true if the buffer is empty (read = write)
  return read == write; 
}

int buffer_full() {     // return true if the buffer is full.  
  return (write + 1) % BUFLEN == read; 
}

int buffer_read() {     // reads from current buffer location; assumes buffer not empty
  int val = data_buf[read];
  ++read;               // increments read index
  if(read >= BUFLEN) {  // wraps the read index around if necessary
    read = 0;
  }
  return val;
}

void buffer_write(int data) { // add an element to the buffer.  
  if(!buffer_full()) {        // if the buffer is full the data is lost
    data_buf[write] = data;
    ++write;                  // increment the write index and wrap around if necessary
    if(write >= BUFLEN) {
      write = 0;
    }
  }
}
  
void __ISR(_TIMER_1_VECTOR, IPL5SOFT) Timer1ISR(void) {  // timer 1 isr operates at 5 kHz
  static int i = 0;       // the data returned from the isr
  if(start) {
    buffer_write(i);      // add the data to the buffer
    ++i;                  // modify the data (here we just increment it as an example)
  }
  IFS0bits.T1IF = 0;      // clear interrupt flag
}

int main(void) {
  int sent = 0;
  char msg[100] = {};
  NU32_Startup();                   // cache on, interrupts on, LED/button init, UART init

  __builtin_disable_interrupts();   // INT step 2: disable interrupts at CPU
  T1CONbits.TCKPS = 0b01;           // PBCLK prescaler value of 1:8
  PR1 = 1999;                   // The frequency is 80 MHz / (8 * (1999 + 1)) = 5 kHz
  TMR1 = 0;       
  IPC1bits.T1IP = 5;              // interrupt priority 5
  IFS0bits.T1IF = 0;              // clear the interrupt flag
  IEC0bits.T1IE = 1;              // enable the interrupt
  T1CONbits.ON  = 1;              // turn the timer on                                 
  __builtin_enable_interrupts();    // INT step 7: enable interrupts at CPU

  NU32_ReadUART3(msg,sizeof(msg));  // wait for the user to press enter before continuing
  sprintf(msg, "%d\r\n", NSAMPLES); // tell the client how many samples to expect
  NU32_WriteUART3(msg);
  start = 1; 
  for(sent = 0; sent < NSAMPLES; ++sent) { // send the samples to the client
   while(buffer_empty()) { ; }             // wait for data to be in the queue
    sprintf(msg,"%d\r\n", buffer_read());  // read from the buffer, send data over uart
    NU32_WriteUART3(msg);
  }

  while(1) {
    ;
  }
  return 0;
}
