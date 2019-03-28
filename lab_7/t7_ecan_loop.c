/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t7_ecan_loop.c - app
 */

#include <string.h>
#include "esos.h"
#include "esos_f14ui.h"
#include "esos_pic24.h"
#include "esos_ecan.h"
#include "esos_pic24_ecan.h"
#include "pic24_ecan.h"

#define ESOS_DEBUG_FLAG ESOS_USER_FLAG_F

ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(ecan_sender)
{
    uint8_t buf[2];

    ESOS_TASK_BEGIN();

    while (TRUE) {
        buf[0] = esos_uiF14_isSW1Released();
        buf[1] = esos_uiF14_isSW2Released();

        if (buf[0]) esos_uiF14_turnLED1On();
        else esos_uiF14_turnLED1Off();
        if (buf[1]) esos_uiF14_turnLED2On();
        else esos_uiF14_turnLED2Off();

        ESOS_ECAN_SEND(0x7a0, buf, 2);

        ESOS_TASK_WAIT_TICKS(10);
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_receiver)
{
    uint8_t buf[8] = { 0 };
    uint8_t u8_len;
    uint16_t u16_can_id;

    ESOS_TASK_BEGIN();

    esos_ecan_canfactory_subscribe(__pstSelf, 0x7a0, 0xffff, MASKCONTROL_FIELD_NONZERO);

    while (TRUE) {
        static MAILMESSAGE msg;

        ESOS_TASK_WAIT_FOR_MAIL();
        ESOS_TASK_GET_NEXT_MESSAGE(&msg);
        u16_can_id = msg.au16_Contents[0];
        u8_len = ESOS_GET_PMSG_DATA_LENGTH((&msg - sizeof(uint16_t)));
        memcpy(buf, &msg.au8_Contents[sizeof(uint16_t)], u8_len);

        if (buf[0]) esos_uiF14_turnLED1On();
        else esos_uiF14_turnLED1Off();
        if (buf[1]) esos_uiF14_turnLED2On();
        else esos_uiF14_turnLED2Off();

        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

void user_init(void)
{
    config_esos_uiF14();
    esos_SetUserFlag(ESOS_DEBUG_FLAG); // To resemble figure 13.29

    esos_RegisterTimer(heartbeat_LED, 500);
    esos_RegisterTask(CANFactory);
    esos_RegisterTask(ecan_sender);
    esos_RegisterTask(ecan_receiver);

    __esos_ecan_hw_config_ecan();

    CHANGE_MODE_ECAN1(ECAN_MODE_LOOPBACK);
}
