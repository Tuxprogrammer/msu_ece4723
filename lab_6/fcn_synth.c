#include "revF14.h"
#include "fall17lib.h"
#include "esos_menu.h"
#include "esos_f14ui.h"
#include "esos_sensor.h"
#include "esos_pic24_sensor.h"

static BOOL b_updateLM60;

/*TODO: Ctrl+F replace all instances of mm with a name that abides
        by the coding standards.
*/
static esos_menu_longmenu_t main_menu = {
    .u8_numitems = 8,
    .u8_choice = 0, // Default
    .ast_items =
        {
            { "Set", "wvform", 0},
            { "Set", "freq", 0 },
            { "Set", "ampltd", 0 },
            { "Set", "duty", 1 },
            { "Read", "LM60", 0 },
            { "Read", "1631", 0 },
            { "Set", "LEDs", 0 },
            { "", "About...", 0 },
        },
};

// TODO: determine if this is the correct type of entry for the wvform
static esos_menu_longmenu_t wvform = {
    .u8_numitems = 4,
    .u8_choice = 0,
    .ast_items = { { "Select", "tri", 0 },
                   { "Select", "sine", 0 },
                   { "Select", "square", 0 },
                   { "Select", "user1", 0 } },
};

static esos_menu_entry_t freq = {
    .entries[0].label = "Freq = ",
    .entries[0].value = 1000, // Default, Freq = 1k
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

static esos_menu_sliderbar_t lm60 = {
    .value = 2000,
    .min = 2000,
    .max = 3500,
    .div = 100,
    .type = 1,
    .lines = { { "LM60" }, { "" } },
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

ESOS_USER_TASK(lcd_menu)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        // Display main menu until the user presses SW3 to choose a selection

        // // couldn't get the menu switching to work with pointers for cleaner code
        // // TODO: determine if possible and refactor if so
        // esos_menu_longmenu_t *menu = &main_menu;
        // if (wvform.u8_choice == 2) {
        //     menu = &main_menu_with_duty;
        // }

        // if square wave selected, show duty cycle
        if (wvform.u8_choice == 2) {
            main_menu.ast_items[3].b_hidden = FALSE;
        }

        ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(main_menu);
        if (main_menu.u8_choice == 0) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(wvform);
        } else if (main_menu.u8_choice == 1) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(freq);
        } else if (main_menu.u8_choice == 2) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(ampl);
        } else if (main_menu.u8_choice == 3) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(duty);
        } else if (main_menu.u8_choice == 4) {
            b_updateLM60 = 1;
            ESOS_TASK_WAIT_ESOS_MENU_SLIDERBAR(lm60);
            b_updateLM60 = 0;
        } else if (main_menu.u8_choice == 5) {
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(_1631);
        } else if (main_menu.u8_choice == 6) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(leds);
        } else if (main_menu.u8_choice == 7) {
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(about);
        }
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

ESOS_USER_TASK(update_lm60)
{
    static ESOS_TASK_HANDLE read_temp;
    static uint16_t pu16_out;
    static uint32_t pu32_out;

    ESOS_TASK_BEGIN();
    ESOS_ALLOCATE_CHILD_TASK(read_temp);
    while (TRUE) {
        if (b_updateLM60) {
            ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_ON_AVAILABLE_SENSOR, TEMP_CHANNEL, ESOS_SENSOR_VREF_3V0);
            ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_SENSOR_READ, &pu16_out, ESOS_SENSOR_ONE_SHOT,
                                     ESOS_SENSOR_FORMAT_VOLTAGE);
            ESOS_SENSOR_CLOSE();

            pu32_out = (uint32_t)pu16_out * 1000; // convert to not use decimals
            pu32_out = (pu32_out - 424000) / 625; // millimillivolts to temp
            lm60.value = pu32_out;

            // We could update the menu with text on line 2 here
        }
        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

void user_init()
{
    // This is all called in esos_menu_init
    // config_esos_uiF14();
    // esos_lcd44780_configDisplay();
    // esos_lcd44780_init();
    esos_menu_init();
    esos_RegisterTask(lcd_menu);
    esos_RegisterTask(set_led);
    esos_RegisterTask(update_lm60);
}
