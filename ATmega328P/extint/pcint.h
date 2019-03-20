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

enum PINCHANGE_NUM
{
    pcint23,
    pcint22,
    pcint21,
    pcint20,
    pcint19,
    pcint18,
    pcint17,
    pcint16, /* PCMSK2 */
    pcint14,
    pcint13,
    pcint12,
    pcint11,
    pcint10,
    pcint9,
    pcint8, /* PCMSK1 */
    pcint7,
    pcint6,
    pcint5,
    pcint4,
    pcint3,
    pcint2,
    pcint1,
    pcint0, /* PCMSK0 */
    PCINT_LEN
};


/*! \brief Macros for pin change external interrupts
 *
 *  These macros assigns an integer value for each pin change interrupt
 */
#define PCINTR0 0   //PORTB
#define PCINTR1 1
#define PCINTR2 2
#define PCINTR3 3
#define PCINTR4 4
#define PCINTR5 5
#define PCINTR6 6
#define PCINTR7 7
#define PCINTR8 8  //PORTC
#define PCINTR9 9
#define PCINTR10 10
#define PCINTR11 11
#define PCINTR12 12
#define PCINTR13 13
#define PCINTR14 14
#define PCINTR15 15
#define PCINTR16 16  //PORTD
#define PCINTR17 17
#define PCINTR18 18
#define PCINTR19 19
#define PCINTR20 20
#define PCINTR21 21
#define PCINTR22 22
#define PCINTR23 23
#define PCINTR_LEN 24



// functions

/*! \brief This function enables the external pin change interrupt.
 *         
 *  \param PCINT_NO	The pin change interrupt which has to be enabled. refer PCINTRx  macro define
 *  \param userHandler The pin change interrupt reltated customer func. if it is 0, it means no attatch func for the pin
 */
void enable_pcinterrupt(uint8_t PCINT_NO,void (*userHandler)(void));
/*! \brief This function disables the external pin change interrupt.
 *         
 *  \param PCINT_NO	The pin change interrupt which has to be disabled. refer PCINTRx  macro define
 */
void disable_pcinterrupt(uint8_t PCINT_NO);


#endif
