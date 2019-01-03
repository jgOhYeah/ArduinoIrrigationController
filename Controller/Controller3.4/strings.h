//Store the error messages
char completeErrorMsg[41] = "Nothing yet"; //Assuming the lcd can store 40 chars in its ram when it comes to printing
char charBuffer[30]; //Some parts of the program do not like this as a local variable for some reason, so declaring it as global and lots of bits sharing
char charBuffer2[30]; //Some parts of the program do not like this as a local variable for some reason, so declaring it as global and lots of bits sharing

const byte bayOptions[] = {'?','S','H','O','X'};
//Strings
const char colon[] PROGMEM = ":"; //Want to be a string to use with strcpy
const char space[] PROGMEM = " ";
const char all[] PROGMEM = "ALL,";
const char zero[] PROGMEM = "0";
const char comma[] PROGMEM = ",";
const char bay[] PROGMEM = "Bay";
const char sSpace[] PROGMEM = "s ";
const char irrigation[] PROGMEM = "Irrigation ";
const char controller[] PROGMEM = "Controller";
const char initialising[] PROGMEM = "Initialising...";
const char stringWaiting[] PROGMEM = "Waiting...";
const char crnl[] PROGMEM = "\r\n";

//Lcd Strings
const char leftButtonSymbol[] PROGMEM = "< ";
const char rightButtonSymbol[] PROGMEM = " >";
const char stringBack[] PROGMEM = "*Back ";
const char stringMore[] PROGMEM = "*More";
const char stringResetAll[] PROGMEM = "*Reset all";
const char stringLastErrMsg[] PROGMEM = "*Err msg";
const char stringSettings[] PROGMEM = " *Setup";
const char stringSelectBay[] PROGMEM = "SELECT";
const char stringBay[] PROGMEM = " BAY";
const char stringToEdit[]PROGMEM = "TO ";
const char stringEdit[] PROGMEM = "EDIT";
const char stringError[] PROGMEM = "ERROR ";
const char stringUp[] PROGMEM = "Up";
const char stringDown[] PROGMEM = "Down";
const char stringTime[] PROGMEM = " Time";
const char stringHalfPos[] PROGMEM = "Half pos";
const char stringBaud[] PROGMEM = "Baud";
const char stringRate[] PROGMEM = " Rate";

//Error Strings
const char stringPJON[] PROGMEM = "PJON";
const char stringErrorCode[] PROGMEM = " Err, Code: ";
const char noReply[] PROGMEM = " Disconnected";
const char stringUnkown[] PROGMEM = "Unkown";
const char stringPacket[] PROGMEM = "Packet";
const char stringData[] PROGMEM = " Data: ";

