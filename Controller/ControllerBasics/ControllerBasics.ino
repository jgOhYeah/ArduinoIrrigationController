#include <buttons.h>
#include <LiquidCrystal.h>
#include <avr/wdt.h>

#define dateCompiled static_cast<String>(__DATE__) + " at " + static_cast<String>(__TIME__)
#define softwareVersion " V1.0"
//Buttons
#define buttonRight 6
#define buttonLeft 7
#define buttonSelect 8
#define buttonTimer 300
#define buttonRefresh 300
#define buttonMode OneShotTimer
#define serialEnable 9

//Baud rate to use for serial communication
#define baudRate 1200
#define startChar '~'
#define endChar 10 //Line feed
#define bayCallbackMaxTime 10000
#define serialEnableTimeout 5000
#define serialRecieveTimeout 3000
//Message length including start and end chars
#define messagePacketLength 4
byte message[] = {0,0,0,0};
//Called when there is serial ready to be recieved
//Address to use for all bays
#define allBays 123

//Commands to bays
#define resetBays 50
#define getBayStatus 25

//Number of gates installed
#define numberOfBays 7

//Number of ms between each time the clock is updated
#define clockUpdateSpeed 5000
//Speed at which to scroll the display
#define displayScrollSpeed

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

unsigned long bayCallbacks[numberOfBays];
byte bayStatus[numberOfBays];
const byte bayOptions[] = {'?','S','H','O','X'};
/*
 * 0 ? = Unknown Status
 * 1 S = Shut
 * 2 H = Half
 * 3 O = Open
 * 4 X = Not connected / issues
 */

//#define ellipsis 0
//byte ellipsisCustomChar[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x15,0x00};

#define scrollSkips 9
byte cursorPos = 0;
byte cursorRow = 0;
byte scrollPos = 0;
String errorMessage = "Nothing to show";
bool allTheSame = false;

const String sharedStrings[] = {"Bay "," Rx msg ","*Home ","Irrigation","Controller"};
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

Button leftButton;
Button rightButton;
Button selectButton;

void setup() {
  //Set up serial for debugging - later lower the baud rate and use for serial bus
  Serial.begin(baudRate);

  pinMode(serialEnable,INPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  setupButtons();
  
  //Set up the lcd and display the date the program was compiled
  lcd.begin(16, 2);
  //lcd.createChar(ellipsis, ellipsisCustomChar);
  //Useful info for when last updated
  lcd.clear();
  lcd.print(sharedStrings[3] + softwareVersion);
  lcd.setCursor(0,1);
  lcd.print(sharedStrings[4]);
  Serial.println(sharedStrings[3] + ' ' + sharedStrings[4] + softwareVersion);
  Serial.println(String(F("Compiled on ")) + dateCompiled);
  //Let the rest of the system start
  delay(2000);
  lcd.clear();
  //Give the user something to look at if takes a while
  lcd.print(F("Checking bays..."));
  //Set all the bays to unknown status
  for(byte i = 0; i < numberOfBays; i++) {
    bayStatus[i] = bayNotPresent;
    bayCallbacks[i] = 0;
  }
  
  //Get the status of the bays if known - in case controller gets reset
  sendBayCommand(allBays,getBayStatus);
  //Wait until the timeout or all bays are present and accounted for
  unsigned long startTime = millis();
  bool repliesStillNeeded = true;
  //repeat for a while to let replies come in. Give up after a timeout to say that those that have not replied are not connected
  while(millis() - startTime < bayCallbackMaxTime && repliesStillNeeded) {
    checkForReplies(false);
    //Iterate over the array and check if any still equal not present. repliesStillNeeded will still be false if not.
    repliesStillNeeded = false;
    for(byte i = 0; i < numberOfBays; i++) {
      if(bayStatus[i] == bayNotPresent) {
        repliesStillNeeded = true;
      }
    }
  }
  lcd.cursor();
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
  //Recieve messages
  checkForReplies(true);
  //Check for any unresponsive commands
  checkCallbacks();
}
