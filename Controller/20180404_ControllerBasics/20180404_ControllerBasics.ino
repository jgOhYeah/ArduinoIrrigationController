#include <buttons.h>
#include <LiquidCrystal.h>

#define dateCompiled static_cast<String>(__DATE__) + " at " + static_cast<String>(__TIME__)
//Buttons
#define buttonRight 6
#define buttonLeft 7
#define buttonSelect 8
#define buttonTimer 1500
#define buttonRefresh 500
#define buttonMode OneShotTimer
//Serial Lines. Reserved for now, need to implement
#define serialRx 9
#define serialTx 10

//Number of gates installed
#define numberOfBays 7

//Number of ms between each time the clock is updated
#define clockUpdateSpeed 5000

#define mainScreen 0
#define menuScreen 1
#define errorScreen 2
 /*
  * 0 is the main screen
  * 1 is the menu screen
  * 2 is the error log
  */
#define bayUnknown 0
#define bayShut 1
#define bayHalf 2
#define bayOpen 3
#define bayNotPresent 4
byte bayStatus[numberOfBays];
const byte bayOptions[] = {'?',0,1,2,'X'};
byte lcdShutChar[8] = {0x1F,0x11,0x11,0x11,0x11,0x11,0x11,0x1F};
byte lcdHalfChar[8] = {0x1F,0x11,0x11,0x11,0x1F,0x1F,0x1F,0x1F};
byte lcdOpenChar[8] = {0x1F,0x11,0x17,0x11,0x17,0x17,0x1F,0x1F};
/*
 * 0 ? = Unknown Status
 * 1 S = Shut
 * 2 H = Half
 * 3 O = Open
 * 4 X = Not connected / issues
 */

 byte cursorPos = 0;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

Button leftButton;
Button rightButton;
Button selectButton;

void setup() {
  Serial.begin(38400);

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
  
  //Set all the bays to unknown status
  for(byte i = 0; i < numberOfBays; i++) {
    bayStatus[i] = bayUnknown;
  }
  //Set up the lcd and display the date the program was compiled
  lcd.begin(16, 2);
  lcd.createChar(0,lcdShutChar);
  lcd.createChar(1,lcdHalfChar);
  lcd.createChar(2,lcdOpenChar);
  //Useful info for when last updated
  lcd.print("Compiled");
  lcd.setCursor(0,1);
  lcd.print(dateCompiled);
  Serial.println("Compiled on " + dateCompiled);
  //Let the rest of the system start
  delay(5000);
  drawMainScreen();
}

void loop() {
    updateTime();
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
          cursorPos = numberOfBays - 1;
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
        case numberOfBays - 1:
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
        changeScreen(menuScreen);
        //Else if the bay is connected and online, increment through the options.
      } else if(bayStatus[cursorPos] != bayNotPresent) {
        if(bayStatus[cursorPos] == bayOpen) {
          bayStatus[cursorPos] = bayShut;
        } else {
          bayStatus[cursorPos]++;
        }
        //Write the updated status to the lcd
        lcd.write(byte(bayOptions[bayStatus[cursorPos]]));
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
void sendBayCommand(byte bay,byte command) {
  Serial.println("Sending a command to bay " + String(bay) + ". The command is " + String(command) + ".");
}
void changeScreen(byte screenMode) {
    //When Changing screen
  switch(screenMode) {
    case mainScreen:
      drawMainScreen();
      //Stuff
      break;
    case menuScreen:
      //Stuff
      Serial.println("Menu screen called");
      lcd.clear();
      lcd.print("Menu screen.");
      while(true);
      break;
    case errorScreen:
      drawClock();
      break;
  }
}
void drawMainScreen() {
  lcd.clear();
  //Print out the bay numbers
  for(byte i = 1; i <= numberOfBays; i++) {
    lcd.print(i);
  }
  //Go to the row beneath and print out the staus of each
  lcd.setCursor(0,1);
  for(byte i = 0; i < numberOfBays; i++) {
    lcd.write(byte(bayOptions[bayStatus[i]]));
  }
  lcd.setCursor(11,1);
  lcd.print("*More");
  drawClock();
  lcd.setCursor(0,1);
  lcd.blink();
  lcd.cursor();
  cursorPos = 0;
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
