#include <UnevenTime.h>
//#include <EventObjects.h>


int LEDSTATE;
int trigerPin=12;



uint32_t reversvalueA [] = { 
10,
40,
500,
4,
300,
20,
50,
20,
5,
20,

};

uint32_t reversvalueB [] = { 
10,
40,
500,
4,
300,
20,
50,
20,
5,
20,

};


uint32_t reversvalueC [] = { 
10,
40,
500,
4,
300,
20,
50,
20,
5,
20,

};











void setup() {
	pinMode(ledPin, OUTPUT);
	pinMode(trigerPin, OUTPUT);
	LEDSTATE=0;
	
	digitalWrite(trigerPin, 1);digitalWrite(ledPin, LEDSTATE);
	delay(150);
	digitalWrite(trigerPin, 0);
	delay(500);
	Serial.println("off:");
}


void blockFunction(EventBaseObject * ThisFunctionsInfoPointer){

	Serial.println ("this is the fast function");
	
	if (LEDSTATE==1){LEDSTATE=0;}else{LEDSTATE=1;}
	digitalWrite(ledPin, LEDSTATE);
	

}

void generalFunction(EventBaseObject * ThisFunctionsInfoPointer){

	Serial.println ("this is the slow function");

	if (LEDSTATE==1){LEDSTATE=0;}else{LEDSTATE=1;}
	digitalWrite(ledPin, LEDSTATE);
	

}

UnevenTimeTriger UnevenTimerBlock;

int ticks=0;
void loop() {
  


	#if defined(KINETISK)
		Serial.print("KINETISK: ");
	#endif
    
	Serial.print("ticks: ");
	Serial.print(ticks);
	Serial.println(" ");

	ticks++;
	LEDSTATE=1;

	digitalWrite(trigerPin, 1);digitalWrite(ledPin, LEDSTATE);

	Serial.println("StartTimer: ");
	UnevenTimerBlock.StartTimer(reversvalueA,(uint32_t)10,generalFunction,blockFunction);
	Serial.println("StartTimered! ");
	delay(100);
	digitalWrite(trigerPin, 0);
	Serial.println( UnevenTimerBlock.getLiveCount());
	delay(2000);
    

}
