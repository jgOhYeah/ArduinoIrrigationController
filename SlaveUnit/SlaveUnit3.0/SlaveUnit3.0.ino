//Include the PJON library
#define PJON_INCLUDE_ASYNC_ACK true
#define PJON_MAX_PACKETS 6
#define PJON_PACKET_MAX_LENGTH 22
#define PJON_INCLUDE_TS
#include <PJON.h>
#include <ButtonOnPress.h>
//Include the other files in this folder before this file
#include "typedefs.h"
#include "settings.h"
#include "defines.h"

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
  setupBays();
}

void loop() {
  for(byte i = 0; i < numberOfBays; i++) { //Update stuff in each bay
    bays[i].bus.update(); //Update the serial bus
    bays[i].bus.receive(); //Recieve new messages
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
