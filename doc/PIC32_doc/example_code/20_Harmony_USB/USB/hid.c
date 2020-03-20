#include "hid.h"
#include "system_config.h"
#include "system_definitions.h"
#include <sys/attribs.h>

// the USB device descriptor, part of the usb standard.
const USB_DEVICE_DESCRIPTOR device_descriptor = {
    0x12,                      // the descriptor size, in bytes
    USB_DESCRIPTOR_DEVICE,     // 0x01, indicating that this is a device descriptor
    0x0200,                    // usb version 2.0, BCD format of AABC == version AA.B.C
    0x00,                      // Class code 0, class will be in configuration descriptor
    0x00,                      // subclass 0, subclass will be in configuration descriptor
    0x00,                      // protocol unused, it is in the configuration descriptor
    USB_DEVICE_EP0_BUFFER_SIZE,// max size for packets to control endpoint (endpoint 0)
    0x04d8,                    // Microchip's vendor id, assigned by usb-if 
    NU32_PID,                  // product id (do not conflict with existing pid's)
    0x0000,                    // device release number
    0x01,                   // string descriptor index; string describes the manufacturer
    0x02,                   // product name string index
    0x00,                   // serial number string index, 0 to indicate not used
    0x01                    // only one possible configuration
};

// Configuration descriptor, from the USB standard.
// All configuration descriptors are stored contiguously in memory in
// this byte array.  Remember, the pic32's CPU is little endian.
const uint8_t configuration_descriptor[] = {
    // configuration descriptor header
    0x09,                           // descriptor is 9 bytes long
    USB_DESCRIPTOR_CONFIGURATION,   // 0x02, this is a configuration descriptor
    41,0, // total length of all descriptors is 41 bytes (remember, little endian)
    1,                           // configuration has only 1 interface
    1,                           // configuration value (host uses this to select config)
    0,                           // configuration string index, 0 indicates not used
    USB_ATTRIBUTE_DEFAULT | USB_ATTRIBUTE_SELF_POWERED,  // device is self-powered
    50,                                          // max power needed 100 mA (2 mA units)

    // interface descriptor
    0x09,                           // descriptor is 9 bytes long
    USB_DESCRIPTOR_INTERFACE,       // 0x04, this is an interface descriptor
    0,                              // interface number 0
    0,                              // interface 0 in the alternate configuration
    2,                              // 2 endpoints (not including endpoint 0)
    USB_HID_CLASS_CODE,             // uses the hid class
    NU32_HID_SUBCLASS,              // hid boot interface subclass, in system_config.h
    NU32_HID_PROTOCOL,              // hid protoocol, defined in system_config.h
    0,                              // no string for this interface

    // the hid class descriptor
    0x09,                           // descriptor is 9 bytes long
    USB_HID_DESCRIPTOR_TYPES_HID,   // 0x21 indicating that this is a HID descriptor
    0x11, 0x01,                     // use HID version 1.11, (BCD format, little endian)
    0x00,                           // no country code
    0x1,                            // Number of class descriptors, including this one
    // as part of the hid descriptor, class descriptors follow (only one for this example)
    USB_HID_DESCRIPTOR_TYPES_REPORT,// this is a report descriptor
    sizeof(HID_REPORT),0x00,        // size of the report descriptor

    // in endpoint descriptors
    0x07,                           // the descriptor is 7 bytes long
    USB_DESCRIPTOR_ENDPOINT,        // 0x05, endpoint descriptor type
    0x1 | USB_EP_DIRECTION_IN,      // in to host direction, address 1
    USB_TRANSFER_TYPE_INTERRUPT,    // use interrupt transfers
    0x40, 0x00,                     // maximum packet size, 64 bytes
    0x01,                           // sampling interval 1 frame count

    // out endpoint descriptor
    0x07,                           // the descriptor is 7 bytes long
    USB_DESCRIPTOR_ENDPOINT,        // 0x05, endpoint descriptor type
    0x1 | USB_EP_DIRECTION_OUT,     // in to host direction, address 1
    USB_TRANSFER_TYPE_INTERRUPT,    // use interrupt transfers
    0x40, 0x00,                     // maximum packet size, 64 bytes
    0x01                            // sampling interval 1 frame count
};

