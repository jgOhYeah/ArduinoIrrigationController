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
void drawClock() {
  String timeSinceLaunch = runningTime();
  lcd.setCursor(16-timeSinceLaunch.length()+scrollPos,0);
  lcd.print(timeSinceLaunch);
  //Serial.println("Time: " + timeSinceLaunch);
}
String runningTime() {
  unsigned long milliSeconds = millis();
  byte minutes = milliSeconds / 60000 % 60;
  unsigned int hours = milliSeconds / 3600000;
  String timeSinceLaunch;
  if(minutes < 10) {
    timeSinceLaunch = String(hours) + ":0" + String(minutes);
  } else {
    timeSinceLaunch = String(hours) + ':' + String(minutes);
  }
  return timeSinceLaunch;
}
void changeScreen() {
  //When Changing screen
  scrollPos = 0;
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
      /*if(allTheSame) {
        lcd.write(byte(bayOptions[bayStatus[0]]));
      }*/
      lcd.write('*');
      lcd.setCursor(11,1);
      lcd.print(F("*More"));
      drawClock();
      cursorPos = 0;
      cursorRow = 1;
      //Stuff
      break;
    case menuScreen:
      //Stuff
      //Serial.println(F("Menu screen displayed"));
      lcd.print(sharedStrings[2] + F("*Reset all"));
      lcd.setCursor(0,1);
      lcd.print(F("*Last Error Msg"));
      cursorPos = 0;
      cursorRow = 0;
      break;
    case errorScreen:
      //Serial.println(F("Error logs displayed"));
      drawErrorScreenTop();
      lcd.setCursor(0,1);
      lcd.print(errorMessage);
      cursorPos = 0;
      cursorRow = 0;
      break;
  }
  lcd.setCursor(cursorPos,cursorRow);
}
void drawErrorScreenTop() {
  lcd.setCursor(0+scrollPos,0);
  lcd.print(sharedStrings[2] + F("ERROR "));
  drawClock();
}
void createErrorMessage(String message) {
  errorMessage = runningTime() + " " + message;
  Serial.println(String(F("Error: ")) + errorMessage);
  digitalWrite(LED_BUILTIN,LOW);
  currentScreen = errorScreen;
  changeScreen();
}

