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
// app_ecan_sender.c - send a message using the CANFactory
// *****************************
// ESOS application program to send a mail message using the CANFactory task.

// Includes
#include    <string.h>
#include    "esos.h"
#include    "esos_pic24.h"
#include    "esos_ecan.h"

// Defines
#define TRUE            1
#define FALSE           0

#define CONFIG_LED1()   CONFIG_RF4_AS_DIG_OUTPUT()
#define CONFIG_LED2()   CONFIG_RB14_AS_DIG_OUTPUT()
#define CONFIG_LED3()   CONFIG_RB15_AS_DIG_OUTPUT()
#define LED1            _LATF4
#define LED2            _LATB14
#define LED3            _LATB15

#define CONFIG_SW1()    {   CONFIG_RB13_AS_DIG_INPUT(); \
                            ENABLE_RB13_PULLUP(); \
                            DELAY_US( 1 ); \
                        }
#define CONFIG_SW2()    {   CONFIG_RB12_AS_DIG_INPUT(); \
                            ENABLE_RB12_PULLUP(); \
                            DELAY_US( 1 ); \
                        }
#define SW1             _RB13
#define SW2             _RB12

// Prototypes

// Globals

/************************************************************************
 * User supplied functions
 ************************************************************************
 */

/*
 * An ESOS task to mimic the heartbeat LED found
 * in the PIC24 support library code used in Chapters 8-13.
 *
 * Toggle LED3, wait 250ms, repeat forever.
 *
 * \hideinitializer
 */
ESOS_USER_TASK ( heartbeat_LED ) {
    ESOS_TASK_BEGIN();
    while (TRUE) {
        LED3 = !LED3;
        ESOS_TASK_WAIT_TICKS( 500 );
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK ( ecan_sender ) {
    uint8_t buf[2];
    
    ESOS_TASK_BEGIN();
    
    while ( TRUE ) {
        buf[0] = !SW1;
        buf[1] = !SW2;
        
        LED1 = buf[0];
        LED2 = buf[1];
        
        ESOS_ECAN_SEND( 0x7a0, buf, 2 );
        
        ESOS_TASK_WAIT_TICKS( 10 );
    }
    
    ESOS_TASK_END();
}

/****************************************************
 *  user_init()
 ****************************************************
 */
void user_init ( void ) {
    __esos_unsafe_PutString( HELLO_MSG );

    CONFIG_LED1();
    CONFIG_LED2();
    CONFIG_LED3();
    CONFIG_SW1();
    CONFIG_SW2();
    
    esos_RegisterTask( heartbeat_LED );
    esos_RegisterTask( CANFactory );
    esos_RegisterTask( ecan_sender );
}
