void recieveData (uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  byte senderId;
  if(packet_info.header & PJON_TX_INFO_BIT) {
    senderId = packet_info.sender_id;
    if(senderId < firstSlaveAddress || senderId > lastSlaveAddress) { //Invallid Address
      errorHandler(invalidAddress,senderId,NULL);
      return; //Stop execution there and don't continue.
    }
    if(length < 1) {
      errorHandler(packetError,senderId,NULL);
      return; //Stop execution there and don't continue.
    }
  }
  switch(payload[0]) {
    case overwriteStatus: {
      //More error checking just to be sure.
      if(length < 2) {
        errorHandler(packetError,senderId,NULL);
        return; //Stop execution there and don't continue.
      } else if(payload[1] < bayUnknown || payload[1] > bayOpen) {
        errorHandler(packetError,senderId,NULL);
        return; //Stop execution there and don't continue.
      }
      bayStatus[byte(senderId)-firstSlaveAddress] = payload[1];
      if(currentScreen == mainScreen) {
        changeScreen(); //Refresh the screen if nessecary
      }
      break;
    }
    default: {
      errorHandler(packetError,senderId,NULL);
      return; //Stop execution there and don't continue.
    }
  }
}

void errorHandler(uint8_t code, uint16_t data, void *custom_pointer) {
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println();
  Serial.print("Error: ");
  Serial.print(code);
  Serial.print("\tData: ");
  Serial.println(data);
  //Put the time into the message
  runningTime(completeErrorMsg);
  strcat(completeErrorMsg," ");
  switch(code) {
    case PJON_CONNECTION_LOST: {
      bayStatus[byte(bus.packets[data].content[0])-firstSlaveAddress] = bayNotPresent; //Update the array of bay statuses to be disconnected.
      //Build the error message
      //Copy "all" to the string.
      strcat_P(completeErrorMsg, bay);
      //Check to see if all devices have been affected. If so, replace numbers with "All"
      bool allDisconnected = true;
      for(byte i = 0; i < numberOfDevices; i++) {
        if(bayStatus[i] != bayNotPresent) {
          allDisconnected = false;
          break; //Speed processing up a bit possibly
        }
      }
      if(allDisconnected) {
        //Copy "all" to the string.
        strcat_P(completeErrorMsg, all);
      } else {
        for(byte i = 0; i < numberOfDevices; i++) {
          if(bayStatus[i] == bayNotPresent) {
            char charBuffer[4]; //Should only need three, but just in case I decide to add more than 10 bay outlets in future.
            itoa(i + 1,charBuffer,10);
            strcat(charBuffer,",");
            strcat(completeErrorMsg,charBuffer);
          }
        }
      }
      //Add the not connected message.
      strcat_P(completeErrorMsg,noReply);
      //0000:00 Bays 1,2,3,4,5,6,7, Disconnected
      break;
    }
    case PJON_PACKETS_BUFFER_FULL: { //If the master is trying to send or recieve more data at a higher rate than the bus is capable. Should not be an issue once set up.
      strcat_P(completeErrorMsg, bufferTooLong);
      strcat_P(completeErrorMsg, lengthString);
      char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case PJON_CONTENT_TOO_LONG: {
      strcat_P(completeErrorMsg, contentTooLong);
      strcat_P(completeErrorMsg, lengthString);
      char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case invalidAddress: {
      strcat_P(completeErrorMsg, invalidAddressString);
      char charBuffer[5];
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    case packetError: {
      strcat_P(completeErrorMsg, packetErrorString);
      char charBuffer[5];
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      break;
    }
    default: {
      strcat_P(completeErrorMsg, unknownError);
      char charBuffer[5]; //Just to be sure there is enough space (buffer would have to be bigger than total ram in an Atmega328p to overflow >9999 bytes)
      itoa(data,charBuffer,10);
      strcat(completeErrorMsg,charBuffer);
      //UnkownErrorMessage
    }
  }
  //Swap to the error screen
  currentScreen = errorScreen;
  changeScreen();
  digitalWrite(LED_BUILTIN,LOW);
}
void setupSerial() {
  //Start the PJON Serial Network over rs485
  Serial.begin(baudRate);
  bus.strategy.set_serial(&Serial);
  //Set configuration to send packet requesting asynchronous acknowledgement response
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  bus.strategy.set_enable_RS485_pin(serialEnablePin);
  bus.set_error(errorHandler);
  bus.begin();
  bus.set_receiver(recieveData);
  
  for(byte i = firstSlaveAddress; i <= lastSlaveAddress; i++) {
    bayStatus[i - firstSlaveAddress] = bayUnknown;
    bus.send(i, reportStatus, 1);
    bus.update();
    bus.receive(TS_TIME_IN + latency);
  }
}

