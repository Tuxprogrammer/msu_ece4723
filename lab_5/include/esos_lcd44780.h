/*
 * "Copyright (c) 2013 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
 * All rights reserved.
 * (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
 * (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
 * (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 *
 */

/**
 * \addtogroup ESOS_Task_LCD_Service
 * @{
 */

/** \file
 *  This file contains macros, prototypes, and definitions for
 *  LCD services for ESOS tasks.
 *
 */

#ifndef ESOS_LCD_H
#define ESOS_LCD_H

/* I N C L U D E S **********************************************************/
#include <esos.h>

/* D E F I N E S ************************************************************/
#define ESOS_TASK_WAIT_ON_LCD44780_REFRESH() ESOS_TASK_WAIT_UNTIL(esos_lcd44780_isCurrent())

#define ESOS_LCD44780_CMD_WAKE 0b00110000
#define ESOS_LCD44780_CMD_CLEAR_DISPLAY 0b00000001
#define ESOS_LCD44780_CMD_RETURN_HOME 0b00000010
#define ESOS_LCD44780_CMD_ENTRY_MODE_SET 0b00000100
#define ESOS_LCD44780_CMD_ENTRY_MODE_SET_INC 0b00000010
#define ESOS_LCD44780_CMD_ENTRY_MODE_SET_SHIFT 0b00000001
#define ESOS_LCD44780_CMD_DISPLAY_ON_OFF 0b00001000
#define ESOS_LCD44780_CMD_DISPLAY_ON_OFF_DISP 0b00000100
#define ESOS_LCD44780_CMD_DISPLAY_ON_OFF_CUR 0b00000010
#define ESOS_LCD44780_CMD_DISPLAY_ON_OFF_BLINK 0b00000001
#define ESOS_LCD44780_CMD_CUR_DISP_SHIFT 0b00010000
#define ESOS_LCD44780_CMD_FUNCTION_SET 0b00100000
#define ESOS_LCD44780_CMD_SET_CGRAM_ADDR 0b01000000
#define ESOS_LCD44780_CMD_SET_DDRAM_ADDR 0b10000000
#define ESOS_LCD44780_MEM_WIDTH 40
#define ESOS_LCD44780_MEM_HEIGHT 2
#define ESOS_LCD44780_NUM_CUSTOM_CHARS 8

#define LCD44780_READ 1
#define LCD44780_WRITE 0

#define ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(u8_cmd)                                                                  \
    do {                                                                                                               \
        ESOS_ALLOCATE_CHILD_TASK(th_lcd44780_child);                                                                   \
        ESOS_TASK_SPAWN_AND_WAIT(th_lcd44780_child, __esos_lcd44780_write_u8, u8_cmd, FALSE, TRUE);                    \
    } while (0)

#define ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT(u8_cmd)                                                           \
    do {                                                                                                               \
        ESOS_ALLOCATE_CHILD_TASK(th_lcd44780_child);                                                                   \
        ESOS_TASK_SPAWN_AND_WAIT(th_lcd44780_child, __esos_lcd44780_write_u8, u8_cmd, FALSE, TRUE);                    \
    } while (0)

#define ESOS_TASK_WAIT_LCD44780_SET_CG_ADDRESS(u8_addr)                                                                \
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(u8_addr | ESOS_LCD44780_CMD_SET_CGRAM_ADDR)

#define ESOS_TASK_WAIT_LCD44780_SET_DATA_ADDRESS(u8_addr)                                                              \
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(u8_addr | ESOS_LCD44780_CMD_SET_DDRAM_ADDR)

#define ESOS_TASK_WAIT_LCD44780_READ_ADDRESS(u8_addr)                                                                  \
    do {                                                                                                               \
        ESOS_ALLOCATE_CHILD_TASK(th_lcd44780_child);                                                                   \
        ESOS_TASK_SPAWN_AND_WAIT(th_lcd44780_child, __esos_lcd44780_read_u8, u8_addr, FALSE, TRUE);                    \
    } while (0)

#define ESOS_TASK_WAIT_LCD44780_WRITE_DATA(u8_data)                                                                    \
    do {                                                                                                               \
        ESOS_ALLOCATE_CHILD_TASK(th_lcd44780_child);                                                                   \
        ESOS_TASK_SPAWN_AND_WAIT(th_lcd44780_child, __esos_lcd44780_write_u8, u8_data, TRUE, TRUE);                    \
    } while (0)

#define ESOS_TASK_WAIT_LCD44780_READ_DATA(u8_addr)                                                                     \
    do {                                                                                                               \
        ESOS_ALLOCATE_CHILD_TASK(th_lcd44780_child);                                                                   \
        ESOS_TASK_SPAWN_AND_WAIT(th_lcd44780_child, __esos_lcd44780_read_u8, u8_addr, TRUE, TRUE);                     \
    } while (0)

/* S T R U C T U R E S ******************************************************/
typedef struct {
    uint8_t au8_data[8];
} esos_lcd44780_char_t;

// allocate space for the child task used by the LCD character module
//  service.  Only one child should ever be active at a time.
static ESOS_TASK_HANDLE th_lcd44780_child;

/* P U B L I C  P R O T O T Y P E S *****************************************/
void esos_lcd44780_configDisplay(void);
void esos_lcd44780_init(void);
void esos_lcd44780_clearScreen(void);
void esos_lcd44780_setCursorHome(void);
void esos_lcd44780_setCursor(uint8_t u8_row, uint8_t u8_column);
void esos_lcd44780_writeChar(uint8_t u8_row, uint8_t u8_column, uint8_t u8_data);
uint8_t esos_lcd44780_getChar(uint8_t u8_row, uint8_t u8_column);
void esos_lcd44780_writeBuffer(uint8_t u8_row, uint8_t u8_column, uint8_t *pu8_data, uint8_t u8_bufflen);
void esos_lcd44780_getBuffer(uint8_t u8_row, uint8_t u8_column, uint8_t *pu8_data, uint8_t u8_bufflen);
void esos_lcd44780_writeString(uint8_t u8_row, uint8_t u8_column, char *psz_data);
void esos_lcd44780_setCursorDisplay(BOOL u8_state);
BOOL esos_lcd44780_getCursorDisplay(void);
void esos_lcd44780_setCursorBlink(BOOL u8_state);
BOOL esos_lcd44780_getCursorBlink(void);
void esos_lcd44780_setDisplayVisible(BOOL u8_state);
BOOL esos_lcd44780_getDisplayVisible(void);
void esos_lcd44780_setCustomChar(uint8_t u8_charSlot, uint8_t *pu8_charData);
void esos_lcd44780_getCustomChar(uint8_t u8_charSlot, uint8_t *pu8_charData);
BOOL esos_lcd44780_isCurrent(void);

ESOS_USER_TASK(__esos_lcd44780_service);
ESOS_CHILD_TASK(__esos_task_wait_lcd44780_while_busy);
ESOS_CHILD_TASK(__esos_lcd44780_write_u8, uint8_t u8_data, BOOL b_isData, BOOL b_useBusyFlag);
ESOS_CHILD_TASK(__esos_lcd44780_read_u8, uint8_t *pu8_data, BOOL b_isData, BOOL b_useBusyFlag);

#endif
