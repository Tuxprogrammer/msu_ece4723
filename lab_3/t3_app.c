#include "esos_f14ui.h"
#include "esos.h"
#include "esos_pic24.h"
#include "t3_strings.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(rpg_interface)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if (!(esos_uiF14_isRPGTurning())) {
            esos_uiF14_turnLED2Off();
            esos_uiF14_flashLED2(0);
        } else if (esos_uiF14_isRPGTurningSlow()) {
            esos_uiF14_turnLED2On();
            esos_uiF14_flashLED2(0);
        } else if (esos_uiF14_isRPGTurningMedium()) {
            esos_uiF14_turnLED2On();
            esos_uiF14_flashLED2(500);
        } else if (esos_uiF14_isRPGTurningFast()) {
            esos_uiF14_turnLED2On();
            esos_uiF14_flashLED2(100);
        }
        ESOS_TASK_WAIT_TICKS(1000);
    }

    ESOS_TASK_END();
};

ESOS_USER_TASK(switch_interface)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if ((esos_uiF14_isSW3Pressed() && esos_uiF14_isSW2Pressed()) ||
            (!esos_uiF14_isSW3Pressed() && esos_uiF14_isSW1Pressed())) {
            esos_uiF14_turnLED1On();
        } else if ((!esos_uiF14_isSW2Pressed() && esos_uiF14_isSW3Pressed()) ||
                   (!esos_uiF14_isSW1Pressed() && !esos_uiF14_isSW3Pressed())) {
            esos_uiF14_turnLED1Off();
        }

        if ((esos_uiF14_isSW3Pressed() && esos_uiF14_isSW2DoublePressed()) ||
            (!esos_uiF14_isSW3Pressed() && esos_uiF14_isSW1DoublePressed())) {
            static uint8_t count;
            esos_uiF14_turnLED1Off();

            for (count = 0; count < 6; count++) {
                esos_uiF14_toggleLED1();
                ESOS_TASK_WAIT_TICKS(100);
            }
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
};

ESOS_USER_TASK(feedback)
{
    static BOOL e_SW1_PRESSED;
    static BOOL e_SW2_PRESSED;
    static BOOL e_SW3_PRESSED;
    static BOOL e_LED1_ON;
    static BOOL e_LED2_ON;
    static BOOL e_LED3_ON;
    static BOOL e_RPG_TURNING;
    static BOOL e_RPG_CW;
    static BOOL e_RPG_CCW;
    static BOOL e_RPG_SLOW;
    static BOOL e_RPG_MEDIUM;
    static BOOL e_RPG_FAST;
    static uint16_t e_LAST_RPG_COUNTER;
    static uint16_t e_RPG_COUNTER;
    static int16_t e_RPG_VELOCITY;

    e_SW1_PRESSED = esos_uiF14_isSW1Pressed();
    e_SW2_PRESSED = esos_uiF14_isSW2Pressed();
    e_SW3_PRESSED = esos_uiF14_isSW3Pressed();
    e_LED1_ON = esos_uiF14_isLED1On();
    e_LED2_ON = esos_uiF14_isLED2On();
    e_LED3_ON = esos_uiF14_isLED3On();
    e_RPG_TURNING = esos_uiF14_isRPGTurning();
    e_RPG_CW = esos_uiF14_isRPGTurningCW();
    e_RPG_CCW = esos_uiF14_isRPGTurningCCW();
    e_RPG_SLOW = esos_uiF14_isRPGTurningSlow();
    e_RPG_MEDIUM = esos_uiF14_isRPGTurningMedium();
    e_RPG_FAST = esos_uiF14_isRPGTurningFast();
    e_LAST_RPG_COUNTER = _esos_uiF14_getLastRPGCounter();
    e_RPG_COUNTER = _esos_uiF14_getRPGCounter();
    e_RPG_VELOCITY = esos_uiF14_getRPGVelocity();

    ESOS_TASK_BEGIN();
    while (TRUE) {
        // I TRIED FOR TWO HOURS TO MAKE THESE A DEFINE, BUT THIS !@#%^&!%@# LIBRARY WON'T LET ME
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_SIGNAL_BUSY_OUT_COMM();
        if (e_SW1_PRESSED != esos_uiF14_isSW1Pressed()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW1_PRESSED);
        }
        if (e_SW2_PRESSED != esos_uiF14_isSW2Pressed()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW2_PRESSED);
        }
        if (e_SW3_PRESSED != esos_uiF14_isSW3Pressed()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW3_PRESSED);
        }
        if (e_LED1_ON != esos_uiF14_isLED1On()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED1_ON);
        }
        if (e_LED2_ON != esos_uiF14_isLED2On()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED2_ON);
        }
        if (e_LED3_ON != esos_uiF14_isLED3On()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED3_ON);
        }
        if (e_RPG_TURNING != esos_uiF14_isRPGTurning()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_TURNING);
        }
        if (e_RPG_CW != esos_uiF14_isRPGTurningCW()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_CW);
        }
        if (e_RPG_CCW != esos_uiF14_isRPGTurningCCW()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_CCW);
        }
        if (e_RPG_SLOW != esos_uiF14_isRPGTurningSlow()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_SLOW);
        }
        if (e_RPG_MEDIUM != esos_uiF14_isRPGTurningMedium()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_MEDIUM);
        }
        if (e_RPG_FAST != esos_uiF14_isRPGTurningFast()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_FAST);
        }
        if (e_LAST_RPG_COUNTER != _esos_uiF14_getLastRPGCounter()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LAST_RPG_COUNTER);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)e_LAST_RPG_COUNTER);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
        }
        if (e_RPG_COUNTER != _esos_uiF14_getRPGCounter()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_COUNTER);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)e_RPG_COUNTER);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
        }
        if (e_RPG_VELOCITY != esos_uiF14_getRPGVelocity()) {
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_VELOCITY);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)e_RPG_VELOCITY);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
        }
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_RELEASE_OUT_COMM();


        // UPDATE previous values
        e_SW1_PRESSED = esos_uiF14_isSW1Pressed();
        e_SW2_PRESSED = esos_uiF14_isSW2Pressed();
        e_SW3_PRESSED = esos_uiF14_isSW3Pressed();
        e_LED1_ON = esos_uiF14_isLED1On();
        e_LED2_ON = esos_uiF14_isLED2On();
        e_LED3_ON = esos_uiF14_isLED3On();
        e_RPG_TURNING = esos_uiF14_isRPGTurning();
        e_RPG_CW = esos_uiF14_isRPGTurningCW();
        e_RPG_CCW = esos_uiF14_isRPGTurningCCW();
        e_RPG_SLOW = esos_uiF14_isRPGTurningSlow();
        e_RPG_MEDIUM = esos_uiF14_isRPGTurningMedium();
        e_RPG_FAST = esos_uiF14_isRPGTurningFast();
        e_LAST_RPG_COUNTER = _esos_uiF14_getLastRPGCounter();
        e_RPG_COUNTER = _esos_uiF14_getRPGCounter();
        e_RPG_VELOCITY = esos_uiF14_getRPGVelocity();
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}
ESOS_USER_TASK(menu)
{
    ESOS_TASK_BEGIN();
    ESOS_TASK_YIELD();
    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();

    esos_RegisterTimer(heartbeat, 500);
    esos_RegisterTask(rpg_interface);
    esos_RegisterTask(switch_interface);
    esos_RegisterTask(feedback);
    esos_RegisterTask(menu);
}