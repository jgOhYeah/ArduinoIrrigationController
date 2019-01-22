
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
  char charBuffer[SERIAL_INPUT_MAX_DIGITS]; //For 10 digit number
  byte charCount = 0; // = Serial.peek();
  unsigned long startTime = millis();
  long number = 0;
  bool isNegative = false;
  while(millis() - startTime <= SERIAL_TIMEOUT) {
    if(Serial.available()) {
      char currentChar = Serial.peek();
      if(currentChar >= '0' && currentChar <= '9') {
        break;
      } else if(currentChar == '-') {
		Serial.read();				 
        isNegative = true;
        break;
      } else {
        char tempChar = Serial.read(); //Not wanted remove it.
      }
    }
  }
  //Get the something
  while(millis() - startTime <= SERIAL_TIMEOUT) {
    if(Serial.available()) {
      charBuffer[charCount] = Serial.read();
      if((charBuffer[charCount] >= '0' && charBuffer[charCount] <= '9') && (charCount < SERIAL_INPUT_MAX_DIGITS)) { //The negative must be the first char received
        //Accepted value
        charCount++; //Get ready for the next
      } else {
        //Give up. charCount will hold the number of digits.
        break;
      }
    }
  }
  if(millis() - startTime > SERIAL_TIMEOUT) {
    number = SERIAL_INPUT_TIMEOUT_VALUE;
    return number;
  }
  char maxChars[] = {'2','1','4','7','4','8','3','6','4','7'}; //Max size of long: 2,147,483,647 - if bigger (or smaller), resize the number
  bool isAtMax = true;
  for(byte i = 0; i < charCount; i++) {
    if(charCount == SERIAL_INPUT_MAX_DIGITS) { //Don't want to overflow - only bother if using all 10 digits
      if(charBuffer[i] >= maxChars[i] && isAtMax) {
        charBuffer[i] = maxChars[i];
      } else {
        isAtMax = false;
      }
    } else {
      isAtMax = false;
    }
    number+= (charBuffer[i] - '0') * intPosPow(10,charCount-i-1); //Add that digit to the number  digit*10^pos
  }
  if(isNegative) {
    number*= -1; //Make the number negative;
  }
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
