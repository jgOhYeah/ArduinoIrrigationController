//Software Version of the slave
#define SOFTWARE_VERSION " V3.6 test"
#define MASTER_ID 255
byte myId; //44 is bay 5, 45 is bay 6... 40 is bay 1
//#define defaultBaudRate 9600 //Default baud rate to reset the value stored in eeprom to.

//#define ledFlashSpeed 500
#define LONG_PRESS_TIME 5000
#define SERIAL_TIMEOUT 600000 //10 minutes
//#define enableReset //Do not define this if this code is running on an atmega 328p running the arduino nano bootloader! (arduino uno bootloader is fine)
char bayStatus = bayUnknown;
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
unsigned long callbackTime = 0;
unsigned long timeAtDown = 0;
unsigned long timeAtOpen = 0;
#define BAY_NOTHING 0
#define BAY_HOMING 1
#define BAY_TO_HALF 2
char callbackOperation = BAY_NOTHING;

//ADDRESSES OF VALUES STORED IN EEPROM
//Longs take 4 bytes, ints take 2 and bytes take 1
#define EEPROM_ADDRESS 0
#define EEPROM_HALFWAY_POS 1
#define EEPROM_DOWN_TRAVEL_SPEED 2
#define EEPROM_UP_TRAVEL_SPEED 6
#define EEPROM_SERIAL_BAUD 10


//Leds
#define FAST_FLASH 100
#define SLOW_FLASH 1000
#define LEDS_STEADY 0
#define LEDS_FAST 1
#define LEDS_SLOW 2
#define LEDS_CHASE 3

byte flashMode = LEDS_STEADY;
byte ledStates = 0;
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
unsigned int ledFlashSpeed = FAST_FLASH;
unsigned long ledCallback = 0; //time that the led driving function needs to next change the led states

#define ALL_LEDS(a) digitalWrite(upLed,a); digitalWrite(downLed,a); digitalWrite(halfLed,a)
