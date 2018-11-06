void updateBay(char newStatus) {
  callbackOperation = bayNothing;
  switch(newStatus) {
    case bayShut:
      closeBayMotor();
      digitalWrite(downLed,HIGH);
      digitalWrite(halfLed,LOW);
      digitalWrite(upLed,LOW);
      timeAtDown = millis();
      break;
    case bayHalf:
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
          callbackOperation = bayToHalf;
          callbackTime = halfDownTime - timeShut;
        } else {
          callbackOperation = bayHoming;
          if(timeShut >= downTravelSpeed) { //Know that the bay must be at the bottom
            callbackTime = 0;
          } else {
            callbackTime = downTravelSpeed - timeShut; //Delay only the minimum required to garuntee that it is shut
          }
        }
      } else {
        callbackOperation = bayHoming;
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
    informMaster(masterId);
  }
}
void checkCallbacks() {
  if(millis() - startDelayTime >= callbackTime) {
    switch(callbackOperation) {
      case bayHoming: {
        Serial.print(F("Callback called. Operation: "));
        Serial.println(byte(callbackOperation));
        openBayMotor();
        callbackOperation = bayToHalf;
        callbackTime = halfUpTime;
        startDelayTime = millis();
        break;
      }
      case bayToHalf: {
        Serial.print(F("Callback called. Operation: "));
        Serial.println(byte(callbackOperation));
        stopBayMotor();
        callbackOperation = bayNothing;
        break;
      }
    }
  }
}
//#error "Stuff to do to flash leds here!"
void flashLeds(byte mode) {
  static unsigned long startTime = 0;
  static byte position = 0; //Position in sequence
  
  bool timeThisRound = false;
  if(millis() - startTime >= ledFlashSpeed) {
    startTime = millis();
    timeThisRound = true;
  }
  switch(mode) {
    case flashAllOn:
      ALL_LEDS(HIGH);
      break;
    case flashAllOff:
      ALL_LEDS(LOW);
      break;
  }
}
//The arrays in these functions must be at least 4 bytes long.
unsigned long arrayToLong(byte * arrayToConvert) {
  unsigned long number = 0;
  for(char i = 3; i >= 0; i--) {
    number = number << 8; //0 << x = 0 all the time (hopefully)
    number += arrayToConvert[i];
  }
  return number;
}
void longToArray(byte * arrayToReceive, unsigned long longToConvert) {
  for(byte i = 0; i < 4; i++) {
    arrayToReceive[i] = longToConvert & 255);
    longToConvert = longToConvert >> 8; //We don't care that value will be 0
  }
}

