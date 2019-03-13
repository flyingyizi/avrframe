//*****************************************************************************
// File Name	: timertest.c
//
// Title		: example usage of timer library functions
// Revision		: 1.0
// Notes		:
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 30-Apr-2003	pstang		Created the program
//*****************************************************************************
//修改为适用于atmega328p

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		   // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h> // include interrupt support
#include <util/delay.h>
#include "../serial/serial.h"
#include "../util/print.h"
#include <avr/pgmspace.h>

#include "timerx8.h" // include timer function library (timing, PWM, etc)

#ifndef sbi
#define sbi(reg, bit) reg |= (_BV(bit))
#endif

//example
// int main()
// {
//   // Initialize system upon power-up.
//   serial_init(); // Setup serial baud rate and interrupts
//   // ;
//   timer1Init();  // initialize the timer system
//   sei();         // Enable interrupts

//   timer1Test();

//   while (1)
//     ;

//   return 0;
// }



void timer1Test(void)
{
	// print a little intro message so we know things are working
	printPgmString(PSTR("\r\n\n\nWelcome to the timer library test program!\r\n"));

	// // example: wait for 1/2 of a second, or 500ms
	// printPgmString(PSTR("\r\nTest of timerPause() function\r\n"));
	// printPgmString(PSTR("Here comes a 1/2-second delay...\r\n"));
	// timerPause(500); // 如果需要非常精确的延时，那使用利用timer0的timerPause(500);
	// printPgmString(PSTR("Done!\r\n"));

	// here's an example of using the timer library to do
	// pulse-width modulation or PWM.  PWM signals can be created on
	// any output compare (OCx) pin.  See your processor's data sheet
	// for more information on which I/O pins have output compare
	// capability.
	printPgmString(PSTR("\r\nTest of timer1 PWM output\r\n"));

	// set the OC1x port pins to output
	// We need to do this so we can see and use the PWM signal
	// ** these settings are correct for most processors, but not for all
	DDRB |= _BV(DDB1);
	DDRB |= _BV(DDB2);

	// initialize timer1 for PWM output
	// - you may use 8,9, or 10 bit PWM resolution
	printPgmString(PSTR("Initializing timer1 for PWM\r\n"));
	timer1PWMInit(8);

	// turn on the channel A PWM output of timer1
	// - this signal will come out on the OC1A I/O pin
	printPgmString(PSTR("Turning on timer1 channel A PWM output\r\n"));
	timer1PWMAOn();

	// set the duty cycle of the channel A output
	// - let's try 25% duty, or 256*25% = 64
	printPgmString(PSTR("Setting duty cycle to 25%%\r\n"));
	timer1PWMASet(64);

	// turn on channel B and set it to 75% duty cycle
	printPgmString(PSTR("Turning on channel B too, with 75%% duty\r\n"));
	timer1PWMBOn();
	timer1PWMBSet(192);

	// wait for 5 seconds
	printPgmString(PSTR("Pause for 5 seconds...\r\n"));
	_delay_ms(5000); 

	// now turn off all PWM on timer1
	printPgmString(PSTR("Turning off all PWM on timer1\r\n"));
	timer1PWMOff();
}
