/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t4_sensor2.c - ESOS Sensor Application 2
 */

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

void user_init()
{
    esos_RegisterTimer(heartbeat, 250);
    
}