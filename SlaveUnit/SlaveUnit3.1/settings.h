 //PJON STUFF
//The bus id that PJON uses. Because it is a local network it doesn't matter really, but needs to be the same on all devices on the network.
const byte busId[] = {0,0,0,1};
#define serialEnablePin 2 //3 is reserved for if a serial read enable pin is required
#define baudRate 9600
//Maximum number of packets can be edited in main file near top.
#define serialDebug //Only define during development, comment out for normal use as may cause interference
//Button settings
#define btnOffTime 150 //Minimum amount of time between presses
#define buttonPressedState LOW //Which way the pin is pulled when it is pressed - must be low if pullups used.
#define pullupsEnabled false //Enable the internal pullup resistors
#define ledFlashTime 10000
//Individual bay settings
#define bay1Address 44
#define bay2Address 45
//Bay main settings
bayStruct bays[] = { //Two bays are predefined
  { //Bay 0 of this slave
    {4, 5, 6, 7, 8}, //OUTPUT PINS: Motor pin A, Motor pin B, Up led, Half led, Down led
    44, //Serial address of bay
    bayUnknown, //Initial bay status
    ButtonOnPress(9,btnOffTime,buttonPressedState,pullupsEnabled) //Input button
  }/*,
  { //Bay 1 of this slave - We need to resort to using adc pins as digital pins because we have run out of boring pins
    {10, 11, 12, A0, A1}, //OUTPUT PINS: Motor pin A, Motor pin B, Up led, Half led, Down led
    bay2Address, //Serial address of bay
    bayUnknown, //Initial bay status
    PJON<ThroughSerial>(busId, bay2Address), //Serial setup
    ButtonOnPress(A2,btnOffTime,buttonPressedState,pullupsEnabled) //Input button
  }*/
};
