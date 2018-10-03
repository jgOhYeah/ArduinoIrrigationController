/*int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}*/

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
  digitalWrite(LED_BUILTIN,HIGH);
  currentScreen = errorScreen;//Freeze the main screen if active
  //Put the time into the message
  runningTime(completeErrorMsg);
  strcat(completeErrorMsg," ");
  switch(code) {
    case PJON_CONNECTION_LOST: {
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
            char charBuffer[4]; //Should only need three, but just in case I decide to add more than 10 bay outlets in future.
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
      char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case PJON_CONTENT_TOO_LONG: {
      strcat_P(completeErrorMsg, contentTooLong);
      strcat_P(completeErrorMsg, lengthString);
      char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case invalidAddress: {
      strcat_P(completeErrorMsg, invalidAddressString);
      char charBuffer[5];
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case packetError: {
      strcat_P(completeErrorMsg, packetErrorString);
      char charBuffer[5];
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    default: {
      strcat_P(completeErrorMsg, unknownError);
      char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      //UnkownErrorMessage
    }
  }
  //Swap to the error screen
  changeScreen();
  digitalWrite(LED_BUILTIN,LOW);
}
