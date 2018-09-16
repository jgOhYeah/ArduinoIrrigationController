/*
ButtonOnPress - A library for debouncing buttons
*/
#ifndef ButtonOnPress_h
#define ButtonOnPress_h
#include "Arduino.h"
class ButtonOnPress {
public:
	//onState is either HIGH or LOW
	ButtonOnPress(byte pin, unsigned int minOffTime, byte onState, bool internalPullup);
	void begin();
	bool checkButton();
	bool isPressed();
private:
	byte pin_;
	unsigned int minOffTime_;
	byte onState_;
	bool internalPullup_;
	//Moved from being static variables in CheckButton in sButtonOnPress.cpp to work with multiple instances
	unsigned long nextOffTime = 0;
	bool currentlyPressed = false;
};


#endif