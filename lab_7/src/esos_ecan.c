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
 * \addtogroup ESOS_Task_ECAN_Service
 * @{
 */

 /** \file
 *  This file contains macros, prototypes, and definitions for
 *  ECAN services for ESOS tasks.
 */

/*** I N C L U D E S *************************************************/
#include    <string.h>
#include    "esos.h"
#include    "esos_ecan.h"
#include    "pic24/esos_pic24_ecan.h"

/*** D E F I N E ******************************************************/
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/*** T H E   C O D E *************************************************/
client_t canfactory_clients[MAX_CANFACTORY_CLIENTS];
ESOS_TASK_HANDLE __esos_ecan_hTask;
MAILMESSAGE __esos_ecan_mailMsg;
uint8_t u8_numCANFactoryClients = 0;

__eds__ ECANMSG __esos_ecan_msgBuf[__ESOS_ECAN_HW_NUM_BUFS] __attribute__((space(dma),aligned(__ESOS_ECAN_HW_NUM_BUFS*16),eds));

/*void esos_ecan_send ( uint16_t u16_can_id, uint8_t *pu8_msg, uint8_t u8_len ) {

    __esos_ecan_hTask = esos_GetTaskHandle( CANFactory );
    if ( ESOS_TASK_MAILBOX_GOT_AT_LEAST_DATA_BYTES( __esos_ecan_hTask, __MAIL_MSG_HEADER_LEN + sizeof( uint8_t ) * u8_len + sizeof( uint16_t ) ) ) {
        *( ( uint16_t* ) &__esos_mail_msgBuf[0] ) = u16_can_id;
        memcpy( &__esos_mail_msgBuf[ sizeof( uint16_t ) ], pu8_msg, u8_len );
        ESOS_TASK_MAKE_MSG_AUINT8 ( __esos_ecan_mailMsg, __esos_mail_msgBuf, sizeof( uint8_t ) * u8_len + sizeof( uint16_t ) );
        ESOS_TASK_SEND_MESSAGE( __esos_ecan_hTask, &__esos_ecan_mailMsg );
    }

    return;
}*/

ESOS_USER_TASK ( CANFactory ) {
    uint8_t             u8_i;
    static uint8_t      au8_msgBuf[10], u8_y;
    static MAILMESSAGE  st_Msg;
	static uint8_t          u8_len;
	static uint16_t         u16_rx_buff_id, u16_msg_id;
	static ESOS_TASK_HANDLE hTask;
	//__esos_ecan_hw_config_ecan();
	ESOS_TASK_BEGIN();
	//DISABLE_DEBUG_MODE();
	//__esos_ecan_hw_config_ecan();
    while ( TRUE ) {
        /* Check to see if any CAN messages have arrived.
         * If so, parse and send mail to the appropriate task.
         * If task does not have room in their task mailbox, discard message.
         */
		//ESOS_TASK_WAIT_TICKS(1000);
        if ( __ESOS_ECAN_HW_ARE_MESSAGES_WAITING() ) {


			_DMA1IF = 0; //Rx accepted
            u16_rx_buff_id = __ESOS_ECAN_HW_GET_BUFFER_ID();
            u16_msg_id = __esos_ecan_msgBuf[ u16_rx_buff_id ].w0.SID;
            u8_len = __esos_ecan_msgBuf[ u16_rx_buff_id ].w2.DLC;
																									//(u16_msg_id & canfactory_clients[ u8_i ].u16_idMask ) == canfactory_clients[ u8_i ].u16_canID)
																									//esos_ecan_mask_check (uint16_t u16_subscribed, uint16_t u16_recieved, uint16_t u16_mask)
            for ( u8_i = 0; u8_i < u8_numCANFactoryClients; ++u8_i ) {
                if ( ( canfactory_clients[ u8_i ].m_idMaskControl == MASKCONTROL_FIELD_NONZERO    && ( esos_ecan_mask_check(canfactory_clients[u8_i].u16_canID, u16_msg_id, canfactory_clients[u8_i].u16_idMask)) ) |
                     ( canfactory_clients[ u8_i ].m_idMaskControl == MASKCONTROL_EQUAL            && ( u16_msg_id == canfactory_clients[ u8_i ].u16_idMask ) )
                   ) {
                    hTask = esos_GetTaskHandle( canfactory_clients[ u8_i ].pf_task );

                    if ( ESOS_TASK_MAILBOX_GOT_AT_LEAST_DATA_BYTES( hTask, __MAIL_MSG_HEADER_LEN + sizeof( uint8_t ) * u8_len + sizeof( uint16_t ) ) ) {
                        uint8_t u8_count;
                        *( ( uint16_t* ) &au8_msgBuf[0] ) = u16_msg_id;
                        for ( u8_count = 0; u8_count < u8_len; ++u8_count ) {
                            au8_msgBuf[ sizeof( uint16_t ) + u8_count ] = __esos_ecan_msgBuf[ u16_rx_buff_id ].data.u8[u8_count];

                        }
                        ESOS_TASK_MAKE_MSG_AUINT8( st_Msg, au8_msgBuf, sizeof( uint8_t ) * u8_len + sizeof( uint16_t ) );
                        ESOS_TASK_SEND_MESSAGE( hTask, &st_Msg );

						if (CHECK_DEBUG_MODE_ENABLED()){
							ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
							ESOS_TASK_WAIT_ON_SEND_STRING("Rx Buff: ");
							ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u16_rx_buff_id);
							ESOS_TASK_WAIT_ON_SEND_STRING(" ");
							ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
						}

                    }
                }
				else{
					if (CHECK_DEBUG_MODE_ENABLED()){
						ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
						ESOS_TASK_WAIT_ON_SEND_STRING("Rx Buff: ");
						ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u16_rx_buff_id);
						ESOS_TASK_WAIT_ON_SEND_STRING(" ");
						ESOS_TASK_WAIT_ON_SEND_STRING("Message ID: ");
						ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING(u16_msg_id);
						ESOS_TASK_WAIT_ON_SEND_STRING(" rejected by acceptance filter.\n");
						ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
					}
				}
            }
            __esos_ecan_hw_mark_message_received( u16_rx_buff_id );
        }
		else if (!__ESOS_ECAN_HW_ARE_MESSAGES_WAITING()){
			//printf("Message ID 0x%X rejected by acceptance filter.\n", u16_msg_id);
			/*ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_STRING("Rx Buff: ");
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(u16_rx_buff_id);
			ESOS_TASK_WAIT_ON_SEND_STRING(" ");
			ESOS_TASK_SEND_STRING("Message ID: ");
			ESOS_TASK_WAIT_ON_SEND_UINT12_AS_HEX_STRING(u16_msg_id);
			ESOS_TASK_SEND_STRING(" rejected by acceptance filter.\n");
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();*/
		}
        /* Check to see if any user tasks have sent mail containing a CAN
         * message to transmit.
         * If so, parse the incoming mail message(s), creating and sending the
         * CAN message(s).
         */
        while ( ESOS_TASK_IVE_GOT_MAIL() ) {
            static ECANMSG         temp_Msg;
            static uint8_t         u8_len, u8_payload_len;
            static uint16_t u16_can_id;
			//printf("GOT MESSAGE");
            ESOS_TASK_GET_NEXT_MESSAGE( &st_Msg );
            u8_len = ESOS_GET_PMSG_DATA_LENGTH( ( &st_Msg ) );
            u16_can_id = st_Msg.au16_Contents[0];
            u8_payload_len = u8_len - 2;
            u8_y = 0;
            for ( u8_i = 0; u8_i < u8_payload_len; ++u8_i ) {
                temp_Msg.data.u8[ u8_i ] = st_Msg.au8_Contents[ u8_i + 2 ];
                //ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                //ESOS_TASK_WAIT_ON_SEND_STRING("data: ");
                //ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(st_Msg.au8_Contents[u8_i + 2]);
                //ESOS_TASK_WAIT_ON_SEND_STRING("\n");
                //ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            }
			/*ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(temp_Msg.data.u8[1]);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();*/

            __esos_ecan_hw_format_and_send( &temp_Msg, u16_can_id, u8_payload_len );
            ESOS_TASK_WAIT_WHILE( __ESOS_ECAN_HW_TX_IN_PROGRESS() ); //It freezes here.
			_DMA0IF = 1;

        }

        ESOS_TASK_YIELD();
    }

    ESOS_TASK_END();
}

