/*
PollingCallback - A library for calling functions in a while
This file contatins the structs and various default defined values. The values can be overwritten if need be
*/
#ifndef MAX_CALLBACKS //The maximum number of active callbacks at the same time (default 3).
	#define MAX_CALLBACKS 3
#endif

#ifndef ERR_CALLBACK_BUFFER_FULL //The error code (byte) for having run out of space in the buffer for new callbacks (default 255).
	#define ERR_CALLBACK_BUFFER_FULL 255
#endif

struct callback {
  unsigned long startTime;
  unsigned long duration;
  void (*returnFunction)();
  bool active; //True when yet to be called, false after called
  bool repeat; //True when repeating, false when single shot.
};