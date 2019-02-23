/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t4_sensor2.c - ESOS Sensor Application 1
 */

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"
#include "esos_sensor.h"
#include "t4_strings.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

static uint16_t pu16_buf[64];
static uint8_t output_state;

ESOS_USER_TASK(display_output)
{
    static ESOS_TASK_HANDLE read_adc;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        switch (output_state) {
            case 0:
                // output nothing, go straight to yield.
                break;
            case 1:
                // output once, then go to yield
                ESOS_ALLOCATE_CHILD_TASK(read_adc);
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_ON_AVAILABLE_SENSOR, ESOS_SENSOR_CH00, ESOS_SENSOR_VREF_3V0);
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_SENSOR_QUICK_READ, pu16_buf);
                ESOS_SENSOR_CLOSE();
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(pu16_buf[0]);
                ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                output_state = 0;
                break;
            case 2:
                // output every 1 second, until the output_state flag is unset
                ESOS_ALLOCATE_CHILD_TASK(read_adc);
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_ON_AVAILABLE_SENSOR, ESOS_SENSOR_CH00, ESOS_SENSOR_VREF_3V0);
                do {
                    ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_SENSOR_QUICK_READ, pu16_buf);
                    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                    ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(pu16_buf[0]);
                    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                    ESOS_TASK_WAIT_TICKS(1000);
                } while (output_state == 2);
                ESOS_SENSOR_CLOSE();
                break;
            case 3:
                // Print menu options
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING(str_m_PROCESS_SETTINGS);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_PROCESS_ONE);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_PROCESS_AVG);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_PROCESS_MIN);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_PROCESS_MAX);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_PROCESS_MED);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                // Wait for selection
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

                // Set processing state based on selection
                switch (u8_input) {
                    case '1':
                        //todo
                        break;
                    case '2':
                        break;
                    case '3':
                        break;
                    case '4':
                        break;
                    case '5':
                        break;
                    // default is no change
                }

                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING(str_m_SAMPLE_SETTINGS);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_SAMPLE_2);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_SAMPLE_4);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_SAMPLE_8);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_SAMPLE_16);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_SAMPLE_32);
                ESOS_TASK_WAIT_ON_SEND_STRING(str_madc_SAMPLE_64);
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                // Wait for selection
                ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
                ESOS_TASK_WAIT_ON_GET_UINT8(u8_input);
                ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

                // Set processing state based on selection
                switch (u8_input) {
                    case '1':
                        //todo
                        break;
                    case '2':
                        break;
                    case '3':
                        break;
                    case '4':
                        break;
                    case '5':
                        break;
                    case '6':
                        break;
                    // default is no change
                }
        }

        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(pot_interface)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if (esos_uiF14_isSW1Pressed()) {
            if (output_state == 2)
                output_state = 0; // if continuous is running, stop it.
            else
                output_state = 1; // otherwise print a single value out.
        }
        if (esos_uiF14_isSW2Pressed()) {
            if (output_state == 2)
                output_state = 0; // if continuous is running, stop it.
            else
                output_state = 2; // otherwise start it.
        }
        if (esos_uiF14_isSW3Pressed()) {
            if (output_state == 2)
                output_state = 0; // if continuous is running, stop it.
            else
                output_state = 3; // otherwise ask for user input
        }
    }
    ESOS_TASK_END();
}

void user_init()
{
    esos_RegisterTimer(heartbeat, 250);

    config_esos_uiF14();
    esos_RegisterTask(pot_interface);
    esos_RegisterTask(display_output);
}