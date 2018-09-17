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
  Serial.print("Slave ready. Listening as device ");
  Serial.println(myId);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  //Need more error checking!!! See master reciever function for inspiration.
  Serial.println();
  Serial.print("Message recieved: ");
  Serial.println(payload[0]);
  switch(payload[0]) {
    case reportStatus: {
      Serial.println("Reporting status as bayShut");
      digitalWrite(LED_BUILTIN,HIGH);
      char msgToSend[2] = {overwriteStatus,bayStatus};
      bus.send(packet_info.sender_id,msgToSend,2);
      digitalWrite(LED_BUILTIN,LOW);
      Serial.println("Sent");
      break;
    }
    case setStatus: {
      Serial.print("Setting Status to ");
      Serial.println(payload[1]);
      bayStatus = payload[1];
    }
    case reset: //The watchdog timer with the builtin bootloader crashes arduino nanos, so don't reset or
    //reset with hardware (gpio pin connected to reset pin). OR reflash with arduino uno bootloader.
    default: {
      for(byte i = 0; i < 10; i++) { //Flash a led a bit so that it is obvious something unkown was sent.
        digitalWrite(LED_BUILTIN,HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN,LOW);
        delay(250);
      }
    }
  }
  Serial.println("Done...");
}

void loop() {
  bus.update();
  bus.receive();
};
