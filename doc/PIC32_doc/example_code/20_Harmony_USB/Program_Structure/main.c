#include <stddef.h>                   // defines NULL
#include "system/common/sys_module.h" // SYS_Initialize and SYS_Tasks prototypes

int main(void) {
 
  SYS_Initialize(NULL); // initializes the system

  while(1) {
    SYS_Tasks();        // updates the state machines of polled harmony modules
  }
  
  return 0;
}
