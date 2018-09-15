void scrollLeds() {
  //Every few seconds
  static unsigned long nextTime = 0;
  static byte currentLedOn = 0;
  unsigned long milliSeconds = millis();
  if(milliSeconds >= nextTime) {
    nextTime = milliSeconds + unknownFlashSpeed;
    currentLedOn++;
    if(currentLedOn > 2) {
      currentLedOn = 0;
    }
    //Crude scanning - nightrider with limited leds
    switch(currentLedOn) {
      case 0:
        digitalWrite(shutLed,LOW);
        digitalWrite(halfLed,HIGH);
        digitalWrite(openLed,LOW);
        break;
      case 1:
        digitalWrite(shutLed,LOW);
        digitalWrite(halfLed,LOW);
        digitalWrite(openLed,HIGH);
        break;
      case 2:
        digitalWrite(shutLed,HIGH);
        digitalWrite(halfLed,LOW);
        digitalWrite(openLed,LOW);
        break;
    }
  }
}
void flashLeds() {
  static bool ledsOn = false;
  static unsigned long nextTime = 0;
  unsigned long milliSeconds = millis();
  if(milliSeconds >= nextTime) {
    nextTime = milliSeconds + settingsFlashSpeed;
    ledsOn = !ledsOn;
    for(byte i = shutLed; i <= openLed; i ++) {
      digitalWrite(i,ledsOn);
    }
  }
}
