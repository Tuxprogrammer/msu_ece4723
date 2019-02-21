/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * esos_f14ui.h - C header file for ESOS user-interface (UI) service
 */

/* HARDWARE SYSTEM RESOURCES USED BY THIS SERVICE:
 * Input Compare 14 ?
 * Input Compare 15 ?
 * Input Compare 16 ?
 * Output Compare 11 (LED1)
 * Output Compare 12 (LED1)
 * Output Compare 13 (LED2)
 * Output Compare 14 (LED2)
 * Output Compare 15 (LED3_HB)
 * Output Compare 16 (LED3_HB)
 * Pins RF1 [RP97], RF4 [RP100] (LED1)
 * Pins RF2 [RP98], RB14 [RPI46] (LED2)
 * Pins RF3 [RP99], RB15 [RPI47] (LED3_HB)
 */

#ifndef ESOS_UIF14_H
#define ESOS_UIF14_H

#include "esos.h"

// Constants
#define __ESOS_UIF14_DEFAULT_SWDP_PERIOD 500
#define __ESOS_UIF14_DEFAULT_RPGS_PERIOD 1000
#define __ESOS_UIF14_DEFAULT_RPGM_PERIOD 250
#define __ESOS_UIF14_DEFAULT_RPGF_PERIOD 125
#define __ESOS_UIF14_POLL_RATE 64
#define __ESOS_UIF14_RPG_TURNS_PER_REV 12

// UI Internals Structure
typedef struct _st_esos_uiF14Data {
    BOOL b_SW1Pressed;
    BOOL b_SW1DoublePressed;
    uint16_t u16_SW1DoublePressedPeriod;
    BOOL b_SW2Pressed;
    BOOL b_SW2DoublePressed;
    uint16_t u16_SW2DoublePressedPeriod;
    BOOL b_SW3Pressed;
    BOOL b_SW3DoublePressed;
    uint16_t u16_SW3DoublePressedPeriod;

    BOOL b_LED1On;
    uint16_t u16_LED1FlashPeriod;
    BOOL b_LED2On;
    uint16_t u16_LED2FlashPeriod;
    BOOL b_LED3On;
    uint16_t u16_LED3FlashPeriod;

    int16_t i16_RPGVelocity;
    int16_t i16_RPGCounter;
    int16_t i16_lastRPGCounter;
    int16_t u16_RPGSlowThreshold;
    int16_t u16_RPGMediumThreshold;
    int16_t u16_RPGFastThreshold;
} _st_esos_uiF14Data_t;

// Private Data/Functions
extern volatile _st_esos_uiF14Data_t _st_esos_uiF14Data;
ESOS_USER_TIMER(__esos_uiF14_poll);

// Private ESOS UI Macros
// Macro to setup Output Compare modules with specified remappable output pin
#define __ESOS_UIF14_CONFIG_TOGGLE_OC(Rxy_RP, OCodd, OCeven) __ESOS_UIF14__CONFIG_TOGGLE_OC(Rxy_RP, OCodd, OCeven)
#define __ESOS_UIF14__CONFIG_TOGGLE_OC(Rxy_RP, OCodd, OCeven)                                                          \
    {                                                                                                                  \
        _RP##Rxy_RP##R = _RPOUT_OC##OCeven;                                                                            \
        OC##OCodd##CON1 = OC##OCodd##CON2 = OC##OCeven##CON1 = OC##OCeven##CON2 = 0x0000;                              \
        OC##OCodd##CON1bits.TRIGMODE = OC##OCeven##CON1bits.TRIGMODE = 1;                                              \
        OC##OCodd##CON1bits.OCTSEL = OC##OCeven##CON1bits.OCTSEL = 0b111;                                              \
        OC##OCodd##CON2 = OC##OCeven##CON2 = OC_SYNCSEL_OCxRS | OC_IC32_ON;                                            \
        OC##OCodd##CON2 |= OC_TRIS_ON;                                                                                 \
    }

// Macro to setup Input Capture modules with specified remappable input pin
#define __ESOS_UIF14_CONFIG_IC(pin, ICodd, ICeven) __ESOS_UIF14__CONFIG_IC(pin, ICodd, ICeven)
#define __ESOS_UIF14__CONFIG_IC(pin, ICodd, ICeven)                                                                    \
    {                                                                                                                  \
        _IC##ICodd##R = pin; /* Configure Input Capture ICodd to pin */                                                \
        IC##ICodd##CON1bits.ICM = IC##ICeven##CON1bits.ICM = 0b000;                                                    \
        IC##ICodd##CON1bits.ICTSEL = IC##ICeven##CON1bits.ICTSEL = 0b111;                                              \
        IC##ICodd##CON2bits.IC32 = IC##ICeven##CON2bits.IC32 = 1;                                                      \
        IC##ICodd##CON2bits.ICTRIG = IC##ICeven##CON2bits.ICTRIG = 1;                                                  \
        IC##ICodd##CON2bits.SYNCSEL = IC##ICeven##CON2bits.SYNCSEL = 0b00000;                                          \
        IC##ICodd##CON1bits.ICI = 0b00;                                                                                \
        IC##ICodd##CON1bits.ICM = IC##ICeven##CON1bits.ICM = 0b001;                                                    \
    }

