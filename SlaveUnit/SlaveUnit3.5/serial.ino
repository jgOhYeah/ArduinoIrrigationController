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
  //Initial error checking - same as master reciever function.
  byte senderId;
  //Error checking on top of lots of error checking. Just in case another device stops playing nice / or does something weird
  //Get the device's address and make sure that it is a bay outlet address
  if(packet_info.header & PJON_TX_INFO_BIT) {
    senderId = packet_info.sender_id;
    if(senderId < firstSlaveAddress || senderId > lastSlaveAddress) { //Invallid Address
      errorHandler(invalidAddress,senderId,NULL);
      return; //Stop execution there and don't continue.
    }
  } else { //We don't know who they are, so the packet is not much use.
    errorHandler(invalidAddress,256,NULL); //256 is an address that I will not use, so is dud.
    return; //Stop execution there and don't continue.
  }
  //Check that the packet has at least an instruction.
  if(length < 1) {
    errorHandler(packetError,senderId,NULL);
    return; //Stop execution there and don't continue.
  }
  //See what the message tells us to do.
  switch(payload[0]) {
    case reportStatus: {
      digitalWrite(LED_BUILTIN,HIGH);
      informMaster(packet_info.sender_id); //use thesender id in case the system is expanded in future to have multiple masters...
      digitalWrite(LED_BUILTIN,LOW);
      break;
    }
    case setStatus: {
      if(length < 2) {
        errorHandler(packetError,senderId,NULL);
        return; //Stop execution there and don't continue.
      }
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
    case setEeprom: {
      #error "need to finish this bit"
      if(length < 2) {
        errorHandler(packetError,senderId,NULL);
        return; //Stop execution there and don't continue.
      }
      break;
    }
    case readEeprom: {
      #error "need to finish this bit"
      if(length < 2) { //Put this in a common function between the read and write?
        errorHandler(packetError,senderId,NULL);
        return; //Stop execution there and don't continue.
      }
      switch (payload[0]) {
        case bAddress: //1 byte
        case halfPos: //1 byte
        case uTravelSpeed: //4 bytes
        case dTravelSpeed: //4 bytes
        case baudRate: //4 bytes
          break;
      }
      break;
    }
    default: {
      errorHandler(unrecognisedCommand,payload[0],NULL);
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
