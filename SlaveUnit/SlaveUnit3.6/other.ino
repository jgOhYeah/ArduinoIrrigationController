void updateBay(char newStatus) {
  switch(newStatus) {
    case STATE_SHUT:
      closeBayMotor();
      digitalWrite(PIN_DOWN_LED,HIGH);
      digitalWrite(PIN_HALF_LED,LOW);
      digitalWrite(PIN_UP_LED,LOW);
      timeAtDown = millis();
      break;
    case STATE_HALF:
      closeBayMotor();
      digitalWrite(PIN_DOWN_LED,LOW);
      digitalWrite(PIN_HALF_LED,HIGH);
      digitalWrite(PIN_UP_LED,LOW);
      Serial.println(F("Bay at half called"));
      if(bayStatus == STATE_SHUT) {
        Serial.println(F("Bay was shut"));
        unsigned long timeShut = millis() - timeAtDown; //TODO: Stuff
        //If was definitely open, then if the time spent shutting is less than half open time.
        Serial.print(F("Bay was open for: "));
        Serial.print(millis()-timeAtOpen);
        Serial.print(F("\tBay has been shutting for: "));
        Serial.println(timeShut);
        if(millis() - timeAtOpen >= upTravelSpeed && timeShut <= halfDownTime) { //Before we started stuffing various settings up, was the outlet previously at the top and has the outlet traveled past the half way point?
          callback.add(halfDownTime-timeShut,false,stopBayMotor); //Stop the motor at the right time. - It is already moving
        } else {
          unsigned long callbackTime = 0;
          if(timeShut < downTravelSpeed) { //The bay is not at the bottom yet, so wait until it gets there
            callbackTime = downTravelSpeed - timeShut; //Delay only the minimum required to garuntee that it is shut
          }
          callback.add(callbackTime,false,openToHalf); //Reverse the direction of the motor at the right time
        }
      } else {
        //We can't be sure about the position of much else, so do the full amount just to be safe.
        callback.add(downTravelSpeed,false,openToHalf); //Reverse the direction of the motor at the right time
      }
      startDelayTime = millis();
      break;
    case STATE_OPEN:
      openBayMotor();
      digitalWrite(PIN_DOWN_LED,LOW);
      digitalWrite(PIN_HALF_LED,LOW);
      digitalWrite(PIN_UP_LED,HIGH);
      timeAtOpen = millis();
      break;
    default:
      digitalWrite(PIN_DOWN_LED,HIGH);
      digitalWrite(PIN_HALF_LED,HIGH);
      digitalWrite(PIN_UP_LED,HIGH);
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
#ifdef SERIAL_DEBUG
  Serial.println();
  Serial.print(F("ERROR:\tError Code: "));
  Serial.print(code);
  Serial.print(F("\tData: "));
  Serial.println(data);
#endif
  for(byte i = 0; i < 5; i++) { //Flash a led a bit so that it is obvious something is wrong
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    delay(100);
  }
}
