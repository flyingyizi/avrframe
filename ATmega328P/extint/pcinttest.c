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
	printPgmString(PSTR("Initializing pin change interrupt library\r\n"));
	
	pcint0Init();
	pcint2Init();


	// configure external interrupts for rising-edge triggering.
	// when a rising-edge pulse arrives on INT0 or INT1,
	// the interrupt will be triggered
	printPgmString(PSTR("Configuring external interrupts\r\n"));

	// attach user interrupt routines.
	// when the interrupt is triggered, the user routines will be executed
	printPgmString(PSTR("Attaching user interrupt routines\r\n"));
	pcint0Attach(PCINT3, mypcint0Handler);
	pcint2Attach(PCINT23, mypcint2Handler);

	// enable the interrupts
	printPgmString(PSTR("Enabling pin change interrupts\r\n"));
	// (support for this has not yet been added to the library)
	pcint0Enable( pcint0Intrrupts   );
	pcint2Enable( pcint2Intrrupts  );

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

