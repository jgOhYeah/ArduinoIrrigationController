# Arduino Irrigation Controller
An irrigation controller for a small farm that remotely controls a set of linear actuators that raise and lower flaps over pipes.
## This is still a WORK IN PROGRESS!
I am mainly using this repository to transfer files! If you are interested in using this, have a look at `SlaveUnit/SlaveUnit3.6` and `Controller/3.4`. **There will be bugs!**
## Links
Most of these notes are very out of date and based off old versions - I have not properly updated the documentation for quite a while!
- [User Guide](Documentation/UserGuide.md)
- [Technical Notes and rambling](Documentation/TechnicalNotes)
# Parts
## Master
- Arduino Mega
- 3 buttons (left, right and select)
- Character lcd (currently using a 40 by 2 lcd, also works on a 16 by 2 lcd by selecting macros and `#defines`, could be modified for a 20 by 4).
## Slaves
- Arduino Uno / Nano / Atmega328p driving a H bridge motor driver to power a linear actuator opening and shutting the bay outlets