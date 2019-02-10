/*******************************************************************
 *
 * C code framework for ESOS user-interface (UI) service
 *
 *    requires the EMBEDDED SYSTEMS taris rev. F14
 *
 * ****************************************************************/
#include "esos_f14ui.h"

volatile _st_esos_uiF14Data_t _st_esos_uiF14Data;

// PRIVATE FUNCTIONS
inline void _esos_uiF14_setRPGCounter(uint16_t newValue)
{
    _st_esos_uiF14Data.u16_RPGCounter = newValue;
    return;
}

inline uint16_t _esos_uiF14_getRPGCounter(void)
{
    return _st_esos_uiF14Data.u16_RPGCounter;
}

inline void _esos_uiF14_setLastRPGCounter(uint16_t newValue)
{
    _st_esos_uiF14Data.u16_lastRPGCounter = newValue;
    return;
}

inline uint16_t _esos_uiF14_getLastRPGCounter(void)
{
    return _st_esos_uiF14Data.u16_lastRPGCounter;
}

// PUBLIC SWITCH FUNCTIONS
inline BOOL esos_uiF14_isSW1Pressed(void)
{
    return (_st_esos_uiF14Data.b_SW1Pressed == TRUE);
}

inline BOOL esos_uiF14_isSW1Released(void)
{
    return (_st_esos_uiF14Data.b_SW1Pressed == FALSE);
}

inline BOOL esos_uiF14_isSW1DoublePressed(void)
{
    if (_st_esos_uiF14Data.b_SW1DoublePressed == TRUE) {
        _st_esos_uiF14Data.b_SW1DoublePressed = FALSE;
        return TRUE;
    } else
        return FALSE;
}

inline void esos_uiF14_setSW1DoublePressed(void)
{
    _st_esos_uiF14Data.b_SW1DoublePressed = TRUE;
}

inline uint16_t esos_uiF14_getSW1DoublePressedPeriod(void)
{
    return _st_esos_uiF14Data.u16_SW1DoublePressedPeriod;
}
inline void esos_uiF14_setSW1DoublePressedPeriod(uint16_t period)
{
    _st_esos_uiF14Data.u16_SW1DoublePressedPeriod = period;
}

// SWITCH 2
inline BOOL esos_uiF14_isSW2Pressed(void)
{
    return (_st_esos_uiF14Data.b_SW2Pressed == TRUE);
}

inline BOOL esos_uiF14_isSW2Released(void)
{
    return (_st_esos_uiF14Data.b_SW2Pressed == FALSE);
}

inline BOOL esos_uiF14_isSW2DoublePressed(void)
{
    if (_st_esos_uiF14Data.b_SW2DoublePressed == TRUE) {
        _st_esos_uiF14Data.b_SW2DoublePressed = FALSE;
        return TRUE;
    } else
        return FALSE;
}

inline void esos_uiF14_setSW2DoublePressed(void)
{
    _st_esos_uiF14Data.b_SW2DoublePressed = TRUE;
}

inline uint16_t esos_uiF14_getSW2DoublePressedPeriod(void)
{
    return _st_esos_uiF14Data.u16_SW2DoublePressedPeriod;
}
inline void esos_uiF14_setSW2DoublePressedPeriod(uint16_t period)
{
    _st_esos_uiF14Data.u16_SW2DoublePressedPeriod = period;
}

// SWITCH 3
inline BOOL esos_uiF14_isSW3Pressed(void)
{
    return (_st_esos_uiF14Data.b_SW3Pressed == TRUE);
}

inline BOOL esos_uiF14_isSW3Released(void)
{
    return (_st_esos_uiF14Data.b_SW3Pressed == FALSE);
}

inline BOOL esos_uiF14_isSW3DoublePressed(void)
{
    return (_st_esos_uiF14Data.b_SW3DoublePressed == TRUE);
}

// PUBLIC LED FUNCTIONS

inline BOOL esos_uiF14_getRPGA(void)
{
    return (_st_esos_uiF14Data.b_RPGAHigh);
}
inline BOOL esos_uiF14_getRPGB(void)
{
    return (_st_esos_uiF14Data.b_RPGBHigh);
}

