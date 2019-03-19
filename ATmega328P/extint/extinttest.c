//*****************************************************************************
// File Name	: extinttest.c
// 
// Title		: example usage of external interrupt library functions
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
// 
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 14-Dec-2004	pstang		Created the program
//*****************************************************************************


//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include <stdio.h>
#include <util/delay.h>
#include "../serial/serial.h"
#include "../util/print.h"
#include <avr/pgmspace.h>

#include "extint.h"		// include external interrupt library

// global variables
volatile  uint16_t Int0Count;
volatile uint16_t Int1Count;

// functions
void extintTest(void);
void myInt0Handler(void);
void myInt1Handler(void);


// int main()
// {
//   // Initialize system upon power-up.
//   serial_init(); // Setup serial baud rate and interrupts
//   sei();         // Enable interrupts
//   // Write your code here
//   // Start main loop. Processes program inputs and executes them.
//   //protocol_main_loop();
//   extintTest();
//   return 0;
// }

void extintTest(void)
{
	#ifndef sbi
		#define sbi(reg,bit)	reg |= (_BV(bit))
	#endif

	uint16_t temp0, temp1;

	// print a little intro message so we know things are working
	printPgmString(PSTR("\r\n\n\nWelcome to the External Interrupt library test program!\r\n"));
	
	// initialize the external interrupt library
	printPgmString(PSTR("Initializing external interrupt library\r\n"));
	extintInit();

	// configure external interrupts for rising-edge triggering.
	// when a rising-edge pulse arrives on INT0 or INT1,
	// the interrupt will be triggered
	printPgmString(PSTR("Configuring external interrupts\r\n"));
	extintConfigure(EXTINT0, EXTINT_EDGE_RISING);
	extintConfigure(EXTINT1, EXTINT_EDGE_RISING);

	// attach user interrupt routines.
	// when the interrupt is triggered, the user routines will be executed
	printPgmString(PSTR("Attaching user interrupt routines\r\n"));
	extintAttach(EXTINT0, myInt0Handler);
	extintAttach(EXTINT1, myInt1Handler);

	// enable the interrupts
	printPgmString(PSTR("Enabling external interrupts\r\n"));
	// (support for this has not yet been added to the library)
	sbi(EIMSK, INT0);
	sbi(EIMSK, INT1);

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

void myInt0Handler(void)
{
	// count this interrupt event
	Int0Count++;
}

void myInt1Handler(void)
{
	// count this interrupt event
	Int1Count++;
}

