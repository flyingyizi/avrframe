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
//*****************************************************************************

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "pcint.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "../util/print.h"

// Global variables
struct PCGLOBAL {
    Callback *b ;
    Callback *c ;
    Callback *d ;
} ;

struct PCGLOBAL myglobal = {NULL, NULL,NULL};

inline enum AVRPIN_GROUP _whichpingroup(uint8_t PCINTR_NO);

static Callback *addCallback(uint8_t PCINTR_NO, void (*handler)(void*), void *handlerParam)
{
    enum AVRPIN_GROUP gr = _whichpingroup( PCINTR_NO);
    Callback **p=NULL;
    switch (gr)
    {
        case PIN_PORTB:
            p = &(myglobal.b);
            break;
        case PIN_PORTC:
            p = &(myglobal.c);
            break;
        case PIN_PORTD:
            p = &(myglobal.d);
            break;    
        default:
            return NULL;
            break;
    }

    if(NULL ==*p )       {
        *p = (Callback *)malloc( sizeof(Callback) );          
    }

    if (NULL != *p)
    {
        (*p)->pcintr_no = PCINTR_NO;
        (*p)->func = handler; 
        (*p)->func_param = handlerParam; 

        if (NULL == fillPinInfo(&(  (*p)->info), PCINTR_NO))
        {
            return NULL;
        }
    } 

    return *p;
}

/////////////////////////////////////



inline enum AVRPIN_GROUP _whichpingroup(uint8_t PCINTR_NO)
{
    enum AVRPIN_GROUP  avr_pingroup = PIN_UNKNOWN;
    if (PCINTR_NO >= 0 && PCINTR_NO <= 7)
    {
        avr_pingroup = PIN_PORTB;
    }else if (PCINTR_NO >= 8 && PCINTR_NO <= 15)
    {
        avr_pingroup = PIN_PORTC;
    } else
    {
        avr_pingroup = PIN_PORTD;
    }
    return avr_pingroup;
}

PinInfo *fillPinInfo(PinInfo *info, uint8_t PCINT_NO)
{
    if (NULL == info)
    {
        return NULL;
    }

    uint8_t avr_pcie_no = 0; //avr 标准定义PCIEx
    uint8_t avr_pcint_no = 0; //avr 标准定义PCINTx

    uint8_t pinmask, portmask, ddrmask;
    pinmask = portmask = ddrmask = 0;
    enum AVRPIN_GROUP  avr_pingroup = _whichpingroup(PCINT_NO);

    uint8_t valid = 1;
    if (  avr_pingroup == PIN_PORTB)
    {
        avr_pcie_no = PCIE0;
        info->p_pin=&PINB;
        info->p_port=&PORTB;
        info->p_ddr=&DDRB;
        info->p_pcmsk=&PCMSK0;

        switch (PCINT_NO)
        {
        case PCINTR0:
            avr_pcint_no = PCINT0;
            pinmask = _BV(PINB0);
            ddrmask = _BV(DDB0);
            portmask = _BV(PORTB0);
            break;
        case PCINTR1:
            avr_pcint_no = PCINT1;
            pinmask = _BV(PINB1);
            ddrmask = _BV(DDB1);
            portmask = _BV(PORTB1);
            break;
        case PCINTR2:
            avr_pcint_no = PCINT2;
            pinmask = _BV(PINB2);
            ddrmask = _BV(DDB2);
            portmask = _BV(PORTB2);
            break;
        case PCINTR3:
            avr_pcint_no = PCINT3;
            pinmask = _BV(PINB3);
            ddrmask = _BV(DDB3);
            portmask = _BV(PORTB3);
            break;
        case PCINTR4:
            avr_pcint_no = PCINT4;
            pinmask = _BV(PINB4);
            ddrmask = _BV(DDB4);
            portmask = _BV(PORTB4);
            break;
        case PCINTR5:
            avr_pcint_no = PCINT5;
            pinmask = _BV(PINB5);
            ddrmask = _BV(DDB5);
            portmask = _BV(PORTB5);
            break;
        case PCINTR6:
            avr_pcint_no = PCINT6;
            pinmask = _BV(PINB6);
            ddrmask = _BV(DDB6);
            portmask = _BV(PORTB6);
            break;
        case PCINTR7:
            avr_pcint_no = PCINT7;
            pinmask = _BV(PINB7);
            ddrmask = _BV(DDB7);
            portmask = _BV(PORTB7);
            break;
        default:
            valid = 0;
            break;
        }
    }
    else if (avr_pingroup == PIN_PORTC)
    {
        avr_pcie_no = PCIE1;
        
        info->p_pin=&PINC;
        info->p_port=&PORTC;
        info->p_ddr=&DDRC;
        info->p_pcmsk=&PCMSK1;

        switch (PCINT_NO)
        {
        case PCINTR8:
            avr_pcint_no = PCINT8;
            pinmask = _BV(PINC0);
            ddrmask = _BV(DDC0);
            portmask = _BV(PORTC0);
            break;
        case PCINTR9:
            avr_pcint_no = PCINT9;
            pinmask = _BV(PINC1);
            ddrmask = _BV(DDC1);
            portmask = _BV(PORTC1);
            break;
        case PCINTR10:
            avr_pcint_no = PCINT10;
            pinmask = _BV(PINC2);
            ddrmask = _BV(DDC2);
            portmask = _BV(PORTC2);
            break;
        case PCINTR11:
            avr_pcint_no = PCINT11;
            pinmask = _BV(PINC3);
            ddrmask = _BV(DDC3);
            portmask = _BV(PORTC3);
            break;
        case PCINTR12:
            avr_pcint_no = PCINT12;
            pinmask = _BV(PINC4);
            ddrmask = _BV(DDC4);
            portmask = _BV(PORTC4);
            break;
        case PCINTR13:
            avr_pcint_no = PCINT13;
            pinmask = _BV(PINC5);
            ddrmask = _BV(DDC5);
            portmask = _BV(PORTC5);
            break;
        case PCINTR14:
            avr_pcint_no = PCINT14;
            pinmask = _BV(PINC6);
            ddrmask = _BV(DDC6);
            portmask = _BV(PORTC6);
            break;
        #ifdef PCINT15
        case PCINTR15:
            avr_pcint_no = PCINT15;
            pinmask = _BV(PINC7);
            ddrmask = _BV(DDC7);
            portmask = _BV(PORTC7);
            break;
        #endif
        default:
            valid = 0;
            break;
        }
    }
    else if (avr_pingroup == PIN_PORTD)
    {
        avr_pcie_no = PCIE2;
        info->p_pin=&PIND;
        info->p_port=&PORTD;
        info->p_ddr=&DDRD;
        info->p_pcmsk=&PCMSK2;

        switch (PCINT_NO)
        {
        case PCINTR16:
            avr_pcint_no = PCINT16;
            pinmask = _BV(PIND0);
            portmask = _BV(PORTD0);
            ddrmask = _BV(DDD0);
            break;
        case PCINTR17:
            avr_pcint_no = PCINT17;
            pinmask = _BV(PIND1);
            portmask = _BV(PORTD1);
            ddrmask = _BV(DDD1);
            break;
        case PCINTR18:
            avr_pcint_no = PCINT18;
            pinmask = _BV(PIND2);
            portmask = _BV(PORTD2);
            ddrmask = _BV(DDD2);
            break;
        case PCINTR19:
            avr_pcint_no = PCINT19;
            pinmask = _BV(PIND3);
            portmask = _BV(PORTD3);
            ddrmask = _BV(DDD3);
            break;
        case PCINTR20:
            avr_pcint_no = PCINT20;
            pinmask = _BV(PIND4);
            portmask = _BV(PORTD4);
            ddrmask = _BV(DDD4);
            break;
        case PCINTR21:
            avr_pcint_no = PCINT21;
            pinmask = _BV(PIND5);
            portmask = _BV(PORTD5);
            ddrmask = _BV(DDD5);
            break;
        case PCINTR22:
            avr_pcint_no = PCINT22;
            pinmask = _BV(PIND6);
            portmask = _BV(PORTD6);
            ddrmask = _BV(DDD6);
            break;
        case PCINTR23:
            avr_pcint_no = PCINT23;
            pinmask = _BV(PIND7);
            portmask = _BV(PORTD7);
            ddrmask = _BV(DDD7);
            break;
        default:
            valid = 0;
            break;
        }
    }

    if (valid == 0)
    {
        return NULL;
    }
    info->pin_mask = pinmask;
    info->port_mask = portmask;
    info->ddr_mask = ddrmask;
    info->avr_pingroup = avr_pingroup;
    info->avr_pcint_no = avr_pcint_no;
    info->avr_pcie_no=avr_pcie_no;

    return info;
}

