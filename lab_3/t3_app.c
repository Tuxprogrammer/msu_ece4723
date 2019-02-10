#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(rpg_interface)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if (!(esos_uiF14_isRPGTurning())) {
            esos_uiF14_turnLED2Off();
        } else if (esos_uiF14_isRPGTurningSlow()) {
            esos_uiF14_turnLED2On();
            esos_uiF14_flashLED2(0);
        } else if (esos_uiF14_isRPGTurningMedium()) {
            esos_uiF14_turnLED2On();
            esos_uiF14_flashLED2(500);
        } else if (esos_uiF14_isRPGTurningFast()) {
            esos_uiF14_turnLED2On();
            esos_uiF14_flashLED2(100);
        }
        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
};

ESOS_USER_TASK(switch_interface)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if ((esos_uiF14_isSW3Pressed() && esos_uiF14_isSW2Pressed()) ||
            (!esos_uiF14_isSW3Pressed() && esos_uiF14_isSW1Pressed())) {
            esos_uiF14_turnLED1On();
        } else {
            esos_uiF14_turnLED1Off();
        }

        if ((esos_uiF14_isSW3Pressed() && esos_uiF14_isSW2DoublePressed()) ||
            (!esos_uiF14_isSW3Pressed() && esos_uiF14_isSW1DoublePressed())) {
            static uint8_t count;
            esos_uiF14_turnLED1Off();

            for (count = 0; count < 6; count++) {
                esos_uiF14_toggleLED1();
                ESOS_TASK_WAIT_TICKS(100);
            }
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
};
ESOS_USER_TASK(feedback)
{
    ESOS_TASK_BEGIN();
    ESOS_TASK_YIELD();
    ESOS_TASK_END();
}
ESOS_USER_TASK(menu)
{
    ESOS_TASK_BEGIN();
    ESOS_TASK_YIELD();
    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();

    esos_RegisterTimer(heartbeat, 500);
    esos_RegisterTask(rpg_interface);
    esos_RegisterTask(switch_interface);
    esos_RegisterTask(feedback);
    esos_RegisterTask(menu);
}