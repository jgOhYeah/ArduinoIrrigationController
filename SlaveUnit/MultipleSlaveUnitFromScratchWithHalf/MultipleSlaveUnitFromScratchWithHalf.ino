/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>
#include "defines.h"
#include <ButtonOnPress.h>
uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
//PJON<ThroughSerial> bus(bus_id, myId);
//PJON<ThroughSerial> bus2(bus_id, myId2);
ButtonOnPress button(buttonPin,150,LOW,false);
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(upLed, OUTPUT);
  pinMode(halfLed, OUTPUT);
  pinMode(downLed, OUTPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off
  updateBay(bayStatus); //Set the leds up
  Serial.begin(9600);
  /*bus.strategy.set_serial(&Serial);
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  bus.set_communication_mode(PJON_HALF_DUPLEX);
  bus.begin();
  bus.set_receiver(receiver_function);

  bus2.strategy.set_serial(&Serial);
  bus2.set_synchronous_acknowledge(false);
  bus2.set_asynchronous_acknowledge(true);
  bus2.begin();
  bus2.set_receiver(receiver_function2);*/
  Serial.print("Slave ready. Listening as device ");
  Serial.println(myId);
}
void receiver_function2(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
   /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  Serial.print("Header: ");
  Serial.print(packet_info.header, BIN);
  // If packet formatted for a shared medium
  if(packet_info.header & PJON_MODE_BIT) {
    Serial.print(" Receiver bus id: ");
    Serial.print(packet_info.receiver_bus_id[0]);
    Serial.print(packet_info.receiver_bus_id[1]);
    Serial.print(packet_info.receiver_bus_id[2]);
    Serial.print(packet_info.receiver_bus_id[3]);
    Serial.print(" Receiver id: ");
    Serial.print(packet_info.receiver_id);
    // If sender info is included
    if(packet_info.header & PJON_TX_INFO_BIT) {
      Serial.print(" Sender bus id: ");
      Serial.print(packet_info.sender_bus_id[0]);
      Serial.print(packet_info.sender_bus_id[1]);
      Serial.print(packet_info.sender_bus_id[2]);
      Serial.print(packet_info.sender_bus_id[3]);
    }
  }
  // If sender device id is included
  if(packet_info.header & PJON_TX_INFO_BIT) {
    Serial.print(" Sender id: ");
    Serial.print(packet_info.sender_id);
  }
  // Payload Length
  Serial.print(" Length: ");
  Serial.print(length);
  // If port id is included
  if(packet_info.header & PJON_PORT_BIT) {
    Serial.print(" Port bit: ");
    Serial.print(packet_info.port);
  }
  Serial.println();
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
      updateBay(payload[1]);
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
  /*bus.update();
  bus.receive();
  bus2.update();
  bus2.receive();*/
  checkButtons();
  checkCallbacks();
}
void informMaster(byte address) {
  char msgToSend[2] = {overwriteStatus,bayStatus};
  //bus.send(address,msgToSend,2);
}
void updateBay(char newStatus) {
  //if(bayStatus != newStatus) {
  callbackOperation = bayNothing;
  switch(newStatus) {
    case bayShut:
      closeBayMotor();
      digitalWrite(downLed,HIGH);
      digitalWrite(halfLed,LOW);
      digitalWrite(upLed,LOW);
      timeAtDown = millis();
      break;
    case bayHalf:
      closeBayMotor();
      digitalWrite(downLed,LOW);
      digitalWrite(halfLed,HIGH);
      digitalWrite(upLed,LOW);
      callbackOperation = bayHoming;
      if(bayStatus == bayShut) {
        unsigned long timeShut = millis() - timeAtDown;
        if(timeShut >= downTravelSpeed) { //Know that the bay must be at the bottom
          callbackTime = 0;
        } else {
          callbackTime = downTravelSpeed - timeShut; //Delay only the minimum required to garuntee that it is shut
        }
      } else {
        callbackTime = downTravelSpeed; //We can't be sure about the position of much else, so do the full amount just to be safe.
      }
      startDelayTime = millis();
      break;
    case bayOpen:
      openBayMotor();
      digitalWrite(downLed,LOW);
      digitalWrite(halfLed,LOW);
      digitalWrite(upLed,HIGH);
      break;
    default:
      digitalWrite(downLed,HIGH);
      digitalWrite(halfLed,HIGH);
      digitalWrite(upLed,HIGH);
  }
  bayStatus = newStatus;
  //}
}

void checkButtons() {
  if(button.checkButton()) { //If the button is pressed
    char newStatus = bayStatus + 1;
    if(newStatus > bayOpen) {
      newStatus = bayShut;
    }
    updateBay(newStatus);
    informMaster(masterId);
  }
}
void checkCallbacks() {
  if(millis() - startDelayTime >= callbackTime) {
    switch(callbackOperation) {
      case bayHoming: {
        openBayMotor();
        callbackOperation = bayToHalf;
        callbackTime = halfUpTime;
        startDelayTime = millis();
        break;
      }
      case bayToHalf: {
        stopBayMotor();
        callbackOperation = bayNothing;
        break;
      }
    }
  }
}

