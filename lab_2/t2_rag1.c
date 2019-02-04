#include "revF14.h"

ESOS_USER_TASK(light_loop) {
    ESOS_TASK_BEGIN();
    while(1) {
        LIGHT_GREEN(EW);
        ESOS_TASK_WAIT_TICKS(10000);
        LIGHT_AMBER(EW);
        ESOS_TASK_WAIT_TICKS(3000);
        LIGHT_RED(EW);
        LIGHT_GREEN(NS);
        ESOS_TASK_WAIT_TICKS(10000);
        LIGHT_AMBER(NS);
        ESOS_TASK_WAIT_TICKS(3000);
        LIGHT_RED(NS);
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(display_state) {
    ESOS_TASK_BEGIN();
    while(1) {
        if(SW3_PRESSED) {
            DISPLAY_STATE(NS);
            ESOS_TASK_WAIT_TICKS(15);
        } else {
            DISPLAY_STATE(EW);
            ESOS_TASK_WAIT_TICKS(15);
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

void user_init() {
    LED1_CONFIG();
    LED2_CONFIG();
    LED3_HB_CONFIG();

    SW1_CONFIG();
    SW2_CONFIG();
    SW3_CONFIG();

    esos_RegisterTask(light_loop);
    esos_RegisterTask(display_state);
}