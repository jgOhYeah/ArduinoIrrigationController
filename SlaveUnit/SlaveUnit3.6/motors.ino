void openBayMotor() {
  DEBUG_MOTORS_LN(F("Motor opening"));
  digitalWrite(PIN_MOTOR_B,LOW);
  digitalWrite(PIN_MOTOR_A,HIGH);
}
void closeBayMotor() {
  DEBUG_MOTORS_LN(F("Motor closing"));
  digitalWrite(PIN_MOTOR_B,HIGH);
  digitalWrite(PIN_MOTOR_A,LOW);
}
void stopBayMotor() {
  DEBUG_MOTORS_LN(F("Motor stopping"));
  digitalWrite(PIN_MOTOR_B,LOW);
  digitalWrite(PIN_MOTOR_A,LOW);
  bayMovedCallback = NONEXISTANT_CALLBACK;
  stopLedsFlashing(); //The motor is not moving now, so fix the led
}
void setupOutputs() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_UP_LED, OUTPUT);
  pinMode(PIN_HALF_LED, OUTPUT);
  pinMode(PIN_DOWN_LED, OUTPUT);
  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off
}
void openToHalf() {
  DEBUG_MOTORS_LN(F("Motor opening to half"));
  openBayMotor();
  bayMovedCallback = callback.add(halfUpTime,false,stopBayMotor); //Stop the motor at the right time
}
