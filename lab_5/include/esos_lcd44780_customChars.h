/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * esos_lcd44780_customChars.h - custom characters to be used for the hd44780 LCD display
 */

#ifndef ESOS_LCD44780_CUSTOMCHARS
#define ESOS_LCD44780_CUSTOMCHARS

#include "esos_lcd44780.h"

static uint8_t char_1EIGTH_BAR[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F };
static uint8_t char_2EIGTH_BAR[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F };
static uint8_t char_3EIGTH_BAR[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F };
static uint8_t char_4EIGTH_BAR[8] = { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F };
static uint8_t char_5EIGTH_BAR[8] = { 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
static uint8_t char_6EIGTH_BAR[8] = { 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
static uint8_t char_7EIGTH_BAR[8] = { 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
static uint8_t char_8EIGTH_BAR[8] = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
enum custom_chars_bar {
    _1EIGTH_BAR,
    _2EIGTH_BAR,
    _3EIGTH_BAR,
    _4EIGTH_BAR,
    _5EIGTH_BAR,
    _6EIGTH_BAR,
    _7EIGTH_BAR,
    _8EIGTH_BAR,
};

static uint8_t char_SLIDER_LINE[8] = { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00 };
static uint8_t char_SLIDER_BAR1[8] = { 0x00, 0x00, 0x10, 0x10, 0x1F, 0x10, 0x10, 0x00 };
static uint8_t char_SLIDER_BAR2[8] = { 0x00, 0x00, 0x08, 0x08, 0x1F, 0x08, 0x08, 0x00 };
static uint8_t char_SLIDER_BAR3[8] = { 0x00, 0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00 };
static uint8_t char_SLIDER_BAR4[8] = { 0x00, 0x00, 0x02, 0x02, 0x1F, 0x02, 0x02, 0x00 };
static uint8_t char_SLIDER_BAR5[8] = { 0x00, 0x00, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x00 };
enum custom_chars_slider { SLIDER_LINE, SLIDER_BAR1, SLIDER_BAR2, SLIDER_BAR3, SLIDER_BAR4, SLIDER_BAR5 };

void esos_lcd44780_init_custom_chars_bar()
{
    esos_lcd44780_setCustomChar(_1EIGTH_BAR, char_1EIGTH_BAR);
    esos_lcd44780_setCustomChar(_2EIGTH_BAR, char_2EIGTH_BAR);
    esos_lcd44780_setCustomChar(_3EIGTH_BAR, char_3EIGTH_BAR);
    esos_lcd44780_setCustomChar(_4EIGTH_BAR, char_4EIGTH_BAR);
    esos_lcd44780_setCustomChar(_5EIGTH_BAR, char_5EIGTH_BAR);
    esos_lcd44780_setCustomChar(_6EIGTH_BAR, char_6EIGTH_BAR);
    esos_lcd44780_setCustomChar(_7EIGTH_BAR, char_7EIGTH_BAR);
    esos_lcd44780_setCustomChar(_8EIGTH_BAR, char_8EIGTH_BAR);
}

void esos_lcd44780_init_custom_chars_slider()
{
    esos_lcd44780_setCustomChar(SLIDER_LINE, char_SLIDER_LINE);
    esos_lcd44780_setCustomChar(SLIDER_BAR1, char_SLIDER_BAR1);
    esos_lcd44780_setCustomChar(SLIDER_BAR2, char_SLIDER_BAR2);
    esos_lcd44780_setCustomChar(SLIDER_BAR3, char_SLIDER_BAR3);
    esos_lcd44780_setCustomChar(SLIDER_BAR4, char_SLIDER_BAR4);
    esos_lcd44780_setCustomChar(SLIDER_BAR5, char_SLIDER_BAR5);
}

#endif
