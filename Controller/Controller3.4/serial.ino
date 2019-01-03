void recieveData (uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  byte senderId;
  //Error checking on top of lots of error checking. Just in case another device stops playing nice / or does something weird
  //Get the device's address and make sure that it is a bay outlet address
  if(packet_info.header & PJON_TX_INFO_BIT) {
    senderId = packet_info.sender_id;
    if(senderId < FIRST_BAY_ADDRESS || senderId > LAST_BAY_ADDRESS) { //Invallid Address
      errorHandler(ERR_INVALID_ADDRESS,senderId,NULL);
      return; //Stop execution there and don't continue.
    }
  } else { //We don't know who they are, so the packet is not much use.
    errorHandler(ERR_INVALID_ADDRESS,256,NULL); //256 is an address that I will not use, so is dud.
    return; //Stop execution there and don't continue.
  }
  //Check that the packet has at least an instruction.
  if(length < 1) {
    errorHandler(ERR_PACKET_LENGTH,length,NULL);
    return; //Stop execution there and don't continue.
  }
  switch(payload[0]) {
    case CMD_OVERWRITE_STATUS: {
      //More error checking just to be sure.
      if(length < 2) {
        errorHandler(ERR_PACKET_LENGTH,length,NULL);
        return; //Stop execution there and don't continue.
      } else if(payload[1] > STATE_OPEN) { //Make sure that the status is valid. (dunno, shut, half or open) payload[1] cannot be less than 0.
        errorHandler(ERR_UNKOWN_PARAMETERS,payload[1],NULL);
        return; //Stop execution there and don't continue.
      }
      //Update the array of bay statuses.
      bayStatus[byte(senderId)-FIRST_BAY_ADDRESS] = payload[1];
      if(currentScreen == LCD_MAIN) {
        //changeScreen(); //Refresh the screen if nessecary
        drawBayStates();
        lcd.setCursor(cursorPos,cursorRow);
      }
      break;
    }
    case CMD_REPORT_EEPROM: {
      if(length < 2) {
        errorHandler(ERR_PACKET_LENGTH,length,NULL);
        return; //Stop execution there and don't continue.
      }
      unsigned long number;
      switch(payload[1]) {
        case SETTING_ADDRESS: //8 bit settings
        case SETTING_HALF_POS:
          if(length != 3) {
            errorHandler(ERR_PACKET_LENGTH,length,NULL);
            return; //Stop execution there and don't continue.
          }
          number = payload[2];
          break;
        case SETTING_UP_SPEED: //32 bit settings
        case SETTING_DOWN_SPEED:
        case SETTING_BAUD_RATE:
          if(length != 6) {
            errorHandler(ERR_PACKET_LENGTH,length,NULL);
            return; //Stop execution there and don't continue.
          }
          number = arrayToLong(payload,2); //Convert the number from being 4 bytes into an unsigned long
          break;
        default: //Something unrecognised
          errorHandler(ERR_UNKOWN_PARAMETERS,payload[0],NULL);
          return; //Stop execution there and don't continue.
      }
      if((senderId == valueBayAddress || (valueBayAddress == LAST_BAY_ADDRESS + 1 && senderId == FIRST_BAY_ADDRESS + FIRST_BAY_INDEX)) && payload[1] == valueEepromAddress) { //This is the request we made from eeprom a while ago.
        callbackFunctionStored(number);
      } else { //Uncalled for packet was sent, so complain
        errorHandler(ERR_UNEXPECTED_PACKET,senderId,NULL);
        return; //Stop execution there and don't continue.
      }
      break;
    }
    case CMD_ERROR: {
      if(length != 6) {
        errorHandler(ERR_PACKET_LENGTH,length,NULL);
        return; //Stop execution there and don't continue.
      }
      otherDeviceError errorDetails; //Fudge more parameters into the error handler function.
      errorDetails.code = payload[1];
      errorDetails.data = arrayToLong(payload,2);
      
      errorHandler(ERR_SRC_OTHER_DEVICE,senderId,&errorDetails);
      break;
    }
    default: {
      errorHandler(ERR_UNKOWN_COMMAND,payload[0],NULL); //Unexpected data recieved
      return; //Stop execution there and don't continue.
    }
  }
}
void setupSerial() {
  //Start the PJON Serial Network over rs485
  Serial.begin(BAUD_RATE);
  bus.strategy.set_serial(&Serial);
  //Set configuration to send packet requesting asynchronous acknowledgement response
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  bus.set_communication_mode(PJON_HALF_DUPLEX);
  bus.strategy.set_RS485_rxe_pin(PIN_RS485_RX);
  // Set RS485 transmission enable pin
  bus.strategy.set_RS485_txe_pin(PIN_RS485_TX);
  bus.set_error(errorHandler);
  bus.begin();
  bus.set_receiver(recieveData);
  sendToAll(CMD_REPORT_STATUS,1);
}
void sendToAll(const char * data,byte length) {
  for(byte i = FIRST_BAY_ADDRESS+FIRST_BAY_INDEX; i <= LAST_BAY_ADDRESS; i++) {
    bus.send(i, data, length);
    bus.update();
    bus.receive();
  }
}
void retrieveEepromNumber(byte address,char eepromAddress,void (*callbackFunction)(unsigned long)) {
  callbackFunctionStored = callbackFunction; //Where to call back from
  valueBayAddress = address;
  valueEepromAddress = eepromAddress;
  
  charsToSend[0] = CMD_READ_EEPROM;
  charsToSend[1] = eepromAddress; //Prepare the data
  charsToSend[2] = char(0);
  if(valueBayAddress > LAST_BAY_ADDRESS) { //Send to all but for now only get the value off the first bay.
    address = FIRST_BAY_ADDRESS +FIRST_BAY_INDEX;
  }
  bus.send(address,charsToSend,2);
}
void sendEepromLong(unsigned long number) {
  charBuffer[0] = CMD_SET_EEPROM;
  charBuffer[1] = valueEepromAddress;
  longToArray((byte*)charBuffer,2,number);
  Serial.println();
  Serial.print(F("Sending eeprom long to "));
  Serial.println(valueBayAddress);
  if(valueBayAddress > LAST_BAY_ADDRESS) { //Send to all
    Serial.println(F("Send to all."));
    sendToAll(charBuffer,6);
  } else {
    bus.send(valueBayAddress,charBuffer,6);
  }
}
void sendEepromByte(unsigned long number) { //Using longs so the code is compatitble with the code that actually requires unsigned longs
  charBuffer[0] = CMD_SET_EEPROM;
  charBuffer[1] = valueEepromAddress;
  charBuffer[2] = number;
  Serial.println();
  Serial.print(F("Sending eeprom long to "));
  Serial.println(valueBayAddress);
  if(valueBayAddress > LAST_BAY_ADDRESS) { //Send to all
    Serial.println(F("Send to all."));
    sendToAll(charBuffer,3);
  } else {
    bus.send(valueBayAddress,charBuffer,3);
  }
}

