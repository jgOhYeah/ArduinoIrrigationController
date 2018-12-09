/*A set of commands and constants that are the same on the master*/
//Commands
#define CMD_REPORT_STATUS 's'
#define CMD_OVERWRITE_STATUS 'w'
#define CMD_RESET 'x'
#define CMD_SET_STATUS 'r'
#define CMD_SET_EEPROM 'm'
#define CMD_READ_EEPROM 'n'
#define CMD_REPORT_EEPROM 'o' //The reply message sent to CMD_READ_EEPROM

//EEPROM value codes
#define SETTING_ADDRESS 'a'
#define SETTING_HALF_POS 'b'
#define SETTING_UP_SPEED 'c'
#define SETTING_DOWN_SPEED 'd'
#define SETTING_BAUD_RATE 'e'

//Bay states
#define STATE_UNKOWN 0
#define STATE_SHUT 1
#define STATE_HALF 2
#define STATE_OPEN 3
#define STATE_NOT_PRESENT 4

//Error codes
#define ERR_INVALID_ADDRESS 50
#define ERR_PACKET_LENGTH 51
#define ERR_UNKOWN_COMMAND 52
#define ERR_UNKOWN_PARAMETERS 53
#define ERR_OUTSIDE_RANGE 54
#define ERR_UNEXPECTED_PACKET 55
