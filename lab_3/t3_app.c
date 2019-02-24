#include "esos_f14ui.h"
#include "esos.h"
#include "esos_pic24.h"

#include "t3_strings.h"
#include "t3_app_menu.h"

ESOS_USER_TASK(rpg_interface)
{
    ESOS_TASK_BEGIN();

    static BOOL b_rpg_prev, b_rpg_slow_prev, b_rpg_med_prev, b_rpg_fast_prev, b_rpg_cw_prev, b_rpg_ccw_prev;
    while (TRUE) {
        if (esos_uiF14_isRPGTurning() && esos_uiF14_isRPGTurning() != b_rpg_prev) {
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_TURNING);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }

        if (!esos_uiF14_isRPGTurning()) {
            esos_uiF14_turnLED2Off();
        }

        if (esos_uiF14_isRPGTurningSlow() && esos_uiF14_isRPGTurningSlow() != b_rpg_slow_prev) {
            esos_uiF14_turnLED2On();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_SLOW);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (esos_uiF14_isRPGTurningMedium() && esos_uiF14_isRPGTurningMedium() != b_rpg_med_prev) {
            esos_uiF14_flashLED2(500);
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_TURNING);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_MEDIUM);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (esos_uiF14_isRPGTurningFast() && esos_uiF14_isRPGTurningFast() != b_rpg_fast_prev) {
            esos_uiF14_flashLED2(100);
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_TURNING);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_FAST);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }

        if (esos_uiF14_isRPGTurningCW() && esos_uiF14_isRPGTurningCW() != b_rpg_cw_prev) {
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_CW);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (esos_uiF14_isRPGTurningCCW() && esos_uiF14_isRPGTurningCCW() != b_rpg_ccw_prev) {
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_RPG_CCW);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }

        b_rpg_prev = esos_uiF14_isRPGTurning();
        b_rpg_slow_prev = esos_uiF14_isRPGTurningSlow();
        b_rpg_med_prev = esos_uiF14_isRPGTurningMedium();
        b_rpg_fast_prev = esos_uiF14_isRPGTurningFast();
        b_rpg_cw_prev = esos_uiF14_isRPGTurningCW();
        b_rpg_ccw_prev = esos_uiF14_isRPGTurningCCW();

        ESOS_TASK_YIELD();
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

            esos_uiF14_flashLED1(250);
            ESOS_TASK_WAIT_TICKS(750);
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
};

ESOS_USER_TASK(sw_feedback)
{
    static BOOL e_SW1_PRESSED;
    static BOOL e_SW2_PRESSED;
    static BOOL e_SW3_PRESSED;
    static BOOL e_SW1_DPRESSED;
    static BOOL e_SW2_DPRESSED;
    static BOOL e_SW3_DPRESSED;

    ESOS_TASK_BEGIN();
    while (TRUE) {
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
        if (e_SW1_DPRESSED != esos_uiF14_isSW1DoublePressed()) {
            e_SW1_DPRESSED = esos_uiF14_isSW1DoublePressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW1_DPRESSED);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (e_SW2_DPRESSED != esos_uiF14_isSW2DoublePressed()) {
            e_SW2_DPRESSED = esos_uiF14_isSW2DoublePressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW2_DPRESSED);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (e_SW3_DPRESSED != esos_uiF14_isSW3DoublePressed()) {
            e_SW3_DPRESSED = esos_uiF14_isSW3DoublePressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW3_DPRESSED);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();
    esos_uiF14_flashLED3(500); // heartbeat LED

    esos_RegisterTask(rpg_interface);
    esos_RegisterTask(switch_interface);
    esos_RegisterTask(sw_feedback);
    esos_RegisterTask(menu);
}