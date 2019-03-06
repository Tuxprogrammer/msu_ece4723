/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * temperature_sensor.c - ESOS LCD Temperature Sensor
 */

//#define ESOS_LCD44780_NIBBLE_MODE

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"
#include "esos_pic24_lcd44780.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

void user_init()
{
    config_esos_uiF14();
    esos_lcd44780_init();
    esos_lcd44780_configDisplay();
    esos_RegisterTimer(heartbeat, 500);
}