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
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(upLed, OUTPUT);
  pinMode(halfLed, OUTPUT);
  pinMode(downLed, OUTPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off
  updateBay(); //Set the leds up
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  bus.begin();
  bus.set_receiver(receiver_function);
  Serial.print("Slave ready. Listening as device ");
  Serial.println(myId);
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  //Need more error checking!!! See master reciever function for inspiration.
  switch(payload[0]) {
    case reportStatus: {
      digitalWrite(LED_BUILTIN,HIGH);
      informMaster(packet_info.sender_id); //use thesender id in case the system is expanded in future to have multiple masters...
      digitalWrite(LED_BUILTIN,LOW);
      break;
    }
    case setStatus: {
      bayStatus = payload[1];
      updateBay();
      break;
    }
    case reset: { //The watchdog timer with the builtin bootloader crashes arduino nanos, so don't reset or
    //reset with hardware (gpio pin connected to reset pin). OR reflash with arduino uno bootloader.
      Serial.println("I have been told to reset.");
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
  Serial.println("Done...");
}

void loop() {
  bus.update();
  bus.receive();
  if(button.checkButton()) { //If the button is pressed
    bayStatus++;
    if(bayStatus > bayOpen) {
      bayStatus = bayShut;
    }
    updateBay();
    informMaster(masterId);
  }
}
void informMaster(byte address) {
  char msgToSend[2] = {overwriteStatus,bayStatus};
  bus.send(address,msgToSend,2);
}
void updateBay() {
  switch(bayStatus) {
    case bayShut:
      digitalWrite(motorB,HIGH);
      digitalWrite(motorA,LOW);
      digitalWrite(downLed,HIGH);
      digitalWrite(halfLed,LOW);
      digitalWrite(upLed,LOW);
      break;
    case bayHalf:
      digitalWrite(downLed,LOW);
      digitalWrite(halfLed,HIGH);
      digitalWrite(upLed,LOW);
      break;
    case bayOpen:
      digitalWrite(motorB,LOW);
      digitalWrite(motorA,HIGH);
      digitalWrite(downLed,LOW);
      digitalWrite(halfLed,LOW);
      digitalWrite(upLed,HIGH);
      break;
    default:
      digitalWrite(downLed,HIGH);
      digitalWrite(halfLed,HIGH);
      digitalWrite(upLed,HIGH);
  }
}

