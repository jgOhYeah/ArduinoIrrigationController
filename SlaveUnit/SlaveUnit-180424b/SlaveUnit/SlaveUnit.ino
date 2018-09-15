#include <EEPROM.h>
#include <LongDistanceSerial.h>
#include <buttons.h>
#include "defines.h"

LongDistanceSerial serialCom(&Serial,1200,serialEnablePin,startChar,endChar,0);
Button builtinButton;

void setup() {
  serialCom.begin();
  pinMode(LED_BUILTIN,OUTPUT);
  for(byte i = shutLed; i <= openLed; i ++) {
    pinMode(i,OUTPUT);
  }
  pinMode(motorA,OUTPUT);
  pinMode(motorB,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);

  builtinButton.assign(buttonPin);
  builtinButton.setMode(buttonMode);
  builtinButton.setTimer(buttonTimer);
  builtinButton.setRefresh(buttonRefresh);
  //If button was pressed while starting up, enter programming mode
  if(builtinButton.check() == ON) {
    setEepromSettings();
  }
  bayOpenTime = EEPROM.read(bayOpenTimeAddress);
  bayCloseTime = EEPROM.read(bayCloseTimeAddress);
  bayHalfTimeFraction = EEPROM.read(bayHalfTimeFractionAddress);
  deviceAddress = EEPROM.read(deviceIdAddress);
  //Make a random seed based on the device address so differs from others hopefully
  randomSeed(deviceAddress);
}
void loop() {
  byte address, commandOne, commandTwo; 
  byte recieveStatus = serialCom.recieveCommand(address, commandOne, commandTwo,serialRecieveTimeout);
  //Recieve any commands with either the individual device address or all of them. Just ignore errors for now
  if(recieveStatus == serialSuccess && (address == deviceAddress || address == allDevices)) {
    Serial.println(bayStatus);
    //motorControl(commandOne);
  }
  if(bayStatus == bayUnknown) {
    scrollLeds();
  }
  if(builtinButton.check() == ON) {
    bayStatus++;
    if(bayStatus > bayOpen) {
      bayStatus = bayShut;
    }
    Serial.println(bayStatus);
  }
  //Stop the motor if a time was set to turn off
  /*if(millis() >= shutoffTime && shutoffTime != 0) {
    haltMotor();
    shutoffTime = 0;
  }*/
}
void sendReply(byte commandOne, byte commandTwo) {
  delay(random(maxDelayTime));
  serialCom.sendCommand(deviceAddress,commandOne,commandTwo,enableLineWait);
}
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
/*void motorControl(byte state){
  switch(state) {
    case reportStatus:
      sendReply(bayStatus,serialWrite);
      break;
    case reset:
      //Reset when find the code
      while(true);
      break;
    case bayShut:
      bayStatus = bayShut;
      sendReply(bayStatus,serialRead);
      closeMotorDir();
      digitalWrite(shutLed,HIGH);
      digitalWrite(halfLed,LOW);
      digitalWrite(openLed,LOW);
      break;
    case bayHalf:
      bayStatus = bayHalf;
      sendReply(bayStatus,serialRead);
      digitalWrite(shutLed,LOW);
      digitalWrite(halfLed,HIGH);
      digitalWrite(openLed,LOW);
      break;
    case bayOpen:
      bayStatus = bayOpen;
      sendReply(bayStatus,serialRead);
      digitalWrite(shutLed,LOW);
      digitalWrite(halfLed,LOW);
      digitalWrite(openLed,HIGH);
      break;
  }
}
*/
