/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * revF14.h - hardware configuration and control macros using pic24lib
 */

#ifndef REVF14_H
#define REVF14_H
#ifndef __dsPIC33EP512GP806__
#define __dsPIC33EP512GP806__
#endif

#include "pic24_all.h"

// configure LEDs
#define LED1_CONFIG() CONFIG_RF4_AS_DIG_OUTPUT()
#define LED1 (_LATF4)
#define LED1_ON() (_LATF4 = 1)
#define LED1_OFF() (_LATF4 = 0)
#define LED1_HB_TOGGLE() (_LATF4 = !_LATF4)

#define LED2_CONFIG() CONFIG_RB14_AS_DIG_OUTPUT()
#define LED2 (_LATB14)
#define LED2_ON() (_LATB14 = 1)
#define LED2_OFF() (_LATB14 = 0)
#define LED2_HB_TOGGLE() (_LATB14 = !_LATB14)

#define LED3_HB_CONFIG() CONFIG_RB15_AS_DIG_OUTPUT()
#define LED3_HB (_LATB15)
#define LED3_HB_ON() (_LATB15 = 0)
#define LED3_HB_OFF() (_LATB15 = 1)
#define LED3_HB_TOGGLE() (_LATB15 = !_LATB15)

// configure switches
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

// configure RPG
#define RPG_CONFIG()                                                                                                   \
    {                                                                                                                  \
        CONFIG_RB8_AS_DIG_INPUT();                                                                                     \
        CONFIG_RB9_AS_DIG_INPUT();                                                                                     \
        ENABLE_RB8_PULLUP();                                                                                           \
        ENABLE_RB9_PULLUP();                                                                                           \
        CONFIG_INT1_TO_RP(40);                                                                                         \
        DELAY_US(1);                                                                                                   \
    }

#define RPGA (_RB8)
#define RPGB (_RB9)
#define RPGA_HIGH (_RB8 == 1)
#define RPGA_LOW (_RB8 == 0)
#define RPGB_HIGH (_RB9 == 1)
#define RPGB_LOW (_RB9 == 0)

#define POT1 (RB2_AN)
#define TEMP1 (RB3_AN)

// configure Analog Peripherals
#define ANALOG_CONFIG()                                                                                                \
    {                                                                                                                  \
        CONFIG_RB2_AS_ANALOG();                                                                                        \
        CONFIG_RB3_AS_ANALOG();                                                                                        \
    }

// The above might need to be changed.
// #define ADC_VREF_EXT_AVSS           0x2000  // A/D Voltage reference configuration Vref+ external and Vref- is AVss
// #define ADC_VREF_AVDD_EXT           0x4000  // A/D Voltage reference configuration Vref+ AVdd and Vref- external
// #define ADC_VREF_EXT_EXT            0x6000  // A/D Voltage reference configuration both Vref+ and Vref- are external
// #define ADC_VREF_AVDD_AVSS          0x8000  // A/D Voltage reference configuration Vref+ is AVdd and Vref- is AVss
#define ADC_CONFIG(pin, tad)                                                                                           \
    {                                                                                                                  \
        configADC1_ManualCH0(pin, tad, 1);                                                                             \
        AD1CON1bits.ADON = 0;                                                                                          \
        AD1CON2 = ADC_VREF_EXT_AVSS;                                                                                   \
        AD1CON1bits.ADON = 1;                                                                                          \
    }
#endif
