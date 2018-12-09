void openBayMotor() {
  digitalWrite(PIN_MOTOR_B,LOW);
  digitalWrite(PIN_MOTOR_A,HIGH);
}
void closeBayMotor() {
  digitalWrite(PIN_MOTOR_B,HIGH);
  digitalWrite(PIN_MOTOR_A,LOW);
}
void stopBayMotor() {
  digitalWrite(PIN_MOTOR_B,LOW);
  digitalWrite(PIN_MOTOR_A,LOW);
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
  openBayMotor();
  callback.add(halfUpTime,false,stopBayMotor); //Stop the motor at the right time
}
