/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t4_sensor1.c - ESOS Sensor Application 1
 */

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

/*Create an ESOS application called t4_sensor1 that will exercise your new ESOS sensor ser-vice.
 Write an ESOS application that willflash a heartbeat LED (LED3) every 250 ms.DONE
 Use the ESOS user interface services created in Task #3, write an application that will display the potentiometer ADC
 result in 16-bit hexadecimal format on a computer connected via the serial port when the user depresses switch SW1. If
 the user single-presses SW2, the application will (repeatedly) sample the potentiometer (R5)voltage every 1 s and
 display the result on the computer screen. This will continue until the userpresses either SW2 again or SW1.
*/
ESOS_USER_TASK(pot_interface)
{
    static BOOL e_SW1_PRESSED;
    static BOOL e_SW2_PRESSED;
    static BOOL e_SW3_PRESSED;
    static BOOL e_SW1_DPRESSED;
    static BOOL e_SW2_DPRESSED;
    static BOOL e_SW3_DPRESSED;

    ESOS_TASK_BEGIN();
    while (TRUE) {
        if (e_SW1_PRESSED != esos_uiF14_isSW1Pressed()) {
            e_SW1_PRESSED = esos_uiF14_isSW1Pressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_SW1_PRESSED) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW1_PRESSED);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW1_RELEASED);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (e_SW2_PRESSED != esos_uiF14_isSW2Pressed()) {
            e_SW2_PRESSED = esos_uiF14_isSW2Pressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_SW2_PRESSED) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW2_PRESSED);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW2_RELEASED);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
        if (e_SW3_PRESSED != esos_uiF14_isSW3Pressed()) {
            e_SW3_PRESSED = esos_uiF14_isSW3Pressed();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            if (e_SW3_PRESSED) {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW3_PRESSED);
            } else {
                ESOS_TASK_WAIT_ON_SEND_STRING(str_e_SW3_RELEASED);
            }
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        }
    }
    ESOS_TASK_END();
}

void user_init()
{
    esos_RegisterTimer(heartbeat, 250);

    config_esos_uiF14();
    esos_RegisterTask(readpot)
}
