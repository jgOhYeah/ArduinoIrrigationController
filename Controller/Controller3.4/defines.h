//This is version 3.0. Version 1.0 used strings and probably
//would have crashed after a while
const char softwareVersion[] PROGMEM = " V3.5";
//Pins
#define PIN_RS485_RX 10
#define PIN_RS485_TX 9

//Serial
//#define BAUD_RATE 9600
#define BAUD_RATE 38400
#define PJON_PORT Serial1 //Set to Serial if the arduino has only one port
//Addresses 0 to NUMBER_OF_BAYS for individual addressing, all devices are 'a'
#define MASTER_ADDRESS 255
#define NUMBER_OF_BAYS 7 //7
#define FIRST_BAY_INDEX 5 //5 To allow numbers to match up with the bays when being installed not all at a time.
#define FIRST_BAY_ADDRESS 40
#define LAST_BAY_ADDRESS FIRST_BAY_ADDRESS + NUMBER_OF_BAYS - 1
//Buttons
#define PIN_BUTTON_RIGHT 6
#define PIN_BUTTON_LEFT 7
#define PIN_BUTTON_SELECT 8
#define MIN_BUTTON_OFF_TIME 150 //Minimum amount of time between presses
#define BUTTON_PRESSED_STATE LOW //Which way the pin is pulled when it is pressed - must be low if pullups used.
#define BUTTON_PULLUPS_ENABLED false //Enable the internal pullup resistors
//Stuff for the multiple pressing of the buttons - only sometimes used.
#define BUTTON_INITIAL_DELAY 1500
#define BUTTON_REPEAT_DELAY 100
bool buttonsEnabled = true;
#define BTN_ENABLE_CHECK() if(!buttonsEnabled) return

#define START_DELAY 2000
#define REPLY_DELAY 6000
char charsToSend[3]; //For some reason this cannot be a local variable.


//Variables and strings
byte bayStatus[NUMBER_OF_BAYS];

//LCD Stuff
#define LCD_WIDTH 40
#define LCD_HEIGHT 2 //Not used yet for layouts, only init.
#define LCD_LEGEND //Only define on larger screens that can fit it - legend for bay number and state
#define LCD_CLOCK_LEGEND //Up time legend for clock
#define LCD_DYNAMIC_ALL_BAYS_BTN //If all bays are the same, display 'S', 'H' or 'O', otherwise '*' if disabled, always '*' or LCD_ALL_BAYS_DEFAULT_BTN
#define LCD_WEBSITE LCD_ABOUT //Only on screens large enough - minimum of 25 chars long - set to LCD_ABOUT, LCD_INIT or NONE
#ifdef LCD_LEGEND //Offset for buttons... - length of string 
  #define LCD_LEGEND_LEFT_OFFSET 7
#else
  #define LCD_LEGEND_LEFT_OFFSET 0
#endif

#define CLOCK_UPDATE_SPEED 5000 //The time between which the clock is refreshed on the lcd
//Screen selection
#define LCD_MAIN 0
#define LCD_MENU 1
#define LCD_ERROR 2
#define LCD_INIT 3
#define LCD_SETUP 4
#define LCD_EDIT_EEPROM_1 5
#define LCD_EDIT_EEPROM_2 6
#define LCD_SET_VALUE 7
#define LCD_ABOUT 8
byte currentScreen = LCD_INIT;
byte previousScreen = LCD_MAIN;
byte screenBeforeThat = LCD_MAIN;
byte clockCallback = NONEXISTANT_CALLBACK;
//Speed at which scrolls - number of chars at a time
#define LCD_SCROLL_SKIPS 9
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

struct otherDeviceError { //Fudge more parameters into the error handler function.
  byte code;
  unsigned long data;
};