// String descriptor table.  String descriptors provide human readable information 
// to the hosts.
// The syntax \xRR inserts a byte with value 0xRR into the string.
// As per the USB standard, the first byte is the total length of the descriptor
// the next byte is the descriptor type, (0x03 for string descriptor). The following bytes
// are the string itself.  Since each character is two bytes, we insert a \0 after 
// every character.  The descriptors are placed into a table for use with harmony.
const USB_DEVICE_STRING_DESCRIPTORS_TABLE string_descriptors[] = {
  // 1st byte:  length of string (0x04 = 4 bytes)
  // 2nd byte:  string descriptor (3)
  // 3rd and 4th byte:  language code, 0x0409 for English (remember, little endian) 
  "\x04\x03\x09\x04", 
  // manufacturer string: Microchip Technology Inc.
  "\x34\x03M\0i\0c\0r\0o\0c\0h\0i\0p\0 \0T\0e\0c\0h\0n\0o\0l\0o\0g\0y\0 \0I\0n\0c\0.\0", 
 // name of the device, defined in system_config.h
  NU32_DEVICE_NAME                                
};

//  512-byte-aligned table needed by the harmony device layer
static uint8_t __attribute__((aligned(512))) 
  endpoint_table[USB_DEVICE_ENDPOINT_TABLE_SIZE];

// harmony structure for storing the configuration descriptors.
// a device can have multiple configurations but only one can be active at one time
// we have only one configuration
const USB_DEVICE_CONFIGURATION_DESCRIPTORS_TABLE configuration_table[]= { 
  configuration_descriptor };

// table of descriptors used by the harmony USB device layer
const USB_DEVICE_MASTER_DESCRIPTOR master_descriptor = {
    &device_descriptor,       // Full speed descriptor
    1,                        // Total number of full speed configurations available
    configuration_table,      // Pointer to array of full speed configurations descriptors
    NULL, 0, NULL,            // usb high speed info, high speed not supported on PIC32MX
    3,                        // Total number of string descriptors available
    string_descriptors,       // Pointer to array of string descriptors
    NULL, NULL, NULL          // unsupported features, should be NULL
};

// harmony HID initialization structure
const USB_DEVICE_HID_INIT hid_init = {
  sizeof(HID_REPORT), // size of the hid report descriptor
  &HID_REPORT,        // the hid report descriptor
  1,1                 // send and receive queues of 1 byte each
};

// register hid functions with the Harmony device layer
const USB_DEVICE_FUNCTION_REGISTRATION_TABLE function_table[] = {
  {
    USB_SPEED_FULL,                        // full speed mode
    1,                                     // use configuration number 1
    0,                                     // use interface 0 of configuration number 1
    1,                                     // only one interface is used
    0,                                     // use instance 0 of the usb function driver
    (void*)&hid_init,                      // the initialization for the driver
    (void*)USB_DEVICE_HID_FUNCTION_DRIVER, // use the HID function layer
  }
};

// used to initialize the device layer
const USB_DEVICE_INIT usb_device_init = {
  {SYS_MODULE_POWER_RUN_FULL}, // power state
  USB_ID_1,                    // use usb module 1 (PLIB USB_ID to use)
  false,  false,               // don't stop in idle or suspend in sleep modes
  INT_SOURCE_USB_1, 0,         // use usb 1 interrupt, not using dma so set source to 0
  endpoint_table,              // the endpoint table
  1,                           // only one function driver is registered
  (USB_DEVICE_FUNCTION_REGISTRATION_TABLE*)function_table, // function drivers for HID 
  (USB_DEVICE_MASTER_DESCRIPTOR*)&master_descriptor,       // all of the descriptors
  USB_SPEED_FULL,              // use usb full speed mode
  //1,1                        // endpoint read/write queues of 1 byte each
};

volatile SYS_MODULE_OBJ usb;  // handle to the usb device middleware

