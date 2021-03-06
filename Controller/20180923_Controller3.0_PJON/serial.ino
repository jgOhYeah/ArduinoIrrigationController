void recieveData (uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
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
  switch(payload[0]) {
    case overwriteStatus: {
      //More error checking just to be sure.
      if(length < 2) {
        errorHandler(packetError,senderId,NULL);
        return; //Stop execution there and don't continue.
      } else if(payload[1] > bayOpen) { //Make sure that the status is valid. (dunno, shut, half or open) payload[1] cannot be less than 0.
        errorHandler(packetError,senderId,NULL);
        return; //Stop execution there and don't continue.
      }
      //Update the array of bay statuses.
      bayStatus[byte(senderId)-firstSlaveAddress] = payload[1];
      if(currentScreen == mainScreen) {
        //changeScreen(); //Refresh the screen if nessecary
        drawBayStates();
        lcd.setCursor(cursorPos,cursorRow);
      }
      break;
    }
    default: {
      errorHandler(packetError,senderId,NULL); //Unexpected data recieved
      return; //Stop execution there and don't continue.
    }
  }
}
void setupSerial() {
  //Start the PJON Serial Network over rs485
  Serial.begin(baudRate);
  bus.strategy.set_serial(&Serial);
  //Set configuration to send packet requesting asynchronous acknowledgement response
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  bus.set_communication_mode(PJON_HALF_DUPLEX);
  bus.strategy.set_RS485_rxe_pin(rs485rxPin);
  // Set RS485 transmission enable pin
  bus.strategy.set_RS485_txe_pin(rs485txPin);
  bus.set_error(errorHandler);
  bus.begin();
  bus.set_receiver(recieveData);
  sendToAll(reportStatus,1);
}
void sendToAll(const char * data,byte length) {
  for(byte i = firstSlaveAddress+firstDevice; i <= lastSlaveAddress; i++) {
    bus.send(i, data, length);
    bus.update();
    bus.receive();
  }
}

