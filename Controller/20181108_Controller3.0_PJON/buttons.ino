void menuScreenButtons() {
  if(leftButton.checkButton()) {
    //stuff
    //Serial.println(F("Left button pressed"));
    //Currently Home button
    if(cursorPos == 0 && cursorRow == 0) {
      cursorRow = 1;
    //Reset all
    } else if (cursorPos == 6) {
      cursorPos = 0;
    //Error logs
    } else {
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
    //Error logs
    } else {
      cursorRow = 0;
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
    //Error logs
    } else {
      //Show the error logs
      //Serial.println(F("Error logs called"));
      changeScreen(errorScreen, menuScreen);
    }
  }
}
void errorScreenButtons() {
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
  
}

