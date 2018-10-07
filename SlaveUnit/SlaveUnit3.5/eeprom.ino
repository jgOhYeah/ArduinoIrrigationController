void loadSettings() {
  myId = EEPROM.read(addressAddress);
  halfwayPos = EEPROM.read(halfwayPosAddress);
  downTravelSpeed = readUInt(downTravelSpeedAddress);
  upTravelSpeed = readUInt(upTravelSpeedAddress);
  calculateSettings();
}
//TODO: SaveSettings does not work!
void saveSettings() { //Using update so do not write to settings that have not changed
  EEPROM.update(addressAddress,myId);
  EEPROM.update(halfwayPosAddress,halfwayPos);
  writeUInt(downTravelSpeedAddress,downTravelSpeed);
  writeUInt(upTravelSpeedAddress,upTravelSpeed);
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

