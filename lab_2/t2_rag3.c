#include "t2_common.h"

ESOS_USER_TASK(light_loop)
{
    ESOS_TASK_BEGIN();
    while (1)
    {
        SET_LIGHT_TURN(EAST_WEST);
        ESOS_TASK_WAIT_TICKS(10000);

        SET_LIGHT_GREEN(EAST_WEST);
        if (SW1_PRESSED)
        {
            ESOS_TASK_WAIT_TICKS(30000);
        }
        else
        {
            ESOS_TASK_WAIT_TICKS(10000);
        }

        SET_LIGHT_AMBER(EAST_WEST);
        ESOS_TASK_WAIT_TICKS(3000);

        SET_LIGHT_RED(EAST_WEST);
        if (SW1_PRESSED)
        {
            ESOS_TASK_WAIT_TICKS(1000);
        }

        SET_LIGHT_TURN(NORTH_SOUTH);
        ESOS_TASK_WAIT_TICKS(10000);

        SET_LIGHT_GREEN(NORTH_SOUTH);
        if (SW1_PRESSED)
        {
            ESOS_TASK_WAIT_TICKS(30000);
        }
        else
        {
            ESOS_TASK_WAIT_TICKS(10000);
        }

        SET_LIGHT_AMBER(NORTH_SOUTH);
        ESOS_TASK_WAIT_TICKS(3000);

        SET_LIGHT_RED(NORTH_SOUTH);
        if (SW1_PRESSED)
        {
            ESOS_TASK_WAIT_TICKS(1000);
        }
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(display_state)
{
    ESOS_TASK_BEGIN();
    while (1)
    {
        if (SW3_PRESSED)
        {
            DISPLAY_STATE(NORTH_SOUTH);
            ESOS_TASK_WAIT_TICKS(15);
        }
        else
        {
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
        || (state[1] == STATE_TURN && SW3_RELEASED))
    { // if EAST_WEST is STATE_TURN and SW3 is released
        LED1_OFF();
        LED2_OFF();
        LED3_HB_TOGGLE();
    }
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
    esos_RegisterTimer(left_turn, 125);
}
