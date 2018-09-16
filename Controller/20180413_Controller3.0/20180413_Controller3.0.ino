#include <buttons.h>
#include <LiquidCrystal.h>
#include <avr/wdt.h>

//Include the defines.h file in the same directory
#include "defines.h"

//Set up instances of objects for various libraries
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Button leftButton;
Button rightButton;
Button selectButton;

void setup() {
  //Start the interfaces
  lcd.begin(16,2);
  //Set up the buttons
  setupButtons();
  //Set up the builtin LED to show something or other
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  //Set up lcd and serial port, wait for system to start
  setupLcd();
  //Wait for system to power up
  delay(startDelay);
  changeScreen();
  getBayStatus();
  currentScreen = mainScreen;
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
  //checkForReplies();
  //Check for any unresponsive commands
  //checkCallbacks();
  //Serial.println(freeRam());
}

