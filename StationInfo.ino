// StationInfo.ino
//---------------------------------------------------------------
  
#define ABOUT "StationInfo.ino\n(c)Dave Harris v0.2 Sep19\nBuild "__DATE__" "__TIME__ 

// IDE:   Arduino v1.8.9
// Board: Arduino Nano 3
// uPC:   ATmega329P (Old Bootloader)
// IDE Serial: 9600 baud. Disgnostic messages.
// Screen: 0.96" TFT 80x160 RGB565 colour, SPI, with ST7735S controller (china clone)


/* Compile time stats...
Sketch uses 16724 bytes (54%) of program storage space. Maximum is 30720 bytes.
Global variables use 803 bytes (39%) of dynamic memory, leaving 1245 bytes for local variables.
*/

#include "StationInfo.h"  // message array, mTable[] and timetable array, tt[]


// Libraries

#include <SPI.h>             // 1.0    \arduino\avr\libraries\SPI 
#include <Adafruit_GFX.h>    // 1.5.6  https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_ST7735.h> // 1.4.3  https://github.com/adafruit/Adafruit-ST7735-Library



//------Arduino Nano / AtMega328P pin defines-------------

// Pins to or from ST7735S screen [0.96"80x160(RGB)IPS] RGB565 16K colour
#define TFT_DC   8
#define TFT_RST  9
#define TFT_CS  10
// h/w SPI  SDA 11
// h/w SPI  SCL 13
// VCC on 3.3V


//---------Screen object----------------

Adafruit_ST7735 tft = Adafruit_ST7735( TFT_CS, TFT_DC, TFT_RST );

// Clone screen colours vary from library defines. Red & Blue are swapped!
// Redefined...
#define C_RED    0x001F
#define C_BLUE   0xF800
#define C_GREEN  0x07E0
#define C_YELLOW 0x07FF
#define C_BLACK  0x0000
#define C_WHITE  0xFFFF
#define C_GREY   0xCE59
// lightgrey 0x7BEF


//-----------global variables----------------


uint8_t ttIndex = 16; // train at top of the display

uint8_t ttElements;   // number of tt elements

uint16_t ttMins;      // time of top train on display
                      // Will be >= fastClockMins +1

volatile uint16_t fastClockMins = 570; 
volatile boolean  clockFlag = false;


#define MINUTES24HOURS  1440




//---------------getMessageString-------------------------

char * getMessageString( byte index ) {  // See StationInfo.h
  
  //params...
  // index: table index
  //return: string from the message table

  static char msg[25];

  strcpy_P( msg, ( ( char* )pgm_read_word( & mTable[index] ) ) );
  return msg;
}


//-------------getMinsFromHHMM-------------------

uint16_t getMinsFromHHMM( char hhmm[] ) {

  // Convert time string to time of day minutes
  
  //params...
  // hhmm: time of day "hh:mm" string, 24 hour clock
  //return: time of day in minutes

  uint16_t mins = ( hhmm[ 0 ] - '0' ) * 600; // tens hour
  mins += ( ( hhmm[ 1 ] - '0' ) * 60 );     // hour 
  mins += ( ( hhmm[ 3 ] - '0' ) * 10 );    // tens minute
  mins += ( hhmm[ 4 ] - '0' );            // minute

  return mins;
}


//------------getHHMMfromMins------------------

char * getHHMMfromMins( uint16_t mins ) {

  // Convert time of day minutes to time string
  
  //params...
  // mins: time of day in minutes. 0 is 00:00, 1439 is 23:59
  //return: string

  static char t[6]; // HH:MM +null = 6 char
  
  sprintf( t, "%02d:%02d", mins / 60, mins % 60 );
  return t;
}


//-------------updateTTindex--------------------

boolean updateTTindex( void ) {

  // 

  //params none, returns true if updated

  for( uint8_t index = 0; index <= ttElements; index++ ) {

    if( tt[index].minute > fastClockMins ) {
      
      if( index != ttIndex ) {

        Serial.print( index, DEC );
        Serial.print( " index TT=" );
        Serial.println( getHHMMfromMins( tt[index].minute ) );

        ttIndex = index;
        return true;
      }
      else {
         
         return false;
      }
    }
  }
  ttIndex = 0;
  Serial.println( ".0 index TT=" );
  Serial.println( getHHMMfromMins( tt[0].minute ) );
  
  return true;
}


//-------------displayStatus---------------------

void displayStatus( String text, uint16_t colour ) {

  // Display status text at bottom of tft screen

  //params...
  // text:   string of 25 chars
  // colour: RGB565 code
  //return: none

  tft.fillRect( 0, tft.height() -9, tft.width(), 9, C_BLUE ); // blank area
  
  tft.setTextColor( colour ); 

  tft.setCursor( 20, tft.height() -9 );
  tft.print( text );
}


//-------------displayTTrow----------------------

