/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t8synth.c - app for wave generator and temperature sensor with menu
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
static BOOL b_requestLM60;
static BOOL b_updateDS1631;
static BOOL b_requestDS1631;

static uint16_t wvform_dataA[128];
static uint16_t wvform_dataB[128];

#define SINE_WVFORM_ADDR 0
#define USER_WVFORM_ADDR 128

#define TRI_WVFORM 0
#define SQUARE_WVFORM 1
#define SINE_WVFORM 2
#define USER_WVFORM 3

#define MENU_TYPE_SET_WAVEFORM 1
#define MENU_TYPE_SET_FREQ 2
#define MENU_TYPE_SET_AMPLTD 3
#define MENU_TYPE_SET_DUTY 4
#define MENU_TYPE_READ_LM60 5
#define MENU_TYPE_READ_1631 6
#define MENU_TYPE_SET_LEDS 7
#define MENU_TYPE_ABOUT 8
#define MENU_TYPE_SET_BOARD 0

// #define ECAN_BEACON_INTERVAL 60000
// #define ECAN_BEACON_TEMP_INTERVAL 30000
// #define ECAN_CLEAN_INTERVAL 120000

#define ECAN_BEACON_INTERVAL 6000
#define ECAN_BEACON_TEMP_INTERVAL 300000
#define ECAN_CLEAN_INTERVAL 10000

#define TEMP_REQUEST_INTERVAL 1000

#define CONFIG_FCNSYN_TIMER()                                                                                          \
    {                                                                                                                  \
        T4CONbits.T32 = 0;                                                                                             \
        T4CON = T4_PS_1_8 | T4_SOURCE_INT;                                                                             \
        TMR4 = 0;                                                                                                      \
        PR4 = FCY / 8 / 128 / freq.entries[0].value;                                                                   \
        TMR4 = 0;                                                                                                      \
        T4CONbits.TON = 1;                                                                                             \
        T5CON = T5_PS_1_8 | T5_SOURCE_INT;                                                                             \
        TMR5 = 0;                                                                                                      \
        PR5 = FCY / 8 / 128 / freq.entries[0].value;                                                                   \
        TMR5 = 0;                                                                                                      \
        T5CONbits.TON = 1;                                                                                             \
    }

typedef struct {
    CAN_ID can_id;
    uint32_t tick;
    int16_t temp_lm60;
    int16_t temp_1631;
    uint8_t wvform;
    uint8_t potentiometer;
    uint16_t freq;
    uint8_t ampl;
} network_member;

static network_member network[NUM_OF_IDS] = { 0 };

/*TODO: Ctrl+F replace all instances of mm with a name that abides
        by the coding standards.
*/
static esos_menu_longmenu_t main_menu = {
    .u8_numitems = 9,
    .u8_choice = 0, // Default
    .ast_items = { { "Set", "Board", 0 },
                   { "   Set", "wvform", 0 },
                   { "   Set", "freq", 0 },
                   { "   Set", "ampltd", 0 },
                   { "   Set", "duty", 1 },
                   { "   Read", "LM60", 0 },
                   { "   Read", "1631", 0 },
                   { "   Set", "LEDs", 0 },
                   { "", "About...", 0 } },
};

// static esos_menu_longmenu_item_t network_id_menu_list[NUM_OF_IDS];
static esos_menu_longmenu_t network_menu = {
    .u8_numitems = NUM_OF_IDS,
    .u8_choice = MY_ID,
    .ast_items =
        {
            { "cbb330", "1", 1 },  { "sc2257", "1", 1 }, { "lec426", "1", 1 }, { "woc17", "1", 1 },
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
    .max = 3200,
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

void write_DAC(uint16_t u16_data, uint8_t u8_dacAB)
{
    // WRITE COMMAND
    // 0b<a/b><buf><GA><SHDN><d11:0>
    // 0b0011 | 16-bit number

    u16_data = ((u8_dacAB == 1) ? 0xB000 : 0x3000) | (u16_data >> 4);

    SLAVE_ENABLE();
    writeSPI(&u16_data, NULLPTR, 1);
    SLAVE_DISABLE();
}

char *i7point8toa(uint8_t buffer[2], char result[8], BOOL decimal_fpart)
{
    convert_uint32_t_to_str(buffer[0], result, 8, 10);
    result[2] = '.';
    if (!decimal_fpart)
        buffer[1] = buffer[1] * 100 / 256;
    convert_uint32_t_to_str(buffer[1], result + 3, 5, 10);
    if (buffer[1] >= 0 && buffer[1] <= 9) {
        result[4] = result[3];
        result[3] = '0';
    }
    return result;
}

// Update DACs
static uint8_t u8_wvformA_idx = 0;
ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_T4)
{
    write_DAC(wvform_dataA[u8_wvformA_idx], 0);
    u8_wvformA_idx = ++u8_wvformA_idx % 128;
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T4);
}

