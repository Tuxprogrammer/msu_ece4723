#include "esos_f14ui.h"
#include "esos.h"
#include "esos_pic24.h"

ESOS_USER_TIMER(heartbeat) {
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(rpg_interface);
ESOS_USER_TASK(switch_interface);
ESOS_USER_TASK(feedback);
ESOS_USER_TASK(menu);


void user_init() {
    config_esos_uiF14();

    esos_RegisterTimer(heartbeat, 500);
    esos_RegisterTask(rpg_interface);
    esos_RegisterTask(switch_interface);
    esos_RegisterTask(feedback);
    esos_RegisterTask(menu);
}