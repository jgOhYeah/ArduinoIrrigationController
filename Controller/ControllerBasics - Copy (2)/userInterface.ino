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
  switch(leftButton.check()) {
    case ON:
      //stuff
      Serial.println(F("Left button pressed"));
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
      break;
    case Hold:
        Serial.println(F("Left button on HOLD"));
      //Stuff
      break;
  }
  switch(rightButton.check()) {
    case ON:
      //stuff
      Serial.println(F("Right button pressed"));
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
      break;
    case Hold:
      Serial.println(F("Right button on HOLD"));
      //Stuff
      break;
  }
  switch(selectButton.check()) {
    case ON:
      //stuff
      Serial.println(F("Select button pressed"));
      //Currently Home button
      if(cursorPos == 0 && cursorRow == 0) {
        //Go to the home screen
        Serial.println(F("Going to the home screen"));
        currentScreen = mainScreen;
        changeScreen();
      //Reset all
      } else if (cursorPos == 6) {
        Serial.println(F("Resetting everything in theory"));
        //Reset everything
      //Error logs
      } else {
        //Show the error logs
        Serial.println(F("Error logs called"));
        currentScreen = errorScreen;
        changeScreen();
      }
      break;
    case Hold:
      Serial.println(F("Select button on HOLD"));
      //Stuff
      break;
  }
}
void errorScreenButtons() {
  switch(selectButton.check()) {
    case ON:
      //stuff
      Serial.println(F("Select button pressed"));
      currentScreen = mainScreen;
      changeScreen();
      break;
    case Hold:
      Serial.println(F("Select button on HOLD"));
      //Stuff
      break;
  }
}
void mainScreenButtons() {
  switch(leftButton.check()) {
    case ON:
      //stuff
      Serial.println("Left button pressed");
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
      break;
    case Hold:
        Serial.println("Left button on HOLD");
      //Stuff
      break;
  }
  switch(rightButton.check()) {
    case ON:
      //stuff
      Serial.println("Right button pressed");
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
      break;
    case Hold:
      Serial.println("Right button on HOLD");
      //Stuff
      break;
  }
  switch(selectButton.check()) {
    case ON:
      //stuff
      Serial.println("Select button pressed");
      if(cursorPos == 11) {
        //Call the menu screen if on the menu button
        currentScreen = menuScreen;
        changeScreen();
        //Master Setting
      } else if (cursorPos == numberOfBays) {
        byte masterStatus;
        Serial.println("All the same: " + allTheSame);
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
        lcd.setCursor(0,1);
        for(byte i = 0; i < numberOfBays; i++) {
          bayStatus[i] = masterStatus;
          sendBayCommand(i,masterStatus);
          lcd.write(byte(bayOptions[masterStatus]));
        }
        lcd.write(byte(bayOptions[masterStatus]));
        lcd.setCursor(cursorPos,1);
        allTheSame = true;
        //Else if the bay is connected and online, increment through the options.
      } else if (bayStatus[cursorPos] != bayNotPresent) {
        if(bayStatus[cursorPos] == bayOpen) {
          bayStatus[cursorPos] = bayShut;
        } else {
          bayStatus[cursorPos]++;
        }
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
        lcd.setCursor(numberOfBays,1);
        if(allTheSame) {
          lcd.write(byte(bayOptions[bayStatus[0]]));
        } else {
          lcd.write(' ');
        }
        lcd.setCursor(cursorPos,1);
        //Send the command to the bay
        sendBayCommand(cursorPos,bayStatus[cursorPos]);
      }
      break;
    case Hold:
      Serial.println("Select button on HOLD");
      //Stuff
      break;
  }
}
void updateTime() {
  //Every few seconds
  static unsigned long nextTime = 0;
  unsigned long milliSeconds = millis();
  if(milliSeconds >= nextTime) {
    nextTime = milliSeconds + clockUpdateSpeed;
    drawClock();
    lcd.setCursor(cursorPos,1);
  }
}
void drawClock() {
  unsigned long milliSeconds = millis();
  byte minutes = milliSeconds / 60000 % 60;
  unsigned int hours = milliSeconds / 3600000;
  String timeSinceLaunch;
  if(minutes < 10) {
    timeSinceLaunch = String(hours) + ":0" + String(minutes);
  } else {
    timeSinceLaunch = String(hours) + ':' + String(minutes);
  }
  lcd.setCursor(16-timeSinceLaunch.length(),0);
  lcd.print(timeSinceLaunch);
  Serial.println("Time: " + timeSinceLaunch);
}
void changeScreen() {
  //When Changing screen
  lcd.clear();
  switch(currentScreen) {
    case mainScreen:
      //Print out the bay numbers
      for(byte i = 1; i <= numberOfBays; i++) {
        lcd.print(i);
      }
      //Master control
      lcd.print('A');
      
      //Go to the row beneath and print out the staus of each
      lcd.setCursor(0,1);
      allTheSame = true;
      if(bayStatus[0] == bayUnknown || bayStatus[0] == bayNotPresent) {
        allTheSame = false;
      }
      for(byte i = 0; i < numberOfBays; i++) {
        lcd.write(byte(bayOptions[bayStatus[i]]));
        if(bayStatus[i] != bayStatus[0]) {
          allTheSame = false;
        }
      }
      if(allTheSame) {
        lcd.write(byte(bayOptions[bayStatus[0]]));
      }
      lcd.setCursor(11,1);
      lcd.print(F("*More"));
      drawClock();
      lcd.setCursor(0,1);
      cursorPos = 0;
      //Stuff
      break;
    case menuScreen:
      //Stuff
      Serial.println(F("Menu screen called"));
      lcd.print(F("*Home *Reset all"));
      lcd.setCursor(0,1);
      lcd.print(F("*Last Error Msg"));
      lcd.setCursor(0,0);
      cursorPos = 0;
      cursorRow = 0;
      break;
    case errorScreen:
      Serial.println(F("Error logs called"));
      lcd.print(F("*Home"));
      drawClock();
      lcd.setCursor(0,1);
      lcd.print(errorMessage);
      lcd.setCursor(0,0);
      break;
  }
}

