void recieveData (uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
#ifdef serialDebug
  Serial.println();
  Serial.print(F("RECEIVED MESSAGE"));
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
  bayDetails((bayStruct*)packet_info.custom_pointer);
#endif
  //Preliminary validation to make sure that we have a command and a bay outlet to act on.
  if(length < 1) { //Check that there is at least 1 char for a command.
    errorHandler(packetError,packet_info.sender_id,NULL);
    return; //Stop execution there and don't continue.
  }
  if(!packet_info.custom_pointer) {
    //Something went wrong as there is no bay outlet
    errorHandler(receiverError,packet_info.sender_id,NULL);
    return;
  }
  
  switch(payload[0]) {
    //Possibly add a command for remotely setting eeprom?
    case reportStatus: {
      digitalWrite(LED_BUILTIN,HIGH);
      informMaster((bayStruct*)packet_info.custom_pointer,packet_info.sender_id); //use thesender id in case the system is expanded in future to have multiple masters...
      digitalWrite(LED_BUILTIN,LOW);
      break;
    }
    case setStatus: {
#ifdef serialDebug
      Serial.println();
      Serial.println(F("Updating Bay"));
#endif
      updateBay((bayStruct*)packet_info.custom_pointer,payload[1]);
      break;
    }
    case reset: { //The watchdog timer with the builtin bootloader crashes arduino nanos, so don't reset or
    //reset with hardware (gpio pin connected to reset pin). OR reflash with arduino uno bootloader.
      Serial.println("I have been told to reset.");
      break;
    }
    default: {
      errorHandler(invalidCommand,packet_info.sender_id,NULL);
    }
  }
}
void errorHandler (uint8_t code, uint16_t data, void *custom_pointer) {
#ifdef serialDebug
  Serial.println();
  Serial.print(F("ERROR:\tError Code: "));
  Serial.print(code);
  Serial.print(F("\tData: "));
  Serial.println(data);
#endif
  for(byte i = 0; i < 10; i++) { //Flash a led a bit so that it is obvious something is wrong
    digitalWrite(LED_BUILTIN,HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN,LOW);
    delay(250);
  }
}
void informMaster(bayStruct * bayToChange,byte address) {
  char msgToSend[2] = {overwriteStatus,bayToChange->status};
  bayToChange->bus.send(address,msgToSend,2);
}

#ifdef serialDebug
void bayDetails(bayStruct * bay) {
  Serial.println();
  Serial.println(F("BAY INFORMATION"));
  Serial.print(F("\t- Motor Pin A: "));
  Serial.println(bay->outputPins[motorPinA]);
  Serial.print(F("\t- Motor Pin B: "));
  Serial.println(bay->outputPins[motorPinB]);
  Serial.print(F("\t- Up Led Pin: "));
  Serial.println(bay->outputPins[upLed]);
  Serial.print(F("\t- Half Led Pin: "));
  Serial.println(bay->outputPins[halfLed]);
  Serial.print(F("\t- Down Led Pin: "));
  Serial.println(bay->outputPins[downLed]);
  
  Serial.print(F("\t- Serial Address: "));
  Serial.println(bay->address);
  Serial.print(F("\t- Bay Status: "));
  Serial.println(byte(bay->status));
  Serial.print(F("\t- Button is pressed: "));
  Serial.println(bay->button.isPressed());
  Serial.print(F("FREE RAM: "));
  Serial.println(freeRam());
}
void bayDetails(bayStruct & bay) {
  Serial.println();
  Serial.println(F("BAY INFORMATION"));
  Serial.print(F("\t- Motor Pin A: "));
  Serial.println(bay.outputPins[motorPinA]);
  Serial.print(F("\t- Motor Pin B: "));
  Serial.println(bay.outputPins[motorPinB]);
  Serial.print(F("\t- Up Led Pin: "));
  Serial.println(bay.outputPins[upLed]);
  Serial.print(F("\t- Half Led Pin: "));
  Serial.println(bay.outputPins[halfLed]);
  Serial.print(F("\t- Down Led Pin: "));
  Serial.println(bay.outputPins[downLed]);
  
  Serial.print(F("\t- Serial Address: "));
  Serial.println(bay.address);
  Serial.print(F("\t- Bay Status: "));
  Serial.println(byte(bay.status));
  Serial.print(F("\t- Button is pressed: "));
  Serial.println(bay.button.isPressed());
  Serial.print(F("FREE RAM: "));
  Serial.println(freeRam());
}
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif
