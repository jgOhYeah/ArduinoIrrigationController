void informMaster(byte address) {
  char msgToSend[2] = {CMD_OVERWRITE_STATUS,bayStatus};
  bus.send(address,msgToSend,2);
}
void receiverFunction(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  //Initial error checking - same as master reciever function.
  byte senderId;
  //Error checking on top of lots of error checking. Just in case another device stops playing nice / or does something weird
  //Get the device's address and make sure that it is a bay outlet address
  if(packet_info.header & PJON_TX_INFO_BIT) {
    senderId = packet_info.sender_id;
  } else { //We don't know who they are, so the packet is not much use.
    errorHandler(ERR_INVALID_ADDRESS,256,NULL); //256 is an address that I will not use, so is dud.
    return; //Stop execution there and don't continue.
  }
  //Check that the packet has at least an instruction.
  if(isLengthError(length,1,false)) {
    return; //Stop execution there and don't continue.
  }
  //See what the message tells us to do.
  switch(payload[0]) {
    case CMD_REPORT_STATUS: {
      digitalWrite(LED_BUILTIN,HIGH);
      informMaster(packet_info.sender_id); //use thesender id in case the system is expanded in future to have multiple masters...
      digitalWrite(LED_BUILTIN,LOW);
      break;
    }
    case CMD_SET_STATUS: {
      if(isLengthError(length,2,false)) {
        return; //Stop execution there and don't continue.
      }
      updateBay(payload[1]);
      break;
    }
    case CMD_RESET: { //The watchdog timer with the builtin bootloader crashes arduino nanos, so don't CMD_RESET or
    //CMD_RESET with hardware (gpio pin connected to CMD_RESET pin). OR reflash with arduino uno bootloader.
#ifdef ENABLE_RESET
      reset();
#endif
      break;
    }
    case CMD_READ_EEPROM: { //Remotely read settings in the bay
      if(isLengthError(length,2,false)) {
        return; //Stop execution there and don't continue.
      }
      char charBuffer[7]; //A buffer to temporarily store the character buffer.
      byte charsToSend = 3; //The number of chars that will be sent in the packet.
      switch(payload[1]) { //Copy the values into the char array.
        case SETTING_HALF_POS: {
          //Chars to send is already set right.
          charBuffer[2] = halfwayPos;
          break;
        }
        case SETTING_ADDRESS: {
          //Chars to send is already set right.
          charBuffer[2] = myId;
          break;
        }
        case SETTING_BAUD_RATE: {
          charsToSend = 6;
          longToArray((byte *)charBuffer,2,readULong(EEPROM_SERIAL_BAUD));
          break;
        }
        case SETTING_DOWN_SPEED: {
          charsToSend = 6;
          longToArray((byte *)charBuffer,2,downTravelSpeed);
          break;
        }
        case SETTING_UP_SPEED: {
          charsToSend = 6;
          longToArray((byte *)charBuffer,2,upTravelSpeed);
          break;
        }
        default: {
          //A value has been requested that this version does not support.
          errorHandler(ERR_UNKOWN_PARAMETERS,payload[1],NULL);
          return;
        }
      }
      //Add the headers and data types
      charBuffer[0] = CMD_REPORT_EEPROM;
      charBuffer[1] = payload[1];
      //Send the data
      bus.send(senderId,charBuffer,charsToSend);
      break;
    }
    case CMD_SET_EEPROM: { //A command to remotely set the settings in the bay. THIS IS A SECURITY ISSUE IF THE NETWORK IS TO BE EXPOSED TO THE WIDER WORLD!!! (but it's convenient)
      if(isLengthError(length,2,false)) {
        return; //Stop execution there and don't continue.
      }
      switch (payload[1]) {
        case SETTING_HALF_POS:
        case SETTING_ADDRESS: {//1 byte
          if(isLengthError(length,3,true)) {
            return; //Stop execution there and don't continue.
          }
          byte maximumAccepted = 255;
          if(payload[1] == SETTING_HALF_POS) { //If the setting is not a bay time, set the range to be the accepted serial values.
            maximumAccepted = 99;
          }
          if(outsideRange(payload[2],1,maximumAccepted)) {
            errorHandler(ERR_OUTSIDE_RANGE,payload[2],NULL);
            return;
          }
          if(payload[1] == SETTING_HALF_POS) { //If the setting is not a bay time, set the range to be the accepted serial values.
            halfwayPos = payload[2];
          } else {
            myId = payload[2];
          }
          break;
        }
        /*case SETTING_ADDRESS: {//1 byte
          if(isLengthError(length,3,true,senderId)) {
            return; //Stop execution there and don't continue.
          }
          if(outsideRange(payload[2],1,255)) {
            errorHandler(packetError,senderId,NULL);
            return;
          }
          myId = payload[2]; //0 to 255 does not need range checking.
          break;
        }
        case SETTING_HALF_POS: {//1 byte
          if(isLengthError(length,3,true,senderId)) {
            return; //Stop execution there and don't continue.
          }
          if(outsideRange(payload[2],1,99)) {
            errorHandler(packetError,senderId,NULL);
            return;
          }
          halfwayPos = payload[2];
        }*/
        case SETTING_BAUD_RATE: //Write any of the 4 byte versions using the same code with if statements to separate if needed.
        case SETTING_DOWN_SPEED: //4 bytes
        case SETTING_UP_SPEED: { //4 bytes
          if(isLengthError(length,6,true)) {
            return; //Stop execution there and don't continue.
          }
          unsigned long number = arrayToLong(payload,2); //Convert the 4 bytes sent into an unsigned long.
          unsigned long minimumAccepted = 1; //Defaults for the bay opening and closing times
          unsigned long maximumAccepted = MAX_BAY_TIME;
          if(payload[1] == SETTING_BAUD_RATE) { //If the setting is not a bay time, set the range to be the accepted serial values.
            minimumAccepted = MIN_SERIAL_BAUD;
            maximumAccepted = MAX_SERIAL_BAUD;
          }
          if(outsideRange(number,minimumAccepted,maximumAccepted)) { //Now check the range
            errorHandlerLong(ERR_OUTSIDE_RANGE,number,NULL);
            return;
          }
          switch(payload[1]) { //Now is the time that we want to treat them all differently
            case SETTING_BAUD_RATE:
               writeULong(EEPROM_SERIAL_BAUD,number); //We are not bothering with saving this value in RAM permenantly as it is only used at startup and debugging.
              break;
            case SETTING_DOWN_SPEED:
              downTravelSpeed = number;
              break;
            case SETTING_UP_SPEED:
             upTravelSpeed = number;
              break;
              //No need for a default here as the next up switch statement will pick it.
          }
          break;
        }
          /*
          unsigned long number = readUnsignedLongMsg(1,MAX_BAY_TIME,senderId);
          if(number == 0) { //There was an error - already been dealt with.
            return;
          }
          downTravelSpeed = number;*/
        default: { //Unrecognised properties
          errorHandler(ERR_UNKOWN_PARAMETERS,payload[1],NULL);
          return;
        }
      }
      saveSettings();
      break;
    }
    default: {
      errorHandler(ERR_UNKOWN_COMMAND,payload[0],NULL);
    }
  }
}
void setupSerial() {
  bus.strategy.set_serial(&Serial);
  bus.set_communication_mode(PJON_HALF_DUPLEX);
  // Set RS485 reception enable pin
  bus.strategy.set_RS485_rxe_pin(PIN_RS485_RX);
  // Set RS485 transmission enable pin
  bus.strategy.set_RS485_txe_pin(PIN_RS485_TX);
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  bus.set_receiver(receiverFunction);
  bus.set_error(errorHandler);
  bus.begin();
}
void setBaudRate() {
  Serial.flush();
  Serial.begin(readULong(EEPROM_SERIAL_BAUD));
  //Serial.begin(9600); //Temporary while sorting serial settings out
}

//Throws an error if the length is below a certain number or the length is not equal to a number if notEqualTo is true.
bool isLengthError(unsigned int length, unsigned int compareVal, bool notEqualTo) {
  if((length < compareVal) || ((length != compareVal) && notEqualTo)) {
    errorHandler(ERR_PACKET_LENGTH,length,NULL);
    return true; //Stop execution there and don't continue.
  }
  return false;
}
