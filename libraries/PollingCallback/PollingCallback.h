/*
PollingCallback - A library for calling functions in a while
*/
#ifndef PollingCallback_h
#define PollingCallback_h
#include "Arduino.h"
#include <PollingCallbackDefines.h>
class PollingCallback {
public:
	byte add(unsigned long duration,bool repeat, void (*returnFunction)());
	void check();
	void cancel(byte index);
	callback callbacks[MAX_CALLBACKS]; //The buffer to hold the callbacks in - see PollingCallbacksDefines.h for its structure
private:
	//Nothing here YET
};


#endif