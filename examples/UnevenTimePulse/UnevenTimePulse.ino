#include <UnevenTime.h>



void setup() {
  pinMode(ledPin, OUTPUT);
  //Serial.begin(115200);
  setupFTM0();
}

void loop() {
  
    // Clear channel interrupt flag

#if defined(KINETISK)
 Serial.print("KINETISK: ");
#endif
    
    Serial.print("Count: ");
    Serial.print(getLiveCount()/(31250.0/2)); // Captured time of wheel revolution
    Serial.println("s");
//    Serial.print(" Overflow: ");
//    Serial.println(getOverflow()); // Overflow happens after ~ 4.2s
    delay(100);
    
//  if ((FTM0_C0SC&0x80) != 0) { // Look for channel interrupt flag
//    digitalWrite(ledPin, HIGH);
//  }else {
//    digitalWrite(ledPin, LOW);
//  }
//  FTM0_C0SC &= ~0x80;
}
