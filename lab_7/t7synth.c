/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t7synth.c - app for wave generator and temperature sensor with menu
 */

#include "revF14.h"
#include "fall17lib.h"
#include "embedded_lab_CANLab.h"
#include "esos_ecan.h"
#include "esos_menu.h"
#include "esos_f14ui.h"
#include "esos_sensor.h"
#include "esos_pic24.h"
#include "esos_pic24_ecan.h"
#include "esos_pic24_sensor.h"
#include "esos_pic24_spi.h"
#include "esos_pic24_i2c.h"
#include "esos_pic24_irq.h"
#include "esos_at24c02d.h"
#include "pic24_timer.h"

static BOOL b_updateLM60;
static BOOL b_updateDS1631;

static uint16_t wvform_data[128];

#define SINE_WVFORM_ADDR 0
#define USER_WVFORM_ADDR 128

#define TRI_WVFORM 0
#define SQUARE_WVFORM 1
#define SINE_WVFORM 2
#define USER_WVFORM 3

// #define ECAN_BEACON_INTERVAL 30000
// #define ECAN_CLEAN_INTERVAL 120000
#define ECAN_BEACON_INTERVAL 3000
#define ECAN_CLEAN_INTERVAL 10000

#define CONFIG_FCNSYN_TIMER()                                                                                          \
    {                                                                                                                  \
        T4CONbits.T32 = 0;                                                                                             \
        T4CON = T4_PS_1_8 | T4_SOURCE_INT;                                                                             \
        TMR4 = 0;                                                                                                      \
        PR4 = FCY / 8 / 128 / freq.entries[0].value;                                                                   \
        TMR4 = 0;                                                                                                      \
        T4CONbits.TON = 1;                                                                                             \
    }

typedef struct {
    CAN_ID can_id;
    uint32_t tick;
    uint16_t temp;
} network_member;

network_member network[NUM_OF_IDS] = { 0 };

/*TODO: Ctrl+F replace all instances of mm with a name that abides
        by the coding standards.
*/
static esos_menu_longmenu_t main_menu = {
    .u8_numitems = 9,
    .u8_choice = 0, // Default
    .ast_items =
        {
            { "Set", "wvform", 0 },
            { "Set", "freq", 0 },
            { "Set", "ampltd", 0 },
            { "Set", "duty", 1 },
            { "Read", "LM60", 0 },
            { "Read", "1631", 0 },
            { "Set", "LEDs", 0 },
            { "", "About...", 0 },
            { "Browse", "Network", 0 },
        },
};

// static esos_menu_longmenu_item_t network_id_menu_list[NUM_OF_IDS];
static esos_menu_longmenu_t network_menu = {
    .u8_numitems = NUM_OF_IDS,
    .u8_choice = MY_ID,
    .ast_items =
        {
            { "cbb330", "1", 1 },  { "sc2257", "1", 0 }, { "lec426", "1", 1 }, { "woc17", "1", 1 },
            { "jdf469", "1", 1 },  { "jtn136", "2", 1 }, { "nrs171", "2", 1 }, { "igh9", "2", 1 },
            { "law448", "2", 1 },  { "rkh134", "2", 1 }, { "gs656", "3", 1 },  { "lrh282", "3", 1 },
            { "reo74", "3", 1 },   { "bmf151", "3", 1 }, { "rfj18", "3", 1 },  { "dc2274", "4", 1 },
            { "mam1218", "4", 1 }, { "mf1413", "4", 1 }, { "bcw253", "4", 1 }, { "jmp784", "4", 1 },
            { "bcj162", "7", 1 },  { "Vuk", "7", 1 },
        },
};

