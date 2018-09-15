void setupButtons() {
  //Set up the buttons
  leftButton.assign(buttonLeft);
  rightButton.assign(buttonRight);
  selectButton.assign(buttonSelect);
  
  leftButton.setMode(buttonMode);
  leftButton.setTimer(buttonTimer);
  leftButton.setRefresh(buttonRefresh);
  
  rightButton.setMode(buttonMode);
  rightButton.setTimer(buttonTimer);
  rightButton.setRefresh(buttonRefresh);
  
  selectButton.setMode(buttonMode);
  selectButton.setTimer(buttonTimer);
  selectButton.setRefresh(buttonRefresh);
}
void menuScreenButtons() {
  if(leftButton.check() == ON) {
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
    lcd.setCursor(cursorPos,cursorRow);
  }
  if(rightButton.check() == ON) {
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
  if(selectButton.check() == ON) {
    //stuff
    //Serial.println(F("Select button pressed"));
    //Currently Home button
    if(cursorPos == 0 && cursorRow == 0) {
      //Go to the home screen
      //Serial.println(F("Going to the home screen"));
      currentScreen = mainScreen;
      changeScreen();
    //Reset all
    } else if (cursorPos == 6) {
      Serial.println(F("Resetting everything"));
      if(!sendBayCommand(allBays,resetBays)) return;
      //Set the watchdog to timeout and reset 
      wdt_enable(WDTO_15MS);
      while(true);
      //Reset everything
    //Error logs
    } else {
      //Show the error logs
      //Serial.println(F("Error logs called"));
      currentScreen = errorScreen;
      changeScreen();
    }
  }
}
void errorScreenButtons() {
  //If not already the most left, scroll left.
  if(errorMessage.length() > 16 && leftButton.check() == ON && scrollPos > 0) {
    //Scroll the message to the left if more to left.
    scrollPos-= scrollSkips;
    for(byte i = 0; i < scrollSkips; i++) {
      lcd.scrollDisplayRight();
    }
    drawErrorScreenTop();
    lcd.setCursor(scrollPos,0);
  }
  if(errorMessage.length() > 16 && rightButton.check() == ON && scrollPos < errorMessage.length() - 16) {
    scrollPos+= scrollSkips;
    for(byte i = 0; i < scrollSkips; i++) {
      lcd.scrollDisplayLeft();
    }
    drawErrorScreenTop();
    lcd.setCursor(scrollPos,0);
  }
  if(selectButton.check() == ON) {
    //stuff
    //Serial.println(F("Select button pressed"));
    currentScreen = mainScreen;
    changeScreen();
  }
}
void mainScreenButtons() {
  if(leftButton.check() == ON) {
    //stuff
    //Serial.println(F("Left button pressed"));
    switch(cursorPos) {
      //Loop back to more button
      case 0:
        cursorPos = 11;
        break;
      //Skip a few to get back to selected
      case 11:
        cursorPos = numberOfBays;
        break;
      default:
        cursorPos--;
        break;
    }
    lcd.setCursor(cursorPos,1);
  }
  if(rightButton.check() == ON) {
    //stuff
    //Serial.println(F("Right button pressed"));
    switch(cursorPos) {
      //Loop back to more button
      case 11:
        cursorPos = 0;
        break;
      //Skip a few to get back to selected
      case numberOfBays:
        cursorPos = 11;
        break;
      default:
        cursorPos++;
        break;
    }
    lcd.setCursor(cursorPos,1);
  }
  if(selectButton.check() == ON) {
    //stuff
    //Serial.println("Select button pressed");
    if(cursorPos == 11) {
      //Call the menu screen if on the menu button
      currentScreen = menuScreen;
      changeScreen();
      //Master Setting
    } else if (cursorPos == numberOfBays) {
      byte masterStatus;
      //Serial.println("All the same: " + allTheSame);
      if(allTheSame) {
        masterStatus = bayStatus[0];
        if(masterStatus == bayOpen) {
          masterStatus = bayShut;
        } else {
          masterStatus++;
        }
      } else {
        masterStatus = bayShut;
      }
      if(!sendBayCommand(allBays,masterStatus)) return;
      lcd.setCursor(0,1);
      for(byte i = 0; i < numberOfBays; i++) {
        bayStatus[i] = masterStatus;
        lcd.write(byte(bayOptions[masterStatus]));
      }
      lcd.setCursor(cursorPos,1);
      allTheSame = true;
      //Else if the bay is connected and online, increment through the options.
    //} else if (bayStatus[cursorPos] != bayNotPresent) {
    } else {
      if(bayStatus[cursorPos] >= bayOpen) {
        bayStatus[cursorPos] = bayShut;
      } else {
        bayStatus[cursorPos]++;
      }
      //Send the command to the bay
      if(!sendBayCommand(cursorPos,bayStatus[cursorPos])) return;
      //Write the updated status to the lcd
      lcd.write(byte(bayOptions[bayStatus[cursorPos]]));
      lcd.setCursor(cursorPos,1);
      //Check if still all the same
      allTheSame = true;
      if(bayStatus[0] == bayUnknown || bayStatus[0] == bayNotPresent) {
        allTheSame = false;
      }
      for(byte i = 0; i < numberOfBays; i++) {
        if(bayStatus[i] != bayStatus[0]) {
          allTheSame = false;
        }
      }
      /*lcd.setCursor(numberOfBays,1);*/
      /*if(allTheSame) {
        lcd.write(byte(bayOptions[bayStatus[0]]));
      } else {
        lcd.write(' ');
      }*/
      lcd.setCursor(cursorPos,1);
    }
  }
}
