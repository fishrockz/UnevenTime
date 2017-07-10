#include "UnevenTime.h"

extern int LEDSTATE;


// convert from microseconds to I/O clock ticks
#if defined(KINETISK)
#define CLOCKS_PER_MICROSECOND ((double)F_BUS / 1000000.0)
#elif defined(KINETISL)
#define CLOCKS_PER_MICROSECOND ((double)F_PLL / 2000000.0)
#endif






uint16_t  getLiveCount (){

	return FTM1_CNT;

}

#define FTM1_SC_PRESCALE_RATIO 0b111
#define FTM1_SC_PRESCALE FTM_SC_PS(FTM1_SC_PRESCALE_RATIO)
#define  TIMER_TICKS_PER_MICROSECOND (CLOCKS_PER_MICROSECOND /(pow(2, FTM1_SC_PRESCALE_RATIO)))

#define FTM1_SC_VALUE ( FTM_SC_TOIE | FTM_SC_CLKS(0b01) | FTM1_SC_PRESCALE)



//FTM_SC_TOIE |
//FTM_SC_TOIE    = enable overflow inturupt
//FTM_SC_CLKS(1) 1 = system clock
//FTM_SC_CLKS(10) 10 = FF clock
//FTM_SC_PS(0)  0 = no divied
//TIMER_TICKS_PER_MICROSECOND

const int numberOfTimes=10;
uint32_t reversvalue [numberOfTimes] = { 
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

uint32_t TimeAsTicks [numberOfTimes] = { 
999,
999,
999,
999,
999,
999,
999,
999,
999,
999,
};








int CurentTimerIndex=0;
int RollOverCount=0;


int Trigdown = 		0b00000100;
int TrigUp = 		0b00001000;
int trigInternal = 	0b00010000;
int trigExtPinAndISR = 	0b00100000;
int trigISR=		0b01000000;


//uint32_t getWaitTicks (int index){
// return reversvalue[III] *( TIMER_TICKS_PER_MICROSECOND*1000);
//}
// Setup function for FlexTimer0



extern EventObjectScheduler WsSEventManger;
UnevenTimeEventObject  ThisUnevenTimeEventInfo;

void StartTimer(voidFunctionWithEventBaseObjectParameter newFunction) {
//FTM1_SC =  FTM1_SC_VALUE;
setupFTM1( newFunction);

CurentTimerIndex=0;
RollOverCount=0;
}


void setupFTM1(voidFunctionWithEventBaseObjectParameter  newFunction) {

	SIM_SCGC6|=1<<25; //enable FTM1 and FTM1 module clock
//	SIM_SCGC6|=0x03000000
	FTM1_CONF=0xC0; //set up BDM in 11
	FTM1_FMS=0x00; //clear the WPEN so that WPDIS is set in FTM1_MODE register
	FTM1_MODE|=0x05;


	#if defined(KINETISK)
	FTM1_MODE = 0;
	#endif
	FTM1_SC = 0x00; // Set this to zero before changing the modulus
	FTM1_CNTIN = 0x0000; // Reset the count to zero
	FTM1_CNT = 0x0000; // Reset the count to zero
	FTM1_MOD = 0xFFFF; // max modulus = 65535


	FTM1_C0SC = TrigUp | Trigdown |  trigISR | trigInternal;// internal isr's but no output on pin 22


	//FTM1_C0SC &= ~SetBit; // this affect whether or not we will change the output but not if the isr gets trigered

	//FTM1_C0SC = cscClear;
	//            76543210 
	//cscSet = 0b01011100;
	//FTM1_C0V = 10000;
	//  FTM1_C0SC = 0b100000;
	FTM1_SC =  FTM1_SC_VALUE;
	#if defined(KINETISK)
	FTM1_MODE = 0;
	#endif

	
	
	for (int III=0;III<numberOfTimes;III++){
	
		TimeAsTicks[III] = reversvalue[III] *( TIMER_TICKS_PER_MICROSECOND*1000);
		//Serial.println(TimeAsTicks[III]);
	}
	//#define TIMER_TICKS_PER_MICROSECOND (CLOCKS_PER_MICROSECOND * FTM1_SC_PRESCALE_RATIO)
	CurentTimerIndex=0;
	FTM1_C0V = TimeAsTicks[CurentTimerIndex];
	Serial.print(CLOCKS_PER_MICROSECOND);
	Serial.print(" ");
	Serial.print(TIMER_TICKS_PER_MICROSECOND);
	Serial.print(" Hi there ");
	Serial.print(TimeAsTicks[0]);Serial.print(" ");
	Serial.println(FTM1_C0V);
	//*portConfigRegister(22) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
	//NVIC_SET_PRIORITY(IRQ_FTM1, 32);
	
	userUnevenFunc=newFunction;
	
	
	NVIC_ENABLE_IRQ(IRQ_FTM1);

	/* Pins that can be used for Input Capture:
	Teensy Port  Name  CPU Pin  Signal
	9            PTC3  46       FTM1_CH2
	10           PTC4  49       FTM1_CH3
	22           PTC1  44       FTM1_CH0
	23           PTC2  45       FTM1_CH1
	*/

	// PIN configuration, alternative function 4 on Pin 44 (Teensy 22) (FTM1_CH0)
	//PORTC_PCR1 |= 0x400;
}








// Interrupt Service Routine for FlexTimer0 Module

void ftm1_isr(void) {

	if  (FTM1_SC & 0x80){
		FTM1_SC &= ~0x80;
			//if (LEDSTATE==1){LEDSTATE=0;}else{LEDSTATE=1;}
			//digitalWrite(ledPin, LEDSTATE);
			//Serial.println("over");
			if (RollOverCount>0){
			RollOverCount-=1;
			if (RollOverCount==0){
				FTM1_C0SC = TrigUp | Trigdown |  trigISR | trigInternal;
			}
			}
	}


	if  (FTM1_C0SC & 0x80){
		//FTM1_C0SC &= ~0x80;
		FTM1_C0SC&=0x7F;
		if (LEDSTATE==1){LEDSTATE=0;}else{LEDSTATE=1;}
		digitalWrite(ledPin, LEDSTATE);
		CurentTimerIndex+=1;
		if (CurentTimerIndex<numberOfTimes){
		
		
			FTM1_C0V = FTM1_C0V + TimeAsTicks[CurentTimerIndex] ;
			FTM1_PWMLOAD|=0x200;// why 0x200???
			//Serial.print("boo: ");Serial.println(TimeAsTicks[CurentTimerIndex]);
			if (TimeAsTicks[CurentTimerIndex]>pow(2,16)){
				FTM1_C0SC=0;
				RollOverCount=TimeAsTicks[CurentTimerIndex]/pow(2,16);
				//Serial.print("overflows: ");Serial.println(RollOverCount);
			
			
			
			}
		
		}else{
			FTM1_C0SC=0;
		
			WsSEventManger.trigger( &ThisUnevenTimeEventInfo, userUnevenFunc );
		}
	}

}
