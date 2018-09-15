//This is version 2.0. Version 1.0 used strings and probably
//would have crashed after a while
const char softwareVersion[] PROGMEM = " V2.0";

//Pins
#define serialEnablePin 9

//Serial
#define baudRate 1200

//Addresses 0 to numberOfDevices for individual addressing, all devices are 'a'
#define allDevices 'a'
#define numberOfDevices 7

//Commands and states
#define reportStatus 's'
#define reset 'x'
#define overwriteStatus 'w'
#define readStatus 'r'
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
#define bayNotPresent 4

//Start and end characters
#define startChar '~' //Something weird and not often used so cannot ba an accident
#define endChar 10 //Newline char

//Timeouts
#define enableLineWait 3000
#define serialRecieveTimeout 3000
#define callbackTimeout 10000

//Buttons
#define buttonRight 6
#define buttonLeft 7
#define buttonSelect 8
#define buttonTimer 300
#define buttonRefresh 300
#define buttonMode OneShotTimer

#define startDelay 2000


//Error codes not already defined - continueing on from library
#define serialAddressTooLarge 6
#define serialRxNotMatchExpected 7
#define serialInvalidCommand 8
#define serialNoReply 9

//Variables and strings
byte bayStatus[numberOfDevices];

//LCD Stuff
#define clockUpdateSpeed 5000
//Screen selection
#define mainScreen 0
#define menuScreen 1
#define errorScreen 2
#define initScreen 3
byte currentScreen = initScreen;
//Speed at which scrolls - number of chars at a time
#define scrollSkips 9
//Cursor and current position
byte cursorPos = 0;
byte cursorRow = 0;
byte scrollPos = 0;
bool allTheSame = false;
//Store the error messages
char completeErrorMsg[41] = "Nothing yet"; //Assuming the lcd can store 40 chars in its ram when it comes to printing

const byte bayOptions[] = {'?','S','H','O','X'};

//Strings
const char colon[] PROGMEM = ":"; //Want to be a string to use with strcpy
const char space[] PROGMEM = " ";
const char all[] PROGMEM = "ALL,";
const char zero[] PROGMEM = "0";
const char comma[] PROGMEM = ",";
const char bay[] PROGMEM = " Bay(s): ";
const char irrigation[] PROGMEM = "Irrigation ";
const char controller[] PROGMEM = "Controller";
const char initialising[] PROGMEM = "Initialising...";
const char crnl[] PROGMEM = "\r\n";

//Lcd Strings
const char stringHome[] PROGMEM = "*Home ";
const char stringMore[] PROGMEM = "*More";
const char stringResetAll[] PROGMEM = "*Reset all";
const char stringLastErrMsg[] PROGMEM = "*Last error msg";
const char stringError[] PROGMEM = "ERROR ";

//Error Strings
const char empty[] PROGMEM = "";
const char enableLineLow[] PROGMEM = "Enable Line Low";
const char rxTimedOut[] PROGMEM = "Rx timed out";
const char msgTooLong[] PROGMEM = "Msg Too Long";
const char checksumNotMatch[] PROGMEM = "Checksum not match";
const char addressTooLarge[] PROGMEM = "Invalid Address";
const char rxNotMatchExpected[] PROGMEM = "Rx not match expected";
const char invalidCommand[] PROGMEM = "Invalid Command";
const char noReply[] PROGMEM = "No reply";


const char* const errorMessages[] PROGMEM = {empty, enableLineLow, empty, rxTimedOut, msgTooLong, checksumNotMatch, addressTooLarge, rxNotMatchExpected, invalidCommand,noReply};

