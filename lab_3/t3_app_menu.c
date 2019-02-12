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
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_LED_FLASH_SETTINGS);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_RPG_THRESH_SETTINGS);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_INSTR);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Print selection
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_UINT8(u8_input);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Set next state based on selection
            switch (u8_input) {
            case '1':
                menu_state = DBL_PRESS_SETTINGS;
                break;
            case '2':
                menu_state = LED_FLASH_SETTINGS;
                break;
            case '3':
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
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_INSTR);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Print selection
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_UINT8(u8_input);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

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
        } else if (menu_state == LED_FLASH_SETTINGS) {
            // Print menu options
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mled_LED1_PERIOD);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mled_LED2_PERIOD);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mled_LED3_PERIOD);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_INSTR);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Print selection
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_UINT8(u8_input);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Set next state based on selection
            switch (u8_input) {
            case '1':
                // menu_state = DBL_PRESS_SETTINGS;
                break;
            case '2':
                // menu_state = LED_FLASH_SETTINGS;
                break;
            case '3':
                // menu_state = RPG_THRESH_SETTINGS;
                break;
            case ESC_KEY:
                menu_state = TOP_MENU;
                break;
            default:
                menu_state = LED_FLASH_SETTINGS;
                break;
            }
        } else if (menu_state == RPG_THRESH_SETTINGS) {
            // Print menu options
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mrpg_RPG_SLOW);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mrpg_RPG_MED);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_mrpg_RPG_FAST);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_BACK);
            ESOS_TASK_WAIT_ON_SEND_STRING(str_m_INSTR);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Wait for selection
            ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
            ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
            ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

            // Print selection
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_UINT8(u8_input);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            // Set next state based on selection
            switch (u8_input) {
            case '1':
                // menu_state = DBL_PRESS_SETTINGS;
                break;
            case '2':
                // menu_state = LED_FLASH_SETTINGS;
                break;
            case '3':
                // menu_state = RPG_THRESH_SETTINGS;
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
            ESOS_TASK_WAIT_ON_SEND_STRING(str_sw1p_NEW_VALUE);
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
        }

        // Print separator
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING(str_m_SEPARATOR);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    }

    ESOS_TASK_END();
}

void config_ui_menu() {
    esos_RegisterTask(menu);
}