void errorHandler (uint8_t code, uint16_t data, void *custom_pointer) {
#ifdef serialDebug
  Serial.println();
  Serial.print(F("ERROR:\tError Code: "));
  Serial.print(code);
  Serial.print(F("\tData: "));
  Serial.println(data);
#endif
  for(byte i = 0; i < 5; i++) { //Flash a led a bit so that it is obvious something is wrong
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    delay(100);
  }
}
void informMaster(byte address) {
  char msgToSend[2] = {overwriteStatus,bayStatus};
  bus.send(address,msgToSend,2);
}
void receiverFunction(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
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
#ifdef enableReset
      reset();
#endif
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
void setupSerial() {
  bus.strategy.set_serial(&Serial);
  bus.set_communication_mode(PJON_HALF_DUPLEX);
  // Set RS485 reception enable pin
  bus.strategy.set_RS485_rxe_pin(rs485rxPin);
  // Set RS485 transmission enable pin
  bus.strategy.set_RS485_txe_pin(rs485txPin);
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  bus.set_receiver(receiverFunction);
  bus.set_error(errorHandler);
  bus.begin();
}
void setBaudRate() {
  Serial.flush();
  Serial.begin(readULong(serialBaudAddress));
  //Serial.begin(9600); //Temporary while sorting serial settings out
}
