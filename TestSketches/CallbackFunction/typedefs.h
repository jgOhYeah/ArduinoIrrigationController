//Have a look at this?: https://arduino.stackexchange.com/questions/23063/user-callback-functions
//Put structs into an an array and loop over them from time to time to check if any needs calling.
//Use link above to find out how to do varying params.
struct callback {
  unsigned long startTime;
  unsigned long duration;
  void (*returnFunction)();
  bool active; //True when yet to be called, false after called
  bool repeat; //True when repeating, false when single shot.
};
byte registerCallback(unsigned long duration,bool repeat, void (*returnFunction));

