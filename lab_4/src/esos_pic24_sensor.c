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
    switch (e_senCh) {
    case ESOS_SENSOR_CH00:
        ADC_CONFIG(POT1, 31);
        break;
    case ESOS_SENSOR_CH01:
        ADC_CONFIG(TEMP1, 31);
        break;
    case ESOS_SENSOR_CH02:

        break;
    case ESOS_SENSOR_CH03:

        break;
    case ESOS_SENSOR_CH04:

        break;
    case ESOS_SENSOR_CH05:

        break;
    case ESOS_SENSOR_CH06:

        break;
    case ESOS_SENSOR_CH07:

        break;
    case ESOS_SENSOR_CH08:

        break;
    case ESOS_SENSOR_CH09:

        break;
    case ESOS_SENSOR_CH0A:

        break;
    case ESOS_SENSOR_CH0B:

        break;
    case ESOS_SENSOR_CH0C:

        break;
    case ESOS_SENSOR_CH0D:

        break;
    case ESOS_SENSOR_CH0E:

        break;
    case ESOS_SENSOR_CH0F:

        break;
    }

    
}

/**
Initiate a conversion for a configured sensor
\hideinitializer
 */
void esos_sensor_initiate_hw(void)
{
    esos_SetUserFlag(ESOS_SENSOR_IS_CONVERTING_FLAG);
}

/**
Receive the value from a conversion that has already been initiated
\hideinitializer
 */
uint16_t esos_sensor_getvalue_u16_hw(void)
{
    if (esos_IsUserFlagClear(ESOS_SENSOR_IS_CONVERTING_FLAG))
        return 0;
    uint16_t u16_adcVal = convertADC1();
    return u16_adcVal;
}

/**
Release any pending conversions for the sensor
\hideinitializer
 */
void esos_sensor_release_hw(void)
{
    esos_ClearUserFlag(ESOS_SENSOR_IS_CONVERTING_FLAG);
}

/**
Determine truth of: the sensor is currently converting
\hideinitializer
 */
BOOL esos_sensor_is_converting_hw(void)
{
    esos_IsUserFlagSet(ESOS_SENSOR_IS_CONVERTING_FLAG);
}
