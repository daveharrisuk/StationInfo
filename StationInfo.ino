/// @file StationInfo.ino
///
/// @brief Display a timetable on Station Info TFT screen
///
/// @author (c) Dave Harris
/// @date started Aug-2019
///
/// @date last edit 19-Apr-2022
/// @version 0.61
///
  
const char ABOUT[] = "StationInfo\n(c)Dave Harris v0.61 Apr22\n";

// IDE:   Arduino v1.8.13
// Board: Arduino Nano v3 (use Old Bootloader)
// uPC:   ATmega329P
// IDE Serial: 9600 baud. Disgnostic messages.
// Screen: 0.96" TFT 80x160 65k colour, SPI, ST7735S chip, china clone.
//           VCC pin is 3.3V from Nano

/* Compile time stats...
Sketch uses 17240 bytes (56%) of program storage space.
Maximum is 30720 bytes.
Global variables use 750 bytes (36%) of dynamic memory
, leaving 1298 bytes for local variables. Maximum is 2048 bytes.
*/


// local project files

#include "StationInfo.h"    // data defines

#include "TTdata.h"         // Timetable data


// Libraries

#include<SPI.h>             // 1.0   \arduino\avr\libraries\SPI

#include<Adafruit_GFX.h>    // 1.10.10 github.com/adafruit/Adafruit-GFX-Library

#include<Adafruit_ST7735.h> // 1.9.3  github.com/adafruit/Adafruit-ST7735-Library


Adafruit_ST7735 tft = Adafruit_ST7735( PIN_TFTCS, PIN_TFTDC, PIN_TFTRST );



//-------------------------------------------------------------------
/// @brief getMessageString
///  Extract string from PROGMEM string store
///
/// @param index [in]  message table index
/// @return pointer to string
///

char * getMessageString( uint8_t index )
{ 
  static char msg[25];

  strcpy_P( msg, ( ( char* )pgm_read_word( & mTable[index] ) ) );
  return  msg;
}



//-------------------------------------------------------------------
/// @brief getHHMMfromMins
///  Convert time of day minutes to time string
/// 
/// @param mins [in] Time of day in minutes. 1439 is 23:59
/// @return string pointer
///

char * getHHMMfromMins( uint16_t mins ) 
{
  static char t[ 6 ]; // HH:MM + end null = 6 char
  
  sprintf( t, "%02d:%02d", (mins/60), (mins%60) );
  return t;
}



//-------------------------------------------------------------------
/// @brief isTTindexUpdated
///  Scan the TT to see if update needed 
///
/// @param none
/// @return true if updated
///

boolean isTTindexUpdated() 
{
  for( uint8_t indx = 0; indx <= ttElements; indx++ )
  {
    if( tt[ indx ].minute > fastClockMins )
    {
      if( indx != ttIndex )
      {
        Serial.print( indx, DEC );
        Serial.print("indx");
        Serial.println( getHHMMfromMins( tt[indx].minute ) );

        ttIndex = indx;
        return true;
      }
      else 
      {
         return false;
      }
    }
  }
  ttIndex = 0;
  Serial.println("0indx");
  return true;
}



//-------------------------------------------------------------------
/// @brief displayStatus
///  Display status text at bottom row of TFT screen
///
/// @param text [in] String array of 25 chars
/// @param colour [in] The RGB565 colour code
/// @return none
///

void displayStatus( String text, uint16_t colour ) 
{
  tft.fillRect( 2, tft.height() -9, tft.width(), 9, COLOR_BLUE ); // blank area
  
  tft.setTextColor( colour ); 

  tft.setCursor( 20, tft.height() -9 );
  tft.print( text );
}




//-------------------------------------------------------------------
/// @brief displayTTrow
///  Display one row of timetable on screen
///
/// @param row [in] The row number, 0 to 5
/// @param index [in] The index into timetable
/// @return none
///

void displayTTrow( uint8_t row, uint8_t index ) 
{
  uint8_t  y = 19 + ( row * 10 ); // calculate row vertical position

  tft.fillRect( 0, y, tft.width(), 10, COLOR_BLUE ); // blank area
  
  tft.setTextColor( COLOR_WHITE );
  
  tft.setCursor( 4, y ); // time
  tft.print( getHHMMfromMins( tt[index].minute ) );
 
  tft.setCursor( 39, y ); // platform number
  tft.print( tt[index].platform, DEC );
  
  tft.setCursor( 49, y ); // description
  tft.print( getMessageString( tt[index].message ) );
}




//-------------------------------------------------------------------
/// @brief DisplayTimeTable
///  Display 5 rows of time table, starting at ttIndex
///
/// @param  none
/// @return none
///

