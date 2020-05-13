
void checkForSettings() {
  if(button.isPressed()) { //Was the button pressed on startup?
    flashLeds(B01101111); //All leds fast flash - Up and down starting on, half starting off.
    unsigned long startTime = millis();
    while(millis() - startTime < LONG_PRESS_TIME) {
      callback.check();
      if(!button.isPressed()) { //If someone lets go of the button, exit
        flashLeds(0); //Swith off all leds
        return;
      }
    }
    flashLeds(B10111111); //All leds slow flash - All starting on.
    digitalWrite(PIN_RS485_RX,HIGH); //Disable the rs485 transiever to use the serial port to talk usb.
    digitalWrite(PIN_RS485_TX,LOW);
    //We are now in settings mode
    Serial.setTimeout(SERIAL_TIMEOUT);
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
      bool lastButtonState = button.isPressed();
      while(!Serial.available()) { //The looping part of the settings menu - do this stuff while waiting for input or not sending data.
        bool buttonPressed = button.isPressed();
        callback.check(); //Flash the leds
        if(!btnNotPressed && !buttonPressed) {
          btnNotPressed = true; //The button must be released before it can be pressed again
        }
        if(button.checkButton()) { //must be called before button.longPress()
          flashLeds(B01101010); //Set up to flash a different pattern the first time the button is pressed
        } else if(lastButtonState == true && buttonPressed == false) { //We released the button
          flashLeds(B10111111); //Set back to the other way
        }
        lastButtonState = buttonPressed;
        if(button.longPress(LONG_PRESS_TIME) && btnNotPressed) { //If the button has been pressed for a long time, reset the eeprom value for the serial baud rate (safety measure)
          btnNotPressed = false;
          unsigned long oldBaud = readULong(EEPROM_SERIAL_BAUD);
          //reset the eeprom baud rate
          Serial.println(F("About to set the baud rate back to " BAUD_RATE_STRING ".")); //A quick message for anyone listening
          writeULong(EEPROM_SERIAL_BAUD,DEFAULT_BAUD_RATE);
          setBaudRate();
          Serial.print(F("Baud rate is now "));
          Serial.print(DEFAULT_BAUD_RATE);
          Serial.println(F("bps."));
          Serial.print(F("Baud rate was "));
          Serial.print(oldBaud);
          Serial.println(F("bps."));
          Serial.println(F("To view the help text, type \"1\" or \"h\" and then enter."));
          specialFlashLeds(B01010010,2000);
        }
      } //Wait until we have something to read - check without removing from buffer
      int incomingCommand = Serial.read();
      printPrompt = true;
      switch(incomingCommand) {
        //Ignore enter and nothing sent
        case 13: //Carraige return
        case 10: //New line
        case -1: //Stray char in buffer or nothing to read
          printPrompt = false;
          break;
        case '0': { //Exit settings
          Serial.print(F("Are you sure you want to exit settings?"));
          if(confirmationDialog()) {
            Serial.println(F("Exiting settings and returning to normal operation."));
            digitalWrite(PIN_RS485_RX,LOW); //Re-enable the rs485 transiever to use the serial port to talk usb.
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
        /*case '3': { //Bus id
          Serial.print(F("The "));
          Serial.print(F("bus id"));
          Serial.print(F(" of this bay "));
          Serial.print(F("is currently "));
          printBusId();
          Serial.println(F('.'));
          Serial.print(F("To change this, type all four new values separated by a '.' or space. e.g. \"aaa.bbb.ccc.ddd\""));
          byte tempId[4];
          clearSerialBuff();
          byte i = 0;
          for(; i < 4; i++) {
            long inputNumber = serialParseInt();
            if(inputNumber == SERIAL_INPUT_TIMEOUT_VALUE) {
              Serial.println(F("Timed out after 10 minutes waiting for response."));
              break;
            } else if(outsideRange(inputNumber,0,255)) {
              //Invalid
              Serial.println(F("Each value needs to be between 0 and 255 (inclusive)."));
              break;
            }
          }
          if(i == 4) {
            //Copy temp to current, save and refresh
            for(i = 0; i < 4; i++) {
              busId[i] = tempId[i]; //Copy the array
            }
            saveSettings();
            // included in saveSettings() calculateSettings(); //Not actually possible (AFAIK) to set the PJON bus id at runtime, so commenting this code of for possible use later
          } else { //Something went wrong and not all loops were completed, do not update
            Serial.print(F("Exiting without setting a value."));
            Serial.println(F("Please run this command again if you want to update this."));
            Serial.println();
          }
        }*/
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
          long number = acceptNewValue(1,MAX_BAY_TIME);
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
          long number = acceptNewValue(1,MAX_BAY_TIME);
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
          Serial.print(readULong(EEPROM_SERIAL_BAUD));
          Serial.println(F("."));
          long number = acceptNewValue(MIN_SERIAL_BAUD,MAX_SERIAL_BAUD); //Atmega 328ps can get up to 2Mbps, although I'm not sure that the network will be able to! Interesting read: https://arduino.stackexchange.com/questions/296/how-high-of-a-baud-rate-can-i-go-without-errors
          if(number > 0) {
            Serial.print(F("You are about to change the baud rate to "));
            Serial.print(number);
            Serial.println(F("bps."));
            Serial.print(F("Are you "));
            Serial.print(F("sure you want to do this?"));
            Serial.println(F(" Changing the baud rate to another value will mean that the settings for the rest of the network will have to be changed to match, as will those of the computer currently connected to this bay."));
            Serial.println(F("If for some reason you cannot connect to or communicate with this bay after changing the baud rate, reset the baud rate to " BAUD_RATE_STRING "bps by pressing the button for at least 5 seconds after entering settings mode."));
            Serial.print(F("The "));
            Serial.print(F("default")); //New
            Serial.print(F(" baud rate will be set the next time this bay controller is reset"));
            Serial.println(F("."));
            Serial.print(F("Are you "));
            Serial.print(F("REALLY "));
            Serial.print(F("sure you want to do this?"));
            if(confirmationDialog()) {
              //We got a yes
              writeULong(EEPROM_SERIAL_BAUD,number); //Save it 
              Serial.print(F("Do you want to change the baud rate now? Otherwise, the new"));
              Serial.print(F(" baud rate will be set the next time this bay controller is reset"));
              if(confirmationDialog()) {
                setBaudRate();
                Serial.print(F("Baud rate is now "));
                Serial.print(DEFAULT_BAUD_RATE);
                Serial.println(F("bps."));
              }
            }
          }
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
  //Serial.println(F(" {3} Change the serial bus id.")); //PJON bus id setting at runtime is not implemented yet (AFAIK)
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
  Serial.println(readULong(EEPROM_SERIAL_BAUD));
  Serial.print(F(" - Bus id (need to reprogram to change): "));
  printBusId();
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
  #define fullSoftwareVers "Software version: " SOFTWARE_VERSION
  Serial.println(F(fullSoftwareVers));
  Serial.println(F(compiledDatetime));
  Serial.println(F("For more news, information, updates and improvements, go to \"https://github.com/jgOhYeah/ArduinoIrrigationController\"."));
  //A message that people hopefully read before they stuff things up about how to get this communicating again :)
  Serial.print(F("A quick reminder: "));
  Serial.println(F("If for some reason you cannot connect to or communicate with this bay after changing the baud rate, reset the baud rate to " BAUD_RATE_STRING "bps by pressing the button for at least 5 seconds after entering settings mode."));
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
    callback.check(); //Flash the leds
    number = serialParseInt();
    switch(number) {
      case SERIAL_INPUT_TIMEOUT_VALUE: //Timed out
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
  while(millis() - startTime < SERIAL_TIMEOUT && involuntryExit) { //Set a timeout of 10 minutes
    callback.check(); //Flash the leds
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
void printBusId() {
  for(byte i = 0; i < 3; i++) {
    Serial.print(busId[i]);
    Serial.print('.');
  }
  Serial.println(busId[3]); //No dot afterwards
}
long serialParseInt() { //A slightly improved version of parseint that accepts returns a negative number instead of 0 on timeout.
  char charBuffer[SERIAL_INPUT_MAX_DIGITS+1]; //For 10 digit number
  byte charCount = 0; // = Serial.peek();
  unsigned long startTime = millis();
  long number = 0;
  bool isNegative = false;
  //Wait until there is something
  //Serial.print(F("\tAbout to clear buffer"));
  /*while(!isDigit(charCount) && charCount != '-' && charCount != -1 && millis() - startTime <= SERIAL_TIMEOUT) { //Negative sign and negative one which means nothing to read
    if(Serial.available()) {
      //Serial.print(F("WaitChar: "));
      //Serial.println(charCount);
      Serial.read(); //Remove the char as we don't care about it.
      charCount = Serial.peek(); //Get the next digit
    }
  }*/
  while(millis() - startTime <= SERIAL_TIMEOUT) {
    if(Serial.available()) {
      char currentChar = Serial.peek();
      if(currentChar >= '0' && currentChar <= '9') {
        //Serial.print(F("Received "));
        //Serial.print(currentChar);
        //Serial.println(F(". Proceeding."));
        break;
      } else if(currentChar == '-') {
        //Serial.println(F("Got NEGATIVE!"));
        Serial.read(); //Clear the minus sign
        isNegative = true;
        break;
      } else {
        char tempChar = Serial.read(); //Not wanted remove it.
        //Serial.print(F("Removed "));
        //Serial.println(tempChar);
      }
      
    }
  }
  //Serial.println(F("\tAbout to receive"));
  //Get the something
  while(millis() - startTime <= SERIAL_TIMEOUT) {
    if(Serial.available()) {
      //Serial.print(F("\tGot "));
      charBuffer[charCount] = Serial.read();
      //Serial.println(charBuffer[charCount]);
      if((charBuffer[charCount] >= '0' && charBuffer[charCount] <= '9') && (charCount < SERIAL_INPUT_MAX_DIGITS)) { //The negative must be the first char received
        //Accepted value
        charCount++; //Get ready for the next
        //Serial.print(F("Charcount: "));
        //Serial.println(charCount);
      } else {
        //Give up. charCount will hold the number of digits.
        break;
      }
    }
  }
  //Serial.print(F("Processing. There are "));
  //Serial.print(charCount);
  //Serial.print(F(" chars in the buffer. They are: \""));
  for(byte a = 0; a < charCount; a++) {
    //Serial.print(charBuffer[a]);
  }
  //Serial.println(F("\"."));
  if(millis() - startTime > SERIAL_TIMEOUT) {
    number = SERIAL_INPUT_TIMEOUT_VALUE;
    //Serial.println(F("Timed out"));
    return number;
  }
  char maxChars[] = {'2','1','4','7','4','8','3','6','4','7'}; //Max size of long: 2,147,483,647
  bool isAtMax = true;
  //Serial.println(charCount);
  for(byte i = 0; i < charCount; i++) {
    //Serial.print(F("Loop: "));
    //Serial.print(i);
    //Serial.print(F("\tChar: "));
    //Serial.print(charBuffer[i]);
    //Serial.print(F("\tisAtMax: "));
    //Serial.print(isAtMax);
    //Serial.print(F("\tNumber so far: "));
    //Serial.println(number);
    if(charCount >= SERIAL_INPUT_MAX_DIGITS) { //Don't want to overflow
      if(charBuffer[i] >= maxChars[i] && isAtMax) {
        charBuffer[i] = maxChars[i];
        //Serial.print(F("Char is too big! Set to "));
        //Serial.println(maxChars[i]);
      } else {
        //Serial.println(F("isAtMax is now false."));
        isAtMax = false;
      }
    } else {
      isAtMax = false;
    }
    number+= (charBuffer[i] - '0') * intPosPow(10,charCount-i-1); //Add that digit to the number  digit*10^pos
  }
  //Serial.print(F("Is negative: "));
  //Serial.println(isNegative);
  //Serial.println(F("Done processing"));
  if(isNegative) {
    number*= -1; //Make the number negative;
    //Serial.println(F("Number is now negative"));
  }
  //Serial.print(F("Number is: "));
  //Serial.println(number);
  return number;
}

long intPosPow(byte base, byte power) {
  long number = base;
  if(power == 0) {
    number = 1;
  } else if (power > 1) { //power >1
    for(byte i = 1; i < power; i++) {
      number *= base; //Square the number
    }
  }
  return number;
}
