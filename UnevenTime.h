

#include <Arduino.h>
#include <EventObjects.h>



class UnevenTimeEventObject: public EventBaseObject
{
public:
	int testvalue(void){return 99;};

};





void setupFTM1(voidFunctionWithEventBaseObjectParameter newFunction);
int  getOverflow ();
uint16_t  getCount ();
uint16_t  getLiveCount ();
void StartTimer(voidFunctionWithEventBaseObjectParameter newFunction);
//void useFunc(EventBaseObject * );
//void ( * userUnevenFunc)(EventBaseObject * );
static void ( * userUnevenFunc)(EventBaseObject * );

const int ledPin = 13;




