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
#include "esos_pic24_sensor.h"
#include "t4_strings.h"

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

static uint8_t u8_proctype; // process type
static uint8_t u8_procnum; // number of processes
static uint16_t pu16_out;

static BOOL output_enabled;
static BOOL output_continuous;
static BOOL sw1_state;
static BOOL sw2_state;
static BOOL sw3_state;
static esos_sensor_process_t output_state;

/*
typedef enum {
    ESOS_SENSOR_ONE_SHOT = 0x00,
    ESOS_SENSOR_AVG2 = 0x01,      // arithmetic average
    ESOS_SENSOR_AVG4 = 0x02,
    ESOS_SENSOR_AVG8 = 0x03,
    ESOS_SENSOR_AVG16 = 0x04,
    ESOS_SENSOR_AVG32 = 0x05,
    ESOS_SENSOR_AVG64 = 0x06,
    ESOS_SENSOR_MIN2 = 0x21,      // minimum
    ESOS_SENSOR_MIN4 = 0x22,
    ESOS_SENSOR_MIN8 = 0x23,
    ESOS_SENSOR_MIN16 = 0x24,
    ESOS_SENSOR_MIN32 = 0x25,
    ESOS_SENSOR_MIN64 = 0x26,
    ESOS_SENSOR_MAX2 = 0x41,      // maximum
    ESOS_SENSOR_MAX4 = 0x42,
    ESOS_SENSOR_MAX8 = 0x43,
    ESOS_SENSOR_MAX16 = 0x44,
    ESOS_SENSOR_MAX32 = 0x45,
    ESOS_SENSOR_MAX64 = 0x46,
    ESOS_SENSOR_MEDIAN2 = 0x81,   // median
    ESOS_SENSOR_MEDIAN4 = 0x82,
    ESOS_SENSOR_MEDIAN8 = 0x83,
    ESOS_SENSOR_MEDIAN16 = 0x84,
    ESOS_SENSOR_MEDIAN32 = 0x85,
    ESOS_SENSOR_MEDIAN64 = 0x86,
} esos_sensor_process_t; */

