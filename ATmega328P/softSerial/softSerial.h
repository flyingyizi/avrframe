
#ifndef SoftSerial_h
#define SoftSerial_h

#include <stdint.h>
#include <avr/io.h>
#include "../pcint/pcint.h"

#define TRUE 1
#define FALSE 0

#ifndef _SS_MAX_RX_BUFF
#define _SS_MAX_RX_BUFF 64 // RX buffer size
#endif

#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

typedef struct SoftSerialT 
{
  uint8_t _receivePin;
  uint8_t _receiveBitMask;
  volatile uint8_t *_receivePortRegister;
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;

  // Expressed as 4-cycle delays (must never be 0!)
  uint16_t _rx_delay_centering;
  uint16_t _rx_delay_intrabit;
  uint16_t _rx_delay_stopbit;
  uint16_t _tx_delay;

  uint16_t _buffer_overflow:1;

  // static data
  /*static*/ uint8_t _receive_buffer[_SS_MAX_RX_BUFF]; 
  /*static*/ volatile uint8_t _receive_buffer_tail;
  /*static*/ volatile uint8_t _receive_buffer_head;
}SoftSerial;

SoftSerial *NewSoftSerial(uint8_t rx /* PCINT_NO */,
                          uint8_t tx /* PCINT_NO */,
                          long speed);
void begin(SoftSerial *p);

int available(SoftSerial *p);

// Read data from buffer
int read(SoftSerial *p);
size_t write(SoftSerial *p,uint8_t b);


#endif