static uint8_t u8_wvformB_idx = 0;
ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_T5)
{
    write_DAC(wvform_dataB[u8_wvformB_idx], 1);
    u8_wvformB_idx = ++u8_wvformB_idx % 128;
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T5);
}

// update the main menu strings to reflect the board choice
ESOS_CHILD_TASK(update_board_choice, esos_menu_longmenu_t *ps_menu, uint8_t u8_choice)
{
    ESOS_TASK_BEGIN();
    // 2 index max for the menu, 1 index for null terminator
    static char str_choice[3];
    itoa(GET_MENU_N(u8_choice), str_choice, 10);

    static uint8_t u8_i;
    for (u8_i = 1; u8_i < ps_menu->u8_numitems - 1; u8_i++) {
        ps_menu->ast_items[u8_i].ac_line1[0] = str_choice[0];
        if (GET_MENU_N(u8_choice) < 10) {
            ps_menu->ast_items[u8_i].ac_line1[1] = ' ';
        } else {
            ps_menu->ast_items[u8_i].ac_line1[1] = str_choice[1];
        }
    }
    ESOS_TASK_END();
}

ESOS_CHILD_TASK(update_wvform, uint8_t u8_dacAB, uint8_t u8_type, uint8_t u8_duty, uint8_t u8_ampl)
{
    ESOS_TASK_BEGIN();

    static uint16_t *wvform_data;

    static uint8_t u16_addr;
    static uint16_t u16_scaledData;
    static uint8_t u8_rawData;
    static uint16_t i;

    static uint8_t u8_currAmplitude;
    u8_currAmplitude = u8_ampl * UINT8_MAX / 30;

    wvform_data = (u8_dacAB == 1) ? wvform_dataB : wvform_dataA;

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
    static uint8_t u8_selected_board_ID = MY_ID;
    static ESOS_TASK_HANDLE update_hdl;
    ESOS_TASK_BEGIN();
    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 0, wvform.u8_choice, duty.entries[0].value,
                             ampl.entries[0].value);
    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 1, wvform.u8_choice, duty.entries[0].value,
                             ampl.entries[0].value);
    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_board_choice, &main_menu, network_menu.u8_choice);

    while (TRUE) {
        // Display main menu until the user presses SW3 to choose a selection
        u8_selected_board_ID = network_menu.u8_choice;

        // if square wave selected, show duty cycle
        if (wvform.u8_choice == 1) {
            main_menu.ast_items[MENU_TYPE_SET_DUTY].b_hidden = FALSE;
        } else {
            main_menu.ast_items[MENU_TYPE_SET_DUTY].b_hidden = TRUE;
        }

        ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(main_menu);
        if (main_menu.u8_choice == MENU_TYPE_SET_WAVEFORM) {
            static uint8_t tempval;
            tempval = wvform.u8_choice;
            wvform.u8_choice = network[u8_selected_board_ID].wvform; // read waveform we have stored
            if (u8_selected_board_ID != MY_ID) {
                wvform.ast_items[3].b_hidden = 1;
            }
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(wvform); // display waveform menu with above setting as default
            if (u8_selected_board_ID != MY_ID) {
                wvform.ast_items[3].b_hidden = 0;
            }
            network[u8_selected_board_ID].wvform = wvform.u8_choice;
            if (u8_selected_board_ID == MY_ID) {
                ESOS_ALLOCATE_CHILD_TASK(update_hdl);
                ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 0, wvform.u8_choice, duty.entries[0].value,
                                         ampl.entries[0].value);
            } else {
                ESOS_ALLOCATE_CHILD_TASK(update_hdl);
                ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 1, network[u8_selected_board_ID].wvform, 50,
                                         network[u8_selected_board_ID].ampl);
                wvform.u8_choice = tempval;
            }

            static uint8_t buf[1];
            buf[0] = network[u8_selected_board_ID].wvform;
            ESOS_ECAN_SEND(CANMSG_TYPE_WAVEFORM | calcMsgID(u8_selected_board_ID), buf, 1);
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("sending wvform: ");
            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(buf[0]);
            ESOS_TASK_WAIT_ON_SEND_STRING(" to board: ");
            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u8_selected_board_ID);
            ESOS_TASK_WAIT_ON_SEND_STRING("\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        } else if (main_menu.u8_choice == MENU_TYPE_SET_FREQ) {
            // ESOS_TASK_WAIT_ESOS_MENU_ENTRY(freq);
            // PR4 = FCY / 8 / 128 / freq.entries[0].value;
            // ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(PR4);
            // ESOS_TASK_WAIT_ON_SEND_UINT8('\n');

            static int16_t tempval;
            tempval = freq.entries[0].value;
            freq.entries[0].value = network[u8_selected_board_ID].freq; // read waveform we have stored
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(freq); // display waveform menu with above setting as default

            network[u8_selected_board_ID].freq = freq.entries[0].value; // set new waveform selection to local storage
            if (u8_selected_board_ID == MY_ID) {
                // if message targets our board, store data in freq menu and update daca
                PR4 = FCY / 8 / 128 / freq.entries[0].value;
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Setting PR4: ");
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(PR4);
                ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            } else {
                PR5 = FCY / 8 / 128 / freq.entries[0].value;
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Setting PR5: ");
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(PR5);
                ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                freq.entries[0].value = tempval;
            }
            static uint8_t buf[2];
            static uint16_t u16_freq;
            u16_freq = network[u8_selected_board_ID].freq;
            buf[0] = (uint8_t)(u16_freq >> 8);
            buf[1] = (uint8_t)(u16_freq & 0xFF);

            ESOS_ECAN_SEND(CANMSG_TYPE_FREQUENCY | calcMsgID(u8_selected_board_ID), buf, 2);
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("sending freq: ");
            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[0]);
            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[1]);
            ESOS_TASK_WAIT_ON_SEND_STRING(" to board: ");
            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u8_selected_board_ID);
            ESOS_TASK_WAIT_ON_SEND_STRING("\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        } else if (main_menu.u8_choice == MENU_TYPE_SET_AMPLTD) {
            static uint8_t tempval;
            tempval = ampl.entries[0].value;
            ampl.entries[0].value = network[u8_selected_board_ID].ampl; // read waveform we have stored

            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(ampl); // display waveform menu with above setting as default

            network[u8_selected_board_ID].ampl = ampl.entries[0].value;
            if (u8_selected_board_ID == MY_ID) {
                ESOS_ALLOCATE_CHILD_TASK(update_hdl);
                ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 0, wvform.u8_choice, duty.entries[0].value,
                                         ampl.entries[0].value);
            } else {
                ESOS_ALLOCATE_CHILD_TASK(update_hdl);
                ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 1, network[u8_selected_board_ID].wvform, 50,
                                         network[u8_selected_board_ID].ampl);
                ampl.entries[0].value = tempval;
            }

            static uint8_t buf[1];
            buf[0] = network[u8_selected_board_ID].ampl;
            buf[0] = dtou3p5(buf[0] / 10, buf[0] % 10);
            ESOS_ECAN_SEND(CANMSG_TYPE_AMPLITUDE | calcMsgID(u8_selected_board_ID), buf, 1);

            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("sending ampl: ");
            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[0]);
            ESOS_TASK_WAIT_ON_SEND_STRING(" to board: ");
            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u8_selected_board_ID);
            ESOS_TASK_WAIT_ON_SEND_STRING("\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        } else if (main_menu.u8_choice == MENU_TYPE_SET_DUTY) {
            ESOS_TASK_WAIT_ESOS_MENU_ENTRY(duty);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 0, wvform.u8_choice, duty.entries[0].value,
                                     ampl.entries[0].value);
        } else if (main_menu.u8_choice == MENU_TYPE_READ_LM60) {
            if (u8_selected_board_ID == MY_ID)
                b_updateLM60 = 1;
            else
                b_requestLM60 = 1;
            // TODO: propagate updates to lm60 menu item...
            ESOS_TASK_WAIT_ESOS_MENU_SLIDERBAR(lm60);
            b_updateLM60 = 0;
            b_requestLM60 = 0;
        } else if (main_menu.u8_choice == MENU_TYPE_READ_1631) {
            if (u8_selected_board_ID == MY_ID)
                b_updateDS1631 = 1;
            else
                b_requestDS1631 = 1;
            ESOS_TASK_WAIT_ESOS_MENU_SLIDERBAR(_1631);
            b_updateDS1631 = 0;
            b_requestDS1631 = 0;
        } else if (main_menu.u8_choice == MENU_TYPE_SET_LEDS) {
            if (u8_selected_board_ID == MY_ID) {
                // set boolean to update led display
                ESOS_TASK_WAIT_ESOS_MENU_ENTRY(leds);
            } else {
                static uint32_t tempval;
                tempval = leds.entries[0].value;
                ESOS_TASK_WAIT_ESOS_MENU_ENTRY(leds);
                static uint8_t buf[1];
                buf[0] = leds.entries[0].value;
                ESOS_ECAN_SEND(CANMSG_TYPE_LEDS | calcMsgID(u8_selected_board_ID), buf, 1);
                leds.entries[0].value = tempval;
            }
        } else if (main_menu.u8_choice == MENU_TYPE_ABOUT) {
            ESOS_TASK_WAIT_ESOS_MENU_STATICMENU(about);
        } else if (main_menu.u8_choice == MENU_TYPE_SET_BOARD) {
            ESOS_TASK_WAIT_ESOS_MENU_LONGMENU(network_menu);
            u8_selected_board_ID = network_menu.u8_choice;
            ESOS_ALLOCATE_CHILD_TASK(update_hdl);
            ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_board_choice, &main_menu, u8_selected_board_ID);
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
            ESOS_TASK_WAIT_ON_SEND_STRING("Choosing board ");
            ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u8_selected_board_ID);
            ESOS_TASK_WAIT_ON_SEND_STRING(" and grabbing its data");
            ESOS_TASK_WAIT_ON_SEND_STRING("\n");
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            ESOS_ECAN_SEND(CANMSG_TYPE_WAVEFORM | calcMsgID(u8_selected_board_ID), 0, 0);
            ESOS_TASK_WAIT_TICKS(1);
            ESOS_ECAN_SEND(CANMSG_TYPE_FREQUENCY | calcMsgID(u8_selected_board_ID), 0, 0);
            ESOS_TASK_WAIT_TICKS(1);
            ESOS_ECAN_SEND(CANMSG_TYPE_AMPLITUDE | calcMsgID(u8_selected_board_ID), 0, 0);
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

            uint8_t buf[2] = { temp32_ipart, temp32_fpart };
            i7point8toa(buf, temp32_str, 1);
            temp32_str[5] = 'C';

            for (i = 0; i < 8; i++) {
                lm60.lines[1][i] = temp32_str[i];
            }

            network[MY_ID].temp_lm60 = (int16_t)u32_out;
            lm60.value = u32_out;
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

            uint8_t buf[2] = { u8_hi, u8_lo };
            i7point8toa(buf, temp32_str, 0);
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

