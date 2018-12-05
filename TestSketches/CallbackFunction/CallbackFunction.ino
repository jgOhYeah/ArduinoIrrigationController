//Have a look at this?: https://arduino.stackexchange.com/questions/23063/user-callback-functions
//Put structs into an an array and loop over them from time to time to check if any needs calling.
//Use link above to find out how to do varying params.
struct structCallback (
  unsigned long startTime;
  unsigned long duration;
  void (*returnFunction)(unsigned long);
};
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
