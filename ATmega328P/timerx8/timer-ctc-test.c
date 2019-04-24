//*****************************************************************************
// File Name	: timertest.c
//
// Title		: example usage of timer library functions
// Revision		: 1.0
// Notes		:
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 30-Apr-2003	pstang		Created the program
//*****************************************************************************
//修改为适用于atmega328p

//----- Include Files ---------------------------------------------------------
#include <avr/interrupt.h>  // include interrupt support
#include <avr/io.h>  // include I/O definitions (port names, pin names, etc)
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "../serial/serial.h"
#include "../util/print.h"

#include "timerx8.h"  // include timer function library (timing, PWM, etc)

#define sbi(reg, bit) (reg |= (_BV(bit)))
#define cbi(reg, bit) (reg &= ~(_BV(bit)))

// example
// int main()
// {
//   // Initialize system upon power-up.
//   serial_init(); // Setup serial baud rate and interrupts
//   // ;
//   timer1OVFInit(0);  // initialize the timer system
//   sei();         // Enable interrupts

//   timer1Test();

//   while (1)
//     ;

//   return 0;
// }

void timer1Test(void) {
  // print a little intro message so we know things are working
  printPgmString(
      PSTR("\r\n\n\nWelcome to the timer library test program!\r\n"));

  // // example: wait for 1/2 of a second, or 500ms
  // printPgmString(PSTR("\r\nTest of timerPause() function\r\n"));
  // printPgmString(PSTR("Here comes a 1/2-second delay...\r\n"));
  // timerPause(500); //
  // 如果需要非常精确的延时，那使用利用timer0的timerPause(500);
  // printPgmString(PSTR("Done!\r\n"));

  // here's an example of using the timer library to do
  // pulse-width modulation or PWM.  PWM signals can be created on
  // any output compare (OCx) pin.  See your processor's data sheet
  // for more information on which I/O pins have output compare
  // capability.
  printPgmString(PSTR("\r\nTest of timer1 PWM output\r\n"));

  // set the OC1x port pins to output
  // We need to do this so we can see and use the PWM signal
  // ** these settings are correct for most processors, but not for all
  DDRB |= _BV(DDB1);
  DDRB |= _BV(DDB2);

  // initialize timer1 for PWM output
  // - you may use 8,9, or 10 bit PWM resolution
  printPgmString(PSTR("Initializing timer1 for PWM\r\n"));
  timer1PWMInit(8);

  // turn on the channel A PWM output of timer1
  // - this signal will come out on the OC1A I/O pin
  printPgmString(PSTR("Turning on timer1 channel A PWM output\r\n"));
  timer1PWMAOn();

  // set the duty cycle of the channel A output
  // - let's try 25% duty, or 256*25% = 64
  printPgmString(PSTR("Setting duty cycle to 25%%\r\n"));
  timer1PWMASet(64);

  // turn on channel B and set it to 75% duty cycle
  printPgmString(PSTR("Turning on channel B too, with 75%% duty\r\n"));
  timer1PWMBOn();
  timer1PWMBSet(192);

  // wait for 5 seconds
  printPgmString(PSTR("Pause for 5 seconds...\r\n"));
  _delay_ms(5000);

  // now turn off all PWM on timer1
  printPgmString(PSTR("Turning off all PWM on timer1\r\n"));
  timer1PWMOff();
}

// void setup() {
//   pinMode (LED, OUTPUT);

//   // set up Timer 1
//   TCCR1A = 0;          // normal operation
//   TCCR1B = bit(WGM12) | bit(CS10);   // CTC, no pre-scaling
//   OCR1A =  999;       // compare A register value (1000 * clock speed)
//   TOP模式 TIMSK1 = bit (OCIE1A);             // interrupt on Compare A Match
// }  // end of setup

