/*! \file extint.h \brief External-Interrupt function library. */
//*****************************************************************************
//
// File Name	: 'extint.h'
// Title		: External-Interrupt function library
// Author		: Pascal Stang - Copyright (C) 2002-2004
// Created		: 5/10/2002
// Revised		: 11/16/2004
// Version		: 1.0
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// Notes:	This library provides convenient standardized configuration and
//			access to external interrupts.  The library is designed to make
//			it possible to write code that uses external interrupts without
//			digging into the processor datasheets to find register names and
//			bit-defines.  The library also strives to allow code which uses
//			external interrupts to more easily cross-compile between different
//			microcontrollers.
//
//			NOTE: Using this library has certain advantages, but also adds
//			overhead and latency to interrupt servicing.  If the smallest
//			code size or fastest possible latency is needed, do NOT use this
//			library; link your interrupts directly.
//
//*****************************************************************************

#ifndef EXTINT_H
#define EXTINT_H


// constants/macros/typdefs

// interrupt macros for attaching user functions to external interrupts
// use these with extintAttach( intNum, function )
typedef enum EXTINTTYPET {
/*#define*/ EXTINT0		=		0x00,	///< External Interrupt 0
/*#define*/ EXTINT1		=		0x01,	///< External Interrupt 1
/*#define*/ EXTINT2		=		0x02,	///< External Interrupt 2
/*#define*/ EXTINT3		=		0x03,	///< External Interrupt 3
/*#define*/ EXTINT4		=		0x04,	///< External Interrupt 4
/*#define*/ EXTINT5		=		0x05,	///< External Interrupt 5
/*#define*/ EXTINT6		=		0x06,	///< External Interrupt 6
/*#define*/ EXTINT7		=		0x07	///< External Interrupt 7
}EXTINTTYPE;

typedef enum EXTINTMODET {
/*#define*/ EXTINT_LEVEL_LOW	= 0x00,	///< Trigger on low level
/*#define*/ EXTINT_EDGE_ANY		= 0x01,	///< Trigger on any edge
/*#define*/ EXTINT_EDGE_FALLING	= 0x02,	///< Trigger on falling edge
/*#define*/ EXTINT_EDGE_RISING	= 0x03	///< Trigger on rising edge
}EXTINTMODE;

// processor-adaptive defines
// mainstream AVR processors generally have 1,2,3, or 8 external interrupts
// (if someone has a better idea of how to manage this, let me know)
#ifdef INT7_vect
	#define EXTINT_NUM_INTERRUPTS	8
#else
	#ifdef INT2_vect
		#define EXTINT_NUM_INTERRUPTS	3
	#else
		#ifdef INT1_vect
			#define EXTINT_NUM_INTERRUPTS	2
		#else
			#define EXTINT_NUM_INTERRUPTS	1
		#endif
	#endif
#endif

// functions

//! initializes extint library
void extintInit(void);

//! Configure external interrupt trigger

// extintAttach and extintDetach commands
//		These functions allow the attachment (or detachment) of any user
//		function to an external interrupt.  "Attaching" one of your own
//		functions to an interrupt means that it will be called whenever
//		that interrupt is triggered.  Example usage:
//
//		extintAttach(EXTINT0, myInterruptHandler);
//		extintDetach(EXTINT0);
//
//		extintAttach causes the myInterruptHandler() to be attached, and therefore
//		execute, whenever the corresponding interrupt occurs.  extintDetach removes
//		the association and executes no user function when the interrupt occurs.
//		myInterruptFunction must be defined with no return value and no arguments:
//
//		void myInterruptHandler(void) { ... }

//! Attach a user function to an external interrupt
void extintAttach(EXTINTTYPE interruptNum, void (*userHandler)(void), EXTINTMODE mode);
//! Detach a user function from an external interrupt
void extintDetach(uint8_t interruptNum);

#endif
