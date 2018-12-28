/*
 * TODO: Flash leds - CURRENTLY IN THE MIDDLE OF IMPLEMENTING - ERRORS
 *    - When button is pressed to reset the baud rate in settings - lights sometime do not change / get stuck in the wrong pattern.
 * TODO: Set default baud to be 38400 instead of 9600
 * TODO: Debug mode?
 * TODO: Optimise strings so that they are stored in separate arrays and shared to save memory?
 * 
 * TODO: Have a look at the stop leds flashing function - leds sometimes turn off all the time afterwards.
 *  - Possibly not setting the last 3 bits properly? - Look at debugging when stop leds flashing is called. - Possibly something else when the motor swaps direction??????
 *  
 *  Check out how many callbacks are actually needed - is there a duplicate callback when it says 4 are active when in half?
 * 
 * 
 * 
 * 
 */
