# avrframe
avr serial framework, maintenance by multi sub makefile

1/抽取grbl中的主框架，作为AVR程序与外界通信的通用框架

2/构建AVR 程序的多模块makefile。

[Proteus仿真AVR单片机时使用外部晶振熔丝位设置](http://www.eeworld.com.cn/mcu/2015/1012/article_22884.html)

# AVR的GCC中断向量说明

This page can be quickly reached from the link: http://www.gammon.com.au/interrupts

When writing an Interrupt Service Routine (ISR):

    Keep it short
    Don't use delay ()
    Don't do serial prints
    Make variables shared with the main code volatile
    Variables shared with main code may need to be protected by "critical sections" (see below)
    Don't try to turn interrupts off or on

**Warning**: if you are not sure if interrupts are already on or not, then you need to save the current state and restore it afterwards. For example, the code from the millis() function does this:

  ```c
  unsigned long millis()
  {
    unsigned long m;
    uint8_t oldSREG = SREG;
    // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    cli();
    m = timer0_millis;
    SREG = oldSREG;
    return m;
  }
  ```

Can ISRs be interrupted?

    In short, no, not unless you want them to be.

    When an ISR is entered, interrupts are disabled. Naturally they must have been enabled in the first place, otherwise the ISR would not be entered. However to avoid having an ISR itself be interrupted, the processor turns interrupts off.

    When an ISR exits, then interrupts are enabled again. The compiler also generates code inside an ISR to save registers and status flags, so that whatever you were doing when the interrupt occurred will not be affected.

Empty interrupts

  If you merely want an interrupt to wake the processor, but not do anything in particular, you can use the EMPTY_INTERRUPT define, eg.
  `EMPTY_INTERRUPT (PCINT1_vect);`
  This simply generates a "reti" (return from interrupt) instruction. Since it doesn't try to save or restore registers this would be the fastest way to get an interrupt to wake it up.


## Timer interrupts

The "normal" Arduino IDE uses timer 0 to provide a "clock" being a number that is incremented, and adjusted, to give you a count per millisecond. You can read that by doing something like this:

    unsigned long nowMs = millis ();

You can also find a more accurate timer value by using micros (), which takes the current value from the millis counter, and adds in the current reading from timer 0 (thus correcting for "right now"). eg.

    unsigned long nowUs = micros ();

However you can make your own interrupts, say using Timer 1, like this:

```c
    const byte LED = 13;

    ISR(TIMER1_COMPA_vect)
    {
    static boolean state = false;
      state = !state;  // toggle
      digitalWrite (LED, state ? HIGH : LOW);
    }

    void setup() {
      pinMode (LED, OUTPUT);
      
      // set up Timer 1
      TCCR1A = 0;          // normal operation
      TCCR1B = bit(WGM12) | bit(CS10);   // CTC, no pre-scaling
      OCR1A =  999;       // compare A register value (1000 * clock speed)
      TIMSK1 = bit (OCIE1A);             // interrupt on Compare A Match
    }  // end of setup

    void loop() { }
```
This uses CTC (Clear Timer on Compare), so it counts up to the specified value, clears the timer, and starts again. This toggles pin D13 faster than you can see (once every 62.5 µS). (It turns on every second toggle, that is every 125 µS, giving a frequency of 1/0.000125 = 8 KHz).

Note: The counter is zero-relative. So if the counter is 1, then it actually counts two times the clock_speed/pre-scaler (0 and 1). So for an interval of 1000 slower than the internal clock, we need to set the counter to 999.

If you change the prescaler it toggles every 64 mS, slow enough for you to see it flashing. (That is, the LED lights every 128 mS, being 1/0.128 = 7.8125 Hz).

```c
    const byte LED = 13;

    ISR(TIMER1_COMPA_vect)
    {
    static boolean state = false;
      state = !state;  // toggle
      digitalWrite (LED, state ? HIGH : LOW);
    }

    void setup() {
      pinMode (LED, OUTPUT);
      
      // set up Timer 1
      TCCR1A = 0;          // normal operation
      TCCR1B = bit(WGM12) | bit(CS10) | bit (CS12);   // CTC, scale to clock / 1024
      OCR1A =  999;       // compare A register value (1000 * clock speed / 1024)
      TIMSK1 = bit (OCIE1A);             // interrupt on Compare A Match
    } // end of setup

    void loop() { }
```

And even more simply, you can output a timer result without even using interrupts:

```c
    const byte LED = 9;

    void setup() {
      pinMode (LED, OUTPUT);
      
      // set up Timer 1
      TCCR1A = bit (COM1A0);  // toggle OC1A on Compare Match
      TCCR1B = bit(WGM12) | bit(CS10) | bit (CS12);   // CTC, scale to clock / 1024
      OCR1A =  4999;       // compare A register value (5000 * clock speed / 1024)
    }  // end of setup

    void loop() { }
```

Plug an LED (and resistor in series, say 470 ohms) between D9 and Gnd, and you will see it toggle every 320 mS. (1/(16000000 / 1024) * 5000).

More information on timers [here](http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=50106)
