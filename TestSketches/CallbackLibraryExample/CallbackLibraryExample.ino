//These #defines change the default values of the callback library - these need to be BEFORE the #include <PollingCallback.h>
#define MAX_CALLBACKS 3 //The maximum number of active callbacks there can be at a time (default 3).
//#define CALLBACK_BUFFER_FULL 255 //Change the error code if needed 
// - theis should be >= to MAX_CALLBACKS so the code adding the callback can tell between the index of the callback and the callback buffer being full.

#include <PollingCallback.h> //Include the library
PollingCallback callback; //Make a callback object to manage the callbacks.

byte ledFlashIndex;
void setup() {
  Serial.begin(38400);
  Serial.println(F("Hello, this is an example of PollingCallback library."));
  Serial.println(F("If this is working properly, the inbuilt LED will flash once every second repeatedly and the arduino will send a once off message to the serial monitor in around 5 seconds."));
  ledFlashIndex = callback.add(1000,true,flashLED); //Set up a callback to repeatedly call the flashLED function every second. We do need to know the index of this if we want to stop it later.
  if(callback.add(5000,false,sendSerial) == CALLBACK_BUFFER_FULL || ledFlashIndex == CALLBACK_BUFFER_FULL) { //Set up a callback to call the sendSerial function once in 5000 ms. Check for the CALLBACK_BUFFER_FULL error message to tell the user if either callback could not be registered.
    //We don't really care about the callback index, so just use this directly inside an if statement.
    Serial.print(F("Cannot register a new callback as the callback buffer is full of already active callbacks. Maximum number of callbacks: "));
    Serial.println(MAX_CALLBACKS);
  }
}

void loop() {
  callback.check(); //Check the callbacks and call the functions if the time is right
  if(Serial.available() && ledFlashIndex != CALLBACK_BUFFER_FULL) { //If we have recieved a message and the creation of the ledFlash callback was successful, stop flashing the led for 10 secinds and then start again.
    callback.callbacks[ledFlashIndex].duration = 10000; //Set call the flash led function in 10 seconds - will be set back to 1 second when next called.
    if(callback.add(5000,false,sendSerial) == CALLBACK_BUFFER_FULL) {
      //We don't really care about the callback index, so just use this directly inside an if statement.
      Serial.print(F("Cannot register a new callback as the callback buffer is full of already active callbacks. Maximum number of callbacks: "));
      Serial.println(MAX_CALLBACKS);
    }
    Serial.println(F("You just sent me a message. The LED will stop flashing for 10 seconds. A message will also be sent to the serial port in 5 seconds"));
    while(Serial.available()) { //Clear the serial buffer to avoid repeats.
      Serial.read();
    }
  }
  
}
void flashLED() {
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  Serial.print(F("Flashing LED at "));
  Serial.print(millis());
  Serial.println(F("ms."));
  if(callback.callbacks[ledFlashIndex].duration != 1000) { //Reset the callback time to be 1000 ms if it has been changed
    callback.callbacks[ledFlashIndex].duration = 1000;
  }
}
void sendSerial() {
  Serial.print(F("Single callback after "));
  Serial.print(millis());
  Serial.println(F("ms."));
}


