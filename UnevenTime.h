#include <Arduino.h>
#include <EventObjects.h>



class UnevenTimeEventObject: public EventBaseObject
{
public:
	int testvalue(void){return 99;};
	int CurentTimerIndex = 0;
};



const int ledPin = 13;


class UnevenTimeTriger
{
public:
	//UnevenTimeTriger(void);

	void StartTimer();
	void StartTimer(voidFunctionWithEventBaseObjectParameter  newEndFunction,voidFunctionWithEventBaseObjectParameter  newTickFunction);
	
	// keep?
	int  getOverflow ();
	uint16_t  getCount ();
	uint16_t  getLiveCount ();
	
	friend void ftm1_isr(void); // now the isr can see list etc.
	
private:
	void ( * userUnevenTickFunc)(EventBaseObject * );
	void ( * userUnevenEndFunc)(EventBaseObject * );
	
	void setupFTM1(voidFunctionWithEventBaseObjectParameter  newEndFunction,voidFunctionWithEventBaseObjectParameter  newTickFunction);
	
	uint32_t TimeAsTicks [10];
	uint32_t reversvalue [10];
	
	static UnevenTimeTriger *list[2];  // can be 8 if using ftm0 or ftm3 on teensy
	
	int CurentTimerIndex = 0;
	int RollOverCount = 0;
};

