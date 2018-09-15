#include <buttons.h>
#include <LiquidCrystal.h>

#define dateCompiled static_cast<String>(__DATE__) + " at " + static_cast<String>(__TIME__)
#define softwareVersion "1.0"
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
byte currentScreen = mainScreen;

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
const byte bayOptions[] = {'?','S','H','O','X'};
/*
 * 0 ? = Unknown Status
 * 1 S = Shut
 * 2 H = Half
 * 3 O = Open
 * 4 X = Not connected / issues
 */

byte cursorPos = 0;
byte cursorRow = 0;
//Not using mulitdimensional arrays to get different data types.
/*
 * Error types
 * 0 = no error in spot
 * 1 = Not connected
 * ...
 * 
 */

String errorMessage = "Nothing to show";
bool allTheSame = false;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

Button leftButton;
Button rightButton;
Button selectButton;

void setup() {
  //Set up serial for debugging - later lower the baud rate and use for serial bus
  Serial.begin(38400);

  setupButtons();
  //Set all the bays to unknown status
  for(byte i = 0; i < numberOfBays; i++) {
    bayStatus[i] = bayUnknown;
  }
  
  //Set up the lcd and display the date the program was compiled
  lcd.begin(16, 2);
  //Useful info for when last updated
  lcd.clear();
  lcd.print("Irrigation");
  lcd.setCursor(0,1);
  lcd.print("Controller");
  Serial.println("Irrigation Controller");
  Serial.println("Compiled on " + dateCompiled);
  //Let the rest of the system start
  delay(1000);
  lcd.cursor();
  getBayStatus();
  changeScreen();
}

void loop() {
  //UI stuff
  switch(currentScreen) {
    case mainScreen:
      updateTime();
      mainScreenButtons();
      break;
    case menuScreen:
      menuScreenButtons();
      break;
    case errorScreen:
      updateTime();
      errorScreenButtons();
      break;
  }
}
