//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		   // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h> // include interrupt support

#include <stdio.h>
#include <util/delay.h>
#include "../serial/serial.h"
#include "../util/print.h"
#include <avr/pgmspace.h>

#include "softSerial.h" 



// SoftwareSerial mySerial(10, 11); // RX, TX
// void setup() {
//   // Open serial communications and wait for port to open:
//   Serial.begin(57600);
//   while (!Serial) {
//     ; // wait for serial port to connect. Needed for native USB port only
//   }
//   Serial.println("Goodnight moon!");

//   // set the data rate for the SoftwareSerial port
//   mySerial.begin(4800);
//   mySerial.println("Hello, world?");
// }

// void loop() { // run over and over
//   if (mySerial.available()) {
//     Serial.write(mySerial.read());
//   }
//   if (Serial.available()) {
//     mySerial.write(Serial.read());
//   }
// }



void softSerialTest(void)
{
	// 写到硬口print a little intro message so we know things are working
	printPgmString(PSTR("\r\n\n\nWelcome to the softserial library test program!\r\n"));

   long speed = 57600;
   SoftSerial *mySerial= NewSoftSerial( PCINTR19  /* RX */, PCINTR20 /* tx */, speed);

   begin(mySerial);

   while(1){
      if (available(mySerial)) {
	      printPgmString(PSTR("\r\n\n\nbb!\r\n"));
         //serial_write(read(mySerial));//todo 硬件串口写
      }

      uint8_t d= serial_read();
      //硬串口有数据读
      if(SERIAL_NO_DATA!=d) {
           write(mySerial, d);// 软串口写
      }
   }

}