void displayTTrow( uint8_t row, uint8_t index ) {

  // Display one row of timetable on screen

  //params...
  // row:   row number, 0 to 5
  // index: index to timetable
  //return: none

  uint8_t  y = 17 + ( row * 10 ); // calculate row vertical position

  tft.fillRect( 0, y, tft.width(), 10, C_BLUE ); // blank area
  
  tft.setTextColor( C_WHITE );
  
  tft.setCursor( 1, y ); // time
  tft.print( getHHMMfromMins( tt[index].minute ) );
 
  tft.setCursor( 35, y ); // platform number
  tft.print( tt[index].platform, DEC );
  
  tft.setCursor( 45, y ); // description
  tft.print( getMessageString( tt[index].message ) );
}


//-------------DisplayTimeTable-----------------

void displayTimeTable( void ) {

  // Display 5 rows of time table, starting at ttIndex

  //params none, returns nothing

  #define NUMROWS 5
  
  ttMins = tt[ttIndex].minute; // top of display time
   
  uint8_t index = ttIndex;
   
  for( uint8_t row = 0; row < NUMROWS; row++ ) {
    
    displayTTrow( row, index );
    
    if( ++index >= ttElements ) {
      
      index = 0;
    }
  }
}


//-------------displayTimeOfDay------------------

void displayTimeOfDay( void ) {

  //Display time of day clock at top right of tft screen
   
  //params none, returns nothing.

  tft.setTextColor( C_WHITE, C_BLUE );
  
  tft.setCursor( tft.width() -32, 3 );
  tft.print( getHHMMfromMins(  fastClockMins ) );
}

//--------------displayHeader-----------------------

void displayHeader( void ) {

  //Display header row on screen
  
  //params none, returns nothing.

  tft.setCursor( 3, 3 );
  tft.print( getMessageString( M_HEADER) );
  
  tft.drawFastHLine( 0, 12, tft.width(), C_GREY );  
  tft.drawFastHLine( 0, tft.height() -13, tft.width(), C_GREY );

  tft.fillRect( 105, 1, 16, 10, C_RED ); // DB logo
  tft.drawRoundRect( 105, 0, 17, 11, 1, C_WHITE );
  tft.setTextColor( C_WHITE ); 
  tft.setCursor( 108, 2 );
  tft.print( "DB" );

  tft.setCursor( tft.width() -32, 3 );
  tft.print( "hh:mm" );
}


//--------------initScreen-----------------------

void initScreen( void ) {

  // initialise tft screen and layout
  
  //params none, returns nothing.

  tft.initR( INITR_MINI160x80 );

  tft.fillScreen( C_BLUE ); // blank screen
  tft.setRotation( 3 );
  tft.setTextSize( 1 );
  tft.setTextWrap( false );
  
  tft.setTextColor( C_WHITE );
  tft.setCursor( 0, 30 );
  tft.print( ABOUT ); // title, author, version
  delay( 1000 );

  tft.fillScreen( C_BLUE );
  tft.setTextWrap( false );
  tft.setTextColor( C_WHITE );
     
  displayHeader();
  
  displayTimeTable(); 
  
  displayStatus( getMessageString( M_NODELAYS ), C_GREEN );
}


//-------------------setup--------------------------------------

void setup( void ) {

  // Arduino calls this first at boot time
  
  //params none, returns nothing.

  Serial.begin( 9600 );
  Serial.println( ABOUT );

  ttElements = buildTimeTable();
  
  Serial.print( ttElements, DEC );
  Serial.print( " timetable elements loaded, max size=" );
  Serial.println( sizeof( tt ) / 4, DEC );


  noInterrupts(); // stop interrupts for timer setup
  
                      // Timer1 is only 16 bit timer
  TCCR1A = B00000000; // all 0 = no toggling pins
  TCCR1B = B00001100; // bit2 is clk div 256, so 16MHz/256 = 62.5KHz
  TIMSK1 = B00000010; // bit1 is interrupt on OCR1A match
  OCR1A  = 62500;     // comparator, one second per tick

  interrupts(); // restart interrupts

  
  initScreen();
}


//-------Timer1 compare match interrupt service routine---------

ISR( TIMER1_COMPA_vect ) {

  // Interrupt each second

  //params none, returns nothing.

  static uint8_t  countSeconds; // one second clock tick

  if( ++countSeconds > SECONDSPERMINUTE ) { // Fast clock tick?
    
    countSeconds = 0;

    if( ++fastClockMins >= MINUTES24HOURS ) { // past 23:59?
      
      fastClockMins = 0; // reset Fast clock 
    }
    clockFlag = true;
  }
}


//--------------loop----------------------------------------------------

void loop( void ) {
  
  //Arduino calls after setup() finishes. Is re-called when ends.
  
  //params none, returns nothing.

  if( clockFlag ) { // clockFlag set by ISR

    clockFlag = false;
    displayTimeOfDay();
    
    if( updateTTindex() ) {
      
      displayTimeTable();
    }
    Serial.println( getHHMMfromMins(  fastClockMins ) );
  }
}


//---------------end of file--------------------
