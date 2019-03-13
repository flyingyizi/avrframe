
#include <avr/io.h>

#include "../serial/serial.h"
#include "print.h"
#include "report.h"
#include <avr/pgmspace.h>


// Prints feedback messages. This serves as a centralized method to provide additional
// user feedback for things that are not of the status/alarm message protocol. These are
// messages such as setup warnings, switch toggling, and how to exit alarms.
// NOTE: For interfaces, messages are always placed within brackets. And if silent mode
// is installed, the message number codes are less than zero.
// TODO: Install silence feedback messages option in settings
void report_feedback_message(uint8_t message_code)
{
  printPgmString(PSTR("["));
  switch(message_code) {
    case MESSAGE_CRITICAL_EVENT:
    printPgmString(PSTR("Reset to continue")); break;
    case MESSAGE_ALARM_LOCK:
    printPgmString(PSTR("'$H'|'$X' to unlock")); break;
    case MESSAGE_ALARM_UNLOCK:
    printPgmString(PSTR("Caution: Unlocked")); break;
    case MESSAGE_ENABLED:
    printPgmString(PSTR("Enabled")); break;
    case MESSAGE_DISABLED:
    printPgmString(PSTR("Disabled")); break; 
    case MESSAGE_SAFETY_DOOR_AJAR:
    printPgmString(PSTR("Check Door")); break;
    case MESSAGE_PROGRAM_END:
    printPgmString(PSTR("Pgm End")); break;
    case MESSAGE_RESTORE_DEFAULTS:
    printPgmString(PSTR("Restoring defaults")); break;
  }
  printPgmString(PSTR("]\r\n"));
}
