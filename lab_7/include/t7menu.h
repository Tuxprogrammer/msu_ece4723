#ifndef __T7MENU_H
#define __T7MENU_H
#include "t7synth.h"

static esos_menu_longmenu_t main_menu = {
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

// static esos_menu_longmenu_item_t network_id_menu_list[NUM_OF_IDS];
static esos_menu_longmenu_t network_menu = {
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

static esos_menu_longmenu_t wvform = {
    .u8_numitems = 4,
    .u8_choice = 0,
    .ast_items = { { "Select", "tri", 0 },
                   { "Select", "square", 0 },
                   { "Select", "sine", 0 },
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
    .entries[0].max = 30,
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
    .value = 2500,
    .min = 2000,
    .max = 3500,
    .div = 100,
    .type = 1,
    .lines = { { "LM60" }, { "       " } },
};

static esos_menu_sliderbar_t _1631 = {
    .value = 25,
    .min = 18,
    .max = 34,
    .div = 1,
    .type = 1,
    .lines = { { "DS1631" }, { "       " } },
};

// TODO: figure out what goes in about
static esos_menu_staticmenu_t about = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "team" }, { "gg" } },
};

ESOS_USER_TASK(lcd_menu);

void initialize_network_menu_list();
#endif