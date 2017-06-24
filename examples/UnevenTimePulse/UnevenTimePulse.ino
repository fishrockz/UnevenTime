#include <UnevenTime.h>

int LEDSTATE;

void setup() {
  pinMode(ledPin, OUTPUT);
  //Serial.begin(115200);
  LEDSTATE=0;
}


int ticks=0;
void loop() {
  
    // Clear channel interrupt flag

#if defined(KINETISK)
 Serial.print("KINETISK: ");
#endif
    
    Serial.print("ticks: ");
    Serial.print(ticks); // Captured time of wheel revolution
    Serial.println(" ");
//    Serial.print(" Overflow: ");
//    Serial.println(getOverflow()); // Overflow happens after ~ 4.2s
    ticks++;
    LEDSTATE=1;
    digitalWrite(ledPin, LEDSTATE);
    
    StartTimer();
    delay(1000);
    
//  if ((FTM0_C0SC&0x80) != 0) { // Look for channel interrupt flag
//    digitalWrite(ledPin, HIGH);
//  }else {
//    digitalWrite(ledPin, LOW);
//  }
//  FTM0_C0SC &= ~0x80;
}
