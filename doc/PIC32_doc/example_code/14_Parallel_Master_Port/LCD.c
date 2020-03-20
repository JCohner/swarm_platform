#include "LCD.h"
#include<xc.h>    // SFR definitions from the processor header file and some other macros

#define PMABIT 10 // which PMA bit number to use

// wait for the peripheral master port (PMP) to be ready
// should be called before every read and write operation
static void waitPMP(void)
{
  while(PMMODEbits.BUSY) { ; }
}

// wait for the LCD to finish its command.
// We check this by reading from the LCD
static void waitLCD() {
  volatile unsigned char val = 0x80;
  
  // Read from the LCD until the Busy flag (BF, 7th bit) is 0
  while (val & 0x80) {
    val = LCD_Read(0);
  }
  int i = 0;
  for(i = 0; i < 50; ++i) { // slight delay
    _nop();
  }
}

// set up the parallel master port (PMP) to control the LCD 
// pins RE0 - RE7 (PMD0 - PMD7) connect to LCD pins D0 - D7
// pin RD4 (PMENB) connects to LCD pin E
// pin RD5 (PMRD/PMWR) Connects to LCD pin R/W
// pin RB13 (PMA10) Connects to RS.
// interrupts will be disabled while this function executes
void LCD_Setup() {
  int en = __builtin_disable_interrupts();  // disable interrupts, remember initial state
  
  IEC1bits.PMPIE = 0;    // disable PMP interrupts
  PMCON = 0;             // clear PMCON, like it is on reset
  PMCONbits.PTWREN = 1;  // PMENB strobe enabled
  PMCONbits.PTRDEN = 1;  // PMRD/PMWR enabled
  PMCONbits.WRSP = 1;    // Read/write strobe is active high
  PMCONbits.RDSP = 1;    // Read/write strobe is active high

  PMMODE = 0;            // clear PMMODE like it is on reset
  PMMODEbits.MODE = 0x3; // set master mode 1, which uses a single strobe

  // Set up wait states.  The LCD requires data to be held on its lines
  // for a minimum amount of time.
  // All wait states are given in peripheral bus clock
  // (PBCLK) cycles.  PBCLK of 80 MHz in our case
  // so one cycle is 1/80 MHz = 12.5 ns.
  // The timing controls asserting/clearing PMENB (RD4) which
  // is connected to the E pin of the LCD (we refer to the signal as E here)
  // The times required to wait can be found in the LCD controller's data sheet.
  // The cycle is started when reading from or writing to the PMDIN SFR.
  // Note that the wait states for writes start with minimum of 1 (except WAITE)
  // We add some extra wait states to make sure we meet the time and
  // account for variations in timing amongst different HD44780 compatible parts.
  // The timing we use here is for the KS066U which is faster than the HD44780.
  PMMODEbits.WAITB = 0x3;  // Tas in the LCD datasheet is 60 ns 
  PMMODEbits.WAITM = 0xF;  // PWeh in the data sheet is 230 ns (we don't quite meet this)
                           // If not working for your LCD you may need to reduce PBCLK
  PMMODEbits.WAITE = 0x1;  // after E is low wait Tah (10ns)

  PMAEN |= 1 << PMABIT;   // PMA is an address line
  
  PMCONbits.ON = 1;        // enable the PMP peripheral
  // perform the initialization sequence
  LCD_Function(1,0);       // 2 line mode, small font
  LCD_Display(1, 0, 0);    // Display control: display on, cursor off, blinking cursor off
  LCD_Clear();             // clear the LCD 
  LCD_Entry(1, 0);         // Cursor moves left to right. do not shift the display
  
  if(en & 0x1)             // if interrupts were enabled before, re-enable them
  {
    __builtin_enable_interrupts();
  }
}

// Clears the display and returns to the home position (0,0)
void LCD_Clear(void) {
  LCD_Write(0,0x01); //clear the whole screen
}

// Return the cursor and display to the home position (0,0)
void LCD_Home(void) {
  LCD_Write(0,0x02);
}

// Issue the LCD entry mode set command
// This tells the LCD what to do after writing a character
// id : 1 increment cursor, 0 decrement cursor
// s : 1 shift display right, 0 don't shift display
void LCD_Entry(int id, int s) {
  LCD_Write(0, 0x04 | (id << 1) | s);
}

// Issue the LCD Display command
// Changes display settings
// d : 1 display on, 0 display off
// c : 1 cursor on, 0 cursor off
// b : 1 cursor blinks, 0 cursor doesn't blink
void LCD_Display(int d, int c, int b) {
  LCD_Write(0, 0x08 | (d << 2) | (c << 1) | b);
}

// Issue the LCD display shift command
// Move the cursor or the display right or left
// sc : 0 shift cursor, 1 shift display
// rl : 0 to the left, 1 to the right
void LCD_Shift(int sc, int rl) {
  LCD_Write(0,0x1 | (sc << 3) | (rl << 2));
}

// Issue the LCD Functions set command
// This controls some LCD settings
// You may want to clear the screen after calling this
// n : 0 one line, 1 two lines
// f : 0 small font, 1 large font (only if n == 0)
void LCD_Function(int n, int f) {
  LCD_Write(0, 0x30 | (n << 3) | (f << 2));
}

// Move the cursor to the desired line and column
// Does this by issuing a DDRAM Move instruction
// line : line 0 or line 1
// col  : the desired column
void LCD_Move(int line, int col) {
  LCD_Write(0, 0x80 | (line << 6) | col);
}

// Sets the CGRAM address, used for creating custom
// characters
// addr address in the CGRAM to make current
void LCD_CMove(unsigned char addr) {
  LCD_Write(0, 0x40 | addr);
}

// Writes the character to the LCD at the current position
void LCD_WriteChar(char c) {         
  LCD_Write(1, c); 
}

// Write a string to the LCD starting at the current cursor
void LCD_WriteString(const char *str) {
  while(*str) {
    LCD_WriteChar(*str); // increment string pointer after char sent
    ++str;
  }
}

// Make val a custom character.  This only implements 
// The small font version
// val : between 0 and 7
// data : 7 character array.  The first 5 bits of each character
//        determine whether that pixel is on or off
void LCD_CustomChar(unsigned char val, const char * data) {
  int i = 0;
  for(i = 0; i < 7; ++i) {
    LCD_CMove(((val & 7) << 2) | i);
    LCD_Write(1, data[i]);
  }
}

// Write data to the LCD and wait for it to finish by checking the busy flag. 
// rs : the value of the RS signal, 0 for an instruction 1 for data
// data : the byte to send
void LCD_Write(int rs, unsigned char data) {
  waitLCD();           // wait for the LCD to be ready
  if(rs) { // 1 for data
    PMADDRSET = 1 << PMABIT;
  } else { // 0 for command
    PMADDRCLR = 1 << PMABIT; 
  }
  waitPMP();           // Wait for the PMP to be ready
  PMDIN = data;        // send the data
}

// read data from the LCD.  
// rs : the value of the RS signal 0 for instructions status, 1 for data
unsigned char LCD_Read(int rs) {
  volatile unsigned char val = 0; // volatile so 1st read doesn't get optimized away
  if(rs) { // 1 to read data
    PMADDRSET = 1 << PMABIT;
  } else { // 0 to read command status
    PMADDRCLR = 1 << PMABIT;
  }
  // from the PIC32 reference manual, you must read twice to actually get the data
  waitPMP();                  // wait for the PMP to be ready
  val = PMDIN;
  waitPMP();
  val = PMDIN;
  return val;
}
