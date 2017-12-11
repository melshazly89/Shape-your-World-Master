// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
unsigned long delay1;
unsigned long KeyTone;        //Reading Pressed Piano Key

int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
	SYSCTL_RCGC2_R |= 0x0000003F; // activate port B,E
  delay1 = SYSCTL_RCGC2_R;    // allow time to finish activating
  EnableInterrupts();  // enable after all initialization are done

// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
 
	
  while(1){                
// input from keys to select tone
KeyTone=Piano_In();
		if(KeyTone==0x01)
		{
			//Sound_Tone(4778);    //Freq=523.251 Hz
			Sound_Tone(9558);    //Freq=523.251 Hz
			EnableInterrupts();
		}
		else if (KeyTone==0x02)
		{
			//Sound_Tone(4257);    //Freq=587.330 Hz
			Sound_Tone(8514);    //Freq=587.330 Hz
			EnableInterrupts();
		}
		else if (KeyTone==0x04)
		{
			//Sound_Tone(3792);    //Freq=659.255 Hz
			Sound_Tone(7584);    //Freq=659.255 Hz
			EnableInterrupts();
		}
		else if (KeyTone==0x08)
		{
			//Sound_Tone(3189);    //Freq=783.991 Hz
			Sound_Tone(6378);    //Freq=783.991 Hz
			EnableInterrupts();
		}
		else
		{
		Sound_Off();
		//DisableInterrupts();
		}
  }
	delay(10);
            
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


