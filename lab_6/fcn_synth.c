#include "revF14.h"
#include "fall17lib.h"
#include "esos_menu.h"
#include "esos_f14ui.h"
#include "esos_sensor.h"
#include "esos_pic24.h"
#include "esos_pic24_sensor.h"
#include "esos_pic24_spi.h"
#include "esos_pic24_i2c.h"
#include "esos_pic24_irq.h"
#include "esos_at24c02d.h"

static BOOL b_updateLM60;
static BOOL b_updateDS1631;

static uint8_t wvform_data[120];

/*TODO: Ctrl+F replace all instances of mm with a name that abides
        by the coding standards.
*/
static esos_menu_longmenu_t main_menu = {
    .u8_numitems = 8,
    .u8_choice = 0, // Default
    .ast_items =
        {
            { "Set", "wvform", 0},
            { "Set", "freq", 0 },
            { "Set", "ampltd", 0 },
            { "Set", "duty", 1 },
            { "Read", "LM60", 0 },
            { "Read", "1631", 0 },
            { "Set", "LEDs", 0 },
            { "", "About...", 0 },
        },
};

// TODO: determine if this is the correct type of entry for the wvform
static esos_menu_longmenu_t wvform = {
    .u8_numitems = 4,
    .u8_choice = 0,
    .ast_items = { { "Select", "tri", 0 },
                   { "Select", "square", 0 },
                   { "Select", "sine", 0 },
                   { "Select", "user1", 0 } },
};

static esos_menu_entry_t freq = {
    .entries[0].label = "Freq = ",
    .entries[0].value = 1000, // Default, Freq = 1k
    .entries[0].min = 64,
    .entries[0].max = 2047,
};

// note ampl is 10 times the value to avoid floats
static esos_menu_entry_t ampl = {
    .entries[0].label = "Ampl = ",
    .entries[0].value = 10, // Default, ampl = 1.0V
    .entries[0].min = 0,
    .entries[0].max = 30,
};

static esos_menu_entry_t duty = {
    .entries[0].label = "Duty = ",
    .entries[0].value = 50, // Default, duty cycle = 50%
    .entries[0].min = 0,
    .entries[0].max = 100,
};

static esos_menu_entry_t leds = {
    .entries[0].label = "LEDs = ",
    .entries[0].value = 3,
    .entries[0].min = 0,
    .entries[0].max = 7,
};

static esos_menu_sliderbar_t lm60 = {
    .value = 2500,
    .min = 2000,
    .max = 3500,
    .div = 100,
    .type = 1,
    .lines = { { "LM60" }, { "       " } },
};

static esos_menu_sliderbar_t _1631 = {
    .value = 25,
    .min = 18,
    .max = 34,
    .div = 1,
    .type = 1,
    .lines = { { "DS1631" }, { "       " } },
};

// TODO: figure out what goes in about
static esos_menu_staticmenu_t about = {
    .u8_numlines = 2,
    .u8_currentline = 0,
    .lines = { { "its abt" }, { "time" } },
};

void configSPI1(void)
{
    // PIN SETUP
    MCP4922_SCK_CONFIG();
    MCP4922_SDI_CONFIG();
    MCP4922_SDO_CONFIG();
    MCP4922_CS_CONFIG();
    MCP4922_SHDN_CONFIG();

    CONFIG_SCK1OUT_TO_RP(RD5_RP);

    CONFIG_SDO1_TO_RP(RD4_RP);
    CONFIG_SDI1_TO_RP(RD9_RP);

    // spi clock = 40MHz/1*4 = 40MHz/4 = 10MHz
    SPI1CON1 = PRI_PRESCAL_1_1 | // 1:1 Primary prescale
               SEC_PRESCAL_3_1 | // 3:1 Secondary prescale
               CLK_POL_ACTIVE_HIGH | // clock active high (CKP = 0)
               SPI_CKE_ON | // out changes inactive to active (CKE=0)
               SPI_MODE16_ON | // 8-bit mode
               MASTER_ENABLE_ON; // master mode

    MCP4922_ON();
    SLAVE_DISABLE(); // disable the chip select
    SPI1STATbits.SPIEN = 1; // enable SPI mode
}

