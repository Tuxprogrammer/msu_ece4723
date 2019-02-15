/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * esos_f14ui.h - C code framework for ESOS user-interface (UI) service
 */

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"

#if defined(_RPOUT_OC6) || defined(__DOXYGEN__)
#define CONFIG_OC6_TO_RP(Rxy_RP) _CONFIG_OC6_TO_RP(Rxy_RP)
#define _CONFIG_OC6_TO_RP(Rxy_RP) (_RP##Rxy_RP##R = _RPOUT_OC6)
#endif

// Note: uses RF1 as intermediate pin because of RPI pins
#define LED1_UI_CONFIG()                                                                                               \
    {                                                                                                                  \
        LED1_CONFIG();                                                                                                 \
        CONFIG_RF1_AS_DIG_OUTPUT();                                                                                    \
        CONFIG_OC2_TO_RP(RF1_RP);                                                                                      \
        OC1CON1 = OC2CON1 = 0x1C08;                                                                                    \
        OC1CON2 = 0x013F;                                                                                              \
        OC2CON2 = 0x011F;                                                                                              \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC1, ESOS_USER_IRQ_LEVEL2, _IC1Interrupt);                   \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC1);                                                          \
        CONFIG_IC1_TO_RP(RF1_RP);                                                                                      \
        IC1CON1 = 0x1C01;                                                                                              \
        IC1CON2 = 0x001F;                                                                                              \
    }

// Note: uses RF2 as intermediate pin because of RPI pins
#define LED2_UI_CONFIG()                                                                                               \
    {                                                                                                                  \
        LED2_CONFIG();                                                                                                 \
        CONFIG_RF2_AS_DIG_OUTPUT();                                                                                    \
        CONFIG_OC4_TO_RP(RF2_RP);                                                                                      \
        OC3CON1 = OC4CON1 = 0x1C08;                                                                                    \
        OC3CON2 = 0x013F;                                                                                              \
        OC4CON2 = 0x011F;                                                                                              \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC2, ESOS_USER_IRQ_LEVEL2, _IC2Interrupt);                   \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC2);                                                          \
        CONFIG_IC2_TO_RP(RF2_RP);                                                                                      \
        IC2CON1 = 0x1C01;                                                                                              \
        IC2CON2 = 0x001F;                                                                                              \
    }

// Note: uses RF3 as intermediate pin because of RPI pins
#define LED3_UI_CONFIG()                                                                                               \
    {                                                                                                                  \
        LED3_HB_CONFIG();                                                                                              \
        CONFIG_RF3_AS_DIG_OUTPUT();                                                                                    \
        CONFIG_OC6_TO_RP(RF3_RP);                                                                                      \
        OC5CON1 = OC6CON1 = 0x1C08;                                                                                    \
        OC5CON2 = 0x013F;                                                                                              \
        OC6CON2 = 0x011F;                                                                                              \
        ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC3, ESOS_USER_IRQ_LEVEL2, _IC3Interrupt);                   \
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_IC3);                                                          \
        CONFIG_IC3_TO_RP(RF3_RP);                                                                                      \
        IC3CON1 = 0x1C01;                                                                                              \
        IC3CON2 = 0x001F;                                                                                              \
    }

ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_IC2)
{
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_IC2);
    (_RF2) ? LED2_ON() : LED2_OFF();
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
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    OC2CON1bits.OCM = OC_OFF;
    OC1CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.b_LED1On = TRUE;
    LED1_ON();
    return;
}

inline void esos_uiF14_turnLED1Off(void)
{
    // Disable flashing and set the LED off
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    OC2CON1bits.OCM = OC_OFF;
    OC1CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.b_LED1On = FALSE;
    LED1_OFF();
    return;
}

inline void esos_uiF14_toggleLED1(void)
{
    // Disable flashing and invert the LED state
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    OC2CON1bits.OCM = OC_OFF;
    OC1CON1bits.OCM = OC_OFF;
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
        OC2CON1bits.OCM = OC_OFF;
        OC1CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC1/2
    OC1R = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC2R = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC1RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC2RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC2CON1bits.OCM = OC_TOGGLE_PULSE;
    OC1CON1bits.OCM = OC_TOGGLE_PULSE; // OC2 must be enabled first
    return;
}
#pragma endregion
#pragma region LED 2
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
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    OC4CON1bits.OCM = OC_OFF;
    OC3CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.b_LED2On = TRUE;
    LED2_ON();
    return;
}

inline void esos_uiF14_turnLED2Off(void)
{
    // Disable flashing and set the LED off
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    OC4CON1bits.OCM = OC_OFF;
    OC3CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.b_LED2On = FALSE;
    LED2_OFF();
    return;
}

