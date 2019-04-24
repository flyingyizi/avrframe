/*! \file timerx8.c \brief Timer function library for ATmegaXX8 Processors. */
//*****************************************************************************
//
// File Name	: 'timerx8.c'
// Title		: Timer function library for ATmegaXX8 Processors
// Author		: Pascal Stang - Copyright (C) 2000-2005
// Created		: 11/22/2000
// Revised		: 06/15/2005
// Version		: 1.0
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "avrlibdefs.h"
#include "timerx8.h"

// Program ROM constants
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
const unsigned short __attribute__((progmem)) TimerPrescaleFactor[] = {0, 1, 8, 64, 256, 1024};
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024
const unsigned short __attribute__((progmem)) TimerRTCPrescaleFactor[] = {0, 1, 8, 32, 64, 128, 256, 1024};

// Global variables
// time registers
volatile unsigned long TimerPauseReg;
volatile unsigned long Timer0Reg0;
volatile unsigned long Timer2Reg0;

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc[TIMER_NUM_INTERRUPTS];

void timerOVFInit(void)
{
	uint8_t intNum;
	// detach all user functions from interrupts
	for (intNum = 0; intNum < TIMER_NUM_INTERRUPTS; intNum++)
		timerDetach(intNum);

	// initialize all timers
	timer0OVFInit(0);
	timer1OVFInit(0);
#ifdef TCNT2 // support timer2 only if it exists
	timer2OVFInit(0);
#endif
	// enable interrupts
	sei();
}

//timer0 溢出中断初始化
void timer0OVFInit(uint8_t init_value)
{
	// initialize timer 0
	timer0SetPrescaler(TIMER0PRESCALE); // set prescaler
	TCNT0 = init_value;					// reset TCNT0  初值
	sbi(TIMSK0, TOIE0);					// enable TCNT0 overflow interrupt

	timer0ClearOverflowCount(); // initialize time registers
}

void timer1OVFInit(uint16_t init_value)
{
	// initialize timer 1
	timer1SetPrescaler(TIMER1PRESCALE); // set prescaler
	TCNT1 = init_value;					// reset TCNT1  初值
	sbi(TIMSK1, TOIE1);					// enable TCNT1 overflow
}

#ifdef TCNT2 // support timer2 only if it exists
void timer2OVFInit(uint8_t init_value)
{
	// initialize timer 2
	timer2SetPrescaler(TIMER2PRESCALE); // set prescaler
	TCNT2 = init_value;					// reset TCNT2  初值
	sbi(TIMSK2, TOIE2);					// enable TCNT2 overflow

	timer2ClearOverflowCount(); // initialize time registers
}
#endif

void timer0SetPrescaler(PRESCALER_5 prescale)
{
	// set prescaler on timer 0
	TCCR0B = ((TCCR0B & ~TIMER_PRESCALER5_MASK) | prescale);
}

void timer1SetPrescaler(PRESCALER_5 prescale)
{
	// set prescaler on timer 1
	TCCR1B = ((TCCR1B & ~TIMER_PRESCALER5_MASK) | prescale);
}

#ifdef TCNT2 // support timer2 only if it exists
void timer2SetPrescaler(PRESCALER_7 prescale)
{
	// set prescaler on timer 2
	TCCR2B = ((TCCR2B & ~TIMERRTC_PRESCALE7_MASK) | prescale);
}
#endif

uint16_t timer0GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor + (TCCR0B & TIMER_PRESCALER5_MASK)));
}

uint16_t timer1GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor + (TCCR1B & TIMER_PRESCALER5_MASK)));
}

#ifdef TCNT2 // support timer2 only if it exists
uint16_t timer2GetPrescaler(void)
{
	//TODO: can we assume for all 3-timer AVR processors,
	// that timer2 is the RTC timer?

	// get the current prescaler setting
	return (pgm_read_word(TimerRTCPrescaleFactor + (TCCR2B & TIMERRTC_PRESCALE7_MASK)));
}
#endif

