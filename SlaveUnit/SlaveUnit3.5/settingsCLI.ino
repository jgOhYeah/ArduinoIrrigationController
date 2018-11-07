void checkForSettings() {
  if(button.isPressed()) { //Was the button pressed on startup?
    ALL_LEDS(HIGH);
    unsigned long startTime = millis();
    unsigned long startTime2 = startTime;
    bool ledsOn = false;
    while(millis() - startTime < longPressTime) {
      if(millis() - startTime2 >= ledFlashSpeed) {
        startTime2 = millis();
        ledsOn = !ledsOn;
        digitalWrite(upLed,ledsOn);
        digitalWrite(halfLed,!ledsOn);
        digitalWrite(downLed,ledsOn);
      }
      if(!button.isPressed()) { //If someone lets go of the button, exit
        ALL_LEDS(LOW);
        return;
      }
    }
    ALL_LEDS(LOW);
    digitalWrite(rs485rxPin,HIGH); //Disable the rs485 transiever to use the serial port to talk usb.
    digitalWrite(rs485txPin,LOW);
    //We are now in settings mode
    Serial.setTimeout(serialTimeout);
    Serial.println(F("Hello, you have just entered settings mode."));
    printCurrentSettings();
    printHelp();
    bool printPrompt = true;
    bool btnNotPressed = false;
    while(true) { //Run the settings menu unitil told to exit or reset
      if(printPrompt) {
        Serial.println();
        Serial.println();
        Serial.print(F("Ready to accept your input. "));
        Serial.println(F("To view the help text, type \"1\" or \"h\" and then enter."));
        Serial.println();
      }
      while(!Serial.available()) { //The looping part of the settings menu - do this stuff while waiting for input or not sending data.
        bool buttonPressed = button.isPressed();
        if(millis() - startTime >= ledFlashSpeed) {
          startTime = millis();
          ledsOn = !ledsOn;
          digitalWrite(LED_BUILTIN,ledsOn);
          if(buttonPressed) {
            digitalWrite(upLed,ledsOn);
            digitalWrite(halfLed,!ledsOn);
            digitalWrite(downLed,ledsOn);
          }
        }
        if(!btnNotPressed && !buttonPressed) {
          btnNotPressed = true; //The button must be released before it can be pressed again
        }
        button.checkButton(); //must be called before button.longPress()
        if(button.longPress(longPressTime) && btnNotPressed) { //If the button has been pressed for a long time, reset the eeprom value for the serial baud rate (safety measure)
          btnNotPressed = false;
          unsigned long oldBaud = readULong(serialBaudAddress);
          //Reset the eeprom baud rate
          Serial.println(F("About to set the baud rate back to 9600.")); //A quick message for anyone listening
          writeULong(serialBaudAddress,defaultBaudRate);
          setBaudRate();
          Serial.print(F("Baud rate is now "));
          Serial.print(defaultBaudRate);
          Serial.println(F("bps."));
          Serial.print(F("Baud rate was "));
          Serial.print(oldBaud);
          Serial.println(F("bps."));
          Serial.println(F("To view the help text, type \"1\" or \"h\" and then enter."));
          //Flash leds?
          ALL_LEDS(HIGH);
        }
      } //Wait until we have something to read - check without removing from buffer
      int incomingCommand = Serial.read();
      printPrompt = true;
      switch(incomingCommand) {
        //Ignore enter and nothing sent
        case 13:
        case 10:
        case -1:
          printPrompt = false;
          break;
        case '0': { //Exit settings
          Serial.print(F("Are you sure you want to exit settings?"));
          if(confirmationDialog()) {
            Serial.println(F("Exiting settings and returning to normal operation."));
            digitalWrite(rs485rxPin,LOW); //Re-enable the rs485 transiever to use the serial port to talk usb.
            return;
          }
          break;
        }
        case '1': //A couple of different characters people can use to get help if needed
        case 'h':
        case 'H':
          printHelp();
          break;
        case '2': //Current status
          printCurrentSettings();
          break;
        case '3': { //Serial address
          Serial.print(F("The "));
          Serial.print(F("address"));
          Serial.print(F(" of this bay "));
          Serial.print(F("is currently "));
          Serial.print(myId);
          Serial.println(F("."));
          long number = acceptNewValue(1,255);
          if(number > 0) {
            myId = byte(number);
            saveSettings();
            Serial.print(F("The "));
            Serial.print(F("address"));
            Serial.print(F(" of this bay "));
            Serial.print(F("has been updated successfully to "));
            Serial.print(myId);
            Serial.println(F("."));
          }
          break;
        }
        case '4': { //Halfway position
          Serial.print(F("The "));
          Serial.print(F("\"halfway\" position"));
          Serial.print(F(" of this bay "));
          Serial.print(F("is currently "));
          Serial.print(halfwayPos);
          Serial.println(F("% open."));
          long number = acceptNewValue(1,99);
          if(number > 0) {
            halfwayPos = byte(number);
            saveSettings();
            Serial.print(F("The "));
            Serial.print(F("\"halfway\" position"));
            Serial.print(F(" of this bay "));
            Serial.print(F("has been updated successfully to "));
            Serial.print(halfwayPos);
            Serial.println(F("% open."));
          }
          break;
        }
        case '5': { //Up travel time
          Serial.print(F("T"));
          Serial.print(F("he time (in milliseconds) that the bay takes to fully "));
          Serial.print(F("open "));
          Serial.print(F("is currently "));
          Serial.print(upTravelSpeed);
          Serial.println(F("."));
          long number = acceptNewValue(1,maxBayTime);
          if(number > 0) {
            upTravelSpeed = number;
            saveSettings();
            Serial.print(F("T"));
            Serial.print(F("he time (in milliseconds) that the bay takes to fully "));
            Serial.print(F("open "));
            Serial.print(F("has been updated successfully to "));
            Serial.print(upTravelSpeed);
            Serial.println(F("."));
          }
          break;
        }
        case '6': {//Down travel time
          Serial.print(F("T"));
          Serial.print(F("he time (in milliseconds) that the bay takes to fully "));
          Serial.print(F("close "));
          Serial.print(F("is currently "));
          Serial.print(downTravelSpeed);
          Serial.println(F("."));
          long number = acceptNewValue(1,maxBayTime);
          if(number > 0) {
            downTravelSpeed = number;
            saveSettings();
            Serial.print(F("T"));
            Serial.print(F("he time (in milliseconds) that the bay takes to fully "));
            Serial.print(F("close "));
            Serial.print(F("has been updated successfully to "));
            Serial.print(downTravelSpeed);
            Serial.println(F("."));
          }
          break;
        }
        case '7': {//Baud rate
          Serial.print(F("The baud rate used for serial communications (with the rs485 bus and this settings menu) "));
          Serial.print(F("is currently "));
          Serial.print(readULong(serialBaudAddress));
          Serial.println(F("."));
          long number = acceptNewValue(minSerialBaud,maxSerialBaud); //Atmega 328ps can get up to 2Mbps, although I'm not sure that the network will be able to! Interesting read: https://arduino.stackexchange.com/questions/296/how-high-of-a-baud-rate-can-i-go-without-errors
          if(number > 0) {
            Serial.print(F("You are about to change the baud rate to "));
            Serial.print(number);
            Serial.println(F("bps."));
            Serial.print(F("Are you "));
            Serial.print(F("sure you want to do this?"));
            Serial.println(F(" Changing the baud rate to another value will mean that the settings for the rest of the network will have to be changed to match, as will those of the computer currently connected to this bay."));
            Serial.println(F("If for some reason you cannot connect to or communicate with this bay after changing the baud rate, reset the baud rate to 9600bps by pressing the button for at least 5 seconds after entering settings mode."));
            Serial.print(F("The "));
            Serial.print(F("default")); //New
            Serial.print(F(" baud rate will be set the next time this bay controller is reset"));
            Serial.println(F("."));
            Serial.print(F("Are you "));
            Serial.print(F("REALLY "));
            Serial.print(F("sure you want to do this?"));
            if(confirmationDialog()) {
              //We got a yes
              writeULong(serialBaudAddress,number); //Save it 
              Serial.print(F("Do you want to change the baud rate now? Otherwise, the new"));
              Serial.print(F(" baud rate will be set the next time this bay controller is reset"));
              if(confirmationDialog()) {
                setBaudRate();
                Serial.print(F("Baud rate is now "));
                Serial.print(defaultBaudRate);
                Serial.println(F("bps."));
              }
            }
          }
          break;
        }
        default:
          printNotUnderstand();
          clearSerialBuffer();
      }
    }
  }
}
//Generic syntax error message
void printNotUnderstand() {
  Serial.print(F("Sorry, I did not recognise what you were sending. "));
  Serial.println(F("To view the help text, type \"1\" or \"h\" and then enter."));
}
//The help prompts
void printHelp() {
  Serial.println();
  Serial.println();
  Serial.println(F("Please select an option below by typing the number next to it and pressing enter."));
  Serial.println(F(" {0} Exit settings and run normally"));
  Serial.println(F(" {1, h or H} View the help text again (this)."));
  Serial.println(F(" {2} View the current settings and status."));
  Serial.println(F(" {3} Set bay address."));
  Serial.println(F(" {4} Set the percentage (1 to 99) that the bay is open in the halfway position."));
  Serial.print(F(" {5"));
  Serial.print(F("} Set t"));
  Serial.print(F("he time (in milliseconds) that the bay takes to fully "));
  Serial.println(F("open."));
  Serial.print(F(" {6"));
  Serial.print(F("} Set t"));
  Serial.print(F("he time (in milliseconds) that the bay takes to fully "));
  Serial.println(F("close."));
  //Serial.println(F(" {7} Set the bay to a certain position (open, half or shut).")); //TODO: Not yet implemented
  Serial.println(F(" {7} Change the baud rate of the serial connection (Potentially DANGEROUS!)."));
}
//Prints current configuration
void printCurrentSettings() {
  Serial.println();
  Serial.println();
  Serial.println(F("Current settings are as follows:"));
  Serial.print(F(" - Serial Baud Rate: "));
  Serial.println(readULong(serialBaudAddress));
  Serial.print(F(" - Bay address: "));
  Serial.println(myId);
  Serial.print(F(" - Time (in milliseconds) taken for bay to travel ")); //Hopefully the compiler recognises that there are shared strings here and saves memory.
  Serial.print(F("upwards: "));
  Serial.println(upTravelSpeed);
  Serial.print(F(" - Time (in milliseconds) taken for bay to travel ")); //Hopefully the compiler recognises that there are shared strings here and saves memory.
  Serial.print(F("downwards: "));
  Serial.println(downTravelSpeed);
  Serial.print(F(" - The percentange open that the bay is in the \"halfway\" position: "));
  Serial.println(halfwayPos);
  Serial.println();

  Serial.println(F("From these settings, I have calculated:"));
  Serial.print(F(" - Time from the "));
  Serial.print(F("bottom"));
  Serial.print(F(" going "));
  Serial.print(F("upwards"));
  Serial.print(F(" that this bay takes to reach the halfway position (in milliseconds): "));
  Serial.println(halfUpTime);
  Serial.print(F(" - Time from the "));
  Serial.print(F("top"));
  Serial.print(F(" going "));
  Serial.print(F("downwards"));
  Serial.print(F(" that this bay takes to reach the halfway position (in milliseconds): "));
  Serial.println(halfDownTime);
  Serial.println();

  #define compiledDatetime "This software was compiled at " __TIME__ " on " __DATE__ "."
  Serial.println(F(compiledDatetime));
  Serial.println(F("For more news, information, updates and improvements, go to \"https://github.com/jgOhYeah/ArduinoIrrigationController\"."));
  //A message that people hopefully read before they stuff things up about how to get this communicating again :)
  Serial.print(F("A quick reminder: "));
  Serial.println(F("If for some reason you cannot connect to or communicate with this bay after changing the baud rate, reset the baud rate to 9600bps by pressing the button for at least 5 seconds after entering settings mode."));
}
//Stuff to receive a number within a range with prompts from the user.
long acceptNewValue(unsigned long minimum, unsigned long maximum) {
  Serial.print(F("To change this, type the new value in and then press enter."));
  printNewValueRange(minimum,maximum);
  Serial.println(F("To exit without setting a value, type \"-1\" then enter."));
  clearSerialBuffer(); //Clear it here A to be called by most functions and B to give unwanted serial data time to be recieved.
  bool incorrectNumber = true;
  long number;
  do { //Will always run at least once
    number = Serial.parseInt();
    switch(number) {
      case 0: //Timed out
        Serial.println(F("Timed out after 10 minutes waiting for response. Please try again."));
        //No break on purpose
      case -1: //Did not want to change value
        Serial.println(F("Exiting without setting a value."));
        Serial.println();
        incorrectNumber = false;
        break;
      default:
        if(outsideRange(number,minimum,maximum)) {
          Serial.print(F("The value you have entered, "));
          Serial.print(number);
          Serial.print(F(", is outside the required range."));
          printNewValueRange(minimum,maximum);
          Serial.println(F("Please try again."));
        } else {
          incorrectNumber = false; //We have a number that we will work with
        }
    }
  } while (incorrectNumber);
  return number;
}
void printNewValueRange(unsigned long minimum, unsigned long maximum) {
  Serial.print(F(" The new value must be between "));
  Serial.print(minimum);
  Serial.print(F(" and "));
  Serial.print(maximum);
  Serial.println(F(" (inclusive)."));
}
void clearSerialBuffer() {
  while(Serial.available()) {
    Serial.read();
  }
}
bool confirmationDialog () {
  Serial.println(F(" (Type \"y\" for yes or \"n\" for no)."));
  clearSerialBuffer();
  unsigned long startTime = millis();
  bool involuntryExit = true;
  while(millis() - startTime < serialTimeout && involuntryExit) { //Set a timeout of 10 minutes
    if(Serial.available()) {
      switch(Serial.read()) {
        case 'y': //Account for uppercase and lowercase
        case 'Y':
          return true;
          break;
        //Ignore new lines
        case 13:
        case 10:
          break;
        default:
          involuntryExit = false;
      }
    }
  }
  if(involuntryExit) {
    Serial.println(F("Timed out after 10 minutes waiting for a response. Please try again."));
  }
  Serial.println(F("Returning to settings menu."));
  Serial.println();
  return false;
}
