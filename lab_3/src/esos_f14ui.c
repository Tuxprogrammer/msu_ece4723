/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * esos_f14ui.h - C code framework for ESOS user-interface (UI) service
 */

#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"

#include "revF14.h"

ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_CN)
{
    (__LED1_IMDT_PIN) ? LED1_ON() : LED1_OFF();
    (__LED2_IMDT_PIN) ? LED2_ON() : LED2_OFF();
    (__LED3_IMDT_PIN) ? LED3_HB_ON() : LED3_HB_OFF();

    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_CN);
}

volatile _st_esos_uiF14Data_t _st_esos_uiF14Data;

#pragma region PRIVATE RPG FUNCTIONS
inline void _esos_uiF14_setRPGCounter(uint16_t newValue)
{
    _st_esos_uiF14Data.u16_RPGCounter = newValue;
}

inline uint16_t _esos_uiF14_getRPGCounter(void)
{
    return _st_esos_uiF14Data.u16_RPGCounter;
}

inline void _esos_uiF14_setLastRPGCounter(uint16_t newValue)
{
    _st_esos_uiF14Data.u16_lastRPGCounter = newValue;
}

inline uint16_t _esos_uiF14_getLastRPGCounter(void)
{
    return _st_esos_uiF14Data.u16_lastRPGCounter;
}
#pragma endregion

#pragma region SWITCH 1
inline BOOL esos_uiF14_isSW1Pressed(void)
{
    return _st_esos_uiF14Data.b_SW1Pressed;
}

inline BOOL esos_uiF14_isSW1Released(void)
{
    return _st_esos_uiF14Data.b_SW1Pressed == FALSE;
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
#pragma endregion

#pragma region SWITCH 2
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
#pragma endregion

#pragma region SWITCH 3
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
    if (_st_esos_uiF14Data.b_SW3DoublePressed == TRUE) {
        _st_esos_uiF14Data.b_SW3DoublePressed = FALSE;
        return TRUE;
    } else
        return FALSE;
}

inline void esos_uiF14_setSW3DoublePressed(void)
{
    _st_esos_uiF14Data.b_SW3DoublePressed = TRUE;
}

inline uint16_t esos_uiF14_getSW3DoublePressedPeriod(void)
{
    return _st_esos_uiF14Data.u16_SW3DoublePressedPeriod;
}

inline void esos_uiF14_setSW3DoublePressedPeriod(uint16_t period)
{
    _st_esos_uiF14Data.u16_SW3DoublePressedPeriod = period;
}
#pragma endregion

#pragma region LED1
inline BOOL esos_uiF14_isLED1On(void)
{
    // Return true if the LED is solid or flashing
    return ((_st_esos_uiF14Data.b_LED1On || _st_esos_uiF14Data.u16_LED1FlashPeriod > 0) == TRUE);
}

inline BOOL esos_uiF14_isLED1Off(void)
{
    // Return true if the LED is NOT solid or flashing
    return ((_st_esos_uiF14Data.b_LED1On || _st_esos_uiF14Data.u16_LED1FlashPeriod > 0) == FALSE);
}

inline void esos_uiF14_turnLED1On(void)
{
    // Disable flashing and set the LED on
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED1On = TRUE;
    LED1_ON();
    return;
}

inline void esos_uiF14_turnLED1Off(void)
{
    // Disable flashing and set the LED off
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED1On = FALSE;
    LED1_OFF();
    return;
}

inline void esos_uiF14_toggleLED1(void)
{
    // Disable flashing and invert the LED state
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED1On ^= 1;
    LED1_HB_TOGGLE();
    return;
}

