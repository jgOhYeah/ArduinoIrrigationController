/*
 * Things to think about
 * Respose when changed rapidly S to H - reply for each causes difference warning
 * 
 * Serial Commands
 * 126, 0, 0, 10
 * ~  , ?, ?, line feed
 * ~ (126), Bay #, Command, Line Feed (10)
 * 
 * Addressing
 * Zero indexed
 * 0 - 6 in aseven bay setup
 * 123 is all bays
 * 
 * Commands
 * 0 is unknown - cannot set
 * 1 is shut
 * 2 is half
 * 3 is open
 * 4 is disconnected - cannot set
 * 
 * 50 is reset
 * 25 is report status
 * 
 * All commands except for reset expect a reply in the same form, except with the bay state instead of a command (often it is the same).
 */
