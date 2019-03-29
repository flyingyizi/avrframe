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
#include <stdio.h>
#include <avr/interrupt.h>

#include "pcint.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "../util/print.h"

// Global variables
Callback *g_callbacks = NULL;
uint8_t g_callbacks_len = 0;
//记录注册了的PIN change
volatile static uint8_t g_mask_pinb = 0x0; //存放PinB中注册了pin change的端口
volatile static uint8_t g_mask_pinc = 0x0; //存放PinC中注册了pin change的端口
volatile static uint8_t g_mask_pind = 0x0; //存放PinD中注册了pin change的端口

// PIN CHANGE的默认初始为1状态，因此注意在代码中需要初始化PORTXn设置初始状态为pull up

volatile static uint8_t g_historyb = 0xFF; //存放PinB 的bit变更
volatile static uint8_t g_historyc = 0xFF; //存放PinC 的bit变更
volatile static uint8_t g_historyd = 0xFF; //存放PinD 的bit变更

Callback *findCallback(int PCINT_NO)
{
    int i;
    for (i = 0; i < g_callbacks_len; i++)
    {
        if ( PCINT_NO== g_callbacks[i].pcintr_no)
        {
            return  &(g_callbacks[i]);
        }
    }
    return NULL;
}

static void initCallback(Callback *p) {
    if(NULL==p) {
        return;
    }
    memset(p, 0, sizeof(Callback ) );

    p->pcintr_no = PCINTR_UNKNOWN;
    p->func = NULL;
    p->func_param = NULL;

    p->info.avr_pingroup = PIN_UNKNOWN;
    p->info.avr_pcmsk = PCMSK_UNKNOWN;
    p->info.p_pin=NULL;
    p->info.p_port=NULL;
    p->info.p_ddr=NULL;
}

/*! \brief This function add a pin change interrupt.
 *         
 *  \param handler  related pin change interrupt func, if it is NULL, will not 
 *                      modify the related pin change interrupt func
 *  \param handlerParam  input param for the pin change interrupt func, if it is NULL, will not 
 *                      modify the related func's input
 */
static Callback *addCallback(uint8_t PCINT_NO, void (*handler)(void*), void *handlerParam)
{
    //尝试在历史中找
    Callback *p = findCallback(PCINT_NO);

    //新分配重试
    if (NULL == p)
    {
        //为g_callbacks增加空间,增长幅度固定为1
        Callback *temp = (Callback *)realloc(g_callbacks, sizeof(Callback) *(1+g_callbacks_len));        
        if (temp == g_callbacks) {
            //说明原址增加成功
            p = &(g_callbacks[g_callbacks_len]);
            initCallback( p   );
            g_callbacks_len++;
        } else if (temp != g_callbacks) {
            memcpy(temp, g_callbacks, g_callbacks_len * sizeof(Callback) );
            p = &(temp[g_callbacks_len]) ;
            initCallback( p  );
            g_callbacks_len++;
            g_callbacks = temp;
        } 
    }

    if (NULL != p)
    {
        p->pcintr_no = PCINT_NO;
        p->func = handler; 
        p->func_param = handlerParam; 

        if (NULL == fillPinInfo(&(p->info), PCINT_NO))
        {
            return NULL;
        }
    } 

    return p;
}

/////////////////////////////////////



#define bit(n) (1 << (n))
#define bit_false(x, mask) (x) &= ~(mask)
#define bit_istrue(x, mask) ((x & mask) != 0)
#define min(a, b) (((a) < (b)) ? (a) : (b))

