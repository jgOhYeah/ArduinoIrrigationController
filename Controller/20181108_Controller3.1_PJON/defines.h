//This is version 3.0. Version 1.0 used strings and probably
//would have crashed after a while
const char softwareVersion[] PROGMEM = " V3.1";
//Pins
#define rs485rxPin 10
#define rs485txPin 9

//Serial
//#define baudRate 9600
#define baudRate 38400
//Addresses 0 to numberOfDevices for individual addressing, all devices are 'a'
#define masterAddress 255
#define numberOfDevices 7
#define firstDevice 5 //To allow numbers to match up with the bays when being installed not all at a time.
#define firstSlaveAddress 40
#define lastSlaveAddress firstSlaveAddress + numberOfDevices - 1
//Buttons
#define buttonRight 6
#define buttonLeft 7
#define buttonSelect 8
#define btnOffTime 150 //Minimum amount of time between presses
#define buttonPressedState LOW //Which way the pin is pulled when it is pressed - must be low if pullups used.
#define pullupsEnabled false //Enable the internal pullup resistors

#define startDelay 2000
#define replyDelay 3000
char charsToSend[3]; //For some reason this cannot be a local variable.
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
#define setupScreen 4
#define editEepromScreen1 5
#define editEepromScreen2 6
#define setValueScreen 7
byte currentScreen = initScreen;
byte previousScreen = mainScreen;
byte screenBeforeThat = mainScreen;
//Speed at which scrolls - number of chars at a time
#define scrollSkips 9
//Cursor and current position
byte cursorPos = 0;
byte cursorRow = 0;
byte scrollPos = 0;
bool allTheSame = false;

//Stuff for a number entry menu - I can't think of a more efficient way then to declare as global.
//const char* const errorMessages[] PROGMEM = {empty, enableLineLow, empty, rxTimedOut, msgTooLong, checksumNotMatch, addressTooLarge, rxNotMatchExpected, invalidCommand,noReply};
void (*callbackFunctionStored)(unsigned long);
void retrieveEepromNumber(byte address,char eepromAddress,void (*callbackFunction)(unsigned long));
unsigned long minimumValue, maximumValue, currentNumber;
unsigned int valueStepSize;
byte maximumDigits, editingBay, valueBayAddress;
char valueSuffix1, valueSuffix2, valueEepromAddress;
void requestNumberInput (char * title,char suffix1,char suffix2, unsigned int stepSize, unsigned long defaultValue, unsigned long minimum, unsigned long maximum, byte numberOfDigits, void (*callbackFunction)(unsigned long));
