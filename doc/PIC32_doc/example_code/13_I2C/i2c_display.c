#include "i2c_master_int.h"
#include "i2c_display.h"
#include <stdlib.h>
// control the SSD1306 OLED display

#define DISPLAY_ADDR 0x3C

#define SIZE WIDTH*HEIGHT/8 //display size, in bytes

static unsigned char video_buffer[SIZE+1] = {0};// buffer corresponding to display pixels 
                                                // for sending over I2C. The first byte 
                                                // lets us to store the control character
static unsigned char * gddram = video_buffer + 1; // the video buffer start, excluding 
                                                  // address byte we write these pixels 
                                                  // to GDDRAM over I2C


void display_command(unsigned char cmd) {// write a command to the display
  unsigned char to_write[] = {0x00,cmd}; // 1st byte = 0 (CO = 0, DC = 0), 2nd is command
  i2c_write_read(DISPLAY_ADDR, to_write,2, NULL, 0); 
}

void display_init() {
  i2c_master_setup();     
                          // goes through the reset procedure
  display_command(0xAE);  // turn off display

  display_command(0xA8);     // set the multiplex ratio (how many rows are updated per
                             // oled driver clock) to the number of rows in the display
  display_command(HEIGHT-1); // the ratio set is the value sent+1, so subtract 1

                         // we will always write the full display on a single update.
  display_command(0x20); // set address mode
  display_command(0x00); // horizontal address mode
  display_command(0x21); // set column address
  display_command(0x00); // start at 0
  display_command(0xFF); // end at 127
                         // with this address mode, the address will go through all 
                         // the pixels and then return to the start,
                         // hence we never need to set the address again

  display_command(0x8d); // charge pump
  display_command(0x14); // turn on charge pump to create ~7 Volts needed to light pixels
  display_command(0xAF); // turn on the display
  video_buffer[0] = 0x40;// co = 0, dc =1, allows us to send data directly from video 
                         // buffer, 0x40 is the "next bytes have data" byte
}

void display_draw() {    // copies data to the gddram on the oled chip
  i2c_write_read(DISPLAY_ADDR, video_buffer, SIZE + 1, NULL, 0);
}

void display_clear() {
  memset(gddram,0,SIZE);
}

// get the position in gddram of the pixel position
static inline int pixel_pos(int row, int col) { 
  return (row/8)*WIDTH + col;
}

// get a bitmask for the actual pixel position, based on row
static inline unsigned char pixel_mask(int row) { 
  return 1 << (row % 8);
}

// invert the pixel at the given row and column
void display_pixel_set(int row, int col,int val) { 
  if(val) { 
    gddram[pixel_pos(row,col)] |= pixel_mask(row);   // set the pixel
  } else {
    gddram[pixel_pos(row,col)] &= ~pixel_mask(row);  // clear the pixel
  }
}

int display_pixel_get(int row, int col) {
  return (gddram[pixel_pos(row,col)] & pixel_mask(row)) != 0;
}

