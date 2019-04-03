/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t7send.c - esos app to send a message using the CANFactory task
 */

#include <string.h>
#include "esos.h"
#include "esos_f14ui.h"
#include "esos_pic24.h"
#include "esos_ecan.h"

#define ECAN_MSG_ID 0x7A0


ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(send_ecan_sender)
{
    uint8_t buf[2];

    ESOS_TASK_BEGIN();

    while (TRUE) {
        buf[0] = esos_uiF14_isSW1Pressed();
        buf[1] = esos_uiF14_isSW2Pressed();

        buf[0] ? esos_uiF14_turnLED1On() : esos_uiF14_turnLED1Off();
        buf[1] ? esos_uiF14_turnLED2On() : esos_uiF14_turnLED2Off();
        // LED1 = buf[0];
        // LED2 = buf[1];

        ESOS_ECAN_SEND(ECAN_MSG_ID, buf, 2);

        ESOS_TASK_WAIT_TICKS(10);
    }

    ESOS_TASK_END();
}

void user_init(void)
{
    __esos_unsafe_PutString(HELLO_MSG);

    config_esos_uiF14();
    __esos_ecan_hw_config_ecan();

    esos_RegisterTimer(heartbeat_LED, 500);
    esos_RegisterTask(CANFactory);
    esos_RegisterTask(send_ecan_sender);
    CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);
}