// Note: uses RF1 as intermediate pin because of RPI pins
#define __LED1_IMDT_PIN _RF1
#define __LED1_UI_CONFIG()                                                                                             \
    {                                                                                                                  \
        LED1_CONFIG();                                                                                                 \
        CONFIG_RF1_AS_DIG_OUTPUT(); /* Use RF1 as intermediate pin for RF4 */                                          \
        __ESOS_UIF14_CONFIG_TOGGLE_OC(RF1_RP, 11, 12); /* Configure LED Flsash Controller on RF1 using OC11/12 */      \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_CN, ESOS_USER_IRQ_LEVEL2, _CNInterrupt);                     \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_CN);                                                           \
        ENABLE_RF1_CN_INTERRUPT();                                                                                     \
    }

// Note: uses RF2 as intermediate pin because of RPI pins
#define __LED2_IMDT_PIN _RF2
#define __LED2_UI_CONFIG()                                                                                             \
    {                                                                                                                  \
        LED2_CONFIG();                                                                                                 \
        CONFIG_RF2_AS_DIG_OUTPUT(); /* Use RF2 as intermediate pin for RB14 */                                         \
        __ESOS_UIF14_CONFIG_TOGGLE_OC(RF2_RP, 13, 14); /* Configure LED Flash Controller on RF2 using OC13/14 */       \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_CN, ESOS_USER_IRQ_LEVEL2, _CNInterrupt);                     \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_CN);                                                           \
        ENABLE_RF2_CN_INTERRUPT();                                                                                     \
    }

// Note: uses RF3 as intermediate pin because of RPI pins
#define __LED3_IMDT_PIN _RF3
#define __LED3_UI_CONFIG()                                                                                             \
    {                                                                                                                  \
        LED3_HB_CONFIG();                                                                                              \
        CONFIG_RF3_AS_DIG_OUTPUT(); /* Use RF3 as intermediate pin for RB15 */                                         \
        __ESOS_UIF14_CONFIG_TOGGLE_OC(RF3_RP, 15, 16); /* Configure LED Flash Controller on RF3 using OC15/16 */       \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_CN, ESOS_USER_IRQ_LEVEL2, _CNInterrupt);                     \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_CN);                                                           \
        ENABLE_RF3_CN_INTERRUPT();                                                                                     \
    }

#define __RPGA_CLEAN_PIN _RD6
#define __RPGB_CLEAN_PIN _RD7
#define __RPG_INT_PIN _RD8
#define __RPG_TIMER_VAL (((uint32_t)IC10TMR << 16) | (uint32_t)IC9TMR)
#define __RPG_TIMER_RESET()                                                                                            \
    {                                                                                                                  \
        IC9CON1bits.ICM = IC10CON1bits.ICM = 0b000;                                                                    \
        IC9CON1bits.ICM = IC10CON1bits.ICM = 0b001;                                                                    \
    }
#define __RPG_UI_CONFIG()                                                                                              \
    {                                                                                                                  \
        RPG_CONFIG();                                                                                                  \
        CONFIG_RD6_AS_DIG_OUTPUT(); /* Use RD6 as debounced output from RPGA */                                        \
        CONFIG_RD7_AS_DIG_OUTPUT(); /* Use RD7 as debounced output from RPGB */                                        \
        CONFIG_RD8_AS_DIG_OUTPUT(); /* Use RD8 as XORed output from RPG */                                             \
        __ESOS_UIF14_CONFIG_IC(RD8_RP, 9, 10);                                                                         \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC9, ESOS_USER_IRQ_LEVEL1, _IC9Interrupt);                   \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC9);                                                          \
    }

// Note: Uses CN interrupt to debounce switch input then routes to IC1 module for press analysis
#define __SW1_CLEAN_PIN _RD1
#define __SW1_TIMER_VAL (((uint32_t)IC12TMR << 16) | (uint32_t)IC11TMR)
#define __SW1_TIMER_RESET()                                                                                            \
    {                                                                                                                  \
        IC11CON1bits.ICM = IC12CON1bits.ICM = 0b000;                                                                   \
        IC11CON1bits.ICM = IC12CON1bits.ICM = 0b001;                                                                   \
    }