void timerAttach(TIMERINTTYPE interruptNum, void (*userFunc)(void))
{
	// make sure the interrupt number is within bounds
	if (interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntFunc[interruptNum] = userFunc;
	}
}

void timerDetach(TIMERINTTYPE interruptNum)
{
	// make sure the interrupt number is within bounds
	if (interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run nothing
		TimerIntFunc[interruptNum] = 0;
	}
}

void timer0ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer0Reg0 = 0; // initialize time registers
}

long timer0GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer0ClearOverflowCount() command was called)
	return Timer0Reg0;
}

#ifdef TCNT2 // support timer2 only if it exists
void timer2ClearOverflowCount(void)
{
	// clear the timer overflow counter registers
	Timer2Reg0 = 0; // initialize time registers
}

long timer2GetOverflowCount(void)
{
	// return the current timer overflow count
	// (this is since the last timer2ClearOverflowCount() command was called)
	return Timer2Reg0;
}
#endif



void timer1PWMInit(uint8_t bitRes)
{
	// configures timer1 for use with PWM output
	// on OC1A and OC1B pins

	// enable timer1 as 8,9,10bit PWM
	if (bitRes == 9)
	{ // 9bit mode
		sbi(TCCR1A, PWM11);
		cbi(TCCR1A, PWM10);
	}
	else if (bitRes == 10)
	{ // 10bit mode
		sbi(TCCR1A, PWM11);
		sbi(TCCR1A, PWM10);
	}
	else
	{ // default 8bit mode
		cbi(TCCR1A, PWM11);
		sbi(TCCR1A, PWM10);
	}

	// clear output compare value A
	OCR1A = 0;
	// clear output compare value B
	OCR1B = 0;
}

#ifdef WGM10
// include support for arbitrary top-count PWM
// on new AVR processors that support it
void timer1PWMInitICR(uint16_t topcount)
{
	// set PWM mode with ICR top-count
	cbi(TCCR1A, WGM10);
	sbi(TCCR1A, WGM11);
	sbi(TCCR1B, WGM12);
	sbi(TCCR1B, WGM13);

	// set top count value
	ICR1 = topcount;

	// clear output compare value A
	OCR1A = 0;
	// clear output compare value B
	OCR1B = 0;
}
#endif

void timer1PWMOff(void)
{
	// turn off timer1 PWM mode
	cbi(TCCR1A, PWM11);
	cbi(TCCR1A, PWM10);
	// set PWM1A/B (OutputCompare action) to none
	timer1PWMAOff();
	timer1PWMBOff();
}

void timer1PWMAOn(void)
{
	// turn on channel A (OC1A) PWM output
	// set OC1A as non-inverted PWM
	sbi(TCCR1A, COM1A1);
	cbi(TCCR1A, COM1A0);
}

void timer1PWMBOn(void)
{
	// turn on channel B (OC1B) PWM output
	// set OC1B as non-inverted PWM
	sbi(TCCR1A, COM1B1);
	cbi(TCCR1A, COM1B0);
}

void timer1PWMAOff(void)
{
	// turn off channel A (OC1A) PWM output
	// set OC1A (OutputCompare action) to none
	cbi(TCCR1A, COM1A1);
	cbi(TCCR1A, COM1A0);
}

void timer1PWMBOff(void)
{
	// turn off channel B (OC1B) PWM output
	// set OC1B (OutputCompare action) to none
	cbi(TCCR1A, COM1B1);
	cbi(TCCR1A, COM1B0);
}

void timer1PWMASet(uint16_t pwmDuty)
{
	// set PWM (output compare) duty for channel A
	// this PWM output is generated on OC1A pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	//outp( (pwmDuty>>8), OCR1AH);		// set the high 8bits of OCR1A
	//outp( (pwmDuty&0x00FF), OCR1AL);	// set the low 8bits of OCR1A
	OCR1A = pwmDuty; //TOP模式
}

