
#include "stdlib.h"
#include "string.h"
#include "esos_menu.h"
#include "fall17lib.h"
#include "esos_lcd44780.h"
#include "esos_f14ui.h"
#include "esos_lcd44780_customChars.h"

__esos_menu_conf_t __esos_menu_conf;

// 00011110
uint8_t au8_upArrow[] = {
    0b00000, 0b00000, 0b00000, 0b00100, 0b01110, 0b11111, 0b00000, 0b00000, 0b00000,
};

// 00011111
uint8_t au8_dnArrow[] = {
    0b00000, 0b00000, 0b00000, 0b11111, 0b01110, 0b00100, 0b00000, 0b00000, 0b00000,
};

ESOS_USER_TASK(esos_menu_task)
{
    ESOS_TASK_BEGIN();

    while (TRUE) {
        while (__esos_menu_conf.e_menutype == NONE) {
            ESOS_TASK_YIELD();
        }

        while (__esos_menu_conf.e_menutype == LONGMENU) {
            static BOOL b_firstMenu;
            static BOOL b_lastMenu;
            static esos_menu_longmenu_t *pst_menu;
            static uint8_t u8_current;

            // Draw the menu, then wait for a button
            pst_menu = __esos_menu_conf.pv_data;

            // Clamp the starting choice to possible values.
            pst_menu->u8_choice = MIN(pst_menu->u8_choice, pst_menu->u8_numitems - 1);

            // Draw the correct menu.
            esos_lcd44780_clearScreen();

            esos_lcd44780_setCustomChar(0, au8_upArrow);
            esos_lcd44780_setCustomChar(1, au8_dnArrow);
            esos_lcd44780_writeString(0, 0, pst_menu->ast_items[pst_menu->u8_choice].ac_line1);
            esos_lcd44780_writeString(1, 0, pst_menu->ast_items[pst_menu->u8_choice].ac_line2);

            b_firstMenu = (pst_menu->u8_choice == 0);
            b_lastMenu = (pst_menu->u8_choice >= pst_menu->u8_numitems - 1);

            // Draw the arrows.
            if (!b_firstMenu)
                esos_lcd44780_writeChar(0, 7, '\x00');
            if (!b_lastMenu)
                esos_lcd44780_writeChar(1, 7, '\x01');

            // Wait for the user to spin the wheel, or press the button.
            while (TRUE) {
                if (esos_uiF14_isSW3Pressed()) {
                    ESOS_TASK_WAIT_UNTIL(!esos_uiF14_isSW3Pressed());
                    // The user has chosen.  Bail out.
                    __esos_menu_conf.e_menutype = NONE;
                    break;
                } else if (esos_uiF14_isRPGTurning() && esos_uiF14_isRPGTurningCW() &&
                           esos_uiF14_getRPGVelocity_i16() > 750 && !b_lastMenu) {
                    u8_current = pst_menu->u8_choice;
                    // Attempt to increase the current value.
                    ++pst_menu->u8_choice;
                    // if its hidden don't show the menu
                    while (pst_menu->ast_items[pst_menu->u8_choice].b_hidden == 1) {
                        if (pst_menu->u8_choice == pst_menu->u8_numitems - 1) {
                            pst_menu->u8_choice = u8_current; // the rest of the menus are all hidden
                        } else {
                            ++pst_menu->u8_choice;
                        }
                    }
                    ESOS_TASK_WAIT_UNTIL(esos_uiF14_getRPGVelocity_i16() < 750);
                    break;
                } else if (esos_uiF14_isRPGTurning() && esos_uiF14_isRPGTurningCCW() &&
                           esos_uiF14_getRPGVelocity_i16() < -750 && !b_firstMenu) {
                    u8_current = pst_menu->u8_choice;
                    --pst_menu->u8_choice;
                    // if its hidden don't show the menu
                    while (pst_menu->ast_items[pst_menu->u8_choice].b_hidden == 1) {
                        if (pst_menu->u8_choice == 0) {
                            pst_menu->u8_choice = u8_current; // the rest of the menus are all hidden
                        } else {
                            --pst_menu->u8_choice;
                        }
                    }
                    ESOS_TASK_WAIT_UNTIL(esos_uiF14_getRPGVelocity_i16() > -750);
                    break;
                }

                ESOS_TASK_YIELD();
            }
        }

        while (__esos_menu_conf.e_menutype == STATICMENU) {
            static BOOL b_firstLine;
            static BOOL b_lastLine;
            static esos_menu_staticmenu_t *pst_menu;

            // Draw the menu, then wait for a button
            pst_menu = __esos_menu_conf.pv_data;

            // Clamp the currentline.
            pst_menu->u8_currentline = MIN(pst_menu->u8_currentline, pst_menu->u8_numlines - 1);

            b_firstLine = (pst_menu->u8_currentline == 0);
            b_lastLine = (pst_menu->u8_currentline + 1 >= pst_menu->u8_numlines);

            // Draw the lines.  Possibly there is only one line, so handle that.
            esos_lcd44780_clearScreen();
            esos_lcd44780_writeString(0, 0, pst_menu->lines[pst_menu->u8_currentline]);
            if (!b_lastLine)
                esos_lcd44780_writeString(1, 0, pst_menu->lines[pst_menu->u8_currentline + 1]);

            // Wait for the user to spin the wheel, or press a button.
            while (TRUE) {
                if (esos_uiF14_isSW3Pressed() || esos_uiF14_isSW1Pressed() || esos_uiF14_isSW2Pressed()) {
                    // The user is done.  Bail out.
                    ESOS_TASK_WAIT_UNTIL(
                        !(esos_uiF14_isSW3Pressed() || esos_uiF14_isSW1Pressed() || esos_uiF14_isSW2Pressed()));
                    __esos_menu_conf.e_menutype = NONE;
                    break;
                } else if (esos_uiF14_isRPGTurning() && esos_uiF14_isRPGTurningCW() && !b_lastLine) {
                    ++pst_menu->u8_currentline;
                    ESOS_TASK_WAIT_UNTIL(!esos_uiF14_isRPGTurning());
                    break;
                } else if (esos_uiF14_isRPGTurning() && esos_uiF14_isRPGTurningCCW() && !b_firstLine) {
                    --pst_menu->u8_currentline;
                    ESOS_TASK_WAIT_UNTIL(!esos_uiF14_isRPGTurning());
                    break;
                }
                ESOS_TASK_YIELD();
            }
            ESOS_TASK_YIELD();
        }

        while (__esos_menu_conf.e_menutype == ENTRY) {
            static esos_menu_entry_t *pst_menu;
            static uint8_t au8_intbuffer[7];
            static uint8_t u8_idlen1, u8_idlen2;
            static esos_menu_entry_item_t *pst_entry;

            pst_menu = __esos_menu_conf.pv_data;

            // Draw the display.
            esos_lcd44780_clearScreen();

            u8_idlen1 = strlen(pst_menu->entries[0].label);
            u8_idlen2 = strlen(pst_menu->entries[1].label);

            // TODO. We only handle one value right now.
            if (u8_idlen2 > 0 && FALSE) {
                /* esos_lcd_writeString(0, 0, pst_menu->label1); */
                /* itoa(pst_menu->value1, (char*)au8_intbuffer, 10); */
                /* esos_lcd_writeString(0, u8_idlen1, (char*)au8_intbuffer); */

                /* esos_lcd_writeString(1, 0, pst_menu->label2); */
                /* itoa(pst_menu->value1, (char*)au8_intbuffer, 10); */
                /* esos_lcd_writeString(1, u8_idlen2, (char*)au8_intbuffer); */
            } else {
                pst_entry = &pst_menu->entries[0];

                if (pst_entry->min != 0 || pst_entry->max != 0) {
                    // Clamp the value.
                    pst_entry->value = MIN(pst_entry->value, pst_entry->max);
                    pst_entry->value = MAX(pst_entry->value, pst_entry->min);
                }

                esos_lcd44780_writeString(0, 0, pst_entry->label);
                itoa(pst_entry->value, (char *)au8_intbuffer, 10);
                esos_lcd44780_writeString(1, 0, (char *)au8_intbuffer);

                esos_uiF14_resetRPG();

                while (TRUE) {
                    if (esos_uiF14_isSW3Pressed()) {
                        // We're done; the user has chosen.  Bail out.
                        ESOS_TASK_WAIT_UNTIL(!esos_uiF14_isSW3Pressed());
                        __esos_menu_conf.e_menutype = NONE;
                        break;
                    } else if (esos_uiF14_getRPGValue_i16() <= -1) {
                        // Will: Commented out because our code does this already?
                        // _esos_uiF14_setRPGCounter(esos_uiF14_getRPGValue_i16() + 1);
                        if (esos_uiF14_isRPGTurningFast())
                            pst_entry->value -= 100;
                        else if (esos_uiF14_isRPGTurningMedium())
                            pst_entry->value -= 20;
                        else
                            pst_entry->value -= 1;
                        break;
                    } else if (esos_uiF14_getRPGValue_i16() >= 1) {
                        // Will: Commented out because our code does this already?
                        // _esos_uiF14_setRPGCounter(esos_uiF14_getRPGValue_i16() - 1);
                        if (esos_uiF14_isRPGTurningFast())
                            pst_entry->value += 100;
                        else if (esos_uiF14_isRPGTurningMedium())
                            pst_entry->value += 20;
                        else
                            pst_entry->value += 1;
                        break;
                    }
                    ESOS_TASK_YIELD();
                }
            }
        }

        while (__esos_menu_conf.e_menutype == SLIDERBAR) {
            static BOOL b_firstLine;
            static BOOL b_lastLine;
            static esos_menu_sliderbar_t *pst_menu;
            static uint32_t acc;
            static uint8_t i;

            // Draw the menu, then wait for a button
            pst_menu = __esos_menu_conf.pv_data;

            esos_lcd44780_clearScreen();

            // load custom characters
            if (pst_menu->type == 0) {
                esos_lcd44780_init_custom_chars_slider();
            } else {
                esos_lcd44780_init_custom_chars_bar();
            }

            ESOS_TASK_WAIT_TICKS(1); // give the characters time to save

            // Wait for the user to press a button.
            while (TRUE) {
                esos_lcd44780_writeString(0, 0, pst_menu->lines[0]);
                esos_lcd44780_writeString(1, 0, pst_menu->lines[1]);

                ESOS_TASK_WAIT_TICKS(1); // give the characters time to save
                // Draw the slider or bargraph.
                if (pst_menu->type == 0) {
                    // slider
                    static uint8_t au8_slider[8];
                    for (i = 0; i < 8; i++) {
                        au8_slider[i] = SLIDER_LINE;
                    }

                    // increment i until we reach the correct value
                    i = 0;
                    acc = pst_menu->min; // pst_menu->min / pst_menu->div;
                    while (acc < pst_menu->value && i < 0x07) {
                        i++;
                        acc += pst_menu->div;
                    };

                    au8_slider[i] = ((pst_menu->value & 0x1FF) / 0x067) + 1;

                    esos_lcd44780_writeBuffer(1, 0, au8_slider, 8);
                } else if (pst_menu->type == 1) {
                    // bar
                    uint8_t u8_barTop = ' ';
                    uint8_t u8_barBottom = ' ';

                    // increment i until we reach the correct value
                    i = 0;
                    acc = pst_menu->min; // pst_menu->min / pst_menu->div;
                    while (acc < pst_menu->value && i < 0x10) {
                        i++;
                        acc += pst_menu->div;
                    };

                    // ESOS_TASK_WAIT_ON_SEND_STRING("DRAW BAR ");
                    // ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(pst_menu->value);
                    // ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
                    // ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(i);
                    // ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
                    // ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(acc);
                    // ESOS_TASK_WAIT_ON_SEND_UINT8('\n');

                    switch (i) {
                    case 0x00:
                    default:
                        break;
                    case 0x01:
                        u8_barBottom = _1EIGTH_BAR;
                    case 0x02:
                        u8_barBottom = _2EIGTH_BAR;
                        break;
                    case 0x03:
                        u8_barBottom = _3EIGTH_BAR;
                        break;
                    case 0x04:
                        u8_barBottom = _4EIGTH_BAR;
                        break;
                    case 0x05:
                        u8_barBottom = _5EIGTH_BAR;
                        break;
                    case 0x06:
                        u8_barBottom = _6EIGTH_BAR;
                        break;
                    case 0x07:
                        u8_barBottom = _7EIGTH_BAR;
                        break;
                    case 0x08:
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    case 0x09:
                        u8_barTop = _1EIGTH_BAR;
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    case 0x0A:
                        u8_barTop = _2EIGTH_BAR;
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    case 0x0B:
                        u8_barTop = _3EIGTH_BAR;
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    case 0x0C:
                        u8_barTop = _4EIGTH_BAR;
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    case 0x0D:
                        u8_barTop = _5EIGTH_BAR;
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    case 0x0E:
                        u8_barTop = _6EIGTH_BAR;
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    case 0x0F:
                        u8_barTop = _7EIGTH_BAR;
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    case 0x10:
                        u8_barTop = _8EIGTH_BAR;
                        u8_barBottom = _8EIGTH_BAR;
                        break;
                    }

                    // ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_barTop);
                    // ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
                    // ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_barBottom);
                    // ESOS_TASK_WAIT_ON_SEND_UINT8('\n');

                    //ESOS_TASK_WAIT_TICKS(1);
                    esos_lcd44780_writeChar(0, 7, u8_barTop);
                    esos_lcd44780_writeChar(1, 7, u8_barBottom);
                }

                if (esos_uiF14_isSW3Pressed() || esos_uiF14_isSW1Pressed() || esos_uiF14_isSW2Pressed()) {
                    // The user is done.  Bail out.
                    ESOS_TASK_WAIT_UNTIL(
                        !(esos_uiF14_isSW3Pressed() || esos_uiF14_isSW1Pressed() || esos_uiF14_isSW2Pressed()));
                    __esos_menu_conf.e_menutype = NONE;
                    break;
                }
                ESOS_TASK_WAIT_TICKS(100);
            }
            ESOS_TASK_YIELD();
        }

        // Clean up the display after finishing a menu.
        esos_lcd44780_clearScreen();
    }

    ESOS_TASK_END();
}

void esos_menu_init(void)
{
    config_esos_uiF14();
    esos_lcd44780_configDisplay();
    esos_lcd44780_init();

    esos_RegisterTask(esos_menu_task);
}
