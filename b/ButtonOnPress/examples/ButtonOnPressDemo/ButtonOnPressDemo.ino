#include <ButtonOnPress.h>

//Set up the button
#define buttonPin 6 //Pin the button is attached to
#define minOffTime 300 //Minimum amount of time between presses
#define buttonPressedState HIGH //Which way the pin is pulled when it is pressed
#define pullupsEnabled false //Enable the internal pullup resistors
ButtonOnPress button(buttonPin,minOffTime,buttonPressedState,pullupsEnabled);

//Toggle the internal led light each time the button is pressed
bool ledState = true;

//Count the number of times the button has been pressed
unsigned int pressCount = 0;

void setup() {
  //Set up the button pin
  button.begin();
  
  //Start the serial port for debugging purposes
  Serial.begin(38400);
  Serial.println(F("ButtonOnPress library demo"));

  //Set up the led
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
}

void loop() {
  //Check if the button has just been pressed
  if(button.checkButton()) {
    //Toggle the led
    ledState = !ledState;
    digitalWrite(LED_BUILTIN,ledState);
    
    //Send the number of times the button has been pressed to the serial port.
    pressCount++;
    Serial.print(F("The button has been pressed "));
    Serial.print(pressCount);
    Serial.println(F(" time(s)."));
  }
}
