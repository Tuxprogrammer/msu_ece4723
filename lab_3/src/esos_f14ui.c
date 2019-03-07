/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * esos_f14ui.h - C code framework for ESOS user-interface (UI) service
 */

#include "esos.h"
#include "esos_pic24.h"

#include "revF14.h"
#include "esos_f14ui.h"

// Defines and Constants
#define ABS(x) (x < 0 ? -x : x)

// Private Data
volatile _st_esos_uiF14Data_t _st_esos_uiF14Data;

#pragma region ESOS UI Timers
ESOS_USER_TIMER(__esos_uiF14_sw_poll)
{
    // Debounce Switches
    __SW1_CLEAN_PIN = SW1_PRESSED;
    __SW2_CLEAN_PIN = SW2_PRESSED;
    __SW3_CLEAN_PIN = SW3_PRESSED;

    // Check Double Press Timers
    if (__SW1_TIMER_VAL / CYCLES_PER_MS > _st_esos_uiF14Data.u16_SW1DoublePressedPeriod) {
        IC11CON2bits.ICTRIG = IC12CON2bits.ICTRIG = 1;
        __SW1_TIMER_RESET();
    }

    if (__SW2_TIMER_VAL / CYCLES_PER_MS > _st_esos_uiF14Data.u16_SW2DoublePressedPeriod) {
        IC13CON2bits.ICTRIG = IC14CON2bits.ICTRIG = 1;
        __SW2_TIMER_RESET();
    }

    if (__SW3_TIMER_VAL / CYCLES_PER_MS > _st_esos_uiF14Data.u16_SW3DoublePressedPeriod) {
        IC15CON2bits.ICTRIG = IC16CON2bits.ICTRIG = 1;
        __SW3_TIMER_RESET();
    }
}

// Debounce RPG
static uint8_t u8_rpg_prv = 3; // 3 = starting position
ESOS_USER_TIMER(__esos_uiF14_rpg_poll)
{
    __RPGA_CLEAN_PIN = RPGA;
    __RPGB_CLEAN_PIN = RPGB;

    // Check for increment/decrement condition
    if (u8_rpg_prv == 0 && (__RPG_VALUE == 1 || __RPG_VALUE == 2)) {
        _st_esos_uiF14Data.i16_RPGCounter += (__RPG_VALUE == 1) ? -1 : 1; // Increment/Decrement counter
    }
    u8_rpg_prv = __RPG_VALUE;
}

static volatile int32_t i32fp_vel_avg = 0, i32fp_vel_cur = 0, i32fp_pos_prev = 0, i32fp_pos_curr = 0;
ESOS_USER_TIMER(__esos_uiF14_rpg_vel)
{
    i32fp_pos_curr = ((int32_t)_st_esos_uiF14Data.i16_RPGCounter) << 12;

    i32fp_vel_cur = (i32fp_pos_curr - i32fp_pos_prev);
    i32fp_vel_avg = (3 * i32fp_vel_cur / 8) + (5 * i32fp_vel_avg / 8); // Moving exponential average
    _st_esos_uiF14Data.i16_RPGVelocity = i32fp_vel_avg;

    i32fp_pos_prev = i32fp_pos_curr;
}
#pragma endregion
#pragma region ESOS UI Interrupts
ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_CN)
{
    // Update LED Values
    (__LED1_IMDT_PIN) ? LED1_ON() : LED1_OFF();
    (__LED2_IMDT_PIN) ? LED2_ON() : LED2_OFF();
    (__LED3_IMDT_PIN) ? LED3_HB_ON() : LED3_HB_OFF();

    // Mark interrupt serviced
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_CN);
}

ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_IC11)
{
    // Read in required data
    BOOL b_sw_pressed = __SW1_CLEAN_PIN;

    // Determine if the event was a button press
    if (b_sw_pressed) {
        _st_esos_uiF14Data.b_SW1Pressed = TRUE;
        _st_esos_uiF14Data.b_SW1DoublePressed = FALSE; // Clear double pressed flag on subsequent press

        // Check if double press timer is already started
        if (IC11CON2bits.ICTRIG == 0) {
            // Check for double press
            if (__SW1_TIMER_VAL / CYCLES_PER_MS <= _st_esos_uiF14Data.u16_SW1DoublePressedPeriod) {
                _st_esos_uiF14Data.b_SW1DoublePressed = TRUE;
            }
            IC11CON2bits.ICTRIG = IC12CON2bits.ICTRIG = 1; // Stop double press timer
        } else {
            __SW1_TIMER_RESET();
            IC11CON2bits.ICTRIG = IC12CON2bits.ICTRIG = 0; // Start double press timer
        }
    } else {
        _st_esos_uiF14Data.b_SW1Pressed = FALSE;
    }

    // Mark interrupt serviced
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_IC11);
}

ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_IC13)
{
    // Read in required data
    BOOL b_sw_pressed = __SW2_CLEAN_PIN;

    // Determine if the event was a button press
    if (b_sw_pressed) {
        _st_esos_uiF14Data.b_SW2Pressed = TRUE;
        _st_esos_uiF14Data.b_SW2DoublePressed = FALSE; // Clear double pressed flag on subsequent press

        // Check if double press timer is already started
        if (IC13CON2bits.ICTRIG == 0) {
            // Check for double press
            if (__SW2_TIMER_VAL / CYCLES_PER_MS <= _st_esos_uiF14Data.u16_SW2DoublePressedPeriod) {
                _st_esos_uiF14Data.b_SW2DoublePressed = TRUE;
            }
            IC13CON2bits.ICTRIG = IC14CON2bits.ICTRIG = 1; // Stop double press timer
        } else {
            __SW2_TIMER_RESET();
            IC13CON2bits.ICTRIG = IC14CON2bits.ICTRIG = 0; // Start double press timer
        }

    } else {
        _st_esos_uiF14Data.b_SW2Pressed = FALSE;
    }

    // Mark interrupt serviced
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_IC13);
}

ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_IC15)
{
    // Read in required data
    BOOL b_sw_pressed = __SW3_CLEAN_PIN;

    // Determine if the event was a button press
    if (b_sw_pressed) {
        _st_esos_uiF14Data.b_SW3Pressed = TRUE;
        _st_esos_uiF14Data.b_SW3DoublePressed = FALSE; // Clear double pressed flag on subsequent press

        // Check if double press timer is already started
        if (IC15CON2bits.ICTRIG == 0) {
            // Check for double press
            if (__SW3_TIMER_VAL / CYCLES_PER_MS <= _st_esos_uiF14Data.u16_SW3DoublePressedPeriod) {
                _st_esos_uiF14Data.b_SW3DoublePressed = TRUE;
            }
            IC15CON2bits.ICTRIG = IC16CON2bits.ICTRIG = 1; // Stop double press timer
        } else {
            __SW3_TIMER_RESET();
            IC15CON2bits.ICTRIG = IC16CON2bits.ICTRIG = 0; // Start double press timer
        }

    } else {
        _st_esos_uiF14Data.b_SW3Pressed = FALSE;
    }

    // Mark interrupt serviced
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_IC15);
}
#pragma endregion

