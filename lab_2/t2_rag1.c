/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 * 
 * t2_rag1.c - embedded systems Lab 02, Task 01 solution
 */

#include "t2_common.h"

ESOS_USER_TASK(light_loop)
{
    ESOS_TASK_BEGIN();
    while (1) {
        SET_LIGHT_GREEN(EAST_WEST);
        ESOS_TASK_WAIT_TICKS(10000);
        SET_LIGHT_AMBER(EAST_WEST);
        ESOS_TASK_WAIT_TICKS(3000);
        SET_LIGHT_RED(EAST_WEST);
        SET_LIGHT_GREEN(NORTH_SOUTH);
        ESOS_TASK_WAIT_TICKS(10000);
        SET_LIGHT_AMBER(NORTH_SOUTH);
        ESOS_TASK_WAIT_TICKS(3000);
        SET_LIGHT_RED(NORTH_SOUTH);
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(display_state)
{
    ESOS_TASK_BEGIN();
    while (1) {
        if (SW3_PRESSED) {
            DISPLAY_STATE(NORTH_SOUTH);
            ESOS_TASK_WAIT_TICKS(15);
        } else {
            DISPLAY_STATE(EAST_WEST);
            ESOS_TASK_WAIT_TICKS(15);
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

void user_init()
{
    LED1_CONFIG();
    LED2_CONFIG();
    LED3_HB_CONFIG();

    SW1_CONFIG();
    SW2_CONFIG();
    SW3_CONFIG();

    esos_RegisterTask(light_loop);
    esos_RegisterTask(display_state);
}
