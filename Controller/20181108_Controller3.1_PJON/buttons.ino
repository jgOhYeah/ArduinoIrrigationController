void menuScreenButtons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    //stuff
    //Serial.println(F("Left button pressed"));
    //Currently Home button
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorRow = 1;
      cursorPos = 9;
    //Reset all
    } else if (cursorPos == 6) { //Reset all
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
    //Reset all
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
      changeScreen(mainScreen, mainScreen);
    //Reset all
    } else if (cursorPos == 6) {
      digitalWrite(LED_BUILTIN,HIGH);
      bus.send(PJON_BROADCAST,reset,1); //Send the command for everything to reset. - broadcast as nothing can really be trusted if the system has given up enough to need to be reset.
      serialDelay(replyDelay); //Wait long enough to hopefully send the message, otherwise too bad.
      //Set the watchdog to timeout and reset 
      wdt_enable(WDTO_15MS); //This does not work on arduino nanos as there is a bug in the bootloader that crashes them until power is removed and reaplied.
      while(true);
      //Reset everything
    } else if (cursorPos == 9) { //Settings
      changeScreen(setupScreen,menuScreen);
    //Error logs
    } else {
      //Show the error logs
      //Serial.println(F("Error logs called"));
      changeScreen(errorScreen, menuScreen);
    }
  }
}
void errorScreenButtons() {
  BTN_ENABLE_CHECK();
  byte errorMsgLength = strlen(completeErrorMsg);
  //If not already the most left, scroll left.
  if(errorMsgLength > 16 && leftButton.checkButton() && scrollPos > 0) {
    //Scroll the message to the left if more to left.
    scrollPos-= scrollSkips;
    for(byte i = 0; i < scrollSkips; i++) {
      lcd.scrollDisplayRight();
    }
    drawErrorScreenTop();
    lcd.setCursor(scrollPos,0);
  }
  if(errorMsgLength > 16 && rightButton.checkButton() && scrollPos < errorMsgLength - 16) {
    scrollPos+= scrollSkips;
    for(byte i = 0; i < scrollSkips; i++) {
      lcd.scrollDisplayLeft();
    }
    drawErrorScreenTop();
    lcd.setCursor(scrollPos,0);
  }
  if(selectButton.checkButton()) {
    //Go to main screen
    //Serial.println(F("Select button pressed"));
    changeScreen(previousScreen,mainScreen);
  }
}
//Handler for the buttons on the main screen.
void mainScreenButtons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    //stuff
    //Serial.println(F("Left button pressed"));
    switch(cursorPos) {
      //Loop back to more button
      case firstDevice:
        cursorPos = 11;
        break;
      //Skip a few to get back to selected
      case 11:
        cursorPos = numberOfDevices;
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
      case 11:
        cursorPos = firstDevice;
        break;
      //Skip a few to get back to selected
      case numberOfDevices:
        cursorPos = 11;
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
    if(cursorPos == 11) {
      //Call the menu screen if on the menu button
      changeScreen(menuScreen,mainScreen);
      //Master Setting
    } else if (cursorPos == numberOfDevices) { //On the change all bays at once button
      byte masterStatus = bayShut;
      //Serial.println("All the same: " + allTheSame);
      if(allTheSame) {
        masterStatus = bayStatus[firstDevice];
        if(masterStatus == bayOpen) {
          masterStatus = bayShut;
        } else {
          masterStatus++;
        }
      }
      //char charsToSend[3];
      //char charsToSend[3]; //For some reason this cannot be a local variable.
      charsToSend[0] = setStatus;
      charsToSend[1] = masterStatus; //Prepare the data - needs to be written at runtime so putting in {} in one line does not seem to work.
      charsToSend[2] = char(0); //Null terminate the array just in cse something attempts to use it as a string.
      sendToAll(charsToSend,2);
      //bus.update();
      lcd.setCursor(firstDevice,1);
      byte bayOptionsChar = byte(bayOptions[masterStatus]);
      for(byte i=firstDevice; i < numberOfDevices; i++) {
        bayStatus[i] = masterStatus;
        lcd.write(bayOptionsChar);
      }
      lcd.setCursor(cursorPos,1);
      allTheSame = true;
    } else { //Change the state of an individual bay.
      if(bayStatus[cursorPos] >= bayOpen) {
        bayStatus[cursorPos] = bayShut;
      } else {
        bayStatus[cursorPos]++;
      }
      //char charsToSend[3];
      //char charsToSend[3]; //For some reason this cannot be a local variable.
      charsToSend[0] = setStatus;
      charsToSend[1] = bayStatus[cursorPos]; //Prepare the data
      charsToSend[2] = char(0);
      bus.send(cursorPos+firstSlaveAddress,charsToSend,2); //Send the data
      bus.update();
      //Send the command to the bay
      //if(!sendBayCommand(cursorPos,bayStatus[cursorPos])) return;
      //byte error = serialSuccess; //serialCom.sendCommandWithCallbacks(cursorPos,bayStatus[cursorPos],overwriteStatus, enableLineWait, callbackTimeout);
      //Update the lcd.
      lcd.write(byte(bayOptions[bayStatus[cursorPos]]));
      lcd.setCursor(cursorPos,1);
      //Check if still all the same
      allTheSame = true;
      if(bayStatus[firstDevice] == bayUnknown || bayStatus[firstDevice] == bayNotPresent) {
        allTheSame = false;
      }
      for(byte i = firstDevice; i < numberOfDevices; i++) {
        if(bayStatus[i] != bayStatus[firstDevice]) {
          allTheSame = false;
        }
      }
      lcd.setCursor(cursorPos,1);
    }
  }
}
void inputValueScreenButtons() {
  BTN_ENABLE_CHECK();
  if(leftButton.pressEvery(btnInitialDelay,btnRepeatDelay)) { //Decrease the value
    if(currentNumber > minimumValue + valueStepSize) {
      currentNumber -= valueStepSize;
    } else {
      currentNumber = minimumValue;
    }
    inputValueBottomRow();
  }
  if(rightButton.pressEvery(btnInitialDelay,btnRepeatDelay)) { //Increase the value
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
void setupScreenButtons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorRow = 1;
      cursorPos = numberOfDevices - firstDevice;
    //Reset all
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
    //Reset all
    } else if (cursorPos == (numberOfDevices - firstDevice) && cursorRow == 1) { //Far right of bottom row.
      cursorRow = 0;
      cursorPos = 0;
    } else { //Left and middle of bottom row
      cursorPos++;
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(selectButton.checkButton()) {
    editingBay = cursorPos+firstDevice;
    if(cursorPos == 0 && cursorRow == 0) { //On the back button
      changeScreen(previousScreen,mainScreen);
    } else { //Edit either 1 or all bays.
      cursorPos = 0;
      cursorRow = 0;
      changeScreen(editEepromScreen1,setupScreen);
    }
  }
}
void eepromScreen1Buttons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorPos = 11;
      cursorRow = 1;
      changeScreen(editEepromScreen2,previousScreen);
    //Reset all
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
      changeScreen(editEepromScreen2,previousScreen); //Go to the next screen
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(selectButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //On the back button
      changeScreen(previousScreen,menuScreen);
    } else if(cursorPos == 0) { //Edit up time
      buttonsEnabled = false; //Disable the buttons so that they cannot press anything unwanted when trying to get data from the slave
      waitingMessage();
      retrieveEepromNumber(editingBay+firstSlaveAddress,uTravelSpeed,retrievedEepromValue);
      //char charBuffer[17];
    } else { //Edit down time
      buttonsEnabled = false; //Disable the buttons so that they cannot press anything unwanted when trying to get data from the slave
      waitingMessage();
      retrieveEepromNumber(editingBay+firstSlaveAddress,dTravelSpeed,retrievedEepromValue);
    }
  }
}
void eepromScreen2Buttons() {
  BTN_ENABLE_CHECK();
  if(leftButton.checkButton()) {
    if (cursorPos == 1) { //Far left of bottom row.
      cursorPos = 4;
      cursorRow = 1;
      changeScreen(editEepromScreen1,previousScreen);
    } else { //Far right
      cursorPos = 1;
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(rightButton.checkButton()) {
    if(cursorPos == 0 && cursorRow == 0) { //Back Button
      cursorRow = 1;
      cursorPos = 1;
    //Reset all
    } else if (cursorPos == 1) { //Bottom left
      cursorPos = 11;
    } else { //Bottom right
      cursorRow = 0;
      cursorPos = 0;
      changeScreen(editEepromScreen1,previousScreen);
    }
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(selectButton.checkButton()) {
    //Do something. The cursor will be on one of the two buttons in the bottom row
    buttonsEnabled = false; //Disable the buttons so that they cannot press anything unwanted when trying to get data from the slave
    waitingMessage();
    if(cursorPos == 1) { //Edit half position
      retrieveEepromNumber(editingBay+firstSlaveAddress,halfPos,retrievedEepromValue);
      //char charBuffer[17];
    } else { //Edit baud rate
      retrieveEepromNumber(editingBay+firstSlaveAddress,eBaudRate,retrievedEepromValue);
    }
  }
}
