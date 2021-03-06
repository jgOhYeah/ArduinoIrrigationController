//Call this function often - this will check if any callbacks need to be executed and run them.
void checkCallbacks() {
  unsigned long currentTime = millis();
  //Loop over all the current callbacks, check if they are active, check if the time is right, then execute.
  for(byte index = 0; index < maxCallbacks; index++) {
    if(callbacks[index].active == true) { //Simple operation first
      if(currentTime - callbacks[index].startTime >= callbacks[index].duration) { //More complex 32 bit. Check the actual time.
        //The callback will be executed.
        if(callbacks[index].repeat) { //If the callback should be repeated, enter the next time, else stop it.
          callbacks[index].startTime = currentTime; //Set the next time to callback.
        } else {
          callbacks[index].active = false;
        }
        //Call the function
        callbacks[index].returnFunction();
      }
    }
  }
}
//Disble a callback - usefull for looped callbacks
void cancelCallback(byte index) {
  callbacks[index].active = false;
}
//Register a callback into an empty callback slot. If the buffer is full, this will return callbackBufferFull (255), otherwise the index of the callback in the buffer
byte registerCallback(unsigned long duration,bool repeat, void (*returnFunction)) {
  //Do a linear search to find a non assigned callback, otherwise return a callback overload error code
  //Deliver the goods into the empty callback in the array
  byte index;
  for(index = 0; index < maxCallbacks; index++) {
    if(callbacks[index].active == false) {
      break; //Exit the for loop - we found an empty callback spot.
    }
  }
  if(index == maxCallbacks) { //We didn't find an empty slot for the callback.
    return callbackBufferFull;
  }
  //Set up the callback
  callbacks[index].startTime = millis();
  callbacks[index].duration = duration;
  callbacks[index].returnFunction = returnFunction;
  callbacks[index].active = true;
  callbacks[index].repeat = repeat;
  return index; //Send back the index of the callback function 'cause why not.
}
