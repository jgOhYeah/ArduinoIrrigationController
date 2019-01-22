/*A set of commands and constants that are the same on the master*/
//Commands
#define CMD_REPORT_STATUS 's'
#define CMD_OVERWRITE_STATUS 'w'
#define CMD_RESET 'x'
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
#define ERR_INVALID_ADDRESS 50
#define ERR_PACKET_LENGTH 51
#define ERR_UNKOWN_COMMAND 52
#define ERR_UNKOWN_PARAMETERS 53
#define ERR_OUTSIDE_RANGE 54
#define ERR_UNEXPECTED_PACKET 55
#define ERR_UNDERVOLTAGE 56

//Other common values
#define NONEXISTANT_CALLBACK 254

//Shared PJON settings/ default values
#define DEFAULT_PJON_BUS_ID {0,0,1,73} //An official bus id for rs485 in Northern Victoria, registered 15/01/2018
#define DEFAULT_ADDRESS 40 //The default address of the slave
#define DEFAULT_UP_TIME 24000 //These defaults are offone of the inital batch of linear actuators running off a 12v switchmode power supply - will not be right for other ones
#define DEFAULT_DOWN_TIME 23700
#define DEFAULT_HALF_POS 50
#define RESET_SETTINGS_EEPROM B10011001 //Random number Change this to force the slave to reset all eeprom to the default when it starts up for the first time after programming. - If the arduino is new, the value will probably be 255

