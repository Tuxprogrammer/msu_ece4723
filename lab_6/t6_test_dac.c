/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t6_program_eeprom.c - Contains EEPROM preprogrammed values and programming code
 */

#include "revF14.h"
#include "esos.h"
#include "esos_f14ui.h"
#include "esos_at24c02d.h"
#include "esos_pic24.h"
#include "esos_pic24_spi.h"

ESOS_TASK_HANDLE dac_hdl;

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

ESOS_CHILD_TASK(_WRITE_DAC_AB, uint16_t u16_data)
{
    ESOS_TASK_BEGIN();

    // WRITE COMMAND
    // 0b<a/b><buf><GA><SHDN><d11:0>
    // 0b0011 | 16-bit number

    u16_data = 0x3000 | (u16_data >> 4);

    /*
    ESOS_TASK_WAIT_ON_SEND_STRING("_WRITE_DAC_AB: ");
    ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_data);
    ESOS_TASK_WAIT_ON_SEND_STRING("\n");
    */
    ESOS_TASK_WAIT_ON_AVAILABLE_SPI();
    SLAVE_ENABLE();
    // ESOS_TASK_WAIT_ON_WRITE1SPI1(u16_data);
    writeSPI(&u16_data, NULLPTR, 1);
    ESOS_TASK_WAIT_TICKS(1);
    SLAVE_DISABLE();
    ESOS_TASK_SIGNAL_AVAILABLE_SPI();

    ESOS_TASK_END();
}

ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(test_dac)
{
    ESOS_TASK_BEGIN();
    ESOS_ALLOCATE_CHILD_TASK(dac_hdl);
    static uint32_t i;

    while (TRUE) {

        for (i = 0x0000; i < 0xFF00; i += 0xF00) {
            ESOS_TASK_SPAWN_AND_WAIT(dac_hdl, _WRITE_DAC_AB, i);
            ESOS_TASK_YIELD();
        }

        for (i = 0xFF00; i >= 0x0F00; i -= 0xF00) {
            ESOS_TASK_SPAWN_AND_WAIT(dac_hdl, _WRITE_DAC_AB, i);
            ESOS_TASK_YIELD();
        }
    }

    ESOS_TASK_YIELD();
    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();
    esos_pic24_configI2C1(400);
    configSPI1();
    esos_RegisterTimer(heartbeat_LED, 500);
    esos_RegisterTask(test_dac);
}