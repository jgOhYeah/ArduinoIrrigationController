#include <CapacitiveSensor.h>
CapacitiveSensor   csSensor = CapacitiveSensor(12,A5);

void setup() {
  csSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate, we don't need it.
  csSensor.set_CS_Timeout_Millis(5000);           //raise timeout to allow for a more accurate reading
  // put your setup code here, to run once:
  Serial.begin(38400);
  Serial.println(F("Capacitive Sensor test"));
  #define COMPILED_STRING "Compiled at " __TIME__ " on " __DATE__ "."
  Serial.println(F(COMPILED_STRING));
  Serial.println();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long readTime = millis();
  long total = csSensor.capacitiveSensor(100); //run 100 measure samples
  readTime = millis() - readTime;
  Serial.print(F("Time: "));
  Serial.print(millis());
  Serial.print(F("\tCapacitance: "));
  Serial.print(total);
  Serial.print(F("\tReading Time: "));
  Serial.println(readTime);
  delay(2000);
}
