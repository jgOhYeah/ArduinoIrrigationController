#define serialDebug
#define masterId 255
byte myId; //44 is bay 5, 45 is bay 6... 40 is bay 1
#define reportStatus 's'
#define overwriteStatus 'w'
#define reset 'x'
#define setStatus 'r'
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
#define bayNotPresent 4
#define buttonPin 9
#define upLed 8
#define halfLed 7
#define downLed 6
#define motorA 4
#define motorB 5
#define rs485rxPin 3
#define rs485txPin 2
#define longPressTime 5000
#define serialTimeout 600000 //10 minutes
char bayStatus = bayUnknown;

byte halfwayPos; //25 Percent from the bottom
unsigned int downTravelSpeed; //Default 26000
unsigned int upTravelSpeed; //Default 24000

unsigned int halfUpTime; //Will be worked out using values from EEPROM on start
unsigned int halfDownTime;

unsigned long startDelayTime = 0;
unsigned long callbackTime = 0;
unsigned long timeAtDown = 0;
unsigned long timeAtOpen = 0;
#define bayNothing 0
#define bayHoming 1
#define bayToHalf 2
char callbackOperation = bayNothing;

//ADDRESSES OF VALUES STORED IN EEPROM
#define addressAddress 0
#define halfwayPosAddress 1
#define downTravelSpeedAddress 2
#define upTravelSpeedAddress 4

#define confirmYes 0
#define confirmNo 1
#define confirmTimeout 2
