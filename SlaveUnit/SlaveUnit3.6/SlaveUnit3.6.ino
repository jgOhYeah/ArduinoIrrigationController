#include "commands.h"
#include "settings.h"
#include "defines.h"
/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>
#include <ButtonOnPress.h>
#include <PollingCallback.h>
#include <EEPROM.h>
uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, 44); //44 is the default id and will be overwritten by the one from EEPROM
ButtonOnPress button(PIN_BUTTON,150,LOW,false);
PollingCallback callback; //Make a callback object to manage the callbacks.

void setup() {
  setBaudRate();
  setupSerial();
  loadSettings();
  button.begin();
  setupOutputs();
  checkForSettings();
  updateBay(bayStatus); //Set the leds up
  housekeeping();
  callback.add(5000,true,housekeeping); //Mostly debugging stuff that does not need to be done that often
}

void loop() {
  bus.update();
  bus.receive();
  checkButtons();
  callback.check();
}
