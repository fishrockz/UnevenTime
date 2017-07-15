#include "UnevenTime.h"

extern int LEDSTATE;


// convert from microseconds to I/O clock ticks
#if defined(KINETISK)
#define CLOCKS_PER_MICROSECOND ((double)F_BUS / 1000000.0)
#elif defined(KINETISL)
#define CLOCKS_PER_MICROSECOND ((double)F_PLL / 2000000.0)
#endif



UnevenTimeTriger * UnevenTimeTriger::list[2];
int UnevenTimeTriger::Activelist[2]={0,0};

uint16_t  UnevenTimeTriger::getLiveCount (){

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


/*
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


*/







const int Trigdown = 		0b00000100;
const int TrigUp = 		0b00001000;
const int trigInternal = 	0b00010000;
const int trigExtPinAndISR = 	0b00100000;
const int trigISR=		0b01000000;

const int  FTM1_C0SC_VALUE = TrigUp | Trigdown |  trigISR | trigInternal;
//uint32_t getWaitTicks (int index){
// return reversvalue[III] *( TIMER_TICKS_PER_MICROSECOND*1000);
//}
// Setup function for FlexTimer0



extern EventObjectScheduler WsSEventManger;
UnevenTimeEventObject  ThisUnevenTimeEventInfo;

void UnevenTimeTriger::StartTimer(uint32_t * TimesArray,uint32_t maxTimes,voidFunctionWithEventBaseObjectParameter  newEndFunction,voidFunctionWithEventBaseObjectParameter  newTickFunction) {


	userUnevenEndFunc=newEndFunction;
	userUnevenTickFunc=newTickFunction;
	Serial.print("maxIndex: ");Serial.println(maxIndex);
	maxIndex=maxTimes;
	for (uint32_t III=0;III<maxIndex;III++){
	
		TimeAsTicks[III] = (uint32_t)((float)TimesArray[III] * ( TIMER_TICKS_PER_MICROSECOND*1000));
		
	}
	
	
	setupFTM1();


}


void UnevenTimeTriger::setupFTM1() {

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

	
	FTM1_C0SC = FTM1_C0SC_VALUE;// internal isr's but no output on pin 22



	FTM1_SC =  FTM1_SC_VALUE;
	#if defined(KINETISK)
	FTM1_MODE = 0;
	#endif
	Serial.print(" Update Ticks ");
	
	
	
	Serial.print(" Updated Ticks ");
	Serial.print(TimeAsTicks[CurentTimerIndex]);
	
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
	
	
	
	list[0] = this;
	Activelist[0] = 1;
	Serial.print(" Enable ");
	NVIC_ENABLE_IRQ(IRQ_FTM1);
	Serial.print(" All set up ");

}








// Interrupt Service Routine for FlexTimer1 Module

void ftm1_isr(void) {

	if  (FTM1_SC & 0x80){
		FTM1_SC &= ~0x80;
		
		if (UnevenTimeTriger::Activelist[0]==1 ) {	
			if (UnevenTimeTriger::list[0]->RollOverCount>0){
				UnevenTimeTriger::list[0]->RollOverCount-=1;
				if (UnevenTimeTriger::list[0]->RollOverCount==0){
					FTM1_C0SC = TrigUp | Trigdown |  trigISR | trigInternal;
				}
			}
		}
		if (UnevenTimeTriger::Activelist[1]==1 ) {
			if (UnevenTimeTriger::list[1]->RollOverCount>0){
				UnevenTimeTriger::list[1]->RollOverCount-=1;
				if (UnevenTimeTriger::list[1]->RollOverCount==0){
					FTM1_C1SC = TrigUp | Trigdown |  trigISR | trigInternal;
				}
			}
		}
	}

	UnevenTimeTriger * ThisUnevenTimeTriger;
	if  (FTM1_C0SC & 0x80){

		FTM1_C0SC &= 0x7F;
		ThisUnevenTimeTriger=UnevenTimeTriger::list[0];
		
		ThisUnevenTimeTriger->CurentTimerIndex+=1;		
		int localCurIndex=ThisUnevenTimeTriger->CurentTimerIndex;
		
		if (localCurIndex<ThisUnevenTimeTriger->maxIndex){
			FTM1_C0V = FTM1_C0V + ThisUnevenTimeTriger->TimeAsTicks[localCurIndex] ;
			FTM1_PWMLOAD|=0x200;// why 0x200???
			if (ThisUnevenTimeTriger->TimeAsTicks[localCurIndex]>pow(2,16)){
				FTM1_C0SC=0;
				ThisUnevenTimeTriger->RollOverCount=ThisUnevenTimeTriger->TimeAsTicks[localCurIndex]/pow(2,16);
			}
			
			EventBaseObject * UnevenTimeEventObject;
			ThisUnevenTimeTriger->userUnevenTickFunc(UnevenTimeEventObject);
		
		}else{
			FTM1_C0SC=0;
			WsSEventManger.trigger( &ThisUnevenTimeEventInfo, ThisUnevenTimeTriger->userUnevenEndFunc );
		}
	}


	if  (FTM1_C1SC & 0x80){
		FTM1_C1SC&=0x7F;

		
		ThisUnevenTimeTriger=UnevenTimeTriger::list[1];
		ThisUnevenTimeTriger->CurentTimerIndex+=1;
		int localCurIndex=ThisUnevenTimeTriger->CurentTimerIndex;
		
		if (localCurIndex<ThisUnevenTimeTriger->maxIndex){
			FTM1_C1V = FTM1_C1V + ThisUnevenTimeTriger->TimeAsTicks[localCurIndex] ;
			FTM1_PWMLOAD|=0x200;// why 0x200???
			if (ThisUnevenTimeTriger->TimeAsTicks[localCurIndex]>pow(2,16))
			{
				FTM1_C1SC=0;
				ThisUnevenTimeTriger->RollOverCount=ThisUnevenTimeTriger->TimeAsTicks[localCurIndex]/pow(2,16);
			}
			
			EventBaseObject * UnevenTimeEventObject;
			ThisUnevenTimeTriger->userUnevenTickFunc(UnevenTimeEventObject);
		
		}else{
			FTM1_C1SC=0;
			WsSEventManger.trigger( &ThisUnevenTimeEventInfo, ThisUnevenTimeTriger->userUnevenEndFunc );
		}
	}


}
