

#include <Arduino.h>
#include <EventObjects.h>



class UnevenTimeEventObject: public EventBaseObject
{
public:
	int testvalue(void){return 99;};

};





void setupFTM1(voidFunctionWithEventBaseObjectParameter  newEndFunction,voidFunctionWithEventBaseObjectParameter  newTickFunction);
int  getOverflow ();
uint16_t  getCount ();
uint16_t  getLiveCount ();
void StartTimer(voidFunctionWithEventBaseObjectParameter  newEndFunction,voidFunctionWithEventBaseObjectParameter  newTickFunction);
//void useFunc(EventBaseObject * );
//void ( * userUnevenFunc)(EventBaseObject * );
static void ( * userUnevenTickFunc)(EventBaseObject * );
static void ( * userUnevenEndFunc)(EventBaseObject * );
const int ledPin = 13;




