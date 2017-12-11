// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include "TExaS.h"
//#include "tm4c123gh6pm.h"

unsigned long count=0; //Number of times SW1 is Pressed
unsigned long SW1;     //Current PA3 Value
unsigned long Prev;    //Previous PA3 Value


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){ 
	
	unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000001; // activate port A
	//delay=SYSCTL_RCGC2_R;
  GPIO_PORTA_DIR_R|=0x04;      //PA3 Input ,PA2 output
	GPIO_PORTA_DEN_R|=0x0C;       //enable digital pins PA3-0        
	GPIO_PORTA_AFSEL_R&=~0x0C;    //disable analog function for PB3-0
	GPIO_PORTA_AMSEL_R&=~0x0C;    //disable analog function for PB3-0
	GPIO_PORTA_PCTL_R=0x00;       //GPIO clear bit PCTL
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
	NVIC_ST_RELOAD_R = 90908;   // reload value
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1      
  NVIC_ST_CTRL_R = 0x0007;  // enable SysTick with core clock and interrupts
	
}

// called at 880 Hz
void SysTick_Handler(void){
	SW1=GPIO_PORTA_DATA_R&0x08;      //Reading Current PA3 Value
	if(Prev!=SW1)
		if(SW1==0x08)
		{
			count++;                     //Counter Counts Number of Times Switch is Pressed
		}
		
		if(SW1==0x08)
		{
			if(count%2!=0)
			{
				GPIO_PORTA_DATA_R ^=0x04;    //PA3 is Toggling when PA2 is Pressed one time
			}
			else
			{
				GPIO_PORTA_DATA_R&=~0x04;    //PA3 is Toggled when PA2 is Pressed one time
			}
		}
		else if(SW1==0x00)
		{
			if(count%2!=0)
			{
				GPIO_PORTA_DATA_R ^=0x04;    //PA3 remains Toggling when PA2 is released
			}
			else
			{
				GPIO_PORTA_DATA_R&=~0x04;    //PA3 is Cleared when PA2 not Pressed before
			}
			
		}
		Prev=SW1;	                      //Storing Previous State

}

int main(void){// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();         
  EnableInterrupts();   // enable after all initialization are done
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}
