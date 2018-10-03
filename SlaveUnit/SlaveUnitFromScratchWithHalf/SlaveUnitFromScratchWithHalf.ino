#include "defines.h"
/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>
#include <ButtonOnPress.h>
uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, myId);
ButtonOnPress button(buttonPin,150,LOW,false);
void setup() {
  setupOutputs();
  setupSerial();
  updateBay(bayStatus); //Set the leds up
#ifdef serialDebug
  Serial.print("Slave ready. Listening as device ");
  Serial.println(myId);
#endif
}

void loop() {
  bus.update();
  bus.receive();
  checkButtons();
  checkCallbacks();
}
