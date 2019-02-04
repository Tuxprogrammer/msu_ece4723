#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"

// Traffic program defines
#define STATE_RED 0
#define STATE_AMBER 1
#define STATE_GREEN 2
#define STATE_LEFT 3

uint8_t state[2]; //Curent light states 0=NS 1=EW

#define NS 0
#define EW 1

#define LIGHT_GREEN(s)          \
    {                           \
        state[s] = STATE_GREEN; \
        state[!s] = STATE_RED;  \
    }

#define LIGHT_AMBER(s)          \
    {                           \
        state[s] = STATE_AMBER; \
        state[!s] = STATE_RED;  \
    }

#define LIGHT_RED(s)           \
    {                          \
        state[s] = STATE_RED;  \
        state[!s] = STATE_RED; \
    }

#define LIGHT_LEFT(s)          \
    {                          \
        state[s] = STATE_LEFT; \
        state[!s] = STATE_RED; \
    }

#define DISPLAY_STATE(s)        \
    {                           \
        if (state[s] == 0)      \
        {                       \
            LED1_ON();          \
            LED2_OFF();         \
            LED3_HB_OFF();      \
        }                       \
        else if (state[s] == 1) \
        {                       \
            LED1_OFF();         \
            LED2_ON();          \
            LED3_HB_OFF();      \
        }                       \
        else if (state[s] == 2) \
        {                       \
            LED1_OFF();         \
            LED2_OFF();         \
            LED3_HB_ON();       \
        }                       \
    }