inline void esos_uiF14_flashLED1(uint16_t u16_period)
{
    // Set the period
    _st_esos_uiF14Data.u16_LED1FlashPeriod = u16_period;

    // If the period is zero, turn off the flasher and return
    if (u16_period == 0) {
        OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC1/2
    OC11R = OC11RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC12R = OC12RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_TOGGLE_PULSE; // OC12 must be enabled first
    return;
}
#pragma endregion
#pragma region LED2
inline BOOL esos_uiF14_isLED2On(void)
{
    // Return true if the LED is solid or flashing
    return ((_st_esos_uiF14Data.b_LED2On || _st_esos_uiF14Data.u16_LED2FlashPeriod > 0) == TRUE);
}

inline BOOL esos_uiF14_isLED2Off(void)
{
    // Return true if the LED is NOT solid or flashing
    return ((_st_esos_uiF14Data.b_LED2On || _st_esos_uiF14Data.u16_LED2FlashPeriod > 0) == FALSE);
}

inline void esos_uiF14_turnLED2On(void)
{
    // Disable flashing and set the LED on
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED2On = TRUE;
    LED2_ON();
    return;
}

inline void esos_uiF14_turnLED2Off(void)
{
    // Disable flashing and set the LED off
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED2On = FALSE;
    LED2_OFF();
    return;
}

inline void esos_uiF14_toggleLED2(void)
{
    // Disable flashing and invert the LED state
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED2On ^= 1;
    LED2_HB_TOGGLE();
    return;
}

inline void esos_uiF14_flashLED2(uint16_t u16_period)
{
    // Set the period
    _st_esos_uiF14Data.u16_LED2FlashPeriod = u16_period;

    // If the period is zero, turn off the flasher and return
    if (u16_period == 0) {
        OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC3/4
    OC13R = OC13RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC14R = OC14RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_TOGGLE_PULSE; // OC14 must be enabled first
    return;
}
#pragma endregion
#pragma region LED3
inline BOOL esos_uiF14_isLED3On(void)
{
    // Return true if the LED is solid or flashing
    return ((_st_esos_uiF14Data.b_LED3On || _st_esos_uiF14Data.u16_LED3FlashPeriod > 0) == TRUE);
}

inline BOOL esos_uiF14_isLED3Off(void)
{
    // Return true if the LED is NOT solid or flashing
    return ((_st_esos_uiF14Data.b_LED3On || _st_esos_uiF14Data.u16_LED3FlashPeriod > 0) == FALSE);
}

inline void esos_uiF14_turnLED3On(void)
{
    // Disable flashing and set the LED on
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED3On = TRUE;
    LED3_HB_ON();
    return;
}

inline void esos_uiF14_turnLED3Off(void)
{
    // Disable flashing and set the LED off
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED3On = FALSE;
    LED3_HB_OFF();
    return;
}

inline void esos_uiF14_toggleLED3(void)
{
    // Disable flashing and invert the LED state
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED3On ^= 1;
    LED3_HB_TOGGLE();
    return;
}

inline void esos_uiF14_flashLED3(uint16_t u16_period)
{
    // Set the period
    _st_esos_uiF14Data.u16_LED3FlashPeriod = u16_period;

    // If the period is zero, turn off the flasher and return
    if (u16_period == 0) {
        OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC5/6
    OC15R = OC15RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC16R = OC16RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_TOGGLE_PULSE; // OC16 must be enabled first
    return;
}
#pragma endregion
#pragma region RAGLEDs
inline void esos_uiF14_turnRedLEDOn(void)
{
    esos_uiF14_turnLED1On();
    return;
}

inline void esos_uiF14_turnRedLEDOff(void)
{
    esos_uiF14_turnLED1Off();
    return;
}

inline void esos_uiF14_turnYellowLEDOn(void)
{
    esos_uiF14_turnLED2On();
    return;
}

inline void esos_uiF14_turnYellowLEDOff(void)
{
    esos_uiF14_turnLED2Off();
    return;
}

inline void esos_uiF14_turnGreenLEDOn(void)
{
    esos_uiF14_turnLED3On();
    return;
}

inline void esos_uiF14_turnGreenLEDOff(void)
{
    esos_uiF14_turnLED3Off();
    return;
}
#pragma endregion

#pragma region PUBLIC RPG FUNCTIONS

inline BOOL esos_uiF14_getRPGA(void)
{
    return _st_esos_uiF14Data.b_RPGAHigh;
}
inline void esos_uiF14_setRPGA(BOOL rpg)
{
    _st_esos_uiF14Data.b_RPGAHigh = rpg;
}
inline BOOL esos_uiF14_getRPGB(void)
{
    return _st_esos_uiF14Data.b_RPGBHigh;
}
inline void esos_uiF14_setRPGB(BOOL rpg)
{
    _st_esos_uiF14Data.b_RPGBHigh = rpg;
}

inline int16_t esos_uiF14_getRPGVelocity(void)
{
    return _st_esos_uiF14Data.i16_RPGVelocity;
}
inline void esos_uiF14_setRPGVelocity(int16_t v)
{
    _st_esos_uiF14Data.i16_RPGVelocity = v;
}

inline BOOL esos_uiF14_isRPGTurning(void)
{
    return (esos_uiF14_getRPGVelocity() != 0);
}

inline BOOL esos_uiF14_isRPGTurningSlow(void)
{
    int16_t v = esos_uiF14_getRPGVelocity();
    BOOL res = (v > 0) && (v < esos_uiF14_getRPGSlowThreshold());
    res = res || (v < 0 && v > -(esos_uiF14_getRPGSlowThreshold()));
    return res;
}

inline int16_t esos_uiF14_getRPGSlowThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGSlowThreshold;
}

inline void esos_uiF14_setRPGSlowThreshold(int16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGSlowThreshold = threshold;
}

inline BOOL esos_uiF14_isRPGTurningMedium(void)
{
    int16_t v = esos_uiF14_getRPGVelocity();
    BOOL res = (v > esos_uiF14_getRPGSlowThreshold()) && (v < esos_uiF14_getRPGMediumThreshold());
    res = res || (v < -(esos_uiF14_getRPGSlowThreshold()) && v > -(esos_uiF14_getRPGMediumThreshold()));
    return res;
}

inline int16_t esos_uiF14_getRPGMediumThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGMediumThreshold;
}

inline void esos_uiF14_setRPGMediumThreshold(int16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGMediumThreshold = threshold;
}

inline BOOL esos_uiF14_isRPGTurningFast(void)
{
    int16_t v = esos_uiF14_getRPGVelocity();
    BOOL res = (v > esos_uiF14_getRPGMediumThreshold()) && (v < esos_uiF14_getRPGFastThreshold());
    res = res || (v < -(esos_uiF14_getRPGMediumThreshold()) && v > -(esos_uiF14_getRPGFastThreshold()));
    return res;
}

inline int16_t esos_uiF14_getRPGFastThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGFastThreshold;
}

