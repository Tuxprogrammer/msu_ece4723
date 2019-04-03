/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t7rcv.c - esos app to receive a message using the CANFactory task
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

ESOS_USER_TASK(rcv_ecan_receiver)
{
    static uint8_t buf[2] = { 0 };
    static uint8_t u8_len;
    static uint16_t u16_can_id;
    static MAILMESSAGE msg;

    ESOS_TASK_BEGIN();

    esos_ecan_canfactory_subscribe(__pstSelf, ECAN_MSG_ID, 0xFFFF, MASKCONTROL_FIELD_NONZERO);

    while (TRUE) {
        ESOS_TASK_WAIT_FOR_MAIL();
        ESOS_TASK_GET_NEXT_MESSAGE(&msg);
        u16_can_id = msg.au16_Contents[0];
        u8_len = ESOS_GET_PMSG_DATA_LENGTH((&msg)) - sizeof(uint16_t);
        memcpy(buf, &msg.au8_Contents[sizeof(uint16_t)], u8_len);

        buf[0] ? esos_uiF14_turnLED1On() : esos_uiF14_turnLED1Off();
        buf[1] ? esos_uiF14_turnLED2On() : esos_uiF14_turnLED2Off();
        // LED1 = buf[0];
        // LED2 = buf[1];

        ESOS_TASK_YIELD();
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
    esos_RegisterTask(rcv_ecan_receiver);
    CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);
}
