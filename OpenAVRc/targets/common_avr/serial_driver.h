/*
 *************************************************************
 *                      OpenAVRc                           *
 *                                                           *
 *             -> Build your DIY MEGA 2560 TX                *
 *                                                           *
 *      Based on code named                                  *
 *      OpenTx - https://github.com/opentx/opentx            *
 *                                                           *
 *         Only avr code here for lisibility ;-)             *
 *                                                           *
 *  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
 *                                                           *
 *************************************************************
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "../../OpenAVRc.h"


typedef enum serial_tx_state_ {
	TX_STATE_EMPTY = 0, //
	TX_STATE_READY, //
	TX_STATE_BUSY
} serial_tx_state_t;
extern serial_tx_state_t serialTxState;

//! \brief Baudrate selection.
enum SERIAL_BAUDS {
	BAUDS_4800 = 0,
	BAUDS_9600,
	BAUDS_14400,
	BAUDS_19200,
	BAUDS_38400,
	BAUDS_57600,
	BAUDS_76800
};

//! \brief Definition of baudrate settings item choices.
typedef void (*SerialFuncP)(uint8_t event);

extern SerialFuncP RXHandler;

#define MAX_TX_BUFFER 32
//#if 0
extern uint8_t serialTxBuffer[MAX_TX_BUFFER]; // 32 characters
extern uint8_t serialTxBufferCount;
extern uint8_t * ptrTxISR;
//#endif

void SERIAL_Init(void);
//void SERIAL_transmitBuffer(uint8_t len);
extern void SERIAL_start_uart_send();
extern void SERIAL_end_uart_send();
extern void SERIAL_send_uart_bytes(uint8_t * buf, uint16_t len);

void SERIAL_startTX(void);
#define IS_TX_BUSY (serialTxState!=TX_STATE_EMPTY)

#endif
