void setEepromSettings() {
  displayHelp();
  //Time when to give up
  unsigned long giveUpTime = millis() + eepromInactivityTimeout;
  while(giveUpTime >= millis()) {
    flashLeds();
    byte addressCommand, commandOne, commandTwo;
    byte recieveStatus = serialCom.recieveCommand(addressCommand,commandOne,commandTwo,serialRecieveUserTimeout);
    switch(recieveStatus) {
      //The checksum will not match, but just in case it does
      case serialSuccess:
      case serialChecksumNotMatch:
        //Reset the time every time there is a serial event
        giveUpTime = millis() + eepromInactivityTimeout;
        switch(addressCommand) {
          case 'a':
            if(commandOne == serialWrite) {
              //address read and write
              EEPROM.write(deviceIdAddress,commandTwo);
              Serial.print(F("Set the device id to "));
              Serial.println(commandTwo);
            } else {
              Serial.print(F("The device id is currently "));
              Serial.println(EEPROM.read(deviceIdAddress));
            }
            break;
          case 'o':
            if(commandOne == serialWrite) {
              //address read and write
              EEPROM.write(bayOpenTimeAddress,commandTwo);
              Serial.print(F("Set the bay open time to "));
              Serial.print(commandTwo);
              tenthsOfASecond();
            } else {
              Serial.print(F("The bay open time is currently "));
              Serial.print(EEPROM.read(bayOpenTimeAddress));
              tenthsOfASecond();
            }
            break;
          case 'c':
            if(commandOne == serialWrite) {
              //address read and write
              EEPROM.write(bayCloseTimeAddress,commandTwo);
              Serial.print(F("Set the bay close time to "));
              Serial.print(commandTwo);
              tenthsOfASecond();
            } else {
              Serial.print(F("The bay close time is currently "));
              Serial.print(EEPROM.read(bayCloseTimeAddress));
              tenthsOfASecond();
            }
            break;
          case 'f':
            if(commandOne == serialWrite) {
              //address read and write
              EEPROM.write(bayHalfTimeFractionAddress,commandTwo);
              Serial.print(F("Set the bay part open fraction to "));
              Serial.print(commandTwo);
              over255();
            } else {
              Serial.print(F("The bay part open fraction is currently "));
              Serial.print(EEPROM.read(bayHalfTimeFractionAddress));
              over255();
            }
            break;
          case 'x':
            Serial.println(F("Exiting"));
            return;
            break;
          case 'h':
            displayHelp();
            break;
          default:
            Serial.println(F("Invalid command. See help below"));
            displayHelp();
        }
        break;
      case serialRecieveTimedOut:
        Serial.println(F("Recieving command timed out. Please be quicker."));
        break;
      case serialNothingToRead:
        break;
      default:
        Serial.print(F("Something went wrong. See help below and try again. Error code "));
        Serial.println(recieveStatus);
        displayHelp();
        break;
    }
    flashLeds();
  }
  Serial.println(F("Timed out. Exiting."));
}
void displayHelp() {
  //Help menu
  Serial.println(F("Bay unit slave config"));
  Serial.println(F("Command structure:"));
  Serial.println(F("~[command][read / write][value to write]:\\n"));
  Serial.println(F("Please note ALL parameters must be provided, even if they are not used."));
  Serial.println(F("COMMANDS:"));
  Serial.println(F("  h = help"));
  Serial.println(F("  a = device address"));
  //Open and shut times separate in case motor spins faster one way than other
  Serial.println(F("  o = open time tenths second"));
  Serial.println(F("  c = close time tenths second"));
  Serial.println(F("  f = fraction of time for bay to be half open (sent as f/255)"));
  Serial.println(F("  x = exit"));
  Serial.println(F("READ / WRITE:"));
  Serial.println(F("  r = read"));
  Serial.println(F("  w = write"));
  Serial.println(F("VALUE:"));
  Serial.println(F("  Send as a byte. i.e. a = 97"));
  Serial.println();
  Serial.println();
  Serial.println();
}
void tenthsOfASecond() {
  Serial.println(F(" tenths of a second"));
}
void over255() {
  Serial.println(F("/255"));
}
