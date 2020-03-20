#include "NU32.h"         // constants, funcs for startup and UART
#include "i2c_display.h"
// Tests the OLED driver by drawing pixels

int main() {
  NU32_Startup();  // cache on, interrupts on, LED/button init, UART init
  display_init();
  int row, col;
  for(col = 0; col < WIDTH; ++col) { // draw a diagonal line
    row = col % HEIGHT;              // when we hit the last row
    display_pixel_set(row,col,1);   // start from row 0, but keep advancing
                                    // the column
    display_draw();                 // we draw every update, to display progress.
  }
  display_draw();
  
  return 0;
}
