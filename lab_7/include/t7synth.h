/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t7synth.c - app for wave generator and temperature sensor with menu
 */

#ifndef __T7SYNTH_H
#define __T7SYNTH_H

#include "revF14.h"
#include "fall17lib.h"
#include "esos_ecan.h"
#include "esos_menu.h"
#include "esos_f14ui.h"
#include "esos_sensor.h"
#include "esos_pic24.h"
#include "esos_pic24_ecan.h"
#include "esos_pic24_sensor.h"
#include "esos_pic24_spi.h"
#include "esos_pic24_i2c.h"
#include "esos_pic24_irq.h"
#include "esos_at24c02d.h"
#include "pic24_timer.h"
#include "esos_i2c_utils.h"
#include "esos_spi_utils.h"

static BOOL b_updateLM60;
static BOOL b_requestLM60;
static BOOL b_updateDS1631;
static BOOL b_requestDS1631;

static uint8_t u8_wvform_idx = 0;
static uint16_t wvform_data[128];

#define SINE_WVFORM_ADDR 0
#define USER_WVFORM_ADDR 128

#define TRI_WVFORM 0
#define SQUARE_WVFORM 1
#define SINE_WVFORM 2
#define USER_WVFORM 3

// #define ECAN_BEACON_INTERVAL 30000
// #define ECAN_CLEAN_INTERVAL 120000
#define ECAN_BEACON_INTERVAL 3000
#define ECAN_CLEAN_INTERVAL 10000

#define TEMP_REQUEST_INTERVAL 1000

#define CONFIG_FCNSYN_TIMER()                                                                                          \
    {                                                                                                                  \
        T4CONbits.T32 = 0;                                                                                             \
        T4CON = T4_PS_1_8 | T4_SOURCE_INT;                                                                             \
        TMR4 = 0;                                                                                                      \
        PR4 = FCY / 8 / 128 / freq.entries[0].value;                                                                   \
        TMR4 = 0;                                                                                                      \
        T4CONbits.TON = 1;                                                                                             \
    }

typedef struct {
    char *psz_name;
    char *psz_netID;
    uint8_t u8_teamID;
    uint8_t u8_memberID;
} CAN_ID;

typedef struct {
    CAN_ID can_id;
    uint32_t tick;
    int16_t temp_lm60;
    int16_t temp_1631;
} network_member;

#define NUM_OF_IDS 22 // 20 Students + 1 TA + 1 Professor

static const CAN_ID aCANID_IDs[NUM_OF_IDS] = {
    //"Name", "netID", teamID, memberID, // Array Index
    { "CB Bush", "cbb330", 1, 1 }, // 0
    { "SC Callicott", "sc2257", 1, 2 }, // 1
    { "LE Casey", "lec426", 1, 3 }, // 2
    { "WO Carroll", "woc17", 1, 4 }, // 3
    { "JD Fletcher", "jdf469", 1, 5 }, // 4
    { "JT Nguyen", "jtn136", 2, 1 }, // 5
    { "NR Siano", "nrs171", 2, 2 }, // 6
    { "IG Herbert", "igh9", 2, 3 }, // 7
    { "LA Ward", "law448", 2, 4 }, // 8
    { "RK Hunter", "rkh134", 2, 5 }, // 9
    { "G Singh", "gs656", 3, 1 }, // 10
    { "LR Haugh", "lrh282", 3, 2 }, // 11
    { "RE Omalley", "reo74", 3, 3 }, // 12
    { "BM Fritzhugh", "bmf151", 3, 4 }, // 13
    { "RF Jensen", "rfj18", 3, 5 }, // 14
    { "D Chapagain", "dc2274", 4, 1 }, // 15
    { "MA Merlin", "mam1218", 4, 2 }, // 16
    { "M Farhad", "mf1413", 4, 3 }, // 17
    { "BC Wilkinson", "bcw253", 4, 4 }, // 18
    { "JM Prather", "jmp784", 4, 5 }, // 19
    { "BC Joyner", "bcj162", 7, 1 }, // 20
    { "V Marojevic", "Vuk", 7, 2 } // 21
};

#ifndef MY_ID
#define MY_ID 3 // Look above for your array index
#endif

static network_member network[NUM_OF_IDS] = { 0 };

#endif