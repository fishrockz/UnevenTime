#include <UnevenTime.h>

int LEDSTATE;
int trigerPin=12;


void setup() {
	pinMode(ledPin, OUTPUT);
	pinMode(trigerPin, OUTPUT);
	LEDSTATE=0;
}


void blockFunction(EventBaseObject * ThisFunctionsInfoPointer){

	Serial.println ("this is the fast function");
}

void generalFunction(EventBaseObject * ThisFunctionsInfoPointer){

	Serial.println ("this is the slow function");

}



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


	StartTimer(generalFunction,blockFunction);
	delay(100);
	digitalWrite(trigerPin, 0);
	Serial.println( getLiveCount());
	delay(2000);
    

}
