void getBayStatus() {
  for(byte i = 0; i < numberOfDevices; i++) {
    bayStatus[i] = bayNotPresent;
  }
  //Get what state the bays are in if they know
  byte error = serialCom.sendCommandWithCallbacks(allDevices,reportStatus,overwriteStatus,enableLineWait,callbackTimeout);
  if(error != serialSuccess) {
    createErrorMessage(error,false,NULL);
  }
  //Wait until the timeout or all bays are present and accounted for
  unsigned long endTime = millis() + callbackTimeout;
  bool repliesStillNeeded = true;
  //repeat for a while to let replies come in. Give up after a timeout to say that those that have not replied are not connected
  while(millis() <= endTime && repliesStillNeeded) {
    //Serial.println(freeRam());
    checkForReplies();
    //Iterate over the array and check if any still equal not present. repliesStillNeeded will still be false if not.
    repliesStillNeeded = false;
    for(byte i = 0; i < numberOfDevices; i++) {
      if(bayStatus[i] == bayNotPresent) {
        repliesStillNeeded = true;
      }
    }
  }
}
void checkForReplies() {
  byte address = 0;
  byte commandOne = 0;
  byte commandTwo = 0;
  byte recieveStatus = serialCom.recieveCommandWithCallbacks(address, commandOne, commandTwo, callbackTimeout);
  switch(recieveStatus) {
    case serialSuccess:
      //Analyse as there is something
      if(address >= numberOfDevices) {
        recieveStatus = serialAddressTooLarge;
        createErrorMessage(recieveStatus,false,NULL);
        break;
      }
      if(commandTwo == overwriteStatus) {
        //If the command is not right
        if(commandOne > bayNotPresent) {
          recieveStatus = serialInvalidCommand;
          bool offendingBays[numberOfDevices];
          //Just in case not already false
          for(byte a = 0; a < numberOfDevices; a++) {
            offendingBays[a] = false;
          }
          offendingBays[address] = true;
          createErrorMessage(recieveStatus,true,offendingBays);
        }
        bayStatus[address] = commandOne;
        //Only redraw the screen if already on main screen
        if(currentScreen == mainScreen) {
          lcd.setCursor(address,1);
          lcd.write(byte(bayOptions[bayStatus[address]]));
          lcd.setCursor(cursorPos,1);
        }
        //Redraw the lcd
      } else if (commandOne != bayStatus[address]) {
        //Does not match and checking only.
        recieveStatus = serialRxNotMatchExpected;
        bool offendingBays[numberOfDevices];
        //Just in case not already false
        for(byte a = 0; a < numberOfDevices; a++) {
          offendingBays[a] = false;
        }
        offendingBays[address] = true;
        createErrorMessage(recieveStatus,true,offendingBays);
      }
      break;
    case serialNothingToRead:
      //Don't do anything as nothing to read
      break;
    default: //Anything else is an error
      Serial.print(address);
      Serial.print(',');
      Serial.print(commandOne);
      Serial.print(',');
      Serial.println(commandTwo);
      createErrorMessage(recieveStatus,false,NULL);
      break;
  }
}
void checkCallbacks(){
  //Hello
  bool offendingBays[numberOfDevices];
  //Just in case not already false
  for(byte i = 0; i < numberOfDevices; i++) {
    offendingBays[i] = false;
  }
  unsigned long currentTime = millis();
  bool errorRequired = false;
  for(byte i = 0; i < numberOfDevices; i++) {
    if(serialCom.callbacks[i] <= currentTime && serialCom.callbacks[i] != callbacksInactive) {
      //Timed out
      offendingBays[i] = true;
      errorRequired = true;
      serialCom.callbacks[i] = callbacksInactive;
      bayStatus[i] = bayNotPresent;
    }
  }
  if(errorRequired) {
    createErrorMessage(serialNoReply,true,offendingBays);
  }
}

