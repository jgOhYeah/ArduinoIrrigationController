//Settings to be edited before uploading

//Debugging
#define DEFAULT_BAUD_RATE 9600 //Default baud rate to CMD_RESET the value stored in eeprom to.
//Debug settings
#define DEBUG_ENABLE //Enable debugging. The other debugging features will only be active if this is defined.

//#define DEBUG_CALLBACK_LIBRARY_ENABLE //Used for the callbacks library to send serial data.
//#define DEBUG_EEPROM_ENABLE //Used to debug reading and saving longs to EEPROM
//#define DEBUG_MOTORS_ENABLE //Used to debug the current motor state
//#define DEBUG_UI_ENABLE //When leds are flashing
//#define DEBUG_PJON_ENABLE //Used to debug PJON error messages...
//#define DEBUG_HOUSEKEEPING_ENABLE //Supply voltage, current state, time...
#define DEBUG_ERRORS_ENABLE // Error messages

//Error conditions
#define MINIMUM_VOLTAGE 4000 //The voltage at which the arduino starts complaining - motor on, high resistance power wires...

//Pins
#define PIN_BUTTON 9
#define PIN_UP_LED 8
#define PIN_HALF_LED 7
#define PIN_DOWN_LED 6
#define PIN_MOTOR_A 4
#define PIN_MOTOR_B 5
#define PIN_RS485_RX 3
#define PIN_RS485_TX 2

