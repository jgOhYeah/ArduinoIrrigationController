void setupBays() {
  for(byte i = 0; i < numberOfBays; i++) { //Set the bays up
    //Set up Output pins - used as an array so can do this quick
    for(byte j = 0; j < numberOutputPins; j++) {
      pinMode(bays[i].outputPins[j],OUTPUT);
    }
    //Set up the leds
    updateBay(&bays[i],bayUnknown);
    //Set up button pin
    bays[i].button.begin();
    //Set up the serial interface for the bay
  }
}
void updateBay(bayStruct * bayToChange, char newStatus) {
  switch(newStatus) {
    case bayShut:
      closeBay(bayToChange);
      LED_ON(bayToChange,downLed);
      LED_OFF(bayToChange,halfLed);
      LED_OFF(bayToChange,upLed);
      break;
    case bayHalf:
      closeBay(bayToChange);
      LED_OFF(bayToChange,downLed);
      LED_ON(bayToChange,halfLed);
      LED_OFF(bayToChange,upLed);
      break;
    case bayOpen:
      openBay(bayToChange);
      LED_OFF(bayToChange,downLed);
      LED_OFF(bayToChange,halfLed);
      LED_ON(bayToChange,upLed);
      break;
    default:
      LED_ON(bayToChange,downLed);
      LED_ON(bayToChange,halfLed);
      LED_ON(bayToChange,upLed);
  }
 bayToChange->status = newStatus;
}
//We are using boring, slow digitalWrite commands here as the pins are spread across different ports.
void openBay(bayStruct * bayToChange) {
  digitalWrite(bayToChange->outputPins[motorPinA],HIGH);
  digitalWrite(bayToChange->outputPins[motorPinB],LOW);
}
void closeBay(bayStruct * bayToChange) {
  digitalWrite(bayToChange->outputPins[motorPinA],LOW);
  digitalWrite(bayToChange->outputPins[motorPinB],HIGH);
}
void stopBay(bayStruct * bayToChange) {
  digitalWrite(bayToChange->outputPins[motorPinA],LOW);
  digitalWrite(bayToChange->outputPins[motorPinB],LOW);
}