inline void esos_uiF14_setRPGFastThreshold(int16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGFastThreshold = threshold;
}

inline BOOL esos_uiF14_isRPGTurningCW(void)
{
    return (esos_uiF14_getRPGVelocity() > 0);
}

inline BOOL esos_uiF14_isRPGTurningCCW(void)
{
    return esos_uiF14_getRPGVelocity() != 0 && !(esos_uiF14_isRPGTurningCW());
}
#pragma endregion

#pragma region USER TASKS
ESOS_USER_TIMER(__esos_uiF14_update_rpg_velocity)
{
    esos_uiF14_setRPGVelocity((_st_esos_uiF14Data.u16_RPGCounter - _st_esos_uiF14Data.u16_lastRPGCounter)
                              /*/ __ESOS_UIF14_RPG_PERIOD*/);
    _esos_uiF14_setLastRPGCounter(_esos_uiF14_getRPGCounter());
}

// ESOS_USER_TASK(__esos_uiF14_SW1_double_pressed)
// {
//     static uint32_t start = 0, stop = 0;
//     ESOS_TASK_BEGIN();
//     while (TRUE) {
//         ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED();
//         ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();
//         stop = esos_GetSystemTick();
//         if (stop - start < esos_uiF14_getSW1DoublePressedPeriod()) {
//             esos_uiF14_setSW1DoublePressed();
//         }
//         start = esos_GetSystemTick();
//     }
//     ESOS_TASK_END();
// }

// ESOS_USER_TASK(__esos_uiF14_SW2_double_pressed)
// {
//     static uint32_t start = 0, stop = 0;
//     ESOS_TASK_BEGIN();
//     while (TRUE) {
//         ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED();
//         ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
//         stop = esos_GetSystemTick();
//         if (stop - start < esos_uiF14_getSW2DoublePressedPeriod()) {
//             esos_uiF14_setSW2DoublePressed();
//         }
//         start = esos_GetSystemTick();
//     }
//     ESOS_TASK_END();
// }

// ESOS_USER_TASK(__esos_uiF14_SW3_double_pressed)
// {
//     static uint32_t start = 0, stop = 0;
//     ESOS_TASK_BEGIN();
//     while (TRUE) {
//         ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();
//         ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();
//         stop = esos_GetSystemTick();
//         if (stop - start < esos_uiF14_getSW3DoublePressedPeriod()) {
//             esos_uiF14_setSW3DoublePressed();
//         }
//         start = esos_GetSystemTick();
//     }
//     ESOS_TASK_END();
// }

