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
#include "esos_sensor.h"
#include "esos_pic24_sensor.h"
#include "t4_strings.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

static uint16_t pu16_out;
static uint8_t output_state;

/*Create an ESOS application called t4_sensor1 that will exercise your new ESOS sensor ser-vice.
 Write an ESOS application that willflash a heartbeat LED (LED3) every 250 ms.DONE
 Use the ESOS user interface services created in Task #3, write an application that will display the potentiometer ADC
 result in 16-bit hexadecimal format on a computer connected via the serial port when the user depresses switch SW1. If
 the user single-presses SW2, the application will (repeatedly) sample the potentiometer (R5)voltage every 1 s and
 display the result on the computer screen. This will continue until the userpresses either SW2 again or SW1.
*/

ESOS_USER_TASK(display_output)
{
    static ESOS_TASK_HANDLE read_adc;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if (output_state == 0) {
            // output nothing, go straight to yield.
        } else if (output_state == 1) {
            // output once, then go to yield
            ESOS_ALLOCATE_CHILD_TASK(read_adc);
            ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_ON_AVAILABLE_SENSOR, POT_CHANNEL, ESOS_SENSOR_VREF_3V0);
            ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_SENSOR_QUICK_READ, &pu16_out);
            ESOS_SENSOR_CLOSE();
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(pu16_out);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            output_state = 0;
        } else if (output_state == 2) {
            // output every 1 second, until the output_state flag is unset
            ESOS_ALLOCATE_CHILD_TASK(read_adc);
            ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_ON_AVAILABLE_SENSOR, POT_CHANNEL, ESOS_SENSOR_VREF_3V0);
            do {
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_SENSOR_QUICK_READ, &pu16_out);
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(pu16_out);
                ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_TICKS(1000);
            } while (output_state == 2);
            ESOS_SENSOR_CLOSE();
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(pot_interface)
{
    static BOOL sw1_state, sw2_state;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if (sw1_state != esos_uiF14_isSW1Pressed()) {
            sw1_state = esos_uiF14_isSW1Pressed();
            if (sw1_state) {
                if (output_state == 2)
                    output_state = 0; // if continuous is running, stop it.
                else
                    output_state = 1; // otherwise print a single value out.
            }
        }
        if (sw2_state != esos_uiF14_isSW2Pressed()) {
            sw2_state = esos_uiF14_isSW2Pressed();
            if (sw2_state) {
                if (output_state == 2)
                    output_state = 0; // if continuous is running, stop it.
                else
                    output_state = 2; // otherwise start it.
            }
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();

    esos_RegisterTimer(heartbeat, 250);
    esos_RegisterTask(pot_interface);
    esos_RegisterTask(display_output);
}
