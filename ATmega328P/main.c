#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "serial/serial.h"

#include "util/print.h"
#include "util/report.h"
#include <avr/pgmspace.h>

// #include "pcint/pcinttest.h" 
#include "softSerial/softSerialtest.h"


// Line buffer size from the serial input stream to be executed.
// NOTE: Not a problem except for extreme cases, but the line buffer size can be too small
// and g-code blocks can get truncated. Officially, the g-code standards support up to 256
// characters. In future versions, this will be increased, when we know how much extra
// memory space we can invest into here or we re-write the g-code parser not to have this
// buffer.
#ifndef LINE_BUFFER_SIZE
#define LINE_BUFFER_SIZE 80
#endif

//TODO
// Directs and executes one line of formatted input from protocol_process. While mostly
// incoming streaming g-code blocks, this also directs and executes Grbl internal commands,
// such as settings, initiating the homing cycle, and toggling switch states.
static void protocol_execute_line(char *line) {}

// Auto-cycle start has two purposes: 1. Resumes a plan_synchronize() call from a function that
// requires the planner buffer to empty (spindle enable, dwell, etc.) 2. As a user setting that
// automatically begins the cycle when a user enters a valid motion command manually. This is
// intended as a beginners feature to help new users to understand g-code. It can be disabled
// as a beginner tool, but (1.) still operates. If disabled, the operation of cycle start is
// manually issuing a cycle start command whenever the user is ready and there is a valid motion
// command in the planner queue.
// NOTE: This function is called from the main loop, buffer sync, and mc_line() only and executes
// when one of these conditions exist respectively: There are no more blocks sent (i.e. streaming
// is finished, single commands), a command that needs to wait for the motions in the buffer to
// execute calls a buffer sync, or the planner buffer is full and ready to go.
void protocol_auto_cycle_start() { bit_true_atomic(sys_rt_exec_state, EXEC_CYCLE_START); }
//TODO
// Executes run-time commands, when required. This is called from various check points in the main
// program, primarily where there may be a while loop waiting for a buffer to clear space or any
// point where the execution time from the last check point may be more than a fraction of a second.
// This is a way to execute realtime commands asynchronously (aka multitasking) with grbl's g-code
// parsing and planning functions. This function also serves as an interface for the interrupts to
// set the system realtime flags, where only the main program handles them, removing the need to
// define more computationally-expensive volatile variables. This also provides a controlled way to
// execute certain tasks without having two or more instances of the same task, such as the planner
// recalculating the buffer upon a feedhold or override.
// NOTE: The sys_rt_exec_state variable flags are set by any process, step or serial interrupts, pinouts,
// limit switches, or the main program.
void protocol_execute_realtime() {}

static char line[LINE_BUFFER_SIZE]; // Line to be executed. Zero-terminated.

// Define different comment types for pre-parsing.
#define COMMENT_NONE 0
#define COMMENT_TYPE_PARENTHESES 1
#define COMMENT_TYPE_SEMICOLON 2

/* 
   PRIMARY LOOP:
*/
void protocol_main_loop()
{
  // ------------------------------------------------------------
  // Complete initialization procedures upon a power-up or reset.
  // ------------------------------------------------------------

  // Print welcome message
  printPgmString(PSTR("\r\nserial demo "
                      "xxx"
                      " ['$' for help]\r\n"));

  // ---------------------------------------------------------------------------------
  // Primary loop! Upon a system abort, this exits back to main() to reset the system.
  // ---------------------------------------------------------------------------------

  uint8_t comment = COMMENT_NONE;
  uint8_t char_counter = 0;
  uint8_t c;
  for (;;)
  {

    // Process one line of incoming serial data, as the data becomes available. Performs an
    // initial filtering by removing spaces and comments and capitalizing all letters.

    // NOTE: While comment, spaces, and block delete(if supported) handling should technically
    // be done in the g-code parser, doing it here helps compress the incoming data into Grbl's
    // line buffer, which is limited in size. The g-code standard actually states a line can't
    // exceed 256 characters, but the Arduino Uno does not have the memory space for this.
    // With a better processor, it would be very easy to pull this initial parsing out as a
    // seperate task to be shared by the g-code parser and Grbl's system commands.

    while ((c = serial_read()) != SERIAL_NO_DATA)
    {
      if ((c == '\n') || (c == '\r'))
      {                         // End of line reached
        line[char_counter] = 0; // Set string termination character.

        printString((line));
        //http://home.eeworld.com.cn/forum.php?mod=viewthread&tid=607191&extra=page%3D1

        protocol_execute_line(line); // Line is complete. Execute it!
        comment = COMMENT_NONE;
        char_counter = 0;
      }
      else
      {
        if (comment != COMMENT_NONE)
        {
          // Throw away all comment characters
          if (c == ')')
          {
            // End of comment. Resume line. But, not if semicolon type comment.
            if (comment == COMMENT_TYPE_PARENTHESES)
            {
              comment = COMMENT_NONE;
            }
          }
        }
        else
        {
          if (c <= ' ')
          {
            // Throw away whitepace and control characters
          }
          else if (c == '/')
          {
            // Block delete NOT SUPPORTED. Ignore character.
            // NOTE: If supported, would simply need to check the system if block delete is enabled.
          }
          else if (c == '(')
          {
            // Enable comments flag and ignore all characters until ')' or EOL.
            // NOTE: This doesn't follow the NIST definition exactly, but is good enough for now.
            // In the future, we could simply remove the items within the comments, but retain the
            // comment control characters, so that the g-code parser can error-check it.
            comment = COMMENT_TYPE_PARENTHESES;
          }
          else if (c == ';')
          {
            // NOTE: ';' comment to EOL is a LinuxCNC definition. Not NIST.
            comment = COMMENT_TYPE_SEMICOLON;

            // TODO: Install '%' feature
            // } else if (c == '%') {
            // Program start-end percent sign NOT SUPPORTED.
            // NOTE: This maybe installed to tell Grbl when a program is running vs manual input,
            // where, during a program, the system auto-cycle start will continue to execute
            // everything until the next '%' sign. This will help fix resuming issues with certain
            // functions that empty the planner buffer to execute its task on-time.
          }
          else if (char_counter >= (LINE_BUFFER_SIZE - 1))
          {
            // Detect line buffer overflow. Report error and reset line buffer.
            //TODO report_status_message(STATUS_OVERFLOW);
            comment = COMMENT_NONE;
            char_counter = 0;
          }
          else if (c >= 'a' && c <= 'z')
          { // Upcase lowercase
            line[char_counter++] = c - 'a' + 'A';
          }
          else
          {
            line[char_counter++] = c;
          }
        }
      }
    }

    // If there are no more characters in the serial read buffer to be processed and executed,
    // this indicates that g-code streaming has either filled the planner buffer or has
    // completed. In either case, auto-cycle start, if enabled, any queued moves.
    protocol_auto_cycle_start();

    protocol_execute_realtime(); // Runtime command check point.
    //TODO if (sys.abort) { return; } // Bail to main() program loop to reset system.
  }

  return; /* Never reached */
}

int main()
{
  // Initialize system upon power-up.
  serial_init(); // Setup serial baud rate and interrupts
  // ;
  // timer1Init();  // initialize the timer system
  sei();         // Enable interrupts

  softSerialTest();


  // Write your code here
  // Start main loop. Processes program inputs and executes them.
  //protocol_main_loop();
  // while (1)
  //   ;

  return 0;
}