#include <stdio.h>
#include <stddef.h>
#include "hidapi.h"

// Client that talks to the HID device
// using hidapi, from www.signal11.us.
// hidapi allows you to directly communicate with hid devices
// without needing a special driver.
// To use hidapi you first must compile the library.
// You need its header to be on your path and you need to
// link against the library.  The procedure for doing this
// varies by platform.
// Note: error checking code omitted for clarity
#define REPORT_LEN 65 // 64 bytes per report plus report ID
#define MAX_STR 255   // max length for a descriptor string

int main(void) {
  char outbuf[REPORT_LEN] = "";
  char inbuf[REPORT_LEN] = ""; 
  wchar_t wstr[MAX_STR] = L""; // use 2-character "wide chars" for USB string descriptors
  hid_device *handle = NULL;

  // open the hid device using the VID and PID
  handle = hid_open(0x4d8, 0x1769, NULL);
  printf("Opened HID device.\n");

  // use blocking mode so hid_read will wait for data before returning
  hid_set_nonblocking(handle, 0);

  // get the manufacturer string
  hid_get_manufacturer_string(handle, wstr, MAX_STR);
  printf("Manufacturer String: %ls\n", wstr); // the ls is to print a wide string

  // get the product string
  hid_get_product_string(handle, wstr, MAX_STR);
  printf("Product String: %ls\n", wstr);

  while(1) {
    printf("Say something to PIC (blank to exit): ");
    // get string of max length REPORT_LEN-1 from user
    // first byte is the report id (always 0)
    fgets(outbuf + 1, REPORT_LEN - 1, stdin); 
    if(outbuf[1] == '\n') { // if blank line, exit
      break;
    }
    hid_write(handle, (unsigned char *)outbuf, REPORT_LEN);// send report to the device

    // read the pic's reply, wait for bytes to actually be read
    while(hid_read(handle, (unsigned char *)inbuf, REPORT_LEN) == 0) {
      ; // (on some platforms hid_read returns 0 even in blocking mode, hence the loop)
    }
    printf("\nPIC Replies: %s\n", inbuf); 
  }
  return 0;
}