void timer1PWMBSet(uint16_t pwmDuty)
{
	// set PWM (output compare) duty for channel B
	// this PWM output is generated on OC1B pin
	// NOTE:	pwmDuty should be in the range 0-255 for 8bit PWM
	//			pwmDuty should be in the range 0-511 for 9bit PWM
	//			pwmDuty should be in the range 0-1023 for 10bit PWM
	//outp( (pwmDuty>>8), OCR1BH);		// set the high 8bits of OCR1B
	//outp( (pwmDuty&0x00FF), OCR1BL);	// set the low 8bits of OCR1B
	OCR1B = pwmDuty;
}

//! Interrupt handler for tcnt0 overflow interrupt//(SIG_OVERFLOW0)
ISR(TIMER0_OVF_vect)
{
	Timer0Reg0++; // increment low-order counter

	// increment pause counter
	TimerPauseReg++;

	// if a user function is defined, execute it too
	if (TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}

//! Interrupt handler for tcnt1 overflow interrupt(SIG_OVERFLOW1)
ISR(TIMER1_OVF_vect)
{
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER1OVERFLOW_INT])
		TimerIntFunc[TIMER1OVERFLOW_INT]();
}

#ifdef TCNT2 // support timer2 only if it exists
//! Interrupt handler for tcnt2 overflow interrupt

// (SIG_OVERFLOW2)
ISR(TIMER2_OVF_vect)
{
	Timer2Reg0++; // increment low-order counter

	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT]();
}
#endif

#if  defined(OCR0)
// include support for Output Compare 0 for new AVR processors that support it
//! Interrupt handler for OutputCompare0 match (OC0) interrupt(SIG_OUTPUT_COMPARE0)
ISR(TIMER0_COMP_vect)
{
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER0OUTCOMPARE_INT])
		TimerIntFunc[TIMER0OUTCOMPARE_INT]();
}
#elif defined(OCR0A) && defined(OCR0B) 
ISR(TIMER0_COMPA_vect)
{
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER0OUTCOMPAREA_INT])
		TimerIntFunc[TIMER0OUTCOMPAREA_INT]();
}
ISR(TIMER0_COMPB_vect)
{
	Timer0_b_cmpReg++;
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER0OUTCOMPAREB_INT])
		TimerIntFunc[TIMER0OUTCOMPAREB_INT]();
}
#endif


//! Interrupt handler for CutputCompare1A match (OC1A) interrupt(SIG_OUTPUT_COMPARE1A)
ISR(TIMER1_COMPA_vect)
{
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER1OUTCOMPAREA_INT])
		TimerIntFunc[TIMER1OUTCOMPAREA_INT]();
}

//! Interrupt handler for OutputCompare1B match (OC1B) interrupt(SIG_OUTPUT_COMPARE1B)
ISR(TIMER1_COMPB_vect)
{
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER1OUTCOMPAREB_INT])
		TimerIntFunc[TIMER1OUTCOMPAREB_INT]();
}

//! Interrupt handler for InputCapture1 (IC1) interrupt(SIG_INPUT_CAPTURE1)
ISR(TIMER1_CAPT_vect)
{
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER1INPUTCAPTURE_INT])
		TimerIntFunc[TIMER1INPUTCAPTURE_INT]();
}

//! Interrupt handler for OutputCompare2A match (OC2A) interrupt

// (SIG_OUTPUT_COMPARE2A)
ISR(TIMER2_COMPA_vect)
{
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER2OUTCOMPARE_INT])
		TimerIntFunc[TIMER2OUTCOMPARE_INT]();
}

//! Interrupt handler for OutputCompare2B match (OC2B) interrupt(SIG_OUTPUT_COMPARE2B)
ISR(TIMER2_COMPB_vect)
{
	// if a user function is defined, execute it
	if (TimerIntFunc[TIMER2OUTCOMPARE_INT])
		TimerIntFunc[TIMER2OUTCOMPARE_INT]();
}
