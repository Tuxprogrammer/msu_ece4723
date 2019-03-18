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

static uint8_t au8_EEPROM_DATA[AT24C02D_NUM_PAGES * AT24C02D_PAGE_SIZE] = {
    'S',  'I',  'N',  'E',  0x00, 0x00, 0x00, 0x02, // Page 0 - SINE Name Entry, Scale Factor = (1 / 2)
    0x00, 0x03, 0x06, 0x0A, 0x0D, 0x10, 0x14, 0x17, // Page 1 - SINE 1/4 Wave Entry
    0x1A, 0x1E, 0x21, 0x24, 0x28, 0x2B, 0x2E, 0x32, // Page 2
    0x35, 0x38, 0x3C, 0x3F, 0x42, 0x45, 0x49, 0x4C, // Page 3
    0x4F, 0x52, 0x55, 0x58, 0x5C, 0x5F, 0x62, 0x65, // Page 4
    0x68, 0x6B, 0x6E, 0x71, 0x74, 0x77, 0x7A, 0x7D, // Page 5
    0x80, 0x83, 0x86, 0x89, 0x8B, 0x8E, 0x91, 0x94, // Page 6
    0x96, 0x99, 0x9C, 0x9E, 0xA1, 0xA4, 0xA6, 0xA9, // Page 7
    0xAB, 0xAE, 0xB0, 0xB3, 0xB5, 0xB7, 0xBA, 0xBC, // Page 8
    0xBE, 0xC0, 0xC3, 0xC5, 0xC7, 0xC9, 0xCB, 0xCD, // Page 9
    0xCF, 0xD1, 0xD3, 0xD5, 0xD7, 0xD8, 0xDA, 0xDC, // Page 10
    0xDD, 0xDF, 0xE1, 0xE2, 0xE4, 0xE5, 0xE7, 0xE8, // Page 11
    0xE9, 0xEB, 0xEC, 0xED, 0xEE, 0xF0, 0xF1, 0xF2, // Page 12
    0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF7, 0xF8, 0xF9, // Page 13
    0xFA, 0xFA, 0xFB, 0xFB, 0xFC, 0xFC, 0xFD, 0xFD, // Page 14
    0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, // Page 15
    'N',  'O',  'I',  'S',  'E',  0x00, 0x00, 0x01, // Page 16 - NOISE Name Entry, Scale Factor = (1 / 1)
    0xCE, 0x7F, 0x9C, 0x46, 0x1F, 0x2F, 0x89, 0x45, // Page 17 - NOISE 1/4 Wave Entry
    0xA7, 0xB1, 0xB0, 0x53, 0xDE, 0xE4, 0xB6, 0x71, // Page 18
    0x75, 0x6A, 0x83, 0x23, 0xCD, 0x24, 0x50, 0x6C, // Page 19
    0xD0, 0xDC, 0x04, 0x8E, 0x4E, 0xF3, 0x4D, 0x10, // Page 20
    0x4E, 0xFB, 0x37, 0xD6, 0x0F, 0x47, 0x93, 0x7D, // Page 21
    0x99, 0x21, 0xEA, 0xCC, 0x70, 0x54, 0x40, 0x1A, // Page 22
    0x22, 0xD0, 0xFC, 0x04, 0x9F, 0x52, 0x42, 0xA1, // Page 23
    0xC9, 0x66, 0x2A, 0x97, 0x2C, 0x50, 0x00, 0xAF, // Page 24
    0xCB, 0x2E, 0xFB, 0x80, 0x81, 0xD9, 0x0A, 0xE8, // Page 25
    0xAF, 0x0A, 0x5B, 0xB5, 0x64, 0x1A, 0xFA, 0xD0, // Page 26
    0x65, 0x2B, 0x2E, 0xF7, 0x69, 0xC2, 0x8F, 0xD2, // Page 27
    0xB9, 0xFA, 0xAD, 0xE3, 0xE2, 0xF1, 0x1C, 0x1C, // Page 28
    0xA3, 0xF0, 0xF4, 0xCF, 0xAE, 0xA0, 0xF1, 0x4B, // Page 29
    0x27, 0xB4, 0xAC, 0xDA, 0xE9, 0x99, 0xB2, 0x56, // Page 30
    0x3D, 0x1B, 0x6A, 0x41, 0x24, 0x97, 0x2D, 0xB0, // Page 31
};

ESOS_USER_TIMER(heartbeat_LED)
{
    esos_uiF14_toggleLED3();
}

ESOS_USER_TASK(program_eeprom)
{
    ESOS_TASK_BEGIN();

    uint8_t u8_addr;

    // Write au8_EEPROM_DATA to entire memory
    for (u8_addr = 0; u8_addr < AT24C02D_NUM_PAGES * AT24C02D_PAGE_SIZE; u8_addr += AT24C02D_PAGE_SIZE) {
        /*                                             ↓ 1337 h4x (may not work) */
        ESOS_AT24C02D_WRITE_PAGE(u8_addr, (uint8_t *)(au8_EEPROM_DATA + u8_addr), AT24C02D_PAGE_SIZE);
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("\nEEPROM Write Page - Addr: ");
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_addr);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_YIELD();
    }

    ESOS_TASK_YIELD();
    ESOS_TASK_END();
}

ESOS_USER_TASK(test_eeprom)
{
    ESOS_TASK_BEGIN();

    ESOS_TASK_WAIT_TICKS(1000);

    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("Verifying memory state...\n");
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

    uint8_t u8_addr, *pu8_data;
    for (u8_addr = 0; u8_addr < AT24C02D_NUM_PAGES * AT24C02D_PAGE_SIZE; u8_addr++) {
        ESOS_AT24C02D_READ_BYTE(u8_addr, pu8_data); // WHY DOESNT THIS WORK
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("\nADDR: ");
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_addr);
        ESOS_TASK_WAIT_ON_SEND_STRING(" EXP: ");
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(au8_EEPROM_DATA[u8_addr]);
        ESOS_TASK_WAIT_ON_SEND_STRING(" ACT: ");
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(*pu8_data);
        ESOS_TASK_WAIT_ON_SEND_STRING((*pu8_data == au8_EEPROM_DATA[u8_addr]) ? " GOOD" : " BAD");
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        ESOS_TASK_YIELD();
    }

    ESOS_TASK_YIELD();
    ESOS_TASK_END();
}

void user_init()
{
    config_esos_uiF14();
    esos_RegisterTimer(heartbeat_LED, 500);
    esos_RegisterTask(program_eeprom);
}