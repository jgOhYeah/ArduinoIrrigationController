//Software Version of the slave
#define softwareVersion " V3.5 test"
#define masterId 255
byte myId; //44 is bay 5, 45 is bay 6... 40 is bay 1
//#define defaultBaudRate 9600 //Default baud rate to reset the value stored in eeprom to.

#define ledFlashSpeed 500
#define longPressTime 5000
#define serialTimeout 600000 //10 minutes
//#define enableReset //Do not define this if this code is running on an atmega 328p running the arduino nano bootloader! (arduino uno bootloader is fine)
char bayStatus = bayUnknown;
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define maxBayTime 43383508 //(2^32-1)/99 is the largest number you can have before it possibly overflows
#define minSerialBaud 50
#define maxSerialBaud 2000000
byte halfwayPos; //25 Percent from the bottom
unsigned long downTravelSpeed; //Default 26000
unsigned long upTravelSpeed; //Default 24000

unsigned long halfUpTime; //Will be worked out using values from EEPROM on start
unsigned long halfDownTime;

unsigned long startDelayTime = 0;
unsigned long callbackTime = 0;
unsigned long timeAtDown = 0;
unsigned long timeAtOpen = 0;
#define bayNothing 0
#define bayHoming 1
#define bayToHalf 2
char callbackOperation = bayNothing;

//ADDRESSES OF VALUES STORED IN EEPROM
//Longs take 4 bytes, ints take 2 and bytes take 1
#define addressAddress 0
#define halfwayPosAddress 1
#define downTravelSpeedAddress 2
#define upTravelSpeedAddress 6
#define serialBaudAddress 10


//Leds
#define fastFlash 100
#define slowFlash 1000
#define ledsSteady 0
#define ledsFast 1
#define ledsSlow 2
#define ledsChase 3

byte flashMode = ledsOn;
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
unsigned int ledFlashSpeed = fastFlash;
unsigned long ledCallback = 0; //time that the led driving function needs to next change the led states

#define ALL_LEDS(a) digitalWrite(upLed,a); digitalWrite(downLed,a); digitalWrite(halfLed,a)
