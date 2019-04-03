#ifndef __T7ECAN_H
#define __T7ECAN_H

#include "t7synth.h"
#include <string.h>
#include <stdint.h>

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

#define MY_NAME (aCANID_IDs[MY_ID].psz_name)
#define MY_NETID (aCANID_IDs[MY_ID].psz_netID)
#define MY_TEAM_ID (aCANID_IDs[MY_ID].u8_teamID)
#define MY_MEMBER_ID (aCANID_IDs[MY_ID].u8_memberID)
#define MY_MSG_ID(type)                                                                                                \
    ((MY_TEAM_ID << TEAM_ID_SHIFT_AMOUNT) | (MY_MEMBER_ID << MEMBER_ID_SHIFT_AMOUNT) | (type & typeIDMask))

ESOS_USER_TASK(request_temp);
ESOS_USER_TASK(ecan_receiver);
ESOS_USER_TASK(ecan_beacon_network);
ESOS_USER_TASK(ecan_clean_network);

void initialize_network();

uint16_t calcMsgID(uint8_t u8_arrayIndex);
uint8_t stripTeamID(uint16_t u16_MSG_ID);
uint8_t stripMemberID(uint16_t u16_MSG_ID);
uint8_t stripTypeID(uint16_t u16_MSG_ID);
int8_t getArrayIndexFromMsgID(uint16_t u16_MSG_ID);
int8_t getNameFromMsgID(uint16_t u16_MSG_ID, char *psz_name, uint8_t u8_len);

#endif
