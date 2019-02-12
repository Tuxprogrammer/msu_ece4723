#ifndef T3_APP_MENU
#define T3_APP_MENU

#include "esos.h"

// Menu States
enum MENU_STATE {
    TOP_MENU,
    DBL_PRESS_SETTINGS,
    RPG_THRESH_SETTINGS,
    SW1_PERIOD,
    SW2_PERIOD,
    SW3_PERIOD,
    RPG_SLOW,
    RPG_MED,
    RPG_FAST
};

// Key Bindings
#define ESC_KEY 0x1B
#define PLUS_KEY 0x2B
#define MINUS_KEY 0x2D

ESOS_USER_TASK(menu);

#endif