void writeSPI(uint16_t *pu16_out, uint16_t *pu16_in, uint16_t u16_cnt)
{
    static uint16_t *pu16_tempPtrIn;
    static uint16_t *pu16_tempPtrOut;
    static uint16_t u16_tempCnt, u16_i;
    static uint8_t u8_isReading, u8_isWriting;
    uint16_t u16_scratch;
    pu16_tempPtrOut = pu16_out;
    pu16_tempPtrIn = pu16_in;
    u16_tempCnt = u16_cnt;

    if (pu16_tempPtrOut == NULLPTR)
        u8_isWriting = FALSE;
    else
        u8_isWriting = TRUE;

    if (pu16_tempPtrIn == NULLPTR)
        u8_isReading = FALSE;
    else
        u8_isReading = TRUE;

    // clear the overflow flag, just in case it is set
    if (SPI1STATbits.SPIROV)
        SPI1STATbits.SPIROV = 0;
    // clear SPI interrupt flag since we are about to write new value to SPI
    _SPI1IF = 0;
    /* read SPI1BUF to clear SPI_RX_BUFFER_FULL bit just in case previous
    SPI use did not read the SPI1BUF that last time!
    */
    u16_scratch = SPI1BUF;
    for (u16_i = 0; u16_i < u16_tempCnt; u16_i++) {
        if (u8_isWriting) {
            SPI1BUF = *pu16_tempPtrOut;
            pu16_tempPtrOut++;
        } else {
            SPI1BUF = 0;
        } // end isWriting

        /* Seen some strange behavior checking _SPI1IF like the
         * hardware support library.  The following method is valid
         * and appears to work in all cases.
         */
        // wait for TX word to be copied to SPI1SR

        while (SPI1STAT & SPI_TX_BUFFER_FULL) {
            // do nothing
        }

        // wait for RX word to be copied from SPI1SR
        while (!(SPI1STAT & SPI_RX_BUFFER_FULL)) {
            // do nothing
        }
        // read the word from SPI (clears SPI_RX_BUFFER_FULL bit)
        u16_scratch = SPI1BUF;
        if (u8_isReading) {
            *pu16_tempPtrIn = u16_scratch;
            pu16_tempPtrIn++;
        } // end isReading
    } // end for()
}

void write_DAC(uint16_t u16_data)
{
    // WRITE COMMAND
    // 0b<a/b><buf><GA><SHDN><d11:0>
    // 0b0011 | 16-bit number

    u16_data = 0x3000 | (u16_data >> 4);

    /*
    ESOS_TASK_WAIT_ON_SEND_STRING("_WRITE_DAC_AB: ");
    ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_data);
    ESOS_TASK_WAIT_ON_SEND_STRING("\n");
    */
    SLAVE_ENABLE();
    // ESOS_TASK_WAIT_ON_WRITE1SPI1(u16_data);
    writeSPI(&u16_data, NULLPTR, 1);
    __builtin_nop();
    SLAVE_DISABLE();
}

static uint8_t u8_cycle_time = 0;
static uint16_t u8_out_prim = 0;
ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_T4)
{
    if (wvform.u8_choice == 0) { // triangle wave
        if (u8_cycle_time < 120) {
            u8_out_prim += 2;
        } else {
            u8_out_prim -= 2;
        }
    } else if (wvform.u8_choice == 1) {
        if (u8_out_prim > 0) {
            u8_out_prim = 0;
        } else {
            u8_out_prim = 255;
        }
    } else if (wvform.u8_choice == 2) {
        if (u8_cycle_time < 60) {
            u8_out_prim = (wvform_data[(u8_cycle_time % 60) * 3 / 2] / 2) + 126;
        } else if (u8_cycle_time < 120) {
            u8_out_prim = (wvform_data[((60 - (u8_cycle_time % 60)) * 3 / 2)] / 2) + 126;
        } else if (u8_cycle_time < 180) {
            u8_out_prim = (-wvform_data[(u8_cycle_time % 60) * 3 / 2] / 2) - 127;
        } else {
            u8_out_prim = (-wvform_data[((60 - (u8_cycle_time % 60)) * 3 / 2)] / 2) - 127;
        }

    } else if (wvform.u8_choice == 3) {
        u8_out_prim = wvform_data[u8_cycle_time % 120];
    }

    uint16_t dac_out_val =
        (((uint16_t)u8_out_prim * 255 / 240) << 8) / (ampl.entries[0].max - ampl.entries[0].value) / 2 * 3;
    write_DAC(dac_out_val);

    u8_cycle_time = (u8_cycle_time + 1) % 240;

    PR4 = 60000 / freq.entries[0].value * 240;
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T4);
}

ESOS_CHILD_TASK(update_wvform, uint8_t u8_type)
{
    ESOS_TASK_BEGIN();

    static uint16_t u16_addr;
    static uint8_t u8_data;
    static uint16_t i;

    if (u8_type == 2 || u8_type == 3) {
        if (u8_type == 2) {
            u16_addr = 8; // sine wave data start at 8
        } else {
            u16_addr = 136; // noise wave data starts at 136
        }

        for (i = 0; i < 120; i++) {
            ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
            ESOS_TASK_WAIT_ON_WRITE1I2C1(AT24C02D_ADDR, u16_addr + i);
            ESOS_TASK_WAIT_ON_READ1I2C1(AT24C02D_ADDR, u8_data);
            ESOS_TASK_SIGNAL_AVAILABLE_I2C();

            wvform_data[i] = u8_data;
        }
    }

    ESOS_TASK_YIELD();
    ESOS_TASK_END();
}