// LED 1
inline BOOL esos_uiF14_isLED1On(void)
{
    return (_st_esos_uiF14Data.b_LED1On == TRUE);
}

inline BOOL esos_uiF14_isLED1Off(void)
{
    return (_st_esos_uiF14Data.b_LED1On == FALSE);
}

inline void esos_uiF14_turnLED1On(void)
{
    _st_esos_uiF14Data.b_LED1On = TRUE;
    LED1_ON();
    return;
}

inline void esos_uiF14_turnLED1Off(void)
{
    _st_esos_uiF14Data.b_LED1On = FALSE;
    LED1_OFF();
    return;
}

inline void esos_uiF14_toggleLED1(void)
{
    _st_esos_uiF14Data.b_LED1On ^= 1;
    LED1_HB_TOGGLE();
    return;
}

inline void esos_uiF14_flashLED1(uint16_t u16_period)
{
    _st_esos_uiF14Data.u16_LED1FlashPeriod = u16_period;
    return;
}

// LED 2
inline BOOL esos_uiF14_isLED2On(void)
{
    return (_st_esos_uiF14Data.b_LED2On == TRUE);
}

inline BOOL esos_uiF14_isLED2Off(void)
{
    return (_st_esos_uiF14Data.b_LED2On == FALSE);
}

inline void esos_uiF14_turnLED2On(void)
{
    _st_esos_uiF14Data.b_LED2On = TRUE;
    LED2_ON();
    return;
}

inline void esos_uiF14_turnLED2Off(void)
{
    _st_esos_uiF14Data.b_LED2On = FALSE;
    LED2_OFF();
    return;
}

inline void esos_uiF14_toggleLED2(void)
{
    _st_esos_uiF14Data.b_LED2On ^= 1;
    LED2_HB_TOGGLE();
    return;
}

inline void esos_uiF14_flashLED2(uint16_t u16_period)
{
    _st_esos_uiF14Data.u16_LED2FlashPeriod = u16_period;
    return;
}

// LED 3
inline BOOL esos_uiF14_isLED3On(void)
{
    return (_st_esos_uiF14Data.b_LED3On == TRUE);
}

inline BOOL esos_uiF14_isLED3Off(void)
{
    return (_st_esos_uiF14Data.b_LED3On == FALSE);
}

inline void esos_uiF14_turnLED3On(void)
{
    _st_esos_uiF14Data.b_LED3On = TRUE;
    LED3_HB_ON();
    return;
}

inline void esos_uiF14_turnLED3Off(void)
{
    _st_esos_uiF14Data.b_LED3On = FALSE;
    LED3_HB_OFF();
    return;
}

inline void esos_uiF14_toggleLED3(void)
{
    _st_esos_uiF14Data.b_LED3On ^= 1;
    LED3_HB_TOGGLE();
    return;
}

inline void esos_uiF14_flashLED3(uint16_t u16_period)
{
    _st_esos_uiF14Data.u16_LED3FlashPeriod = u16_period;
    return;
}

// Red
inline void esos_uiF14_turnRedLEDOn(void)
{
    _st_esos_uiF14Data.b_LED1On = TRUE;
    LED1_ON();
    return;
}

inline void esos_uiF14_turnRedLEDOff(void)
{
    _st_esos_uiF14Data.b_LED1On = FALSE;
    LED1_OFF();
    return;
}

// Yellow
inline void esos_uiF14_turnYellowLEDOn(void)
{
    _st_esos_uiF14Data.b_LED2On = TRUE;
    LED2_ON();
    return;
}

inline void esos_uiF14_turnYellowLEDOff(void)
{
    _st_esos_uiF14Data.b_LED2On = FALSE;
    LED2_OFF();
    return;
}

// Green
inline void esos_uiF14_turnGreenLEDOn(void)
{
    _st_esos_uiF14Data.b_LED3On = TRUE;
    LED3_HB_ON();
    return;
}

inline void esos_uiF14_turnGreenLEDOff(void)
{
    _st_esos_uiF14Data.b_LED3On = FALSE;
    LED3_HB_OFF();
    return;
}

// PUBLIC RPG FUNCTIONS

