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

//ERROR Codes
#define invalidAddress 50
#define packetError 51
#define unrecognisedCommand 52

//ADDRESSES OF VALUES STORED IN EEPROM
//Longs take 4 bytes, ints take 2 and bytes take 1
#define addressAddress 0
#define halfwayPosAddress 1
#define downTravelSpeedAddress 2
#define upTravelSpeedAddress 6
#define serialBaudAddress 10


//Leds
//#error "Defines for leds need sorting out!"
#define ALL_LEDS(state) digitalWrite(upLed,state); digitalWrite(halfLed,state); digitalWrite(downLed,state)
#define flashAllOn 0
#define flashAllOff 1
#define flashAllFlash 2
#define flashAllScan 3
#define flashUpOn 4
#define flashUpOff 5
#define flashUpFlash 6
#define flashHalfOn 7
#define flashHalfOff 8
#define flashHalfFlash 9
#define flashDownOn 10
#define flashDownOff 11
#define flashDownFlash 12
//Count rates to skip
#define flashDutyQuarter 0
#define flashDutyHalf 1
#define flashDuty3Quarter 2
