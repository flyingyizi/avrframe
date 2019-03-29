//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		   // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h> // include interrupt support

#include <stdio.h>
#include <util/delay.h>
#include "../serial/serial.h"
#include "../util/print.h"
#include <avr/pgmspace.h>

#include "softSerial.h" 


void softSerialTest(void)
{
	// 写到硬口print a little intro message so we know things are working
	printPgmString(PSTR("\r\n\n\nWelcome to the softserial library test program!\r\n"));

   long speed = 57600;
   SoftSerial *mySerial= NewSoftSerial( PCINTR19  /* RX */, PCINTR20 /* tx */, speed);

   begin(mySerial);

   while(1){
      if (available(mySerial)) {
	      // printPgmString(PSTR("\r\n\n\nbb!\r\n"));
         serial_write(read(mySerial));//todo 硬件串口写
      }

      uint8_t d= serial_read();
      //硬串口有数据读
      if(SERIAL_NO_DATA!=d) {
           write(mySerial, d);// 软串口写
      }
   }

}
