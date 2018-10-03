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
    bays[i].bus.strategy.set_serial(&Serial);
    bays[i].bus.set_synchronous_acknowledge(false);
    bays[i].bus.set_asynchronous_acknowledge(true);
    bays[i].bus.set_communication_mode(PJON_HALF_DUPLEX);
    bays[i].bus.strategy.set_enable_RS485_pin(serialEnablePin);
    bays[i].bus.set_error(errorHandler);
    bays[i].bus.set_custom_pointer(&bays[i]);
    bays[i].bus.set_receiver(recieveData);
    bays[i].bus.begin();
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
