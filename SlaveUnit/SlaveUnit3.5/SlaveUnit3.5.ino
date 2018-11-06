#include "defines.h"
#include "settings.h"
#include "commands.h"
/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>
#include <ButtonOnPress.h>
#include <EEPROM.h>
uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, 44); //44 is the default id and will be overwritten by the one from EEPROM
ButtonOnPress button(buttonPin,150,LOW,false);
#define mainFile __FILE__
void setup() {
  setupSerial();
  loadSettings();
  setBaudRate();
  button.begin();
  setupOutputs();
  checkForSettings();
  updateBay(bayStatus); //Set the leds up
}

void loop() {
  bus.update();
  bus.receive();
  checkButtons();
  checkCallbacks();
}
