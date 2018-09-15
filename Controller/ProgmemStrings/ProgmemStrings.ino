#include <avr/pgmspace.h>

const char error_0[] PROGMEM = "String 0";   // "String 0" etc are strings to store - change to suit.
const char error_1[] PROGMEM = "String 1";
const char error_2[] PROGMEM = "String 2";
const char error_3[] PROGMEM = "String 3";
const char error_4[] PROGMEM = "String 4";
const char error_5[] PROGMEM = "String 5";

char buffer[30];    // make sure this is large enough for the largest string it must hold
// Then set up a table to refer to your strings.

const char* const string_table[] PROGMEM = {error_0, error_1, error_2, error_3, error_4, error_5};
void setup() {
  i = 0; //Spot want to get
  // put your setup code here, to run once:
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i]))); // Necessary casts and dereferencing, just copy.
}

void loop() {
  // put your main code here, to run repeatedly:

}
