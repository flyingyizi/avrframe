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
//   timer1OVFInit(0);  // initialize the timer system
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

// void setup() {
//   pinMode (LED, OUTPUT);

//   // set up Timer 1
//   TCCR1A = 0;          // normal operation
//   TCCR1B = bit(WGM12) | bit(CS10);   // CTC, no pre-scaling
//   OCR1A =  999;       // compare A register value (1000 * clock speed)  TOP模式
//   TIMSK1 = bit (OCIE1A);             // interrupt on Compare A Match
// }  // end of setup

void timer0_ForceOutputCompare_test(void)
{
	// 14.5.1 Force Output Compare
	// In non-PWM waveform generation modes, the match output of the comparator can be forced by
	// writing a one to the Force Output Compare (FOC0x) bit. Forcing compare match will not set the
	// OCF0x Flag or reload/clear the timer, but the OC0x pin will be updated as if a real compare
	// match had occurred (the COM0x1:0 bits settings define whether the OC0x pin is set, cleared or
	// toggled).
	//set OC0A to output  (PCINT22/OC0A/AIN0) PD6
	DDRD |= _BV(DDD6);

	//OCR0A
	//从Table 14-8.知道， 只有以下几种WGM组合是non-PWM mode
	//WGM02 WGM01 WGM00
	//0     0     0      Normal
	//0     1     0      CTC
	//ctc
	// TCCR0B &= ~(_BV(WGM02));TCCR0A |= (_BV(WGM01));TCCR0A &= ~(_BV(WGM00));
	//normal
	TCCR0B &= ~(_BV(WGM02));   TCCR0A &= ~(_BV(WGM01));TCCR0A &= ~(_BV(WGM00));

	OCR0A = 12; //ctc mode TOP模式

	// 	• Bit 7 – FOC0A: Force Output Compare A
	// The FOC0A bit is only active when the WGM bits specify a non-PWM mode.
	// However, for ensuring compatibility with future devices, this bit must be set to zero when
	// TCCR0B is written when operating in PWM mode. When writing a logical one to the FOC0A bit,
	// an immediate Compare Match is forced on the Waveform Generation unit. The OC0A output is
	// changed according to its COM0A1:0 bits setting.

	//COM0A1 COM0A0 Description
	//0      1      Toggle OC0A on Compare Match
	//1      0      Clear OC0A on Compare Match
	//1      1      Set OC0A on Compare Match

	//toggle
	TCCR0A &= ~(_BV(COM0A1));
	TCCR0A |= ~(_BV(COM0A0));
	// #define TCCR0A _SFR_IO8(0x24)
	// #define WGM00 0
	// #define WGM01 1
	// #define COM0B0 4
	// #define COM0B1 5
	// #define COM0A0 6
	// #define COM0A1 7

	// #define TCCR0B _SFR_IO8(0x25)
	// #define CS00 0
	// #define CS01 1
	// #define CS02 2
	// #define WGM02 3
	// #define FOC0B 6
	// #define FOC0A 7

	// CS02 CS01 CS00 Description
	// 0 0 0 No clock source (Timer/Counter stopped)
	// 0 0 1 clkI/O/(No prescaling)
	TCCR0B &= ~(_BV(CS02) | _BV(CS01));
	TCCR0B |= _BV(CS00);

	//作用是强加在波形发生器上一个比较匹配成功信号
	TCCR0B |= _BV(FOC0A); //The FOC0A bit is only active when the WGM bits specify a non-PWM mode
}