PinInfo *fillPinInfo(PinInfo *info, uint8_t PCINT_NO)
{
    if (NULL == info)
    {
        return NULL;
    }

    uint8_t avr_pcie_no = 0; //avr 标准定义PCIEx
    enum AVRPCMSK_GROUP pcmsk = PCMSK_UNKNOWN;
    uint8_t avr_pcint_no = 0; //avr 标准定义PCINTx

    enum AVRPIN_GROUP  avr_pingroup = PIN_UNKNOWN;
    uint8_t pinmask, portmask, ddrmask;
    pinmask = portmask = ddrmask = 0;

    uint8_t valid = 1;
    if (PCINT_NO >= 0 && PCINT_NO <= 7)
    {
        avr_pcie_no = PCIE0;
        avr_pingroup = PIN_PORTB;
        info->p_pin=&PINB;
        info->p_port=&PORTB;
        info->p_ddr=&DDRB;

        pcmsk = enumPCMSK0;

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
    else if (PCINT_NO >= 8 && PCINT_NO <= 15)
    {
        avr_pcie_no = PCIE1;
        avr_pingroup = PIN_PORTC;
        info->p_pin=&PINC;
        info->p_port=&PORTC;
        info->p_ddr=&DDRC;

        pcmsk = enumPCMSK1;

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
    else
    {
        avr_pcie_no = PCIE2;
        avr_pingroup = PIN_PORTD;
        info->p_pin=&PIND;
        info->p_port=&PORTD;
        info->p_ddr=&DDRD;

        pcmsk = enumPCMSK2;
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
    info->avr_pcmsk = pcmsk;
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

    //对PIN CHANGE的端口，一律设置为pullup输入口，因为识别状态变化的初始态值设为了1
    //更新全局mask, 设置对应PIN为input pullup
    *(p->info.p_ddr) &= ~(p->info.ddr_mask); //设置为input
    *(p->info.p_port) |= (p->info.port_mask);//pullup

    return p;
}

Callback *enable_pcinterrupt(Callback *p)
{
    //对PIN CHANGE的端口，一律设置为pullup输入口，因为识别状态变化的初始态值设为了1
    //更新全局mask, 设置对应PIN为input pullup
    if (PIN_UNKNOWN == p->info.avr_pingroup) {
        return NULL;
    }
    switch (p->info.avr_pingroup)
    {
    case PIN_PORTB:
        g_mask_pinb |= p->info.pin_mask;
        break;
    case PIN_PORTC:
        g_mask_pinc |= p->info.pin_mask;
        break;
    case PIN_PORTD:
        g_mask_pind |= p->info.pin_mask;
        break;
    default:
        break;
    }

    //
    PCICR |= _BV(p->info.avr_pcie_no); //e.g.  PCIE0
    switch (p->info.avr_pcmsk)
    {
    case enumPCMSK0:
        PCMSK0 |= _BV(p->info.avr_pcint_no);
        break;
    case enumPCMSK1:
        PCMSK1 |= _BV(p->info.avr_pcint_no);
        break;
    case enumPCMSK2:
        PCMSK2 |= _BV(p->info.avr_pcint_no);
        break;
    default:
        break;
    }

    return p;
}


/*! \brief This function disables the external pin change interrupt.
 *         
 *  \param PCINT_NO	The pin change interrupt which has to be disabled.
 */
void disable_pcinterrupt(Callback *p)
{
    if (NULL == p)
    {
        return;
    }

    //cancel
    switch (p->info.avr_pcmsk)
    {
    case enumPCMSK0:
        PCMSK0 &= ~(_BV(p->info.avr_pcint_no));
        break;
    case enumPCMSK1:
        PCMSK1 &= ~(_BV(p->info.avr_pcint_no));
        break;
    case enumPCMSK2:
        PCMSK2 &= ~(_BV(p->info.avr_pcint_no));
        break;
    default:
        break;
    }

    //更新全局mask,将对应bit clear
    switch (p->info.avr_pingroup)
    {
    case PIN_PORTB:
        g_mask_pinb &= ~(p->info.pin_mask);
        break;
    case PIN_PORTC:
        g_mask_pinc &= ~(p->info.pin_mask);
        break;
    case PIN_PORTD:
        g_mask_pind &= ~(p->info.pin_mask);
        break;
    default:
        break;
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
    uint8_t diffb = 0, diffc = 0, diffd = 0;
    if (g_callbacks)
    {
        //每个port存储8位
        if(0x00 !=g_mask_pinb) {
            diffb = (PINB ^ g_historyb) & g_mask_pinb;
            //更新portbhistory
            g_historyb = PINB;
        }
        if(0x00 !=g_mask_pinc) {
            diffc = (PINC ^ g_historyc) & g_mask_pinc;
            g_historyc = PINC;
        }
        if(0x00 !=g_mask_pind) {
            diffd = (PIND ^ g_historyd) & g_mask_pind;
            g_historyd = PIND;
        }    

        // debugPrintfSerial("enter the main entry: diffb %d,diffc %d,diffd %d; \r\n", 
        //    diffb, diffc,diffd     );
        // if a user function is defined, execute it
        uint8_t i;
        volatile Callback *p = NULL;
        for (i = 0; i < g_callbacks_len; i++)
        {
            p = &(g_callbacks[i]);

            if (PCINTR_UNKNOWN == p->pcintr_no)
            {
                continue;
            }

            switch (p->info.avr_pingroup)
            {
            case PIN_PORTB:
                if (diffb && bit_istrue(diffb, (p->info.pin_mask)) && (p->func) != NULL)
                {
                    (p->func)(p->func_param);
                }
                break;
            case PIN_PORTC:
                if (diffc && bit_istrue(diffc, (p->info.pin_mask)) && (p->func) != NULL)
                {
                    (p->func)(p->func_param);
                }
                break;
            case PIN_PORTD:
                if (diffd && bit_istrue(diffd, (p->info.pin_mask)) && (p->func) != NULL)
                {
                    (p->func)(p->func_param);
                }
                break;
            default:
                break;
            }
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
