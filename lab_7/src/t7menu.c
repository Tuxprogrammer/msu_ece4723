#include "t7menu.h"
#include "t7sensors.h"

volatile esos_menu_longmenu_t main_menu = {
    .u8_numitems = 9,
    .u8_choice = 0, // Default
    .ast_items =
        {
            { "Set", "wvform", 0 },
            { "Set", "freq", 0 },
            { "Set", "ampltd", 0 },
            { "Set", "duty", 1 },
            { "Read", "LM60", 0 },
            { "Read", "1631", 0 },
            { "Set", "LEDs", 0 },
            { "", "About...", 0 },
            { "Browse", "Network", 0 },
        },
};

// volatile esos_menu_longmenu_item_t network_id_menu_list[NUM_OF_IDS];
volatile esos_menu_longmenu_t network_menu = {
    .u8_numitems = NUM_OF_IDS,
    .u8_choice = MY_ID,
    .ast_items =
        {
            { "cbb330", "1", 1 },  { "sc2257", "1", 1 }, { "lec426", "1", 1 }, { "woc17", "1", 1 },
            { "jdf469", "1", 1 },  { "jtn136", "2", 1 }, { "nrs171", "2", 1 }, { "igh9", "2", 1 },
            { "law448", "2", 1 },  { "rkh134", "2", 1 }, { "gs656", "3", 1 },  { "lrh282", "3", 1 },
            { "reo74", "3", 1 },   { "bmf151", "3", 1 }, { "rfj18", "3", 1 },  { "dc2274", "4", 1 },
            { "mam1218", "4", 1 }, { "mf1413", "4", 1 }, { "bcw253", "4", 1 }, { "jmp784", "4", 1 },
            { "bcj162", "7", 1 },  { "Vuk", "7", 1 },
        },
};

volatile esos_menu_longmenu_t wvform = {
    .u8_numitems = 4,
    .u8_choice = 0,
    .ast_items = { { "Select", "tri", 0 },
                   { "Select", "square", 0 },
                   { "Select", "sine", 0 },
                   { "Select", "user1", 0 } },
};

volatile esos_menu_entry_t freq = {
    .entries[0].label = "Freq = ",
    .entries[0].value = 1000, // Default, Freq = 1k
    .entries[0].min = 64,
    .entries[0].max = 2047,
};

// note ampl is 10 times the value to avoid floats
volatile esos_menu_entry_t ampl = {
    .entries[0].label = "Ampl = ",
    .entries[0].value = 10, // Default, ampl = 1.0V
    .entries[0].min = 0,
    .entries[0].max = 30,
};

volatile esos_menu_entry_t duty = {
    .entries[0].label = "Duty = ",
    .entries[0].value = 50, // Default, duty cycle = 50%
    .entries[0].min = 0,
    .entries[0].max = 100,
};

volatile esos_menu_entry_t leds = {
    .entries[0].label = "LEDs = ",
    .entries[0].value = 3,
    .entries[0].min = 0,
    .entries[0].max = 7,
};

volatile esos_menu_sliderbar_t lm60 = {
    .value = 2500,
    .min = 2000,
    .max = 3500,
    .div = 100,
    .type = 1,
    .lines = { { "LM60" }, { "       " } },
};

volatile esos_menu_sliderbar_t _1631 = {
    .value = 25,
    .min = 18,
    .max = 34,
    .div = 1,
    .type = 1,
    .lines = { { "DS1631" }, { "       " } },
};

// TODO: figure out what goes in about
volatile esos_menu_staticmenu_t about = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "team" }, { "gg" } },
};

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