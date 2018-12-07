void updateBay(char newStatus) {
  callbackOperation = BAY_NOTHING;
  switch(newStatus) {
    case bayShut:
      closeBayMotor();
      digitalWrite(downLed,HIGH);
      digitalWrite(halfLed,LOW);
      digitalWrite(upLed,LOW);
      timeAtDown = millis();
      break;
    case BAY_HALF:
      closeBayMotor();
      digitalWrite(downLed,LOW);
      digitalWrite(halfLed,HIGH);
      digitalWrite(upLed,LOW);
      Serial.println(F("Bay at half called"));
      if(bayStatus == bayShut) {
        Serial.println(F("Bay was shut"));
        unsigned long timeShut = millis() - timeAtDown; //TODO: Stuff
        //If was definitely open, then if the time spent shutting is less than half open time.
        Serial.print(F("Bay was open for: "));
        Serial.print(millis()-timeAtOpen);
        Serial.print(F("\tBay has been shutting for: "));
        Serial.println(timeShut);
        if(millis() - timeAtOpen >= upTravelSpeed && timeShut <= halfDownTime) { //Before we started stuffing various settings up, was the outlet previously at the top and has the outlet traveled past the half way point?
          callbackOperation = BAY_TO_HALF;
          callbackTime = halfDownTime - timeShut;
        } else {
          callbackOperation = BAY_HOMING;
          if(timeShut >= downTravelSpeed) { //Know that the bay must be at the bottom
            callbackTime = 0;
          } else {
            callbackTime = downTravelSpeed - timeShut; //Delay only the minimum required to garuntee that it is shut
          }
        }
      } else {
        callbackOperation = BAY_HOMING;
        callbackTime = downTravelSpeed; //We can't be sure about the position of much else, so do the full amount just to be safe.
      }
      startDelayTime = millis();
      break;
    case bayOpen:
      openBayMotor();
      digitalWrite(downLed,LOW);
      digitalWrite(halfLed,LOW);
      digitalWrite(upLed,HIGH);
      timeAtOpen = millis();
      break;
    default:
      digitalWrite(downLed,HIGH);
      digitalWrite(halfLed,HIGH);
      digitalWrite(upLed,HIGH);
  }
  bayStatus = newStatus;
}
void checkButtons() {
  if(button.checkButton()) { //If the button is pressed
    char newStatus = bayStatus + 1;
    if(newStatus > bayOpen) {
      newStatus = bayShut;
    }
    updateBay(newStatus);
    informMaster(MASTER_ID);
  }
}
void checkCallbacks() {
  if(millis() - startDelayTime >= callbackTime) {
    switch(callbackOperation) {
      case BAY_HOMING: {
        Serial.print(F("Callback called. Operation: "));
        Serial.println(byte(callbackOperation));
        openBayMotor();
        callbackOperation = BAY_TO_HALF;
        callbackTime = halfUpTime;
        startDelayTime = millis();
        break;
      }
      case BAY_TO_HALF: {
        Serial.print(F("Callback called. Operation: "));
        Serial.println(byte(callbackOperation));
        stopBayMotor();
        callbackOperation = BAY_NOTHING;
        break;
      }
    }
  }
}
/* ledStates structure in binary:
 * ZZYYYXXX
 * Where:
 *   ZZ is the mode the leds should be in.
 *   YYY is the current state of each led (up, half, down in that order - 0 = off, 1 = on) - also the initial state
 *   XXX is if the led state should be changed when updated. - In chasing mode this is also the stage that the sequence is up to.
 * Examples:
 *   ZZ YYY XXX
 *   00 100 000 = The LEDs on steady, Up led starting on, both others off, do not change the state at any time.
 *   01 010 110 = The LEDs flashing fast, Up and Down LEDs starting off while the Half LED starting on, Only update the Up and Half led when it is time to change their state (Down led stays off)
 *   10 111 111 = Slow flash, All LEDs starting on, all LEDs flashing.
 *   11 111 111 = Chasing - All LEDs involved.
 */
void flashLeds(byte states) {
  
}
void updateLeds() {
  
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
#ifdef serialDebug
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
