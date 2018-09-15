
void motorControl(byte state){
  static unsigned long commandStartTime = 0;
  switch(state) {
    case bayShut:
      bayStatus = bayShut;
      closeMotorDir();
      commandStartTime = millis();
      digitalWrite(shutLed,HIGH);
      digitalWrite(halfLed,LOW);
      digitalWrite(openLed,LOW);
      break;
    case bayHalf:
/*//Only halt the motor when the time has come if bayStatus == bayHalf - this will stop it effecting others.
      //Work out previous pos to decide whether to go up or down.
      //Fraction up = something
      //If franction up > half fraction then one way, else other or not at all.
      unsigned long timeRunning = millis() - commandStartTime;
      //268435455 is the maximum that can be stored if going to be multiplied by 16 later to get around integer maths limitations
      timeRunning = min(268435455,timeRunning);
      unsigned long bayHalfOpenFractionBy16 = (bayHalfTimeFraction * 16) / 255;
      switch(bayStatus) {
        case bayShut:
        break;
        case bayOpen:
          //Will not work because integer maths - that is why multiplying both sides by 16 (hopefully easy number to work with)
          //Not yet passed half way
          //timeRunning / bayOpenTime is the fraction open
          unsigned long fractionOpenBy16 = (timeRunning * 16) / (bayOpenTime * 100);
          if(fractionOpenBy16 < bayHalfOpenFractionBy16) {
            //Keep opening
            timeToStop = (difference(fractionOpenBy16,bayHalfOpenFractionBy16) * bayOpenTime) / 16; //fraction still to go * time for travel
            timeToStop += millis();
            Serial.print(F("Time to stop: "));
            Serial.println(timeToStop);
            //Already opening, so no need to change motor dir
          } else {
            //Reverse or stop
            timeToStop = (difference(fractionOpenBy16,bayHalfOpenFractionBy16) * bayCloseTime) / 16; //fraction still to go * time for travel
            closeMotorDir();
            timeToStop += millis();
            Serial.print(F("Time to stop: "));
            Serial.println(timeToStop);
          }
        break;
        default:
        break;
      }*/
      bayStatus = bayHalf;
      digitalWrite(shutLed,LOW);
      digitalWrite(halfLed,HIGH);
      digitalWrite(openLed,LOW);
      break;
    case bayOpen:
      bayStatus = bayOpen;
      openMotorDir();
      commandStartTime = millis();
      digitalWrite(shutLed,LOW);
      digitalWrite(halfLed,LOW);
      digitalWrite(openLed,HIGH);
      break;
  }
}

void openMotorDir() {
  digitalWrite(motorA,HIGH);
  digitalWrite(motorB,LOW);
}
void closeMotorDir() {
  digitalWrite(motorA,LOW);
  digitalWrite(motorB,HIGH);
}
void haltMotor() {
  digitalWrite(motorA,LOW);
  digitalWrite(motorB,LOW);
}
unsigned long difference(unsigned long valueOne, unsigned long valueTwo) {
  if(valueOne > valueTwo) {
    return valueOne - valueTwo;
  } else {
    return valueTwo - valueOne;
  }
}

