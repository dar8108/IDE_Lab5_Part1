/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* LJBeato
* 1/14/2021
*
* Filename: main_timer_template.c
*/
#include <stdio.h>
#include <stdlib.h>

#include "msp.h"
#include "uart.h"
#include "switches.h"
#include "leds.h"
#include "Timer32.h"
#include "CortexM.h"
#include "Common.h"
extern uint32_t SystemCoreClock;

// these are not used by the timer
BOOLEAN g_sendData = FALSE;
uint16_t line[128];

int colorIndex = 0;
BYTE colors[7] = { RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE };

BOOLEAN Timer1RunningFlag = FALSE;
BOOLEAN Timer2RunningFlag = FALSE;

unsigned long MillisecondCounter = 0;

//  I/O interrupt pin setup
//
// DIR     SEL0/SEL1    IE    IES     Port Mode
//  0          00        0     0       Input, rising edge trigger
//  0          00        0     1       Input, falling edge trigger, interrupt
//  0          00        1     0       Input, rising edge trigger, interrupt
//  0          00        1     1       Input, falling edge trigger, interrupt
//

void Switch1_Interrupt_Init(void)
{
   // disable interrupts
   DisableInterrupts();
    
   // initialize the Switch as per previous lab
   Switch1_Init();
		
   //7-0 PxIFG RW 0h Port X interrupt flag
   //0b = No interrupt is pending.
   //1b = Interrupt is pending.
   // clear flag1 (reduce possibility of extra interrupt)	
   SWITCH_1_PORT->IFG &= ~SWITCH_1_PIN;

   //7-0 PxIE RW 0h Port X interrupt enable
   //0b = Corresponding port interrupt disabled
   //1b = Corresponding port interrupt enabled	
   // arm interrupt on  P1.1	
   SWITCH_1_PORT->IE |= SWITCH_1_PIN;

   //7-0 PxIES RW Undefined Port X interrupt edge select
   //0b = PxIFG flag is set with a low-to-high transition.
   //1b = PxIFG flag is set with a high-to-low transition
   // now set the pin to cause falling edge interrupt event
   // P1.1 is falling edge event
   SWITCH_1_PORT->IES |= SWITCH_1_PIN; 
	
   // now set the pin to cause falling edge interrupt event
   NVIC_IPR8 = (NVIC_IPR8 & 0x00FFFFFF)|0x40000000; // priority 2
	
   // enable Port 1 - interrupt 35 in NVIC	
   NVIC_ISER1 = 0x00000008;  
	
   // enable interrupts  (// clear the I bit	)
   EnableInterrupts();              
	
}
void Switch2_Interrupt_Init(void)
{
   // disable interrupts
   DisableInterrupts();
	
   // initialize the Switch as per previous lab
   Switch2_Init();
	
   // now set the pin to cause falling edge interrupt event
   // P1.4 is falling edge event
   SWITCH_2_PORT->IES |= SWITCH_2_PIN;
  
   // clear flag4 (reduce possibility of extra interrupt)
   SWITCH_2_PORT->IFG &= ~SWITCH_2_PIN; 
  
   // arm interrupt on P1.4 
   SWITCH_2_PORT->IE |= SWITCH_2_PIN;      
	
   // now set the pin to cause falling edge interrupt event
   NVIC_IPR8 = (NVIC_IPR8&0x00FFFFFF)|0x40000000; // priority 2
	
   // enable Port 1 - interrupt 35 in NVIC
   NVIC_ISER1 = 0x00000008;         
	
   // enable interrupts  (// clear the I bit	)
   EnableInterrupts();              
	
}

//
// Interrupt Service Routine for Timer32-1
//
void Timer32_1_ISR(void)
{   
    // Flash LED1
	if (LED1_State() == FALSE)
	{
		LED1_On();
	}
	else 
    {
        LED1_Off(); 
    }
}

//
// Interrupt Service Routine for Timer32-2
//
void Timer32_2_ISR(void)
{        
    // Increment timer counter
    MillisecondCounter++;
}

// PORT 1 IRQ Handler
// LJBeato
// Will be triggered if any pin on the port causes interrupt
//
// Derived From: Jonathan Valvano
void PORT1_IRQHandler(void)
{
	int numSeconds = 0;

	// First we check if it came from Switch1 ?
   if(P1->IFG & SWITCH_1_PIN)  
   {
       // acknowledge P1.1 is pressed, by setting BIT1 to zero - remember P1.1 is switch 1   
	   // clear flag, acknowledge   
       SWITCH_1_PORT->IFG &= ~SWITCH_1_PIN;

       // Switch 1 pressed to turn off LED
       if (Timer1RunningFlag)
       {
           Timer1RunningFlag = FALSE;
           Timer32_1_Disable();
           LED1_Off();
       }
       else // Switch 1 pressed to turn start flashing LED
       {
           Timer1RunningFlag = TRUE;
           Timer32_1_Enable();
       }       
   }
	// Now check to see if it came from Switch2 ?
   if(P1->IFG & SWITCH_2_PIN)
   {
	   // acknowledge P1.4 is pressed, by setting BIT4 to zero - remember P1.4 is switch 2
       // clear flag4, acknowledge
       SWITCH_2_PORT->IFG &= ~SWITCH_2_PIN;

       // 2nd Switch 2 press
       if (Timer2RunningFlag)
       {
           Timer2RunningFlag = FALSE;
           Timer32_2_Disable();
           LED2_Off();
           numSeconds = MillisecondCounter;
           MillisecondCounter = 0;
           uart0_put("Time between 1st and 2nd press: ");
           putnumU(numSeconds);
           uart0_put("ms\r\n");
       }
       else // 1st Switch 2 press
       {
           Timer2RunningFlag = TRUE;
           Timer32_2_Enable();
               // Check index to change color of LED2
            switch (colors[colorIndex])
            {
                case RED:
                    LED2_RED();
                    break;
                
                case GREEN:
                    LED2_GREEN();
                    break;
                
                case BLUE:
                    LED2_BLUE();
                    break;
                
                case CYAN:
                    LED2_CYAN();
                    break;
                
                case MAGENTA:
                    LED2_MAGENTA();
                    break;
                
                case YELLOW:
                    LED2_YELLOW();
                    break;
                
                case WHITE:
                    LED2_WHITE();
                    break;
                    
                default:
                    break;          
            };
            
            // Increment color index to advance to next color
            colorIndex++;
       }          
   }
}

//
// main
//
int main(void){
   //initializations
   uart0_init();
   uart0_put("Lab5 Timer demo\r\n");
    
   // Set the Timer32-1 to 2Hz (0.5 sec between interrupts)
   Timer32_1_Init(&Timer32_1_ISR, SystemCoreClock/2, T32DIV1); // initialize Timer A32-1;
        
   // Setup Timer32-2 with a .001 second timeout.
   // So use DEFAULT_CLOCK_SPEED/(1/0.001) = SystemCoreClock/1000
   Timer32_2_Init(&Timer32_2_ISR, SystemCoreClock/1000, T32DIV1); // initialize Timer A32-1;
    
   Switch1_Interrupt_Init();
   Switch2_Interrupt_Init();
   LED1_Init();
   LED2_Init();
   EnableInterrupts();
    
   while(1)
   {
       WaitForInterrupt();
   }
}
