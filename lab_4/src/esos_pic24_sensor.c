/*
 * "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
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
 * \addtogroup ESOS_Task_Sensor_Service
 * @{
 */

/*** I N C L U D E S *************************************************/
#include "esos_pic24_sensor.h"

/*** T H E   C O D E *************************************************/

/*********************************************************
 * Public functions intended to be called by other files *
 *********************************************************/

/** \file
 *  Sensor support functions. \see hwxxx_sensor.h for details.
 */

/*
Configure and enable the sensor module for hwxxx hardware.
\param e_senCh   specifies sensor channel
\param e_senVRef specifies sensor voltage reference
\hideinitializer
*/
void esos_sensor_config_hw(esos_sensor_ch_t e_senCh, esos_sensor_vref_t e_senVRef)
{
    ANALOG_CONFIG();

    // sample at 31 Tad
    ADC_CONFIG();

    AD1CON1bits.ADON = 0;
    /* Channel setup */
    AD1CHS0bits.CH0SA = e_senCh; /* Select AN<ch> for CH0 +ve */
    AD1CHS0bits.CH0NA = 0; /* select Vref- for ch0 -ve */

    AD1CHS123 = 0; /* disable ch1,2,3 */
    AD1CSSH = 0; /* Set Input Scan Select Register High */
    AD1CSSL = 0; /* Set Input Scan Select Register Low */
    AD1CON1bits.ADON = 1;
}

/**
Initiate a conversion for a configured sensor
\hideinitializer
 */
void esos_sensor_initiate_hw(void)
{
    esos_SetUserFlag(ESOS_SENSOR_IS_CONVERTING_FLAG);
    AD1CON1bits.SAMP = 1; // initiate a conversion;
}

/**
Receive the value from a conversion that has already been initiated
\hideinitializer
 */
uint16_t esos_sensor_getvalue_u16_hw(void)
{
    if (!esos_IsUserFlagClear(ESOS_SENSOR_IS_CONVERTING_FLAG)) {
        // wait a minute ... who ARE you ???
        return 0;
    }

    uint16_t u16_adcVal = (ADC1BUF0); // grab the value.
    return u16_adcVal;
}

/**
Release any pending conversions for the sensor
\hideinitializer
 */
void esos_sensor_release_hw(void)
{
    esos_ClearUserFlag(ESOS_SENSOR_IS_CONVERTING_FLAG);
    AD1CON1bits.ADON = 0; // turn off the adc
}

/**
Determine truth of: the sensor is currently converting
\hideinitializer
 */
BOOL esos_sensor_is_converting_hw(void)
{
    if (AD1CON1bits.DONE == 1) {
        esos_ClearUserFlag(ESOS_SENSOR_IS_CONVERTING_FLAG);
        return FALSE;
    }
    return esos_IsUserFlagSet(ESOS_SENSOR_IS_CONVERTING_FLAG);
}

int convert_uint32_t_to_str(uint32_t u32_val, char *str, uint32_t len, int base)
{
    uint32_t i = 0;
    uint32_t digit;

    if (len == 0)
        return -1;
    for (; i < 10; i++)
        str[i] = 0;
    i = 0;
    do {
        digit = u32_val % base;
        if (digit < 0xA)
            str[i++] = '0' + digit;
        else
            str[i++] = 'A' + digit - 0xA;

        u32_val /= base;
    } while (u32_val && (i < (len - 1)));

    if (i == (len - 1) && u32_val)
        return -1;

    str[i] = '\0';

    // strrev:
    char *p = str;
    char *q = p;
    while (q && *q)
        ++q;
    for (--q; p < q; ++p, --q)
        *p = *p ^ *q, *q = *p ^ *q, *p = *p ^ *q;

    return 0;
}