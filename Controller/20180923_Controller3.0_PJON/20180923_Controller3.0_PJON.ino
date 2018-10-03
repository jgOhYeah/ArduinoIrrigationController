//Include the defines.h file in the same directory
#include "defines.h"
/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true
#define PJON_MAX_PACKETS 15
#define PJON_INCLUDE_TS
#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, masterAddress);

// 1 millisecond maximum expected latency using short wires, higher if necessary
uint32_t latency = 1000;

#include <ButtonOnPress.h>
#include <LiquidCrystal.h>
#include <avr/wdt.h>

//Set up instances of objects for various libraries
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
ButtonOnPress leftButton(buttonLeft,btnOffTime,buttonPressedState,pullupsEnabled);
ButtonOnPress rightButton(buttonRight,btnOffTime,buttonPressedState,pullupsEnabled);
ButtonOnPress selectButton(buttonSelect,btnOffTime,buttonPressedState,pullupsEnabled);

void setup() {
  for(byte i = 0; i < numberOfDevices; i++) {
    bayStatus[i] = bayUnknown;
  }
  //Start the interfaces
  lcd.begin(16,2);
  //Set up the builtin LED to show something or other
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  //Set up lcd and serial port, wait for system to start
  setupLcd();
  delay(startDelay); //Wait for system to power up
  changeScreen(); //Screen is set to init by default
  setupSerial();
  serialDelay(replyDelay);
  if(currentScreen != errorScreen) {
    currentScreen = mainScreen;
    changeScreen();
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

