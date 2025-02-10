
#include "leds.h"

/*
/! Initialize LED1
*/
void LED1_Init(void)
{
   // configure PortPin for LED1 as port I/O 
   LED1_PORT->SEL0 &= ~LED1_PIN;
   LED1_PORT->SEL1 &= ~LED1_PIN;

   // make built-in LED1 LED high drive strength
   LED1_PORT->DS |= LED1_PIN;
    
   // make built-in LED1 out	 
   LED1_PORT->DIR |= LED1_PIN;

   // turn off LED
   LED1_PORT->OUT |= LED1_PIN;

}

/*
/! Initialize LED2
*/
void LED2_Init(void)
{
   // configure PortPin for LED2 as port I/O 
   LED2_PORT->SEL0 &= ~(LED2_RED_PIN | LED2_GREEN_PIN | LED2_BLUE_PIN);
   LED2_PORT->SEL1 &= ~(LED2_RED_PIN | LED2_GREEN_PIN | LED2_BLUE_PIN);

   // make built-in LED2 LEDs high drive strength
   LED1_PORT->DS |= (LED2_RED_PIN | LED2_GREEN_PIN | LED2_BLUE_PIN);

   // make built-in LED2 out	 
   LED2_PORT->DIR |= (LED2_RED_PIN | LED2_GREEN_PIN | LED2_BLUE_PIN);

   // turn off LED
   LED2_PORT->OUT |= LED2_RED_PIN;
   LED2_PORT->OUT |= LED2_GREEN_PIN;
   LED2_PORT->OUT |= LED2_BLUE_PIN;
}

/*
/! Turn on LED1 (RED)
*/
void LED1_On(void)
{
    LED1_PORT->OUT |= LED1_PIN;
}

/*
/! Turn off LED1
*/
void LED1_Off(void)
{
    LED1_PORT->OUT &= ~LED1_PIN;
}
