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
#include "esos_pic24_spi.h"

// Uncomment to enable Nibble-Wide Mode for LCD
#define ESOS_LCD44780_NIBBLE_MODE

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

#define DS1631ADDR 0x90

#define MCP4922_CS (_LATD0)
#define MCP4922_CS_CONFIG()                                                                                            \
    {                                                                                                                  \
        CONFIG_RD0_AS_DIG_OUTPUT();                                                                                    \
    }
#define MCP4922_SHDN_CONFIG()                                                                                          \
    {                                                                                                                  \
        CONFIG_RD2_AS_DIG_OUTPUT();                                                                                    \
    }
#define SLAVE_DISABLE() (_LATD0 = 1)
#define SLAVE_ENABLE() (_LATD0 = 0)
#define MCP4922_ON() (_LATD2 = 1)
#define MCP4922_OFF() (_LATD2 = 0)

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
#define ADC_CONFIG()                                                                                                   \
    {                                                                                                                  \
        /* AD1CON1 */                                                                                                  \
        AD1CON1bits.ADON = 0;                                                                                          \
        AD1CON1bits.ADSIDL = 0; /* Continue operation in Idle mode */                                                  \
        AD1CON1bits.ADDMABM = 1; /* DMA Buffers are written in order of conversion */                                  \
        AD1CON1bits.AD12B = 1; /* Enable 12 bit mode */                                                                \
        AD1CON1bits.FORM = 0; /* Integer Output Mode */                                                                \
        AD1CON1bits.SSRC = 0b111; /* Enable Auto-Conversion */                                                         \
        AD1CON1bits.SSRCG = 0; /* Sample clock source group */                                                         \
        AD1CON1bits.ASAM = 0; /* Do not auto sample */                                                                 \
        AD1CON1bits.SAMP = 0; /* Turn off Sample */                                                                    \
        AD1CON1bits.DONE = 0; /* Clear Done flag */                                                                    \
        /* AD1CON2 */                                                                                                  \
        AD1CON2bits.VCFG = 0b001; /* Use external Vref+ and internal Vref- */                                          \
        AD1CON2bits.CSCNA = 0; /* Do not scan inputs */                                                                \
        AD1CON2bits.CHPS = 0b00; /* Channel select bits */                                                             \
        AD1CON2bits.SMPI = 0b00000; /* Generate interrupt on every conversion */                                       \
        AD1CON2bits.BUFM = 0;                                                                                          \
        AD1CON2bits.ALTS = 0;                                                                                          \
        /* AD1CON3 */                                                                                                  \
        AD1CON3bits.ADRC = 1;                                                                                          \
        AD1CON3bits.SAMC = 0b11111; /* set Tad to 31 */                                                                \
        /* AD1CON4 */                                                                                                  \
        AD1CON4bits.ADDMAEN = 0;                                                                                       \
        AD1CON1bits.ADON = 1;                                                                                          \
    }

// LCD Defines
#define LCD44780_E (_LATD10)
#define LCD44780_E_CONFIG()                                                                                            \
    {                                                                                                                  \
        CONFIG_RD10_AS_DIG_OUTPUT();                                                                                   \
    }

#define LCD44780_RW (_LATD11)
#define LCD44780_RW_CONFIG()                                                                                           \
    {                                                                                                                  \
        CONFIG_RD11_AS_DIG_OUTPUT();                                                                                   \
    }

#define LCD44780_RS (_LATC12)
#define LCD44780_RS_CONFIG()                                                                                           \
    {                                                                                                                  \
        CONFIG_RC12_AS_DIG_OUTPUT();                                                                                   \
    }

#define LCD44780_D0 (_LATE0)
#define LCD44780_D0_CONFIG_INPUT()                                                                                     \
    {                                                                                                                  \
        CONFIG_RE0_AS_DIG_INPUT();                                                                                     \
        ENABLE_RE0_PULLUP();                                                                                           \
    }
#define LCD44780_D0_CONFIG_OUTPUT()                                                                                    \
    {                                                                                                                  \
        CONFIG_RE0_AS_DIG_OUTPUT();                                                                                    \
        DISABLE_RE0_PULLUP();                                                                                          \
    }

