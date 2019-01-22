//Store the error messages
char completeErrorMsg[41] = "Nothing yet"; //Assuming the lcd can store 40 chars in its ram when it comes to printing
char charBuffer[41]; //Some parts of the program do not like this as a local variable for some reason, so declaring it as global and lots of bits sharing
char charBuffer2[11]; //Some parts of the program do not like this as a local variable for some reason, so declaring it as global and lots of bits sharing

const byte bayOptions[] = {'?','S','H','O','X'};
#define LCD_BACK_BUTTON "\x7F"   //"\x7F" is '<-' Arrow on the lcd, otherwise use "*"
#define LCD_NORMAL_BUTTON "\x7E" //"\x7E" is '->' Arrow on the lcd, otherwise use "*"
#define LCD_ALL_BAYS_DEFAULT_BTN '*' //NOTE single char (') used instead of string ("). The default symbol for the change all bay button
#define LCD_DISABLED_BAY_SYMBOL '-' //Signle char - used for blocking off the first few bays if not yet installed
//Strings
const char colon[] PROGMEM = ":"; //Want to be a string to use with strcpy
const char space[] PROGMEM = " ";
const char all[] PROGMEM = "ALL,";
const char zero[] PROGMEM = "0";
const char comma[] PROGMEM = ",";
const char bay[] PROGMEM = "Bay";
const char sSpace[] PROGMEM = "s ";
const char irrigation[] PROGMEM = "Irrigation ";
const char controller[] PROGMEM = "Controller";
const char initialising[] PROGMEM = "Initialising...";
const char stringWaiting[] PROGMEM = "Waiting...";
const char crnl[] PROGMEM = "\r\n";

//Lcd Strings
const char leftButtonSymbol[] PROGMEM = "< ";
const char rightButtonSymbol[] PROGMEM = " >";
#define TEMP_STRING_BACK LCD_BACK_BUTTON "Back "
const char stringBack[] PROGMEM = TEMP_STRING_BACK;
#define TEMP_STRING_MORE LCD_NORMAL_BUTTON "More"
const char stringMore[] PROGMEM = TEMP_STRING_MORE;
#define TEMP_STRING_RESET_ALL LCD_NORMAL_BUTTON "Reset all"
const char stringResetAll[] PROGMEM = TEMP_STRING_RESET_ALL;
#define TEMP_STRING_ERR_MSG LCD_NORMAL_BUTTON "Err msg "
const char stringLastErrMsg[] PROGMEM = TEMP_STRING_ERR_MSG;
#define TEMP_STRING_SETUP LCD_NORMAL_BUTTON "Setup"
const char stringSettings[] PROGMEM = TEMP_STRING_SETUP;
const char stringSelectBay[] PROGMEM = "SELECT";
const char stringBay[] PROGMEM = " BAY";
const char stringToEdit[]PROGMEM = "TO ";
const char stringEdit[] PROGMEM = "EDIT";
const char stringError[] PROGMEM = "ERROR ";
const char stringUp[] PROGMEM = "Up";
const char stringDown[] PROGMEM = "Down";
const char stringTime[] PROGMEM = " Time";
const char stringHalfPos[] PROGMEM = "Half pos";
const char stringBaud[] PROGMEM = "Baud";
const char stringRate[] PROGMEM = " Rate";

#ifdef LCD_LEGEND
const char stringBayNum[] PROGMEM = "Bay #:";
const char stringBayState[] PROGMEM = "State:";
const char stringUpTime[] PROGMEM = "Run Time: " ;
#endif
#if LCD_WEBSITE==LCD_INIT || LCD_WEBSITE==LCD_ABOUT
  #if LCD_WIDTH>=40 //Different urls and prompts for different sizes of screen
  const char stringWebsite[] PROGMEM = "See \"https://git.io/fhcaU\" for more info";
  #elif LCD_WIDTH>=24
  const char stringWebsite[] PROGMEM = "See https://git.io/fhcaU";
  #elif LCD_WIDTH>=20
  const char stringWebsite[] PROGMEM = "https://git.io/fhcaU";
  #else //16 by 2 displays - don't display url if anything smaller
  const char stringWebsite[] PROGMEM = "git.io/fhcaU";
  #endif
  
  //const char stringWebsite[] PROGMEM = "https://github.com/jgOhYeah/ArduinoIrrigationController" //Too big - full url
#endif
//Error Strings
const char stringPJON[] PROGMEM = "PJON";
const char stringErrorCode[] PROGMEM = " Err, Code: ";
const char noReply[] PROGMEM = " Disconnected";
const char stringUnkown[] PROGMEM = "Unkown";
const char stringPacket[] PROGMEM = "Packet";
const char stringData[] PROGMEM = " Data: ";