inline void esos_uiF14_toggleLED2(void)
{
    // Disable flashing and invert the LED state
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    OC4CON1bits.OCM = OC_OFF;
    OC3CON1bits.OCM = OC_OFF;
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
        OC4CON1bits.OCM = OC_OFF;
        OC3CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC3/4
    OC3R = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC4R = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC3RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC4RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC4CON1bits.OCM = OC_TOGGLE_PULSE;
    OC3CON1bits.OCM = OC_TOGGLE_PULSE; // OC4 must be enabled first
    return;
}
#pragma endregion
#pragma region LED 3
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
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    OC6CON1bits.OCM = OC_OFF;
    OC5CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.b_LED3On = TRUE;
    LED3_HB_ON();
    return;
}

inline void esos_uiF14_turnLED3Off(void)
{
    // Disable flashing and set the LED off
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    OC6CON1bits.OCM = OC_OFF;
    OC5CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.b_LED3On = FALSE;
    LED3_HB_OFF();
    return;
}

inline void esos_uiF14_toggleLED3(void)
{
    // Disable flashing and invert the LED state
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    OC6CON1bits.OCM = OC_OFF;
    OC5CON1bits.OCM = OC_OFF;
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
        OC6CON1bits.OCM = OC_OFF;
        OC5CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC5/6
    OC5R = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC6R = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC5RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC6RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC6CON1bits.OCM = OC_TOGGLE_PULSE;
    OC5CON1bits.OCM = OC_TOGGLE_PULSE; // OC6 must be enabled first
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

ESOS_USER_TASK(__esos_uiF14_SW1_double_pressed)
{
    static uint32_t start = 0, stop = 0;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();
        stop = esos_GetSystemTick();
        if (stop - start < esos_uiF14_getSW1DoublePressedPeriod()) {
            esos_uiF14_setSW1DoublePressed();
        }
        start = esos_GetSystemTick();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(__esos_uiF14_SW2_double_pressed)
{
    static uint32_t start = 0, stop = 0;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED();
        stop = esos_GetSystemTick();
        if (stop - start < esos_uiF14_getSW2DoublePressedPeriod()) {
            esos_uiF14_setSW2DoublePressed();
        }
        start = esos_GetSystemTick();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(__esos_uiF14_SW3_double_pressed)
{
    static uint32_t start = 0, stop = 0;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();
        ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();
        stop = esos_GetSystemTick();
        if (stop - start < esos_uiF14_getSW3DoublePressedPeriod()) {
            esos_uiF14_setSW3DoublePressed();
        }
        start = esos_GetSystemTick();
    }
    ESOS_TASK_END();
}

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
    LED1_UI_CONFIG();
    LED2_UI_CONFIG();
    LED3_UI_CONFIG();

    // configure Switches
    SW1_CONFIG();
    SW2_CONFIG();
    SW3_CONFIG();

    // configure RPG
    RPG_CONFIG();

    // zero out the RPG counter
    _esos_uiF14_setRPGCounter(0);
    _esos_uiF14_setLastRPGCounter(0);

    esos_uiF14_setRPGSlowThreshold(5);
    esos_uiF14_setRPGMediumThreshold(15);
    esos_uiF14_setRPGFastThreshold(30);

    // Register and start tasks/timers
    esos_RegisterTask(__esos_uiF14_task);
    esos_RegisterTimer(__esos_uiF14_update_rpg_velocity, __ESOS_UIF14_RPG_PERIOD);
    esos_RegisterTask(__esos_uiF14_SW1_double_pressed);
    esos_RegisterTask(__esos_uiF14_SW2_double_pressed);
    esos_RegisterTask(__esos_uiF14_SW3_double_pressed);
    esos_RegisterTask(__esos_uiF14_update_rpg);
}

// UIF14 task to manage user-interface
ESOS_USER_TASK(__esos_uiF14_task)
{
    ESOS_TASK_BEGIN();
    esos_uiF14_setSW1DoublePressedPeriod(1000);
    esos_uiF14_setSW2DoublePressedPeriod(1000);
    esos_uiF14_setSW3DoublePressedPeriod(1000);

    while (TRUE) {
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

        if (RPGB_HIGH) {
            _st_esos_uiF14Data.b_RPGBHigh = TRUE;
        } else {
            _st_esos_uiF14Data.b_RPGBHigh = FALSE;
        }

        ESOS_TASK_WAIT_TICKS(__ESOS_UIF14_UI_PERIOD_MS);
    }
    ESOS_TASK_END();
}