#pragma region SW1 Functions
inline BOOL esos_uiF14_isSW1Pressed(void)
{
    return _st_esos_uiF14Data.b_SW1Pressed;
}

inline BOOL esos_uiF14_isSW1Released(void)
{
    return !_st_esos_uiF14Data.b_SW1Pressed;
}

inline BOOL esos_uiF14_isSW1DoublePressed(void)
{
    if (_st_esos_uiF14Data.b_SW1DoublePressed) {
        _st_esos_uiF14Data.b_SW1DoublePressed = FALSE;
        return TRUE;
    } else {
        return FALSE;
    }
}
#pragma endregion
#pragma region SW2 Functions
inline BOOL esos_uiF14_isSW2Pressed(void)
{
    return _st_esos_uiF14Data.b_SW2Pressed;
}

inline BOOL esos_uiF14_isSW2Released(void)
{
    return !_st_esos_uiF14Data.b_SW2Pressed;
}

inline BOOL esos_uiF14_isSW2DoublePressed(void)
{
    if (_st_esos_uiF14Data.b_SW2DoublePressed) {
        _st_esos_uiF14Data.b_SW2DoublePressed = FALSE;
        return TRUE;
    } else {
        return FALSE;
    }
}
#pragma endregion
#pragma region SW3 Functions
inline BOOL esos_uiF14_isSW3Pressed(void)
{
    return _st_esos_uiF14Data.b_SW3Pressed;
}

inline BOOL esos_uiF14_isSW3Released(void)
{
    return !_st_esos_uiF14Data.b_SW3Pressed;
}

inline BOOL esos_uiF14_isSW3DoublePressed(void)
{
    if (_st_esos_uiF14Data.b_SW3DoublePressed) {
        _st_esos_uiF14Data.b_SW3DoublePressed = FALSE;
        return TRUE;
    } else
        return FALSE;
}
#pragma endregion

#pragma region LED1 Functions
inline void esos_uiF14_turnLED1On(void)
{
    // Disable flashing and set the LED on
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED1On = TRUE;
    __LED1_IMDT_PIN = 1;
    return;
}

inline void esos_uiF14_turnLED1Off(void)
{
    // Disable flashing and set the LED off
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED1On = FALSE;
    __LED1_IMDT_PIN = 0;
    return;
}

inline void esos_uiF14_toggleLED1(void)
{
    // Disable flashing and invert the LED state
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED1FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED1On ^= 1;
    __LED1_IMDT_PIN ^= 1;
    return;
}

