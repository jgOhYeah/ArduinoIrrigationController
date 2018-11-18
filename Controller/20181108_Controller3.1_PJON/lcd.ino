void setupLcd() {
  //Print the welcome message
  lcd.clear();
  //char charBuffer[30];//Originally was 16
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

void changeScreen(byte nextMode, byte previousMode) {
  buttonsEnabled = true; //Reenable the buttons if they have been disabled
  previousScreen = previousMode;
  currentScreen = nextMode;
 //When Changing screen
  scrollPos = 0;
  lcd.clear();
  //char charBuffer[15];
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
      print1ToNumberOfDevices(1);
      drawBayStates();
      lcd.setCursor(11,1);
      strcpy_P(charBuffer,stringMore);
      lcd.write(charBuffer);
      drawClock();
      cursorPos = firstDevice;
      cursorRow = 1;
      //Stuff
      break;
    case menuScreen:
      strcpy_P(charBuffer,stringBack);
      lcd.write(charBuffer);
      strcpy_P(charBuffer, stringResetAll);
      lcd.write(charBuffer);
      lcd.setCursor(0,1);
      strcpy_P(charBuffer, stringLastErrMsg);
      lcd.write(charBuffer);
      strcpy_P(charBuffer, stringSettings);
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
    //Screens for remotely reading and writing eeprom in the slaves.
    case setupScreen:
      cursorPos = 0;
      cursorRow = 0;
      strcpy_P(charBuffer,stringBack);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringSelectBay);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringBay);
      lcd.write(charBuffer);
      lcd.setCursor(0,1);
      print1ToNumberOfDevices(firstDevice + 1);
      lcd.setCursor(9,1);
      strcpy_P(charBuffer,stringToEdit);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringEdit);
      lcd.write(charBuffer);
      break;
    case editEepromScreen1:
    case editEepromScreen2:
      //Top row
      strcpy_P(charBuffer,stringBack);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringEdit);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringBay);
      lcd.write(charBuffer);
      lcd.setCursor(15,0);
      if(editingBay == numberOfDevices) {
        lcd.print('A');
      } else {
        lcd.print(editingBay+1);
      }
      //Bottom Row - different for pages 1 and 2
      lcd.setCursor(0,1);
      if(currentScreen == editEepromScreen1) { //First page
        lcd.write('*');
        strcpy_P(charBuffer,stringUp);
        lcd.write(charBuffer);
        lcd.write(' ');
        lcd.write('*');
        strcpy_P(charBuffer,stringDown);
        lcd.write(charBuffer);
        strcpy_P(charBuffer,stringTime);
        lcd.write(charBuffer);
        strcpy_P(charBuffer,rightButtonSymbol);
        lcd.write(charBuffer);
      } else { //Second page
        lcd.write('<');
        lcd.write('*');
        strcpy_P(charBuffer,stringHalfPos);
        lcd.write(charBuffer);
        lcd.write(' ');
        lcd.write('*');
        strcpy_P(charBuffer,stringBaud);
        lcd.write(charBuffer);
      }
      break;
    case setValueScreen:
      cursorPos = 0;
      cursorRow = 0;
      break;
  }
  lcd.setCursor(cursorPos,cursorRow);
}
void waitingMessage() { //Not a distinct screen as such
  lcd.clear();
  strcpy_P(charBuffer,stringWaiting);
  lcd.write(charBuffer);
  lcd.setCursor(16,0); //Set the cursor to a non visable area to temporarily hide it
}
void drawErrorScreenTop() {
  //char charBuffer[7];
  lcd.setCursor(0+scrollPos,0);
  strcpy_P(charBuffer, stringBack);
  lcd.write(charBuffer);
  strcpy_P(charBuffer, stringError);
  lcd.write(charBuffer);
  drawClock();
}
void drawClock() {
  //char charBuff[8];
  runningTime(charBuffer);
  lcd.setCursor(16-strlen(charBuffer)+scrollPos,0);
  //Serial.println(strlen(timeFormatted));
  lcd.write(charBuffer);
}
void drawBayStates() {
  //Go to the row beneath and print out the status of each
  lcd.setCursor(0,1);
  for(byte i = 0; i < firstDevice; i++) { //Blank out unused bays
    lcd.write('-');
  }
  allTheSame = true;
  if(bayStatus[firstDevice] == bayUnknown || bayStatus[firstDevice] == bayNotPresent) {
    allTheSame = false;
  }
  for(byte i = firstDevice; i < numberOfDevices; i++) {
    lcd.write(byte(bayOptions[bayStatus[i]]));
    if(bayStatus[i] != bayStatus[firstDevice]) {
      allTheSame = false;
    }
  }
  lcd.write('*');
}

/* The screen should look like this:
0123456789ABCDEF
*Back __TITLE___
 < 0000000000X >
 */
void inputValueBottomRow() {
  //char charBuffer[11];
  byte xStartPos = 16-maximumDigits-4; //Right align. 4 spots are for the buttons.
  if(valueSuffix1 != 0) { //If there is a suffix included, add a space for it.
    xStartPos--;
  }
  if(valueSuffix2 != 0) {
    xStartPos--;
  }
  lcd.setCursor(xStartPos,1);
  strcpy_P(charBuffer, leftButtonSymbol);
  lcd.write(charBuffer);
  ltoa(currentNumber,charBuffer,10); //Convert the default number to a string.
  byte numberOfSpaces = maximumDigits - strlen(charBuffer); //Add spaces to right justify
  for(byte i = 0; i < numberOfSpaces; i++) {
    lcd.write(' ');
  }
  lcd.write(charBuffer);
  lcd.write(valueSuffix1); //Should not write a null char
  lcd.write(valueSuffix2);
  strcpy_P(charBuffer, rightButtonSymbol);
  lcd.write(charBuffer);
  //That should take us to the last character
  lcd.home();
}
void print1ToNumberOfDevices(byte startNumber) {
  for(byte i = startNumber; i <= numberOfDevices; i++) {
    lcd.print(i);
  }
  //Master control
  lcd.write('A');
}

