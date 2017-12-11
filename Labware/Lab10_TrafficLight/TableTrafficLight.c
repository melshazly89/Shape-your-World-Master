// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"


#define SENSOR    (*((volatile unsigned long *)0x400243FC))
#define LIGHT_1   (*((volatile unsigned long *)0x400053FC))
#define LIGHT_2   (*((volatile unsigned long *)0x400253FC))
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
	

//Data Structure
struct state{
unsigned long out1; //6-bit pattern output to w/s cars
unsigned long out2; // 4-bit pattern to pedestrain to walk/not walk
unsigned long delay; //delay in systick Counts
unsigned long next[8]; //next state
};
typedef const struct state stype;
#define S0   0
#define S1   1
#define S2   2
#define S3   3
#define S4   4
#define S5   5
#define S6   6
#define S7   7
#define S8   8
#define S9   9

stype FSM[10]={
	{0x0C,0x02,80000000,{S0,S0,S1,S1,S1,S1,S1,S1}},
	{0x14,0x02,40000000,{S2,S2,S2,S2,S4,S4,S4,S2}},
	{0x21,0x02,80000000,{S2,S3,S2,S3,S3,S3,S3,S3}},
	{0x22,0x02,40000000,{S0,S0,S0,S0,S4,S4,S4,S4}},
	{0x24,0x08,80000000,{S4,S5,S5,S5,S4,S5,S5,S5}},
	{0x24,0x00,40000000,{S6,S6,S6,S6,S6,S6,S6,S6}},
	{0x24,0x02,40000000,{S7,S7,S7,S7,S7,S7,S7,S7}},
	{0x24,0x00,40000000,{S8,S8,S8,S8,S8,S8,S8,S8}},
	{0x24,0x02,40000000,{S9,S9,S9,S9,S9,S9,S9,S9}},
	{0x24,0x00,40000000,{S0,S0,S2,S0,S4,S0,S2,S0}}
};
// ***** 2. Global Declarations Section *****
unsigned long delay;
unsigned long s;     //Current State
unsigned long input; //Reading Sensors
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PORT_Init(void);         //Port B,E,F Initialization
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay); //Systick Wait Function

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  PORT_Init();
  SysTick_Init();
  EnableInterrupts();
	s=S0;
  while(1){

LIGHT_1=FSM[s].out1; //Set South-west Lights
LIGHT_2=FSM[s].out2; // Set Walk/don't Walk lights
//SysTick_Wait10ms(FSM[s].delay); //delay
SysTick_Wait(FSM[s].delay); //delay
input=SENSOR&0x07; // Reading Sensors
s=FSM[s].next[input]; //Next State
  }
}
void PORT_Init(void)
{
	SYSCTL_RCGC2_R |= 0x32;      // 1)Activate Clock for Port B,E,F
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE2-0
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;     // allow changes to PF4-0 
	GPIO_PORTF_AMSEL_R &= ~0x0A; // 3) disable analog function on PF3,1
  GPIO_PORTF_PCTL_R &= ~0x0000000F; // 4) enable regular GPIO
  GPIO_PORTF_DIR_R |= 0x0A;   // 5) outputs on PF3,1
  GPIO_PORTF_AFSEL_R |= ~0x0A; // 6) regular function on PF3,1
  GPIO_PORTF_DEN_R |= 0x0A;    // 7) enable digital on PF3,1
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// 10000us equals 10ms
//void SysTick_Wait10ms(unsigned long delay){

//  for(i=0; i<delay; i++){
//		SysTick_Wait(800000);  // wait 10ms
//  }
//}
