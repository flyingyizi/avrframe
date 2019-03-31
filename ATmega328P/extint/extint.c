/*! \file extint.c \brief External-Interrupt function library. */
//*****************************************************************************
//
// File Name	: 'extint.c'
// Title		: External-Interrupt function library
// Author		: flyingyizi - Copyright (C) 2002-2004
// Created		: 03/10/2019
// Revised		:
// Version		: 1.0
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "extint.h"

#if defined(__AVR_ATmega128__)
#error "NOT SUPPORT"
#endif


// Global variables
typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr ExtIntFunc[EXTINT_NUM_INTERRUPTS];

// functions

//! initializes extint library
void extintInit(void)
{	
	uint8_t intNum;
	// detach all user functions from interrupts
	for(intNum=0; intNum<EXTINT_NUM_INTERRUPTS; intNum++)
		extintDetach(intNum);

}



//! Configure external interrupt trigger
// NOTE: this function is not complete!!!
// TODO Not yet work extintAttach()
void extintAttach(EXTINTTYPE interruptNum, void (*userHandler)(void), EXTINTMODE mode)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < EXTINT_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		ExtIntFunc[interruptNum] = userHandler;
	}

	if(interruptNum == EXTINT0)
	{
    #if defined(EICRA) && defined(ISC00) && defined(EIMSK)
        EICRA =  (EICRA & ~( _BV(ISC00) | _BV(ISC01))  ) | (mode << ISC00);
        EIMSK |= (1 << INT0);
		#elif defined(MCUCR) && defined(ISC00) && defined(GICR)
        MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
        GICR |= (1 << INT0);
        #elif defined(MCUCR) && defined(ISC00) && defined(GIMSK)
        MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
        GIMSK |= (1 << INT0);
        #else
        #error extintAttach not finished for this CPU (case 0)
		#endif
	} 
#ifdef INT1_vect	
	else if(interruptNum == EXTINT1)
	{
        #if defined(EICRA) && defined(ISC10) && defined(ISC11) && defined(EIMSK)
        EICRA =  (EICRA & ~( _BV(ISC10) | _BV(ISC11))  ) | (mode << ISC10);
        EIMSK |= (1 << INT1);
        #elif defined(MCUCR) && defined(ISC10) && defined(ISC11) && defined(GICR)
        MCUCR = (MCUCR & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
        GICR |= (1 << INT1);
        #elif defined(MCUCR) && defined(ISC10) && defined(GIMSK) && defined(GIMSK)
        MCUCR = (MCUCR & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
        GIMSK |= (1 << INT1);
        #else
        #warning extintAttach may need some more work for this cpu (case 1)
        #endif
	} 
#endif
#ifdef INT2_vect	
	else if(interruptNum == EXTINT2)
    {   
        #if defined(EICRA) && defined(ISC20) && defined(ISC21) && defined(EIMSK)
          EICRA = (EICRA & ~( _BV(ISC20) | _BV(ISC21))  ) | (mode << ISC20);
          EIMSK |= (1 << INT2);
        #elif defined(MCUCR) && defined(ISC20) && defined(ISC21) && defined(GICR)
          MCUCR = (MCUCR & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
          GICR |= (1 << INT2);
        #elif defined(MCUCR) && defined(ISC20) && defined(GIMSK) && defined(GIMSK)
          MCUCR = (MCUCR & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
          GIMSK |= (1 << INT2);
        #else
        #warning extintAttach may need some more work for this cpu (case 1)
        #endif
	}
#endif	
	// need to handle a lot more cases
	// and differences between processors.
	// looking for clean way to do it...
}

//! Detach a user function from an external interrupt
void extintDetach(uint8_t interruptNum)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < EXTINT_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		ExtIntFunc[interruptNum] = 0;
	}

    if(interruptNum == EXTINT0) {
		#if defined(EIMSK) && defined(INT0)
		EIMSK &= ~(1 << INT0);
		#elif defined(GICR) && defined(ISC00)
		GICR &= ~(1 << INT0); // atmega32
		#elif defined(GIMSK) && defined(INT0)
		GIMSK &= ~(1 << INT0);
		#else
		#error extintDetach not finished for this cpu
		#endif
	} 
#ifdef INT1_vect	
	else if(interruptNum == EXTINT1) {
		#if defined(EIMSK) && defined(INT1)
		EIMSK &= ~(1 << INT1);
		#elif defined(GICR) && defined(INT1)
		GICR &= ~(1 << INT1); // atmega32
		#elif defined(GIMSK) && defined(INT1)
		GIMSK &= ~(1 << INT1);
		#else
		#warning extintDetach may need some more work for this cpu (case 1)
		#endif
	}
#endif	 
#ifdef INT2_vect	
	else if(interruptNum == EXTINT2) {     
		#if defined(EIMSK) && defined(INT2)
		EIMSK &= ~(1 << INT2);
		#elif defined(GICR) && defined(INT2)
		GICR &= ~(1 << INT2); // atmega32
		#elif defined(GIMSK) && defined(INT2)
		GIMSK &= ~(1 << INT2);
		#elif defined(INT2)
		#warning extintDetach may need some more work for this cpu (case 2)
		#endif
	}
#endif
}

//! Interrupt handler for INT0
ISR(INT0_vect)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT0])
		ExtIntFunc[EXTINT0]();
}

#ifdef INT1_vect
//! Interrupt handler for INT1
ISR(INT1_vect)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT1])
		ExtIntFunc[EXTINT1]();
}
#endif

#ifdef INT2_vect
//! Interrupt handler for INT2
ISR(INT2_vect)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT2])
		ExtIntFunc[EXTINT2]();
}
#endif

#ifdef INT3_vect
//! Interrupt handler for INT3
ISR(INT3_vect)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT3])
		ExtIntFunc[EXTINT3]();
}
#endif

#ifdef INT4_vect
//! Interrupt handler for INT4
ISR(INT4_vect)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT4])
		ExtIntFunc[EXTINT4]();
}
#endif

#ifdef INT5_vect
//! Interrupt handler for INT5
ISR(INT5_vect)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT5])
		ExtIntFunc[EXTINT5]();
}
#endif

#ifdef INT6_vect
//! Interrupt handler for INT6
ISR(INT6_vect)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT6])
		ExtIntFunc[EXTINT6]();
}
#endif

#ifdef INT7_vect
//! Interrupt handler for INT7
ISR(INT7_vect)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT7])
		ExtIntFunc[EXTINT7]();
}
#endif

