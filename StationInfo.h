/// @file StationInfo.h
///
/// @brief header file for StationInfo.ino
///  included in StationInfo.ino sketch
///



//------------Arduino Nano pin defines--------------

enum Pin : uint16_t 
{
    PIN_TFTDC   = 8, // All TFT/SPI wire need series 470R 
    PIN_TFTRST  = 9,
    PIN_TFTCS   = 10,
    
    PIN_SPIMOSI = 11, // TFT names this as SDA
    PIN_SPIMISO = 12, // not used by TFT (write only)
    PIN_SPISCLK = 13, // TFT names this as SCL
    
    PIN_I2CSDA  = A4, // TBA, I2C sync screens
    PIN_I2CSCL  = A5,
};


//--------------Screen object------------------
// clone 0.96" TFT display has inverted colour, so invert

enum Color : int16_t  // TFT colours are RGB565 encoding
{
  COLOR_RED    = ~ 0x001F, 
  COLOR_BLUE   = ~ 0xF800,
  COLOR_GREEN  = ~ 0x07E0,
  COLOR_YELLOW = ~ 0x07FF,
  COLOR_BLACK  = ~ 0x0000,
  COLOR_WHITE  = ~ 0xFFFF,
  COLOR_GREY   = ~ 0xCE59,
};


//-----------global variables----------------


uint8_t  ttIndex = 16; // train at top of the display

uint8_t  ttElements;   // number of tt elements

uint16_t ttMins;       // time of top train on display
                       // Will be >= fastClockMins +1

volatile uint16_t fastClockMins = 570; 

volatile boolean  clockFlag = false;


const uint16_t    MINUTES24HOURS = 1440;

const uint8_t SECONDSPERMINUTE = 5; // Fast clock speed



// build empty timetable array tt[]

struct tt_struct 
{
  uint16_t minute;
  uint8_t  platform;
  uint8_t  message;
};
struct tt_struct  tt[ 50 ]; // 4 byte struct of 50 elements



uint16_t getMinsFromHrMin( uint8_t hrs, uint8_t mins )
{
  return( hrs * 60 ) + mins; // see buildTimeTable() below
}



//----------End of File---------------
