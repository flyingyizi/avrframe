/*! \file extint.c \brief pin change Interrupt function library. */
//*****************************************************************************
//
// File Name	: 'pcint.c'
// Title		: pin change Interrupt function library
// Author		: flyingyizi - Copyright (C) 2002-2004
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

typedef void (*voidFuncPtr)(void);

// Global variables
// PIN CHANGE的默认初始为1状态，因此注意在代码中需要初始化PORTXn设置初始状态为pull up
volatile static uint16_t portbhistoryH = 0xFFFF;  //存放      ~ PORTD 的bit变更
volatile static uint16_t portbhistoryL = 0xFFFF;  //存放PORTC ~ PORTB 的bit变更
volatile static voidFuncPtr PinChangeIntFunc[PCINTR_LEN];

/*! \brief This function enables the external pin change interrupt.
 *         
 *  \param PCINT_NO	The pin change interrupt which has to be enabled. refer PCINTRx  macro define
 *  \param userHandler The pin change interrupt reltated customer func. if it is 0, it means no attatch func for the pin
 */
void enable_pcinterrupt(uint8_t PCINT_NO,void (*userHandler)(void))
{
    uint8_t valid=1;
    if (PCINT_NO >= 0 && PCINT_NO <= 7)
    {
        PCICR = (PCICR & (~(1 << PCIE0))) | (1 << PCIE0);
        switch (PCINT_NO)
        {
        case 0:
            PCMSK0 |= (1 << PCINT0);
            break;
        case 1:
            PCMSK0 |= (1 << PCINT1);
            break;
        case 2:
            PCMSK0 |= (1 << PCINT2);
            break;
        case 3:
            PCMSK0 |= (1 << PCINT3);
            break;
        case 4:
            PCMSK0 |= (1 << PCINT4);
            break;
        case 5:
            PCMSK0 |= (1 << PCINT5);
            break;
        case 6:
            PCMSK0 |= (1 << PCINT6);
            break;
        case 7:
            PCMSK0 |= (1 << PCINT7);
            break;
        default:
            valid=0;
            break;
        }
    }
    else if (PCINT_NO >= 8 && PCINT_NO <= 15)
    {
        PCICR = (PCICR & (~(1 << PCIE1))) | (1 << PCIE1);
        switch (PCINT_NO)
        {
        case 8:
            PCMSK1 |= (1 << PCINT8);
            break;
        case 9:
            PCMSK1 |= (1 << PCINT9);
            break;
        case 10:
            PCMSK1 |= (1 << PCINT10);
            break;
        case 11:
            PCMSK1 |= (1 << PCINT11);
            break;
        case 12:
            PCMSK1 |= (1 << PCINT12);
            break;
        case 13:
            PCMSK1 |= (1 << PCINT13);
            break;
        case 14:
            PCMSK1 |= (1 << PCINT14);
            break;
        #ifdef PCINT15
        case 15:
            PCMSK1 |= (1 << PCINT15);
            break;
        #endif
        default:
            valid=0;
            break;
        }
    }
    else
    {
        PCICR = (PCICR & (~(1 << PCIE2))) | (1 << PCIE2);
        switch (PCINT_NO)
        {
        case 16:
            PCMSK2 |= (1 << PCINT16);
            break;
        case 17:
            PCMSK2 |= (1 << PCINT17);
            break;
        case 18:
            PCMSK2 |= (1 << PCINT18);
            break;
        case 19:
            PCMSK2 |= (1 << PCINT19);
            break;
        case 20:
            PCMSK2 |= (1 << PCINT20);
            break;
        case 21:
            PCMSK2 |= (1 << PCINT21);
            break;
        case 22:
            PCMSK2 |= (1 << PCINT22);
            break;
        case 23:
            PCMSK2 |= (1 << PCINT23);
            break;
        default:
            valid=0;
            break;
        }
    }

    if (valid !=0) {
        PinChangeIntFunc[PCINT_NO]=userHandler;
    }
}

/*! \brief This function disables the external pin change interrupt.
 *         
 *  \param PCINT_NO	The pin change interrupt which has to be disabled.
 */
