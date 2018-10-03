void recieveData (uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  //Preliminary validation to make sure that we have a command and a bay outlet to act on.
  if(!(packet_info.header & PJON_MODE_BIT) || !(packet_info.header & PJON_TX_INFO_BIT) || length < 1) { //Packet not set up correctly
    errorHandler(packetError,packet_info.sender_id,NULL); //Sender id might not always exist.
    return; //Stop execution there and don't continue.
  }
  byte startLoopAddress = 0;
  byte endLoopAddress = numberOfBays;
  bool packetApplies = false;
  if(packet_info.receiver_id == PJON_BROADCAST) { //If the message was sent to all devices, repeat for all of them connected
    packetApplies = true;
  }
  if(!packetApplies) { //We know that the message wasn't a broadcast, check for individual bays
    //Check if the recieved address is registered to this device
    for(startLoopAddress = 0; startLoopAddress < numberOfBays; startLoopAddress++) {
      if(packet_info.receiver_id == bays[startLoopAddress].address) { //This packet applies to this bay
        packetApplies = true;
        endLoopAddress = startLoopAddress + 1; //The for loop will only iterate once.
        break; //Stop searching
      }
    }
  }
  if(!packetApplies) { //We can safely ignore this packet as it does not apply to us.
    return;
  }
  digitalWrite(LED_BUILTIN,HIGH);
  //Actions
  for(byte i = startLoopAddress; i < endLoopAddress; i++) {
    switch(payload[0]) {
      //Possibly add a command for remotely setting eeprom?
      case reportStatus: {
        informMaster(&bays[i],packet_info.sender_id); //use thesender id in case the system is expanded in future to have multiple masters...
        break;
      }
      case setStatus: {
  #ifdef serialDebug
        Serial.println();
        Serial.print(F("Updating Bay "));
        Serial.println(i);
  #endif
        updateBay(&bays[i],payload[1]); //TODO: Error checking
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
  digitalWrite(LED_BUILTIN,LOW);
}
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
void informMaster(bayStruct * bayToChange,byte address) {
  char msgToSend[2] = {overwriteStatus,bayToChange->status};
  bus.set_id(bayToChange->address); //Make it look like it came from the right bay.
  bus.send(address,msgToSend,2);
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
