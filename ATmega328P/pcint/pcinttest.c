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
#include <avr/io.h>		   // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h> // include interrupt support

#include <stdio.h>
#include <util/delay.h>
#include "../serial/serial.h"
#include "../util/print.h"
#include <avr/pgmspace.h>

#include "pcint.h" // include pin change interrupt library

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
volatile uint16_t Int0Count;
volatile uint16_t Int1Count;

// functions
void mypcint0Handler(void*);
void mypcint2Handler(void*);

void pcintTest(void)
{
	uint16_t temp0, temp1;

	// print a little intro message so we know things are working
	printPgmString(PSTR("\r\n\n\nWelcome to the pin change Interrupt library test program!\r\n"));

	// initialize the external interrupt library
	printPgmString(PSTR("Initializing PD2(PCINT18) and  PD3(PCINT19) to pullup-input\r\n"));

	// PD2, PD3 现在处于上拉输入状态下

	// attach user interrupt routines.
	// when the interrupt is triggered, the user routines will be executed
	printPgmString(PSTR("Attaching user interrupt routines\r\n"));

	Callback *p19=register_pcinterrupt(PCINTR19, mypcint2Handler,NULL);
	Callback *p18=register_pcinterrupt(PCINTR18, mypcint0Handler,NULL);
	enable_pcinterrupt(p19);
	enable_pcinterrupt(p18);

	// In this loop we will count the number of external interrupts,
	// and therefore the number of rising edges, that occur in one second.
	// This is precisely the frequency, in cycles/second or Hz, of the signal
	// that is triggering the interrupt.
	while (1)
	{
		// reset interrupt counters
		Int0Count = 0;
		Int1Count = 0;
		// wait 1 second
		_delay_ms(5000); //timerPause(1000);
		// get counter values
		temp0 = Int0Count;
		temp1 = Int1Count;
		// print results
		debugPrintfSerial("Frequency on PCINTR18 pin: %dHz -- On PCINTR19 pin: %dHz \r\n", temp0, temp1);
	}
// end:;
}

void mypcint0Handler(void* t)
{
    // debugPrintfSerial("ENTER isr  %s \r\n","mypcint0Handler");

	// count this interrupt event
	Int0Count++;
}

void mypcint2Handler(void* t)
{
    // debugPrintfSerial("ENTER isr  %s \r\n","mypcint2Handler");
	// count this interrupt event
	Int1Count++;
}