void disable_pcinterrupt(uint8_t PCINT_NO)
{
    uint8_t valid=1;
    switch (PCINT_NO)
    {
    case 0:
        PCMSK0 = (PCMSK0 & (~(1 << PCINT0)));
        break;
    case 1:
        PCMSK0 = (PCMSK0 & (~(1 << PCINT1)));
        break;
    case 2:
        PCMSK0 = (PCMSK0 & (~(1 << PCINT2)));
        break;
    case 3:
        PCMSK0 = (PCMSK0 & (~(1 << PCINT3)));
        break;
    case 4:
        PCMSK0 = (PCMSK0 & (~(1 << PCINT4)));
        break;
    case 5:
        PCMSK0 = (PCMSK0 & (~(1 << PCINT5)));
        break;
    case 6:
        PCMSK0 = (PCMSK0 & (~(1 << PCINT6)));
        break;
    case 7:
        PCMSK0 = (PCMSK0 & (~(1 << PCINT7)));
        break;
    case 8:
        PCMSK1 = (PCMSK1 & (~(1 << PCINT8)));
        break;
    case 9:
        PCMSK1 = (PCMSK1 & (~(1 << PCINT9)));
        break;
    case 10:
        PCMSK1 = (PCMSK1 & (~(1 << PCINT10)));
        break;
    case 11:
        PCMSK1 = (PCMSK1 & (~(1 << PCINT11)));
        break;
    case 12:
        PCMSK1 = (PCMSK1 & (~(1 << PCINT12)));
        break;
    case 13:
        PCMSK1 = (PCMSK1 & (~(1 << PCINT13)));
        break;
    case 14:
        PCMSK1 = (PCMSK1 & (~(1 << PCINT14)));
        break;
    #ifdef PCINT15
    case 15:
        PCMSK1 = (PCMSK1 & (~(1 << PCINT15)));
        break;
    #endif
    case 16:
        PCMSK2 = (PCMSK2 & (~(1 << PCINT16)));
        break;
    case 17:
        PCMSK2 = (PCMSK2 & (~(1 << PCINT17)));
        break;
    case 18:
        PCMSK2 = (PCMSK2 & (~(1 << PCINT18)));
        break;
    case 19:
        PCMSK2 = (PCMSK2 & (~(1 << PCINT19)));
        break;
    case 20:
        PCMSK2 = (PCMSK2 & (~(1 << PCINT20)));
        break;
    case 21:
        PCMSK2 = (PCMSK2 & (~(1 << PCINT21)));
        break;
    case 22:
        PCMSK2 = (PCMSK2 & (~(1 << PCINT22)));
        break;
    case 23:
        PCMSK2 = (PCMSK2 & (~(1 << PCINT23)));
        break;
    default:
        valid=0;
        break;
    }

    //清除可能挂载过的自定义函数
    if (valid !=0) {
        PinChangeIntFunc[PCINT_NO]=0;
    }

    if (PCMSK0 == 0x00)
    {
        PCICR = (PCICR & (~(1 << PCIE0)));
    }
    else if (PCMSK1 == 0x00)
    {
        PCICR = (PCICR & (~(1 << PCIE1)));
    }
    else if (PCMSK2 == 0x00)
    {
        PCICR = (PCICR & (~(1 << PCIE2)));
    }
}


#if defined(PCINT0_vect)
ISR(PCINT0_vect)
{
    uint8_t  diff=0;
    uint16_t changedbitsL=0,changedbitsH=0 ; //设立一个无符号16位整数来记录寄存器的变化

    //每个port存储8位
    diff = PINB ^ (portbhistoryL & 0xff);
    changedbitsL = diff;
    diff = PINC ^ ((portbhistoryL & 0xff00)>> 8) ;
    changedbitsL |= diff<<8;
    diff = PIND ^ (portbhistoryH & 0xff) ;
    changedbitsH = diff;

    //更新portbhistory
    portbhistoryL = (PINC<<8) |(PINB) ;
    portbhistoryH = PIND ;

    // if a user function is defined, execute it
    #define bit(n) (1 << (n)) 
    #define bit_istrue(x,mask) ((x & mask) != 0)
    #define min(a,b) (((a) < (b)) ? (a) : (b))

    uint8_t i, len;
    //最多记录16+16 bit变化
    if (changedbitsL !=0 ) {
        len = min(sizeof(PinChangeIntFunc) / sizeof(voidFuncPtr), 16 );
        for (i = 0; i < len; i++)
        {
            if ( bit_istrue(changedbitsL, bit(i) )  && PinChangeIntFunc[i]  )
                PinChangeIntFunc[i]();
        }
    }
    if (changedbitsH !=0) {
        len = (sizeof(PinChangeIntFunc) / sizeof(voidFuncPtr) );
        for (i = 16; i < len; i++)
        {
            if ( bit_istrue(changedbitsH, bit(i-16) )  && PinChangeIntFunc[i]  )
                PinChangeIntFunc[i]();
        }
    }

}
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#if defined(PCINT3_vect)
ISR(PCINT3_vect, ISR_ALIASOF(PCINT0_vect));
#endif