inline void esos_uiF14_flashLED1(uint16_t u16_period)
{
    // Do nothing if same period is set
    if (u16_period == _st_esos_uiF14Data.u16_LED1FlashPeriod) {
        return;
    }

    // Set the period
    _st_esos_uiF14Data.u16_LED1FlashPeriod = u16_period;

    // If the period is zero, turn off the flasher and return
    if (u16_period == 0) {
        OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC11/12
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_OFF;
    OC11R = OC11RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC12R = OC12RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC11CON1bits.OCM = OC12CON1bits.OCM = OC_TOGGLE_PULSE; // OC12 must be enabled first
    return;
}
#pragma endregion
#pragma region LED2 Functions
inline void esos_uiF14_turnLED2On(void)
{
    // Disable flashing and set the LED on
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED2On = TRUE;
    __LED2_IMDT_PIN = 1;
    return;
}

inline void esos_uiF14_turnLED2Off(void)
{
    // Disable flashing and set the LED off
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED2On = FALSE;
    __LED2_IMDT_PIN = 0;
    return;
}

inline void esos_uiF14_toggleLED2(void)
{
    // Disable flashing and invert the LED state
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED2FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED2On ^= 1;
    __LED2_IMDT_PIN ^= 1;
    return;
}

inline void esos_uiF14_flashLED2(uint16_t u16_period)
{
    // Do nothing if same period is set
    if (u16_period == _st_esos_uiF14Data.u16_LED2FlashPeriod) {
        return;
    }

    // Set the period
    _st_esos_uiF14Data.u16_LED2FlashPeriod = u16_period;

    // If the period is zero, turn off the flasher and return
    if (u16_period == 0) {
        OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC13/14
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_OFF;
    OC13R = OC13RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC14R = OC14RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC13CON1bits.OCM = OC14CON1bits.OCM = OC_TOGGLE_PULSE; // OC14 must be enabled first
    return;
}
#pragma endregion
#pragma region LED3 Functions
inline void esos_uiF14_turnLED3On(void)
{
    // Disable flashing and set the LED on
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED3On = TRUE;
    __LED3_IMDT_PIN = 1;
    return;
}

inline void esos_uiF14_turnLED3Off(void)
{
    // Disable flashing and set the LED off
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED3On = FALSE;
    __LED3_IMDT_PIN = 0;
    return;
}

inline void esos_uiF14_toggleLED3(void)
{
    // Disable flashing and invert the LED state
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
    _st_esos_uiF14Data.u16_LED3FlashPeriod = 0;
    _st_esos_uiF14Data.b_LED3On ^= 1;
    __LED3_IMDT_PIN ^= 1;
    return;
}

inline void esos_uiF14_flashLED3(uint16_t u16_period)
{
    // Do nothing if same period is set
    if (u16_period == _st_esos_uiF14Data.u16_LED3FlashPeriod) {
        return;
    }

    // Set the period
    _st_esos_uiF14Data.u16_LED3FlashPeriod = u16_period;

    // If the period is zero, turn off the flasher and return
    if (u16_period == 0) {
        OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
        return;
    }

    // Enable the hardware flasher OC15/16
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_OFF;
    OC15R = OC15RS = ((u16_period * CYCLES_PER_MS) / 2) & 0x00FF;
    OC16R = OC16RS = ((u16_period * CYCLES_PER_MS) / 2) >> 16;
    OC15CON1bits.OCM = OC16CON1bits.OCM = OC_TOGGLE_PULSE; // OC16 must be enabled first
    return;
}
#pragma endregion
#pragma region RAG LED Functions
inline void esos_uiF14_turnRedLEDOn(void)
{
    esos_uiF14_turnLED1On();
}

inline void esos_uiF14_turnRedLEDOff(void)
{
    esos_uiF14_turnLED1Off();
}

inline void esos_uiF14_turnYellowLEDOn(void)
{
    esos_uiF14_turnLED2On();
}

inline void esos_uiF14_turnYellowLEDOff(void)
{
    esos_uiF14_turnLED2Off();
}

inline void esos_uiF14_turnGreenLEDOn(void)
{
    esos_uiF14_turnLED3On();
}

inline void esos_uiF14_turnGreenLEDOff(void)
{
    esos_uiF14_turnLED3Off();
}
#pragma endregion

#pragma region RPG Functions
inline void esos_uiF14_resetRPG(void) {
    _st_esos_uiF14Data.i16_RPGCounter = 0;
    _st_esos_uiF14Data.i16_RPGVelocity = 0;
    i32fp_vel_avg = i32fp_vel_cur = i32fp_pos_prev = i32fp_pos_curr = 0;
}

inline int16_t esos_uiF14_getRPGValue_i16(void)
{
    return _st_esos_uiF14Data.i16_RPGCounter;
}

inline int16_t esos_uiF14_getRPGVelocity_i16(void)
{
    return _st_esos_uiF14Data.i16_RPGVelocity;
}

inline BOOL esos_uiF14_isRPGTurning(void)
{
    return esos_uiF14_isRPGTurningCW() || esos_uiF14_isRPGTurningCCW();
}

inline BOOL esos_uiF14_isRPGTurningSlow(void)
{
    int16_t vel = ABS(_st_esos_uiF14Data.i16_RPGVelocity);
    return esos_uiF14_getRPGSlowThreshold() <= vel && vel < esos_uiF14_getRPGMediumThreshold();
}

inline BOOL esos_uiF14_isRPGTurningMedium(void)
{
    int16_t vel = ABS(_st_esos_uiF14Data.i16_RPGVelocity);
    return esos_uiF14_getRPGMediumThreshold() <= vel && vel < esos_uiF14_getRPGFastThreshold();
}

inline BOOL esos_uiF14_isRPGTurningFast(void)
{
    return esos_uiF14_getRPGFastThreshold() < ABS(_st_esos_uiF14Data.i16_RPGVelocity);
}

inline BOOL esos_uiF14_isRPGTurningCW(void)
{
    int16_t vel = _st_esos_uiF14Data.i16_RPGVelocity;
    return (vel > 0) && (esos_uiF14_getRPGSlowThreshold() <= ABS(vel));
}

inline BOOL esos_uiF14_isRPGTurningCCW(void)
{
    int16_t vel = _st_esos_uiF14Data.i16_RPGVelocity;
    return (vel < 0) && (esos_uiF14_getRPGSlowThreshold() <= ABS(vel));
}
#pragma endregion

#pragma region SW Settings
inline uint16_t esos_uiF14_getSW1DoublePressedPeriod(void)
{
    return _st_esos_uiF14Data.u16_SW1DoublePressedPeriod;
}

inline void esos_uiF14_setSW1DoublePressedPeriod(uint16_t period)
{
    _st_esos_uiF14Data.u16_SW1DoublePressedPeriod = period;
}
inline uint16_t esos_uiF14_getSW2DoublePressedPeriod(void)
{
    return _st_esos_uiF14Data.u16_SW2DoublePressedPeriod;
}

inline void esos_uiF14_setSW2DoublePressedPeriod(uint16_t period)
{
    _st_esos_uiF14Data.u16_SW2DoublePressedPeriod = period;
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
#pragma region RPG Settings
inline int16_t esos_uiF14_getRPGSlowThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGSlowThreshold;
}

inline void esos_uiF14_setRPGSlowThreshold(int16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGSlowThreshold = threshold;
}

inline int16_t esos_uiF14_getRPGMediumThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGMediumThreshold;
}

inline void esos_uiF14_setRPGMediumThreshold(int16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGMediumThreshold = threshold;
}

inline int16_t esos_uiF14_getRPGFastThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGFastThreshold;
}

