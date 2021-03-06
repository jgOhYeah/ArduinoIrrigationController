void checkButtons() {
  if(button.checkButton()) { //If the button is pressed
    digitalWrite(LED_BUILTIN,LOW); //Reset the lasting message that an error has occured.
    char newStatus = bayStatus + 1;
    if(newStatus > STATE_OPEN) {
      newStatus = STATE_SHUT;
    }
    updateBay(newStatus);
    informMaster(MASTER_ID);
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
    ledStates = states; //Set the global variable
    callback.cancel(ledSpecialCallback); //Cancel the special flashing of the leds if needed
    callback.cancel(ledCallback); //Get rid of the old callback to stop this becoming a memory leak
    //Set the initial states of LEDs.
    setLeds();
    //Set up the callbacks
    byte mode = (states & B11000000) >> 6; //Get the top two bits as a number easily processed.
    switch(mode) {
      case LEDS_STEADY:
        //Don't set any callbacks - initial state only.
        ledCallback = NONEXISTANT_CALLBACK;
        break;
      case LEDS_FAST:
        ledCallback = callback.add(FAST_FLASH_RATE,true,updateLeds); //NEED TO SAVE ADDRESS TO CANCEL THIS CALLBACK BEFORE THE NEXT IS CALLED
        break;
      case LEDS_SLOW:
        ledCallback = callback.add(SLOW_FLASH_RATE,true,updateLeds); //NEED TO SAVE ADDRESS TO CANCEL THIS CALLBACK BEFORE THE NEXT IS CALLED
        break;
      case LEDS_CHASE:
        ledCallback = callback.add(SLOW_FLASH_RATE,true,updateLeds); //NEED TO SAVE ADDRESS TO CANCEL THIS CALLBACK BEFORE THE NEXT IS CALLED
        break;
    }
}
void updateLeds() {
  if((ledStates & B11000000) >> 6 == LEDS_CHASE) { //If the leds are in chasing mode, special treatment, otherwise toggle leds if needed
    //In chase mode
    byte currentStep = ledStates & B00000011; //The current step of the 
    switch(currentStep) {
      case 0:
        ledStates = B11001001;
        break;
      case 1:
        ledStates = B11010010;
        break;
      case 2:
        ledStates = B11100011;
        break;
      case 3:
        ledStates = B11010000;
        break;
    }
  } else {
    //Not in chase mode - will be flashing because steady is not called.
    if(ledStates & B00000100) { //Up led flashing
      ledStates = ledStates ^ B00100000; //Invert the bit for the current led's state
    }
    if(ledStates & B00000010) { //Half led flashing
      ledStates = ledStates ^ B00010000; //Invert the bit for the current led's state
    }
    if(ledStates & B00000001) { //Down led flashing
      ledStates = ledStates ^ B00001000; //Invert the bit for the current led's state
    }
  }
  setLeds();
}
//A function to write the current state of the led
void setLeds() {
  digitalWrite(PIN_UP_LED,(ledStates & B00100000) >> 5); //Write the intial state of the up led (1 = on, 0 = off)
  digitalWrite(PIN_HALF_LED,(ledStates & B00010000) >> 4); //Write the intial state of the half led (1 = on, 0 = off)
  digitalWrite(PIN_DOWN_LED,(ledStates & B00001000) >> 3); //Write the intial state of the down led (1 = on, 0 = off)
}
void stopLedsFlashing() { //WATCH out if calling from something other than a bayMoved callback as it records the bayMoved callback as done.
  ledStates = (ledStates & B00000111) << 3; //Set the currently active flashing leds to be always on.
  setLeds();
  callback.cancel(ledCallback); //Get rid of the old callback to stop this becoming a memory leak and wasting processing time
  bayMovedCallback = NONEXISTANT_CALLBACK;
  DEBUG_UI(F("Stopped LEDs Flashing. State: B"));
  DEBUG_UI_LN((ledStates,2));
}
void specialFlashLeds(byte tempState,unsigned long duration) {
  ledSavedStates = ledStates; //Save the current value to return to it later.
  flashLeds(tempState); //Flash the new config of leds
  ledSpecialCallback = callback.add(duration,false,resetLedsToNormal); //Set the time to reset back to the old mode
  
}
void resetLedsToNormal() {
  flashLeds(ledSavedStates);
}
