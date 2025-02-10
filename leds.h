
#include "msp.h" 
#include "Common.h"

// LED1
#define  LED1_PORT         P1
#define  LED1_PIN          BIT0

// LED2
#define  LED2_PORT         P2

// LED2 - RED
#define  LED2_RED_PIN      BIT0

// LED2 - GREEN
#define  LED2_GREEN_PIN    BIT1

// LED2 - BLUE
#define  LED2_BLUE_PIN     BIT2

// LED Colors
#define RED     0
#define GREEN   1
#define BLUE    2
#define CYAN    3
#define MAGENTA 4
#define YELLOW  5
#define WHITE   6


/* Function prototypes */
extern void LED1_Init(void);
extern void LED2_Init(void);
extern BOOLEAN LED1_State(void);
extern void LED1_On(void);
extern void LED1_Off(void);