inline void esos_uiF14_setRPGFastThreshold(int16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGFastThreshold = threshold;
}
#pragma endregion

#pragma region ESOS UIF14 Config Function
void config_esos_uiF14()
{
    // Configure LEDs/OCs and Intermediate Pin RF1/2/3 ICs/Interrupts
    __LED1_UI_CONFIG();
    __LED2_UI_CONFIG();
    __LED3_UI_CONFIG();

    // Configure Switches
    __SW1_UI_CONFIG();
    __SW2_UI_CONFIG();
    __SW3_UI_CONFIG();

    // Configure RPG
    __RPG_UI_CONFIG();

    // Set default threshold/period values
    esos_uiF14_setSW1DoublePressedPeriod(__ESOS_UIF14_DEFAULT_SWDP_PERIOD);
    esos_uiF14_setSW2DoublePressedPeriod(__ESOS_UIF14_DEFAULT_SWDP_PERIOD);
    esos_uiF14_setSW3DoublePressedPeriod(__ESOS_UIF14_DEFAULT_SWDP_PERIOD);
    esos_uiF14_setRPGSlowThreshold(__ESOS_UIF14_DEFAULT_RPGS_THRESHOLD);
    esos_uiF14_setRPGMediumThreshold(__ESOS_UIF14_DEFAULT_RPGM_THRESHOLD);
    esos_uiF14_setRPGFastThreshold(__ESOS_UIF14_DEFAULT_RPGF_THRESHOLD);

    // Register and start tasks/timers
    esos_RegisterTimer(__esos_uiF14_sw_poll, __ESOS_UIF14_SW_POLL_RATE);
    esos_RegisterTimer(__esos_uiF14_rpg_poll, __ESOS_UIF14_RPG_POLL_RATE);
    esos_RegisterTimer(__esos_uiF14_rpg_vel, __ESOS_UIF14_RPG_VEL_RATE);
}
#pragma endregion