#define __SW1_UI_CONFIG()                                                                                              \
    {                                                                                                                  \
        SW1_CONFIG();                                                                                                  \
        CONFIG_RD1_AS_DIG_OUTPUT(); /* Use RD1 as debounced output from SW1 */                                         \
        __ESOS_UIF14_CONFIG_IC(RD1_RP, 11, 12);                                                                        \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC11, ESOS_USER_IRQ_LEVEL1, _IC11Interrupt);                 \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC11);                                                         \
    }

// Note: Uses CN interrupt to debounce switch input then routes to IC2 module for press analysis
#define __SW2_CLEAN_PIN _RD2
#define __SW2_TIMER_VAL (((uint32_t)IC14TMR << 16) | (uint32_t)IC13TMR)
#define __SW2_TIMER_RESET()                                                                                            \
    {                                                                                                                  \
        IC13CON1bits.ICM = IC14CON1bits.ICM = 0b000;                                                                   \
        IC13CON1bits.ICM = IC14CON1bits.ICM = 0b001;                                                                   \
    }
#define __SW2_UI_CONFIG()                                                                                              \
    {                                                                                                                  \
        SW2_CONFIG();                                                                                                  \
        CONFIG_RD2_AS_DIG_OUTPUT(); /* Use RD2 as debounced output from SW2 */                                         \
        __ESOS_UIF14_CONFIG_IC(RD2_RP, 13, 14);                                                                        \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC13, ESOS_USER_IRQ_LEVEL1, _IC13Interrupt);                 \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC13);                                                         \
    }

// Note: Uses CN interrupt to debounce switch input then routes to IC3 module for press analysis
#define __SW3_CLEAN_PIN _RD3
#define __SW3_TIMER_VAL (((uint32_t)IC16TMR << 16) | (uint32_t)IC15TMR)
#define __SW3_TIMER_RESET()                                                                                            \
    {                                                                                                                  \
        IC15CON1bits.ICM = IC16CON1bits.ICM = 0b000;                                                                   \
        IC15CON1bits.ICM = IC16CON1bits.ICM = 0b001;                                                                   \
    }
#define __SW3_UI_CONFIG()                                                                                              \
    {                                                                                                                  \
        SW3_CONFIG();                                                                                                  \
        CONFIG_RD3_AS_DIG_OUTPUT(); /* Use RD3 as debounced output from SW3 */                                         \
        __ESOS_UIF14_CONFIG_IC(RD3_RP, 15, 16);                                                                        \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC15, ESOS_USER_IRQ_LEVEL1, _IC15Interrupt);                 \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC15);                                                         \
    }

// Public Function Prototypes
inline BOOL esos_uiF14_isSW1Pressed(void);
inline BOOL esos_uiF14_isSW1Released(void);
inline BOOL esos_uiF14_isSW1DoublePressed(void);
inline uint16_t esos_uiF14_getSW1DoublePressedPeriod(void);
inline void esos_uiF14_setSW1DoublePressedPeriod(uint16_t period);

inline BOOL esos_uiF14_isSW2Pressed(void);
inline BOOL esos_uiF14_isSW2Released(void);
inline BOOL esos_uiF14_isSW2DoublePressed(void);
inline uint16_t esos_uiF14_getSW2DoublePressedPeriod(void);
inline void esos_uiF14_setSW2DoublePressedPeriod(uint16_t period);

inline BOOL esos_uiF14_isSW3Pressed(void);
inline BOOL esos_uiF14_isSW3Released(void);
inline BOOL esos_uiF14_isSW3DoublePressed(void);
inline uint16_t esos_uiF14_getSW3DoublePressedPeriod(void);
inline void esos_uiF14_setSW3DoublePressedPeriod(uint16_t period);

inline BOOL esos_uiF14_getRPGA(void);
inline void esos_uiF14_setRPGA(BOOL rpg);
inline BOOL esos_uiF14_getRPGB(void);
inline void esos_uiF14_setRPGB(BOOL rpg);

inline BOOL esos_uiF14_isLED1On(void);
inline void esos_uiF14_turnLED1On(void);
inline void esos_uiF14_turnLED1Off(void);
inline void esos_uiF14_toggleLED1(void);
inline void esos_uiF14_flashLED1(uint16_t);

inline BOOL esos_uiF14_isLED2On(void);
inline void esos_uiF14_turnLED2On(void);
inline void esos_uiF14_turnLED2Off(void);
inline void esos_uiF14_toggleLED2(void);
inline void esos_uiF14_flashLED2(uint16_t);

inline BOOL esos_uiF14_isLED3On(void);
inline void esos_uiF14_turnLED3On(void);
inline void esos_uiF14_turnLED3Off(void);
inline void esos_uiF14_toggleLED3(void);
inline void esos_uiF14_flashLED3(uint16_t);

