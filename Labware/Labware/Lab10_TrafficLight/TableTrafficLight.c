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
#define S0 0  //GO in North/South Direction Traffic Light is Green
#define S1 1  //Wait for North/South Direction Traffic light is yellow
#define S2 2  //GO in East/West Direction Traffic Light is Green
#define S3 3  //Wait for East/West Direction Traffic light is yellow
#define S4 4  //Walk for Pedestrain 
#define S5 5  //Don't walk for pedestrain Flashing
#define S6 6  //Don't walk for pedestrain Flashing
#define S7 7  //Don't walk for pedestrain Flashing
#define S8 8  //Don't walk for pedestrain Flashing
#define S9 9  //Don't walk for pedestrain Flashing



// ***** 2. Global Declarations Section *****
unsigned long int Delay1;
unsigned long int s;     //current State
unsigned long int Sensor;//Sensors Input
struct cstate
{
unsigned long int Out1;  //Cstate Output for Traffic lights in both direction
unsigned long int Out2;  //Cstate Output for Walk/Not Walk Lights for pedestrains
unsigned long int Wait;  //Cstate Wait Time
unsigned long int Next[8];  //Next State
};
typedef struct cstate Stype;

Stype FSM[10]={
{0x21,0x02,5000,{S0,S1,S0,S1,S1,S1,S1,S1}},
{0x22,0x02,500,{S2,S2,S0,S2,S4,S2,S4,S2}},
{0x0C,0x02,5000,{S2,S2,S3,S3,S3,S3,S3,S3}},
{0x14,0x02,500,{S0,S2,S0,S0,S4,S4,S0,S4}},
{0x24,0x08,5000,{S4,S5,S5,S5,S4,S5,S5,S5}},
{0x24,0x00,500,{S6,S6,S6,S6,S6,S6,S6,S6}},
{0x24,0x02,500,{S7,S7,S7,S7,S7,S7,S7,S7}},
{0x24,0x00,500,{S8,S8,S8,S8,S8,S8,S8,S8}},
{0x24,0x02,500,{S9,S9,S9,S9,S9,S9,S9,S9}},
{0x24,0x00,500,{S0,S2,S0,S0,S4,S1,S0,S0}}
};


// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Systick_Init(void);          //Systick Initialization
void Systick(unsigned long int msec);
void PortE_init(void);
void PortF_init(void);
void PortB_init(void);

// ***** 3. Subroutines Section *****
void Systick_Init()
{
	NVIC_ST_CTRL_R=0x00;   //Clear All Bits in The Begining of Initialization
	NVIC_ST_CURRENT_R=0x00;//Clear The Counter
	NVIC_ST_CTRL_R=0x05;  //Set The Enable & Clk of Systick
	NVIC_ST_RELOAD_R=80000; //Delay for 1 ms
	while((NVIC_ST_CURRENT_R&0x00010000)==0x00010000)
	{}
}

void Systick(unsigned long int msec)
{
	while(msec>0)
	{
	Systick_Init();
	msec--;
	}
}

void PortB_init()
{
	GPIO_PORTB_DIR_R|=0x3F;       //Set PB5~0 as Output
	GPIO_PORTB_DEN_R|=0x3F;       //Set The Data Enable for PB5~0
	GPIO_PORTB_AFSEL_R&=~0x3F;    //Disable Alternate Function
	GPIO_PORTB_AMSEL_R&=~0x3F;    //Disable Analog Function
	GPIO_PORTB_PCTL_R=0x00000000; //Disable Alternate Function
	GPIO_PORTB_PUR_R=0x00;        //No Inputs in Port B
}

void PortE_init()
{
	GPIO_PORTE_DIR_R&=~0x07;      //Set PE2~0 as Input
	GPIO_PORTE_DEN_R|=0x07;       //Set The Data Enable for PE2~0
	GPIO_PORTE_AFSEL_R&=~0x07;    //Disable Alternate Function
	GPIO_PORTE_AMSEL_R&=~0x07;    //Disable Analog Function
	GPIO_PORTE_PCTL_R=0x00000000; //Disable Alternate Function
	GPIO_PORTE_PUR_R&=~0x07;      //PE2~0 are Positive Logic Input
}

void PortF_init()
{
	GPIO_PORTF_DIR_R|=0x0A;       //Set PF3,1 as Output
	GPIO_PORTF_DEN_R|=0x0A;       //Set The Data Enable for PF3,1
	GPIO_PORTF_AFSEL_R&=~0x0A;    //Disable Alternate Function
	GPIO_PORTF_AMSEL_R&=~0x0A;    //Disable Analog Function
	GPIO_PORTF_PCTL_R=0x00000000; //Disable Alternate Function
	GPIO_PORTF_PUR_R=0x00;        //No Inputs in Port F
}

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  SYSCTL_RCGC2_R|=0x32; //Enable Clock for Port B,E,F
	Delay1=SYSCTL_RCGC2_R;
  PortB_init();   //Port B initialization
	PortE_init();   //Port E Initiazlization
	PortF_init();   //Port F Initialization
  EnableInterrupts();
  while(1){
		GPIO_PORTB_DATA_R=FSM[s].Out1; //Car Traffic Lights for Current State
		GPIO_PORTF_DATA_R=FSM[s].Out2; //Pedestrain Traffic Light for Current State
		Systick(FSM[s].Wait);          //Delay for Current State
		Sensor=GPIO_PORTE_DATA_R&0x07; //Sensors Input
		s=FSM[s].Next[Sensor];         //Current State
     
  }
}

