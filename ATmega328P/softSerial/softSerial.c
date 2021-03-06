/*
SoftSerial.cpp (formerly NewSoftSerial.cpp) - 
*/

// When set, _DEBUG co-opts pins 11 and 13 for debugging with an
// oscilloscope or logic analyzer.  Beware: it also slightly modifies
// the bit times, so don't rely on it too much at high baud rates
#define _DEBUG 1
#define _DEBUG_PIN1 PCINTR1
#define _DEBUG_PIN2 PCINTR2
//
// Includes
//
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "softSerial.h"
#include <util/delay_basic.h>
#include "../pcint/pcint.h"

#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

static void recv(SoftSerial *p);
static void setRxIntMsk(SoftSerial *p, uint8_t enable);
static uint16_t subtract_cap(uint16_t num, uint16_t sub);

//
// Interrupt handling
//

/* static */
inline void softserial_interrupt(void *p)
{
    if (NULL==p) return;

    SoftSerial *active_object=(SoftSerial *)p;
    recv(active_object);
}


uint8_t rx_pin_read(SoftSerial *p)
{
    return    *(p->p_rx->info.p_pin) & (p->p_rx->info.pin_mask);
    // return * p->_receivePortRegister & p->_receiveBitMask;
}
/* static */
inline void tunedDelay(uint16_t delay)
{
    _delay_loop_2(delay);
}