ESOS_USER_TASK(display_output)
{
    static ESOS_TASK_HANDLE read_adc;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        // ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        // ESOS_TASK_WAIT_ON_SEND_STRING("display_output");
        // ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        if (output_enabled != TRUE) {
            // ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            // ESOS_TASK_WAIT_ON_SEND_STRING("output_disabled");
            // ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        } else {
            if (output_continuous) {
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
                } while (output_continuous == TRUE);
                ESOS_SENSOR_CLOSE();
            } else {
                // output once, then go to yield
                // ESOS_TASK_WAIT_ON_SEND_STRING("WE ARE OUTPUTTING STUFF NOW");
                ESOS_ALLOCATE_CHILD_TASK(read_adc);
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_ON_AVAILABLE_SENSOR, POT_CHANNEL, ESOS_SENSOR_VREF_3V0);
                // ESOS_TASK_WAIT_ON_SEND_STRING("_wait_sensor_availble");
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_SENSOR_READ, &pu16_out, output_state,
                                         ESOS_SENSOR_FORMAT_BITS); // could change this to percent to get a cool output
                // ESOS_TASK_WAIT_ON_SEND_STRING("_wait_sensor_read");

                ESOS_SENSOR_CLOSE();
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(pu16_out);
                ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                output_enabled = FALSE;
            }
        }

        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(adc_interface)
{
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if (sw1_state != esos_uiF14_isSW1Pressed()) {
            sw1_state = esos_uiF14_isSW1Pressed();
            if (sw1_state) {
                if (output_continuous == TRUE) {
                    output_continuous = FALSE; // if continuous is running, stop it.
                    output_enabled = FALSE;
                } else {
                    output_state = ESOS_SENSOR_ONE_SHOT; // otherwise print a single value out.
                    output_enabled = TRUE;
                }
            }
        }
        if (sw2_state != esos_uiF14_isSW2Pressed()) {
            sw2_state = esos_uiF14_isSW2Pressed();
            if (sw2_state) {
                if (output_continuous == TRUE) {
                    output_continuous = FALSE; // if continuous is running, stop it.
                    output_enabled = FALSE;
                } else {
                    output_state = ESOS_SENSOR_ONE_SHOT; // otherwise print a single value out.
                    output_enabled = TRUE;
                    output_continuous = TRUE;
                }
            }
        }
        if (sw3_state != esos_uiF14_isSW3Pressed()) {
            sw3_state = esos_uiF14_isSW3Pressed();
            if (sw3_state) {
                if (output_continuous == TRUE) {
                    output_continuous = FALSE; // if continuous is running, stop it.
                    output_enabled = FALSE;
                } else {
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
                    ESOS_TASK_WAIT_ON_GET_UINT8(u8_proctype);
                    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

                    if (u8_proctype != '1') {
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
                        ESOS_TASK_WAIT_ON_GET_UINT8(u8_procnum);
                        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
                    }

                    // Set processing state based on selection
                    output_state =
                        (u8_proctype == '2' && u8_procnum == '1') ?
                            ESOS_SENSOR_AVG2 :
                            (u8_proctype == '2' && u8_procnum == '2') ?
                            ESOS_SENSOR_AVG4 :
                            (u8_proctype == '2' && u8_procnum == '3') ?
                            ESOS_SENSOR_AVG8 :
                            (u8_proctype == '2' && u8_procnum == '4') ?
                            ESOS_SENSOR_AVG16 :
                            (u8_proctype == '2' && u8_procnum == '5') ?
                            ESOS_SENSOR_AVG32 :
                            (u8_proctype == '2' && u8_procnum == '6') ?
                            ESOS_SENSOR_AVG64 :
                            (u8_proctype == '3' && u8_procnum == '1') ?
                            ESOS_SENSOR_AVG2 :
                            (u8_proctype == '3' && u8_procnum == '2') ?
                            ESOS_SENSOR_AVG4 :
                            (u8_proctype == '3' && u8_procnum == '3') ?
                            ESOS_SENSOR_AVG8 :
                            (u8_proctype == '3' && u8_procnum == '4') ?
                            ESOS_SENSOR_AVG16 :
                            (u8_proctype == '3' && u8_procnum == '5') ?
                            ESOS_SENSOR_AVG32 :
                            (u8_proctype == '3' && u8_procnum == '6') ?
                            ESOS_SENSOR_AVG64 :
                            (u8_proctype == '4' && u8_procnum == '1') ?
                            ESOS_SENSOR_AVG2 :
                            (u8_proctype == '4' && u8_procnum == '2') ?
                            ESOS_SENSOR_AVG4 :
                            (u8_proctype == '4' && u8_procnum == '3') ?
                            ESOS_SENSOR_AVG8 :
                            (u8_proctype == '4' && u8_procnum == '4') ?
                            ESOS_SENSOR_AVG16 :
                            (u8_proctype == '4' && u8_procnum == '5') ?
                            ESOS_SENSOR_AVG32 :
                            (u8_proctype == '4' && u8_procnum == '6') ?
                            ESOS_SENSOR_AVG64 :
                            (u8_proctype == '5' && u8_procnum == '1') ?
                            ESOS_SENSOR_AVG2 :
                            (u8_proctype == '5' && u8_procnum == '2') ?
                            ESOS_SENSOR_AVG4 :
                            (u8_proctype == '5' && u8_procnum == '3') ?
                            ESOS_SENSOR_AVG8 :
                            (u8_proctype == '5' && u8_procnum == '4') ?
                            ESOS_SENSOR_AVG16 :
                            (u8_proctype == '5' && u8_procnum == '5') ?
                            ESOS_SENSOR_AVG32 :
                            (u8_proctype == '5' && u8_procnum == '6') ?
                            ESOS_SENSOR_AVG64 :
                            (u8_proctype == '6' && u8_procnum == '1') ?
                            ESOS_SENSOR_AVG2 :
                            (u8_proctype == '6' && u8_procnum == '2') ?
                            ESOS_SENSOR_AVG4 :
                            (u8_proctype == '6' && u8_procnum == '3') ?
                            ESOS_SENSOR_AVG8 :
                            (u8_proctype == '6' && u8_procnum == '4') ?
                            ESOS_SENSOR_AVG16 :
                            (u8_proctype == '6' && u8_procnum == '5') ?
                            ESOS_SENSOR_AVG32 :
                            (u8_proctype == '6' && u8_procnum == '6') ? ESOS_SENSOR_AVG64 : ESOS_SENSOR_ONE_SHOT;

                    output_enabled = TRUE;
                } // end if continuous
            } // end if sw3 true
        } // end if sw3pressed

        ESOS_TASK_YIELD();
    } // endwhile
    ESOS_TASK_END();
}

void user_init()
{
    u8_procnum = '1';
    u8_proctype = '1'; // doesn't matter
    output_enabled = FALSE;
    output_continuous = FALSE;
    output_state = ESOS_SENSOR_ONE_SHOT;
    sw1_state = FALSE;
    sw2_state = FALSE;
    sw3_state = FALSE;
    esos_RegisterTimer(heartbeat, 250);

    config_esos_uiF14();
    esos_RegisterTask(adc_interface);
    esos_RegisterTask(display_output);
}