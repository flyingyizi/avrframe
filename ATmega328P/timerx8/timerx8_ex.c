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
//__AVR_ATmega16__
//__AVR_ATmega328P__

//http://maxembedded.com/2011/08/avr-timers-pwm-mode-part-i/comment-page-2/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "timerx8.h"

#define cbi(reg, bit) (reg &= ~(BV(bit)))
#define sbi(reg, bit) (reg |= (BV(bit)))

void timer0ClockSel(PRESCALER_5 prescale) {
  /**
   * Table 15-5. Clock Select Bit Description
   * CS12 CS11 CS10 Description
   * 0    0    0    No clock source (Timer/Counter stopped).
   * 0    0    1    clkI/O/1 (No prescaling)
   * 0    1    0    clkI/O/8 (From prescaler)
   * 0    1    1    clkI/O/64 (From prescaler)
   * 1    0    0    clkI/O/256 (From prescaler)
   * 1    0    1    clkI/O/1024 (From prescaler)
   * 1    1    0    External clock source on T1 pin. Clock on falling edge.
   * 1    1    1    External clock source on T1 pin. Clock on rising edge.
   * */
// set prescaler on timer 0
#if defined(TCCR0B)  // e.g. atmega328p
  TCCR0B = ((TCCR0B & ~TIMER_PRESCALER5_MASK) | prescale);
#elif defined(TCCR0)  // e.g. atmega16
  TCCR0 = ((TCCR0 & ~TIMER_PRESCALER5_MASK) | prescale);
#endif
  // atmega16
  // #define TCCR0   _SFR_IO8(0x33)
  // #define CS00    0
  // #define CS01    1
  // #define CS02    2
  // #define WGM01   3
  // #define COM00   4
  // #define COM01   5
  // #define WGM00   6
  // #define FOC0    7
};

void timer0Mode(uint8_t mode) {
  /**
   * atmega16
   * Table 15-4.  
   * Mode WGM01 WGM00 工作模式      TOP  OCR0的更新时间  TOV0的置位时刻
   * 0    0     0     普通         0xFF 立即更新        MAX
   * 1    0     1     PWM相位修正  0xFF TOP            BOTTOM
   * 2    1     0     CTC          OCR0 立即更新        MAX
   * 3    1     1     快速PWM       0xFF TOP           MAX
   * */

  /**
   *atmega328p
   * Table 14-8. Waveform Generation Mode Bit Description
   *Mode WGM02 WGM01 WGM00 ModeOfOperation TOP  Update ofOCRx at  TOV FlagSetOn
   *0    0     0     0     Normal          0xFF Immediate         MAX
   *1    0     0     1     PWMPhaseCorrect 0xFF TOP               BOTTOM
   *2    0     1     0     CTC             OCRA Immediate         MAX
   *3    0     1     1     FastPWM         0xFF BOTTOM            MAX
   *4    1     0     0     Reserved        –    –                 –
   *5    1     0     1     PWMPhaseCorrect OCRA TOP               BOTTOM
   *6    1     1     0     Reserved        –    –                 –
   *7    1     1     1     Fast PWM        OCRA BOTTOM            TOP
   */
  switch (mode) {
    case 0:
      #if defined(TCCR0B) && defined(TCCR0A)  // e.g. atmega328p
      cbi(TCCR1B, WGM02), cbi(TCCR0A, WGM01),cbi(TCCR0A, WGM00);
      #elif defined(TCCR0)  // e.g. atmega16
      cbi(TCCR0, WGM01), cbi(TCCR0, WGM00);
      #endif
      break;
    case 1:
      #if defined(TCCR0B) && defined(TCCR0A)  // e.g. atmega328p
      cbi(TCCR1B, WGM02), cbi(TCCR0A, WGM01),sbi(TCCR0A, WGM00);
      #elif defined(TCCR0)  // e.g. atmega16
      cbi(TCCR0, WGM01), sbi(TCCR0, WGM00);
      #endif
      break;
    case 2:
      #if defined(TCCR0B) && defined(TCCR0A)  // e.g. atmega328p
      cbi(TCCR1B, WGM02), sbi(TCCR0A, WGM01),cbi(TCCR0A, WGM00);
      #elif defined(TCCR0)  // e.g. atmega16
      sbi(TCCR0, WGM01), cbi(TCCR0, WGM00);
      #endif
      break;
    case 3:
      #if defined(TCCR0B) && defined(TCCR0A)  // e.g. atmega328p
      cbi(TCCR1B, WGM02), sbi(TCCR0A, WGM01),sbi(TCCR0A, WGM00);
      #elif defined(TCCR0)  // e.g. atmega16
      sbi(TCCR0, WGM01), sbi(TCCR0, WGM00);
      #endif
      break;
    #if defined(TCCR0B) && defined(TCCR0A)  // e.g. atmega328p
    case 4:
      break;
	case 5:
      sbi(TCCR1B, WGM02), cbi(TCCR0A, WGM01),sbi(TCCR0A, WGM00);
      break;
	case 7:
      sbi(TCCR1B, WGM02), sbi(TCCR0A, WGM01),sbi(TCCR0A, WGM00);
	  break;
    #endif
    default:
      break;
  }


}

