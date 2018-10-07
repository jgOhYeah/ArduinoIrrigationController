void openBayMotor() {
  digitalWrite(motorB,LOW);
  digitalWrite(motorA,HIGH);
}
void closeBayMotor() {
  digitalWrite(motorB,HIGH);
  digitalWrite(motorA,LOW);
}
void stopBayMotor() {
  digitalWrite(motorB,LOW);
  digitalWrite(motorA,LOW);
}
void setupOutputs() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(upLed, OUTPUT);
  pinMode(halfLed, OUTPUT);
  pinMode(downLed, OUTPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off
}

