#define masterId 255
#define myId 44
#define myId2 45
#define reportStatus 's'
#define overwriteStatus 'w'
#define reset 'x'
#define setStatus 'r'
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
#define bayNotPresent 4
#define buttonPin 8
#define upLed 7
#define halfLed 6
#define downLed 5
#define motorA 2
#define motorB 3
char bayStatus = bayUnknown;

#define halfwayPos 25 //Percent from the bottom
#define upTravelSpeed 20000
#define halfUpTime  10000 //halfwayPos*upTravelSpeed/100
#define downTravelSpeed 24000
unsigned long startDelayTime = 0;
unsigned long callbackTime = 0;
unsigned long timeAtDown = 0;
#define bayNothing 0
#define bayHoming 1
#define bayToHalf 2
char callbackOperation = bayNothing;

struct bay {
  byte motorPinA;
  byte motorPinB;
  byte address;
  PJON<ThroughSerial> bus;
};
#define numberOfBays 2
bay connectedBays[numberOfBays] = {{2,3,44;
