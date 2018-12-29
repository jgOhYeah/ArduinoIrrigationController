/*
PollingCallback - A library for calling functions in a while and every so often
The most of it is in the header file so #defines can be done in the sketch and this read them. This does not work if the code is in its own cpp file.
*/
#ifndef PollingCallback_h
#define PollingCallback_h
#include "Arduino.h"
#include <PollingCallbackDefines.h>
class PollingCallback {
public:
	//Register a callback into an empty callback slot. If the buffer is full, this will return callbackBufferFull (255), otherwise the index of the callback in the buffer
	byte add(unsigned long duration, bool repeat, void(*returnFunction)()) {
		//Do a linear search to find a non assigned callback, otherwise return a callback overload error code
		//Deliver the goods into the empty callback in the array
		byte index;
		for (index = 0; index < MAX_CALLBACKS; index++) {
			if (callbacks[index].active == false) {
				break; //Exit the for loop - we found an empty callback spot.
			}
		}
#ifdef DEBUG_CALLBACKS
		Serial.print(F("DEBUG_CALLBACKS ("));
		Serial.print(millis());
		Serial.print(F("):\tAbout to add a new callback. Currently "));
		Serial.print(countActive());
		Serial.print(F(" of the available "));
		Serial.print(MAX_CALLBACKS);
		Serial.println(F(" are active."));

		Serial.print(F("\tDuration: "));
		Serial.println(duration);
		Serial.print(F("\tRepeat: "));
		Serial.println(repeat);
		Serial.print(F("\tIndex: "));
		Serial.println(index);
#endif
		if (index == MAX_CALLBACKS) { //We didn't find an empty slot for the callback.
#ifdef DEBUG_CALLBACKS
			Serial.println(F("\tCALLBACK COULD NOT BE ADDED! Increase MAX_CALLBACKS"));
			Serial.println();
#endif
			return ERR_CALLBACK_BUFFER_FULL;
		}
#ifdef DEBUG_CALLBACKS
		Serial.println();
#endif
		//Set up the callback
		callbacks[index].startTime = millis();
		callbacks[index].duration = duration;
		callbacks[index].returnFunction = returnFunction;
		callbacks[index].active = true;
		callbacks[index].repeat = repeat;
		return index; //Send back the index of the callback function 'cause why not.
	}

	//Call this function often - this will check if any callbacks need to be executed and run them.
	void check() {
		unsigned long currentTime = millis();
		//Loop over all the current callbacks, check if they are active, check if the time is right, then execute.
		for (byte index = 0; index < MAX_CALLBACKS; index++) {
			if (callbacks[index].active == true) { //Simple operation first
				if (currentTime - callbacks[index].startTime >= callbacks[index].duration) { //More complex 32 bit. Check the actual time.
																							 //The callback will be executed.
					if (callbacks[index].repeat) { //If the callback should be repeated, enter the next time, else stop it.
						callbacks[index].startTime = currentTime; //Set the next time to callback.
					}
					else {
						callbacks[index].active = false;
					}
#ifdef DEBUG_CALLBACKS
					Serial.print(F("DEBUG_CALLBACKS ("));
					Serial.print(millis());
					Serial.print(F("):\tAbout to call a callback. Currently "));
					Serial.print(countActive());
					Serial.print(F(" of the available "));
					Serial.print(MAX_CALLBACKS);
					Serial.println(F(" are active."));

					Serial.print(F("\tRepeat: "));
					Serial.println(callbacks[index].repeat);
					Serial.print(F("\tIndex: "));
					Serial.println(index);
					Serial.println();
#endif
					//Call the function
					callbacks[index].returnFunction();
				}
			}
		}
	}

	//Disble a callback - usefull for stopping repeated callbacks callbacks
	//If the index is not valid (>= MAX_CALLBACKS) then nothing will be done.
	void cancel(byte index) {
		if (index < MAX_CALLBACKS) {
#ifdef DEBUG_CALLBACKS
			Serial.print(F("DEBUG_CALLBACKS ("));
			Serial.print(millis());
			Serial.print(F("):\tAbout to disable a callback. Currently "));
			Serial.print(countActive());
			Serial.print(F(" of the available "));
			Serial.print(MAX_CALLBACKS);
			Serial.println(F(" are active."));

			Serial.print(F("\tIndex: "));
			Serial.println(index);
			Serial.println();
#endif
			callbacks[index].active = false;
		}
#ifdef DEBUG_CALLBACKS
		else {
			Serial.print(F("DEBUG_CALLBACKS ("));
			Serial.print(millis());
			Serial.println(F("):\tHave been told to disable a callback that does not exist. Ignoring."));
		}
#endif
	}

	byte countActive() {
		byte activeCount = 0;
		for (byte index = 0; index < MAX_CALLBACKS; index++) {
			if (callbacks[index].active == true) {
				activeCount++;
			}
		}
		return activeCount;
	}
	callback callbacks[MAX_CALLBACKS]; //The buffer to hold the callbacks in - see PollingCallbacksDefines.h for its structure
private:
	//Nothing here YET
};


#endif