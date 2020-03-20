// Demonstrates the harmony UART driver.
// Implements a program similar to talkingPIC.c.

#include "system_config.h"       // macros needed for this program 
#include "system_definitions.h"  // includes header files needed by the program

// UART_init, below, is of type DRV_USART_INIT, a struct.  Here we initialize uart_init.
// The fields in DRV_USART_INIT, according to framework/driver/usart/drv_usart.h, are
// .moduleInit, .usartID, .mode, etc.  Syntax below doesn't give the field names, so the
// values are assigned to fields in the order they appear in the definition of the 
// DRV_USART_INIT struct.

const static DRV_USART_INIT uart_init = {       // initialize struct with driver options
  .moduleInit = {SYS_MODULE_POWER_RUN_FULL},    // no power saving
  .usartID = USART_ID_3,                        // use UART 3   
  .mode = DRV_USART_OPERATION_MODE_NORMAL,      // use normal UART mode
  .modeData = 0,                                // not used in normal mode
  .flags = 0,                                   // no flags needed 
  .brgClock = APP_PBCLK_FREQUENCY,              // peripheral bus clock frequency
  .lineControl = DRV_USART_LINE_CONTROL_8NONE1, // 8 data bits, no parity, 1 stop bit
  .baud = 230400,                               // baud
  .handshake = DRV_USART_HANDSHAKE_FLOWCONTROL, // use flow control
                                                // remaining fields are not needed here
};

// Write a string to the UART. Does not block.  Returns true when finished writing.
int WriteUart(DRV_HANDLE handle, const char * msg);

// Read a string from the UART.  The string is ended with a '\r' or '\n'.
// If more than maxlen characters are read, data wraps around to the beginning.
// Does not block, returns true when '\r' or '\n' is encountered.
int ReadUart(DRV_HANDLE handle, char * msg, int maxlen);

#define BUF_SIZE 100

// states for our FSM
typedef enum {APP_STATE_QUERY, APP_STATE_RECEIVE, APP_STATE_ECHO} APP_STATE;  

int main(void) {
  char buffer[BUF_SIZE]; 
  APP_STATE state = APP_STATE_QUERY;  // initial state of our FSM will ask user for text
  SYS_MODULE_OBJ uart_module;        
  DRV_HANDLE uart_handle;

  // Initialize the UART.
  uart_module = DRV_USART_Initialize(DRV_USART_INDEX_0,(SYS_MODULE_INIT*)&uart_init);
  
  // Open the UART for non-blocking read/write operations.  
  uart_handle = DRV_USART_Open(
      uart_module, DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_NONBLOCKING);
  
  while (1) {
    switch(state) {
      case APP_STATE_QUERY:
        if(WriteUart(uart_handle,"\r\nWhat do you want? ")) { 
                                       // Start/continue writing to UART.
                                       // If we get here, the message has been completed.
          state = APP_STATE_RECEIVE;   // Switch to receive message state.
        }
        break;
      case APP_STATE_RECEIVE:
        if(ReadUart(uart_handle,buffer,BUF_SIZE)) { 
                                       // Start/continue reading msg from user.
                                       // If we get here, the user's message is concluded.
          state = APP_STATE_ECHO;      // Switch to echo state.
        }
        break;
      case APP_STATE_ECHO:
        if( WriteUart(uart_handle,buffer)) { 
                                       // Start/continue echoing message to UART.
                                       // If we get here, we're finished echoing.
          state = APP_STATE_QUERY;     // Switch to user query state.
        }
        break;
      default:
        ;// logic error, impossible state!
    }
    
    // Update the UART FSMs.  Since we are not using UART interrupts, the FSM
    // updating must be done in mainline code, and it should be done often.
    // Typically done at the end of the main loop, and there should be no 
    // blocking functions in the main loop.
    DRV_USART_TasksReceive(uart_module);
    DRV_USART_TasksTransmit(uart_module);
    DRV_USART_TasksError(uart_module);
  }
  return 0;
}

// WriteUart keeps track of the number of characters already sent in the most recent
// message send request.  Once it realizes the last character has been sent, it returns
// TRUE (1), indicating it is finished.  Otherwise it tries to send another byte of 
// the msg.  In any case, it returns quickly (non-blocking).

int WriteUart(DRV_HANDLE handle, const char * msg) {
  static int sent = 0;    // number of characters sent (static so saved between calls)
  if(msg[sent] == '\0') { // we are at the last string character
    sent = 0;             // reset the "sent" count for the next time
    return 1;             // finished sending message
  } else {
    // DRV_USART_Write(handle,str,numbytes) tries to add numbytes from str to the UART
    // send buffer, returning the number of bytes that were placed in the buffer, 
    // so we can keep track. Note that DRV_USART_Write takes a void *, hence the cast
    sent += DRV_USART_Write(handle,(char*)(msg + sent),1);
    return 0;
  }
}

// ReadUart reads bytes into msg.  It keeps track of the number of characters received.  
// If the number exceeds maxlen, then wraps around and begins to write to msg at 
// beginning.  Returns TRUE (1) if the entire user message has been received, or FALSE (0)
// if the end of the message has not been reached.  Regardless, it returns quickly 
// (non-blocking).

int ReadUart(DRV_HANDLE handle, char * msg, int maxlen) {
  static int recv = 0;  // number of characters received 
  int nread = 0;        // number of bytes read
  // DRV_USART_Readh(handle,str,numbytes) tries to read one byte from uart receive buffer.
  // Returns the number of bytes that were actually placed into str.  If no bytes
  // are available, then recv is unchanged.
  nread = DRV_USART_Read(handle,msg + recv, 1);
  if(nread) {           // if we have read one byte
    if(msg[recv] == '\r' || msg[recv] == '\n') {  // check for newline / carriage return
      msg[recv] = '\0'; // insert the null character
      recv = 0;         // prepare to receive another string
      return 1;         // indicate that the string is ready
    } else {
      recv += nread;
      if(recv >= maxlen) { // wrap around to the beginning
        recv = 0;
      }
      return 0;
    }
  }
  return 0;
}
