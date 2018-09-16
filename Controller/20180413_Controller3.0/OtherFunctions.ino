/*int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}*/
void createErrorMessage(byte errorCode, bool displayBays, bool* offendingBays) {
  //Copy time into error message
  char charBuff[8];
  runningTime(charBuff);
  strcpy(completeErrorMsg, charBuff);
  //Only print out the bays if given it.
  if(displayBays) {
    //Add a space followed by "bay(s): "
    strcat_P(completeErrorMsg,bay);
    //Print all instead of 1,2,3,4,5,6,7
    bool allTrue = true;
    for(byte i = 0; i < numberOfDevices; i++) {
      if(!offendingBays[i]) {
        allTrue = false;
        break;
      }
    }
    if(allTrue) {
      strcat_P(completeErrorMsg,all);
      
    } else {
      for(byte i = 0; i < numberOfDevices; i++) {
        if(offendingBays[i]) {
          char number[3];
          itoa(i + 1,number,10);
          strcat(completeErrorMsg,number);
          strcat_P(completeErrorMsg,comma);
        }
      }
    }
    strcat_P(completeErrorMsg,space);
  } else {
    strcat_P(completeErrorMsg,space);
  }
  //Serial.println(errorCode);
  //Append the error message
  //strcat_P(completeErrorMsg,errorMessages[errorCode]);
  strcat_P(completeErrorMsg, (PGM_P)pgm_read_word(&(errorMessages[errorCode])));
  //Send it to a computer (not using any formatting or anything) - will be ignored by other devices.
  //serialCom.sendString(completeErrorMsg);
  printNewLine();
  currentScreen = errorScreen;
  changeScreen();
  //Send to lcd
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
  /*strcat_P(timeSinceStart, colon);
  if (seconds < 10) {
    strcat_P(timeSinceStart, zero);
  }
  itoa(seconds, number, 10);
  strcat(timeSinceStart, number);*/
}
/*char * runningTime() {
  unsigned long milliSeconds = millis();
  byte minutes = milliSeconds / 60000 % 60;
  unsigned int hours = milliSeconds / 3600000;
  static char timeSinceStart[7];
  char number[5];
  itoa(hours,number,10);
  strcpy(timeSinceStart,number);
  strcat_P(timeSinceStart,colon);
  //Add a 0 to keep places
  if(minutes < 10) {
    strcat_P(timeSinceStart,zero);
  }
  itoa(minutes,number,10);
  strcat(timeSinceStart,number);
  return timeSinceStart;
}*/
void printNewLine() {
  //New line for serial port
  char charBuffer[3];
  strcpy_P(charBuffer,crnl);
  //serialCom.sendString(charBuffer);
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

