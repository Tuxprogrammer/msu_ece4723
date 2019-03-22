/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * temperature_lcd.c - ESOS LCD Temperature Sensor
 */

//#define ESOS_LCD44780_NIBBLE_MODE

#include "revF14.h"
#include "esos.h"
#include "esos_pic24.h"
#include "esos_f14ui.h"
#include "esos_sensor.h"
#include "esos_pic24_sensor.h"
#include "esos_lcd44780.h"
#include "esos_lcd44780_customChars.h"

#define REFRESH_RATE 200

static uint16_t pu16_out;
static BOOL display_state;
static uint16_t refresh_timeout;

ESOS_USER_TIMER(heartbeat)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(display_state_setter)
{
    ESOS_TASK_BEGIN();
    display_state = 0;
    while (TRUE) {
        // toggle state only once on press
        ESOS_TASK_WAIT_UNTIL_UIF14_SW3_PRESSED();
        display_state = !display_state;
        esos_lcd44780_clearScreen();
        refresh_timeout = REFRESH_RATE;
        ESOS_TASK_WAIT_UNTIL_UIF14_SW3_RELEASED();

        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(display_output)
{
    static ESOS_TASK_HANDLE read_adc;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if (refresh_timeout == REFRESH_RATE) {
            refresh_timeout = 0;
            if (display_state) {
                esos_lcd44780_init_custom_chars_bar();
                // ESOS_TASK_WAIT_ON_SEND_STRING("WE ARE OUTPUTTING STUFF NOW");
                ESOS_ALLOCATE_CHILD_TASK(read_adc);
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_ON_AVAILABLE_SENSOR, TEMP_CHANNEL, ESOS_SENSOR_VREF_3V0);
                // ESOS_TASK_WAIT_ON_SEND_STRING("_wait_sensor_availble");
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_SENSOR_READ, &pu16_out, ESOS_SENSOR_ONE_SHOT,
                                         ESOS_SENSOR_FORMAT_VOLTAGE); // could change this to percent to get a cool
                                                                      // output
                // ESOS_TASK_WAIT_ON_SEND_STRING("_wait_sensor_read");
                ESOS_SENSOR_CLOSE();

                // Vo = (6.25mV * T) + 424mV
                // T ~= (pu16_out - 424) / 6

                static uint32_t pu32_out;
                static uint32_t temp32_ipart;
                static uint32_t temp32_fpart;

                pu32_out = (uint32_t)pu16_out * 1000; // convert to not use decimals
                pu32_out = (pu32_out - 424000) / 625; // millimillivolts to temp
                temp32_ipart = pu32_out / 100; // convert to get the integer part
                temp32_fpart = pu32_out - temp32_ipart * 100; // subtract out the integer part to get the decimal part

                static uint32_t i = 0;
                static char temp32_str[12];

                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(pu32_out);
                ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
                ESOS_TASK_WAIT_ON_SEND_UINT8('=');
                ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
                convert_uint32_t_to_str(temp32_ipart, temp32_str, 12, 10);
                ESOS_TASK_WAIT_ON_SEND_STRING(temp32_str);
                ESOS_TASK_WAIT_ON_SEND_UINT8('.');
                convert_uint32_t_to_str(temp32_fpart, temp32_str, 12, 10);
                if (temp32_fpart >= 0 && temp32_fpart <= 9) {
                    ESOS_TASK_WAIT_ON_SEND_UINT8('0');
                }
                ESOS_TASK_WAIT_ON_SEND_STRING(temp32_str);
                ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
                ESOS_TASK_WAIT_ON_SEND_UINT8('C');
                ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                esos_lcd44780_writeString(0, 0, "LM60");
                convert_uint32_t_to_str(temp32_ipart, temp32_str, 12, 10);
                esos_lcd44780_writeString(1, 0, temp32_str);
                esos_lcd44780_writeChar(1, 2, ' ');
                esos_lcd44780_writeChar(1, 3, 'C');

                uint8_t u8_barTop = ' ';
                uint8_t u8_barBottom = ' ';

                if (pu32_out < 2000) {
                    // do nothing because its fookin cold
                } else if (pu32_out >= 2000 && pu32_out < 2100) {
                    u8_barBottom = _1EIGTH_BAR;
                } else if (pu32_out >= 2100 && pu32_out < 2200) {
                    u8_barBottom = _2EIGTH_BAR;
                } else if (pu32_out >= 2200 && pu32_out < 2300) {
                    u8_barBottom = _3EIGTH_BAR;
                } else if (pu32_out >= 2300 && pu32_out < 2400) {
                    u8_barBottom = _4EIGTH_BAR;
                } else if (pu32_out >= 2400 && pu32_out < 2500) {
                    u8_barBottom = _5EIGTH_BAR;
                } else if (pu32_out >= 2500 && pu32_out < 2600) {
                    u8_barBottom = _6EIGTH_BAR;
                } else if (pu32_out >= 2600 && pu32_out < 2700) {
                    u8_barBottom = _7EIGTH_BAR;
                } else if (pu32_out >= 2700 && pu32_out < 2800) {
                    u8_barBottom = _8EIGTH_BAR;
                } else if (pu32_out >= 2800 && pu32_out < 2900) {
                    u8_barBottom = _8EIGTH_BAR;
                    u8_barTop = _1EIGTH_BAR;
                } else if (pu32_out >= 2900 && pu32_out < 3000) {
                    u8_barBottom = _8EIGTH_BAR;
                    u8_barTop = _2EIGTH_BAR;
                } else if (pu32_out >= 3000 && pu32_out < 3100) {
                    u8_barBottom = _8EIGTH_BAR;
                    u8_barTop = _3EIGTH_BAR;
                } else if (pu32_out >= 3100 && pu32_out < 3200) {
                    u8_barBottom = _8EIGTH_BAR;
                    u8_barTop = _4EIGTH_BAR;
                } else if (pu32_out >= 3200 && pu32_out < 3300) {
                    u8_barBottom = _8EIGTH_BAR;
                    u8_barTop = _5EIGTH_BAR;
                } else if (pu32_out >= 3300 && pu32_out < 3400) {
                    u8_barBottom = _8EIGTH_BAR;
                    u8_barTop = _6EIGTH_BAR;
                } else if (pu32_out >= 3400 && pu32_out < 3500) {
                    u8_barBottom = _8EIGTH_BAR;
                    u8_barTop = _7EIGTH_BAR;
                } else {
                    u8_barBottom = _8EIGTH_BAR;
                    u8_barTop = _8EIGTH_BAR;
                }

                esos_lcd44780_writeChar(0, 7, u8_barTop);
                esos_lcd44780_writeChar(1, 7, u8_barBottom);

            } else {
                esos_lcd44780_init_custom_chars_slider();
                ESOS_ALLOCATE_CHILD_TASK(read_adc);
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_ON_AVAILABLE_SENSOR, POT_CHANNEL, ESOS_SENSOR_VREF_3V0);
                ESOS_TASK_SPAWN_AND_WAIT(read_adc, _WAIT_SENSOR_QUICK_READ, &pu16_out);
                ESOS_SENSOR_CLOSE();
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(pu16_out);
                ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                esos_lcd44780_writeString(0, 0, "pot 0x");

                static uint8_t pu8_out;
                static char pot_str[3];
                static uint8_t au8_slider[8];
                static int i;
                for (i = 0; i < 8; i++) {
                    au8_slider[i] = SLIDER_LINE;
                }

                pu8_out = pu16_out & 0xFF;

                // determine which 5x8 LCD cell to display the indicator bar
                i = pu16_out >> 9;
                // within the determined 5x8 LCD cell, scale discretely with 5 bar placements
                au8_slider[i] = ((pu16_out & 0x1FF) / 0x067) + 1;

                convert_uint32_t_to_str(pu16_out >> 4, pot_str, 3, 16);
                // hack to fix convert_uint32_t_to_str not leaving leading 0s
                if (pot_str[1] == 0) {
                    pot_str[2] = pot_str[1];
                    pot_str[1] = pot_str[0];
                    pot_str[0] = '0';
                }
                esos_lcd44780_writeString(0, 6, pot_str);
                esos_lcd44780_writeBuffer(1, 0, au8_slider, 8);
            }
        } else {
            refresh_timeout += 10;
            ESOS_TASK_WAIT_TICKS(10);
            continue;
        }

        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();
    esos_lcd44780_init();
    esos_lcd44780_configDisplay();

    esos_RegisterTimer(heartbeat, 500);
    esos_RegisterTask(display_state_setter);
    esos_RegisterTask(display_output);
}