// maintains the status of the usb system, based on the callback events responses
typedef struct {
  bool configured;          // true if the device is configured
  bool sent;                // true if the device report has been sent
  uint16_t idle_rate;       // how often a report should be sent, in 4 ms units
  uint32_t time;            // time in ms, based on usb clock
  unsigned int idle_count;  // the idle count, in 1 ms ticks
  bool received;            // true if a report has been received
  USB_DEVICE_HANDLE device; // harmony device handle
} usb_status;

// the initial status of the device
static usb_status status = {false,false,0,0,0,false,USB_DEVICE_HANDLE_INVALID};

// prototypes for usb event handling functions
static void usb_device_handler(
    USB_DEVICE_EVENT event, void * eventData, uintptr_t context);

static void usb_hid_handler(
    USB_DEVICE_HID_INDEX hidInstance, USB_DEVICE_HID_EVENT event, 
    void * eventData, uintptr_t userData);

void __ISR(_USB_1_VECTOR, IPL4SOFT) USB1_Interrupt(void)
{
  // update the USB state machine
    USB_DEVICE_Tasks_ISR(usb);
}

void hid_setup(void) {
    // set the USB ISR priority
    SYS_INT_VectorPrioritySet(INT_VECTOR_USB1, INT_PRIORITY_LEVEL4);

    // initialize the usb device middleware
    usb = USB_DEVICE_Initialize(USB_DEVICE_INDEX_0, (SYS_MODULE_INIT*)&usb_device_init);
}

bool hid_open(void) {
  // attempt to open the usb device
  status.device = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);

  // if the device is successfully opened
  if(status.device != USB_DEVICE_HANDLE_INVALID) {
    // register a callback for USB device events
    USB_DEVICE_EventHandlerSet(status.device,  usb_device_handler, 0);
    return true;
  } // otherwise opening failed, but this is not usually an error, 
    // we just need to wait more iterations until the USB system is ready
  return false;
}

// update the usb state machine, should be called from the main loop
void hid_update() {
  USB_DEVICE_Tasks(usb);
}

bool hid_receive(uint8_t report[], int length) {
  USB_DEVICE_HID_TRANSFER_HANDLE handle;
  static bool requested = false; // true if we have requested a report
  if(status.configured) {        // the device is configured and plugged in
    if(!requested) {             // have not already requested a report
      requested = true;          // request the report
      status.received = false;   // not received the report yet 
                                 // the next line issues the recieve request. When it 
                                 // completes,  usb_hid_handler will be called with
                                 // event = USB_DEVICE_HID_EVENT_REPORT_RECEIVED
      USB_DEVICE_HID_ReportReceive(USB_DEVICE_HID_INDEX_0, &handle,report,length);
    } 
    if(status.received) {        // requested report has been received
      requested  = false;        // ready for a new receive request
      return true;               // indicate that the report is ready
    }
  }
  return false;                  // requested report is not ready
}

// send a hid report, if we are not busy sending, otherwise return false
bool hid_send(uint8_t report[], int length) {
  USB_DEVICE_HID_TRANSFER_HANDLE handle;
  static bool requested = false;
  if(status.configured) {     // the device is configured and plugged in
    if(!requested) {          // have not requested a hid report to be sent
      requested = true;       // issue the hid report send request
      status.sent = false;    // request has not been sent
      status.idle_count = 0;  // sending a report so reset the idle count
                              // the next line issues the send request. When it 
                              // completes, usb_hid_handler will be called with
                              // event = USB_DEVICE_HID_EVENT_REPORT_SENT
      USB_DEVICE_HID_ReportSend(USB_DEVICE_HID_INDEX_0, &handle, report, length);
    } 
    if(status.sent) {         // finished a send request
      requested = false;      // ready for a new send request
      return true;            // indicate that the report has been sent
    }
  } 
  return false;               // send request is not finished
}

uint32_t hid_time(void) {
  // get a time count in ms ticks from the usb subsystem
  return status.time;
}

bool hid_idle_expired(void) {
 return (status.idle_rate > 0 && status.idle_count*4 >= status.idle_rate);
}

