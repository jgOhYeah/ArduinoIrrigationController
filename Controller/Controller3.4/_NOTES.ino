/*
 * TODO:
 * EEPROM SETTINGS: - error screen does not go back to the right place in eeprom screen 2
 * half pos number entry. - Bug that it can also be set out of range of the slave - also leave in on purpose?
 * Error messages when trying to send the half pos from the slave
 * Possibly remove set serial baud setting or add confirmation - too dangerous?					   
 dEBUGGING
 
 built in eeprom settings.

 length check function for serial recieve - like slave unit

 back button on error screen does not display properly

 startup only proceed once all bays have responded instead of a fixed time.

 serial interface for connecting to a computer.

POSSIBLE DOWN THE TRACK:
 Water sensor in channel to open next bay if channel is overflowing

 LoRaWAN inputs from tractor and sensors in paddock to shut bays

 Started to adapt to 40 by 2 lcd, but not finished - cursor on main screen - pos 11 needs to be replaced with LCD_WIDTH-5
 The settings menu select bay is on the wron line. THE CLOCK does not update automatically - callbackLibrary!!!


 
 */