inline void esos_uiF14_turnRedLEDOn(void);
inline void esos_uiF14_turnRedLEDOff(void);
inline void esos_uiF14_turnGreenLEDOn(void);
inline void esos_uiF14_turnGreenLEDOff(void);
inline void esos_uiF14_turnYellowLEDOn(void);
inline void esos_uiF14_turnYellowLEDOff(void);

inline int16_t esos_uiF14_getRPGVelocity(void);
inline void esos_uiF14_setRPGVelocity(int16_t v);
inline BOOL esos_uiF14_isRPGTurning(void);
inline BOOL esos_uiF14_isRPGTurningSlow(void);
inline int16_t esos_uiF14_getRPGSlowThreshold(void);
inline void esos_uiF14_setRPGSlowThreshold(int16_t threshold);
inline BOOL esos_uiF14_isRPGTurningMedium(void);
inline int16_t esos_uiF14_getRPGMediumThreshold(void);
inline void esos_uiF14_setRPGMediumThreshold(int16_t threshold);
inline BOOL esos_uiF14_isRPGTurningFast(void);
inline int16_t esos_uiF14_getRPGFastThreshold(void);
inline void esos_uiF14_setRPGFastThreshold(int16_t threshold);
inline BOOL esos_uiF14_isRPGTurningCW(void);
inline BOOL esos_uiF14_isRPGTurningCCW(void);

void config_esos_uiF14();

// Public ESOS Macros
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isSW1Pressed());
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isSW1Released());

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED_AND_RELEASED()                                                          \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED();                                                                      \
        ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();                                                                     \
    }

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_DOUBLE_PRESSED() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isSW2DoublePressed());
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isSW2Pressed());
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isSW2Released());

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED_AND_RELEASED()                                                          \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED();                                                                      \
        ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();                                                                     \
    }

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_DOUBLE_PRESSED() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isSW2DoublePressed());
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isSW3Pressed());
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isSW3Released());

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED_AND_RELEASED()                                                          \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();                                                                      \
        ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();                                                                     \
    }

#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurning());
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS_CW() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningCW());
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS_CCW() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningCCW());
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningMedium());
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM_CW()                                                               \
    ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningMedium() && esos_uiF14_isRPGTurningCW());
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM_CCW()                                                              \
    ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningMedium() && esos_uiF14_isRPGTurningCCW());
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST() ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningFast());
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST_CW()                                                                 \
    ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningFast() && esos_uiF14_isRPGTurningCW());
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST_CCW()                                                                \
    ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningFast() && esos_uiF14_isRPGTurningCCW());

#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_REV(y)                                                                    \
    {                                                                                                                  \
        _esos_ui_setLastRPGCounter_u16(_esos_uiF14_getRPGCounter()); /*FIX THIS*/                                      \
        ESOS_TASK_WAIT_UNTIL(                                                                                          \
            _esos_uiF14_getRPGCounter() == _esos_uiF14_getLastRPGCounter() + (y * __ESOS_UIF14_RPG_TURNS_PER_REV) ||   \
            _esos_uiF14_getRPGCounter() == _esos_uiF14_getLastRPGCounter() - (y * __ESOS_UIF14_RPG_TURNS_PER_REV));    \
    }

#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_CW_REV(y)                                                                 \
    {                                                                                                                  \
        _esos_ui_setLastRPGCounter_u16(_esos_uiF14_getRPGCounter());                                                   \
        ESOS_TASK_WAIT_UNTIL(_esos_uiF14_getRPGCounter() ==                                                            \
                             _esos_uiF14_getLastRPGCounter() + (y * __ESOS_UIF14_RPG_TURNS_PER_REV));                  \
    }

#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_CCW_REV(y)                                                                \
    {                                                                                                                  \
        _esos_ui_setLastRPGCounter_u16(_esos_uiF14_getRPGCounter());                                                   \
        ESOS_TASK_WAIT_UNTIL(_esos_uiF14_getRPGCounter() ==                                                            \
                             _esos_uiF14_getLastRPGCounter() - (y * __ESOS_UIF14_RPG_TURNS_PER_REV));                  \
    }

#define ESOS_TASK_WAIT_UNTIL_RPGA_LOW() ESOS_TASK_WAIT_UNTIL(!esos_uiF14_getRPGA());
#define ESOS_TASK_WAIT_UNTIL_RPGA_HIGH() ESOS_TASK_WAIT_UNTIL(esos_uiF14_getRPGA());
#define ESOS_TASK_WAIT_UNTIL_RPGB_LOW() ESOS_TASK_WAIT_UNTIL(!esos_uiF14_getRPGB());
#define ESOS_TASK_WAIT_UNTIL_RPGB_HIGH() ESOS_TASK_WAIT_UNTIL(esos_uiF14_getRPGB());

#endif // ESOS_UIF14_H