#define LCD44780_D1 (_LATE1)
#define LCD44780_D1_CONFIG_INPUT()                                                                                     \
    {                                                                                                                  \
        CONFIG_RE1_AS_DIG_INPUT();                                                                                     \
        ENABLE_RE1_PULLUP();                                                                                           \
    }
#define LCD44780_D1_CONFIG_OUTPUT()                                                                                    \
    {                                                                                                                  \
        CONFIG_RE1_AS_DIG_OUTPUT();                                                                                    \
        DISABLE_RE1_PULLUP();                                                                                          \
    }

#define LCD44780_D2 (_LATE2)
#define LCD44780_D2_CONFIG_INPUT()                                                                                     \
    {                                                                                                                  \
        CONFIG_RE2_AS_DIG_INPUT();                                                                                     \
        ENABLE_RE2_PULLUP();                                                                                           \
    }
#define LCD44780_D2_CONFIG_OUTPUT()                                                                                    \
    {                                                                                                                  \
        CONFIG_RE2_AS_DIG_OUTPUT();                                                                                    \
        DISABLE_RE2_PULLUP();                                                                                          \
    }

#define LCD44780_D3 (_LATE3)
#define LCD44780_D3_CONFIG_INPUT()                                                                                     \
    {                                                                                                                  \
        CONFIG_RE3_AS_DIG_INPUT();                                                                                     \
        ENABLE_RE3_PULLUP();                                                                                           \
    }
#define LCD44780_D3_CONFIG_OUTPUT()                                                                                    \
    {                                                                                                                  \
        CONFIG_RE3_AS_DIG_OUTPUT();                                                                                    \
        DISABLE_RE3_PULLUP();                                                                                          \
    }

#define LCD44780_D4 (_LATE4)
#define LCD44780_D4_CONFIG_INPUT()                                                                                     \
    {                                                                                                                  \
        CONFIG_RE4_AS_DIG_INPUT();                                                                                     \
        ENABLE_RE4_PULLUP();                                                                                           \
    }
#define LCD44780_D4_CONFIG_OUTPUT()                                                                                    \
    {                                                                                                                  \
        CONFIG_RE4_AS_DIG_OUTPUT();                                                                                    \
        DISABLE_RE4_PULLUP();                                                                                          \
    }

#define LCD44780_D5 (_LATE5)
#define LCD44780_D5_CONFIG_INPUT()                                                                                     \
    {                                                                                                                  \
        CONFIG_RE5_AS_DIG_INPUT();                                                                                     \
        ENABLE_RE5_PULLUP();                                                                                           \
    }
#define LCD44780_D5_CONFIG_OUTPUT()                                                                                    \
    {                                                                                                                  \
        CONFIG_RE5_AS_DIG_OUTPUT();                                                                                    \
        DISABLE_RE5_PULLUP();                                                                                          \
    }

#define LCD44780_D6 (_LATE6)
#define LCD44780_D6_CONFIG_INPUT()                                                                                     \
    {                                                                                                                  \
        CONFIG_RE6_AS_DIG_INPUT();                                                                                     \
        ENABLE_RE6_PULLUP();                                                                                           \
    }
#define LCD44780_D6_CONFIG_OUTPUT()                                                                                    \
    {                                                                                                                  \
        CONFIG_RE6_AS_DIG_OUTPUT();                                                                                    \
        DISABLE_RE6_PULLUP();                                                                                          \
    }

#define LCD44780_D7 (_LATE7)
#define LCD44780_D7_CONFIG_INPUT()                                                                                     \
    {                                                                                                                  \
        CONFIG_RE7_AS_DIG_INPUT();                                                                                     \
        ENABLE_RE7_PULLUP();                                                                                           \
    }
#define LCD44780_D7_CONFIG_OUTPUT()                                                                                    \
    {                                                                                                                  \
        CONFIG_RE7_AS_DIG_OUTPUT();                                                                                    \
        DISABLE_RE7_PULLUP();                                                                                          \
    }

#endif