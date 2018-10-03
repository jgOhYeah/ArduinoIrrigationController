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
