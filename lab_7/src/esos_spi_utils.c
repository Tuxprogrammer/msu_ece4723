#include "esos_spi_utils.h"

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