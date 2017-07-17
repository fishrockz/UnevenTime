#include <Arduino.h>
#ifndef Included_EventObjects_H
#define Included_EventObjects_H

#include <EventObjects.h>

#endif // Included_EventObjects_H 


#define MAXdelays  20


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

	//void StartTimer();
	void StartTimer(uint32_t * TimesArray,uint32_t maxTimes,voidFunctionWithEventBaseObjectParameter  newEndFunction,voidFunctionWithEventBaseObjectParameter  newTickFunction);
	
	// keep?
	int  getOverflow ();
	uint16_t  getCount ();
	uint16_t  getLiveCount ();
	
	friend void ftm1_isr(void); // now the isr can see list etc.
	
private:
	void ( * userUnevenTickFunc)(EventBaseObject * );
	void ( * userUnevenEndFunc)(EventBaseObject * );
	
	UnevenTimeEventObject CurrObject;
	
	void setupFTM1();
	
	uint32_t maxIndex=0;
	uint32_t TimeAsTicks [MAXdelays];
	uint32_t * reversvalue [];
	
	static UnevenTimeTriger *list[2];  // can be 8 if using ftm0 or ftm3 on teensy
	static int Activelist[2] ;
	int CurentTimerIndex = 0;
	int RollOverCount = 0;
};