ESOS_USER_TASK(request_temp)
{
    static uint32_t u32_temp_request_tick = 0;
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if ((b_requestLM60 || b_requestDS1631) && network_menu.u8_choice != MY_ID &&
            esos_GetSystemTick() - u32_temp_request_tick > TEMP_REQUEST_INTERVAL) {
            static uint16_t u16_request;
            if (b_requestLM60) {
                u16_request = CANMSG_TYPE_TEMPERATURE1 | calcMsgID(network_menu.u8_choice);
                // ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                // ESOS_TASK_WAIT_ON_SEND_STRING("Requesting LM60: ");
                // ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_request);
                // ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                // ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            } else if (b_requestDS1631) {
                u16_request = CANMSG_TYPE_TEMPERATURE2 | calcMsgID(network_menu.u8_choice);
                // ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                // ESOS_TASK_WAIT_ON_SEND_STRING("Requesting DS1631: ");
                // ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_request);
                // ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                // ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            }

            ESOS_ECAN_SEND(u16_request, 0, 0);
            u32_temp_request_tick = esos_GetSystemTick();
            ESOS_TASK_WAIT_TICKS(TEMP_REQUEST_INTERVAL);
        }
        ESOS_TASK_YIELD();
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_receiver)
{
    static uint8_t buf[2] = { 0 };
    static uint8_t u8_buf_len;
    static uint16_t u16_can_id;
    static uint8_t u8_msg_type;
    static uint8_t u8_team_ID;
    static uint8_t u8_member_ID;
    static int8_t i8_i;
    static MAILMESSAGE msg;

    static ESOS_TASK_HANDLE read_temp;
    static ESOS_TASK_HANDLE update_hdl;
    static uint16_t u16_out;
    static uint32_t u32_out;
    static int16_t i16_temp;
    static uint16_t u16_freq;

    ESOS_TASK_BEGIN();

    // esos_ecan_canfactory_subscribe(__pstSelf, MY_MSG_ID(CANMSG_TYPE_BEACON), 0x0000, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_BEACON, 0x0001, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_TEMPERATURE1, 0x0002, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_TEMPERATURE2, 0x0003, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_WAVEFORM, 0x0004, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_POTENTIOMETER, 0x0005, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_FREQUENCY, 0x0006, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_AMPLITUDE, 0x0007, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_DUTYCYCLE, 0x0008, MASKCONTROL_FIELD_NONZERO);
    // esos_ecan_canfactory_subscribe(__pstSelf, CANMSG_TYPE_LEDS, 0x0009, MASKCONTROL_FIELD_NONZERO);

    esos_ecan_canfactory_subscribe(__pstSelf, NULL, 0x0000, MASKCONTROL_FIELD_NONZERO);

    while (TRUE) {
        ESOS_TASK_WAIT_FOR_MAIL();
        ESOS_TASK_GET_NEXT_MESSAGE(&msg);
        u16_can_id = msg.au16_Contents[0];
        u8_team_ID = stripTeamID(u16_can_id);
        u8_msg_type = stripTypeID(u16_can_id);
        u8_member_ID = stripMemberID(u16_can_id);
        int8_t i8_i = getArrayIndexFromMsgID(u16_can_id);
        u8_buf_len = ESOS_GET_PMSG_DATA_LENGTH((&msg)) - sizeof(uint16_t);
        memcpy(buf, &msg.au8_Contents[sizeof(uint16_t)], u8_buf_len);

        // ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        // ESOS_TASK_WAIT_ON_SEND_STRING("MSG POSTMARK: ");
        // ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(msg.u32_Postmark);
        // ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
        // ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        if (u8_msg_type == CANMSG_TYPE_BEACON) {
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
        } else if (u8_msg_type == CANMSG_TYPE_TEMPERATURE1) {
            if (u8_buf_len == 2) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Storing LM60 data from another board\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                i16_temp = buf[0] << 8 | buf[1];
                network[i8_i].temp_lm60 = i16_temp;
                if (i8_i == network_menu.u8_choice && b_requestLM60) {
                    static char temp[12] = { 0 };
                    i7point8toa(buf, temp, 0);
                    temp[5] = 'C';

                    uint8_t u8_index = 0;
                    for (u8_index = 0; u8_index < 8; u8_index++) {
                        lm60.lines[1][u8_index] = temp[u8_index];
                    }

                    lm60.value = buf[0] * 100 + buf[1];
                }
            } else if (u8_buf_len == 0 && i8_i == MY_ID) {
                ESOS_ALLOCATE_CHILD_TASK(read_temp);
                ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_ON_AVAILABLE_SENSOR, TEMP_CHANNEL, ESOS_SENSOR_VREF_3V0);
                ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_SENSOR_READ, &u16_out, ESOS_SENSOR_ONE_SHOT,
                                         ESOS_SENSOR_FORMAT_VOLTAGE);
                ESOS_SENSOR_CLOSE();

                u32_out = (uint32_t)u16_out * 1000; // convert to not use decimals
                // u32_out = (u32_out - 424000) / 625; // millimillivolts to temp
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("sending temp: ");
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u32_out);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                u32_out = (u32_out - 424000) / 625; // millimillivolts to temp
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u32_out);
                int8_t ipart = u32_out / 100;
                uint16_t fpart = u32_out - ipart * 100;
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(ipart);
                ESOS_TASK_WAIT_ON_SEND_STRING(".");
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(fpart);
                fpart = fpart * 0xFF;
                fpart = fpart / 100;
                uint8_t newfpart = (uint8_t)fpart;
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_WAIT_ON_SEND_STRING("newfpart: ");
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(newfpart);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                network[MY_ID].temp_lm60 = (int16_t)((ipart << 8) | newfpart);

                buf[0] = network[MY_ID].temp_lm60 >> 8;
                buf[1] = network[MY_ID].temp_lm60;
                ESOS_ECAN_SEND(CANMSG_TYPE_TEMPERATURE1 | calcMsgID(MY_ID), buf, 2);
            }
        } else if (u8_msg_type == CANMSG_TYPE_TEMPERATURE2) {
            if (u8_buf_len == 2) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Storing DS1631 data from another board\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                i16_temp = buf[0] << 8 | buf[1];
                network[i8_i].temp_1631 = i16_temp;
                if (i8_i == network_menu.u8_choice && b_requestDS1631) {
                    static char tmp[12] = { 0 };
                    convert_uint32_t_to_str(buf[0], tmp, 12, 10);
                    tmp[2] = '.';
                    // buf[1] = buf[1] * 100 / 256;
                    convert_uint32_t_to_str(buf[1], tmp + 3, 8, 10);
                    if (buf[1] >= 0 && buf[1] <= 9) {
                        tmp[4] = tmp[3];
                        tmp[3] = '0';
                    }
                    tmp[5] = 'C';

                    uint8_t u8_index = 0;
                    for (u8_index = 0; u8_index < 8; u8_index++) {
                        _1631.lines[1][u8_index] = tmp[u8_index];
                    }

                    _1631.value = buf[0];
                }
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("new temp: ");
                ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint16_t)i16_temp);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

            } else if (u8_buf_len == 0 && i8_i == MY_ID) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Responding to DS1631 data request\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
                ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, 0xAA); // Send READ command
                ESOS_TASK_WAIT_ON_READ2I2C1(DS1631ADDR, buf[0], buf[1]);
                ESOS_TASK_SIGNAL_AVAILABLE_I2C();

                static char temp[9] = { 0 };
                i7point8toa(buf, temp, 0);
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("sending temp: ");
                ESOS_TASK_WAIT_ON_SEND_STRING(temp);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                ESOS_ECAN_SEND(CANMSG_TYPE_TEMPERATURE2 | calcMsgID(MY_ID), buf, 2);
            }
        } else if (u8_msg_type == CANMSG_TYPE_WAVEFORM) {
            if (u8_buf_len == 1 && buf[0] <= 2) { // message contains data, so store it and update accordingly
                network[i8_i].wvform = buf[0]; // set new waveform selection to local storage
                if (i8_i == MY_ID) {
                    // if message targets our board, store data in wvform menu and update daca
                    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                    ESOS_TASK_WAIT_ON_SEND_STRING("updating my wvform: ");
                    // ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(buf[0]);
                    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(network[i8_i].wvform);
                    ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                    wvform.u8_choice = buf[0];
                    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
                    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 0, wvform.u8_choice, duty.entries[0].value,
                                             ampl.entries[0].value);
                } else if (i8_i == network_menu.u8_choice) {
                    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                    ESOS_TASK_WAIT_ON_SEND_STRING("updating wvform: ");
                    // ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(buf[0]);
                    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(network[i8_i].wvform);
                    ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                    // if message is updating the board that this board is subscribed to, update dacb
                    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
                    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 1, network[i8_i].wvform, 50,
                                             network[i8_i].ampl);
                }
            } else if (u8_buf_len == 0 && i8_i == MY_ID) { // if message is a request @ us
                buf[0] = network[MY_ID].wvform;
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("sending wvform: ");
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[0]);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                ESOS_ECAN_SEND(CANMSG_TYPE_WAVEFORM | calcMsgID(MY_ID), buf, 1);
            }
        } else if (u8_msg_type == CANMSG_TYPE_POTENTIOMETER) {
        } else if (u8_msg_type == CANMSG_TYPE_FREQUENCY) {
            if (u8_buf_len == 2) { // message contains data, so store it and update accordingly
                u16_freq = buf[0] << 8 | buf[1];
                network[i8_i].freq = u16_freq; // set new waveform selection to local storage
                if (i8_i == MY_ID) {
                    // if message targets our board, store data in freq menu and update daca
                    freq.entries[0].value = u16_freq;
                    PR4 = FCY / 8 / 128 / u16_freq;
                    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                    ESOS_TASK_WAIT_ON_SEND_STRING("PR4: ");
                    ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(PR4);
                    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                } else if (i8_i == network_menu.u8_choice) {
                    // if message is updating the board that this board is subscribed to, update dacb
                    PR5 = FCY / 8 / 128 / u16_freq;
                    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                    ESOS_TASK_WAIT_ON_SEND_STRING("PR5: ");
                    ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(PR5);
                    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
                    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                }
            } else if (u8_buf_len == 0 && i8_i == MY_ID) { // if message is a request @ us
                u16_freq = network[MY_ID].freq;
                buf[0] = (uint8_t)(u16_freq >> 8);
                buf[1] = (uint8_t)(u16_freq & 0xFF);

                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("sending freq: ");
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[0]);
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[1]);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                ESOS_ECAN_SEND(CANMSG_TYPE_FREQUENCY | calcMsgID(MY_ID), buf, 2);
            }
        } else if (u8_msg_type == CANMSG_TYPE_AMPLITUDE) {
            if (u8_buf_len == 1) { // message contains data, so store it and update accordingly
                buf[0] = u3p5toipart(buf[0]) * 10 + u3p5tofpart(buf[0]); // format from 3p5 to deci
                network[i8_i].ampl = buf[0]; // set new waveform selection to local storage
                if (i8_i == MY_ID) {
                    // if message targets our board, store data in wvform menu and update daca
                    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                    ESOS_TASK_WAIT_ON_SEND_STRING("updating my ampl: ");
                    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(network[i8_i].ampl);
                    ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                    ampl.entries[0].value = buf[0];
                    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
                    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 0, wvform.u8_choice, duty.entries[0].value,
                                             ampl.entries[0].value);
                } else if (i8_i == network_menu.u8_choice) {
                    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                    ESOS_TASK_WAIT_ON_SEND_STRING("updating ampl: ");
                    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(network[i8_i].ampl);
                    ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                    // if message is updating the board that this board is subscribed to, update dacb
                    ESOS_ALLOCATE_CHILD_TASK(update_hdl);
                    ESOS_TASK_SPAWN_AND_WAIT(update_hdl, update_wvform, 1, network[i8_i].wvform, 50,
                                             network[i8_i].ampl);
                }
            } else if (u8_buf_len == 0 && i8_i == MY_ID) { // if message is a request @ us
                buf[0] = network[MY_ID].ampl;
                buf[0] = dtou3p5(buf[0] / 10, buf[0] % 10); // format deci to 3p5
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("sending ampl: ");
                ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(buf[0]);
                ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

                ESOS_ECAN_SEND(CANMSG_TYPE_AMPLITUDE | calcMsgID(MY_ID), buf, 1);
            }
        } else if (u8_msg_type == CANMSG_TYPE_LEDS) {
            // 1 byte led display value
            if (u8_buf_len == 1 && i8_i == MY_ID) {
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("Settings LEDs to request\n");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
                leds.entries[0].value = buf[0];
                ESOS_TASK_WAIT_TICKS(1);
            }
        }
        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