ESOS_USER_TASK(lcd_menu)
{
    static ESOS_TASK_HANDLE update_hdl;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        // Display main menu until the user presses SW3 to choose a selection

        // // couldn't get the menu switching to work with pointers for cleaner code
        // // TODO: determine if possible and refactor if so
        // esos_menu_longmenu_t *menu = &main_menu;
        // if (wvform.u8_choice == 2) {
        //     menu = &main_menu_with_duty;
        // }

        // if square wave selected, show duty cycle
        if (wvform.u8_choice == 2) {
            main_menu.ast_items[3].b_hidden = FALSE;
        }

        ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(main_menu);
        if (main_menu.u8_choice == 0) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(wvform);
            ESOS_ALLOCATE_CHILD_TASK(update_hdl);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice);

        } else if (main_menu.u8_choice == 1) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(freq);
        } else if (main_menu.u8_choice == 2) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(ampl);
        } else if (main_menu.u8_choice == 3) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(duty);
        } else if (main_menu.u8_choice == 4) {
            b_updateLM60 = 1;
            ESOS_TASK_WAIT_ESOS_MENU_SLIDERBAR(lm60);
            b_updateLM60 = 0;
        } else if (main_menu.u8_choice == 5) {
            b_updateDS1631 = 1;
            ESOS_TASK_WAIT_ESOS_MENU_SLIDERBAR(_1631);
            b_updateDS1631 = 0;
        } else if (main_menu.u8_choice == 6) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(leds);
        } else if (main_menu.u8_choice == 7) {
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(about);
        }
    }
    ESOS_TASK_END();
}

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

ESOS_USER_TASK(update_lm60)
{
    static ESOS_TASK_HANDLE read_temp;
    static uint16_t pu16_out;
    static uint32_t pu32_out, temp32_ipart, temp32_fpart;
    static char temp32_str[12];
    static uint8_t i;

    ESOS_TASK_BEGIN();

    while (TRUE) {
        if (b_updateLM60) {
            ESOS_ALLOCATE_CHILD_TASK(read_temp);
            ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_ON_AVAILABLE_SENSOR, TEMP_CHANNEL, ESOS_SENSOR_VREF_3V0);
            ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_SENSOR_READ, &pu16_out, ESOS_SENSOR_ONE_SHOT,
                                     ESOS_SENSOR_FORMAT_VOLTAGE);
            ESOS_SENSOR_CLOSE();

            pu32_out = (uint32_t)pu16_out * 1000; // convert to not use decimals
            pu32_out = (pu32_out - 424000) / 625; // millimillivolts to temp

            temp32_ipart = pu32_out / 100; // convert to get the integer part
            temp32_fpart = pu32_out - temp32_ipart * 100; // subtract out the integer part to get the decimal part

            convert_uint32_t_to_str(temp32_ipart, temp32_str, 12, 10);
            temp32_str[2] = '.';
            convert_uint32_t_to_str(temp32_fpart, temp32_str + 3, 8, 10);
            temp32_str[5] = 'C';

            for (i = 0; i < 8; i++) {
                lm60.lines[1][i] = temp32_str[i];
            }

            lm60.value = pu32_out;

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
            convert_uint32_t_to_str(u8_lo, temp32_str + 3, 8, 10);
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

            _1631.value = u8_hi;

            ESOS_TASK_WAIT_TICKS(750); // The Temp sensor can only poll at 750ms with 12 bit mode
            continue;
        }
        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

void user_init()
{
    // This is all called in esos_menu_init
    // config_esos_uiF14();
    // esos_lcd44780_configDisplay();
    // esos_lcd44780_init();
    esos_menu_init();
    esos_pic24_configI2C1(400);
    configSPI1();
    esos_RegisterTask(lcd_menu);
    esos_RegisterTask(set_led);
    esos_RegisterTask(update_lm60);
    esos_RegisterTask(update_ds1631);
    ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4, ESOS_USER_IRQ_LEVEL2, _T4Interrupt);

    // Ensure that Timer2,3 configured as separate timers.
    T4CONbits.T32 = 0; // 32-bit mode off
                       // T3CON set like this for documentation purposes.
    T4CON = T4_PS_1_8 | T4_SOURCE_INT;
    // Subtract 1 from ticks value assigned to PR3 because period is PRx + 1.
    TMR4 = 0;
    PR4 = 1000;
    // Start with a cleared timer2 value.
    TMR4 = 0;
    // Enable Timer3 interrupts.
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T4);
    // Start the timer only after all timer-related configuration is complete.
    T4CONbits.TON = 1;

    ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4);
}
