# Arduino Irrigation Controller User Guide
## Links
- [Home Page](../README.md)
- [Technical Notes and rambling](Documentation/TechnicalNotes)
## Error Codes
Here is a list of possible error codes that will be displayed on the main controller's lcd display. All error messages start with the up time at which they were encountered followed by the details.

### `hh:mm Bays x,y,z Disconnected`
This is the most common error and is displayed when the controller does not recieve an acknowledgement from the bay outlet. If all bay outlets are effected, `All` will be displayed instead of outlet numbers. Check that everything is connected properly and turned on.

### `hh:mm `
Blah blah blah

### LCD Layout
The `*` (Astrixes) are used as buttons and can be clicked on by using the left and right buttons to navigate the cursor (black line under a character) and the select button to click it.
#### Main Screen
```
                     \/ A is an option to control all bays at once.
Bay Number -> 1234567A   hh:mm
Bay Status -> ???????*   *More
```
#### More Options Screen
```
*Home *Reset All
*Last Error Msg
```
#### Error Message Screen
```
*Home ERRORhh:mm
<Error message here. It can be scrolled as well!>
```