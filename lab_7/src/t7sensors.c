#include "t7sensors.h"
#include "t7menu.h"

ESOS_USER_TASK(set_led)
{
    ESOS_TASK_BEGIN();
    LED1_OFF();
    LED2_OFF();
    LED3_HB_OFF();
    while (TRUE) {
        // Note that LED3_HB is negated because the pin goes high when LED3 is off
        LED1 = (leds.entries[0].value & 0b100) == 0b100;
        LED2 = (leds.entries[0].value & 0b010) == 0b010;
        LED3_HB = (leds.entries[0].value & 0b001) != 0b001;
        ESOS_TASK_WAIT_TICKS(50);
    }
    ESOS_TASK_END();
}

ESOS_CHILD_TASK(update_wvform, uint8_t u8_type, uint8_t u8_duty, uint8_t u8_ampl)
{
    ESOS_TASK_BEGIN();

    static uint8_t u16_addr;
    static uint16_t u16_scaledData;
    static uint8_t u8_rawData;
    static uint16_t i;

    static uint8_t u8_currAmplitude;
    u8_currAmplitude = u8_ampl * UINT8_MAX / 30;

    // gen tri wave
    if (u8_type == TRI_WVFORM) {
        // up
        for (i = 0; i < 64; i++) {
            u8_rawData = i * 2;
            u16_scaledData = u8_rawData * u8_currAmplitude;
            wvform_data[i] = u16_scaledData * 2;
        }

        // down
        for (i = 64; i > 0; i--) {
            u8_rawData = i * 2;
            u16_scaledData = u8_rawData * u8_currAmplitude;
            wvform_data[64 + (64 - i)] = u16_scaledData * 2;
        }
    }
    // gen square wave w duty
    if (u8_type == SQUARE_WVFORM) {
        // turn duty cycle into fraction of the 128 points per wave period
        u8_duty = 128 * u8_duty / 100;
        for (i = 0; i < 128; i++) {
            u8_rawData = (i < u8_duty) ? 255 : 0;
            u16_scaledData = u8_rawData * u8_currAmplitude;

            wvform_data[i] = u16_scaledData;
        }
    }
    // fetch sine or user waves
    if (u8_type == SINE_WVFORM || u8_type == USER_WVFORM) {
        if (u8_type == SINE_WVFORM) {
            u16_addr = SINE_WVFORM_ADDR;
        } else {
            u16_addr = USER_WVFORM_ADDR;
        }
        ESOS_DISABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4);
        for (i = 0; i < 128; i++) {
            ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
            ESOS_TASK_WAIT_ON_WRITE1I2C1(AT24C02D_ADDR, u16_addr + i);
            // ESOS_TASK_WAIT_TICKS(1);
            ESOS_TASK_WAIT_ON_READ1I2C1(AT24C02D_ADDR, u8_rawData);
            ESOS_TASK_SIGNAL_AVAILABLE_I2C();
            ESOS_TASK_WAIT_TICKS(1);

            uint16_t a = u8_rawData;
            uint16_t b = u8_currAmplitude;
            u16_scaledData = a * b;

            wvform_data[i] = u16_scaledData;
        }
        ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4);
    }

    ESOS_TASK_YIELD();
    ESOS_TASK_END();
}

ESOS_USER_TASK(update_lm60)
{
    // TODO: make this task read local LM60 and assign a signed 7.8 format temp to network[MY_ID].temp_lm60
    static ESOS_TASK_HANDLE read_temp;
    static uint16_t u16_out;
    static uint32_t u32_out, temp32_ipart, temp32_fpart;
    static char temp32_str[12];
    static uint8_t i;

    ESOS_TASK_BEGIN();

    while (TRUE) {
        if (b_updateLM60) {
            ESOS_ALLOCATE_CHILD_TASK(read_temp);
            ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_ON_AVAILABLE_SENSOR, TEMP_CHANNEL, ESOS_SENSOR_VREF_3V0);
            ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_SENSOR_READ, &u16_out, ESOS_SENSOR_ONE_SHOT,
                                     ESOS_SENSOR_FORMAT_VOLTAGE);
            ESOS_SENSOR_CLOSE();

            u32_out = (uint32_t)u16_out * 1000; // convert to not use decimals
            u32_out = (u32_out - 424000) / 625; // millimillivolts to temp

            network[MY_ID].temp_lm60 = (int16_t)u32_out;

            temp32_ipart = u32_out / 100; // convert to get the integer part
            temp32_fpart = u32_out - temp32_ipart * 100; // subtract out the integer part to get the decimal part

            convert_uint32_t_to_str(temp32_ipart, temp32_str, 12, 10);
            temp32_str[2] = '.';
            convert_uint32_t_to_str(temp32_fpart, temp32_str + 3, 8, 10);
            if (temp32_fpart >= 0 && temp32_fpart <= 9) {
                temp32_str[4] = temp32_str[3];
                temp32_str[3] = '0';
            }

            temp32_str[5] = 'C';

            for (i = 0; i < 8; i++) {
                lm60.lines[1][i] = temp32_str[i];
            }

            network[MY_ID].temp_lm60 = (int16_t)u32_out;
            lm60.value = u32_out;

            // We could update the menu with text on line 2 here
        }
        ESOS_TASK_WAIT_TICKS(125);
    }
    ESOS_TASK_END();
}


ESOS_USER_TASK(update_ds1631)
{
    ESOS_TASK_BEGIN();

    ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
    ESOS_TASK_WAIT_ON_WRITE2I2C1(DS1631ADDR, 0xAC, 0x0C);
    ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, 0x51);
    ESOS_TASK_SIGNAL_AVAILABLE_I2C();
    ESOS_TASK_WAIT_TICKS(250); // wait for ds to come online

    static uint8_t i, u8_hi, u8_lo;
    static char temp32_str[12];

    while (TRUE) {
        if (b_updateDS1631) {
            ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
            ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, 0xAA); // Send READ command
            ESOS_TASK_WAIT_ON_READ2I2C1(DS1631ADDR, u8_hi, u8_lo);
            ESOS_TASK_SIGNAL_AVAILABLE_I2C();

            convert_uint32_t_to_str(u8_hi, temp32_str, 12, 10);
            temp32_str[2] = '.';
            u8_lo = u8_lo * 100 / 256;
            convert_uint32_t_to_str(u8_lo, temp32_str + 3, 8, 10);
            if (u8_lo >= 0 && u8_lo <= 9) {
                temp32_str[4] = temp32_str[3];
                temp32_str[3] = '0';
            }

            temp32_str[5] = 'C';

            // ESOS_TASK_WAIT_ON_SEND_STRING("DS1631 ");
            // ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_hi);
            // ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
            // ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_lo);
            // ESOS_TASK_WAIT_ON_SEND_UINT8(' ');
            // ESOS_TASK_WAIT_ON_SEND_STRING(temp32_str);
            // ESOS_TASK_WAIT_ON_SEND_UINT8('\n');

            for (i = 0; i < 8; i++) {
                _1631.lines[1][i] = temp32_str[i];
            }

            network[MY_ID].temp_1631 = (int16_t)(((uint16_t)u8_hi << 8) | u8_lo);
            _1631.value = u8_hi;

            ESOS_TASK_WAIT_TICKS(750); // The Temp sensor can only poll at 750ms with 12 bit mode
            continue;
        }
        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}