void initialize_network_menu_list()
{
    uint8_t u8_i, u8_j;
    for (u8_i = 0; u8_i < NUM_OF_IDS; u8_i++) {
        for (u8_j = 0; u8_j < 8; u8_j++) {
            network_menu.ast_items[u8_i].ac_line1[u8_j] = aCANID_IDs[u8_i].psz_netID[u8_j];
            if (aCANID_IDs[u8_i].psz_netID[u8_j] == '\0')
                break;
        }
        // utoa(aCANID""_IDs[u8_i].u8_teamID, network_menu.ast_items[u8_i].ac_line2, 10);
        network_menu.ast_items[u8_i].ac_line2[0] = 'h';
        network_menu.ast_items[u8_i].ac_line2[1] = 'e';
        network_menu.ast_items[u8_i].ac_line2[2] = 'l';
        network_menu.ast_items[u8_i].ac_line2[3] = 'p';
        network_menu.ast_items[u8_i].ac_line2[4] = 'p';
        network_menu.ast_items[u8_i].ac_line2[5] = 'l';
        network_menu.ast_items[u8_i].ac_line2[6] = 's';
        network_menu.ast_items[u8_i].ac_line2[7] = '\0';
        network_menu.ast_items[u8_i].b_hidden = u8_i != MY_ID;
        // network_menu.ast_items[u8_i] = network_id_menu_list[u8_i];
    }
}

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
    .lines = { { "team" }, { "gg" } },
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

    SLAVE_ENABLE();
    writeSPI(&u16_data, NULLPTR, 1);
    SLAVE_DISABLE();
}

static uint8_t u8_wvform_idx = 0;
ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_T4)
{
    write_DAC(wvform_data[u8_wvform_idx]);
    u8_wvform_idx = ++u8_wvform_idx % 128;
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T4);
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

