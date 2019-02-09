/*******************************************************************
 *
 * C code framework for ESOS user-interface (UI) service
 *
 *    requires the EMBEDDED SYSTEMS taris rev. F14
 *
 * ****************************************************************/
#include "esos_f14ui.h";

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
    return (_st_esos_uiF14Data.b_SW1DoublePressed == TRUE);
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
    return (_st_esos_uiF14Data.b_SW2DoublePressed == TRUE);
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
    LED3_ON();
    return;
}

inline void esos_uiF14_turnLED3Off(void)
{
    _st_esos_uiF14Data.b_LED3On = FALSE;
    LED3_OFF();
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
    LED3_ON();
    return;
}

inline void esos_uiF14_turnGreenLEDOff(void)
{
    _st_esos_uiF14Data.b_LED3On = FALSE;
    LED3_OFF();
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
    BOOL res = (v > 0) && (v < esos_uiF14_getRPGSlowThreshold());
    res = res || (v < 0 && v > -(esos_uiF14_getRPGSlowThreshold()));
    return res;
}

inline uin16_t esos_uiF14_getRPGSlowThreshold(void)
{
    return _st_esos_uiF14Data.u16_RPGSlowThreshold;
}

inline BOOL esos_uiF14_setRPGSlowThreshold(uint16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGSlowThreshold = threshold;
}

inline BOOL esos_uiF14_isRPGTurningMedium(void)
{
    int16_t v = esos_uiF14_getRPGVelocity_i16();
    BOOL res = (v > esos_uiF14_getRPGSlowThreshold()) && (v < esos_uif14_getRPGMediumThreshold());
    res = res || (v < -(esos_uiF14_getRPGSlowThreshold()) && v > -(esos_uif14_getRPGMediumThreshold()));
    return res;
}

inline uin16_t esos_uiF14_getRPGMediumThreshold(void)
{
    _st_esos_uiF14Data.u16_RPGMediumThreshold = threshold;
}

inline BOOL esos_uiF14_setRPGMediumThreshold(uint16_t threshold)
{
    _st_esos_uiF14Data.u16_RPGMediumThreshold = threshold;
}

inline BOOL esos_uiF14_isRPGTurningFast(void)
{
    int16_t v = esos_uiF14_getRPGVelocity_i16();
    BOOL res = (v > esos_uiF14_getRPGMediumThreshold()) && (v < esos_uif14_getRPGFastThreshold());
    res = res || (v < -(esos_uiF14_getRPGMediumThreshold()) && v > -(esos_uif14_getRPGFastThreshold()));
    return res;
}

inline uin16_t esos_uiF14_getRPGFastThreshold(void)
{
    _st_esos_uiF14Data.u16_RPGFastThreshold = threshold;
}

inline BOOL esos_uiF14_setRPGFastThreshold(uint16_t threshold)
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

int16_t esos_uiF14_getRPGVelocity_i16(void)
{
    return _st_esos_uiF14Data.i16_RPGVelocity;
}

ESOS_USER_TASK(__esos_uiF14_update_rpg_velocity)
{
    _st_esos_uiF14Data.i16_RPGVelocity = _st_esos_uiF14Data.u16_RPGCounter - _st_esos_uiF14Data.u16_lastRPGCounter;
    _esos_uiF14_setLastRPGCounter(esos_uiF14_getRPGCounter());
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
	
    esos_RegisterTask(__esos_uiF14_task);
}

// UIF14 task to manage user-interface
ESOS_USER_TASK(__esos_uiF14_task)
{
    static int16_t _i16_LED1FlashTicksRemaining = 0;
 	static int16_t _i16_LED2FlashTicksRemaining = 0;
 	static int16_t _i16_LED3FlashTicksRemaining = 0;
 
     esos_uiF14_setSWDoublePressedPeriod(1000);
 
     esos_uiF14_setRPGSlowThreshold(200);
     esos_uiF14_setRPGMediumThreshold(100);
     esos_uiF14_setRPGFastThreshold(10);
 
     ESOS_TASK_BEGIN();
 
             _st_esos_uiF14Data.b_SW3Pressed = FALSE;
         }
 
         if (RPGA_HIGH) {
             _st_esos_uiF14Data.b_RPGAHigh = TRUE;
         }
         else {
             _st_esos_uiF14Data.b_RPGAHigh = FALSE;
         }
 
         ESOS_TASK_WAIT_TICKS(__ESOS_UIF14_UI_PERIOD_MS);
     }
     ESOS_TASK_END();
}