void esos_ecan_canfactory_subscribe(ESOS_TASK_HANDLE pst_Task, uint16_t u16_can_id, uint16_t u16_mask, maskcontrol_t m_mask_control) {
    canfactory_clients[ u8_numCANFactoryClients ].pf_task = (*pst_Task).pfn;
    canfactory_clients[ u8_numCANFactoryClients ].u16_canID = u16_can_id;
    canfactory_clients[ u8_numCANFactoryClients ].u16_idMask = u16_mask;
    canfactory_clients[ u8_numCANFactoryClients++ ].m_idMaskControl = m_mask_control;

    return;
}

void esos_ecan_canfactory_unsubscribe ( uint8_t (*pst_Task) (ESOS_TASK_HANDLE), uint16_t u16_can_id, uint16_t u16_mask, maskcontrol_t m_mask_control ) {
    uint8_t u8_i;

    for ( u8_i = 0; u8_i < u8_numCANFactoryClients; ++u8_i ) {
        if ( canfactory_clients[ u8_i ].pf_task == pst_Task && canfactory_clients[ u8_i ].u16_canID == u16_can_id && canfactory_clients[ u8_i ].u16_idMask == u16_mask && canfactory_clients[ u8_i ].m_idMaskControl == m_mask_control ) {
            uint8_t u8_n;

            for ( u8_n = u8_i; u8_n < u8_numCANFactoryClients - 1; ++u8_n ) {
                canfactory_clients[ u8_n ] = canfactory_clients[ u8_n + 1 ];
            }
            --u8_numCANFactoryClients;
            break;
        }
    }

    return;
}

BOOL esos_ecan_mask_check (uint16_t u16_subscribed, uint16_t u16_recieved, uint16_t u16_mask) {
    int8_t i = 15;
    uint8_t u8_errorCount = 0;
    for (i = 15; i >= 0; i--) {
        if (CHECK_BIT(u16_mask,i)) {
            u8_errorCount += (CHECK_BIT(u16_subscribed,i) ^ CHECK_BIT(u16_recieved,i));
        }
    }
    if (u8_errorCount == 0) {
      return TRUE;
    }
    else {
      return FALSE;
    }
}
