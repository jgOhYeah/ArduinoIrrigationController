void updateBay(char newStatus) {
  callback.cancel(bayMovedCallback); //Will be NONEXISTANT_CALLBACK if the callback has already ended as the function it is set to call will set this value when they are called.
  switch(newStatus) {
    case STATE_SHUT:
      closeBayMotor();
      flashLeds(B10001001); //flash down led slowly
      bayMovedCallback = callback.add(downTravelSpeed,false,stopLedsFlashing); //Make the led go steady when we are pretty sure the motor has stopped
      timeAtDown = millis();
      break;
    case STATE_HALF:
      closeBayMotor();
      flashLeds(B10010010); //flash half led slowly - leds will be made steady as part of stopping the motor.
      DEBUG_MOTORS_LN(F("Bay at half called"));
      if(bayStatus == STATE_SHUT) {
        DEBUG_MOTORS_LN(F("Bay was shut"));
        unsigned long timeShut = millis() - timeAtDown; //TODO: Stuff
        //If was definitely open, then if the time spent shutting is less than half open time.
        DEBUG_MOTORS(F("Bay was open for: "));
        DEBUG_MOTORS(millis()-timeAtOpen);
        DEBUG_MOTORS(F("\tBay has been shutting for: "));
        DEBUG_MOTORS_LN(timeShut);
        if(millis() - timeAtOpen >= upTravelSpeed && timeShut <= halfDownTime) { //Before we started stuffing various settings up, was the outlet previously at the top and has the outlet traveled past the half way point?
          bayMovedCallback = callback.add(halfDownTime-timeShut,false,stopBayMotor); //Stop the motor at the right time. - It is already moving
        } else {
          unsigned long callbackTime = 0;
          if(timeShut < downTravelSpeed) { //The bay is not at the bottom yet, so wait until it gets there
            callbackTime = downTravelSpeed - timeShut; //Delay only the minimum required to garuntee that it is shut
          }
          bayMovedCallback = callback.add(callbackTime,false,openToHalf); //Reverse the direction of the motor at the right time
        }
      } else {
        //We can't be sure about the position of much else, so do the full amount just to be safe.
        bayMovedCallback = callback.add(downTravelSpeed,false,openToHalf); //Reverse the direction of the motor at the right time
      }
      startDelayTime = millis();
      break;
    case STATE_OPEN:
      openBayMotor();
      flashLeds(B10100100); //Flash up led slowly
      bayMovedCallback = callback.add(upTravelSpeed,false,stopLedsFlashing); //Make the led go steady when we are pretty sure the motor has stopped
      timeAtOpen = millis();
      break;
    default:
      flashLeds(B11000000); //Chasing
  }
  bayStatus = newStatus;
}

//The arrays in these functions must be at least 4 bytes long.
unsigned long arrayToLong(byte * arrayToConvert, byte startIndex) {
  unsigned long number = 0;
  for(byte i = startIndex+4; i > startIndex; i--) {
    number = number << 8; //0 << x = 0 all the time (hopefully)
    number += arrayToConvert[i-1]; //-1 is to zero index without needing to have i as a char so that is can be -1 to not be >= 0 so it will stop looping.
  }
  return number;
}
void longToArray(byte * arrayToReceive, byte startIndex, unsigned long longToConvert) {
  for(byte i = startIndex; i < startIndex+4; i++) {
    arrayToReceive[i] = longToConvert & 255;
    longToConvert = longToConvert >> 8; //We don't care that value will be 0
  }
}

bool outsideRange(unsigned long number, unsigned long minimum, unsigned long maximum) {
  if(number < minimum || number > maximum) {
    return true;
  } else {
    return false;
  }
}
void errorHandler (uint8_t code, uint16_t data, void *custom_pointer) {
  errorHandlerLong(code,(unsigned long) data,custom_pointer); //Make the PJON errors work
}
void errorHandlerLong (uint8_t code, unsigned long data, void *custom_pointer) {
  DEBUG_ERRORS_LN(());
  DEBUG_ERRORS(F("ERROR:\tError Code: "));
  DEBUG_ERRORS(code);
  DEBUG_ERRORS(F("\tData: "));
  DEBUG_ERRORS_LN(data);
  for(byte i = 0; i < 5; i++) { //Flash a led a bit so that it is obvious something is wrong
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    delay(100);
  }
}
/*void heartbeat() { //Debug and other stuff that needs to be done every so often.
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.print(F("Time: "));
  Serial.print(millis());
  Serial.print(F("\tCallbacks Active: "));
  Serial.println(callback.countActive());
  digitalWrite(LED_BUILTIN,LOW);
}*/

