#include "revF14.h"
#include "fall17lib.h"
#include "esos_menu.h"

/*TODO: Ctrl+F replace all instances of mm with a name that abides
        by the coding standards.
*/
static esos_menu_longmenu_t mm = {
    .u8_numitems = 8,
    .u8_choice = 0, // Default
    .ast_items =
        {
            { "Set", "wvform" },
            { "Set", "freq" },
            { "Set", "ampltd" },
            { "Set", "duty" },
            { "Read", "LM60" },
            { "Read", "1631" },
            { "Set", "LEDs" },
            { "", "About..." },
        },
};

static esos_menu_staticmenu_t err = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "404" }, { "NotFnd" } },
};

// TODO: determine if this is the correct type of entry for the wvform
static esos_menu_longmenu_t wvform = {
    .u8_numitems = 4,
    .u8_choice = 0,
    .ast_items = { { "Select", "tri" }, { "Select", "sine" }, { "Select", "square" }, { "Select", "user1" } },
};

static esos_menu_entry_t freq = {
    .entries[0].label = "Freq = ",
    .entries[0].value = 1000, // Default, freq = 1s
    .entries[0].min = 64,
    .entries[0].max = 2047,
};

// note ampl is 10 times the value to avoid floats
static esos_menu_entry_t ampl = {
    .entries[0].label = "Ampl = ",
    .entries[0].value = 10, // Default, ampl = 1.0V
    .entries[0].min = 0,
    .entries[0].max = 33,
};

static esos_menu_entry_t duty = {
    .entries[0].label = "Duty = ",
    .entries[0].value = 50, // Default, duty cycle = 50%
    .entries[0].min = 0,
    .entries[0].max = 100,
};

static esos_menu_entry_t leds = {
    .entries[0].label = "LEDs = ",
    .entries[0].value = 3,
    .entries[0].min = 0,
    .entries[0].max = 7,
};

static esos_menu_staticmenu_t lm60 = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "Hello" }, { "LM60" } },
};

static esos_menu_staticmenu_t _1631 = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "Hello" }, { "1631" } },
};

// TODO: figure out what goes in about
static esos_menu_staticmenu_t about = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "its abt" }, { "time" } },
};

ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(lcd_example)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        // Display main menu until the user presses SW3 to choose a selection
        ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(mm);
        if (mm.u8_choice == 0)
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(wvform);
        else if (mm.u8_choice == 1)
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(freq);
        else if (mm.u8_choice == 2)
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(ampl);
        else if (mm.u8_choice == 3)
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(duty);
        else if (mm.u8_choice == 4)
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(lm60);
        else if (mm.u8_choice == 5)
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(_1631);
        else if (mm.u8_choice == 6)
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(leds);
        else if (mm.u8_choice == 7)
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(about);
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(set_led)
{
    ESOS_TASK_BEGIN();
    LED1_OFF();
    LED2_OFF();
    LED3_HB_OFF();
    while (TRUE) {
        // Note that LED3_HB is negated because the pin goes high when LED3 is off
        LED1 = (leds.entries[0].value & 0b100) == 0b100;
        LED2 = (leds.entries[0].value & 0b010) == 0b010;
        LED3_HB = (leds.entries[0].value & 0b001) != 0b001;
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

void user_init()
{
    // config_esos_uiF14();
    esos_menu_init();
    esos_RegisterTask(lcd_example);
    esos_RegisterTask(set_led);
    // esos_RegisterTimer(heartbeat_LED, 500);
}
