/*******************************************************************
 *
 * C header file for ESOS user-interface (UI) service
 *
 *    requires the EMBEDDED SYSTEMS target rev. F14
 *
 * ****************************************************************/

#ifndef ESOS_UIF14_H
#define ESOS_UIF14_H

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"

// STRUCTURES

typedef struct {
    BOOL b_SW1Pressed;
    BOOL b_SW1DoublePressed;
    uint16_t u16_SW1DoublePressedPeriod;
    BOOL b_SW2Pressed;
    BOOL b_SW2DoublePressed;
    uint16_t u16_SW2DoublePressedPeriod;
    BOOL b_SW3Pressed;
    BOOL b_SW3DoublePressed;

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

// DEFINEs and CONSTANTs
#define __ESOS_TICKS_TO_MS(x) (x / 1)
#define __ESOS_MS_TO_TICKS(x) (x * 1)
#define __ESOS_UIF14_UI_PERIOD_MS 10
#define __ESOS_UIF14_RPG_TURNS_PER_REV 12
#define __ESOS_UIF14_RPG_PERIOD 500

// PRIVATE DATA

extern volatile _st_esos_uiF14Data_t _st_esos_uiF14Data;

// PRIVATE FUNCTION PROTOTYPES

uint16_t _esos_uiF14_getRPGCounter(void);
void _esos_ui_setRPGCounter(uint16_t);

uint16_t _esos_uiF14_getLastRPGCounter(void);
void _esos_ui_setLastRPGCounter(uint16_t);

ESOS_USER_TASK(__esos_uiF14_task);

// PUBLIC API FUNCTION PROTOTYPES

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

ESOS_USER_TASK(__esos_uiF14_task);
ESOS_USER_TIMER(__esos_uiF14_update_rpg_velocity);
ESOS_USER_TASK(__esos_uiF14_SW1_double_pressed);
ESOS_USER_TASK(__esos_uiF14_SW2_double_pressed);
ESOS_USER_TASK(__esos_uiF14_update_rpg);

void config_esos_uiF14();

// PUBLIC API ESOS TASK MACROS

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
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS()                                                                   \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurning());                                                               \
    }
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS_CW()                                                                \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningCW());                                                             \
    }
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS_CCW()                                                               \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningCCW());                                                            \
    }
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM()                                                                  \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningMedium());                                                         \
    }
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM_CW()                                                               \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningMedium() && esos_uiF14_isRPGTurningCW());                          \
    }
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM_CCW()                                                              \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningMedium() && esos_uiF14_isRPGTurningCCW());                         \
    }
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST()                                                                    \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningFast());                                                           \
    }
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST_CW()                                                                 \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningFast() && esos_uiF14_isRPGTurningCW());                            \
    }
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST_CCW()                                                                \
    {                                                                                                                  \
        ESOS_TASK_WAIT_UNTIL(esos_uiF14_isRPGTurningFast() && esos_uiF14_isRPGTurningCCW());                           \
    }
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
