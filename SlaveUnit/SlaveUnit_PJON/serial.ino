void sendReply(byte commandOne, byte commandTwo) {
  delay(random(maxDelayTime));
  serialCom.sendCommand(deviceAddress,commandOne,commandTwo,enableLineWait);
}
