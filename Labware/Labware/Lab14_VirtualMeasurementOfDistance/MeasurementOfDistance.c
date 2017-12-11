// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
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

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE2(Ain1) and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"

void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
  return 2000*sample/4096+1;  // Convert The 12 Bit ADC to value in cm
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
     NVIC_ST_CTRL_R=0x00;      //Disable Systick
	   NVIC_ST_CURRENT_R=0x00;   //Intialize Systick
	   NVIC_ST_RELOAD_R=period-1; //Reload Value is equivalent to 25 ms
	   NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1 
	   NVIC_ST_CTRL_R=0x07;      //Enable Clk & Interrupt
	   
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
    ADCdata=ADC0_In();          //Capture Sample 
	  Distance=Convert(ADCdata);  //Convert Sample to Distance
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 you implemented this function
 if(n<10)
	{
		String[0]='0';
		String[2]='0';
		String[3]='0';
		String[4]=n+0x30;
	}
	else if(n<100)
	{ 
		
		String[0]='0';
		String[2]='0';
		String[3]=n/10+0x30;
		n=n%10;
		String[4]=n+0x30;
	}
	else if(n<1000)
	{ 
		
		String[0]='0';
		String[2]=n/100+0x30;
		n=n%100;
		String[3]=n/10+0x30;
		n=n%10;
		String[4]=n+0x30;
	}
	else if(n<10000)
	{ 
		String[0]=n/1000+0x30;
		n=n%1000;
		String[2]=n/100+0x30;
		n=n%100;
		String[3]=n/10+0x30;
		n=n%10;
		String[4]=n+0x30;
	}
	else 
	{ 
		String[0]='*';
		String[2]='*';
		String[3]='*';
		String[4]='*';
	}
	String[1]='.';
	String[5]=' ';
	String[6]='c';
	String[7]='m';
	String[8]=0;
}


// main1 is a simple main program allowing you to debug the ADC interface
int main(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, UART0_Emulate_Nokia5110_NoScope);
	SysTick_Init(2000000); //Systick Intialization
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
	Nokia5110_Init(); 
  EnableInterrupts();
  while(1){ 
    Nokia5110_SetCursor(0, 0);
    Distance = Convert(ADCdata);
    UART_ConvertDistance(Distance); // from Lab 11
    Nokia5110_OutString(String);    // output to Nokia5110 LCD (optional)
  }
}


