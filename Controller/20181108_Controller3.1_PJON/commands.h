/*A set of commands and constants that are the same on the master*/
//Commands
#define reportStatus "s" //Different from slave - string instead of char
#define overwriteStatus 'w'
#define reset "x" //Different from slave - string instead of char
#define setStatus 'r'
#define setEeprom 'm'
#define readEeprom 'n'
#define reportEeprom 'o' //The reply message sent to readEeprom

//EEPROM value codes
#define bAddress 'a'
#define halfPos 'b'
#define uTravelSpeed 'c'
#define dTravelSpeed 'd'
#define ebaudRate 'e'

//Bay states
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
#define bayNotPresent 4
