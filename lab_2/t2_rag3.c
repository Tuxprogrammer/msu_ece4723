/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t2_rag3.c - embedded systems Lab 02, Task 03 solution
 */

#include "t2_common.h"

// User task for the light state machine
ESOS_USER_TASK(light_loop)
{
    ESOS_TASK_BEGIN();
    while (1) {
        // Set East-West to turn and N-S to red for 10 seconds
        SET_LIGHT_TURN(EAST_WEST);
        ESOS_TASK_WAIT_TICKS(10000);

        // Set East-West to green and N-S to red for 10 seconds, unless its rush-hour which changes delay to 30 sec
        SET_LIGHT_GREEN(EAST_WEST);
        if (SW1_PRESSED) {
            ESOS_TASK_WAIT_TICKS(30000);
        } else {
            ESOS_TASK_WAIT_TICKS(10000);
        }

        // Set East-West to amber and N-S to red for 3 seconds
        SET_LIGHT_AMBER(EAST_WEST);
        ESOS_TASK_WAIT_TICKS(3000);

        // Set East-West to red and N-S to red for 1 seconds if its rush-hour
        SET_LIGHT_RED(EAST_WEST);
        if (SW1_PRESSED) {
            ESOS_TASK_WAIT_TICKS(1000);
        }

        // Set East-West to red and N-S to turn for 10 seconds
        SET_LIGHT_TURN(NORTH_SOUTH);
        ESOS_TASK_WAIT_TICKS(10000);

        // Set East-West to red and N-S to green for 10 seconds, unless its rush-hour which changes delay to 30 sec
        SET_LIGHT_GREEN(NORTH_SOUTH);
        if (SW1_PRESSED) {
            ESOS_TASK_WAIT_TICKS(30000);
        } else {
            ESOS_TASK_WAIT_TICKS(10000);
        }

        // Set East-West to red and N-S to amber for 3 seconds
        SET_LIGHT_AMBER(NORTH_SOUTH);
        ESOS_TASK_WAIT_TICKS(3000);

        // Set East-West to red and N-S to red for 1 seconds if its rush-hour
        SET_LIGHT_RED(NORTH_SOUTH);
        if (SW1_PRESSED) {
            ESOS_TASK_WAIT_TICKS(1000);
        }
    }
    ESOS_TASK_END();
}

// User task for updating the light display LEDs
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

ESOS_USER_TIMER(left_turn)
{
    if ((state[0] == STATE_TURN && SW3_PRESSED) // if NORTH_SOUTH is STATE_TURN and SW3 is pressed
        || (state[1] == STATE_TURN && SW3_RELEASED)) { // if EAST_WEST is STATE_TURN and SW3 is released
        LED1_OFF();
        LED2_OFF();
        LED3_HB_TOGGLE();
    }
}

// same as main
void user_init()
{
    // Configure LEDs
    LED1_CONFIG();
    LED2_CONFIG();
    LED3_HB_CONFIG();

    // Configure Switches
    SW1_CONFIG();
    SW2_CONFIG();
    SW3_CONFIG();

    // Register and start tasks
    esos_RegisterTask(light_loop);
    esos_RegisterTask(display_state);
    esos_RegisterTimer(left_turn, 125);
}
