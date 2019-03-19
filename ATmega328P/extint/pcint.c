/*! \file extint.c \brief pin change Interrupt function library. */
//*****************************************************************************
//
// File Name	: 'pcint.c'
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

#include <avr/io.h>
#include <avr/interrupt.h>

#include "pcint.h"

// Global variables
// The pin change interrupt PCI2 will trigger if any enabled PCINT23..16 pin toggles. The pin change
// interrupt PCI1 will trigger if any enabled PCINT14..8 pin toggles. The pin change interrupt PCI0
// will trigger if any enabled PCINT7..0 pin toggles.
typedef void (*voidFuncPtr)(void);
#ifdef 	PCINT0_vect
uint8_t  pcint0Attachs = 0x00;  //(PCINT7~PCINT0)
volatile static voidFuncPtr PinChange0IntFunc[PCINT7+1];
#endif
#ifdef 	PCINT1_vect
volatile static voidFuncPtr PinChange1IntFunc[PCINT14+1];
#endif
#ifdef 	PCINT2_vect
volatile static voidFuncPtr PinChange2IntFunc[PCINT23+1];
#endif


// functions

//! initializes pin change library for PCINT7~PCINT0
#ifdef 	PCINT0_vect
void pcint0Init(void)
{	
	// detach all user functions from interrupts
	uint8_t intNum,len;
	len = sizeof(PinChange0IntFunc) / sizeof(voidFuncPtr);

	for( intNum=0; intNum < len; intNum++)
		PinChange0IntFunc[intNum]=0;
}
//! Configure pin change interrupt (PCINT7~PCINT0) trigger
// NOTE: it should be called after pcintAttach at all !!!
// interruptNums: group of PCINT7~PCINT0.
//                e.g., (_BV(PCINT3) |_BV(PCINT4) |_BV(PCINT2)  )	
void pcint0Enable(uint8_t interruptNums)
{ 
    //e.g. PCMSK0 |= (1 << PCINT0);   // 设置 PCINT0 来测量状态变化并产生中断
	PCICR |=( _BV(PCIE0) )	;
	PCMSK0 |= interruptNums;         
}
//! Disables pin change interrupt (PCINT7~PCINT0) trigger.
// interruptNums: group of PCINT7~PCINT0.
//                e.g., (_BV(PCINT3) |_BV(PCINT4) |_BV(PCINT2)  )	
void pcint0Disable(uint8_t interruptNums)
{
  PCMSK0 &= ~interruptNums;  // Disable specific pins of the Pin Change Interrupt
  PCICR &= ~( _BV(PCIE0) );  // Disable Pin Change Interrupt
}

//! Attach a user function to an pin change interrupt
//  interruptNum: valid PCINT7~PCINT0
void pcint0Attach(uint8_t interruptNum, void (*userHandler)(void) )
{
		PinChange0IntFunc[interruptNum] = userHandler;
}
void pcint0Detach(uint8_t interruptNum )
{
		PinChange0IntFunc[interruptNum] = 0;
}

//! Interrupt handler for INT0
ISR(PCINT0_vect)
{
	// if a user function is defined, execute it
	uint8_t i,len;
	len = sizeof(PinChange0IntFunc) / sizeof(voidFuncPtr);
	for ( i=0; i < len;i++) {
	     if(PinChange0IntFunc[i])
		      PinChange0IntFunc[i]();
	}
}

#endif

//! initializes pin change library for PCINT14~PCINT8
#ifdef 	PCINT1_vect
void pcint1Init(void)
{	
	// detach all user functions from interrupts
	uint8_t intNum,len;
	len = sizeof(PinChange1IntFunc) / sizeof(voidFuncPtr);
	for( intNum=0; intNum < len; intNum++)
		PinChange1IntFunc[intNum]=0;
}
//! Configure pin change interrupt (PCINT14~PCINT8) trigger
// NOTE: it should be called after pcintAttach at all !!!
// configuration: group of PCINT14~PCINT8.
//                e.g., (_BV(PCINT8) |_BV(PCINT9) |_BV(PCINT12)  )	
void pcint1Enable(uint8_t interruptNums)
{
	PCICR |=( _BV(PCIE1) )	;
	PCMSK1 |= interruptNums;         
}
//! Disables pin change interrupt (PCINT14~PCINT8) trigger.
// interruptNums: group of PCINT14~PCINT8.
//                e.g., (_BV(PCINT8) |_BV(PCINT9) |_BV(PCINT12)  )	
void pcint1Disable(uint8_t interruptNums)
{
  PCMSK1 &= ~interruptNums;  // Disable specific pins of the Pin Change Interrupt
  PCICR &= ~( _BV(PCIE1) );  // Disable Pin Change Interrupt
}
//! Attach a user function to an pin change interrupt
//  interruptNum: valid PCINT7~PCINT0
void pcint1Attach(uint8_t interruptNum, void (*userHandler)(void) )
{
		PinChange1IntFunc[interruptNum] = userHandler;
}
void pcint1Detach(uint8_t interruptNum )
{
		PinChange1IntFunc[interruptNum] = 0;
}

//! Interrupt handler for INT1
ISR(PCINT1_vect)
{
	// if a user function is defined, execute it
	uint8_t i,len;
	len = sizeof(PinChange1IntFunc) / sizeof(voidFuncPtr);

	for ( i=0;i<len;i++) {
	     if(PinChange1IntFunc[i])
		      PinChange1IntFunc[i]();
	}
}
#endif


#ifdef PCINT2_vect
void pcint2Init(void)
{	
	// detach all user functions from interrupts
	uint8_t intNum,len;
	len = sizeof(PinChange2IntFunc) / sizeof(voidFuncPtr);

	for( intNum=0; intNum < len; intNum++)
		PinChange2IntFunc[intNum]=0;
}
//! Configure pin change interrupt (PCINT23~PCINT16) trigger
// NOTE: it should be called after pcintAttach at all !!!
// interruptNums: group of PCINT23~PCINT16.
//                e.g., (_BV(PCINT18) |_BV(PCINT19)  )	
void pcint2Enable(uint8_t interruptNums)
{
	PCICR |=( _BV(PCIE2) )	;
	PCMSK2 |= interruptNums;         
}
//! Disables pin change interrupt (PCINT23~PCINT16) trigger.
// interruptNums: group of PCINT23~PCINT16.
//                e.g., (_BV(PCINT18) |_BV(PCINT19)  )	
void pcint2Disable(uint8_t interruptNums)
{
  PCMSK2 &= ~interruptNums;  // Disable specific pins of the Pin Change Interrupt
  PCICR &= ~( _BV(PCIE2) );  // Disable Pin Change Interrupt
}

//! Attach a user function to an pin change interrupt
//  interruptNum: valid PCINT7~PCINT0
void pcint2Attach(uint8_t interruptNum, void (*userHandler)(void) )
{
		PinChange2IntFunc[interruptNum] = userHandler;
}
void pcint2Detach(uint8_t interruptNum )
{
		PinChange2IntFunc[interruptNum] = 0;
}

//! Interrupt handler for INT2
ISR(PCINT2_vect)
{
	// if a user function is defined, execute it
	uint8_t i,len;
	len = sizeof(PinChange2IntFunc) / sizeof(voidFuncPtr);

	for ( i=0;i<len;i++) {
	     if(PinChange2IntFunc[i])
		      PinChange2IntFunc[i]();
	}
}
#endif

