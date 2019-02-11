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
            esos_uiF14_flashLED2(250);
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

    ESOS_TASK_BEGIN();
    while (TRUE) {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        e_RPG_COUNTER = _esos_uiF14_getRPGCounter();
        ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(e_RPG_COUNTER);
        ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        if (e_SW1_PRESSED != esos_uiF14_isSW1Pressed()) {
            e_SW1_PRESSED = esos_uiF14_isSW1Pressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_SW1_PRESSED) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW1_PRESSED);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW1_RELEASED);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (e_SW2_PRESSED != esos_uiF14_isSW2Pressed()) {
            e_SW2_PRESSED = esos_uiF14_isSW2Pressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_SW2_PRESSED) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW2_PRESSED);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW2_RELEASED);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (e_SW3_PRESSED != esos_uiF14_isSW3Pressed()) {
            e_SW3_PRESSED = esos_uiF14_isSW3Pressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_SW3_PRESSED) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW3_PRESSED);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW3_RELEASED);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (e_LED1_ON != esos_uiF14_isLED1On()) {
            e_LED1_ON = esos_uiF14_isLED1On();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_LED1_ON) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED1_ON);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED1_OFF);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (e_LED2_ON != esos_uiF14_isLED2On()) {
            e_LED2_ON = esos_uiF14_isLED2On();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_LED2_ON) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED2_ON);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED2_OFF);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        /*if (e_LED3_ON != esos_uiF14_isLED3On()) {
            e_LED3_ON = esos_uiF14_isLED3On();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_LED3_ON) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED3_ON);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_LED3_OFF);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }*/
        if(e_RPG_TURNING != esos_uiF14_isRPGTurning()) {
            e_RPG_TURNING = esos_uiF14_isRPGTurning();
            e_RPG_CW = esos_uiF14_isRPGTurningCW();
            e_RPG_CCW = esos_uiF14_isRPGTurningCCW();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_TURNING);
            if(e_RPG_CW) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_CW);
            } else if (e_RPG_CCW) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_CCW);
            } else {
                //do nothing
            }
            e_RPG_VELOCITY = esos_uiF14_getRPGVelocity();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_VELOCITY);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(e_RPG_VELOCITY);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }

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