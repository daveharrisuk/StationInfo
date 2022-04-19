/// @file TTdata.h
/// @brief StationInfo timetable data header file
///

const uint8_t dataVersion = 1; // displayed on serial monitor



// message text strings stored in program memory

//__max string length_19___|1234567890123456789|
//                         |                   |
const char m0[]  PROGMEM = "Abfahrt";           //screen header: Departures
const uint8_t M_HEADER = 0;

const char m1[]  PROGMEM = "Keine Verspatungen"; //status 1: No delays
const uint8_t M_NODELAYS = 1;

const char m2[]  PROGMEM = "Zugverspatungen!";   //status 2: Train delays
const uint8_t M_DELAYS = 2;

const char m3[]  PROGMEM = "ausser betrieb";     //Out of Service
const uint8_t M_NOSERVICE = 3;

const char m4[]  PROGMEM = "nicht einsteigen";   //Do not board
const uint8_t M_NOBOARD = 4;

const char m5[]  PROGMEM = "IC35 Bonn, Hanover";
const uint8_t M_IC35 = 5;

const char m6[]  PROGMEM = "IC34 Stuttgart";
const uint8_t M_IC34 = 6;

const char m7[]  PROGMEM = "RE23 Trier";
const uint8_t M_RE23 = 7;

const char m8[]  PROGMEM = "RE22 Koln";
const uint8_t M_RE22 = 8;

const char m9[]  PROGMEM = "RB89 Aachen";
const uint8_t M_RB89 = 9;

const char m10[] PROGMEM = "RB88 Bonn";
const uint8_t M_RB88 = 10;

const char m11[] PROGMEM = "RB11 Dortmund";
const uint8_t M_RB11 = 11;

const char m12[] PROGMEM = "RB12 Koln";
const uint8_t M_RB12 = 12;


// build message table from above text strings...

const char * const mTable[] PROGMEM =
{ m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12 };

//Access: Use getMessageString(M_RB88) to retreive a message string 10



/// @brief buildTimeTable
///
/// @param none
/// @return count of array elements

uint8_t buildTimeTable() 
{
  uint8_t  i = 0; //array index
  
    //List must be time sorted //Time, Platform, TrainName
  tt[i++] = { getMinsFromHrMin(  6,  0 ), 5, M_RE22 };
  tt[i++] = { getMinsFromHrMin(  7, 45 ), 1, M_IC35 };
  tt[i++] = { getMinsFromHrMin(  7, 59 ), 3, M_IC34 };
  tt[i++] = { getMinsFromHrMin(  8, 10 ), 2, M_NOBOARD };
  tt[i++] = { getMinsFromHrMin(  8, 32 ), 5, M_RE23 };
  tt[i++] = { getMinsFromHrMin(  9, 45 ), 4, M_RB88 };
  tt[i++] = { getMinsFromHrMin( 10,  3 ), 1, M_RB89 };
  tt[i++] = { getMinsFromHrMin( 12, 25 ), 5, M_RE22 };
  tt[i++] = { getMinsFromHrMin( 13, 45 ), 1, M_RE23 };
  tt[i++] = { getMinsFromHrMin( 13, 59 ), 3, M_IC34 };
  tt[i++] = { getMinsFromHrMin( 14, 10 ), 2, M_IC35 };
  tt[i++] = { getMinsFromHrMin( 14, 32 ), 5, M_RE23 };
  tt[i++] = { getMinsFromHrMin( 14, 45 ), 4, M_RB88 };
  tt[i++] = { getMinsFromHrMin( 15,  3 ), 1, M_RB89 };
  tt[i++] = { getMinsFromHrMin( 16, 10 ), 2, M_NOSERVICE };
  tt[i++] = { getMinsFromHrMin( 16, 32 ), 5, M_RE23 };
  tt[i++] = { getMinsFromHrMin( 17, 36 ), 4, M_RB88 };
  tt[i++] = { getMinsFromHrMin( 18, 03 ), 1, M_RB89 };
  // add further rows here...
  
  return i; // 
}

//Access: Use tt[i].minute, tt[i].platform & tt[i].message


//----------End of File---------------
