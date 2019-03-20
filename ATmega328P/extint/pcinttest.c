//*****************************************************************************
// File Name	: pcinttest.c
// 
// Title		: example usage of pin change interrupt library functions
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
// 
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 14-Dec-2004	flyingyizi		Created the program
//*****************************************************************************


//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include <stdio.h>
#include <util/delay.h>
#include "../serial/serial.h"
#include "../util/print.h"
#include <avr/pgmspace.h>

#include "pcint.h"		// include external interrupt library

//example
// int main()
// {
//   // Initialize system upon power-up.
//   serial_init(); // Setup serial baud rate and interrupts
//   sei();         // Enable interrupts
//   pcintTest();
//   return 0;
// }


// global variables
volatile  uint16_t Int0Count;
volatile uint16_t Int1Count;

// functions
void mypcint0Handler(void);
void mypcint2Handler(void);


void pcintTest(void)
{
	#define pcint0Intrrupts (_BV(PCINT3) |_BV(PCINT4))
	#define pcint2Intrrupts (_BV(PCINT22) |_BV(PCINT23))

	uint16_t temp0, temp1;

	// print a little intro message so we know things are working
	printPgmString(PSTR("\r\n\n\nWelcome to the pin change Interrupt library test program!\r\n"));

	// initialize the external interrupt library
	printPgmString(PSTR("Initializing PD2(PCINT18) and  PD3(PCINT19) to pullup-input\r\n"));
    DDRD &= ~((1 << DDD2) | (1 << DDD3)); //拉低PD2/3 口的电平
                                          // PD2,PD3 (PCINT18, PCINT19 pin) 现在处于输入态
    PORTD |= ((1 << PORTD2) | (1 << PORTD3)) ; // 开启上拉,必须要pullup. 见pcint.c中的说明

    // PD2, PD3 现在处于上拉输入状态下	

	// attach user interrupt routines.
	// when the interrupt is triggered, the user routines will be executed
	printPgmString(PSTR("Attaching user interrupt routines\r\n"));

    enable_pcinterrupt(PCINTR18,mypcint0Handler);
    enable_pcinterrupt(PCINTR19,mypcint2Handler);

	// In this loop we will count the number of external interrupts,
	// and therefore the number of rising edges, that occur in one second.
	// This is precisely the frequency, in cycles/second or Hz, of the signal
	// that is triggering the interrupt.

    char  str[100];
	while(1)
	{
		// reset interrupt counters
		Int0Count = 0;
		Int1Count = 0;
		// wait 1 second
		_delay_ms(1000);//timerPause(1000);
		// get counter values
		temp0 = Int0Count;
		temp1 = Int1Count;
		// print results
		sprintf(str,"Frequency on INT0 pin: %dHz -- On INT1 pin: %dHz\r\n", temp0, temp1);
		printString(str);  
	}
}

void mypcint0Handler(void)
{
	// count this interrupt event
	Int0Count++;
}

void mypcint2Handler(void)
{
	// count this interrupt event
	Int1Count++;
}

