/*A set of commands and constants that are the same on the master*/
//Commands
#define CMD_REPORT_STATUS "s" //Different from slave - string instead of char
#define CMD_OVERWRITE_STATUS 'w'
#define CMD_RESET "x" //Different from slave - string instead of char
#define CMD_SET_STATUS 'r'
#define CMD_SET_EEPROM 'm'
#define CMD_READ_EEPROM 'n'
#define CMD_REPORT_EEPROM 'o' //The reply message sent to CMD_READ_EEPROM
#define CMD_ERROR 'E' //E followed by the error code (byte) and data

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
//Source of the error
#define ERR_SRC_THIS_DEVICE 20 //NOT USED CURRENTLY AS ASSUMED TO BE THIS DEVICE UNLESS OTHERWISE STATED
#define ERR_SRC_OTHER_DEVICE 21
//What the error is
#define ERR_INVALID_ADDRESS 50
#define ERR_PACKET_LENGTH 51
#define ERR_UNKOWN_COMMAND 52
#define ERR_UNKOWN_PARAMETERS 53
#define ERR_OUTSIDE_RANGE 54
#define ERR_UNEXPECTED_PACKET 55
#define ERR_UNDERVOLTAGE 56

//Other common values
#define NONEXISTANT_CALLBACK 254

//Default values for eeprom settings
#define DEFAULT_HALF_POS 50
#define DEFAULT_UP_SPEED 25000
#define DEFAULT_DOWN_SPEED 25000
#define DEFAULT_BAUD_RATE 38400

//Shared PJON settings
#define PJON_BUS_ID {0,0,1,73} //An official bus id for rs485 in Northern Victoria, registered 15/01/2018