// handles HID events, reported by the Harmony HID layer
static void usb_hid_handler(
    USB_DEVICE_HID_INDEX hidInstance, 
    USB_DEVICE_HID_EVENT event, void * eventData, uintptr_t context)
{
  static uint16_t protocol = 0; // store the protocol
  static uint8_t blank_report[sizeof(HID_REPORT)] = ""; // a blank report to return 
                                                        // if requested
  switch(event)
  {
    case USB_DEVICE_HID_EVENT_REPORT_SENT:
      // we have finished sending a report to the host
      status.sent = true;
      break;
    case USB_DEVICE_HID_EVENT_REPORT_RECEIVED:
      // the host has sent a report to us.  Ignore zero length reports
      status.received = true;
      break;
    case USB_DEVICE_HID_EVENT_GET_REPORT:
      // send blank report when requested. Per HID spec, we must send a report when asked
      USB_DEVICE_ControlSend(status.device,blank_report,sizeof(blank_report));
      break;
    case USB_DEVICE_HID_EVENT_SET_IDLE:
      // acknowledge the receipt of the set idle request
      USB_DEVICE_ControlStatus(status.device, USB_DEVICE_CONTROL_STATUS_OK);
      // set new idle rate, in units of 4 ms. report must be sent before period expires
      status.idle_rate = ((USB_DEVICE_HID_EVENT_DATA_SET_IDLE*)eventData)->duration;
      break;
    case USB_DEVICE_HID_EVENT_GET_IDLE:
        // send the idle rate to the host
        USB_DEVICE_ControlSend(status.device, &status.idle_rate,1);
        break;
    case USB_DEVICE_HID_EVENT_SET_PROTOCOL:
      // all usb hid devices that support the boot protocol must implement SET_PROTOCOL &
      // GET_PROTOCOL which allows the host to select between the boot protocol and the 
      // report descriptor we made. our operation remains the same regardless so we just 
      // store the request and return it when asked
      protocol = ((USB_DEVICE_HID_EVENT_DATA_SET_PROTOCOL*)eventData)->protocolCode;
      USB_DEVICE_ControlStatus(status.device, USB_DEVICE_CONTROL_STATUS_OK);
      break;
    case  USB_DEVICE_HID_EVENT_GET_PROTOCOL:
      // return the currently selected protocol to the host
      USB_DEVICE_ControlSend(status.device, &protocol,1);
      break;
    default:
      break; // many other events we simply don't handle
  }
}

// handles USB device events, reported by the Harmony device layer
static void usb_device_handler(
    USB_DEVICE_EVENT event, void * eventData, uintptr_t context) {
  switch(event) {
    case USB_DEVICE_EVENT_SOF:
      // this event occurs at the USB start of frame, every 1 ms per the usb spec
      // the event is enabled by defining USB_DEVICE_EVENT_SOF_ENABLE in system_config.h
      ++status.idle_count; // keep track of how long device has not sent reports
      ++status.time;       // also keep a running time, in ms
      break;
    case USB_DEVICE_EVENT_RESET:
      // usb bus was reset
      status.configured = false;
      break;
    case USB_DEVICE_EVENT_DECONFIGURED:
      // device was deconfigured
      status.configured = false;
      break;
    case USB_DEVICE_EVENT_CONFIGURED:
      // we have been configured.  eventData holds the selected configuration, 
      // but this device has only have one configuration.
      // we can now register a hid event handler
      USB_DEVICE_HID_EventHandlerSet(USB_DEVICE_HID_INDEX_0, usb_hid_handler, 0);
      status.configured = true;
      break;
    case USB_DEVICE_EVENT_POWER_DETECTED:
      // Vbus is detected meaning the device is attached to a host
      USB_DEVICE_Attach(status.device);
      break;
    case USB_DEVICE_EVENT_POWER_REMOVED:
      // the device was removed from a host
      USB_DEVICE_Detach(status.device);
      break;
    default:
      break; // there are other events that we do not handle
  }
}

// The USB device layer, when it initializes the driver layer,
// attempts to call this function, but Harmony does not implement it as of
// v1.06.  Therefore we place it here
void DRV_USB_Tasks_ISR_DMA(SYS_MODULE_OBJ o)
{
  DRV_USB_Tasks_ISR(o);
}
