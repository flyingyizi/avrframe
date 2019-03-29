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

enum PCINR
{
    /*! \brief Macros for pin change external interrupts
 *
 *  These macros assigns an integer value for each pin change interrupt
 */
    PCINTR_UNKNOWN = -1, //PORTB
    PCINTR0 = 0,         //PORTB
    PCINTR1 = 1,
    PCINTR2 = 2,
    PCINTR3 = 3,
    PCINTR4 = 4,
    PCINTR5 = 5,
    PCINTR6 = 6,
    PCINTR7 = 7,
    PCINTR8 = 8, //PORTC
    PCINTR9 = 9,
    PCINTR10 = 10,
    PCINTR11 = 11,
    PCINTR12 = 12,
    PCINTR13 = 13,
    PCINTR14 = 14,
    PCINTR15 = 15,
    PCINTR16 = 16, //PORTD
    PCINTR17 = 17,
    PCINTR18 = 18,
    PCINTR19 = 19,
    PCINTR20 = 20,
    PCINTR21 = 21,
    PCINTR22 = 22,
    PCINTR23 = 23
};

enum AVRPIN_GROUP
{
    PIN_UNKNOWN = -1,
    PIN_PORTB = 2,
    PIN_PORTC = 3,
    PIN_PORTD = 4
};

//
typedef struct PinInfoT
{
    uint8_t avr_pcie_no;           //取值 PCIE0,PCIE1,PCIE2,
    uint8_t avr_pcint_no;          //avr 标准定义PCINTx

    enum AVRPIN_GROUP avr_pingroup; //pin change 对应的AVR标准定义PINx，取值PIN_PORTB/PIN_PORTC/PIN_PORTD
    volatile uint8_t  *p_pin;
    volatile uint8_t  *p_port;
    volatile uint8_t  *p_ddr;
    volatile uint8_t  *p_pcmsk;
    
    uint8_t pin_mask;               //PIN对应的mask e.g.  _BV(PINB0)
    uint8_t port_mask;              //PIN对应的mask e.g.  _BV(PORTB0)
    uint8_t ddr_mask;               //PIN对应的mask e.g.  _BV(DDRB0)
} PinInfo;

//自定义callback函数
typedef void (*voidFuncPtr)(void*);
typedef struct CallbackT
{
    enum PCINR pcintr_no; // 取值见PCINTRx定义, -1是invaild 初始值
    voidFuncPtr func;
    void        *func_param; //为func提供的参数

    PinInfo info;

} Callback;

// functions
PinInfo *fillPinInfo(PinInfo *info, uint8_t PCINT_NO);


/*! \brief This function enables the external pin change interrupt.
 *         
 *  \param PCINTR_NO	The pin change interrupt which has to be enabled. refer PCINTRx  macro define
 *  \param userHandler The pin change interrupt reltated customer func. if it is 0, it means no attatch func for the pin
 *  \param handlerParam  input param for the pin change interrupt func, if it is NULL, will not 
 *                      modify the related func's input
 *  NOTICE!: 考虑中断效率，PCIE0,PCIE1,PCIE2各组中，各自只支持一路，如果对某组中多个注册仅会保留最后一个。
 *           例如同时注册PCINTR0，PCINTR1，那仅仅会保持PCINTR1这后面注册的。
 */

Callback *register_pcinterrupt(uint8_t PCINTR_NO, void (*userHandler)(void*),  void *handlerParam );

inline void enable_pcinterrupt(Callback *p);
inline void disable_pcinterrupt(Callback *p);

#endif
