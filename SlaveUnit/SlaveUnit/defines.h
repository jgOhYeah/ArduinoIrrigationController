//Builtin UI
#define shutLed 3
#define halfLed 4
#define openLed 5

//Motor driving
#define motorA 7
#define motorB 8

//Pins
#define serialEnablePin 2

//Serial
#define baudRate 1200
//Max time to delay in ms to hopefully avoid conflicts
#define maxDelayTime 30
//Addresses 0 to numberOfDevices for individual addressing, all devices are 'a'
#define allDevices 'a'
#define numberOfDevices 7

//Commands and states
#define reportStatus 's'
#define reset 'x'
#define overwriteStatus 'w'
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
#define bayNotPresent 4

//Start and end characters
#define startChar '~' //Something weird and not often used so cannot ba an accident
#define endChar 10 //Newline char
//eepromAddresses
#define deviceIdAddress 0
#define bayOpenTimeAddress 1
#define bayCloseTimeAddress 2
#define bayHalfTimeFractionAddress 3
#define serialRead 'r'
#define serialWrite 'w'
//Timeouts
#define enableLineWait 3000
#define serialRecieveTimeout 3000
//Give the user extra time
#define serialRecieveUserTimeout 20000
#define callbackTimeout 10000
#define eepromInactivityTimeout 120000
//Error codes not already defined - continueing on from library
#define serialAddressTooLarge 6
#define serialRxNotMatchExpected 7


//Set up the button
#define buttonPin 6 //Pin the button is attached to
#define minOffTime 300 //Minimum amount of time between presses
#define buttonPressedState HIGH //Which way the pin is pulled when it is pressed
#define pullupsEnabled false //Enable the internal pullup resistors

//LEDs
#define unknownFlashSpeed 500
#define settingsFlashSpeed 250

//Variable to save settings and states in
byte deviceAddress;
byte bayOpenTime;
byte bayCloseTime;
byte bayHalfTimeFraction;
byte bayStatus = bayUnknown;
//Times and stuff
unsigned long shutoffTime;
unsigned long timeToStop;
