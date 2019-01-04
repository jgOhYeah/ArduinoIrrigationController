//Include the defines.h file in the same directory
#include "commands.h"
#include "strings.h"
#include "defines.h"
#include "settings.h"
/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true
#define PJON_MAX_PACKETS 15
#define PJON_INCLUDE_TS


#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, MASTER_ADDRESS);

// 1 millisecond maximum expected latency using short wires, higher if necessary
uint32_t latency = 1000;

#include <PollingCallback.h>						
#include <ButtonOnPress.h>
#include <LiquidCrystal.h>
#include <avr/wdt.h>

//Set up instances of objects for various libraries
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
ButtonOnPress leftButton(PIN_BUTTON_LEFT,MIN_BUTTON_OFF_TIME,BUTTON_PRESSED_STATE,BUTTON_PULLUPS_ENABLED);
ButtonOnPress rightButton(PIN_BUTTON_RIGHT,MIN_BUTTON_OFF_TIME,BUTTON_PRESSED_STATE,BUTTON_PULLUPS_ENABLED);
ButtonOnPress selectButton(PIN_BUTTON_SELECT,MIN_BUTTON_OFF_TIME,BUTTON_PRESSED_STATE,BUTTON_PULLUPS_ENABLED);
PollingCallback callback;
void setup() {
  for(byte i = 0; i < NUMBER_OF_BAYS; i++) {
    bayStatus[i] = STATE_UNKOWN;
  }
  //Start the interfaces
  lcd.begin(LCD_WIDTH,LCD_HEIGHT);
  //Set up the builtin LED to show something or other
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  //Set up lcd and serial port, wait for system to start
  setupLcd();
  delay(START_DELAY); //Wait for system to power up
  changeScreen(LCD_INIT, LCD_MAIN); //Screen is set to init by default
  setupSerial();
  //serialDelay(REPLY_DELAY);
  waitUntilAllConnected();
  if(currentScreen != LCD_ERROR) {
    changeScreen(LCD_MAIN, LCD_MAIN);
  }
  //Set up the button pins
  leftButton.begin();
  rightButton.begin();
  selectButton.begin();
}

void loop() {
  bus.update();
  bus.receive();
  //bus.receive(TS_TIME_IN + latency);
  //UI stuff
  switch(currentScreen) {
    case LCD_MAIN:
      mainButtons();
      break;
    case LCD_MENU:
      menuButtons();
      break;
    case LCD_ERROR:
      errorButtons();
      break;
    case LCD_SET_VALUE:
      inputValueScreenButtons();
      break;
    case LCD_SETUP:
      setupMenuButtons();
      break;
    case LCD_EDIT_EEPROM_1:
      eepromScreen1Buttons();
      break;
    case LCD_EDIT_EEPROM_2:
      eepromScreen2Buttons();
      break;
  }
}