// // Program ROM constants
// // the prescale division values stored in order of timer control register
// index
// // STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
// const unsigned short __attribute__((progmem)) TimerPrescaleFactor[] = {0, 1,
// 8, 64, 256, 1024};
// // the prescale division values stored in order of timer control register
// index
// // STOP, CLK, CLK/8, CLK/32, CLK/64, CLK/128, CLK/256, CLK/1024
// const unsigned short __attribute__((progmem)) TimerRTCPrescaleFactor[] = {0,
// 1, 8, 32, 64, 128, 256, 1024};

/**
 * Table 15-4.
 * Mode Timer/Counter Mode of Operation TOP    UpdateofOCR1xat  TOV1 FlagSet on
 * 0    Normal                          0xFFFF Immediate        MAX
 * 1    PWM, Phase Correct, 8-bit       0x00FF TOP              BOTTOM
 * 2    PWM, Phase Correct, 9-bit       0x01FF TOP              BOTTOM
 * 3    PWM, Phase Correct, 10-bit      0x03FF TOP              BOTTOM
 * 4    CTC                             OCR1A  Immediate        MAX
 * 5    Fast PWM, 8-bit                 0x00FF BOTTOM           TOP
 * 6    Fast PWM, 9-bit                 0x01FF BOTTOM           TOP
 * 7    Fast PWM, 10-bit                0x03FF BOTTOM           TOP
 * 8    PWM,Phase and Frequency Correct ICR1   BOTTOM           BOTTOM
 * 9    PWM,Phase and FrequencyCorrect  OCR1A  BOTTOM           BOTTOM
 * 10   PWM, Phase Correct              ICR1   TOP              BOTTOM
 * 11   PWM, Phase Correct              OCR1A  TOP              BOTTOM
 * 12   CTC                             ICR1   Immediate        MAX
 * 13   (Reserved)                      –      –                –
 * 14   Fast PWM                        ICR1   BOTTOM           TOP
 * 15   Fast PWM                        OCR1A  BOTTOM           TOP
 * **/
