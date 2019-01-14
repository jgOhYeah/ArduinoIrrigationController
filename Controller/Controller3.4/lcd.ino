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
  lcd.cursor();
  callback.cancel(clockCallback);
  clockCallback = NONEXISTANT_CALLBACK;
  //char charBuffer[15];
  switch(currentScreen) {
    case LCD_INIT:
      //Print the initialising message
      lcd.clear();
      strcpy_P(charBuffer, initialising);
      //serialCom.sendString(charBuffer);
      lcd.write(charBuffer);
      lcd.noCursor();
      //printNewLine();
      //printNewLine();
      break;
    case LCD_MAIN:
      //Print out the bay numbers
      print1ToNumberOfBays(1);
      drawBayStates();
      lcd.setCursor(LCD_WIDTH-5,1); //11
      strcpy_P(charBuffer,stringMore);
      lcd.write(charBuffer);
      drawClock();
      clockCallback = callback.add(CLOCK_UPDATE_SPEED,true,drawClock);
      cursorPos = FIRST_BAY_INDEX;
      cursorRow = 1;
      //Stuff
      break;
    case LCD_MENU:
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
    case LCD_ERROR:
      //Serial.println(F("Error logs displayed"));
      //drawClock(); //Will be calledin drawErrorTop();
      clockCallback = callback.add(CLOCK_UPDATE_SPEED,true,drawClock);
      drawErrorTop();
      lcd.setCursor(0,1);
      lcd.write(completeErrorMsg);
      cursorPos = 0;
      cursorRow = 0;
      break;
    //Screens for remotely reading and writing eeprom in the slaves.
    case LCD_SETUP:
      cursorPos = 0;
      cursorRow = 0;
      strcpy_P(charBuffer,stringBack);
      lcd.write(charBuffer);
      lcd.setCursor(LCD_WIDTH-10,0);
      strcpy_P(charBuffer,stringSelectBay);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringBay);
      lcd.write(charBuffer);
      lcd.setCursor(0,1);
      print1ToNumberOfBays(FIRST_BAY_INDEX + 1);
      lcd.setCursor(LCD_WIDTH-7,1);
      strcpy_P(charBuffer,stringToEdit);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringEdit);
      lcd.write(charBuffer);
      break;
    case LCD_EDIT_EEPROM_1:
    case LCD_EDIT_EEPROM_2:
      //Top row
      strcpy_P(charBuffer,stringBack);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringEdit);
      lcd.write(charBuffer);
      strcpy_P(charBuffer,stringBay);
      lcd.write(charBuffer);
      lcd.setCursor(15,0);
      if(editingBay == NUMBER_OF_BAYS) {
        lcd.print('A');
      } else {
        lcd.print(editingBay+1);
      }
      //Bottom Row - different for pages 1 and 2
      lcd.setCursor(0,1);
      if(currentScreen == LCD_EDIT_EEPROM_1) { //First page
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
    case LCD_SET_VALUE:
      /*cursorPos = 0; //I SHOULD be able to remove this to get the cursor to remember where it is.
      cursorRow = 0;*/
      break;
  }
  lcd.setCursor(cursorPos,cursorRow);
}
void waitingMessage() { //Not a distinct screen as such
  lcd.noCursor();
  lcd.clear();
  strcpy_P(charBuffer,stringWaiting);
  lcd.write(charBuffer);
  //lcd.setCursor(LCD_WIDTH,0); //Set the cursor to a non visable area to temporarily hide it - does not work on 40 by 2 lcds as there is no blank space
}
void drawErrorTop() {
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
  lcd.setCursor(LCD_WIDTH-strlen(charBuffer)+scrollPos,0);
  //Serial.println(strlen(timeFormatted));
  lcd.write(charBuffer);
  lcd.setCursor(cursorPos+scrollPos,cursorRow);
  
}
void drawBayStates() {
  //Go to the row beneath and print out the status of each
  lcd.setCursor(0,1);
  for(byte i = 0; i < FIRST_BAY_INDEX; i++) { //Blank out unused bays
    lcd.write('-');
  }
  allTheSame = true;
  if(bayStatus[FIRST_BAY_INDEX] == STATE_UNKOWN || bayStatus[FIRST_BAY_INDEX] == STATE_NOT_PRESENT) {
    allTheSame = false;
  }
  for(byte i = FIRST_BAY_INDEX; i < NUMBER_OF_BAYS; i++) {
    lcd.write(byte(bayOptions[bayStatus[i]]));
    if(bayStatus[i] != bayStatus[FIRST_BAY_INDEX]) {
      allTheSame = false;
    }
  }
  lcd.write('*');
}

/* The screen should look like this:
0123456789ABCDEF
*Back __TITLE___
 < 000000000XY >
 */
void inputValueBottomRow() {
  //char charBuffer[11];
  byte xStartPos = LCD_WIDTH-maximumDigits-4; //Right align. 4 spots are for the buttons.
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
  if(valueSuffix1 != 0) { //If there is a suffix included, add it.
    lcd.write(valueSuffix1); //Should not write a null char
  }
  if(valueSuffix2 != 0) { //If there is a suffix included, add it.
    lcd.write(valueSuffix2); //Should not write a null char
  }
  strcpy_P(charBuffer, rightButtonSymbol);
  lcd.write(charBuffer);
  //That should take us to the last character
  lcd.home();
}
void print1ToNumberOfBays(byte startNumber) {
  for(byte i = startNumber; i <= NUMBER_OF_BAYS; i++) {
    lcd.print(i);
  }
  //Master control
  lcd.write('A');
}
