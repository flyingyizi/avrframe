/*! \file pcint.h \brief pin change Interrupt function library. */
//*****************************************************************************
//
// File Name	: 'pcint.h'
// Title		: pin change Interrupt function library
// Author		: tuxueqing - Copyright (C) 2002-2004
// Created		: 03/10/2019
// Revised		: 
// Version		: 1.0
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// Notes:	This library provides convenient standardized configuration and
//			access to pin change interrupts.  The library is designed to make
//			it possible to write code that uses pin change interrupts without
//			digging into the processor datasheets to find register names and
//			bit-defines.  The library also strives to allow code which uses
//			pin change interrupts to more easily cross-compile between different
//			microcontrollers.
//
//			NOTE: Using this library has certain advantages, but also adds
//			overhead and latency to interrupt servicing.  If the smallest
//			code size or fastest possible latency is needed, do NOT use this
//			library; link your interrupts directly.
//
//*****************************************************************************

#ifndef PCINT_H
#define PCINT_H


// constants/macros/typdefs




// functions

//! initializes pin change interrupt library
void pcint0Init(void);
void pcint1Init(void);
void pcint2Init(void);


void pcint0Disable(uint8_t interruptNums);
void pcint1Disable(uint8_t interruptNums);
void pcint2Disable(uint8_t interruptNums);


//! Configure pin change interrupt trigger

//! Configure pin change interrupt (PCINT7~PCINT0) trigger
// NOTE: it should be called after pcintAttach at all !!!
// interruptNums: group of PCINT7~PCINT0.
//                e.g., (_BV(PCINT3) |_BV(PCINT4) |_BV(PCINT2)  )	
void pcint0Enable(uint8_t interruptNums);
//! Configure pin change interrupt (PCINT14~PCINT8) trigger
// NOTE: it should be called after pcintAttach at all !!!
// configuration: group of PCINT14~PCINT8.
//                e.g., (_BV(PCINT8) |_BV(PCINT9) |_BV(PCINT12)  )	
void pcint1Enable(uint8_t interruptNums);
//! Configure pin change interrupt (PCINT23~PCINT16) trigger
// NOTE: it should be called after pcintAttach at all !!!
// interruptNums: group of PCINT23~PCINT16.
//                e.g., (_BV(PCINT18) |_BV(PCINT19)  )	
void pcint2Enable(uint8_t interruptNums);

// pcintAttach and pcintDetach commands
//		These functions allow the attachment (or detachment) of any user
//		function to an pin change interrupt.  "Attaching" one of your own
//		functions to an interrupt means that it will be called whenever
//		that interrupt is triggered.  Example usage:
//
//		pcintAttach(EXTINT0, myInterruptHandler);
//		pcintDetach(EXTINT0);
//
//		pcintAttach causes the myInterruptHandler() to be attached, and therefore
//		execute, whenever the corresponding interrupt occurs.  pcintDetach removes
//		the association and executes no user function when the interrupt occurs.
//		myInterruptFunction must be defined with no return value and no arguments:
//
//		void myInterruptHandler(void) { ... }

//! Attach a user function to an pin change interrupt
void pcint0Attach(uint8_t interruptNum, void (*userHandler)(void) );
void pcint1Attach(uint8_t interruptNum, void (*userHandler)(void) );
void pcint2Attach(uint8_t interruptNum, void (*userHandler)(void) );
//! Detach a user function from an pin change interrupt
void pcint0Detach(uint8_t interruptNum);
void pcint1Detach(uint8_t interruptNum);
void pcint2Detach(uint8_t interruptNum);

#endif
