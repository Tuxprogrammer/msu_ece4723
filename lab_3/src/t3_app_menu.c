#include "esos_f14ui.h"
#include "esos.h"
#include "esos_pic24.h"
#include "t3_strings.h"

#include "t3_app_menu.h"

ESOS_USER_TASK(menu)
{
    // State and input variables
    static uint8_t u8_input = 0x00;
    static enum MENU_STATE menu_state = TOP_MENU;

    ESOS_TASK_BEGIN();

    while (TRUE) {
        if (menu_state == TOP_MENU) {
            // Print menu options
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_DBL_PRESS_SETTINGS);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_RPG_THRESH_SETTINGS);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Set next state based on selection
            switch (u8_input) {
            case '1':
                menu_state = DBL_PRESS_SETTINGS;
                break;
            case '2':
                menu_state = RPG_THRESH_SETTINGS;
                break;
            default:
                menu_state = TOP_MENU;
                break;
            }
        } else if (menu_state == DBL_PRESS_SETTINGS) {
            // Print menu options
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mdp_SW1_PERIOD);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mdp_SW2_PERIOD);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mdp_SW3_PERIOD);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Set next state based on selection
            switch (u8_input) {
            case '1':
                menu_state = SW1_PERIOD;
                break;
            case '2':
                menu_state = SW2_PERIOD;
                break;
            case '3':
                menu_state = SW3_PERIOD;
                break;
            case ESC_KEY:
                menu_state = TOP_MENU;
                break;
            default:
                menu_state = DBL_PRESS_SETTINGS;
                break;
            }
        } else if (menu_state == RPG_THRESH_SETTINGS) {
            // Print menu options
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mrpg_RPG_SLOW);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mrpg_RPG_MED);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mrpg_RPG_FAST);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Set next state based on selection
            switch (u8_input) {
            case '1':
                menu_state = RPG_SLOW;
                break;
            case '2':
                menu_state = RPG_MED;
                break;
            case '3':
                menu_state = RPG_FAST;
                break;
            case ESC_KEY:
                menu_state = TOP_MENU;
                break;
            default:
                menu_state = RPG_THRESH_SETTINGS;
                break;
            }
        } else if (menu_state == SW1_PERIOD) {
            // Print current value
            static uint16_t u16_sw1_period_val;
            u16_sw1_period_val = esos_uiF14_getSW1DoublePressedPeriod();

            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_sw1p_CUR_VALUE);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)u16_sw1_period_val);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Perform correct action based on selection
            switch (u8_input) {
            case PLUS_KEY:
                u16_sw1_period_val = esos_uiF14_getSW1DoublePressedPeriod();
                esos_uiF14_setSW1DoublePressedPeriod(u16_sw1_period_val + 1);
                break;
            case MINUS_KEY:
                u16_sw1_period_val = esos_uiF14_getSW1DoublePressedPeriod();
                esos_uiF14_setSW1DoublePressedPeriod(u16_sw1_period_val - 1);
                break;
            case ESC_KEY:
                menu_state = DBL_PRESS_SETTINGS;
                break;
            default:
                menu_state = SW1_PERIOD;
                break;
            }
        } else if (menu_state == SW2_PERIOD) {
            // Print current value
            static uint16_t u16_sw2_period_val;
            u16_sw2_period_val = esos_uiF14_getSW2DoublePressedPeriod();

            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_sw2p_CUR_VALUE);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)u16_sw2_period_val);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Perform correct action based on selection
            switch (u8_input) {
            case PLUS_KEY:
                u16_sw2_period_val = esos_uiF14_getSW2DoublePressedPeriod();
                esos_uiF14_setSW2DoublePressedPeriod(u16_sw2_period_val + 1);
                break;
            case MINUS_KEY:
                u16_sw2_period_val = esos_uiF14_getSW2DoublePressedPeriod();
                esos_uiF14_setSW2DoublePressedPeriod(u16_sw2_period_val - 1);
                break;
            case ESC_KEY:
                menu_state = DBL_PRESS_SETTINGS;
                break;
            default:
                menu_state = SW2_PERIOD;
                break;
            }
        } else if (menu_state == SW3_PERIOD) {
            // Print current value
            static uint16_t u16_sw3_period_val;
            // u16_sw3_period_val = esos_uiF14_getSW3DoublePressedPeriod(); TODO

            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_sw3p_CUR_VALUE);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)u16_sw3_period_val);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Perform correct action based on selection
            switch (u8_input) {
            case PLUS_KEY:
                // u16_sw3_period_val = esos_uiF14_getSW3DoublePressedPeriod(); TODO
                // esos_uiF14_setSW3DoublePressedPeriod(u16_sw3_period_val + 1); TODO
                break;
            case MINUS_KEY:
                // u16_sw3_period_val = esos_uiF14_getSW3DoublePressedPeriod(); TODO
                // esos_uiF14_setSW3DoublePressedPeriod(u16_sw3_period_val - 1); TODO
                break;
            case ESC_KEY:
                menu_state = DBL_PRESS_SETTINGS;
                break;
            default:
                menu_state = SW3_PERIOD;
                break;
            }
        } else if (menu_state == RPG_SLOW) {
            // Print current value
            static uint16_t i16_rpg_slow_val;
            i16_rpg_slow_val = esos_uiF14_getRPGSlowThreshold();

            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_rpgs_CUR_VALUE);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)i16_rpg_slow_val);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Perform correct action based on selection
            switch (u8_input) {
            case PLUS_KEY:
                i16_rpg_slow_val = esos_uiF14_getRPGSlowThreshold();
                esos_uiF14_setRPGSlowThreshold(i16_rpg_slow_val + 1);
                break;
            case MINUS_KEY:
                i16_rpg_slow_val = esos_uiF14_getRPGSlowThreshold();
                esos_uiF14_setRPGSlowThreshold(i16_rpg_slow_val - 1);
                break;
            case ESC_KEY:
                menu_state = RPG_THRESH_SETTINGS;
                break;
            default:
                menu_state = RPG_SLOW;
                break;
            }
        } else if (menu_state == RPG_MED) {
            // Print current value
            static uint16_t i16_rpg_med_val;
            i16_rpg_med_val = esos_uiF14_getRPGMediumThreshold();

            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_rpgm_CUR_VALUE);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)i16_rpg_med_val);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Perform correct action based on selection
            switch (u8_input) {
            case PLUS_KEY:
                i16_rpg_med_val = esos_uiF14_getRPGMediumThreshold();
                esos_uiF14_setRPGMediumThreshold(i16_rpg_med_val + 1);
                break;
            case MINUS_KEY:
                i16_rpg_med_val = esos_uiF14_getRPGMediumThreshold();
                esos_uiF14_setRPGMediumThreshold(i16_rpg_med_val - 1);
                break;
            case ESC_KEY:
                menu_state = RPG_THRESH_SETTINGS;
                break;
            default:
                menu_state = RPG_MED;
                break;
            }
        } else if (menu_state == RPG_FAST) {
            // Print current value
            static uint16_t i16_rpg_fast_val;
            i16_rpg_fast_val = esos_uiF14_getRPGFastThreshold();

            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_rpgf_CUR_VALUE);
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t)i16_rpg_fast_val);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Perform correct action based on selection
            switch (u8_input) {
            case PLUS_KEY:
                i16_rpg_fast_val = esos_uiF14_getRPGFastThreshold();
                esos_uiF14_setRPGFastThreshold(i16_rpg_fast_val + 1);
                break;
            case MINUS_KEY:
                i16_rpg_fast_val = esos_uiF14_getRPGFastThreshold();
                esos_uiF14_setRPGFastThreshold(i16_rpg_fast_val - 1);
                break;
            case ESC_KEY:
                menu_state = RPG_THRESH_SETTINGS;
                break;
            default:
                menu_state = RPG_FAST;
                break;
            }
        }

        // Print separator
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(str_m_SEPARATOR);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    }

    ESOS_TASK_END();
}

void config_ui_menu()
{
    esos_RegisterTask(menu);
}