#include "revF14.h"
#include "fall17lib.h"
#include "esos_menu.h"

/*TODO: Ctrl+F replace all instances of mm with a name that abides
        by the coding standards.
*/
static esos_menu_longmenu_t mm = {
    .u8_numitems = 10,
    .u8_choice = 0, // Default
    .ast_items =
        {
            { "Set", "User" },
            { "Set", "Port" },
            { "Set", "Dest" },
            { "Set", "Temp" },
            { "Set", "T-stat" },
            { "Self", "Test" },
            { "Set", "LEDFP" },
            { "Static", "Greet" },
            { "Greet", "UART" },
            { "", "Back..." },
        },
};

static esos_menu_staticmenu_t err = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "404" }, { "NotFnd" } },
};

static esos_menu_entry_t ledfp = {
    .entries[0].label = "Per = ",
    .entries[0].value = 1000, // Default, per = 1s
    .entries[0].min = 250,
    .entries[0].max = 2000,
};

static esos_menu_staticmenu_t greet = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "Hello" }, { "friend" } },
};

ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(lcd_example)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        // Display our main menu until the user presses SW3 to choose a selection
        ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(mm);
        if (mm.u8_choice < 6)
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(err);
        else if (mm.u8_choice == 6)
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(ledfp);
        else if (mm.u8_choice == 7)
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(greet);
        else if (mm.u8_choice == 8) {
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("Don't procrastinate.\n");
            ESOS_TASK_WAIT_ON_SEND_STRING("It will not end well for embedded systems students.\n");
            ESOS_TASK_WAIT_ON_SEND_STRING("Now ... Go make waves\n");
            ESOS_TASK_WAIT_ON_SEND_STRING("Ba dun tsst\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        } else if (mm.u8_choice == 9) {
        }
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(flash_led)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        ESOS_TASK_WAIT_TICKS(ledfp.entries[0].value);
        LED1 = !LED1;
    }
    ESOS_TASK_END();
}

void user_init()
{
    // config_esos_uiF14();
    esos_menu_init();
    esos_RegisterTask(lcd_example);
    esos_RegisterTask(flash_led);
    esos_RegisterTimer(heartbeat_LED, 500);
}