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
#define __ESOS_UIF14_DEFAULT_RPGS_PERIOD 5
#define __ESOS_UIF14_DEFAULT_RPGM_PERIOD 15
#define __ESOS_UIF14_DEFAULT_RPGF_PERIOD 30
#define __ESOS_UIF14_UI_PERIOD_MS 10
#define __ESOS_UIF14_RPG_TURNS_PER_REV 12
#define __ESOS_UIF14_RPG_PERIOD 2000

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

    BOOL b_RPGAHigh;
    BOOL b_RPGBHigh;

    BOOL b_LED1On;
    uint16_t u16_LED1FlashPeriod;
    BOOL b_LED2On;
    uint16_t u16_LED2FlashPeriod;
    BOOL b_LED3On;
    uint16_t u16_LED3FlashPeriod;

    int16_t i16_RPGVelocity;
    uint16_t u16_RPGCounter;
    uint16_t u16_lastRPGCounter;
    int16_t u16_RPGSlowThreshold;
    int16_t u16_RPGMediumThreshold;
    int16_t u16_RPGFastThreshold;
} _st_esos_uiF14Data_t;

// Private Data
extern volatile _st_esos_uiF14Data_t _st_esos_uiF14Data;

// Private Function Prototypes
uint16_t _esos_uiF14_getRPGCounter(void);
void _esos_ui_setRPGCounter(uint16_t);

uint16_t _esos_uiF14_getLastRPGCounter(void);
void _esos_ui_setLastRPGCounter(uint16_t);

ESOS_USER_TASK(__esos_uiF14_task);
ESOS_USER_TIMER(__esos_uiF14_update_rpg_velocity);
ESOS_USER_TASK(__esos_uiF14_SW1_double_pressed);
ESOS_USER_TASK(__esos_uiF14_SW2_double_pressed);
ESOS_USER_TASK(__esos_uiF14_SW3_double_pressed);
ESOS_USER_TASK(__esos_uiF14_update_rpg);

// Private ESOS UI Macros
// Macro to setup Output Compare modules with specified remappable output pin
#define __ESOS_UIF14_CONFIG_TOGGLE_OC(Rxy_RP, OCodd, OCeven) __ESOS_UIF14__CONFIG_TOGGLE_OC(Rxy_RP, OCodd, OCeven)
#define __ESOS_UIF14__CONFIG_TOGGLE_OC(Rxy_RP, OCodd, OCeven)                                                          \
    {                                                                                                                  \
        _RP##Rxy_RP##R = _RPOUT_OC##OCeven;                                                                            \
        OC##OCodd##CON1 = OC##OCodd##CON2 = OC##OCeven##CON1 = OC##OCeven##CON2 = 0x0000;                              \
        OC##OCodd##CON1bits.TRIGMODE = OC##OCeven##CON1bits.TRIGMODE = 1;                                              \
        OC##OCodd##CON1bits.OCTSEL = OC##OCeven##CON1bits.OCTSEL = 0b111;                                              \
        OC##OCodd##CON2 = OC##OCeven##CON2 = OC_SYNCSEL_OCxRS;                                                         \
        OC##OCodd##CON2 = OC##OCeven##CON2 |= OC_IC32_ON;                                                              \
        OC##OCodd##CON2 |= OC_TRIS_ON;                                                                                 \
    }

// Note: uses RF1 as intermediate pin because of RPI pins
#define LED1_IMDT_PIN _RF1
#define LED1_UI_CONFIG()                                                                                               \
    {                                                                                                                  \
        LED1_CONFIG();                                                                                                 \
        CONFIG_RF1_AS_DIG_OUTPUT(); /* Use RF1 as intermediate pin for RF4 */                                          \
        __ESOS_UIF14_CONFIG_TOGGLE_OC(RF1_RP, 11, 12); /* Configure LED Flsash Controller on RF1 using OC11/12 */      \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_CN, ESOS_USER_IRQ_LEVEL2, _CNInterrupt);                     \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_CN);                                                           \
        ENABLE_RF1_CN_INTERRUPT();                                                                                     \
    }

// Note: uses RF2 as intermediate pin because of RPI pins
#define LED2_IMDT_PIN _RF2
#define LED2_UI_CONFIG()                                                                                               \
    {                                                                                                                  \
        LED2_CONFIG();                                                                                                 \
        CONFIG_RF2_AS_DIG_OUTPUT(); /* Use RF2 as intermediate pin for RB14 */                                         \
        __ESOS_UIF14_CONFIG_TOGGLE_OC(RF2_RP, 13, 14); /* Configure LED Flash Controller on RF2 using OC13/14 */       \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC2, ESOS_USER_IRQ_LEVEL2, _IC2Interrupt);                   \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC2);                                                          \
        ENABLE_RF2_CN_INTERRUPT();                                                                                     \
    }

// Note: uses RF3 as intermediate pin because of RPI pins
#define LED3_IMDT_PIN _RF3
#define LED3_UI_CONFIG()                                                                                               \
    {                                                                                                                  \
        LED3_HB_CONFIG();                                                                                              \
        CONFIG_RF3_AS_DIG_OUTPUT(); /* Use RF3 as intermediate pin for RB15 */                                         \
        __ESOS_UIF14_CONFIG_TOGGLE_OC(RF3_RP, 15, 16); /* Configure LED Flash Controller on RF3 using OC15/16 */       \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC3, ESOS_USER_IRQ_LEVEL2, _IC3Interrupt);                   \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC3);                                                          \
        ENABLE_RF3_CN_INTERRUPT();                                                                                     \
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
        _esos_ui_setLastRPGCounter_u16(_esos_uiF14_getRPGCounter());                                                   \
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
