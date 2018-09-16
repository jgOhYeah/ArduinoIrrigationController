#include "defines.h"
/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, myId);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
  bus.set_synchronous_acknowledge(false);
  bus.begin();
  bus.set_receiver(receiver_function);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  switch(payload[0]) {
    case reportStatus: {
        digitalWrite(LED_BUILTIN,HIGH);
        char msgToSend[2] = {overwriteStatus,bayStatus};
        bus.send(packet_info.sender_id,msgToSend,2);
        digitalWrite(LED_BUILTIN,LOW);
      break;
    }
    default: {
      for(byte i = 0; i < 10; i++) { //Flash a led a bit so that it is obvious something unkown was sent.
        digitalWrite(LED_BUILTIN,HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN,LOW);
        delay(250);
      }
    }
  }
}

void loop() {
  bus.update();
  bus.receive();
};
