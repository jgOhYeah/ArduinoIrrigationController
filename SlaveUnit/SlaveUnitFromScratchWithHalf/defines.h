#define serialDebug
#define masterId 255
#define myId 44
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

char bayStatus = bayUnknown;

#define halfwayPos 25 //Percent from the bottom
#define downTravelSpeed 24000
#define upTravelSpeed 25000

const unsigned int halfUpTime = halfwayPos / (upTravelSpeed / 100);
const unsigned int halfDownTime = (100-halfwayPos) / (downTravelSpeed / 100);

unsigned long startDelayTime = 0;
unsigned long callbackTime = 0;
unsigned long timeAtDown = 0;
unsigned long wasUpTime = 0;
#define bayNothing 0
#define bayHoming 1
#define bayToHalf 2
char callbackOperation = bayNothing;
