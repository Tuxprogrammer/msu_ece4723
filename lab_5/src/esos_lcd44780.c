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

#include "revF14.h"
#include "esos_lcd44780.h"
#include "esos_pic24_lcd44780.h"
#include <esos.h>
#include <stdlib.h>

// Main data structure for updating lcd44780
struct {
    BOOL b_cursorPositionNeedsUpdate;
    uint8_t u8_cursorRow;
    uint8_t u8_cursorCol;

    BOOL b_cursorShownNeedsUpdate;
    BOOL b_cursorShown;

    BOOL b_cursorBlinkNeedsUpdate;
    BOOL b_cursorBlink;

    BOOL b_displayVisibleNeedsUpdate;
    BOOL b_displayVisible;

    BOOL ab_lcdBufferNeedsUpdate[ESOS_LCD44780_MEM_HEIGHT][ESOS_LCD44780_MEM_WIDTH];
    char aac_lcdBuffer[ESOS_LCD44780_MEM_HEIGHT][ESOS_LCD44780_MEM_WIDTH];

    BOOL ab_customCharNeedsUpdate[ESOS_LCD44780_NUM_CUSTOM_CHARS];
    esos_lcd44780_char_t ast_customChar[ESOS_LCD44780_NUM_CUSTOM_CHARS];
} esos_lcd44780_vars;

// Hidden LCD character module service/housekeeping task
ESOS_USER_TASK(__esos_lcd44780_service)
{
    // Can first initialized the LCD module hardware and setup the LCD service software structures
    // Also manages the LCD character module got ESOS
    // The LCD service hidden task will need to maintain a buffer containing the LCD character display
    ESOS_TASK_BEGIN();

#ifndef ESOS_LCD44780_NIBBLE_MODE
    ESOS_TASK_WAIT_TICKS(100); // Wait >15 msec after power is applied
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT(ESOS_LCD44780_CMD_WAKE);
    ESOS_TASK_WAIT_TICKS(10); // must wait 5ms, busy flag not available
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT(ESOS_LCD44780_CMD_WAKE);
    ESOS_TASK_WAIT_TICKS(1); // must wait 160us, busy flag not available
    // 4-bit startup sequence from datasheet
    // #ifdef ESOS_LCD44780_NIBBLE_MODE // Use 4-bit data line mode
    //     ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT(ESOS_LCD44780_CMD_WAKE_4BIT);
    // #else
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT(ESOS_LCD44780_CMD_WAKE);
    // #endif
    ESOS_TASK_WAIT_TICKS(1); // must wait 160us, busy flag not available

    // Send startup sequence from datasheet
    // #ifdef ESOS_LCD44780_NIBBLE_MODE // Use 4-bit data line mode
    //     ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_FUNCTION_SET |
    //     ESOS_LCD44780_CMD_FUNCITON_SET_N_2LINE);
    // #else // Use 8-bit data line mode
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_FUNCTION_SET | ESOS_LCD44780_CMD_FUNCTION_SET_DL_8BIT |
                                          ESOS_LCD44780_CMD_FUNCITON_SET_N_2LINE);
    // #endif
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_CUR_DISP_SHIFT);
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_DISPLAY_ON_OFF | ESOS_LCD44780_CMD_DISPLAY_ON_OFF_DISP);
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_ENTRY_MODE_SET | ESOS_LCD44780_CMD_ENTRY_MODE_SET_INC);
#else // 4-bit mode SEE THIS: http://www.newhavendisplay.com/app_notes/ST7066U.pdf
    ESOS_TASK_WAIT_TICKS(100); // Wait >15 msec after power is applied
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT_4BIT(ESOS_LCD44780_CMD_WAKE);
    ESOS_TASK_WAIT_TICKS(10); // must wait 5ms, busy flag not available
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT_4BIT(ESOS_LCD44780_CMD_WAKE);
    ESOS_TASK_WAIT_TICKS(1); // must wait 160us, busy flag not available
    // 4-bit startup sequence from datasheet
    // #ifdef ESOS_LCD44780_NIBBLE_MODE // Use 4-bit data line mode
    //     ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT(ESOS_LCD44780_CMD_WAKE_4BIT);
    // #else
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_NOWAIT_4BIT(ESOS_LCD44780_CMD_WAKE);
    // #endif
    ESOS_TASK_WAIT_TICKS(1); // must wait 160us, busy flag not available

    // Send startup sequence from datasheet
    // #ifdef ESOS_LCD44780_NIBBLE_MODE // Use 4-bit data line mode
    //     ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_FUNCTION_SET |
    //     ESOS_LCD44780_CMD_FUNCITON_SET_N_2LINE);
    // #else // Use 8-bit data line mode
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND_4BIT(ESOS_LCD44780_CMD_FUNCTION_SET | ESOS_LCD44780_CMD_FUNCTION_SET_DL_4BIT |
                                          ESOS_LCD44780_CMD_FUNCITON_SET_N_2LINE);
    // #endif
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_CUR_DISP_SHIFT);
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_DISPLAY_ON_OFF | ESOS_LCD44780_CMD_DISPLAY_ON_OFF_DISP);
    ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(ESOS_LCD44780_CMD_ENTRY_MODE_SET | ESOS_LCD44780_CMD_ENTRY_MODE_SET_INC);

