//Software Version of the slave
#define SOFTWARE_VERSION " V3.6.1"
#define MASTER_ID 255
byte myId; //44 is bay 5, 45 is bay 6... 40 is bay 1
//#define DEFAULT_BAUD_RATE 9600 //Default baud rate to CMD_RESET the value stored in eeprom to.
//#define ledFlashSpeed 500
#define LONG_PRESS_TIME 5000
#define SERIAL_TIMEOUT 600000 //10 minutes
//#define ENABLE_RESET //Do not define this if this code is running on an atmega 328p running the arduino nano bootloader! (arduino uno bootloader is fine)
char bayStatus = STATE_UNKOWN;
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define MAX_BAY_TIME 43383508 //(2^32-1)/99 is the largest number you can have before it possibly overflows
#define MIN_SERIAL_BAUD 50
#define MAX_SERIAL_BAUD 2000000
byte halfwayPos; //25 Percent from the bottom
unsigned long downTravelSpeed; //Default 26000
unsigned long upTravelSpeed; //Default 24000

unsigned long halfUpTime; //Will be worked out using values from EEPROM on start
unsigned long halfDownTime;

unsigned long startDelayTime = 0;
unsigned long timeAtDown = 0;
unsigned long timeAtOpen = 0;

//ADDRESSES OF VALUES STORED IN EEPROM
//Longs take 4 bytes, ints take 2 and bytes take 1
#define EEPROM_ADDRESS 0
#define EEPROM_HALFWAY_POS 1
#define EEPROM_DOWN_TRAVEL_SPEED 2
#define EEPROM_UP_TRAVEL_SPEED 6
#define EEPROM_SERIAL_BAUD 10


//Leds
#define MAX_CALLBACKS 5 //Increase it a bit to be safe
#define FAST_FLASH_RATE 100
#define SLOW_FLASH_RATE 1000

#define LEDS_STEADY 0
#define LEDS_FAST 1
#define LEDS_SLOW 2
#define LEDS_CHASE 3

byte ledStates = 0;
byte ledSavedStates = 0;
#define ERR_CALLBACK_BUFFER_FULL 255 //Because this bit of code is before the library is included
byte ledCallback = NONEXISTANT_CALLBACK; //Set the index to be inactive so it doesn;t cancel a callback as it starts.
byte ledSpecialCallback = NONEXISTANT_CALLBACK; //Only used for special occasions.
byte bayMovedCallback = NONEXISTANT_CALLBACK;
bool isUndervoltage = false;
/* ledStates structure in binary:
 * ZZYYYXXX
 * Where:
 *   ZZ is the mode the leds should be in.
 *   YYY is the current state of each led (up, half, down in that order - 0 = off, 1 = on) - also the initial state
 *   XXX is if the led state should be changed when updated. - In chasing mode this is also the stage that the sequence is up to.
 * Examples:
 *   ZZ YYY XXX
 *   00 100 000 = The LEDs on steady, Up led starting on, both others off, do not change the state at any time.
 *   01 010 110 = The LEDs flashing fast, Up and Down LEDs starting off while the Half LED starting on, Only update the Up and Half led when it is time to change their state (Down led stays off)
 *   10 111 111 = Slow flash, All LEDs starting on, all LEDs flashing.
 *   11 111 111 = Chasing - All LEDs involved.
*/

#define ALL_LEDS(a) digitalWrite(PIN_UP_LED,a); digitalWrite(PIN_DOWN_LED,a); digitalWrite(PIN_HALF_LED,a)

//Debugging macros to only enable debugging certain parts if debug is defined.
#if defined(DEBUG_CALLBACK_LIBRARY_ENABLE) && defined(DEBUG_ENABLE) //Used for the callbacks library to send serial data.
  #define DEBUG_CALLBACKS
#endif

#if defined(DEBUG_EEPROM_ENABLE) && defined(DEBUG_ENABLE) //Used to debug reading and saving longs to EEPROM
  #define DEBUG_EEPROM(MESSAGE) Serial.print(MESSAGE)
  #define DEBUG_EEPROM_LN(MESSAGE) Serial.println(MESSAGE)
#else
  #define DEBUG_EEPROM(MESSAGE)
  #define DEBUG_EEPROM_LN(MESSAGE)
#endif
#if defined(DEBUG_MOTORS_ENABLE) && defined(DEBUG_ENABLE) //Used to debug the current motor state
  #define DEBUG_MOTORS(MESSAGE) Serial.print(MESSAGE)
  #define DEBUG_MOTORS_LN(MESSAGE) Serial.println(MESSAGE)
#else
  #define DEBUG_MOTORS(MESSAGE)
  #define DEBUG_MOTORS_LN(MESSAGE)
#endif
#if defined(DEBUG_PJON_ENABLE) && defined(DEBUG_ENABLE) //Used to debug PJON error messages
  #define DEBUG_PJON(MESSAGE) Serial.print(MESSAGE)
  #define DEBUG_PJON_LN(MESSAGE) Serial.println(MESSAGE)
#else
  #define DEBUG_PJON(MESSAGE)
  #define DEBUG_PJON_LN(MESSAGE)
#endif
#if defined(DEBUG_UI_ENABLE) && defined(DEBUG_ENABLE) //Used to debug Led flashing...
  #define DEBUG_UI(MESSAGE) Serial.print(MESSAGE)
  #define DEBUG_UI_LN(MESSAGE) Serial.println(MESSAGE)
#else
  #define DEBUG_UI(MESSAGE)
  #define DEBUG_UI_LN(MESSAGE)
#endif
#if defined(DEBUG_ERRORS_ENABLE) && defined(DEBUG_ENABLE) //Used to debug Led flashing...
  #define DEBUG_ERRORS(MESSAGE) Serial.print(MESSAGE)
  #define DEBUG_ERRORS_LN(MESSAGE) Serial.println(MESSAGE)
#else
  #define DEBUG_ERRORS(MESSAGE)
  #define DEBUG_ERRORS_LN(MESSAGE)
#endif
#if defined(DEBUG_HOUSEKEEPING_ENABLE) && defined(DEBUG_ENABLE) //Used to debug voltage, time, state...
  #define DEBUG_HOUSEKEEPING(MESSAGE) Serial.print(MESSAGE)
  #define DEBUG_HOUSEKEEPING_LN(MESSAGE) Serial.println(MESSAGE)
#else
  #define DEBUG_HOUSEKEEPING(MESSAGE)
  #define DEBUG_HOUSEKEEPING_LN(MESSAGE)
#endif
