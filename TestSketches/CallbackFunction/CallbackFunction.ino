//This works. Now to make it into a library to be easily included.
//See PJON library for structs in libraries.
#include "typedefs.h"
#include "defines.h"
#define maxCallbacks 3
callback callbacks[maxCallbacks];
void setup() {
  Serial.begin(38400);
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.println(F("Callback library test / demo"));
  registerCallback(2000,true,flashLED);
  registerCallback(5000,false,serialTest);
}

void loop() {
  checkCallbacks();
}
void flashLED() {
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  Serial.print(F("Flashing LED at "));
  Serial.print(millis());
  Serial.println(F("ms."));
}
void serialTest() {
  Serial.print(F("Single callback after "));
  Serial.print(millis());
  Serial.println(F("ms."));
}

