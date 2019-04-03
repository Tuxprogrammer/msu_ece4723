#ifndef __T7MENU_H
#define __T7MENU_H
#include "t7synth.h"

extern volatile esos_menu_longmenu_t main_menu;

// extern volatile esos_menu_longmenu_item_t network_id_menu_list[NUM_OF_IDS];
extern volatile esos_menu_longmenu_t network_menu;

extern volatile esos_menu_longmenu_t wvform;

extern volatile esos_menu_entry_t freq;

// note ampl is 10 times the value to avoid floats
extern volatile esos_menu_entry_t ampl;

extern volatile esos_menu_entry_t duty;

extern volatile esos_menu_entry_t leds;

extern volatile esos_menu_sliderbar_t lm60;

extern volatile esos_menu_sliderbar_t _1631;

// TODO: figure out what goes in about
extern volatile esos_menu_staticmenu_t about;

ESOS_USER_TASK(lcd_menu);

void initialize_network_menu_list();
#endif