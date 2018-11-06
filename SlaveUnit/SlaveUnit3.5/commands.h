/*A set of commands and constants that are the same on the master*/
//Commands
#define reportStatus 's'
#define overwriteStatus 'w'
#define reset 'x'
#define setStatus 'r'
#define setEeprom 'm'
#define readEeprom 'n'

//EEPROM value codes
#define bAddress 'a'
#define halfPos 'b'
#define uTravelSpeed 'c'
#define dTravelSpeed 'd'
#define baudRate 'e'

//Bay states
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
#define bayNotPresent 4
