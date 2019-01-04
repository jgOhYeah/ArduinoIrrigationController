void menuButtons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    //stuff
    //Serial.println(F("Left button pressed"));
    //Currently Home button
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorRow = 1;
      cursorPos = 9;
    //CMD_RESET all
    } else if (cursorPos == 6) { //CMD_RESET all
      cursorPos = 0;
    } else if (cursorPos == 9) { //Settings
      cursorPos = 0;
    } else { //Last error message
      cursorRow = 0;
      cursorPos = 6;
    }
    lcd.setCursor(cursorPos,cursorRow) ;
  }
  if(rightButton.checkButton()) {
    //stuff
    //Serial.println(F("Right button pressed"));
    //Currently Home button
    if(cursorPos == 0 && cursorRow == 0) {
      cursorPos = 6;
    //CMD_RESET all
    } else if (cursorPos == 6) {
      cursorPos = 0;
      cursorRow = 1;
    } else if (cursorPos == 9) { //Settings
      cursorPos = 0;
      cursorRow = 0;
    //Error logs
    } else {
      cursorPos = 9;
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(selectButton.checkButton()) {
    //Currently Home button
    if(cursorPos == 0 && cursorRow == 0) {
      //Go to the home screen
      //Serial.println(F("Going to the home screen"));
      changeScreen(LCD_MAIN, LCD_MAIN);
    //CMD_RESET all
    } else if (cursorPos == 6) {
      digitalWrite(LED_BUILTIN,HIGH);
      bus.send(PJON_BROADCAST,CMD_RESET,1); //Send the command for everything to CMD_RESET. - broadcast as nothing can really be trusted if the system has given up enough to need to be CMD_RESET.
      wdt_enable(WDTO_4S); //This does not work on arduino nanos as there is a bug in the bootloader that crashes them until power is removed and reaplied.
      while(true) {
        bus.update();
        bus.receive();
      }
      //CMD_RESET everything
    } else if (cursorPos == 9) { //Settings
      changeScreen(LCD_SETUP,LCD_MENU);
    //Error logs
    } else {
      //Show the error logs
      //Serial.println(F("Error logs called"));
      changeScreen(LCD_ERROR, LCD_MENU);
    }
  }
}
void errorButtons() {
  BTN_ENABLE_CHECK();
  byte errorMsgLength = strlen(completeErrorMsg);
  //If not already the most left, scroll left.
  if(errorMsgLength > LCD_WIDTH && leftButton.checkButton() && scrollPos > 0) {
    //Scroll the message to the left if more to left.
    byte numberOfSkips = scrollPos;
    if(scrollPos < LCD_SCROLL_SKIPS) { //If the scrolling ends up being a few characters out and not a multiple of LCD_SCROLL_SKIPS
      scrollPos = 0;
    } else {
      scrollPos-= LCD_SCROLL_SKIPS;
    }
    numberOfSkips -= scrollPos; //Difference of before and after
    for(byte i = 0; i < numberOfSkips; i++) {
      lcd.scrollDisplayRight();
    }
    drawErrorTop();
    lcd.setCursor(scrollPos,0);
  }
  byte maxScrollPos = errorMsgLength - LCD_WIDTH; //Oveflow won't matter as checking in the if statement below
  if(errorMsgLength > LCD_WIDTH && rightButton.checkButton() && scrollPos < maxScrollPos) {
    byte numberOfSkips = scrollPos;
    if(scrollPos > maxScrollPos - LCD_SCROLL_SKIPS) { //If the scrolling ends up being a few characters out and not a multiple of LCD_SCROLL_SKIPS
      scrollPos = maxScrollPos;
    } else {
      scrollPos+= LCD_SCROLL_SKIPS;
    }
    numberOfSkips = scrollPos - numberOfSkips; //Difference of before and after
    for(byte i = 0; i < numberOfSkips; i++) {
      lcd.scrollDisplayLeft();
    }
    drawErrorTop();
    lcd.setCursor(scrollPos,0);
  }
  if(selectButton.checkButton()) {
    //Go to main screen
    //Serial.println(F("Select button pressed"));
    changeScreen(previousScreen,screenBeforeThat);
  }
}
//Handler for the buttons on the main screen.
void mainButtons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    //stuff
    //Serial.println(F("Left button pressed"));
    switch(cursorPos) {
      //Loop back to more button
      case FIRST_BAY_INDEX:
        cursorPos = LCD_WIDTH-5;
        break;
      //Skip a few to get back to selected
      case 11:
        cursorPos = NUMBER_OF_BAYS;
        break;
      default:
        cursorPos--;
        break;
    }
    lcd.setCursor(cursorPos,1);
  }
  if(rightButton.checkButton()) {
    //stuff
   // Serial.println(F("Right button pressed"));
    switch(cursorPos) {
      //Loop back to more button
      case LCD_WIDTH-5:
        cursorPos = FIRST_BAY_INDEX;
        break;
      //Skip a few to get back to selected
      case NUMBER_OF_BAYS:
        cursorPos = LCD_WIDTH-5;
        break;
      default:
        cursorPos++;
        break;
    }
    lcd.setCursor(cursorPos,1);
  }
  if(selectButton.checkButton()) {
    //stuff
    //Serial.println("Select button pressed");
    if(cursorPos == LCD_WIDTH-5) {
      //Call the menu screen if on the menu button
      changeScreen(LCD_MENU,LCD_MAIN);
      //Master Setting
    } else if (cursorPos == NUMBER_OF_BAYS) { //On the change all bays at once button
      byte masterStatus = STATE_SHUT;
      //Serial.println("All the same: " + allTheSame);
      if(allTheSame) {
        masterStatus = bayStatus[FIRST_BAY_INDEX];
        if(masterStatus == STATE_OPEN) {
          masterStatus = STATE_SHUT;
        } else {
          masterStatus++;
        }
      }
      //char charsToSend[3];
      //char charsToSend[3]; //For some reason this cannot be a local variable.
      charsToSend[0] = CMD_SET_STATUS;
      charsToSend[1] = masterStatus; //Prepare the data - needs to be written at runtime so putting in {} in one line does not seem to work.
      charsToSend[2] = char(0); //Null terminate the array just in cse something attempts to use it as a string.
      sendToAll(charsToSend,2);
      //bus.update();
      lcd.setCursor(FIRST_BAY_INDEX,1);
      byte bayOptionsChar = byte(bayOptions[masterStatus]);
      for(byte i=FIRST_BAY_INDEX; i < NUMBER_OF_BAYS; i++) {
        bayStatus[i] = masterStatus;
        lcd.write(bayOptionsChar);
      }
      lcd.setCursor(cursorPos,1);
      allTheSame = true;
    } else { //Change the state of an individual bay.
      if(bayStatus[cursorPos] >= STATE_OPEN) {
        bayStatus[cursorPos] = STATE_SHUT;
      } else {
        bayStatus[cursorPos]++;
      }
      //char charsToSend[3];
      //char charsToSend[3]; //For some reason this cannot be a local variable.
      charsToSend[0] = CMD_SET_STATUS;
      charsToSend[1] = bayStatus[cursorPos]; //Prepare the data
      charsToSend[2] = char(0);
      bus.send(cursorPos+FIRST_BAY_ADDRESS,charsToSend,2); //Send the data
      bus.update();
      //Send the command to the bay
      //if(!sendBayCommand(cursorPos,bayStatus[cursorPos])) return;
      //byte error = serialSuccess; //serialCom.sendCommandWithCallbacks(cursorPos,bayStatus[cursorPos],CMD_OVERWRITE_STATUS, enableLineWait, callbackTimeout);
      //Update the lcd.
      lcd.write(byte(bayOptions[bayStatus[cursorPos]]));
      lcd.setCursor(cursorPos,1);
      //Check if still all the same
      allTheSame = true;
      if(bayStatus[FIRST_BAY_INDEX] == STATE_UNKOWN || bayStatus[FIRST_BAY_INDEX] == STATE_NOT_PRESENT) {
        allTheSame = false;
      }
      for(byte i = FIRST_BAY_INDEX; i < NUMBER_OF_BAYS; i++) {
        if(bayStatus[i] != bayStatus[FIRST_BAY_INDEX]) {
          allTheSame = false;
        }
      }
      lcd.setCursor(cursorPos,1);
    }
  }
}
void inputValueScreenButtons() {
  BTN_ENABLE_CHECK();
  if(leftButton.pressEvery(BUTTON_INITIAL_DELAY,BUTTON_REPEAT_DELAY)) { //Decrease the value
    if(currentNumber > minimumValue + valueStepSize) {
      currentNumber -= valueStepSize;
    } else {
      currentNumber = minimumValue;
    }
    inputValueBottomRow();
  }
  if(rightButton.pressEvery(BUTTON_INITIAL_DELAY,BUTTON_REPEAT_DELAY)) { //Increase the value
    if(currentNumber < maximumValue - valueStepSize) {
      currentNumber += valueStepSize;
    } else {
      currentNumber = maximumValue;
    }
    inputValueBottomRow();
  }
  if(selectButton.checkButton()) { //Return the value
    changeScreen(previousScreen,screenBeforeThat); //Go back to the last screen
    callbackFunctionStored(currentNumber); //Return the number
  }
}
void setupMenuButtons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorRow = 1;
      cursorPos = NUMBER_OF_BAYS - FIRST_BAY_INDEX;
    //CMD_RESET all
    } else if (cursorPos == 0 && cursorRow == 1) { //Far left of bottom row.
      cursorRow = 0;
    } else {
      cursorPos--;
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(rightButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorRow = 1;
      cursorPos = 0;
    //CMD_RESET all
    } else if (cursorPos == (NUMBER_OF_BAYS - FIRST_BAY_INDEX) && cursorRow == 1) { //Far right of bottom row.
      cursorRow = 0;
      cursorPos = 0;
    } else { //Left and middle of bottom row
      cursorPos++;
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(selectButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //On the back button
      changeScreen(previousScreen,LCD_MAIN);
    } else { //Edit either 1 or all bays.
      editingBay = cursorPos+FIRST_BAY_INDEX;
      cursorPos = 0;
      cursorRow = 0;
      changeScreen(LCD_EDIT_EEPROM_1,LCD_SETUP);
    }
  }
}
void eepromScreen1Buttons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorPos = 11;
      cursorRow = 1;
      changeScreen(LCD_EDIT_EEPROM_2,previousScreen);
    //CMD_RESET all
    } else if (cursorPos == 0 && cursorRow == 1) { //Far left of bottom row.
      cursorRow = 0;
    } else {
      cursorPos = 0;
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(rightButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorRow = 1;
      cursorPos = 0;
    } else if (cursorPos == 0 && cursorRow == 1) { //Bottom left
      cursorPos = 4;
    } else { //Bottom right
      cursorRow = 1;
      cursorPos = 1;
      changeScreen(LCD_EDIT_EEPROM_2,previousScreen); //Go to the next screen
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(selectButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //On the back button
      changeScreen(previousScreen,LCD_MENU);
    } else {
      char setting;
      byte address = editingBay+FIRST_BAY_ADDRESS;
      if(cursorPos == 0) { //Edit up time
        setting = SETTING_UP_SPEED;
      } else { //Edit down time
        setting = SETTING_DOWN_SPEED;
      }
      //Ignoring for now as other functions will pick this up and deal with it.
      /*if(editingBay == NUMBER_OF_BAYS) { //Editing all bays, so get the setting from the first bay
        address = FIRST_BAY_ADDRESS+FIRST_BAY_INDEX;
      }*/
      buttonsEnabled = false; //Disable the buttons so that they cannot press anything unwanted when trying to get data from the slave
      waitingMessage();
      retrieveEepromNumber(address,setting,retrievedEepromValue);
    }
  }
}
void eepromScreen2Buttons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    if (cursorPos == 1) { //Far left of bottom row.
      cursorPos = 4;
      cursorRow = 1;
      changeScreen(LCD_EDIT_EEPROM_1,previousScreen);
    } else { //Far right
      cursorPos = 1;
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(rightButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorRow = 1;
      cursorPos = 1;
    //CMD_RESET all
    } else if (cursorPos == 1) { //Bottom left
      cursorPos = 11;
    } else { //Bottom right
      cursorRow = 0;
      cursorPos = 0;
      changeScreen(LCD_EDIT_EEPROM_1,previousScreen);
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(selectButton.checkButton()) {
    //Do something. The cursor will be on one of the two buttons in the bottom row
    char setting;
    byte address = editingBay+FIRST_BAY_ADDRESS;
    if(cursorPos == 1) { //Edit up time
      setting = SETTING_HALF_POS;
    } else { //Edit down time
      setting = SETTING_BAUD_RATE;
    }
    //Ignoring for now as other functions will pick this up and deal with it.
    /*if(editingBay == NUMBER_OF_BAYS) { //Editing all bays, so get the setting from the first bay
      address = FIRST_BAY_ADDRESS+FIRST_BAY_INDEX;
    }*/
    buttonsEnabled = false; //Disable the buttons so that they cannot press anything unwanted when trying to get data from the slave
    waitingMessage();
    retrieveEepromNumber(address,setting,retrievedEepromValue);
  }
}
