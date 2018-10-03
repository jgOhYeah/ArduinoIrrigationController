//Include the PJON library
#define PJON_INCLUDE_ASYNC_ACK true
#define PJON_MAX_PACKETS 12
#define PJON_PACKET_MAX_LENGTH 22
#define PJON_INCLUDE_TS
#include <PJON.h>
#include <ButtonOnPress.h>
//Include the other files in this folder before this file
#include "typedefs.h"
#include "settings.h"
#include "defines.h"
PJON<ThroughSerial> bus(busId, PJON_BROADCAST); //Serial setup
void setup() {
  //TODO: Load values from eeprom at some stage - also include usb serial settings to change them
  //  - Will having both the recieve and transmit enable pins on the transeiver disabled allow the
  //    usb-serial converter to work without a header to disconnect the transeiver?
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(baudRate);
  Serial.println();
  Serial.print(F("There are "));
  Serial.print(numberOfBays);
  Serial.println(F(" connected."));

  bus.strategy.set_serial(&Serial);
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  bus.set_communication_mode(PJON_HALF_DUPLEX);
  bus.strategy.set_enable_RS485_pin(serialEnablePin);
  bus.set_error(errorHandler);
  bus.set_receiver(recieveData);
  bus.set_router(true); //Recieve all packets so one instance can respond to multiple addresses
  bus.begin();
  setupBays();
}

void loop() {
  bus.update();
  bus.receive();
  for(byte i = 0; i < numberOfBays; i++) { //Update stuff in each bay
    if(bays[i].button.checkButton()) { //Increment the bay status if the button is pressed
      char newStatus = bays[i].status + 1;
      if(newStatus > bayOpen) {
        newStatus = bayShut;
      }
      updateBay(&bays[i],newStatus); //Move the motor
      informMaster(&bays[i],masterId); //Inform the main controller of the change
    }
    //Check the callback for the bay
  }
  //Implement a function that gets called once per second to flash leds?
  static unsigned long lastTime = 0;
  if(millis() - lastTime >= ledFlashTime) {
    lastTime = millis();
#ifdef serialDebug
    for(byte i = 0; i < numberOfBays; i++) {
      bayDetails(bays[i]);
    }
#endif
  }
}
