/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 * 
 * t2_common.h - embedded systems Lab 02 task-shared code
 */

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"

// Traffic program defines
#define STATE_RED 0
#define STATE_AMBER 1
#define STATE_GREEN 2
#define STATE_TURN 3

uint8_t state[2]; //Curent light states 0=NS 1=EW

#define NORTH_SOUTH 0
#define EAST_WEST 1

#define SET_LIGHT_GREEN(s)                                                                                             \
    {                                                                                                                  \
        state[s] = STATE_GREEN;                                                                                        \
        state[!s] = STATE_RED;                                                                                         \
    }

#define SET_LIGHT_AMBER(s)                                                                                             \
    {                                                                                                                  \
        state[s] = STATE_AMBER;                                                                                        \
        state[!s] = STATE_RED;                                                                                         \
    }

#define SET_LIGHT_RED(s)                                                                                               \
    {                                                                                                                  \
        state[s] = STATE_RED;                                                                                          \
        state[!s] = STATE_RED;                                                                                         \
    }

#define SET_LIGHT_TURN(s)                                                                                              \
    {                                                                                                                  \
        state[s] = STATE_TURN;                                                                                         \
        state[!s] = STATE_RED;                                                                                         \
    }

#define DISPLAY_STATE(s)                                                                                               \
    {                                                                                                                  \
        if (state[s] == 0) {                                                                                           \
            LED1_ON();                                                                                                 \
            LED2_OFF();                                                                                                \
            LED3_HB_OFF();                                                                                             \
        } else if (state[s] == 1) {                                                                                    \
            LED1_OFF();                                                                                                \
            LED2_ON();                                                                                                 \
            LED3_HB_OFF();                                                                                             \
        } else if (state[s] == 2) {                                                                                    \
            LED1_OFF();                                                                                                \
            LED2_OFF();                                                                                                \
            LED3_HB_ON();                                                                                              \
        }                                                                                                              \
    }