inline int16_t esos_uiF14_getRPGVelocity_i16(void)
{
    return _st_esos_uiF14Data.i16_RPGVelocity;
}
inline uint16_t esos_uiF14_getRPGValue_u16(void)
{
    return _esos_uiF14_getRPGCounter();
}

inline BOOL esos_uiF14_isRPGTurning(void)
{
    return (esos_uiF14_getRPGVelocity_i16() != 0);
}

inline BOOL esos_uiF14_isRPGTurningSlow(void)
{
    int16_t v = esos_uiF14_getRPGVelocity_i16();
    BOOL res = (v > 0) && (v < (int16_t)esos_uiF14_getRPGSlowThreshold());
    res = res || (v < 0 && v > -((int16_t)esos_uiF14_getRPGSlowThreshold()));
    return res;
}

inline uint16_t esos_uiF14_getRPGSlowThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGSlowThreshold;
}

inline void esos_uiF14_setRPGSlowThreshold(uint16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGSlowThreshold = threshold;
}

inline BOOL esos_uiF14_isRPGTurningMedium(void)
{
    int16_t v = esos_uiF14_getRPGVelocity_i16();
    BOOL res = (v > (int16_t)esos_uiF14_getRPGSlowThreshold()) && (v < (int16_t)esos_uiF14_getRPGMediumThreshold());
    res =
        res || (v < -((int16_t)esos_uiF14_getRPGSlowThreshold()) && v > -((int16_t)esos_uiF14_getRPGMediumThreshold()));
    return res;
}

inline uint16_t esos_uiF14_getRPGMediumThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGMediumThreshold;
}

inline void esos_uiF14_setRPGMediumThreshold(uint16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGMediumThreshold = threshold;
}

inline BOOL esos_uiF14_isRPGTurningFast(void)
{
    int16_t v = esos_uiF14_getRPGVelocity_i16();
    BOOL res = (v > (int16_t)esos_uiF14_getRPGMediumThreshold()) && (v < (int16_t)esos_uiF14_getRPGFastThreshold());
    res =
        res || (v < -((int16_t)esos_uiF14_getRPGMediumThreshold()) && v > -((int16_t)esos_uiF14_getRPGFastThreshold()));
    return res;
}

inline uint16_t esos_uiF14_getRPGFastThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGFastThreshold;
}

inline void esos_uiF14_setRPGFastThreshold(uint16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGFastThreshold = threshold;
}

inline BOOL esos_uiF14_isRPGTurningCW(void)
{
    return (esos_uiF14_getRPGVelocity_i16() > 0);
}

inline BOOL esos_uiF14_isRPGTurningCCW(void)
{
    return !(esos_uiF14_isRPGTurningCW());
}

ESOS_USER_TIMER(__esos_uiF14_update_rpg_velocity)
{
    _esos_uiF14_setRPGCounter(_st_esos_uiF14Data.u16_RPGCounter - _st_esos_uiF14Data.u16_lastRPGCounter);
    _esos_uiF14_setLastRPGCounter(_esos_uiF14_getRPGCounter());
}

ESOS_USER_TASK(__esos_uiF14_SW1_double_pressed)
{
    static uint32_t start = 0, stop = 0;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        start = esos_GetSystemTick();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();
        stop = esos_GetSystemTick();
        if (stop - start < esos_uiF14_getSW1DoublePressedPeriod()) {
            esos_uiF14_setSW1DoublePressed();
        }
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(__esos_uiF14_SW2_double_pressed)
{
    static uint32_t start = 0, stop = 0;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        start = esos_GetSystemTick();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
        stop = esos_GetSystemTick();
        if (stop - start < esos_uiF14_getSW2DoublePressedPeriod()) {
            esos_uiF14_setSW2DoublePressed();
        }
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(__esos_uiF14_update_rpg)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        ESOS_TASK_WAIT_UNTIL_RPGA_LOW();
        if (esos_uiF14_getRPGA()) {
            _esos_uiF14_setRPGCounter(_esos_uiF14_getRPGCounter() + 1);
        } else {
            _esos_uiF14_setRPGCounter(_esos_uiF14_getRPGCounter() - 1);
        }
        ESOS_TASK_WAIT_UNTIL_RPGA_HIGH();
    }
    ESOS_TASK_END();
}

