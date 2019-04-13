#ifndef ESOS_MENU_H
#define ESOS_MENU_H

#include "esos.h"

// A single item for a longmenu, see below.
// Contains lines 1 and 2 of text.
typedef struct {
    char ac_line1[8];
    char ac_line2[8];
    BOOL b_hidden;
} esos_menu_longmenu_item_t;

/*
esos_menu_longmenu_t Usage:
    u8_numitems: The number of items contained in the array ast_items[]
    u8_choice:	 Our currently selected item (index) contained in ast_items[]
    ast_items[]: esos_menu_longmenu_item_t structure containing our 2 lines
                 to display for each menu item in ast_items[]
*/
typedef struct {
    uint8_t u8_numitems;
    uint8_t u8_choice;
    esos_menu_longmenu_item_t ast_items[];
} esos_menu_longmenu_t;

/*
esos_menu_staticmenu_t Usage:
    u8_numlines: 	The number of lines contained in 2D array lines[][8]
    u8_currentline: The currently selected line in 2D array lines[][8]
    lines[][8]:		Contains an array of lines to be displayed to the LCD
*/
typedef struct {
    uint8_t u8_numlines;
    uint8_t u8_currentline;
    char lines[][8];
} esos_menu_staticmenu_t;
/*
esos_menu_entry_item_t Usage:
    u8_value: 		The actual value being edited by the user.
    u8_min: 		The minimum value the user is allowed to enter.
    u8_max:			The maximum value the user is allowed to enter.
*/
typedef struct {
    char label[8];
    int16_t value;
    int16_t min;
    int16_t max;
} esos_menu_entry_item_t;

/*
esos_menu_entry_t Usage:
    entries[]: Contains the two visible entries on your screen.
               ex: x = 147
                   y = 256
*/
typedef struct {
    esos_menu_entry_item_t entries[2];
} esos_menu_entry_t;

/*
esos_menu_sliderbar_t Usage:
    value:          Contains the actual value to display with min and max vals
    div:            Contants the size of divisions, used to calculate the bar
    type:           0=Slider 1=Bargraph
    lines:          Optional Text to Display (Note: may get overwritten)
*/
typedef struct {
    uint32_t value;
    uint16_t min;
    uint16_t max;
    uint8_t div;
    uint8_t type;
    char lines[][8];
} esos_menu_sliderbar_t;

// Call this to initialize the cherrymenu subsystem.
void esos_menu_init(void);

// Call these to run and wait for a menu of your creation, passing the
// appropriate structure in.
#define ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(structure)                                                                   \
    do {                                                                                                               \
        esos_menu_longmenu_t *ps_menu = &structure;                                                                    \
        __esos_menu_conf.e_menutype = LONGMENU;                                                                        \
        __esos_menu_conf.pv_data = ps_menu;                                                                            \
        ESOS_TASK_WAIT_UNTIL(__esos_menu_conf.e_menutype == NONE);                                                     \
    } while (0)

#define ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(structure)                                                                 \
    do {                                                                                                               \
        esos_menu_staticmenu_t *ps_menu = &structure;                                                                  \
        __esos_menu_conf.e_menutype = STATICMENU;                                                                      \
        __esos_menu_conf.pv_data = ps_menu;                                                                            \
        ESOS_TASK_WAIT_UNTIL(__esos_menu_conf.e_menutype == NONE);                                                     \
    } while (0)

#define ESOS_TASK_WAIT_ESOS_MENU_ENTRY(structure)                                                                      \
    do {                                                                                                               \
        esos_menu_entry_t *ps_menu = &structure;                                                                       \
        __esos_menu_conf.e_menutype = ENTRY;                                                                           \
        __esos_menu_conf.pv_data = ps_menu;                                                                            \
        ESOS_TASK_WAIT_UNTIL(__esos_menu_conf.e_menutype == NONE);                                                     \
    } while (0)

#define ESOS_TASK_WAIT_ESOS_MENU_SLIDERBAR(structure)                                                                  \
    do {                                                                                                               \
        esos_menu_sliderbar_t *ps_menu = &structure;                                                                   \
        __esos_menu_conf.e_menutype = SLIDERBAR;                                                                       \
        __esos_menu_conf.pv_data = ps_menu;                                                                            \
        ESOS_TASK_WAIT_UNTIL(__esos_menu_conf.e_menutype == NONE);                                                     \
    } while (0)

// Prototypes and definitions below this line are not part of the stable
// API and should not be used by clients.  Here be dragons.

/*Notes: Since *pv_data is void, the value pv_data can point to any
  of our menu types.
*/
typedef struct {
    enum {
        NONE,
        LONGMENU,
        STATICMENU,
        ENTRY,
        SLIDERBAR,
    } e_menutype;
    void *pv_data;
} __esos_menu_conf_t;

extern __esos_menu_conf_t __esos_menu_conf;

#endif
