# Arduino Irrigation Controller
An irrigation controller for a small farm that remotely controls a set of linear actuators that raise and lower flaps over pipes.
## This is still a WORK IN PROGRESS!
I am mainly using this repository to transfer files! If you are interested in using this, have a look at `SlaveUnit/SlaveUnit3.5` and `Controller/20180923_Controller3.0_PJON`. **There will be bugs!**
## Links
- [User Guide](Documentation/UserGuide.md)
- [Technical Notes and rambling](Documentation/TechnicalNotes)
# Notes to self
Sending data like
```cpp
char charsToSend[2];
charsToSend[0] = setStatus;
charsToSend[1] = bayStatus[cursorPos]; //Prepare the data
bus.send(cursorPos+firstSlaveAddress,charsToSend,2); //Send the data
```
Always results in 160 being the first char for some reason.
## Serial Moniter:
```
At startup:

Byte being sent in array: ⸮	 which is: s
About to send a msg to all devices. It is: 
⸮	At length: 1
--------------Stuff------------
Error: 101	Data: 0
and so on.

Also when trying to change state:

About to send a msg to all devices. It is: 
⸮	At length: 2
```
