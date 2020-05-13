void loadSettings() {
  /*for(byte i = 0; i < 4; i++) { //Set the bus id
    busId[i] = EEPROM.read(EEPROM_BUS_ID+i);
  }*/
  myId = EEPROM.read(EEPROM_ADDRESS);
  halfwayPos = EEPROM.read(EEPROM_HALFWAY_POS);
  downTravelSpeed = readULong(EEPROM_DOWN_TRAVEL_SPEED);
  upTravelSpeed = readULong(EEPROM_UP_TRAVEL_SPEED);
  calculateSettings();
}
void saveSettings() { //Using update so do not write to settings that have not changed
  /*for(byte i = 0; i < 4; i++) { //Set the bus id
    EEPROM.update(EEPROM_BUS_ID+i,busId[i]);
  }*/
  EEPROM.update(EEPROM_ADDRESS,myId);
  EEPROM.update(EEPROM_HALFWAY_POS,halfwayPos);
  writeULong(EEPROM_DOWN_TRAVEL_SPEED,downTravelSpeed);
  writeULong(EEPROM_UP_TRAVEL_SPEED,upTravelSpeed);
  calculateSettings();
}
void calculateSettings() {
  bus.set_id(myId); //Set the id in case it has been changed.
  //#error is it actually possible to set the bus id at runtime?
  halfUpTime = halfwayPos * (upTravelSpeed / 100);
  halfDownTime = (100-halfwayPos) * (downTravelSpeed / 100);
}
void checkFirstStart() { //Checks if this is the first time the arduino has started up and burn the default eeprom settings if it is in order to make the arduino communicatable with without fuss.
  if(EEPROM.read(EEPROM_FIRST_START_UP) != RESET_SETTINGS_EEPROM) { //This is the initial startup.
    EEPROM.update(EEPROM_FIRST_START_UP,RESET_SETTINGS_EEPROM); //Write the new value
    setDefaults();
    digitalWrite(LED_BUILTIN,HIGH); //Flash the leds to say this is the first start
    delay(5000);
    digitalWrite(LED_BUILTIN,LOW);
    delay(300);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN,LOW);
  }
}
void setDefaults() {
  myId = DEFAULT_ADDRESS;
  halfwayPos = DEFAULT_HALF_POS;
  upTravelSpeed = DEFAULT_UP_TIME;
  downTravelSpeed = DEFAULT_DOWN_TIME;
  writeULong(EEPROM_SERIAL_BAUD,DEFAULT_BAUD_RATE); //Save the baud rate
  setBaudRate(); //Set the baud rate back to the default
  saveSettings();
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
  DEBUG_EEPROM(F("Reading from EEPROM at address "));
  DEBUG_EEPROM_LN(address);
  unsigned long number = 0;
  for(char i = 3; i >= 0; i--) {
    number = number << 8; //0 << x = 0 all the time (hopefully)
    DEBUG_EEPROM(F("Iteration "));
    DEBUG_EEPROM(byte(i));
    DEBUG_EEPROM(F(". value is "));
    DEBUG_EEPROM_LN((number,2));
    number += EEPROM.read(address+i);
  }
  DEBUG_EEPROM(F("Number is "));
  DEBUG_EEPROM_LN(number);
  return number;
}
void writeULong(unsigned int address, unsigned long value) {
  DEBUG_EEPROM(F("Writing to EEPROM. The number is "));
  DEBUG_EEPROM(value);
  DEBUG_EEPROM(F(" which is: "));
  DEBUG_EEPROM((value,2));
  DEBUG_EEPROM(F(" at address "));
  DEBUG_EEPROM_LN(address);
  for(byte i = 0; i < 4; i++) {
    DEBUG_EEPROM(F("Iteration "));
    DEBUG_EEPROM(i);
    DEBUG_EEPROM(F(". Value is "));
    DEBUG_EEPROM_LN((value & 255,2));
    EEPROM.update(address+i,value & 255);
    value = value >> 8; //We don't care that value will be 0
  }
}
