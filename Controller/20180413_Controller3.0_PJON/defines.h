//This is version 3.0. Version 1.0 used strings and probably
//would have crashed after a while
const char softwareVersion[] PROGMEM = " V3.0";

//Pins
#define serialEnablePin 9

//Serial
#define baudRate 9600

//Addresses 0 to numberOfDevices for individual addressing, all devices are 'a'
#define masterAddress 255
#define numberOfDevices 7
#define firstSlaveAddress 40
#define lastSlaveAddress firstSlaveAddress + numberOfDevices - 1

//Commands and states
#define reportStatus 's' //String so can be sent as a command by istelf
#define reset 'x'
#define overwriteStatus 'w'
#define setStatus 'r'
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
#define bayNotPresent 4

//Buttons
#define buttonRight 6
#define buttonLeft 7
#define buttonSelect 8
#define btnOffTime 150 //Minimum amount of time between presses
#define buttonPressedState LOW //Which way the pin is pulled when it is pressed - must be low if pullups used.
#define pullupsEnabled false //Enable the internal pullup resistors

#define startDelay 2000
#define replyDelay 3000

//Error codes
#define invalidAddress 50
#define packetError 51

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
const char bay[] PROGMEM = "Bays ";
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
const char lengthString[] PROGMEM = "Length: ";
const char contentTooLong[] PROGMEM = "Msg too long.";
const char bufferTooLong[] PROGMEM = "Packet Buffer full.";
const char noReply[] PROGMEM = " Disconnected";
const char unknownError[] PROGMEM = "Unkown Error: ";
const char packetErrorString[] PROGMEM = "Packet Error, ID: ";
const char invalidAddressString[] PROGMEM = "Invalid Address: ";

//const char* const errorMessages[] PROGMEM = {empty, enableLineLow, empty, rxTimedOut, msgTooLong, checksumNotMatch, addressTooLarge, rxNotMatchExpected, invalidCommand,noReply};

