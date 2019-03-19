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

void configSPI1(void)
{
    // spi clock = 40MHz/1*4 = 40MHz/4 = 10MHz
    SPI1CON1 = PRI_PRESCAL_1_1 | // 1:1 Primary prescale
               SEC_PRESCAL_3_1 | // 3:1 Secondary prescale
               CLK_POL_ACTIVE_HIGH | // clock active high (CKP = 0)
               SPI_CKE_ON | // out changes inactive to active (CKE=0)
               SPI_MODE16_ON | // 8-bit mode
               MASTER_ENABLE_ON; // master mode
// #if (defined(__dsPIC33E__) || defined(__PIC24E__))
//     // nothing to do here. On this family, the SPI1 port uses dedicated
//     // pins for higher speed. The SPI2 port can be used with remappable pins.
// #else
//     // all other families (PIC24H/PIC24F/dsPIC33F)
//     CONFIG_SDO1_TO_RP(RD4_RP); // use RB6 for SDO
//     CONFIG_RB6_AS_DIG_OUTPUT(); // Ensure that this is a digital output
//     CONFIG_SCK1OUT_TO_RP(RD5_RP); // use RB7 for SCLK
//     CONFIG_RB7_AS_DIG_INPUT(); // Ensure that this is a digital input
//     CONFIG_SDI1_TO_RP(RD9_RP); // use RP5 for SDI
//     CONFIG_RB5_AS_DIG_INPUT(); // Ensure that this is a digital input
// #endif
    MCP4922_CS_CONFIG(); // chip select for MCP41xxx
    MCP4922_SHDN_CONFIG();
    //CONFIG_SCK1OUT_TO_RP(RD5_RP);
    CONFIG_SDO1_TO_RP(RD4_RP);
    MCP4922_ON();
    SLAVE_DISABLE(); // disable the chip select
    SPI1STATbits.SPIEN = 1; // enable SPI mode
}

ESOS_CHILD_TASK(_WRITE_DAC_AB, uint16_t a)
{
    ESOS_TASK_BEGIN();

    a = (a >> 4) | DAC_COMMAND_MASK;

    ESOS_TASK_WAIT_ON_SEND_STRING("_WRITE_DAC_AB: ");
    ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(a);
    ESOS_TASK_WAIT_ON_SEND_STRING(" ");
    ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(b);
    ESOS_TASK_WAIT_ON_SEND_STRING("\n");



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

    static uint16_t MAX = 0xFF, MIN = 0x00;
    while (TRUE) {
        static uint16_t i;
        for(i=0; i < 4096; i++) {
            MAX = 4096-i; MIN = i;
            ESOS_TASK_SPAWN_AND_WAIT(dac_hdl, _WRITE_DAC_AB, MAX, MIN);
            ESOS_TASK_WAIT_TICKS(50);
        }
        ESOS_TASK_WAIT_TICKS(1000);
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