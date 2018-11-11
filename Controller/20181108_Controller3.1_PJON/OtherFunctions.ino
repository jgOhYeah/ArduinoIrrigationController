int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void runningTime(char * timeSinceStart) {
  unsigned long milliSeconds = millis();
  //byte seconds = milliSeconds / 1000 % 60;
  byte minutes = milliSeconds / 60000 % 60;
  unsigned int hours = milliSeconds / 3600000;
  char number[7];
  itoa(hours, number, 10);
  strcpy(timeSinceStart, number);
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
void updateTime() {
  //Every few seconds
  static unsigned long nextTime = 0;
  unsigned long milliSeconds = millis();
  if(milliSeconds >= nextTime) {
    nextTime = milliSeconds + clockUpdateSpeed;
    drawClock();
    lcd.setCursor(cursorPos,cursorRow);
  }
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
#ifdef serialDebug
  Serial.println();
  Serial.print(F("ERROR:\tError Code: "));
  Serial.print(code);
  Serial.print(F("\tData: "));
  Serial.print(data);
#endif
  digitalWrite(LED_BUILTIN,HIGH);
  byte tempScreen = currentScreen; //Done this so can set the current screen back to what it was before it is changed.
  if(currentScreen == errorScreen) { //Default to the main screen if this was already on the error screen
    tempScreen = mainScreen;
  } else {
    currentScreen = errorScreen;//Freeze the main screen if active
  }
  //Put the time into the message
  runningTime(completeErrorMsg);
  strcat(completeErrorMsg," ");
  switch(code) {
    case PJON_CONNECTION_LOST: {
#ifdef serialDebug
      Serial.print(F("\tAddress: "));
      Serial.print(byte(bus.packets[data].content[0]));
#endif
      bayStatus[byte(bus.packets[data].content[0])-firstSlaveAddress] = bayNotPresent; //Update the array of bay statuses to be disconnected.
      //Build the error message
      //Copy "all" to the string.
      strcat_P(completeErrorMsg, bay);
      //Check to see if all devices have been affected. If so, replace numbers with "All"
      bool allDisconnected = true;
      for(byte i = firstDevice; i < numberOfDevices; i++) {
        if(bayStatus[i] != bayNotPresent) {
          allDisconnected = false;
          break; //Speed processing up a bit possibly
        }
      }
      if(allDisconnected) {
        //Copy "all" to the string.
        strcat_P(completeErrorMsg, all);
      } else {
        for(byte i = firstDevice; i < numberOfDevices; i++) {
          if(bayStatus[i] == bayNotPresent) {
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
    case PJON_PACKETS_BUFFER_FULL: { //If the master is trying to send or recieve more data at a higher rate than the bus is capable. Should not be an issue once set up.
      strcat_P(completeErrorMsg, bufferTooLong);
      strcat_P(completeErrorMsg, lengthString);
      //char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case PJON_CONTENT_TOO_LONG: {
      strcat_P(completeErrorMsg, contentTooLong);
      strcat_P(completeErrorMsg, lengthString);
      //char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case invalidAddress: {
      strcat_P(completeErrorMsg, invalidAddressString);
      //char charBuffer[5];
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case packetError: {
      strcat_P(completeErrorMsg, packetErrorString);
      //char charBuffer[5];
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    default: {
      strcat_P(completeErrorMsg, unknownError);
      //char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      //UnkownErrorMessage
    }
  }
#ifdef serialDebug
  Serial.println();
#endif
  //Swap to the error screen
  currentScreen = tempScreen;
  changeScreen(errorScreen,currentScreen);
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
  changeScreen(setValueScreen,currentScreen); //Will not do anything apart from set the variables up and clear the screen as data needs to be passed to this function to be written
  //static char charBuffer[16];
  strcpy_P(charBuffer,stringBack);
  Serial.println(charBuffer);  
  lcd.write(charBuffer);
  lcd.setCursor(16-strlen(title),0);
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
    case bAddress: //8 bit settings
    case halfPos:
      break;
    case uTravelSpeed: //32 bit settings
      strcpy_P(charBuffer2,stringUp);
      strcat_P(charBuffer2,stringTime);
      strupr(charBuffer2); //Convert Up Time to all capitals
      requestNumberInput(charBuffer2,'m','s',100,number,100,600000,6,sendEepromLong); //Need to retrieve value from slave to display
      break;
    case dTravelSpeed:
      strcpy_P(charBuffer2,stringDown);
      strcat_P(charBuffer2,stringTime);
      strupr(charBuffer2); //Convert Up Time to all capitals
      requestNumberInput(charBuffer2,'m','s',100,number,100,600000,6,sendEepromLong); //Need to retrieve value from slave to display
    case ebaudRate:
      break;
  }
}

