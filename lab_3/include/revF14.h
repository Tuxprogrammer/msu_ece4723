/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * revF14.h - hardware configuration and control macros using pic24lib
 */

/* HARDWARE SYSTEM RESOURCES USED BY THIS CONFIGURATION:
 * Pin RF4  [RP100] (LED1)
 * Pin RB14 [RPI46] (LED2)
 * Pin RB15 [RPI47] (LED3_HB)
 * Pin RB13 [RPI45] (SW1)
 * Pin RB12 [RPI44] (SW2)
 * Pin RC15         (SW3)
 * Pin RB8  [RPI40] (RPGA)
 * Pin RB9  [RPI41] (RPGB)
 *
 * Configuration Not Required:
 * Pin RB10 [RPI43] (ICSP RX)
 * Pin RF0  [RP96]  (ICSP TX)
 * Pin RB11 [RPI43] (N/C)
 */

#ifndef REVF14_H
#define REVF14_H

#ifndef __dsPIC33EP512GP806__
#define __dsPIC33EP512GP806__
#endif

#include "pic24_all.h"

// Configure LEDs
#define LED1_CONFIG() CONFIG_RF4_AS_DIG_OUTPUT()
#define LED1 (_LATF4)
#define LED1_ON() (_LATF4 = 1)
#define LED1_OFF() (_LATF4 = 0)
#define LED1_TOGGLE() (_LATF4 = !_LATF4)

#define LED2_CONFIG() CONFIG_RB14_AS_DIG_OUTPUT()
#define LED2 (_LATB14)
#define LED2_ON() (_LATB14 = 1)
#define LED2_OFF() (_LATB14 = 0)
#define LED2_TOGGLE() (_LATB14 = !_LATB14)

#define LED3_HB_CONFIG() CONFIG_RB15_AS_DIG_OUTPUT()
#define LED3_HB (_LATB15)
#define LED3_HB_ON() (_LATB15 = 0)
#define LED3_HB_OFF() (_LATB15 = 1)
#define LED3_HB_TOGGLE() (_LATB15 = !_LATB15)

// Configure Switches
#define SW1_CONFIG()                                                                                                   \
    {                                                                                                                  \
        CONFIG_RB13_AS_DIG_INPUT();                                                                                    \
        ENABLE_RB13_PULLUP();                                                                                          \
        DELAY_US(1);                                                                                                   \
    }

#define SW1 (_RB13)
#define SW1_PRESSED (_RB13 == 0)
#define SW1_RELEASED (_RB13 == 1)

#define SW2_CONFIG()                                                                                                   \
    {                                                                                                                  \
        CONFIG_RB12_AS_DIG_INPUT();                                                                                    \
        ENABLE_RB12_PULLUP();                                                                                          \
        DELAY_US(1);                                                                                                   \
    }

#define SW2 (_RB12)
#define SW2_PRESSED (_RB12 == 0)
#define SW2_RELEASED (_RB12 == 1)

#define SW3_CONFIG()                                                                                                   \
    {                                                                                                                  \
        CONFIG_RC15_AS_DIG_INPUT();                                                                                    \
        ENABLE_RC15_PULLUP();                                                                                          \
        DELAY_US(1);                                                                                                   \
    }

#define SW3 (_RB15)
#define SW3_PRESSED (_RC15 == 0)
#define SW3_RELEASED (_RC15 == 1)

// Configure RPG
#define RPG_CONFIG()                                                                                                   \
    {                                                                                                                  \
        CONFIG_RB8_AS_DIG_INPUT();                                                                                     \
        CONFIG_RB9_AS_DIG_INPUT();                                                                                     \
        ENABLE_RB8_PULLUP();                                                                                           \
        ENABLE_RB9_PULLUP();                                                                                           \
        DELAY_US(1);                                                                                                   \
    }

#define RPGA (_RB8)
#define RPGB (_RB9)

#endif
