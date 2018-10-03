void setupLcd() {
  //Print the welcome message
  lcd.clear();
  char charBuffer[30];//Originally was 16
  //"Irrigation "
  strcpy_P(charBuffer, irrigation);
  lcd.write(charBuffer);
  lcd.setCursor(0,1);
  //"Controller"
  strcpy_P(charBuffer, controller);
  lcd.write(charBuffer);
  //" V2.0"
  strcpy_P(charBuffer, softwareVersion);
  lcd.write(charBuffer);
  //PrintNewLine
  //printNewLine();
}

void changeScreen() {
 //When Changing screen
  scrollPos = 0;
  lcd.clear();
  char charBuffer[15];
  switch(currentScreen) {
    case initScreen:
      //Print the initialising message
      lcd.clear();
      strcpy_P(charBuffer, initialising);
      //serialCom.sendString(charBuffer);
      lcd.write(charBuffer);
      //printNewLine();
      //printNewLine();
      lcd.cursor();
      break;
    case mainScreen:
      //Print out the bay numbers
      for(byte i = 1; i <= numberOfDevices; i++) {
        lcd.print(i);
      }
      //Master control
      lcd.print('A');
      
      //Go to the row beneath and print out the status of each
      lcd.setCursor(0,1);
      allTheSame = true;
      if(bayStatus[0] == bayUnknown || bayStatus[0] == bayNotPresent) {
        allTheSame = false;
      }
      for(byte i = 0; i < numberOfDevices; i++) {
        lcd.write(byte(bayOptions[bayStatus[i]]));
        if(bayStatus[i] != bayStatus[0]) {
          allTheSame = false;
        }
      }
      lcd.write('*');
      lcd.setCursor(11,1);
      strcpy_P(charBuffer,stringMore);
      lcd.write(charBuffer);
      drawClock();
      cursorPos = 0;
      cursorRow = 1;
      //Stuff
      break;
    case menuScreen:
      strcpy_P(charBuffer,stringHome);
      lcd.write(charBuffer);
      strcpy_P(charBuffer, stringResetAll);
      lcd.write(charBuffer);
      lcd.setCursor(0,1);
      strcpy_P(charBuffer, stringLastErrMsg);
      lcd.write(charBuffer);
      cursorPos = 0;
      cursorRow = 0;
      break;
    case errorScreen:
      //Serial.println(F("Error logs displayed"));
      drawErrorScreenTop();
      lcd.setCursor(0,1);
      lcd.write(completeErrorMsg);
      cursorPos = 0;
      cursorRow = 0;
      break;
  }
  lcd.setCursor(cursorPos,cursorRow);
}
void drawErrorScreenTop() {
  char charBuffer[7];
  lcd.setCursor(0+scrollPos,0);
  strcpy_P(charBuffer, stringHome);
  lcd.write(charBuffer);
  strcpy_P(charBuffer, stringError);
  lcd.write(charBuffer);
  drawClock();
}
void drawClock() {
  char charBuff[8];
  runningTime(charBuff);
  lcd.setCursor(16-strlen(charBuff)+scrollPos,0);
  //Serial.println(strlen(timeFormatted));
  lcd.write(charBuff);
}


