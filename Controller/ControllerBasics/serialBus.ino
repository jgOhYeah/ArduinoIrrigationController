/*void serialEvent() {
  static byte message[] = {0,0,0,0};
  message[0] = Serial.read();
  if(message[0] == startChar && Serial.available()) {
    message[
  }
}*/
bool checkCallbacks() {
  //Loop through each. If callback time != 0 and millis() > callback time, throw and error.
  String badBays;
  for(byte i = 0; i < numberOfBays; i++) {
    if(bayCallbacks[i] != 0 && millis() > bayCallbacks[i]) {
      //Disable so does not continually throw errors.
      bayCallbacks[i] = 0;
      badBays += String(i + 1) + ",";
      //Serial.println(String(i + 1) + ",");
      //Serial.println(badBays);
      bayStatus[i] = bayNotPresent;
    }
  }
  //Allow for null char at end of string
  if(badBays.length() > 1) {
    createErrorMessage(sharedStrings[0] + badBays + F("No reply"));
    return false;
  }
  return true;
}
bool checkForReplies(bool dontDownloadMessage) {
  //Read the serial input and stop if didn't work
  if(!readBay()) {
    return false;
  }
  //Compare what bay thinks it is vs what controller says
  if(message[0] == startChar) {
    //Add in error checking for if message bay number >= number of bays?
    //If the message status does not line up with the expected status
    if(message[1] >= numberOfBays) {
      createErrorMessage(F("Invalid bay"));
      return false;
    }
    if(message[2] != bayStatus[message[1]] && dontDownloadMessage) {
      createErrorMessage(sharedStrings[0] + String(message[1] + 1) + sharedStrings[1] + F("not match."));
      return false;
    } else if(!dontDownloadMessage) {
      //If the message contains an invalid char
      if(message[2] >= bayNotPresent) {
        createErrorMessage(sharedStrings[0] + String(message[1] + 1) + sharedStrings[1] + F("invalid"));
        return false;
      }
      bayStatus[message[1]] = message[2];
    }
    //Cancel callback
    bayCallbacks[message[1]] = 0;
  }
  return true;
}
void resetMessage() {
  for(byte i = 0; i < messagePacketLength; i++) {
    message[i] = 0;
  }
}
bool readBay() {
  resetMessage();
  if(Serial.available()) {
    message[0] = Serial.read();
    if(message[0] == startChar) {
      digitalWrite(LED_BUILTIN,HIGH);
      unsigned long startTime = millis();
      //While not timed out and no end char
      byte i = 0;
      while(message[messagePacketLength - 1] != endChar) {
        //Timeoout to stop listening if message is corrupted
        if(millis() - startTime > serialRecieveTimeout) {
          //Create an error message
          createErrorMessage(sharedStrings[1] + F("timeout."));
          resetMessage();
          return false;
        }
        if(Serial.available()) {
          i++;
          //The message is to long
          if(i >= messagePacketLength) {
            createErrorMessage(sharedStrings[1] + F("too long."));
            resetMessage();
            return false;
          }
          message[i] = Serial.read();
        }
      }
      /*
      Serial.println("Message Successfully Recieved");
      Serial.println("\r\nMessage contents:");
      Serial.println("Start Char :" + String(message[0]));
      Serial.println("Bay :" + String(message[1]));
      Serial.println("Command :" + String(message[2]));
      Serial.println("End Char :" + String(message[3]));
      */
      digitalWrite(LED_BUILTIN,LOW);
    }
  }
  return true;
}
bool sendBayCommand(byte bay,byte command) {
  digitalWrite(LED_BUILTIN,HIGH);
  //Wait while the serial enable line is low. Only send when noone else is using the line. Give up after a while as probably shorted to ground.
  unsigned long startTime = millis();
  while(digitalRead(serialEnable) == LOW) {
    if(millis() - startTime > serialEnableTimeout) {
      //Create an error message
      createErrorMessage(F(" Enable line pulled low timeout"));
      return false;
    }
  }
  pinMode(serialEnable,OUTPUT);
  digitalWrite(serialEnable,LOW);
  delay(20);
  //Calcutalte checksum
  
  //Send the command
  //Serial.println("Sending a command to bay " + String(bay) + ". The command is " + String(command) + ".");
  //Serial.write(String(startChar) + String(bay) + String(command) + String(endChar));
  Serial.write(startChar);
  Serial.write(bay);
  Serial.write(command);
  Serial.write(endChar);
  /*
   * Command ends up being something like
   * ~01\n
   * 126, 0, 0, 10
   * ~  , ?, ?, line feed
   */
  //Wait for it all to be sent
  Serial.flush();
  //Set up a callback max time for a response
  //If it is everything, set all to be that time
  unsigned long callbackTime = millis() + bayCallbackMaxTime;
  if(bay == allBays) {
    for(byte i = 0; i < numberOfBays; i++) {
      bayCallbacks[i] = callbackTime;
    }
  } else {
    //Set the individual in the array
    bayCallbacks[bay] = callbackTime;
  }
  delay(20);
  //Set back to an input - disconnect. Resistors pull high.
  pinMode(serialEnable,INPUT);
  digitalWrite(LED_BUILTIN,LOW);
  return true;
}