// UIF14 INITIALIZATION FUNCTION

void config_esos_uiF14()
{
    // setup your UI implementation
    // configure LEDs
    LED1_CONFIG();
    LED2_CONFIG();
    LED3_HB_CONFIG();

    // configure Switches
    SW1_CONFIG();
    SW2_CONFIG();
    SW3_CONFIG();

    // zero out the RPG counter
    _esos_uiF14_setRPGCounter(0);
    _esos_uiF14_setLastRPGCounter(0);

    // Register and start tasks/timers
    esos_RegisterTask(__esos_uiF14_task);
    esos_RegisterTimer(__esos_uiF14_update_rpg_velocity, __ESOS_UIF14_RPG_PERIOD);
    esos_RegisterTask(__esos_uiF14_SW1_double_pressed);
    esos_RegisterTask(__esos_uiF14_SW2_double_pressed);
    esos_RegisterTask(__esos_uiF14_update_rpg);
}

// UIF14 task to manage user-interface
ESOS_USER_TASK(__esos_uiF14_task)
{
    static int16_t _i16_LED1FlashTicksRemaining = 0;
    static int16_t _i16_LED2FlashTicksRemaining = 0;
    static int16_t _i16_LED3FlashTicksRemaining = 0;

    esos_uiF14_setSW1DoublePressedPeriod(1000);
    esos_uiF14_setSW2DoublePressedPeriod(1000);

    esos_uiF14_setRPGSlowThreshold(200);
    esos_uiF14_setRPGMediumThreshold(100);
    esos_uiF14_setRPGFastThreshold(10);

    ESOS_TASK_BEGIN();

    while (TRUE) {
        // do your UI stuff here

        // LEDs
        if (_st_esos_uiF14Data.u16_LED1FlashPeriod > 0) {
            _i16_LED1FlashTicksRemaining = _i16_LED1FlashTicksRemaining - __ESOS_UIF14_UI_PERIOD_MS;
            if (_i16_LED1FlashTicksRemaining <= 0) {
                _i16_LED1FlashTicksRemaining = _st_esos_uiF14Data.u16_LED1FlashPeriod;
                esos_uiF14_toggleLED1();
            }
        }

        if (_st_esos_uiF14Data.u16_LED2FlashPeriod > 0) {
            _i16_LED2FlashTicksRemaining = _i16_LED2FlashTicksRemaining - __ESOS_UIF14_UI_PERIOD_MS;
            if (_i16_LED2FlashTicksRemaining <= 0) {
                _i16_LED2FlashTicksRemaining = _st_esos_uiF14Data.u16_LED2FlashPeriod;
                esos_uiF14_toggleLED2();
            }
        }

        if (_st_esos_uiF14Data.u16_LED3FlashPeriod > 0) {
            _i16_LED3FlashTicksRemaining = _i16_LED3FlashTicksRemaining - __ESOS_UIF14_UI_PERIOD_MS;
            if (_i16_LED3FlashTicksRemaining <= 0) {
                _i16_LED3FlashTicksRemaining = _st_esos_uiF14Data.u16_LED3FlashPeriod;
                esos_uiF14_toggleLED3();
            }
        }

        // Switches
        if (SW1_PRESSED) {
            _st_esos_uiF14Data.b_SW1Pressed = TRUE;
        } else {
            _st_esos_uiF14Data.b_SW1Pressed = FALSE;
        }

        if (SW2_PRESSED) {
            _st_esos_uiF14Data.b_SW2Pressed = TRUE;
        } else {
            _st_esos_uiF14Data.b_SW2Pressed = FALSE;
        }

        if (SW3_PRESSED) {
            _st_esos_uiF14Data.b_SW3Pressed = TRUE;
        } else {
            _st_esos_uiF14Data.b_SW3Pressed = FALSE;
        }

        if (RPGA_HIGH) {
            _st_esos_uiF14Data.b_RPGAHigh = TRUE;
        } else {
            _st_esos_uiF14Data.b_RPGAHigh = FALSE;
        }

        ESOS_TASK_WAIT_TICKS(__ESOS_UIF14_UI_PERIOD_MS);
    }
    ESOS_TASK_END();
}
