/*******************************************************************
 * 
 * C header file for ESOS user-interface (UI) service
 * 
 *    requires the EMBEDDED SYSTEMS target rev. F14
 * 
 * ****************************************************************/

#ifndef ESOS_UIF14_H
#define ESOS_UIF14_H

#include "esos.h"
#include "esos_pic24.h"

// STRUCTURES

typedef struct {
    BOOL b_SW1Pressed;
    BOOL b_SW1DoublePressed;
    BOOL b_SW2Pressed;
    BOOL b_SW1DoublePressed;    
    BOOL b_SW3Pressed;
    BOOL b_SW1DoublePressed;    
    
    BOOL b_RPGAHigh;
    BOOL b_RPGBHigh;
    
    BOOL b_LED1On;
    uint16_t u16_LED1FlashPeriod;    
    BOOL b_LED2On;
    uint16_t u16_LED2FlashPeriod;        
    BOOL b_LED3On;
    uint16_t u16_LED3FlashPeriod;        
    
    uint16_t u16_RPGCounter;
    uint16_t u16_lastRPGCounter;
} _st_esos_uiF14Data_t;

// DEFINEs and CONSTANTs
#define __ESOS_TICKS_TO_MS(x)           (x/1)
#define __ESOS_MS_TO_TICKS(x)           (x*1)
#define __ESOS_UIF14_UI_PERIOD_MS       10

// PRIVATE DATA 
 
_st_esos_uiF14Data_t _st_esos_uiF14Data;

// PRIVATE FUNCTION PROTOTYPES

uint16_t esos_uiF14_getRPGCounter (void);
void esos_ui_setRPGCounter (uint16_t);

uint16_t esos_uiF14_getLastRPGCounter (void);
void esos_ui_setLastRPGCounter (uint16_t);

ESOS_USER_TASK __uiF14_task;

// PUBLIC API FUNCTION PROTOTYPES

inline BOOL esos_uiF14_isSW1Pressed (void);
inline BOOL esos_uiF14_isSW1Released (void);
inline BOOL esos_uiF14_isSW1DoublePressed (void);

inline BOOL esos_uiF14_isSW2Pressed (void);
inline BOOL esos_uiF14_isSW2Released (void);
inline BOOL esos_uiF14_isSW2DoublePressed (void);

inline BOOL esos_uiF14_isSW3Pressed (void);
inline BOOL esos_uiF14_isSW3Released (void);
inline BOOL esos_uiF14_isSW3DoublePressed (void);

inline void esos_uiF14_turnLED1On (void);
inline void esos_uiF14_turnLED1Off (void);
inline void esos_uiF14_toggleLED1 (void);
inline void esos_uiF14_flashLED1 (uint16_t);

inline void esos_uiF14_turnLED2On (void);
inline void esos_uiF14_turnLED2Off (void);
inline void esos_uiF14_toggleLED2 (void);
inline void esos_uiF14_flashLED2 (uint16_t);

inline void esos_uiF14_turnLED3On (void);
inline void esos_uiF14_turnLED3Off (void);
inline void esos_uiF14_toggleLED3 (void);
inline void esos_uiF14_flashLED3 (uint16_t);

inline void esos_uiF14_turnRedLEDOn (void);
inline void esos_uiF14_turnRedLEDOff (void);
inline void esos_uiF14_turnGreenLEDOn (void);
inline void esos_uiF14_turnGreenLEDOff (void);
inline void esos_uiF14_turnYellowLEDOn (void);
inline void esos_uiF14_turnYellowLEDOff (void);

inline uint16_t esos_uiF14_getRPGValue_u16 (void);
inline BOOL esos_uiF14_isRPGTurning (void);
inline BOOL esos_uiF14_isRPGTurningSlow (void);
inline BOOL esos_uiF14_isRPGTurningMedium (void);
inline BOOL esos_uiF14_isRPGTurningFast (void);
inline BOOL esos_uiF14_isRPGTurningCW (void);
inline BOOL esos_uiF14_isRPGTurningCCW (void);

void config_esos_uiF14();
int16_t esos_uiF14_getRPGVelocity_i16 (void);

// PUBLIC API ESOS TASK MACROS

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED()              ESOS_TASK_WAIT_UNTIL( esos_uiF14_isSW1Pressed() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED()             ESOS_TASK_WAIT_UNTIL( esos_uiF14_isSW1Released() )
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED_AND_RELEASED() do {            /
                            ESOS_TASK_WAIT_UNTIL_UIF14_SW1_PRESSED();           /
                            ESOS_TASK_WAIT_UNTIL_UIF14_SW1_RELEASED();          /
                          } while (0) 
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW1_DOUBLE_PRESSED()       // not yet implemented - christian

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED()              // not yet implemented - jack
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_RELEASED()             // not yet implemented - jack
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_PRESSED_AND_RELEASED() // not yet implemented - jack
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW2_DOUBLE_PRESSED()       // not yet implemented - christian

#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED()              // not yet implemented - jack
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED()             // not yet implemented - jack
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED_AND_RELEASED() // not yet implemented - jack
#define ESOS_TASK_WAIT_UNTIL_UIF14_SW3_DOUBLE_PRESSED()       // not yet implemented - christian

#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS()          // not yet implemented - spencer
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS_CW()       // not yet implemented - spencer
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_UNTIL_TURNS_CCW()      // not yet implemented - spencer
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM()         // not yet implemented - will
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM_CW()      // not yet implemented - will
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_MEDIUM_CCW()     // not yet implemented - will
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST()           // not yet implemented
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST_CW()        // not yet implemented
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_TURNS_FAST_CCW()       // not yet implemented
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_REV(y)           // not yet implemented
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_CW_REV(y)        // not yet implemented
#define ESOS_TASK_WAIT_UNTIL_UIF14_RPG_MAKES_CCW_REV(y)       // not yet implemented


#endif    // ESOS_UIF14_H
