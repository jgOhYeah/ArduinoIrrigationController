void getBayStatus() {
  
}
void sendBayCommand(byte bay,byte command) {
  Serial.println("Sending a command to bay " + String(bay) + ". The command is " + String(command) + ".");
}
