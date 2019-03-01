/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * temperature_sensor.c - ESOS LCD Temperature Sensor
 */

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"
#include "esos_pic24_lcd44780.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(test)
{
    ESOS_TASK_BEGIN();

    esos_lcd44780_configDisplay();
    esos_lcd44780_setCursorHome();
    esos_lcd44780_setCursorDisplay(1);
    esos_lcd44780_setCursorBlink(1);

    esos_lcd44780_writeString(0, 0, "hello");

    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();
    esos_lcd44780_init();
    esos_RegisterTimer(heartbeat, 1000);
    esos_RegisterTask(test);
}