void displayTimeTable() 
{
  const uint8_t NUMROWS = 5;
  
  ttMins = tt[ ttIndex ].minute; // top of display time
   
  uint8_t index = ttIndex;
   
  for( uint8_t row = 0; row < NUMROWS; row++ )
  {
    displayTTrow( row, index );
    
    if( ++index >= ttElements )
    {
      index = 0;
    }
  }
}




//-------------------------------------------------------------------
/// @brief displayTimeOfDay
///  Display time of day clock at top right of screen
///
/// @param  none
/// @return none
///

void displayTimeOfDay()
{
  tft.setTextColor( COLOR_WHITE, COLOR_BLUE );
  
  tft.setCursor( tft.width() -32, 5 );
  tft.print( getHHMMfromMins( fastClockMins ) );
}



//-------------------------------------------------------------------
/// @brief displayLogo
///  Display DB logo on screen
///  
/// @param  none
/// @return none
///

void displayLogo()
{
  tft.fillRect( 105, 3, 16, 11, COLOR_RED );
  tft.drawRoundRect( 105, 2, 17, 12, 1, COLOR_WHITE );
  tft.setTextColor( COLOR_WHITE ); 
  tft.setCursor( 108, 4 );
  tft.print( "DB" );  
}



//-------------------------------------------------------------------
/// @brief displayHeader
///  Display header row on screen
///
///@param  none
///@return none
///

void displayHeader() 
{
  tft.setCursor( 5, 5 );
  tft.print( getMessageString( M_HEADER) );
  
  tft.drawFastHLine( 2, 16, tft.width(), COLOR_GREY );  
  tft.drawFastHLine( 2, tft.height() -11, tft.width(), COLOR_GREY );
}



//-------------------------------------------------------------------
/// @brief initScreen
///  Initialise tft screen and layout
///
/// @param  none
/// @return none
///

void initScreen() 
{
  tft.initR( INITR_MINI160x80 );

  tft.fillScreen( COLOR_BLUE ); // blank screen
  tft.setRotation( 3 );
  
  tft.setTextSize( 1 );
  tft.setTextWrap( false );
  
  tft.setTextColor( COLOR_WHITE );
  tft.setCursor( 4, 30 );
  tft.println( ABOUT ); // title, author, version
  tft.print("DataVersion=");
  tft.print( dataVersion );
  
  delay( 1000 );

  tft.fillScreen( COLOR_BLUE );
  tft.setTextWrap( false );
     
  displayHeader();
  displayLogo();
  displayTimeTable(); 
  displayStatus( getMessageString( M_NODELAYS ), COLOR_GREEN );
}


//-------------------------------------------------------------------
/// @brief setup
///  first call at boot time
///
/// @param  none
/// @return none
///

void setup() 
{
  Serial.begin( 9600 );
  Serial.print( ABOUT );
  Serial.print("DataVersion=");
  Serial.println( dataVersion );


  noInterrupts();   // stop interrupts for timer setup
  {                     // Timer1 is 16 bit timer              
    TCCR1A = B00000000; // all 0 = no toggling pins
    TCCR1B = B00001100; // bit2 is clk div 256, so 16MHz/256 = 62.5KHz
    TIMSK1 = B00000010; // bit1 is interrupt on OCR1A match
    OCR1A  = 62500;     // comparator, one second per tick
  }
  interrupts();     // restart interrupts

  ttElements = buildTimeTable();

  Serial.print("tt index ");
  Serial.println( ttElements, DEC );

  initScreen();
}



//-------------------------------------------------------------------
/// @brief ISR TIMER1_COMPA_vect
///  Timer1 compare match interrupt service routine, each second
///
/// @param  [in] InterruptVector
/// @return none
///

ISR( TIMER1_COMPA_vect )
{
  static uint8_t countSeconds; // one second clock tick

  if( ++countSeconds > SECONDSPERMINUTE ) // Fast clock tick?
  {
    clockFlag = true;    // global clockflag is reset in loop()
    countSeconds = 0;

    if( ++fastClockMins >= MINUTES24HOURS ) // past 23:59?
    {
      fastClockMins = 0; // reset Fast clock 
    }
  }
}



//-------------------------------------------------------------------
/// @brief loop
///  called after setup
///
/// @params none
/// @return none
///

void loop() 
{
  if( clockFlag ) // clockFlag set by ISR
  {
    clockFlag = false;
    displayTimeOfDay();
    
    if( isTTindexUpdated() )
    {
      displayTimeTable();
    }
  }
}


//---------------end of file--------------------
