#include <EEPROM.h>
#include <ButtonOnPress.h>
#include <LongDistanceSerial.h>
#include <avr/wdt.h>

#include "defines.h"

LongDistanceSerial serialCom(&Serial,1200,serialEnablePin,startChar,endChar,0);
ButtonOnPress button(buttonPin,minOffTime,buttonPressedState,pullupsEnabled);
void setup() {
  serialCom.begin();
  pinMode(LED_BUILTIN,OUTPUT);
  for(byte i = shutLed; i <= openLed; i ++) {
    pinMode(i,OUTPUT);
  }
  pinMode(motorA,OUTPUT);
  pinMode(motorB,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);

  button.begin();
  //If button was pressed while starting up, enter programming mode
  if(button.isPressed()) {
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
    switch(commandOne) {
      case reportStatus:
        sendReply(bayStatus,serialWrite);
      break;
    case reset:
      wdt_enable(WDTO_15MS);
      while(true);
      break;
    case bayShut:
    case bayHalf:
    case bayOpen:
      sendReply(commandOne,serialRead);
      motorControl(commandOne);
      break;
    }
  }
  if(button.checkButton()) {
    byte bayStatusLocal = bayStatus + 1;
    if(bayStatusLocal > bayOpen) {
      bayStatusLocal = bayShut;
    }
    sendReply(bayStatusLocal,serialWrite);
    motorControl(bayStatusLocal);
  }
  /*if(bayStatus == bayHalf && millis() > timeToStop && motorRunning) {
    haltMotor();
    Serial.println(F("Motor stopped"));
  }*/
  if(bayStatus == bayUnknown) {
    scrollLeds();
  }
}
