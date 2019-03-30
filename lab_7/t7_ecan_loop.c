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

#define ECAN_MSG_ID 0x7A0
#define ECAN_MSG_ID_MASK 0x7FC

static uint32_t u32_dataHigh;
static uint32_t u32_dataLow;

uint32_t rrot32(uint32_t u32_x)
{
    if (u32_x & 0x1) {
        u32_x = u32_x >> 1;
        u32_x = u32_x | 0x80000000;
    } else
        u32_x = u32_x >> 1;
    return u32_x;
}

ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(ecan_sender)
{
    static uint32_t buf[2] = { 0 };
    static uint8_t u8_i = 0;

    ESOS_TASK_BEGIN();
    u32_dataHigh = 0xFEDCBA98;
    u32_dataLow = 0x76543210;

    while (TRUE) {
        buf[0] = u32_dataHigh;
        buf[1] = u32_dataLow;

        ESOS_ECAN_SEND(ECAN_MSG_ID + u8_i, buf, 8);

        if (++u8_i == 8)
            u8_i = 0;

        ESOS_TASK_WAIT_TICKS(1000);

        // ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_receiver)
{
    static uint32_t buf[2] = { 0 };
    static uint8_t u8_len;
    static uint16_t u16_can_id;
    static MAILMESSAGE msg;

    ESOS_TASK_BEGIN();

    esos_ecan_canfactory_subscribe(__pstSelf, ECAN_MSG_ID, ECAN_MSG_ID_MASK, MASKCONTROL_FIELD_NONZERO);

    while (TRUE) {
        ESOS_TASK_WAIT_FOR_MAIL();
        ESOS_TASK_GET_NEXT_MESSAGE(&msg);
        u16_can_id = msg.au16_Contents[0];
        u8_len = ESOS_GET_PMSG_DATA_LENGTH((&msg)) - sizeof(uint16_t);
        memcpy(buf, &msg.au8_Contents[sizeof(uint16_t)], u8_len);

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("Message ID: ");
        ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_can_id);
        ESOS_TASK_WAIT_ON_SEND_STRING(", ");
        ESOS_TASK_WAIT_ON_SEND_STRING("Out: ");
        ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(buf[0]);
        ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(buf[1]);
        ESOS_TASK_WAIT_ON_SEND_STRING(", ");
        ESOS_TASK_WAIT_ON_SEND_STRING("In: ");
        ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u32_dataHigh);
        ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u32_dataLow);
        ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        u32_dataHigh = rrot32(u32_dataHigh);
        u32_dataLow = rrot32(u32_dataLow);
    }

    ESOS_TASK_END();
}

void user_init(void)
{
    config_esos_uiF14();

    esos_RegisterTimer(heartbeat_LED, 500);
    esos_RegisterTask(CANFactory);
    esos_RegisterTask(ecan_sender);
    esos_RegisterTask(ecan_receiver);

    __esos_ecan_hw_config_ecan();

    CHANGE_MODE_ECAN1(ECAN_MODE_LOOPBACK);
    ENABLE_DEBUG_MODE();
}
