// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
	GPIO_PORTB_DIR_R|=0x0F;      //PB3-0 Output
	GPIO_PORTB_DEN_R|=0x0F;       //enable digital pins PB3-0        
	GPIO_PORTB_AFSEL_R&=~0x0F;    //disable analog function for PB3-0
	GPIO_PORTB_AMSEL_R&=~0x0F;    //disable analog function for PB3-0
	GPIO_PORTB_PCTL_R=0x00;       //GPIO clear bit PCTL
	GPIO_PORTB_DR8R_R=0x0F;       //Enable 8mA for PB3-0 

}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
	GPIO_PORTB_DATA_R=data;         //4-bit DAC out
  
}
