/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * esos_at24c02d.h - Defines library for interacting with Atmel AT24C02D EEPROM
 */

#include "esos_pic24_i2c.h"

#define AT24C02D_ADDR (0b10100000)
#define AT24C02D_NUM_PAGES (32)
#define AT24C02D_PAGE_SIZE (8)

#define ESOS_AT24C02D_WRITE_BYTE(u8_addr, u8_data)                                                                     \
    do {                                                                                                               \
        ESOS_TASK_WAIT_ON_WRITE2I2C1(AT24C02D_ADDR, u8_addr, u8_data);                                                 \
    } while (0);

#define ESOS_AT24C02D_WRITE_PAGE(u8_addr, pu8_data, u8_len)                                                            \
    do {                                                                                                               \
        uint8_t u8_i, au8_tmp_data[u8_len + 1];                                                                        \
        au8_tmp_data[0] = u8_addr;                                                                                     \
        for (u8_i = 0; u8_i < u8_len; u8_i++) {                                                                        \
            au8_tmp_data[u8_i + 1] = pu8_data[u8_i];                                                                   \
        }                                                                                                              \
        ESOS_TASK_WAIT_ON_WRITENI2C1(AT24C02D_ADDR, au8_tmp_data, (uint16_t)(u8_len + 1));                             \
    } while (0);

#define ESOS_AT24C02D_READ_BYTE(u8_addr, pu8_data)                                                                     \
    do {                                                                                                               \
        ESOS_TASK_WAIT_ON_WRITE1I2C1(AT24C02D_ADDR, u8_addr);                                                          \
        ESOS_TASK_WAIT_ON_READ1I2C1(AT24C02D_ADDR, *pu8_data);                                                         \
    } while (0);

#define ESOS_AT34C02D_READ_SEQ(u8_addr, pu8_buffer, u16_len)                                                           \
    do {                                                                                                               \
        ESOS_TASK_WAIT_ON_WRITE1I2C1(AT24C02D_ADDR, u8_addr);                                                          \
        ESOS_TASK_WAIT_ON_READNI2C1(AT24C02D_ADDR, pu8_buffer, u16_len);                                               \
    } while (0);
