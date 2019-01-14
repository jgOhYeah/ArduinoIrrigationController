#include <CapacitiveSensor.h>
#include <openGLCD.h>
CapacitiveSensor   csSensor = CapacitiveSensor(12,A5);

long maximum = -2147483647; //Extremes - should sort themselves out the first time round
long minimum = 2147483647;
void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  tone(3,1023); //Drive the charge pump for the contrast of the display as the displays I have been using do not have -10v supplies.
  csSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate, we don't need it.
  csSensor.set_CS_Timeout_Millis(5000);           //raise timeout to allow for a more accurate reading
  // put your setup code here, to run once:
  Serial.begin(38400);
  Serial.println(F("Capacitive Sensor test"));
  #define COMPILED_STRING "Compiled at " __TIME__ " on " __DATE__ "."
  Serial.println(F(COMPILED_STRING));
  Serial.println();
  GLCD.Init();
  GLCD.ClearScreen();
  GLCD.SelectFont(Callibri11);
  GLCD.SetFontColor(PIXEL_OFF);
  GLCD.DrawString(F("Capacitance Test"), 2, gTextfmt_top, eraseFULL_LINE);
  GLCD.SetFontColor(PIXEL_ON);
  GLCD.DrawString(F(COMPILED_STRING), gTextfmt_left, gTextfmt_row(1), eraseFULL_LINE);
  delay(5000);
  digitalWrite(LED_BUILTIN,LOW);
  GLCD.EraseTextLine(2);
}

void loop() {
  digitalWrite(LED_BUILTIN,HIGH);
  GLCD.SelectFont(Callibri11);
  // put your main code here, to run repeatedly:
  unsigned long readTime = millis();
  long total = csSensor.capacitiveSensorRaw(100); //run 100 measure samples
  readTime = millis() - readTime;
  digitalWrite(LED_BUILTIN,LOW);
  char charBuffer[30];
  if(total > maximum) {
    maximum = total;
  }
  if(total < minimum) {
    minimum = total;
  }
  sprintf(charBuffer,"Capacitance: %ld",total);
  GLCD.DrawString(charBuffer, gTextfmt_left, gTextfmt_row(1), eraseFULL_LINE);
  sprintf(charBuffer,"Time: %ld",millis());
  GLCD.DrawString(charBuffer, gTextfmt_left, gTextfmt_row(2), eraseFULL_LINE);
  GLCD.SelectFont(newbasic3x5);
  sprintf(charBuffer,"Max: %ld",maximum);
  GLCD.DrawString(charBuffer, gTextfmt_right, GLCD.CenterY+1, eraseNONE);
  sprintf(charBuffer,"Min: %ld",minimum);
  GLCD.DrawString(charBuffer, gTextfmt_left, GLCD.CenterY+1, eraseNONE);
  GLCD.DrawHBarGraph(GLCD.Left, GLCD.CenterY+10, GLCD.Width, GLCD.Height/2-10, 1, minimum/32, maximum/32, total/32); //divided by 16 to make it fit within range
  Serial.print(F("Time: "));
  Serial.print(millis());
  Serial.print(F("\tCapacitance: "));
  Serial.print(total);
  Serial.print(F("\tReading Time: "));
  Serial.println(readTime);
  //delay(500);
}
