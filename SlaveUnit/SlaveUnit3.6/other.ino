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
void errorHandler (uint8_t code, uint16_t data, void *custom_pointer) { //Allows the error codes from PJON to work with the same function as the error codes with large numbers in the data value
  errorHandlerLong(code,(unsigned long) data,custom_pointer); //Make the PJON errors work
}
void errorHandlerLong (uint8_t code, unsigned long data, void *custom_pointer) {
  //Only do it if the error is a not a disconnected error - otherwise possibly causing a loop!
  if(code != PJON_CONNECTION_LOST && code != PJON_PACKETS_BUFFER_FULL) { //Possibly need to put in && code != PJON_CONTENT_TOO_LONG
    char msgToSend[7];
    msgToSend[0] = CMD_ERROR;
    msgToSend[1] = code;
    longToArray((byte *)msgToSend,2,data);
    msgToSend[6] = 0; //Null terminate the array just in case
    bus.send(MASTER_ID,msgToSend,6);
  }
  DEBUG_ERRORS_LN();
  DEBUG_ERRORS(F("ERROR:\tError Code: "));
  DEBUG_ERRORS(code);
  DEBUG_ERRORS(F("\tData: "));
  DEBUG_ERRORS_LN(data);
  specialFlashLeds(B01111111,2500);
  digitalWrite(LED_BUILTIN,HIGH); //Lasting message that an error has occured
  /*for(byte i = 0; i < 5; i++) { //Flash a led a bit so that it is obvious something is wrong
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    delay(100);
  }*/
}
void housekeeping() { //Debug and other stuff that needs to be done every so often but not every loop
  unsigned long voltage = readVcc();
  if(voltage < MINIMUM_VOLTAGE) {
     if(!isUndervoltage) {
      isUndervoltage = true;
      //digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN);
      //Stop the motor as well here
      errorHandlerLong(ERR_UNDERVOLTAGE,voltage,NULL);
     }
  } else {
    isUndervoltage = false; //So only calls an undervoltage error once.
  }
  DEBUG_HOUSEKEEPING(F("HOUSEKEEPING ("));
  DEBUG_HOUSEKEEPING(millis());
  DEBUG_HOUSEKEEPING(F("):\tBay state: "));
  DEBUG_HOUSEKEEPING(byte(bayStatus));
  DEBUG_HOUSEKEEPING(F("\tVoltage: "));
  DEBUG_HOUSEKEEPING_LN(voltage);
}

//From https://www.instructables.com/id/Secret-Arduino-Voltmeter/
unsigned long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
     ADMUX = _BV(MUX5) | _BV(MUX0) ;
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