/*! \brief This function register the external pin change interrupt.
 *         
 *  \param PCINT_NO	The pin change interrupt which has to be enabled. refer PCINTRx  macro define
 *  \param userHandler The pin change interrupt reltated customer func. if it is 0, it means no attatch func for the pin
 *  \param handlerParam  input param for the pin change interrupt func, if it is NULL, will not 
 *                      modify the related func's input
 */

Callback *register_pcinterrupt(uint8_t PCINT_NO, void (*userHandler)(void*),  void *handlerParam )
{
    Callback *p = NULL;
    p = addCallback(PCINT_NO, userHandler,handlerParam );
    if (NULL == p)
    {
        return NULL;
    }

    //对PIN CHANGE的端口，一律设置为pullup输入口，
    *(p->info.p_ddr) &= ~(p->info.ddr_mask); //设置为input
    *(p->info.p_port) |= (p->info.port_mask);//pullup

    return p;
}

void enable_pcinterrupt(Callback *p)
{
    if (NULL !=p) {
        PCICR |= _BV(p->info.avr_pcie_no); //e.g.  PCIE0
        *(p->info.p_pcmsk) |= _BV(p->info.avr_pcint_no);
    }
}


/*! \brief This function disables the external pin change interrupt.
 *         
 *  \param PCINT_NO	The pin change interrupt which has to be disabled.
 */
void disable_pcinterrupt(Callback *p)
{
    if (NULL != p)    {
        //cancel
        *(p->info.p_pcmsk) &= ~(_BV(p->info.avr_pcint_no));
    }
}

#if defined(PCINT0_vect)
ISR(PCINT0_vect)
{
    if (myglobal.b)
    {
        ((myglobal.b)->func)((myglobal.b)->func_param);
    }
}
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect)
{
    if (myglobal.c)
    {
        ((myglobal.c)->func)((myglobal.c)->func_param);
    }
}
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect)
{
    if (myglobal.d)
    {
        ((myglobal.d)->func)((myglobal.d)->func_param);
    }
}
#endif


// #if defined(PCINT3_vect)
// ISR(PCINT3_vect, ISR_ALIASOF(PCINT0_vect));
// #endif
