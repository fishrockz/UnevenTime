#include "UnevenTime.h"




// convert from microseconds to I/O clock ticks
#if defined(KINETISK)
#define CLOCKS_PER_MICROSECOND ((double)F_BUS / 1000000.0)
#elif defined(KINETISL)
#define CLOCKS_PER_MICROSECOND ((double)F_PLL / 2000000.0)
#endif

#define TX_MINIMUM_SIGNAL_CLOCKS  (uint32_t)(TX_MINIMUM_SIGNAL * CLOCKS_PER_MICROSECOND)
#define TX_MAXIMUM_SIGNAL_CLOCKS  (uint32_t)(TX_MAXIMUM_SIGNAL * CLOCKS_PER_MICROSECOND)
#define TX_DEFAULT_SIGNAL_CLOCKS  (uint32_t)(TX_DEFAULT_SIGNAL * CLOCKS_PER_MICROSECOND)
#define TX_MINIMUM_SPACE_CLOCKS   (uint32_t)(TX_MINIMUM_SPACE * CLOCKS_PER_MICROSECOND)
#define TX_MINIMUM_FRAME_CLOCKS   (uint32_t)(TX_MINIMUM_FRAME * CLOCKS_PER_MICROSECOND)
#define TX_PULSE_WIDTH_CLOCKS     (uint32_t)(TX_PULSE_WIDTH * CLOCKS_PER_MICROSECOND)
#define RX_MINIMUM_SPACE_CLOCKS   (uint32_t)(RX_MINIMUM_SPACE * CLOCKS_PER_MICROSECOND)


//#define FTM0_SC_VALUE (FTM_SC_TOIE | FTM_SC_CLKS(1) | FTM_SC_PS(0))

#if defined(KINETISK)
#define CSC_CHANGE(reg, val)         ((reg)->csc = (val))
#define CSC_INTACK(reg, val)         ((reg)->csc = (val))
#define CSC_CHANGE_INTACK(reg, val)  ((reg)->csc = (val))
#define FRAME_PIN_SET()              *framePinReg = 1
#define FRAME_PIN_CLEAR()            *framePinReg = 0
#endif


uint16_t  getLiveCount (){

	return FTM0_CNT;

}
#define FTM0_SC_VALUE ( FTM_SC_TOIE | FTM_SC_CLKS(0b01) | FTM_SC_PS(0b0))
//FTM_SC_TOIE |
//FTM_SC_TOIE    = enable overflow inturupt
//FTM_SC_CLKS(1) 1 = system clock
//FTM_SC_CLKS(10) 10 = FF clock
//FTM_SC_PS(0)  0 = no divied



int cscSet = 	0b01011100;
int cscClear = 	0b01011000;
int SetBit = 	0b00000100;


// Setup function for FlexTimer0
void setupFTM0() {
  // Input filter to help prevent glitches from triggering the capture
  // 4+4×val clock cycles, 48MHz = 4+4*7 = 32 clock cycles = 0.75us
  //FTM0_FILTER = 0x07;

  // Must set the Write-protect disable (WPDIS) bit to allow modifying other registers
  // The enable (FTMEN) bit is also set to enable the FlexTimer0 module
  // FAULTIE=0, FAULTM=00, CAPTEST=0, PWMSYNC=0, WPDIS=1, INIT=0, FTMEN=1
  //FTM0_MODE = 0x05;
  //FTM0_FILTER = 0x07;
  // FLEXTimer0 configuration
  // Clock source is Fixed Frequency Clock running at 31.25kHz (FLL Clock input = MCGFFCLK)
  // Dividing that by 2 would have the counter roll over about every 4 seconds
  #if defined(KINETISK)
  FTM0_MODE = 0;
  #endif
  FTM0_SC = 0x00; // Set this to zero before changing the modulus
  //FTM0_CNTIN = 0x0000; // Reset the count to zero
  FTM0_CNT = 0x0000; // Reset the count to zero
  FTM0_MOD = 0xFFFF; // max modulus = 65535




  //FTM0_SC = 0x11; // TOF=0 TOIE=0 CPWMS=0 CLKS=10 (FF clock) PS=001 (divide by 2)
  //FTM0_SC = 0b00010001;
  //FTM0_SC = 0b01001011;
  //            76543210 
//  FTM0_C0SC = 0x48; // CHF=0 CHIE=1 (enable interrupt) MSB=0 MSA=0 ELSB=1 (input capture) ELSA=0 DMA=0
  //FTM0_C0SC = 0b01011100;
  FTM0_C0SC = 0b01010100;
  //FTM0_C0SC &= ~SetBit; // this affect whether or not we will change the output but not if the isr gets trigered
  
  //FTM0_C0SC = cscClear;
  //            76543210 
  //cscSet = 0b01011100;
  FTM0_C0V = 0x7000;
//  FTM0_C0SC = 0b100000;
  FTM0_SC =  FTM0_SC_VALUE;
  #if defined(KINETISK)
  FTM0_MODE = 0;
  #endif
  
  
  *portConfigRegister(22) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
  //NVIC_SET_PRIORITY(IRQ_FTM0, 32);
  NVIC_ENABLE_IRQ(IRQ_FTM0);

  /* Pins that can be used for Input Capture:
     Teensy Port  Name  CPU Pin  Signal
     9            PTC3  46       FTM0_CH2
     10           PTC4  49       FTM0_CH3
     22           PTC1  44       FTM0_CH0
     23           PTC2  45       FTM0_CH1
  */

  // PIN configuration, alternative function 4 on Pin 44 (Teensy 22) (FTM0_CH0)
  //PORTC_PCR1 |= 0x400;
}


const int numberOfTimes=6;

int CurentTimerIndex=0;


uint32_t reversvalue [numberOfTimes] = { 
0x6000,
0x1000,
0x1000,
0x1000,
0x1000,0x1000,
};

void StartTimer(void) {
//FTM0_SC =  FTM0_SC_VALUE;
setupFTM0();



}
int LEDSTATE=0;
// Interrupt Service Routine for FlexTimer0 Module
void ftm0_isr(void) {
	//Serial.println ("fire");
	#if defined(KINETISK)
	FTM0_MODE = 0;
	#endif

	
	if (FTM0_SC & 0x80) {
	
	if (LEDSTATE==1){LEDSTATE=0;}else{LEDSTATE=1;}
		digitalWrite(ledPin, LEDSTATE);
	
	//	digitalWrite(ledPin, LOW);
	//	LEDSTATE=0;
	//	Serial.println ("fireA");
	//	FTM0_C0V = 0x5000;
	
		//FTM0_C0SC |= SetBit; // turn on the set but
		FTM0_SC &= ~0x80;
		
		
	}

	if  (FTM0_C0SC & 0x80){
	
		if (LEDSTATE==1){LEDSTATE=0;}else{LEDSTATE=1;}
		digitalWrite(ledPin, LEDSTATE);
	
		
		if (reversvalue[CurentTimerIndex]+FTM0_C0V> 58000){
		//FTM0_C0SC &= ~SetBit;
		}
		FTM0_C0V += reversvalue[CurentTimerIndex];
		
	
	
		FTM0_C0SC &= ~0x80;
		
//Serial.println ("fireB");
		CurentTimerIndex+=1;
		if (CurentTimerIndex>=numberOfTimes){
		
		CurentTimerIndex=0;
		FTM0_SC = 0x00;
		
		}
	}

	

	
	

	

}