ESOS_USER_TASK(lcd_menu)
{
    static ESOS_TASK_HANDLE update_hdl;
    ESOS_TASK_BEGIN();
    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice, duty.entries[0].value, ampl.entries[0].value);

    while (TRUE) {
        // Display main menu until the user presses SW3 to choose a selection

        // if square wave selected, show duty cycle
        if (wvform.u8_choice == 1) {
            main_menu.ast_items[3].b_hidden = FALSE;
        } else {
            main_menu.ast_items[3].b_hidden = TRUE;
        }

        ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(main_menu);
        if (main_menu.u8_choice == 0) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(wvform);
            ESOS_ALLOCATE_CHILD_TASK(update_hdl);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice, duty.entries[0].value,
                                     ampl.entries[0].value);

        } else if (main_menu.u8_choice == 1) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(freq);
            PR4 = FCY / 8 / 128 / freq.entries[0].value;
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(PR4);
            ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
        } else if (main_menu.u8_choice == 2) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(ampl);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice, duty.entries[0].value,
                                     ampl.entries[0].value);
        } else if (main_menu.u8_choice == 3) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(duty);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, wvform.u8_choice, duty.entries[0].value,
                                     ampl.entries[0].value);
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
        } else if (main_menu.u8_choice == 8) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(network_menu);
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
            if (temp32_fpart >= 0 && temp32_fpart <= 9) {
                temp32_str[4] = temp32_str[3];
                temp32_str[3] = '0';
            }

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

            _1631.value = u8_hi;

            ESOS_TASK_WAIT_TICKS(750); // The Temp sensor can only poll at 750ms with 12 bit mode
            continue;
        }
        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_receiver)
{
    static uint8_t buf[2] = { 0 };
    static uint8_t u8_len;
    static uint16_t u16_can_id;
    static uint8_t u8_msg_type;
    static uint8_t u8_team_ID;
    static uint8_t u8_member_ID;
    static MAILMESSAGE msg;

    ESOS_TASK_BEGIN();

    esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_BEACON, 0x0001, MASKCONTROL_FIELD_NONZERO);

    while (TRUE) {
        ESOS_TASK_WAIT_FOR_MAIL();
        ESOS_TASK_GET_NEXT_MESSAGE(&msg);
        u16_can_id = msg.au16_Contents[0];
        u8_team_ID = stripTeamID(u16_can_id);
        u8_msg_type = stripTypeID(u16_can_id);
        u8_member_ID = stripMemberID(u16_can_id);
        u8_len = ESOS_GET_PMSG_DATA_LENGTH((&msg)) - sizeof(uint16_t);
        memcpy(buf, &msg.au8_Contents[sizeof(uint16_t)], u8_len);

        if (u8_msg_type == CANMSG_TYPE_BEACON) {
            int8_t i8_i = getArrayIndexFromMsgID(u16_can_id);
            if (i8_i >= 0) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                if (network[i8_i].tick == 0) {
                    ESOS_TASK_WAIT_ON_SEND_STRING("Adding board: ");
                    network_menu.ast_items[i8_i].b_hidden = FALSE;
                } else {
                    ESOS_TASK_WAIT_ON_SEND_STRING("Refreshing board: ");
                }
                ESOS_TASK_WAIT_ON_SEND_STRING(network[i8_i].can_id.psz_name);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                network[i8_i].tick = esos_GetSystemTick();
            }
        }

        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_beacon_network)
{
    ESOS_TASK_BEGIN();

    while (TRUE) {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("BEACONING\n");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_BEACON), 0, 0);
        // ESOS_ECAN_SEND(0x7A0, 0, 0);
        ESOS_TASK_WAIT_TICKS(ECAN_BEACON_INTERVAL);
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_clean_network)
{
    ESOS_TASK_BEGIN();

    while (TRUE) {
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("Cleaned network\n");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        // remove network members with currentTick - tick > ECAN_CLEAN_INTERVAL
        static uint32_t u32_curr_tick;
        static uint8_t u8_i;
        for (u8_i = 0; u8_i < NUM_OF_IDS; u8_i++) {
            u32_curr_tick = esos_GetSystemTick();
            // skip if this board or other has board never been online
            if (u8_i == MY_ID || network[u8_i].tick == 0) {
                continue;
            } else if (u32_curr_tick - network[u8_i].tick > ECAN_CLEAN_INTERVAL) {
                network[u8_i].tick = 0;
                network_menu.ast_items[u8_i].b_hidden = TRUE;
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Removing ");
                ESOS_TASK_WAIT_ON_SEND_STRING(network[u8_i].can_id.psz_name);
                ESOS_TASK_WAIT_ON_SEND_STRING("'s board from network.\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            }
            ESOS_TASK_YIELD();
        }
        ESOS_TASK_WAIT_TICKS(1000);
    }

    ESOS_TASK_END();
}

void initialize_network()
{
    uint8_t u8_i;
    for (u8_i = 0; u8_i < NUM_OF_IDS; u8_i++) {
        network[u8_i].can_id = aCANID_IDs[u8_i];
        if (u8_i == MY_ID) {
            network[u8_i].tick = esos_GetSystemTick();
        } else {
            network[u8_i].tick = 0;
        }
        network[u8_i].temp = 0;
    }
}

void user_init()
{
    __esos_unsafe_PutString(HELLO_MSG);

    // This is all called in esos_menu_init
    // config_esos_uiF14();
    // esos_lcd44780_configDisplay();
    // esos_lcd44780_init();
    esos_menu_init();
    esos_pic24_configI2C1(400);
    configSPI1();
    __esos_ecan_hw_config_ecan();
    initialize_network();
    // initialize_network_menu_list();
    network_menu.ast_items[MY_ID].b_hidden = 0;

    esos_RegisterTask(lcd_menu);
    esos_RegisterTask(set_led);
    esos_RegisterTask(update_lm60);
    esos_RegisterTask(update_ds1631);
    esos_RegisterTask(CANFactory);
    esos_RegisterTask(ecan_receiver);
    esos_RegisterTask(ecan_beacon_network);
    esos_RegisterTask(ecan_clean_network);
    CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);

    ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4, ESOS_USER_IRQ_LEVEL2, _T4Interrupt);
    CONFIG_FCNSYN_TIMER();
    ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4);
}
