// ***** 0. Documentation Section *****
// Switch.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation Fire and Special Weapon Switches.  
// Mahmoud El Shazly
// Apri  2016


// fire button connected to PE0
// special weapon fire button connected to PE1
#include "..//tm4c123gh6pm.h"
#include "Switch.h"
unsigned long delay;
unsigned Firew,Specialw;
void PORTE_Init(void)
{
	SYSCTL_RCGC2_R |= 0x10;            // 1)Activate Clock for Port B,E,F
  delay = SYSCTL_RCGC2_R;            // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x03;       // 3) disable analog function on PE1-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF;  // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x03;         // 5) inputs on PE1-0
  GPIO_PORTE_AFSEL_R &= ~0x03;       // 6) regular function on PE1-0
  GPIO_PORTE_DEN_R |= 0x03;          // 7) enable digital on PE2-0
 }

 unsigned long Fire(void)
 {
	 unsigned long weapon;
	 Firew=GPIO_PORTE_DATA_R&0x03;   
	 if(Firew==0x01)
	 {
	 weapon=0x01;                      //Fire is Choosen
	 }
	 else if (Firew==0x02)
	 {
	 weapon=0x02;                      //Laser Weapon is Choosen
	 }
	 else
	 {
		 weapon=0x00;                   //No weapon is Choosen
	 }
	 return weapon;
 }
