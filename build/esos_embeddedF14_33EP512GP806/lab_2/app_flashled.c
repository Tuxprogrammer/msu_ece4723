// .. "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
//    All rights reserved.
//    (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
//    (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
//    (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
//
//    Permission to use, copy, modify, and distribute this software and its
//    documentation for any purpose, without fee, and without written agreement is
//    hereby granted, provided that the above copyright notice, the following
//    two paragraphs and the authors appear in all copies of this software.
//
//    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
//    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
//    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
//    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
//    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
//    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
//    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
//
//    Please maintain this header in its entirety when copying/modifying
//    these files.
//
// *****************************
// app_flashled.c - flash an LED
// *****************************
// ESOS application program to flash an LED on RB15.  Flashing LED is generated
// by a *user task*.
//
// .. note::
//
//    Demonstrates task wait/yields and ESOS application code organization


// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need
#include    "esos.h"
#ifdef __linux
#include    "esos_pc.h"
#include    "esos_pc_stdio.h"

// INCLUDE these so that printf() and our PC hacks work
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#else
#include    "esos_pic24.h"
#include    "esos_pic24_rs232.h"
#endif

// DEFINEs go here
#ifndef __linux
#define   CONFIG_LED1()   CONFIG_RB15_AS_DIG_OUTPUT()
#define   LED1            _LATB15
#else
#define   CONFIG_LED1()   printf("Called CONFIG_LED1()\n");
uint8_t     LED1 = TRUE;      // LED1 is initially "on"
#endif

// PROTOTYPEs go here

// GLOBALs go here
//  Generally, the user-created semaphores will be defined/allocated here


#ifdef __linux
/*
 * Simulate the timer ISR found on a MCU
 *   The PC doesn't have a timer ISR, so this task will periodically
 *   call the timer services callback instead.
 *   USED ONLY FOR DEVELOPMENT AND TESTING ON PC.
 *   Real MCU hardware doesn't need this task
 */
ESOS_USER_TASK( __simulated_isr ) {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    // call the ESOS timer services callback just like a real H/W ISR would
    __esos_tmrSvcsExecute();
    ESOS_TASK_WAIT_TICKS( 1 );

  } // endof while(TRUE)
  ESOS_TASK_END();
} // end child_task
#endif

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

/*
 * An ESOS task to mimic the heartbeat LED found
 * in the PIC24 support library code used in Chapters 8-13.
 *
 * Toggle LED1, wait 250ms, repeat forever.
 *
 * \note Since this heartbeat is performed in an ESOS task,
 * a flashing LED indicates that the ESOS scheduler is still
 * running properly.  If the LED quits flashing, the ESOS
 * scheduler is no longer rotating through the runnable task
 * list.  The most likely reason is that some task has ceased
 * "yielding" the processor, and is caught in some deadlock
 * or otherwise infinite loop.
 * \hideinitializer
 */
ESOS_USER_TASK(heartbeat_LED) {
  ESOS_TASK_BEGIN();
  while (TRUE) {
    LED1 = !LED1;

#ifdef __linux
    if (LED1) {
      printf("\a");
      fflush(stdout);
    }
#endif

    ESOS_TASK_WAIT_TICKS( 500 );
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end heartbeat_LED task

/****************************************************
 *  user_init()
 ****************************************************
 */
void user_init(void) {

  // Call the hardware-provided routines to print the
  // HELLO_MSG to the screen.  Must use this call because
  // the ESOS communications subsystems is not yet fully
  // initialized, since this call is in user_init()
  //
  // In general, users should call hardware-specific
  // function like this.

  __esos_unsafe_PutString( HELLO_MSG );

#ifdef __linux
  // register our little ESOS task to mimic MCU's TIMER T1 IRQ which kicks off
  // the ESOS S/W timers when they expire
  esos_RegisterTask( __simulated_isr );
#endif

  // configure our hardware as needed by the tasks
  CONFIG_LED1();

  // user_init() should register at least one user task
  esos_RegisterTask(heartbeat_LED);

} // end user_init()
