//Find the number of bays entered
//This macro was found here: http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/DDJ/2007/0704/070301ij02/070301ij02.html
// public interface
#define COUNTOF(x)  (                                           \
    0 * sizeof( reinterpret_cast<const ::Bad_arg_to_COUNTOF*>(x) ) +  \
    0 * sizeof( ::Bad_arg_to_COUNTOF::check_type((x), &(x))      ) +  \
    sizeof(x) / sizeof((x)[0])  )
// implementation details
class Bad_arg_to_COUNTOF
{
  public:
    class Is_pointer;  // intentionally incomplete type
    class Is_array {};
    template<typename T>
    static Is_pointer check_type(const T*, const T* const*);
    static Is_array check_type(const void*, const void*);
};
#define numberOfBays COUNTOF(bays) //Number of bays that are connected - worked out from number of elements in the array of bays.

//Error codes (not used yet)
#define invalidAddress 50
#define packetError 51
#define receiverError 52
#define invalidCommand 53

//Serial commands
#define reportStatus 's'
#define overwriteStatus 'w'
#define reset 'x'
#define setStatus 'r'
#define masterId 255 //The address of the main controller in case the bay outlet needs to initiate contact.

//Macro to make it easier to use LEDS
#define LED_ON(BAY,LED) digitalWrite(BAY->outputPins[LED],HIGH)
#define LED_OFF(BAY,LED) digitalWrite(BAY->outputPins[LED],LOW)
