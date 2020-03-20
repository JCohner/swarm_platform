#ifndef HID__H__
#define HID__H__
// code common to all hid examples

#include <stdbool.h>      // bool type with true, false
#include <stdint.h>       // uint8_t
#include "system_config.h"

// following harmony's lead you must define the following variables and macros
// macros in system_config.h
// #define NU32_PID - the product ID for the device
// #define NU32_REPORT_SIZE  the size of the hid report
// #define NU32_DEVICE_NAME  the name of the device in usb string descriptor format
// #define NU32_HID_SUBCLASS the hid subclass
// #define NU32_HID_PROTOCOL the hid protocol
// in one of your .c files you must also define the HID_REPORT
const extern uint8_t HID_REPORT[NU32_REPORT_SIZE]; // the hid report

// initialize the hid device 
void hid_setup(void);

// attempt to open the hid device, return true when 
// the keyboard is successfully opened
bool hid_open(void);

// request a hid report from the host. when the report is available, return true
// returning false indicates that the current request is pending
bool hid_receive(uint8_t report[], int length);

// send a hid report.  return true when finished sending
// returning false indicates that the current request is pending
bool hid_send(uint8_t report[], int length); 

// update the necessary harmony state machines, call from the main loop
void hid_update(void);

// return true if the idle time has expired, indicating that the host expects a report
bool hid_idle_expired(void);

// get the time, in ms, based on the usb clock
uint32_t hid_time(void);

#endif
