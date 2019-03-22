/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t6_program_eeprom.c - Contains EEPROM preprogrammed values and programming code
 */

#include "esos.h"
#include "esos_f14ui.h"
#include "esos_at24c02d.h"
#include "revF14.h"

ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(read_temp_i2c)
{
    ESOS_TASK_BEGIN();

    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("\nInitializing Temperature Sensor...\n");
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
    ESOS_TASK_WAIT_ON_WRITE2I2C1(DS1631ADDR, 0xAC, 0x0C);
    ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, 0x51);
    ESOS_TASK_SIGNAL_AVAILABLE_I2C();
    ESOS_TASK_WAIT_TICKS(500);

    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("Done.\n");
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    static uint8_t u8_hi, u8_low;

    while (TRUE) {
        ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("Write READ_TEMP command\n");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, 0xAA);
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("Done.\n");
        ESOS_TASK_WAIT_ON_SEND_STRING("Read 2-byte val.");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_READ2I2C1(DS1631ADDR, u8_hi, u8_low);
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("Done.\n");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_SIGNAL_AVAILABLE_I2C();

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("READ: ");
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_hi);
        ESOS_TASK_WAIT_ON_SEND_STRING(" ");
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_low);

        ESOS_TASK_WAIT_ON_SEND_STRING("\n");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        ESOS_TASK_WAIT_TICKS(500);
    }

    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();
    esos_pic24_configI2C1(400);
    esos_RegisterTimer(heartbeat_LED, 500);
    esos_RegisterTask(read_temp_i2c);
}