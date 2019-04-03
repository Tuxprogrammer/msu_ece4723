#include "t7menu.h"
#include "t7sensors.h"

ESOS_USER_TASK(lcd_menu)
{
    static ESOS_TASK_HANDLE update_hdl;
    ESOS_TASK_BEGIN();
    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice, duty.entries[0].value, ampl.entries[0].value);

    while (TRUE) {
        // Display main menu until the user presses SW3 to choose a selection

        // if square wave selected, show duty cycle
        if (wvform.u8_choice == 1) {
            main_menu.ast_items[3].b_hidden = FALSE;
        } else {
            main_menu.ast_items[3].b_hidden = TRUE;
        }

        ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(main_menu);
        if (main_menu.u8_choice == 0) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(wvform);
            ESOS_ALLOCATE_CHILD_TASK(update_hdl);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice, duty.entries[0].value,
                                     ampl.entries[0].value);

        } else if (main_menu.u8_choice == 1) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(freq);
            PR4 = FCY / 8 / 128 / freq.entries[0].value;
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(PR4);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
        } else if (main_menu.u8_choice == 2) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(ampl);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice, duty.entries[0].value,
                                     ampl.entries[0].value);
        } else if (main_menu.u8_choice == 3) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(duty);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice, duty.entries[0].value,
                                     ampl.entries[0].value);
        } else if (main_menu.u8_choice == 4) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(network_menu);
            if (network_menu.u8_choice == MY_ID)
                b_updateLM60 = 1;
            else
                b_requestLM60 = 1;
            // TODO: propagate updates to lm60 menu item...
            ESOS_TASK_WAIT_ESOS_MENU_SLIDERBAR(lm60);
            b_updateLM60 = 0;
            b_requestLM60 = 0;
        } else if (main_menu.u8_choice == 5) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(network_menu);
            if (network_menu.u8_choice == MY_ID)
                b_updateDS1631 = 1;
            else
                b_requestDS1631 = 1;
            ESOS_TASK_WAIT_ESOS_MENU_SLIDERBAR(_1631);
            b_updateDS1631 = 0;
            b_requestDS1631 = 0;
        } else if (main_menu.u8_choice == 6) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(leds);
        } else if (main_menu.u8_choice == 7) {
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(about);
        } else if (main_menu.u8_choice == 8) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(network_menu);
        }
    }
    ESOS_TASK_END();
}

void initialize_network_menu_list()
{
    uint8_t u8_i, u8_j;
    for (u8_i = 0; u8_i < NUM_OF_IDS; u8_i++) {
        for (u8_j = 0; u8_j < 8; u8_j++) {
            network_menu.ast_items[u8_i].ac_line1[u8_j] = aCANID_IDs[u8_i].psz_netID[u8_j];
            if (aCANID_IDs[u8_i].psz_netID[u8_j] == '\0')
                break;
        }
        // utoa(aCANID""_IDs[u8_i].u8_teamID, network_menu.ast_items[u8_i].ac_line2, 10);
        network_menu.ast_items[u8_i].ac_line2[0] = 'h';
        network_menu.ast_items[u8_i].ac_line2[1] = 'e';
        network_menu.ast_items[u8_i].ac_line2[2] = 'l';
        network_menu.ast_items[u8_i].ac_line2[3] = 'p';
        network_menu.ast_items[u8_i].ac_line2[4] = 'p';
        network_menu.ast_items[u8_i].ac_line2[5] = 'l';
        network_menu.ast_items[u8_i].ac_line2[6] = 's';
        network_menu.ast_items[u8_i].ac_line2[7] = '\0';
        network_menu.ast_items[u8_i].b_hidden = u8_i != MY_ID;
        // network_menu.ast_items[u8_i] = network_id_menu_list[u8_i];
    }
}