void timer1Mode(uint8_t mode) {
  /**
   * Table 15-4.
   * Mode WGM13 WGM12  WGM11 WGM10
   * 0    0     0      0     0
   * 1    0     0      0     1
   * 2    0     0      1     0
   * 3    0     0      1     1
   * 4    0     1      0     0
   * 5    0     1      0     1
   * 6    0     1      1     0
   * 7    0     1      1     1
   * 8    1     0      0     0
   * 9    1     0      0     1
   * 10   1     0      1     0
   * 11   1     0      1     1
   * 12   1     1      0     0
   * 13   1     1      0     1
   * 14   1     1      1     0
   * 15   1     1      1     1
   * */
  switch (mode) {
    case 0:
      cbi(TCCR1B, WGM13), cbi(TCCR1B, WGM12), cbi(TCCR1A, WGM11),
          cbi(TCCR1A, WGM10);
      break;
    case 1:
      sbi(TCCR1B, WGM13), cbi(TCCR1B, WGM12), cbi(TCCR1A, WGM11),
          sbi(TCCR1A, WGM10);
      break;
    case 2:
      cbi(TCCR1B, WGM13), cbi(TCCR1B, WGM12), sbi(TCCR1A, WGM11),
          cbi(TCCR1A, WGM10);
      break;
    case 3:
      cbi(TCCR1B, WGM13), cbi(TCCR1B, WGM12), sbi(TCCR1A, WGM11),
          sbi(TCCR1A, WGM10);
      break;
    case 4:
      cbi(TCCR1B, WGM13), sbi(TCCR1B, WGM12), cbi(TCCR1A, WGM11),
          cbi(TCCR1A, WGM10);
      break;
    case 5:
      cbi(TCCR1B, WGM13), sbi(TCCR1B, WGM12), cbi(TCCR1A, WGM11),
          sbi(TCCR1A, WGM10);
      break;
    case 6:
      cbi(TCCR1B, WGM13), sbi(TCCR1B, WGM12), sbi(TCCR1A, WGM11),
          cbi(TCCR1A, WGM10);
      break;
    case 7:
      cbi(TCCR1B, WGM13), sbi(TCCR1B, WGM12), sbi(TCCR1A, WGM11),
          sbi(TCCR1A, WGM10);
      break;
    case 8:
      sbi(TCCR1B, WGM13), cbi(TCCR1B, WGM12), cbi(TCCR1A, WGM11),
          cbi(TCCR1A, WGM10);
      break;
    case 9:
      sbi(TCCR1B, WGM13), cbi(TCCR1B, WGM12), cbi(TCCR1A, WGM11),
          sbi(TCCR1A, WGM10);
      break;
    case 10:
      sbi(TCCR1B, WGM13), cbi(TCCR1B, WGM12), sbi(TCCR1A, WGM11),
          cbi(TCCR1A, WGM10);
      break;
    case 11:
      sbi(TCCR1B, WGM13), cbi(TCCR1B, WGM12), sbi(TCCR1A, WGM11),
          sbi(TCCR1A, WGM10);
      break;
    case 12:
      sbi(TCCR1B, WGM13), sbi(TCCR1B, WGM12), cbi(TCCR1A, WGM11),
          cbi(TCCR1A, WGM10);
      break;
    case 13:
      sbi(TCCR1B, WGM13), sbi(TCCR1B, WGM12), cbi(TCCR1A, WGM11),
          sbi(TCCR1A, WGM10);
      break;
    case 14:
      sbi(TCCR1B, WGM13), sbi(TCCR1B, WGM12), sbi(TCCR1A, WGM11),
          cbi(TCCR1A, WGM10);
      break;
    case 15:
      sbi(TCCR1B, WGM13), sbi(TCCR1B, WGM12), sbi(TCCR1A, WGM11),
          sbi(TCCR1A, WGM10);
      break;
    default:
      break;
  }
}

void timer1ClockSel(PRESCALER_5 prescale) {
  // Table 15-5. Clock Select Bit Description
  // CS12 CS11 CS10 Description
  // 0    0    0    No clock source (Timer/Counter stopped).
  // 0    0    1    clkI/O/1 (No prescaling)
  // 0    1    0    clkI/O/8 (From prescaler)
  // 0    1    1    clkI/O/64 (From prescaler)
  // 1    0    0    clkI/O/256 (From prescaler)
  // 1    0    1    clkI/O/1024 (From prescaler)
  // 1    1    0    External clock source on T1 pin. Clock on falling edge.
  // 1    1    1    External clock source on T1 pin. Clock on rising edge.
  // set prescaler on timer 1
  TCCR1B = ((TCCR1B & ~TIMER_PRESCALER5_MASK) | prescale);
};

// #define TCCR1A _SFR_MEM8(0x80)
// #define WGM10 0
// #define WGM11 1
// #define COM1B0 4
// #define COM1B1 5
// #define COM1A0 6
// #define COM1A1 7

// #define TCCR1B _SFR_MEM8(0x81)
// #define CS10 0
// #define CS11 1
// #define CS12 2
// #define WGM12 3
// #define WGM13 4
// #define ICES1 6
// #define ICNC1 7
