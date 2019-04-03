#ifndef __T7SENSOR_H
#define __T7SENSOR_H
#include "t7synth.h"

ESOS_USER_TASK(set_led);
ESOS_CHILD_TASK(update_wvform, uint8_t u8_type, uint8_t u8_duty, uint8_t u8_ampl);
ESOS_USER_TASK(update_lm60);
ESOS_USER_TASK(update_ds1631);
#endif