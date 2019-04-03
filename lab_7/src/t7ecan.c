#include "t7ecan.h"
#include "t7menu.h"

ESOS_USER_TASK(request_temp)
{
    static uint32_t u32_temp_request_tick = 0;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if ((b_requestLM60 || b_requestDS1631) && network_menu.u8_choice != MY_ID &&
            esos_GetSystemTick() - u32_temp_request_tick > TEMP_REQUEST_INTERVAL) {
            static uint16_t u16_request;
            if (b_requestLM60) {
                u16_request = CANMSG_TYPE_TEMPERATURE1 | calcMsgID(network_menu.u8_choice);
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Requesting LM60: ");
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_request);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            } else if (b_requestDS1631) {
                u16_request = CANMSG_TYPE_TEMPERATURE2 | calcMsgID(network_menu.u8_choice);
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Requesting DS1631: ");
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_request);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            }

            ESOS_ECAN_SEND(u16_request, 0, 0);
            u32_temp_request_tick = esos_GetSystemTick();
            ESOS_TASK_WAIT_TICKS(TEMP_REQUEST_INTERVAL);
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_receiver)
{
    static uint8_t buf[2] = { 0 };
    static uint8_t u8_buf_len;
    static uint16_t u16_can_id;
    static uint8_t u8_msg_type;
    static uint8_t u8_team_ID;
    static uint8_t u8_member_ID;
    static int8_t i8_i;
    static MAILMESSAGE msg;

    static ESOS_TASK_HANDLE read_temp;
    static uint16_t u16_out;
    static uint32_t u32_out;
    static int16_t i16_temp;

    ESOS_TASK_BEGIN();

    //esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_BEACON, 0x0001, MASKCONTROL_FIELD_NONZERO);
    esos_ecan_canfactory_subscribe(__pstSelf, MY_MSG_ID(CANMSG_TYPE_BEACON), 0x0000, MASKCONTROL_FIELD_NONZERO);

    while (TRUE) {
        ESOS_TASK_WAIT_FOR_MAIL();
        ESOS_TASK_GET_NEXT_MESSAGE(&msg);
        u16_can_id = msg.au16_Contents[0];
        u8_team_ID = stripTeamID(u16_can_id);
        u8_msg_type = stripTypeID(u16_can_id);
        u8_member_ID = stripMemberID(u16_can_id);
        int8_t i8_i = getArrayIndexFromMsgID(u16_can_id);
        u8_buf_len = ESOS_GET_PMSG_DATA_LENGTH((&msg)) - sizeof(uint16_t);
        memcpy(buf, &msg.au8_Contents[sizeof(uint16_t)], u8_buf_len);

        if (u8_msg_type == CANMSG_TYPE_BEACON) {
            if (i8_i >= 0) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                if (network[i8_i].tick == 0) {
                    ESOS_TASK_WAIT_ON_SEND_STRING("Adding board: ");
                    network_menu.ast_items[i8_i].b_hidden = FALSE;
                } else {
                    ESOS_TASK_WAIT_ON_SEND_STRING("Refreshing board: ");
                }
                ESOS_TASK_WAIT_ON_SEND_STRING(network[i8_i].can_id.psz_name);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                network[i8_i].tick = esos_GetSystemTick();
            }
        } else if (u8_msg_type == CANMSG_TYPE_TEMPERATURE1) {
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("Received CANMSG_TYPE_TEMPERATURE1\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            if (u8_buf_len == 2) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Storing LM60 data from another board\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                i16_temp = buf[0] << 8 | buf[1];
                network[i8_i].temp_lm60 = i16_temp;
                if (i8_i == network_menu.u8_choice && b_requestLM60) {
                    static char tmp[12] = { 0 };
                    convert_uint32_t_to_str(buf[0], tmp, 12, 10);
                    tmp[2] = '.';
                    buf[1] = buf[1] * 100 / 256;
                    convert_uint32_t_to_str(buf[1], tmp + 3, 8, 10);
                    if (buf[1] >= 0 && buf[1] <= 9) {
                        tmp[4] = tmp[3];
                        tmp[3] = '0';
                    }
                    tmp[5] = 'C';

                    uint8_t u8_index = 0;
                    for (u8_index = 0; u8_index < 8; u8_index++) {
                        lm60.lines[1][u8_index] = tmp[u8_index];
                    }
                }
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("new temp: ");
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint16_t)i16_temp);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            } else if (u8_buf_len == 0 && i8_i == MY_ID) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Responding to LM60 data request\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                // ESOS_ALLOCATE_CHILD_TASK(read_temp);
                // ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_ON_AVAILABLE_SENSOR, TEMP_CHANNEL, ESOS_SENSOR_VREF_3V0);
                // ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_SENSOR_READ, &u16_out, ESOS_SENSOR_ONE_SHOT,
                //                          ESOS_SENSOR_FORMAT_VOLTAGE);
                // ESOS_SENSOR_CLOSE();

                // u32_out = (uint32_t)u16_out * 1000; // convert to not use decimals
                // u32_out = (u32_out - 424000) / 625; // millimillivolts to temp

                // network[MY_ID].temp_lm60 = (int16_t)u32_out;

                // buf[0] = network[MY_ID].temp_lm60 >> 8;
                // buf[1] = network[MY_ID].temp_lm60;

                // static char tmp[12] = { 0 };
                // convert_uint32_t_to_str(buf[0], tmp, 12, 10);
                // tmp[2] = '.';
                // buf[1] = buf[1] * 100 / 256;
                // convert_uint32_t_to_str(buf[1], tmp + 3, 8, 10);
                // if (buf[1] >= 0 && buf[1] <= 9) {
                //     tmp[4] = tmp[3];
                //     tmp[3] = '0';
                // }
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("sending temp: ");
                // ESOS_TASK_WAIT_ON_SEND_STRING(tmp);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_TEMPERATURE1), buf, 2);
            }
        } else if (u8_msg_type == CANMSG_TYPE_TEMPERATURE2) {
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("Received CANMSG_TYPE_TEMPERATURE2\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            if (u8_buf_len == 2) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Storing DS1631 data from another board\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                i16_temp = buf[0] << 8 | buf[1];
                network[i8_i].temp_1631 = i16_temp;
                if (i8_i == network_menu.u8_choice && b_requestDS1631) {
                    static char tmp[12] = { 0 };
                    convert_uint32_t_to_str(buf[0], tmp, 12, 10);
                    tmp[2] = '.';
                    buf[1] = buf[1] * 100 / 256;
                    convert_uint32_t_to_str(buf[1], tmp + 3, 8, 10);
                    if (buf[1] >= 0 && buf[1] <= 9) {
                        tmp[4] = tmp[3];
                        tmp[3] = '0';
                    }
                    tmp[5] = 'C';

                    uint8_t u8_index = 0;
                    for (u8_index = 0; u8_index < 8; u8_index++) {
                        _1631.lines[1][u8_index] = tmp[u8_index];
                    }

                    // _1631.lines[1][0] = 'D';
                    // _1631.lines[1][1] = 'S';
                }
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("new temp: ");
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint16_t)i16_temp);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            } else if (u8_buf_len == 0 && i8_i == MY_ID) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Responding to DS1631 data request\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
                ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, 0xAA); // Send READ command
                ESOS_TASK_WAIT_ON_READ2I2C1(DS1631ADDR, buf[0], buf[1]);
                ESOS_TASK_SIGNAL_AVAILABLE_I2C();

                static char tmp[12] = { 0 };
                convert_uint32_t_to_str(buf[0], tmp, 12, 10);
                tmp[2] = '.';
                buf[1] = buf[1] * 100 / 256;
                convert_uint32_t_to_str(buf[1], tmp + 3, 8, 10);
                if (buf[1] >= 0 && buf[1] <= 9) {
                    tmp[4] = tmp[3];
                    tmp[3] = '0';
                }
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("sending temp: ");
                ESOS_TASK_WAIT_ON_SEND_STRING(tmp);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_TEMPERATURE2), buf, 2);
            }
        } else {
            // ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            // ESOS_TASK_WAIT_ON_SEND_STRING("Unable to interpret message type: ");
            // ESOS_TASK_WAIT_ON_SEND_UINT8(u8_msg_type);
            // ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            // ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }

        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_beacon_network)
{
    ESOS_TASK_BEGIN();

    while (TRUE) {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("BEACONING\n");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_BEACON), 0, 0);
        // ESOS_ECAN_SEND(0x7A0, 0, 0);
        ESOS_TASK_WAIT_TICKS(ECAN_BEACON_INTERVAL);
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_clean_network)
{
    ESOS_TASK_BEGIN();

    while (TRUE) {
        // ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        // ESOS_TASK_WAIT_ON_SEND_STRING("Cleaned network\n");
        // ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        // remove network members with currentTick - tick > ECAN_CLEAN_INTERVAL
        static uint32_t u32_curr_tick;
        static uint8_t u8_i;
        for (u8_i = 0; u8_i < NUM_OF_IDS; u8_i++) {
            u32_curr_tick = esos_GetSystemTick();
            // skip if this board or other has board never been online
            if (u8_i == MY_ID || network[u8_i].tick == 0) {
                continue;
            } else if (u32_curr_tick - network[u8_i].tick > ECAN_CLEAN_INTERVAL) {
                network[u8_i].tick = 0;
                network_menu.ast_items[u8_i].b_hidden = TRUE;
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Removing ");
                ESOS_TASK_WAIT_ON_SEND_STRING(network[u8_i].can_id.psz_name);
                ESOS_TASK_WAIT_ON_SEND_STRING("'s board from network.\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            }
            ESOS_TASK_YIELD();
        }
        ESOS_TASK_WAIT_TICKS(1000);
    }

    ESOS_TASK_END();
}

void initialize_network()
{
    uint8_t u8_i;
    for (u8_i = 0; u8_i < NUM_OF_IDS; u8_i++) {
        network[u8_i].can_id = aCANID_IDs[u8_i];
        if (u8_i == MY_ID) {
            network[u8_i].tick = esos_GetSystemTick();
        } else {
            network[u8_i].tick = 0;
        }
        network[u8_i].temp_lm60 = 0;
        network[u8_i].temp_1631 = 0;
    }
}

uint16_t calcMsgID(uint8_t u8_arrayIndex)
{
    return (aCANID_IDs[u8_arrayIndex].u8_teamID << TEAM_ID_SHIFT_AMOUNT) |
           (aCANID_IDs[u8_arrayIndex].u8_memberID << MEMBER_ID_SHIFT_AMOUNT);
}

uint8_t stripTeamID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & teamIDMask) >> TEAM_ID_SHIFT_AMOUNT;
}

uint8_t stripMemberID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & memberIDMask) >> MEMBER_ID_SHIFT_AMOUNT;
}

uint8_t stripTypeID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & typeIDMask) >> TYPE_ID_SHIFT_AMOUNT;
}

int8_t getArrayIndexFromMsgID(uint16_t u16_MSG_ID)
{
    int8_t i8_arrayIndex = -1; // Default - Not Found (-1)
    uint8_t u8_arrayIndex;

    for (u8_arrayIndex = 0; u8_arrayIndex < NUM_OF_IDS; u8_arrayIndex++) {
        if (calcMsgID(u8_arrayIndex) == (u16_MSG_ID & (teamIDMask | memberIDMask))) {
            i8_arrayIndex = u8_arrayIndex;
            break;
        }
    }

    return i8_arrayIndex;
}

int8_t getNameFromMsgID(uint16_t u16_MSG_ID, char *psz_name, uint8_t u8_len)
{
    int8_t i8_arrayIndex;

    i8_arrayIndex = getArrayIndexFromMsgID(u16_MSG_ID);
    if (i8_arrayIndex != -1) // If the Message ID was found
    {
        strncpy(psz_name, aCANID_IDs[i8_arrayIndex].psz_name, u8_len);
        return 0;
    }

    return -1;
}