// TODO: make discovering a board request its current waveform settings
ESOS_USER_TASK(ecan_beacon_network)
{
    ESOS_TASK_BEGIN();

    while (TRUE) {
        ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_BEACON), 0, 0);
        ESOS_TASK_WAIT_TICKS(ECAN_BEACON_INTERVAL);
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_beacon_temp)
{
    static uint32_t u32_out;
    static uint16_t u16_out;
    static uint8_t buf[2];
    static ESOS_TASK_HANDLE read_temp;

    ESOS_TASK_BEGIN();

    while (TRUE) {
        /* lm60 */
        ESOS_ALLOCATE_CHILD_TASK(read_temp);
        ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_ON_AVAILABLE_SENSOR, TEMP_CHANNEL, ESOS_SENSOR_VREF_3V0);
        ESOS_TASK_SPAWN_AND_WAIT(read_temp, _WAIT_SENSOR_READ, &u16_out, ESOS_SENSOR_ONE_SHOT,
                                 ESOS_SENSOR_FORMAT_VOLTAGE);
        ESOS_SENSOR_CLOSE();

        u32_out = (uint32_t)u16_out * 1000; // convert to not use decimals
        // u32_out = (u32_out - 424000) / 625; // millimillivolts to temp
        u32_out = (u32_out - 424000) / 625; // millimillivolts to temp
        static int8_t ipart;
        ipart = u32_out / 100;
        static uint16_t fpart;
        fpart = u32_out - ipart * 100;
        fpart = fpart * 0xFF;
        fpart = fpart / 100;
        static uint8_t newfpart;
        newfpart = (uint8_t)fpart;
        network[MY_ID].temp_lm60 = (int16_t)((ipart << 8) | newfpart);

        buf[0] = network[MY_ID].temp_lm60 >> 8;
        buf[1] = network[MY_ID].temp_lm60;

        ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_TEMPERATURE1), buf, 2);
        ESOS_TASK_WAIT_TICKS(ECAN_BEACON_TEMP_INTERVAL);
        /* ds1631 */
        ESOS_TASK_WAIT_ON_AVAILABLE_I2C();
        ESOS_TASK_WAIT_ON_WRITE1I2C1(DS1631ADDR, 0xAA); // Send READ command
        ESOS_TASK_WAIT_ON_READ2I2C1(DS1631ADDR, buf[0], buf[1]);
        ESOS_TASK_SIGNAL_AVAILABLE_I2C();
        ESOS_ECAN_SEND(MY_MSG_ID(CANMSG_TYPE_TEMPERATURE2), buf, 2);
        ESOS_TASK_WAIT_TICKS(ECAN_BEACON_TEMP_INTERVAL);
    }

    ESOS_TASK_END();
}

ESOS_USER_TASK(ecan_clean_network)
{
    ESOS_TASK_BEGIN();

    while (TRUE) {
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
        network[u8_i].temp_lm60 = 0;
        network[u8_i].temp_1631 = 0;
    }
    network[MY_ID].wvform = 0;
    network[MY_ID].freq = 1000;
    network[MY_ID].ampl = 10;
}

void user_init()
{
    __esos_unsafe_PutString(HELLO_MSG);
    // ENABLE_DEBUG_MODE();

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
    esos_RegisterTask(ecan_beacon_temp);
    esos_RegisterTask(ecan_clean_network);
    esos_RegisterTask(request_temp);
    CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);

    ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4, ESOS_USER_IRQ_LEVEL2, _T4Interrupt);
    ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T5, ESOS_USER_IRQ_LEVEL2, _T5Interrupt);
    CONFIG_FCNSYN_TIMER();
    ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4);
    ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T5);
}