//
// The receive routine called by the interrupt handler
//
static void recv(SoftSerial *p)
{
    uint8_t d = 0;

    // If RX line is high, then we don't see any start bit
    // so interrupt is probably not for us
    if (!rx_pin_read(p))
    {
        // Disable further interrupts during reception, this prevents
        // triggering another interrupt directly after we return, which can
        // cause problems at higher baudrates.
        setRxIntMsk(p,FALSE);

        // Wait approximately 1/2 of a bit width to "center" the sample
        tunedDelay(p->_rx_delay_centering);

        // Read each of the 8 bits
        uint8_t i;
        for ( i = 8; i > 0; --i)
        {
            tunedDelay(p->_rx_delay_intrabit);
            d >>= 1;
            if (rx_pin_read(p))
                d |= 0x80;
        }

        // if buffer full, set the overflow flag and return
        uint8_t next = (p->_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
        if (next != p->_receive_buffer_head)
        {
            // save new data in buffer: tail points to where byte goes
            p->_receive_buffer[p->_receive_buffer_tail] = d; // save new byte
            p->_receive_buffer_tail = next;
        }
        else
        {
            p->_buffer_overflow = TRUE;
        }

        // skip the stop bit
        tunedDelay(p->_rx_delay_stopbit);

        // Re-enable interrupts when we're sure to be inside the stop bit
        setRxIntMsk(p,TRUE);
    }
}


static void applyDelayParam(SoftSerial *p, long speed)
{
    p->_rx_delay_centering = p->_rx_delay_intrabit = 0;
    p->_rx_delay_stopbit = p->_tx_delay = 0;

    //假设串口速率（波特率）为speed， 那一个bit的时间是在1MHz F_CPU的情况下是$1s/speed$
    // Precalculate the various delays, in number of 4-cycle delays
    uint16_t bit_delay = (F_CPU / speed) / 4;

    // 12 (gcc 4.8.2) or 13 (gcc 4.3.2) cycles from start bit to first bit,
    // 15 (gcc 4.8.2) or 16 (gcc 4.3.2) cycles between bits,
    // 12 (gcc 4.8.2) or 14 (gcc 4.3.2) cycles from last bit to stop bit
    // These are all close enough to just use 15 cycles, since the inter-bit
    // timings are the most critical (deviations stack 8 times)
    p->_tx_delay = subtract_cap(bit_delay, 15 / 4);

    {
        #if GCC_VERSION > 40800
        // Timings counted from gcc 4.8.2 output. This works up to 115200 on
        // 16Mhz and 57600 on 8Mhz.
        //
        // When the start bit occurs, there are 3 or 4 cycles before the
        // interrupt flag is set, 4 cycles before the PC is set to the right
        // interrupt vector address and the old PC is pushed on the stack,
        // and then 75 cycles of instructions (including the RJMP in the
        // ISR vector table) until the first delay. After the delay, there
        // are 17 more cycles until the pin value is read (excluding the
        // delay in the loop).
        // We want to have a total delay of 1.5 bit time. Inside the loop,
        // we already wait for 1 bit time - 23 cycles, so here we wait for
        // 0.5 bit time - (71 + 18 - 22) cycles.
        p->_rx_delay_centering = subtract_cap(bit_delay / 2, (4 + 4 + 75 + 17 - 23) / 4);

        // There are 23 cycles in each loop iteration (excluding the delay)
        p->_rx_delay_intrabit = subtract_cap(bit_delay, 23 / 4);

        // There are 37 cycles from the last bit read to the start of
        // stopbit delay and 11 cycles from the delay until the interrupt
        // mask is enabled again (which _must_ happen during the stopbit).
        // This delay aims at 3/4 of a bit time, meaning the end of the
        // delay will be at 1/4th of the stopbit. This allows some extra
        // time for ISR cleanup, which makes 115200 baud at 16Mhz work more
        // reliably
        p->_rx_delay_stopbit = subtract_cap(bit_delay * 3 / 4, (37 + 11) / 4);
        #else // Timings counted from gcc 4.3.2 output
        // Note that this code is a _lot_ slower, mostly due to bad register
        // allocation choices of gcc. This works up to 57600 on 16Mhz and
        // 38400 on 8Mhz.
        p->_rx_delay_centering = subtract_cap(bit_delay / 2, (4 + 4 + 97 + 29 - 11) / 4);
        p->_rx_delay_intrabit = subtract_cap(bit_delay, 11 / 4);
        p->_rx_delay_stopbit = subtract_cap(bit_delay * 3 / 4, (44 + 17) / 4);
        #endif

        tunedDelay(p->_tx_delay); // if we were low this establishes the end
    }

}


//
// Constructor
//
SoftSerial *NewSoftSerial(uint8_t rx /* PCINT_NO */,
                          uint8_t tx /* PCINT_NO */,
                          long speed)
{
    PinInfo txinfo;
    if (NULL == fillPinInfo(&txinfo, tx))
    {
        return NULL;
    }

    SoftSerial *p = (SoftSerial *)malloc(sizeof(SoftSerial));
    Callback *prx = register_pcinterrupt(rx, softserial_interrupt,(void*)p );

    p->_rx_delay_centering = (0);
    p->_rx_delay_intrabit = (0);
    p->_rx_delay_stopbit = (0);
    p->_tx_delay = (0);
    p->_buffer_overflow = (FALSE);

    p->p_rx = prx;

    // setTX(tx);
    // First write, then set output. If we do this the other way around,
    // the pin would be output low for a short while before switching to
    // output high. Now, it is input with pullup for a short while, which
    // is fine.
    //设置为输出，并且为1
    *(txinfo.p_ddr) |= txinfo.ddr_mask;
    *(txinfo.p_port) |= txinfo.port_mask;

    p->_transmitPortRegister = txinfo.p_port;
    p->_transmitBitMask = txinfo.port_mask;

    applyDelayParam(p, speed);

    //


    return p;
}

void begin(SoftSerial *p)
{
    setRxIntMsk(p, TRUE);

}

uint16_t subtract_cap(uint16_t num, uint16_t sub)
{
    if (num > sub)
        return num - sub;
    else
        return 1;
}


static void setRxIntMsk(SoftSerial *p, uint8_t enable)
{
    if (enable)
    {
        enable_pcinterrupt(p->p_rx);
    }
    else
    {
        disable_pcinterrupt(p->p_rx);
    }
}



void end(SoftSerial *p)
{
    setRxIntMsk(p,FALSE);
}






void set_bits_func_correct(volatile uint8_t *port, uint8_t mask)
{
    *port |= mask;
}


// Read data from buffer
int read(SoftSerial *p)
{
    // Empty buffer?
    if (p->_receive_buffer_head == p->_receive_buffer_tail)
        return -1;

    // Read from "head"
    uint8_t d = p->_receive_buffer[p->_receive_buffer_head]; // grab next byte
    p->_receive_buffer_head = (p->_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
    return d;
}

int available(SoftSerial *p)
{
    return (p->_receive_buffer_tail + _SS_MAX_RX_BUFF - p->_receive_buffer_head) % _SS_MAX_RX_BUFF;
}

size_t write(SoftSerial *p,uint8_t b)
{
    if (p->_tx_delay == 0)
    {
        return 0;
    }

    // By declaring these as local variables, the compiler will put them
    // in registers _before_ disabling interrupts and entering the
    // critical timing sections below, which makes it a lot easier to
    // verify the cycle timings
    volatile uint8_t *reg = p->_transmitPortRegister;
    uint8_t reg_mask = p->_transmitBitMask;
    uint8_t inv_mask = ~p->_transmitBitMask;
    uint8_t oldSREG = SREG;
    uint16_t delay = p->_tx_delay;


    cli(); // turn off interrupts for a clean txmit

    // Write the start bit
    *reg &= inv_mask;

    tunedDelay(delay);

    // Write each of the 8 bits
    uint8_t i;
    for ( i = 8; i > 0; --i)
    {
        if (b & 1)            // choose bit
            *reg |= reg_mask; // send 1
        else
            *reg &= inv_mask; // send 0

        tunedDelay(delay);
        b >>= 1;
    }

    // restore pin to natural state
    *reg |= reg_mask;

    SREG = oldSREG; // turn interrupts back on
    tunedDelay(p->_tx_delay);

    return 1;
}


int peek(SoftSerial *p)
{

    // Empty buffer?
    if (p->_receive_buffer_head == p->_receive_buffer_tail)
        return -1;

    // Read from "head"
    return p->_receive_buffer[p->_receive_buffer_head];
}
