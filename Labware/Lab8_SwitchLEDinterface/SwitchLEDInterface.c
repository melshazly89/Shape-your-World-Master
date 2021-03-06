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

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void);         // Disable interrupts
void EnableInterrupts(void);          // Enable interrupts
void Delay100ms(unsigned long time);  //Delay Function
unsigned long delay;
unsigned long SW1;
void PORTE_Init(void)
{
    SYSCTL_RCGC2_R =0x00000001; //enable clock for Port A
    delay=SYSCTL_RCGC2_R;
		GPIO_PORTA_DIR_R|=0x08;         //enable PA3 output
		GPIO_PORTA_PUR_R&=~0x40;        //disable PUR for PA2
		GPIO_PORTA_DEN_R=0x0E;          //Data Enable for PA2,3
		GPIO_PORTA_AFSEL_R=0x00;        //Disable Alternate Function for PA2,3
		GPIO_PORTA_AMSEL_R=0x00;        //Disable Analog Function for PA2,3
		GPIO_PORTA_PCTL_R=0x00;
	  GPIO_PORTA_DATA_R=0x04;
}

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  PORTE_Init();
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
    	SW1=GPIO_PORTA_DATA_R&0x04;  //Reading PA2
		if(SW1==0x04)
		{
			GPIO_PORTA_DATA_R ^=0x08;   //PA3 Toggle when SW1 is pressed
			Delay100ms(1);
		}
		else if(SW1==0x00)
		{
			GPIO_PORTA_DATA_R|=0x08;   //PA3 is on
		}
  }
  
}
void Delay100ms(unsigned long time){
  unsigned long i;
  while(time > 0){
    i = 1333333;  // this number means 100ms
    while(i > 0){
      i = i - 1;
    }
    time = time - 1; // decrements every 100 ms
  }
}
