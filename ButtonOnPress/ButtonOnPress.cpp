#include "Arduino.h"
#include "ButtonOnPress.h"
ButtonOnPress::ButtonOnPress(byte pin, unsigned int minOffTime, byte onState,bool internalPullup) {
	pin_ = pin;
	minOffTime_ = minOffTime;
	onState_ = onState;
	internalPullup_ = internalPullup;
}
//Sets the required pin to be an input with pullup resistors enabled or disbabled.
void ButtonOnPress::begin() {
	if (internalPullup_) {
		pinMode(pin_, INPUT_PULLUP);
	} else {
		pinMode(pin_, INPUT);
	}
}
//Returns true the first time the button has been pressed, false otherwise. Must have a certain time from the start of being pressed to when it is eligable for the next one.
bool ButtonOnPress::checkButton() {
	if (digitalRead(pin_) == onState_) {
		//Only send true reply if this is the first time that the button has been checked and found to be pressed.
		if (!currentlyPressed) {
			currentlyPressed = true;
			nextOffTime = millis() + minOffTime_;
			pressTime = millis();
			return true;
		}
	} else if(millis() > nextOffTime){
		//If the button has not been pressed for at least a certain amount of time and the button is not pressed.
		currentlyPressed = false;
	}
	return false;
}
bool ButtonOnPress::isPressed() {
	if (digitalRead(pin_) == onState_) {
		return true;
	} else {
		return false;
	}
}
//Returns true if the button is currently pressed and has been pressed for at least a ceratin time.
bool ButtonOnPress::longPress(unsigned long timePressed) {
	if (millis() - pressTime >= timePressed && isPressed()) {
		return true;
	} else {
		return false;
	}
}