void timer0_ForceOutputCompare_test(void) {
  // 14.5.1 Force Output Compare
  // In non-PWM waveform generation modes, the match output of the comparator
  // can be forced by writing a one to the Force Output Compare (FOC0x) bit.
  // Forcing compare match will not set the OCF0x Flag or reload/clear the
  // timer, but the OC0x pin will be updated as if a real compare match had
  // occurred (the COM0x1:0 bits settings define whether the OC0x pin is set,
  // cleared or toggled).
  // set OC0A to output  (PCINT22/OC0A/AIN0) PD6
  DDRD |= _BV(DDD6);

  // OCR0A
  //从Table 14-8.知道， 只有以下几种WGM组合是non-PWM mode
  // WGM02 WGM01 WGM00
  // 0     0     0      Normal
  // 0     1     0      CTC

  timer0Mode(2 /* ctc mode */);


  OCR0A = 0xff;  // ctc mode TOP模式

  // 	• Bit 7 – FOC0A: Force Output Compare A
  // The FOC0A bit is only active when the WGM bits specify a non-PWM mode.
  // However, for ensuring compatibility with future devices, this bit must be
  // set to zero when TCCR0B is written when operating in PWM mode. When writing
  // a logical one to the FOC0A bit, an immediate Compare Match is forced on the
  // Waveform Generation unit. The OC0A output is changed according to its
  // COM0A1:0 bits setting.

  // COM0A1 COM0A0 Description
  // 0      1      Toggle OC0A on Compare Match
  // 1      0      Clear OC0A on Compare Match
  // 1      1      Set OC0A on Compare Match

  // toggle
  TCCR0A &= ~(_BV(COM0A1));
  TCCR0A |= (_BV(COM0A0));

	timer0ClockSel(TIMER_CLK_DIV1); // set prescaler

  //作用是强加在波形发生器上一个比较匹配成功信号
  TCCR0B |= _BV(FOC0A);  // The FOC0A bit is only active when the WGM bits
                         // specify a non-PWM mode
  while(1);
}

// 测试普通CTC
void timer0_CTC_test(void) {
  // set OC0A to output  (PCINT22/OC0A/AIN0) PD6
  DDRD |= _BV(DDD6);
  // initialize timer
  TCNT0 = 0;
  // initialize compare value
  OCR0A = 0xff;

  timer0ClockSel(TIMER_CLK_STOP);  // stop
  timer0Mode(2 /* mode */);
  timer0ClockSel(TIMER_CLK_DIV1);

  // loop forever
  while (1) {
    // check whether the flag bit is set
    // if set, it means that there has been a compare match
    // and the timer has been cleared
    // use this opportunity to toggle the led
    if (TIFR0 /* TIFR */ & (1 << OCF0A))  // NOTE: '>=' used instead of '=='
    {
      PORTD ^= (1 << PORTD6);  // toggles the led
    }

    // wait! we are not done yet!
    // clear the flag bit manually since there is no ISR to execute
    // clear it by writing '1' to it (as per the datasheet)
    TIFR0 |= (1 << OCF0A);

    // yeah, now we are done!
  }
}

//测试 普通ctc，自动切换
void timer0_CTC_auto_test(void) {
  // COM0A1 COM0A0 Description
  // 0      1      Toggle OC0A on Compare Match
  // 1      0      Clear OC0A on Compare Match
  // 1      1      Set OC0A on Compare Match
  // toggle
  // TCCR0A &= ~(_BV(COM0A1));
  TCCR0A &= ~(_BV(COM0A1));
  TCCR0A |= (_BV(COM0A0));

  // set OC0A to output  (PCINT22/OC0A/AIN0) PD6
  DDRD |= _BV(DDD6);

  timer0ClockSel(TIMER_CLK_STOP);  // stop
  timer0Mode(2 /* ctc mode */);
	timer0ClockSel(TIMER_CLK_DIV1); // set prescaler
	TCNT0 = 0;					// reset TCNT0  初值
  OCR0A = 0xff;
  while(1);
}

volatile  uint8_t abc;
void __userFunc(void) {
  if(abc <=10) 
     PORTD ^= (1 << PORTD6);  // toggles the led
  else
  {
     ;
  }
  
  abc++;
}
//测试 中断ctc
void timer0_CTC_ISR_test(void) {
  // set OC0A to output  (PCINT22/OC0A/AIN0) PD6
  DDRD |= _BV(DDD6);
  timerAttach(TIMER0OUTCOMPAREA_INT, __userFunc);

  TCCR0B = ((TCCR0B & ~TIMER_PRESCALER5_MASK) | TIMER_CLK_STOP);
  // // timer0ClockSel(TIMER_CLK_STOP);  // stop
  // TCCR0A &= ~(_BV(COM0A1)), TCCR0A |= (_BV(COM0A0));

  abc = 0;

	TCNT0 = 0;					// reset TCNT0  初值
  // OCR0A = 0xff;
  TCCR0B = ((TCCR0B & ~TIMER_PRESCALER5_MASK) | TIMER_CLK_DIV1);
	// timer0ClockSel(TIMER_CLK_DIV1); // set prescaler
  cbi(TCCR1B, WGM02), sbi(TCCR0A, WGM01), cbi(TCCR0A, WGM00);
  // timer0Mode(2 /* ctc mode */);
	sbi(TIMSK0, OCIE0A);					// enable TCNT0 overflow interrupt

  while(1);
}
