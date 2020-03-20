#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

// avoid superfluous warnings when building harmony
#define _PLIB_UNSUPPORTED

// USB driver configuration

// work as a USB device not as a host
#define DRV_USB_DEVICE_SUPPORT      true
#define DRV_USB_HOST_SUPPORT        false

// use only one instance of the usb driver
#define DRV_USB_INSTANCES_NUMBER    1


// operate using usb interrupts
#define DRV_USB_INTERRUPT_MODE      true

// there are 2 usb endpoints
#define DRV_USB_ENDPOINTS_NUMBER    2

// USB device configuration
// use only one device layer instance
#define USB_DEVICE_INSTANCES_NUMBER     1

// size of the endpoint 0 buffer, in bytes
#define USB_DEVICE_EP0_BUFFER_SIZE      8

// enable the USB start of frame event. it happens at 1 ms intervals
#define USB_DEVICE_SOF_EVENT_ENABLE

// USB HID configuration
// use only one instance of the hid driver
#define USB_DEVICE_HID_INSTANCES_NUMBER     1

// total size of the hid read and write queues
#define USB_DEVICE_HID_QUEUE_DEPTH_COMBINED 2

// ports used by NU32 LEDs and USER button
#define NU32_LED_CHANNEL PORT_CHANNEL_F
#define NU32_USER_CHANNEL PORT_CHANNEL_D

// positions of the LEDs and user buttons
#define NU32_LED1_POS PORTS_BIT_POS_0
#define NU32_LED2_POS PORTS_BIT_POS_1
#define NU32_USER_POS PORTS_BIT_POS_7

// macros used by hid.c
#define NU32_PID 0x1769     // usb product id

#define NU32_REPORT_SIZE 28 // hid report is 28 bytes long

// name of the device. first byte is the length, next byte is the string descriptor id 
// (always 3), then the following characters are two bytes each, spelling "Talking HID"
#define NU32_DEVICE_NAME "\x18\x03T\0a\0l\0k\0i\0n\0g\0 \0H\0I\0D\0"
#define NU32_HID_SUBCLASS USB_HID_SUBCLASS_CODE_NO_SUBCLASS
#define NU32_HID_PROTOCOL USB_HID_PROTOCOL_CODE_NONE

#endif 
