#define SERIAL_TIMEOUT 60000 //11 minutes
#define SERIAL_INPUT_MAX_DIGITS 10 //Maximum number of digits that can be entered when typing something. - 10 digits is the maximum a long and unsigned long can hold
#define SERIAL_INPUT_TIMEOUT_VALUE -2
void setup() {
  Serial.begin(38400);
  Serial.println(F("Serial parsing test"));
}

void loop() {
  Serial.print(F("Please type a number"));
  long number = serialParseInt();
  Serial.print(F(":\tYou entered: "));
  Serial.println(number);
  Serial.println();
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
