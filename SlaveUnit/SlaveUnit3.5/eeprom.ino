void loadSettings() {
  myId = EEPROM.read(addressAddress);
  halfwayPos = EEPROM.read(halfwayPosAddress);
  downTravelSpeed = readULong(downTravelSpeedAddress);
  upTravelSpeed = readULong(upTravelSpeedAddress);
  calculateSettings();
}
//TODO: SaveSettings does not work!
void saveSettings() { //Using update so do not write to settings that have not changed
  EEPROM.update(addressAddress,myId);
  EEPROM.update(halfwayPosAddress,halfwayPos);
  writeULong(downTravelSpeedAddress,downTravelSpeed);
  writeULong(upTravelSpeedAddress,upTravelSpeed);
  calculateSettings();
}
void calculateSettings() {
  bus.set_id(myId); //Set the id in case it has been changed.
  halfUpTime = halfwayPos * (upTravelSpeed / 100);
  halfDownTime = (100-halfwayPos) * (downTravelSpeed / 100);
}
unsigned int readUInt(unsigned int address) {
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1); //Build up from two bytes
}
void writeUInt(unsigned int address, unsigned int value) {
  EEPROM.update(address,(value >> 8)&255); //Top half of integer
  EEPROM.update(address+1,value & 255); //Bottom half of integer
}
//#error Something wrong here!
unsigned long readULong(unsigned int address) {
#if defined(serialDebug) && defined(eepromDebug)
  Serial.print(F("Reading from EEPROM at address "));
  Serial.println(address);
#endif
  unsigned long number = 0;
  for(char i = 3; i >= 0; i--) {
    number = number << 8; //0 << x = 0 all the time (hopefully)
#if defined(serialDebug) && defined(eepromDebug)    
    Serial.print(F("Iteration "));
    Serial.print(byte(i));
    Serial.print(F(". value is "));
    Serial.println(number,2);
#endif
    number += EEPROM.read(address+i);
  }
#if defined(serialDebug) && defined(eepromDebug)  
  Serial.print(F("Number is: "));
  Serial.println(number);
#endif  
  return number;
}
void writeULong(unsigned int address, unsigned long value) {
#if defined(serialDebug) && defined(eepromDebug)
  Serial.print(F("Writing to EEPROM. The number is "));
  Serial.print(value);
  Serial.print(F(" which is: "));
  Serial.print(value,2);
  Serial.print(F(" at address "));
  Serial.println(address);
#endif
  for(byte i = 0; i < 4; i++) {
#if defined(serialDebug) && defined(eepromDebug)
    Serial.print(F("Iteration "));
    Serial.print(i);
    Serial.print(F(". Value is "));
    Serial.println(value & 255,2);
#endif
    EEPROM.update(address+i,value & 255);
    value = value >> 8; //We don't care that value will be 0
  }
}
