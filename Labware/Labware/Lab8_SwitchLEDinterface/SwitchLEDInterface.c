// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#define SW                   (*((unsigned volatile long *)0x40024004))   //PE0 Address
#define LED                  (*((unsigned volatile long *)0x40024008))  //PE1 Address
#define GPRO_PORTE_DIR_R     (*((unsigned volatile long *)0x40024400))
#define GPRO_PORTE_DEN_R     (*((unsigned volatile long *)0x4002451C))
#define GPRO_PORTE_AFSEL_R   (*((unsigned volatile long *)0x40024420))
#define GPRO_PORTE_AMSEL_R   (*((unsigned volatile long *)0x40024528))
#define GPRO_PORTE_PUR_R     (*((unsigned volatile long *)0x40024510))
#define GPRO_PORTE_PCTL_R    (*((unsigned volatile long *)0x4002452C))
#define SYSCTL_RCGC2_R       (*((volatile unsigned long *)0x400FE108))

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_init(void);
void Delay1ms(unsigned long msec);
unsigned long Delay;
// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).

void PortE_init(void)
{

	SYSCTL_RCGC2_R|=0x10;     //Enable Clock for PE
	Delay=SYSCTL_RCGC2_R;     //Wait 4 Cycles
	GPRO_PORTE_DIR_R|=0x02;   //Set PE0 as input &PE1 as output
	GPRO_PORTE_DEN_R|=0x03;   //Data Enable for PE0,1
	GPRO_PORTE_AFSEL_R&=~0x03;//Disable Alternate Function
	GPRO_PORTE_PCTL_R=0x00;   //Disable Alternate Function
	GPRO_PORTE_AMSEL_R&=~0x03;//Disable Analog Function
	GPRO_PORTE_PUR_R&=~0x03;  //Disable internal Pull up resistor
}

void Delay1ms(unsigned long msec)
{
	
	while(msec>0)
	{
		unsigned long i=16000;
	   while(i>0)
	   {
	     i--;
	   }
	msec--;
	}
}

int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  PortE_init();
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1)
	{
    if(SW==0x01)
		{
		LED^=0x02;
		Delay1ms(100);
		}
		else 
			LED|=0x02;
  }
  
}