ESOS_USER_TASK(__esos_uiF14_update_rpg)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        ESOS_TASK_WAIT_UNTIL_RPGA_LOW();
        // ESOS_TASK_WAIT_TICKS(2); // simple debounce 1-5ms
        if (esos_uiF14_getRPGB()) {
            _esos_uiF14_setRPGCounter(_esos_uiF14_getRPGCounter() + 1);
        } else {
            _esos_uiF14_setRPGCounter(_esos_uiF14_getRPGCounter() + 1); // change this to a minus -- VERY BAD HAX
        }
        // ESOS_TASK_WAIT_TICKS(2); // simple debounce 1-5ms
        ESOS_TASK_WAIT_UNTIL_RPGA_HIGH();
        // ESOS_TASK_WAIT_TICKS(2); // simple debounce 1-5ms
    }
    ESOS_TASK_END();
}
#pragma endregion

// UIF14 INITIALIZATION FUNCTION

void config_esos_uiF14()
{
    // Configure LEDs/OCs and Intermediate Pin RF1/2/3 ICs/Interrupts
    __LED1_UI_CONFIG();
    __LED2_UI_CONFIG();
    __LED3_UI_CONFIG();

    // configure Switches
    SW1_CONFIG();
    SW2_CONFIG();
    SW3_CONFIG();

    // configure RPG
    RPG_CONFIG();

    // zero out the RPG counter
    _esos_uiF14_setRPGCounter(0);
    _esos_uiF14_setLastRPGCounter(0);

    esos_uiF14_setSW1DoublePressedPeriod(__ESOS_UIF14_DEFAULT_SWDP_PERIOD);
    esos_uiF14_setSW2DoublePressedPeriod(__ESOS_UIF14_DEFAULT_SWDP_PERIOD);
    esos_uiF14_setSW3DoublePressedPeriod(__ESOS_UIF14_DEFAULT_SWDP_PERIOD);
    esos_uiF14_setRPGSlowThreshold(__ESOS_UIF14_DEFAULT_RPGS_PERIOD);
    esos_uiF14_setRPGMediumThreshold(__ESOS_UIF14_DEFAULT_RPGM_PERIOD);
    esos_uiF14_setRPGFastThreshold(__ESOS_UIF14_DEFAULT_RPGF_PERIOD);

    // Register and start tasks/timers
    esos_RegisterTask(__esos_uiF14_task);
    esos_RegisterTimer(__esos_uiF14_update_rpg_velocity, __ESOS_UIF14_RPG_PERIOD);
    // esos_RegisterTask(__esos_uiF14_SW1_double_pressed);
    // esos_RegisterTask(__esos_uiF14_SW2_double_pressed);
    // esos_RegisterTask(__esos_uiF14_SW3_double_pressed);
    esos_RegisterTask(__esos_uiF14_update_rpg);
}

// UIF14 task to manage user-interface
ESOS_USER_TASK(__esos_uiF14_task)
{
    ESOS_TASK_BEGIN();

    while (TRUE) {
        // // Switches
        // if (SW1_PRESSED) {
        //     _st_esos_uiF14Data.b_SW1Pressed = TRUE;
        // } else {
        //     _st_esos_uiF14Data.b_SW1Pressed = FALSE;
        // }

        // if (SW2_PRESSED) {
        //     _st_esos_uiF14Data.b_SW2Pressed = TRUE;
        // } else {
        //     _st_esos_uiF14Data.b_SW2Pressed = FALSE;
        // }

        // if (SW3_PRESSED) {
        //     _st_esos_uiF14Data.b_SW3Pressed = TRUE;
        // } else {
        //     _st_esos_uiF14Data.b_SW3Pressed = FALSE;
        // }

        if (RPGA_HIGH) {
            _st_esos_uiF14Data.b_RPGAHigh = TRUE;
        } else {
            _st_esos_uiF14Data.b_RPGAHigh = FALSE;
        }

        if (RPGB_HIGH) {
            _st_esos_uiF14Data.b_RPGBHigh = TRUE;
        } else {
            _st_esos_uiF14Data.b_RPGBHigh = FALSE;
        }

        ESOS_TASK_WAIT_TICKS(__ESOS_UIF14_UI_PERIOD_MS);
    }
    ESOS_TASK_END();
}