#endif

    while (TRUE) {
        static uint8_t i, u8_col, u8_row;

        if (esos_lcd44780_vars.b_cursorPositionNeedsUpdate) {
            esos_lcd44780_vars.b_cursorPositionNeedsUpdate = FALSE;
            ESOS_TASK_WAIT_LCD44780_SET_DATA_ADDRESS((esos_lcd44780_vars.u8_cursorRow > 0 ? 0x40 : 0x00) |
                                                     esos_lcd44780_vars.u8_cursorCol);
        }

        if (esos_lcd44780_vars.b_cursorShownNeedsUpdate || esos_lcd44780_vars.b_cursorBlinkNeedsUpdate ||
            esos_lcd44780_vars.b_displayVisibleNeedsUpdate) {
            esos_lcd44780_vars.b_cursorShownNeedsUpdate = FALSE;
            esos_lcd44780_vars.b_cursorBlinkNeedsUpdate = FALSE;
            esos_lcd44780_vars.b_displayVisibleNeedsUpdate = FALSE;
            ESOS_TASK_WAIT_LCD44780_WRITE_COMMAND(
                ESOS_LCD44780_CMD_DISPLAY_ON_OFF |
                (esos_lcd44780_vars.b_cursorShown ? ESOS_LCD44780_CMD_DISPLAY_ON_OFF_CUR : 0) |
                (esos_lcd44780_vars.b_cursorBlink ? ESOS_LCD44780_CMD_DISPLAY_ON_OFF_BLINK : 0) |
                (esos_lcd44780_vars.b_displayVisible ? ESOS_LCD44780_CMD_DISPLAY_ON_OFF_DISP : 0));
        }

        for (u8_row = 0; u8_row < ESOS_LCD44780_MEM_HEIGHT; ++u8_row) {
            for (u8_col = 0; u8_col < ESOS_LCD44780_MEM_WIDTH; ++u8_col) {
                // Update all consecutive characters that need it, but only send the address once.
                if (esos_lcd44780_vars.ab_lcdBufferNeedsUpdate[u8_row][u8_col]) {
                    esos_lcd44780_vars.b_cursorPositionNeedsUpdate = TRUE;
                    ESOS_TASK_WAIT_LCD44780_SET_DATA_ADDRESS((u8_row > 0 ? 0x40 : 0x00) | u8_col);
                    do {
                        esos_lcd44780_vars.ab_lcdBufferNeedsUpdate[u8_row][u8_col] = FALSE;
                        ESOS_TASK_WAIT_LCD44780_WRITE_DATA(esos_lcd44780_vars.aac_lcdBuffer[u8_row][u8_col]);
                        ++u8_col;
                        ESOS_TASK_YIELD();
                    } while (esos_lcd44780_vars.ab_lcdBufferNeedsUpdate[u8_row][u8_col] &&
                             u8_col < ESOS_LCD44780_MEM_WIDTH);
                }
                ESOS_TASK_YIELD();
            }
        }

        for (i = 0; i < ESOS_LCD44780_NUM_CUSTOM_CHARS; ++i) {
            // Update all characters that need updating, but only send the CG address once.
            if (esos_lcd44780_vars.ab_customCharNeedsUpdate[i]) {
                // We destroy the hardware's cursor position, so fix it later.
                esos_lcd44780_vars.b_cursorPositionNeedsUpdate = TRUE;
                ESOS_TASK_WAIT_LCD44780_SET_CG_ADDRESS(8 * i);

                do {
                    static int n;

                    esos_lcd44780_vars.ab_customCharNeedsUpdate[i] = FALSE;

                    for (n = 0; n < 8; ++n) {
                        ESOS_TASK_WAIT_LCD44780_WRITE_DATA(esos_lcd44780_vars.ast_customChar[i].au8_data[n]);
                    }
                    ++i;
                    ESOS_TASK_YIELD();
                } while (esos_lcd44780_vars.ab_customCharNeedsUpdate[i] && i < ESOS_LCD44780_NUM_CUSTOM_CHARS);
            }
            ESOS_TASK_YIELD();
        }

        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

void esos_lcd44780_configDisplay(void)
{
    unsigned int u8_column;

    esos_lcd44780_vars.b_displayVisible = TRUE;
    esos_lcd44780_vars.b_displayVisibleNeedsUpdate = TRUE;
    esos_lcd44780_vars.b_cursorShown = FALSE;
    esos_lcd44780_vars.b_cursorShownNeedsUpdate = TRUE;
    esos_lcd44780_vars.b_cursorBlink = FALSE;
    esos_lcd44780_vars.b_cursorBlinkNeedsUpdate = TRUE;

    esos_lcd44780_clearScreen();

    for (u8_column = 0; u8_column < ESOS_LCD44780_NUM_CUSTOM_CHARS; ++u8_column) {
        esos_lcd44780_vars.ab_customCharNeedsUpdate[u8_column] = FALSE;
    }
}

void esos_lcd44780_init(void)
{
    // Set up the hardware aspects of the HWxxx interface of the LCD module service
    //    direction, thresholds, etc
    __esos_lcd44780_hw_configDataPinsAsOutput();
    __ESOS_LCD44780_HW_SET_RW_WRITE();
    __ESOS_LCD44780_HW_SET_RS_REGISTERS();

    // give HW specific code a chance to do anything else to init/config
    __esos_lcd44780_hw_config();

    // install our LCD housekeeping task into the scheduler
    esos_RegisterTask(__esos_lcd44780_service);
}

void esos_lcd44780_clearScreen(void)
{
    uint8_t u8_height;
    uint8_t u8_width;
    // Clears the buffer
    for (u8_height = 0; u8_height < ESOS_LCD44780_MEM_HEIGHT; u8_height++) {
        for (u8_width = 0; u8_width < ESOS_LCD44780_MEM_WIDTH; u8_width++) {
            esos_lcd44780_vars.aac_lcdBuffer[u8_height][u8_width] = ' ';
            esos_lcd44780_vars.ab_lcdBufferNeedsUpdate[u8_height][u8_width] = TRUE;
        }
    }

    esos_lcd44780_setCursor(0, 0);
    esos_lcd44780_vars.b_cursorPositionNeedsUpdate = TRUE;
}

void esos_lcd44780_setCursorHome(void)
{
    esos_lcd44780_setCursor(0, 0);
    esos_lcd44780_vars.b_cursorPositionNeedsUpdate = TRUE;
}

void esos_lcd44780_setCursor(uint8_t u8_row, uint8_t u8_column)
{
    // Move cursor to (u8_row,u8_column) without changing memory buffer or the display
    esos_lcd44780_vars.u8_cursorRow = u8_row;
    esos_lcd44780_vars.u8_cursorCol = u8_column;
    esos_lcd44780_vars.b_cursorPositionNeedsUpdate = TRUE;
}

void esos_lcd44780_writeChar(uint8_t u8_row, uint8_t u8_column, uint8_t u8_data)
{
    esos_lcd44780_vars.aac_lcdBuffer[u8_row % ESOS_LCD44780_MEM_HEIGHT][u8_column % ESOS_LCD44780_MEM_WIDTH] = u8_data;
    esos_lcd44780_vars.ab_lcdBufferNeedsUpdate[u8_row % ESOS_LCD44780_MEM_HEIGHT][u8_column % ESOS_LCD44780_MEM_WIDTH] = TRUE;
}

uint8_t esos_lcd44780_getChar(uint8_t u8_row, uint8_t u8_column)
{
    return esos_lcd44780_vars.aac_lcdBuffer[u8_row % ESOS_LCD44780_MEM_HEIGHT][u8_column % ESOS_LCD44780_MEM_WIDTH];
}

void esos_lcd44780_writeBuffer(uint8_t u8_row, uint8_t u8_column, uint8_t *pu8_data, uint8_t u8_bufflen)
{
    int n;

    // Write u8_bufflen characters from pu8_data to (u8_row,u8_column)
    for (n = 0; n < u8_bufflen; n++) {
        esos_lcd44780_writeChar(u8_row, u8_column + n, pu8_data[n]);
    }
}

// needs testing
void esos_lcd44780_getBuffer(uint8_t u8_row, uint8_t u8_column, uint8_t *pu8_data, uint8_t u8_bufflen)
{
    int n;

    // Write u8_bufflen characters from pu8_data to (u8_row,u8_column)
    for (n = 0; n < u8_bufflen; n++) {
        pu8_data[n] = esos_lcd44780_getChar(u8_row, u8_column + n);
    }
}

void esos_lcd44780_writeString(uint8_t u8_row, uint8_t u8_column, char *psz_data)
{
    int n = 0;

    // Write zero-terminated string psz_data to location starting at (u8_row,u8_column)
    while (psz_data[n] != '\0') {
        esos_lcd44780_writeChar(u8_row, u8_column + n, psz_data[n]);
        n++;
    }
}

void esos_lcd44780_setCursorDisplay(BOOL u8_state)
{
    // Set cursor display state to u8_state, if u8_state is different
    if (u8_state != esos_lcd44780_getCursorDisplay()) {
        esos_lcd44780_vars.b_cursorShown = u8_state;
        esos_lcd44780_vars.b_cursorShownNeedsUpdate = TRUE;
    }
}

// needs testing
BOOL esos_lcd44780_getCursorDisplay(void)
{
    // Return cursor display state
    return esos_lcd44780_vars.b_cursorShown;
}

void esos_lcd44780_setCursorBlink(BOOL u8_state)
{
    // Set cursor blink state to u8_state
    if (u8_state != esos_lcd44780_getCursorBlink()) {
        esos_lcd44780_vars.b_cursorBlink = u8_state;
        esos_lcd44780_vars.b_cursorBlinkNeedsUpdate = TRUE;
    }
}

// needs testing
BOOL esos_lcd44780_getCursorBlink(void)
{
    // Return cursor blink state
    return esos_lcd44780_vars.b_cursorBlink;
}

void esos_lcd44780_setDisplayVisible(BOOL u8_state)
{
    // Set display visible state to u8_state
    if (u8_state != esos_lcd44780_getDisplayVisible()) {
        esos_lcd44780_vars.b_displayVisible = u8_state;
        esos_lcd44780_vars.b_displayVisibleNeedsUpdate = TRUE;
    }
}

// needs testing
BOOL esos_lcd44780_getDisplayVisible(void)
{
    // Return display visible state
    return esos_lcd44780_vars.b_displayVisible;
}

void esos_lcd44780_setCustomChar(uint8_t u8_charSlot, uint8_t *pu8_charData)
{
    int n;

    // Set custom character memory for u8_charSlot to data in pu8_charData
    for (n = 0; n < 8; n++) {
        esos_lcd44780_vars.ast_customChar[u8_charSlot].au8_data[n] = pu8_charData[n];
    }
    esos_lcd44780_vars.ab_customCharNeedsUpdate[u8_charSlot] = TRUE;
}

// needs testing
void esos_lcd44780_getCustomChar(uint8_t u8_charSlot, uint8_t *pu8_charData)
{
    int n;

    // Return pu8_charData with custom character memory for u8_charSlot
    for (n = 0; n < 8; n++) {
        pu8_charData[n] = esos_lcd44780_vars.ast_customChar[u8_charSlot].au8_data[n];
    }
}

BOOL esos_lcd44780_isCurrent(void)
{
    uint8_t u8_row, u8_column;

    if (esos_lcd44780_vars.b_cursorPositionNeedsUpdate || esos_lcd44780_vars.b_cursorBlinkNeedsUpdate ||
        esos_lcd44780_vars.b_displayVisibleNeedsUpdate) {
        return FALSE;
    }

    for (u8_row = 0; u8_row < ESOS_LCD44780_MEM_HEIGHT; ++u8_row) {
        for (u8_column = 0; u8_column < ESOS_LCD44780_MEM_WIDTH; ++u8_column) {
            if (esos_lcd44780_vars.ab_lcdBufferNeedsUpdate[u8_row][u8_column]) {
                return FALSE;
            }
        }
    }

    for (u8_column = 0; u8_column < ESOS_LCD44780_NUM_CUSTOM_CHARS; ++u8_column) {
        if (esos_lcd44780_vars.ab_customCharNeedsUpdate[u8_column]) {
            return FALSE;
        }
    }

    return TRUE;
}

ESOS_CHILD_TASK(__esos_lcd44780_read_u8, uint8_t *pu8_data, BOOL b_isData, BOOL b_useBusyFlag)
{
    ESOS_TASK_BEGIN();

    if (b_useBusyFlag) {
        do {
            ESOS_ALLOCATE_CHILD_TASK(th_lcd44780_child);
            ESOS_TASK_SPAWN_AND_WAIT(th_lcd44780_child, __esos_task_wait_lcd44780_while_busy);
        } while (FALSE);
    }

    if (b_isData) {
        __ESOS_LCD44780_HW_SET_RS_DATA();
    } else {
        __ESOS_LCD44780_HW_SET_RS_REGISTERS();
    }

    __ESOS_LCD44780_HW_SET_RW_READ();
    __esos_lcd44780_hw_configDataPinsAsInput();

    // #ifdef ESOS_LCD44780_NIBBLE_MODE
    //     // Clock in MSB (in 4 bit mode)
    //     __ESOS_LCD44780_HW_SET_E_HIGH();
    //     ESOS_TASK_WAIT_TICKS(1);
    //     *pu8_data = __esos_lcd44780_hw_getDataPins() << 4;
    //     __ESOS_LCD44780_HW_SET_E_LOW();
    //     ESOS_TASK_WAIT_TICKS(1);
    // #endif

    // Clock in LSB (in 4 bit mode) or all bits in 8-bit mode
    __ESOS_LCD44780_HW_SET_E_HIGH();
    ESOS_TASK_WAIT_TICKS(1);
    *pu8_data |= __esos_lcd44780_hw_getDataPins();
    __ESOS_LCD44780_HW_SET_E_LOW();
    ESOS_TASK_WAIT_TICKS(1);

    ESOS_TASK_END();
}

ESOS_CHILD_TASK(__esos_lcd44780_write_u8, uint8_t u8_data, BOOL b_isData, BOOL b_useBusyFlag, BOOL b_send8Bits)
{
    ESOS_TASK_BEGIN();

    if (b_useBusyFlag) {
        do {
            ESOS_ALLOCATE_CHILD_TASK(th_lcd44780_child);
            ESOS_TASK_SPAWN_AND_WAIT(th_lcd44780_child, __esos_task_wait_lcd44780_while_busy);
        } while (FALSE);
    }

    if (b_isData) {
        __ESOS_LCD44780_HW_SET_RS_DATA();
    } else {
        __ESOS_LCD44780_HW_SET_RS_REGISTERS();
    }

    __ESOS_LCD44780_HW_SET_RW_WRITE();
    __esos_lcd44780_hw_configDataPinsAsOutput();

#ifdef ESOS_LCD44780_NIBBLE_MODE
    // Clock out MSB (in 4 bit mode)
    __esos_lcd44780_hw_setDataPins(u8_data >> 4);
    __ESOS_LCD44780_HW_SET_E_HIGH();
    ESOS_TASK_WAIT_TICKS(1);
    __ESOS_LCD44780_HW_SET_E_LOW();
    ESOS_TASK_WAIT_TICKS(1);
#endif

    if (b_send8Bits) {
        // Clock out LSB (in 4 bit mode) or everything in 8-bit mode
        __esos_lcd44780_hw_setDataPins(u8_data);
        __ESOS_LCD44780_HW_SET_E_HIGH();
        ESOS_TASK_WAIT_TICKS(1);
        __ESOS_LCD44780_HW_SET_E_LOW();
        ESOS_TASK_WAIT_TICKS(1);
    }

    ESOS_TASK_END();
}

ESOS_CHILD_TASK(__esos_task_wait_lcd44780_while_busy)
{
    static BOOL b_hw_lcd_isBusy = FALSE;

    ESOS_TASK_BEGIN();

    while (TRUE) {
        __esos_lcd44780_hw_configDataPinsAsInput();
        __ESOS_LCD44780_HW_SET_RS_REGISTERS();
        __ESOS_LCD44780_HW_SET_RW_READ();

        // Clock in LSB (in 4 bit mode)
        __ESOS_LCD44780_HW_SET_E_HIGH();
        ESOS_TASK_WAIT_TICKS(1);
#ifdef ESOS_LCD44780_NIBBLE_MODE
        b_hw_lcd_isBusy = (__esos_lcd44780_hw_getDataPins() >> 3);
        __ESOS_LCD44780_HW_SET_E_LOW();
        ESOS_TASK_WAIT_TICKS(1);
        __ESOS_LCD44780_HW_SET_E_HIGH();
        ESOS_TASK_WAIT_TICKS(1);
#else
        b_hw_lcd_isBusy = (__esos_lcd44780_hw_getDataPins() >> 7);
#endif
        __ESOS_LCD44780_HW_SET_E_LOW();
        ESOS_TASK_WAIT_TICKS(1);

        if (b_hw_lcd_isBusy) {
            ESOS_TASK_WAIT_TICKS(1);
        } else {
            ESOS_TASK_EXIT();
        }
    };

    ESOS_TASK_END();
}
