#ifndef EMBEDDED_CANLAB_H
#define EMBEDDED_CANLAB_H

#include <string.h>
#include <stdint.h>

typedef struct {
    char *psz_name;
    char *psz_netID;
    uint8_t u8_teamID;
    uint8_t u8_memberID;
} CAN_ID;

// TODO: redefine to fit our class members
#define NUM_OF_IDS 17 // 8 Students + 1 TA + 1 Professor

#define teamIDMask 0b0000011100000000 // eight teams
#define memberIDMask 0b0000000011100000 // eight members
#define typeIDMask 0b0000000000011111 // 32 possible MSGs
#define TEAM_ID_SHIFT_AMOUNT 8
#define MEMBER_ID_SHIFT_AMOUNT 5
#define TYPE_ID_SHIFT_AMOUNT 0

// Message Types
#define CANMSG_TYPE_POLL 0
#define CANMSG_TYPE_BEACON 1
#define CANMSG_TYPE_TEMPERATURE1 2
#define CANMSG_TYPE_TEMPERATURE2 3
#define CANMSG_TYPE_WAVEFORM 4
#define CANMSG_TYPE_POTENTIOMETER 5
#define CANMSG_TYPE_FREQUENCY 6
#define CANMSG_TYPE_AMPLITUDE 7
#define CANMSG_TYPE_DUTYCYCLE 8
#define CANMSG_TYPE_LEDS 9

// CAN ID Table
const CAN_ID aCANID_IDs[NUM_OF_IDS] = {
    //"Name", "netID", teamID, memberID, // Array Index
    { "BA Bowlin", "bab615", 1, 1 }, // 0
    { "DD Daniel", "ddd212", 1, 2 }, // 1
    { "KA Pace", "kap389", 1, 3 }, // 2
    { "CM Prehn", "cmp465", 1, 4 }, // 3
    { "JK Aulds", "jka87", 2, 1 }, // 4
    { "ED Farmer", "edf63", 2, 2 }, // 5
    { "B Han", "bh439", 2, 3 }, // 6
    { "JL Long", "jll574", 2, 4 }, // 7
    { "JR Johnson", "jrj309", 3, 1 }, // 8
    { "VD Kingma", "vdk20", 3, 2 }, // 9
    { "NP Lokhande", "npl25", 3, 3 }, // 10
    { "MJ McConville", "mjm719", 3, 4 }, // 11
    { "JW Hastings", "jwb522", 4, 1 }, // 12
    { "S Pacharne", "sp931", 4, 2 }, // 13
    { "AB Powell", "abp168", 4, 3 }, // 14
    { "BC Joiner", "bcj162", 7, 1 }, // 15
    { "JW Bruce", "jwbruce", 7, 7 } // 16
};

#define MY_ID 16 // Look above for your array index
#define MY_NAME (aCANID_IDs[MY_ID].psz_name)
#define MY_NETID (aCANID_IDs[MY_ID].psz_netID)
#define MY_TEAM_ID (aCANID_IDs[MY_ID].u8_teamID)
#define MY_MEMBER_ID (aCANID_IDs[MY_ID].u8_memberID)
#define MY_MSG_ID(type)                                                                                                \
    ((MY_TEAM_ID << TEAM_ID_SHIFT_AMOUNT) | (MY_MEMBER_ID << MEMBER_ID_SHIFT_AMOUNT) | (type & typeIDMask))

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

#endif /* EMBEDDED_CANLAB_H */
