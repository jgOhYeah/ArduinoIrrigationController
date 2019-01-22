int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
void wipeBayStates() {
  for(byte i = FIRST_BAY_INDEX; i < NUMBER_OF_BAYS; i++) {
    bayStatus[i] = STATE_NOT_PRESENT;
  }
}
void waitUntilAllConnected() {
  digitalWrite(LED_BUILTIN,HIGH);
  bool allConnected = false;
  unsigned long startTime = millis();
  while(!allConnected && (millis() - startTime <= REPLY_DELAY)) { //NEED to add a timeout for when a bay is disconnected
    bus.update();
    bus.receive();
    allConnected = true;
    for(byte i = FIRST_BAY_INDEX; i < NUMBER_OF_BAYS; i++) {
      if(bayStatus[i] == STATE_NOT_PRESENT) {
        allConnected = false;
        break;
      }
    }
  }
  digitalWrite(LED_BUILTIN,LOW);
}
void runningTime(char * timeSinceStart, bool concatenate) {
  unsigned long milliSeconds = millis();
  //byte seconds = milliSeconds / 1000 % 60;
  byte minutes = milliSeconds / 60000 % 60;
  unsigned int hours = milliSeconds / 3600000;
  char number[7];
  itoa(hours, number, 10);
  if(concatenate) {
    strcat(timeSinceStart, number);
  } else {
    strcpy(timeSinceStart, number);
  }
  strcat_P(timeSinceStart, colon);
  //Add a 0 to keep places
  if (minutes < 10) {
    strcat_P(timeSinceStart, zero);
  }
  itoa(minutes, number, 10);
  strcat(timeSinceStart, number);
  //Seconds (Not needed for this project, but just leaving in in case I need this code for something else. Need to increase char buffer size if using.)
  /*strcat_P(timeSinceStart, colon);
  if (seconds < 10) {
    strcat_P(timeSinceStart, zero);
  }
  itoa(seconds, number, 10);
  strcat(timeSinceStart, number);*/
}
void serialDelay(unsigned long waitTime) {
  unsigned long startTime = millis();
  while(millis() - startTime < waitTime) {
    bus.update();
    bus.receive();
  }
}
//Function that handles and displays errors. Either called by PJON or manually
void errorHandler(uint8_t code, uint16_t data, void *custom_pointer) {
  digitalWrite(LED_BUILTIN,HIGH);
  byte tempScreen = currentScreen; //Done this so can set the current screen back to what it was before it is changed.
  if(currentScreen == LCD_ERROR) { //Default to the main screen if this was already on the error screen
    tempScreen = LCD_MAIN;
  } else {
    currentScreen = LCD_ERROR;//Freeze the main screen if active
  }
  //Put the time into the message
  runningTime(completeErrorMsg,false);
  strcat(completeErrorMsg," ");
  switch(code) {
    case PJON_CONNECTION_LOST: { //This is the most likely error, so give a bit more explaination
      bayStatus[byte(bus.packets[data].content[0])-FIRST_BAY_ADDRESS] = STATE_NOT_PRESENT; //Update the array of bay statuses to be disconnected.
      //Build the error message
      //Copy "all" to the string.
      strcat_P(completeErrorMsg, bay);
      strcat_P(completeErrorMsg, sSpace);
      //Check to see if all devices have been affected. If so, replace numbers with "All"
      bool allDisconnected = true;
      for(byte i = FIRST_BAY_INDEX; i < NUMBER_OF_BAYS; i++) {
        if(bayStatus[i] != STATE_NOT_PRESENT) {
          allDisconnected = false;
          break; //Speed processing up a bit possibly
        }
      }
      if(allDisconnected) {
        //Copy "all" to the string.
        strcat_P(completeErrorMsg, all);
      } else {
        for(byte i = FIRST_BAY_INDEX; i < NUMBER_OF_BAYS; i++) {
          if(bayStatus[i] == STATE_NOT_PRESENT) {
            //char charBuffer[4]; //Should only need three, but just in case I decide to add more than 10 bay outlets in future.
            itoa(i + 1,charBuffer,10);
            strcat(charBuffer,",");
            strcat(completeErrorMsg,charBuffer);
          }
        }
      }
      //Add the not connected message.
      strcat_P(completeErrorMsg,noReply);
      //0000:00 Bays 1,2,3,4,5,6,7, Disconnected
      break;
    }
    case PJON_PACKETS_BUFFER_FULL: //If the master is trying to send or recieve more data at a higher rate than the bus is capable. Should not be an issue once set up.
    case PJON_CONTENT_TOO_LONG: {
      strcat_P(completeErrorMsg, stringPJON);
      break;
    }
    case ERR_INVALID_ADDRESS: //Generic errors and error checking
    case ERR_PACKET_LENGTH:
    case ERR_UNKOWN_COMMAND:
    case ERR_UNKOWN_PARAMETERS:
    case ERR_OUTSIDE_RANGE:
    case ERR_UNEXPECTED_PACKET: {
      strcat_P(completeErrorMsg, stringPacket);
      break;
    }
    case ERR_SRC_OTHER_DEVICE: {
      //otherDeviceError errorDetails = *(otherDeviceError)custom_pointer;
      otherDeviceError errorDetails = *(otherDeviceError *)custom_pointer;
      strcat_P(completeErrorMsg, bay);
      strcat_P(completeErrorMsg, space);
      itoa(data + 1 - FIRST_BAY_ADDRESS,charBuffer,10);
      strcat(completeErrorMsg, charBuffer);
      strcat_P(completeErrorMsg, comma);
      strcat_P(completeErrorMsg, stringErrorCode);
      itoa(errorDetails.code,charBuffer,10);
      strcat(completeErrorMsg, charBuffer);
      strcat_P(completeErrorMsg, stringData);
      ltoa(errorDetails.data,charBuffer,10);
      strcat(completeErrorMsg, charBuffer);
      break;
    }
    default: {
      strcat_P(completeErrorMsg, stringUnkown);
      break;
    }
  }
  //Add error numbers and details
  if(code != PJON_CONNECTION_LOST && code != ERR_SRC_OTHER_DEVICE) { //Only bother to add the error code and associated data if the error is not a not connected error.
    strcat_P(completeErrorMsg, stringErrorCode);
    itoa(code,charBuffer,10);
    strcat(completeErrorMsg,charBuffer);
    strcat_P(completeErrorMsg, stringData);
    itoa(data,charBuffer,10);
    strcat(completeErrorMsg,charBuffer);
  }
#ifdef DEBUG_ENABLE
  Serial.println();
  Serial.println(completeErrorMsg);
#endif
  //Swap to the error screen
  currentScreen = tempScreen;
  screenBeforeThat = previousScreen;
  changeScreen(LCD_ERROR,currentScreen);
  digitalWrite(LED_BUILTIN,LOW);
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

void requestNumberInput (char * title,char suffix1,char suffix2, unsigned int stepSize, unsigned long defaultValue, unsigned long minimum, unsigned long maximum, byte numberOfDigits, void (*callbackFunction)(unsigned long) ) {
  screenBeforeThat = previousScreen;
  changeScreen(LCD_SET_VALUE,currentScreen); //Will not do anything apart from set the variables up and clear the screen as data needs to be passed to this function to be written
  //static char charBuffer[16];
  lcd.home();
  strcpy_P(charBuffer,stringBack);  
  lcd.write(charBuffer);
  lcd.setCursor(LCD_WIDTH-strlen(title),0);
  lcd.write(title);
  //Bottom row will be drawn later
  
  callbackFunctionStored = callbackFunction; //Save the function to be called with the result.
  minimumValue = minimum;
  maximumValue = maximum;
  maximumDigits = numberOfDigits;
  currentNumber = defaultValue;
  valueStepSize = stepSize;
  valueSuffix1 = suffix1;
  valueSuffix2 = suffix2;
  inputValueBottomRow();
}
void retrievedEepromValue(unsigned long number) {
  switch(valueEepromAddress) {
    //case SETTING_ADDRESS: //8 bit settings
    case SETTING_HALF_POS:
      strcpy_P(charBuffer2,stringHalfPos);
      strupr(charBuffer2); //Convert Up Time to all capitals
      requestNumberInput(charBuffer2,'%',0,1,number,1,100,3,sendEepromByte); //Need to retrieve value from slave to display
      break;
    case SETTING_UP_SPEED: //32 bit settings
      strcpy_P(charBuffer2,stringUp);
      strcat_P(charBuffer2,stringTime);
      strupr(charBuffer2); //Convert Up Time to all capitals
      requestNumberInput(charBuffer2,'m','s',100,number,100,600000,6,sendEepromLong); //Need to retrieve value from slave to display
      break;
    case SETTING_DOWN_SPEED:
      strcpy_P(charBuffer2,stringDown);
      strcat_P(charBuffer2,stringTime);
      strupr(charBuffer2); //Convert Up Time to all capitals
      requestNumberInput(charBuffer2,'m','s',100,number,100,600000,6,sendEepromLong); //Need to retrieve value from slave to display
      break;
    case SETTING_BAUD_RATE:
      strcpy_P(charBuffer2,stringBaud);
      strcat_P(charBuffer2,stringRate);
      strupr(charBuffer2); //Convert Up Time to all capitals
      requestNumberInput(charBuffer2,'B','d',100,number,100,2000000,7,sendEepromLong); //Need to retrieve value from slave to display
      break;
  }
}

