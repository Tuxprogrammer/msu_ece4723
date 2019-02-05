/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 * 
 * t2_rag1.c - embedded systems Lab 02, Task 01 solution
 */

#include "t2_common.h"

//User task for the light state machine
ESOS_USER_TASK(light_loop)
{
    ESOS_TASK_BEGIN();
    while (1) {
        //Set East-West to green and N-S to red for 10 seconds
        SET_LIGHT_GREEN(EAST_WEST);
        ESOS_TASK_WAIT_TICKS(10000);
        //Set East-West to amber and N-S to red for 3 seconds
        SET_LIGHT_AMBER(EAST_WEST);
        ESOS_TASK_WAIT_TICKS(3000);
        //Set East-West to red and N-S to green for 10 seconds
        SET_LIGHT_RED(EAST_WEST);
        SET_LIGHT_GREEN(NORTH_SOUTH);
        ESOS_TASK_WAIT_TICKS(10000);
        SET_LIGHT_AMBER(NORTH_SOUTH);
        //Set East-West to red and N-S to amber for 3 seconds
        ESOS_TASK_WAIT_TICKS(3000);
        SET_LIGHT_RED(NORTH_SOUTH);
    }
    ESOS_TASK_END();
}

//User task for updating the light display LEDs
ESOS_USER_TASK(display_state)
{
    ESOS_TASK_BEGIN();
    //loop always
    while (1) {
        //if SW3, show the N-S lights
        if (SW3_PRESSED) {
            DISPLAY_STATE(NORTH_SOUTH);
            ESOS_TASK_WAIT_TICKS(15);
        } else {
            //otherwise, show the E-W lights
            DISPLAY_STATE(EAST_WEST);
            ESOS_TASK_WAIT_TICKS(15);
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

//same as main
void user_init()
{
    //Configure LEDs
    LED1_CONFIG();
    LED2_CONFIG();
    LED3_HB_CONFIG();

    //Configure Switches
    SW1_CONFIG();
    SW2_CONFIG();
    SW3_CONFIG();

    //Register and start tasks
    esos_RegisterTask(light_loop);
    esos_RegisterTask(display_state);
}
