#define numberOutputPins 5
//Indexing for turning the array output pins back into being readable
#define motorPinA 0
#define motorPinB 1
#define upLed 2
#define halfLed 3
#define downLed 4
//Bays statuses
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
struct bayStruct { //Make a struct to hold all information and settings for the bay
  //Pins
  const byte outputPins[numberOutputPins];
  //Classes used for serial and the button
  const byte address; //Load from eeprom? - need to make non-constant then/
  char status;
  PJON<ThroughSerial> bus;
  ButtonOnPress button;
};
