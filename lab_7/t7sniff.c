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
#include "embedded_lab_CANLab.h"

#define ECAN_MSG_ID 0x7A0

ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(ecan_receiver)
{
    uint8_t buf[8] = { 0 };
    uint8_t u8_len;
    uint16_t u16_can_id;

    ESOS_TASK_BEGIN();

    // not sure if this line is correct
    esos_ecan_canfactory_subscribe(__pstSelf, NULL, 0x0000, MASKCONTROL_FIELD_NONZERO);

    while (TRUE) {
        static MAILMESSAGE msg;

        ESOS_TASK_WAIT_FOR_MAIL();
        ESOS_TASK_GET_NEXT_MESSAGE(&msg);
        u16_can_id = msg.au16_Contents[0];

        u8_len = ESOS_GET_PMSG_DATA_LENGTH((&msg)) - sizeof(uint16_t);
        memcpy(buf, &msg.au8_Contents[sizeof(uint16_t)], u8_len);

        uint8_t u8_msg_type = stripTypeID(u16_can_id);
        uint16_t u16_msg_id = (stripMemberID(u16_can_id) << 8) | stripTeamID(u16_can_id);

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("Msg Type: ");
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u8_msg_type);
        ESOS_TASK_WAIT_ON_SEND_STRING("\n\tMsg ID: ");
        ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_msg_id);
        ESOS_TASK_WAIT_ON_SEND_STRING("\n\tBytes recvd: \n");
        if (u8_len > 0) {
            uint8_t u8_num;
            for (u8_num = 0; u8_num < u8_len; u8_num++) {
                ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u8_num);
                ESOS_TASK_WAIT_ON_SEND_STRING(": ");
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[u8_num]);
                ESOS_TASK_WAIT_ON_SEND_STRING('\n');
            }
        } else {
            ESOS_TASK_WAIT_ON_SEND_STRING("\t");
        }

        ESOS_TASK_WAIT_ON_SEND_STRING("\nEND\n");

        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        if (buf[0])
            esos_uiF14_turnLED1On();
        else
            esos_uiF14_turnLED1Off();
        if (buf[1])
            esos_uiF14_turnLED2On();
        else
            esos_uiF14_turnLED2Off();

        ESOS_TASK_WAIT_TICKS(50);
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
    esos_RegisterTask(ecan_receiver);

    CHANGE_MODE_ECAN1(ECAN_MODE_LISTEN_ONLY);
}