/*******************************************************************
 * 
 * C code framework for ESOS user-interface (UI) service
 * 
 *    requires the EMBEDDED SYSTEMS target rev. F14
 * 
 * ****************************************************************/

#include "esos_f14ui.h"
#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"


// PRIVATE FUNCTIONS
inline void _esos_uiF14_setRPGCounter(uint16_t newValue) {
    _st_esos_uiF14Data.u16_RPGCounter = newValue;
    return;
}

inline void _esos_uiF14_setLastRPGCounter(uint16_t newValue) {
    _st_esos_uiF14Data.u16_lastRPGCounter = newValue;
    return;
}

// PUBLIC SWITCH FUNCTIONS
inline BOOL esos_uiF14_getSW1Pressed(void) {
    return (_st_esos_uiF14Data.b_SW1Pressed == TRUE);
}

inline BOOL esos_uiF14_getSW1Released(void) {
    return (_st_esos_uiF14Data.b_SW1Pressed == FALSE);
}

inline BOOL esos_uiF14_getSW1DoublePressed(void) {
    return (_st_esos_uiF14Data.b_SW1DoublePressed == TRUE);
}

/***** SW2 and SW3 need need similar  *****/

// PUBLIC LED FUNCTIONS

inline BOOL esos_uiF14_isLED1On(void) {
    return (_st_esos_uiF14Data.b_LED1On == TRUE);
}

inline BOOL esos_uiF14_isLED1Off(void) {
    return (_st_esos_uiF14Data.b_LED1On == FALSE);
}

inline void esos_uiF14_turnLED1On(void) {
    _st_esos_uiF14Data.b_LED1On = TRUE;
    return;
}

inline void esos_uiF14_turnLED1Off(void) {
    _st_esos_uiF14Data.b_LED1On = FALSE;
    return;
}

inline void esos_uiF14_toggleLED1(void) {
    _st_esos_uiF14Data.b_LED1On ^= 1;
    return;
}

inline void esos_uiF14_flashLED1(uint16_t u16_period) {
    _st_esos_uiF14Data.u16_LED1FlashPeriod = u16_period;
    return;
}

/****** LED2 and LED3 will need similar.  ********/
/****** RED, GREEN, and YELLOW functions need to be created *******/

// PUBLIC RPG FUNCTIONS

inline uint16_t esos_uiF14_getRpgValue_u16(void) {
    return _st_esos_uiF14Data.u16_RPGCounter;
}

inline BOOL esos_uiF14_isRpgTurning(void) {
    return (esos_uiF14_getRpgVelocity_i16() != 0);
}

inline BOOL esos_uiF14_isRpgTurningSlow(void) {
    // not yet implemented
}

inline BOOL esos_uiF14_isRpgTurningMedium(void) {
    // not yet implemented
}

inline BOOL esos_uiF14_isRpgTurningFast(void) {
    // not yet implemented
}

inline BOOL esos_uiF14_isRpgTurningCW(void) {
    // not yet implemented
}

inline BOOL esos_uiF14_isRpgTurningCCW(void) {
    // not yet implemented
}

int16_t esos_uiF14_getRpgVelocity_i16(void) {
    // not yet implemented
}

// UIF14 INITIALIZATION FUNCTION

void config_esos_uiF14() {
    // setup your UI implementation

    // configure LEDs
    LED1_CONFIG();
    LED2_CONFIG();
    LED3_HB_CONFIG();

    // configure Switches
    SW1_CONFIG();
    SW2_CONFIG();
    SW3_CONFIG();

    esos_RegisterTask(__uiF14_task);
}


// UIF14 task to manage user-interface
ESOS_USER_TASK(__esos_uiF14_task) {
	static int16_t _i16_LED1FlashTicksRemaining = 0;
	static int16_t _i16_LED2FlashTicksRemaining = 0;
	static int16_t _i16_LED3FlashTicksRemaining = 0;

    esos_uiF14_setSWDoublePressedPeriod(1000);

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
        }
        else {
            _st_esos_uiF14Data.b_SW1Pressed = FALSE;
        }

        if (SW2_PRESSED) {
            _st_esos_uiF14Data.b_SW2Pressed = TRUE;
        }
        else {
            _st_esos_uiF14Data.b_SW2Pressed = FALSE;
        }

        if (SW3_PRESSED) {
            _st_esos_uiF14Data.b_SW3Pressed = TRUE;
        }
        else {
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
