/*
**************************************************************************
*                                                                        *
*                 ____                ___ _   _____                      *
*                / __ \___  ___ ___  / _ | | / / _ \____                 *
*               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
*               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
*                   /_/                                                  *
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code(s) named :                       *
*             OpenTx - https://github.com/opentx/opentx                  *
*             Deviation - https://www.deviationtx.com/                   *
*                                                                        *
*                Only AVR code here for visibility ;-)                   *
*                                                                        *
*   OpenAVRc is free software: you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by *
*   the Free Software Foundation, either version 2 of the License, or    *
*   (at your option) any later version.                                  *
*                                                                        *
*   OpenAVRc is distributed in the hope that it will be useful,          *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
*   GNU General Public License for more details.                         *
*                                                                        *
*       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
*                                                                        *
**************************************************************************
*/

/*
Note: This HwSerial driver/module by RC-Navy is mainly based on the arduino HardwareSerial.
      This lightweight version allows dimensionning independently (statically) all the Tx/Rx buffers.
*/

#ifdef ARDUINO
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/atomic.h>
#else
#include "OpenAVRc.h"
#endif

#ifdef SIMU
  #define SIMUSENDHWSBYTE(x) SendHwSByte(x)
#else
  #define SIMUSENDHWSBYTE(x)
#endif

#include "HwSerial.h"

// this next line disables the entire HwSerial.cpp,
// this is so I can support Attiny series and any other chip without a uart
#if (HW_SERIAL0_SUPPORT || HW_SERIAL1_SUPPORT || HW_SERIAL2_SUPPORT || HW_SERIAL3_SUPPORT)

typedef struct{
  volatile uint8_t * const ubrrh;
  volatile uint8_t * const ubrrl;
  volatile uint8_t * const ucsra;
  volatile uint8_t * const ucsrb;
  volatile uint8_t * const ucsrc;
  volatile uint8_t * const udr;
}UartRegSt_t;

#define GET_UART_REG_UBRRH(UartIdx) (uint8_t * const)pgm_read_word(&UartProp[(UartIdx)].Reg.ubrrh)
#define GET_UART_REG_UBRRL(UartIdx) (uint8_t * const)pgm_read_word(&UartProp[(UartIdx)].Reg.ubrrl)
#define GET_UART_REG_UCSRA(UartIdx) (uint8_t * const)pgm_read_word(&UartProp[(UartIdx)].Reg.ucsra)
#define GET_UART_REG_UCSRB(UartIdx) (uint8_t * const)pgm_read_word(&UartProp[(UartIdx)].Reg.ucsrb)
#define GET_UART_REG_UCSRC(UartIdx) (uint8_t * const)pgm_read_word(&UartProp[(UartIdx)].Reg.ucsrc)
#define GET_UART_REG_UDR(UartIdx)   (uint8_t * const)pgm_read_word(&UartProp[(UartIdx)].Reg.udr)

typedef struct{
  UartRegSt_t  Reg;
  uint8_t     *TxBuf;
  uint8_t     *RxBuf;
  uint8_t      TxBufSize;
  uint8_t      RxBufSize;
}UartPropSt_t;

#define GET_UART_TX_BUF(UartIdx)         (uint8_t *)pgm_read_word(&UartProp[(UartIdx)].TxBuf)
#define GET_UART_RX_BUF(UartIdx)         (uint8_t *)pgm_read_word(&UartProp[(UartIdx)].RxBuf)
#define GET_UART_TX_BUF_SIZE(UartIdx)    (uint8_t)  pgm_read_byte(&UartProp[(UartIdx)].TxBufSize)
#define GET_UART_RX_BUF_SIZE(UartIdx)    (uint8_t)  pgm_read_byte(&UartProp[(UartIdx)].RxBufSize)

#define DECLARE_UART_FIFO(UartIdx, Dir, Size) uint8_t Serial##UartIdx##Dir##Buf[(Size)]

#define UART_PROPERTIES(UartIdx)        {{&UBRR##UartIdx##H, &UBRR##UartIdx##L, &UCSR##UartIdx##A, &UCSR##UartIdx##B, &UCSR##UartIdx##C, &UDR##UartIdx}, Serial##UartIdx##TxBuf, Serial##UartIdx##RxBuf, HW_SERIAL##UartIdx##_TX_FIFO_SIZE, HW_SERIAL##UartIdx##_RX_FIFO_SIZE}

/* Declare uart Fifos even if the Uart is not used (to keep correct location vs index)       */
/* If an Uart is not used, the Tx and Rx fifo sizes are automatically set to 0 in HwSerial.h */
DECLARE_UART_FIFO(0, Tx, HW_SERIAL0_TX_FIFO_SIZE);
DECLARE_UART_FIFO(0, Rx, HW_SERIAL0_RX_FIFO_SIZE);
DECLARE_UART_FIFO(1, Tx, HW_SERIAL1_TX_FIFO_SIZE);
DECLARE_UART_FIFO(1, Rx, HW_SERIAL1_RX_FIFO_SIZE);
DECLARE_UART_FIFO(2, Tx, HW_SERIAL2_TX_FIFO_SIZE);
DECLARE_UART_FIFO(2, Rx, HW_SERIAL2_RX_FIFO_SIZE);
DECLARE_UART_FIFO(3, Tx, HW_SERIAL3_TX_FIFO_SIZE);
DECLARE_UART_FIFO(3, Rx, HW_SERIAL3_RX_FIFO_SIZE);

UartPropSt_t const UartProp[] PROGMEM = {UART_PROPERTIES(0), UART_PROPERTIES(1), UART_PROPERTIES(2), UART_PROPERTIES(3)};

#if (HW_SERIAL0_TX_SUPPORT == ISR_CUSTOM)
#define SERIAL0_TX_ISR() Serial0_customTxIsr()
#else
#define SERIAL0_TX_ISR() Serial0._tx_udr_empty_irq()
#endif

#if (HW_SERIAL1_TX_SUPPORT == ISR_CUSTOM)
#define SERIAL1_TX_ISR() Serial1_customTxIsr()
#else
#define SERIAL1_TX_ISR() Serial1._tx_udr_empty_irq()
#endif

#if (HW_SERIAL2_TX_SUPPORT == ISR_CUSTOM)
#define SERIAL2_TX_ISR() Serial2_customTxIsr()
#else
#define SERIAL2_TX_ISR() Serial2._tx_udr_empty_irq()
#endif

#if (HW_SERIAL3_TX_SUPPORT == ISR_CUSTOM)
#define SERIAL3_TX_ISR() Serial3_customTxIsr()
#else
#define SERIAL3_TX_ISR() Serial3._tx_udr_empty_irq()
#endif


#define DECLARE_SERIAL_TX_ISR(UartIdx)\
ISR(USART##UartIdx##_UDRE_vect)\
{\
  SERIAL##UartIdx##_TX_ISR();\
}

#if (HW_SERIAL0_RX_SUPPORT == ISR_CUSTOM)
#define SERIAL0_RX_ISR() Serial0_customRxIsr()
#else
#define SERIAL0_RX_ISR() Serial0._rx_complete_irq()
#endif

#if (HW_SERIAL1_RX_SUPPORT == ISR_CUSTOM)
#define SERIAL1_RX_ISR() Serial1_customRxIsr()
#else
#define SERIAL1_RX_ISR() Serial1._rx_complete_irq()
#endif

#if (HW_SERIAL2_RX_SUPPORT == ISR_CUSTOM)
#define SERIAL2_RX_ISR() Serial2_customRxIsr()
#else
#define SERIAL2_RX_ISR() Serial2._rx_complete_irq()
#endif

#if (HW_SERIAL3_RX_SUPPORT == ISR_CUSTOM)
#define SERIAL3_RX_ISR() Serial3_customRxIsr()
#else
#define SERIAL3_RX_ISR() Serial3._rx_complete_irq()
#endif

#define DECLARE_SERIAL_RX_ISR(UartIdx)\
ISR(USART##UartIdx##_RX_vect)\
{\
  SERIAL##UartIdx##_RX_ISR();\
}

#if (HW_SERIAL0_SUPPORT != 0)
  HwSerial Serial0(HW_SERIAL_INIT_OBJ(0));
  #if (HW_SERIAL0_TX_SUPPORT != ISR_NONE)
  DECLARE_SERIAL_TX_ISR(0)
  #endif
  #if (HW_SERIAL0_RX_SUPPORT != ISR_NONE)
  DECLARE_SERIAL_RX_ISR(0)
  #endif
#endif

#if (HW_SERIAL1_SUPPORT != 0)
  HwSerial Serial1(HW_SERIAL_INIT_OBJ(1));
  #if (HW_SERIAL1_TX_SUPPORT != ISR_NONE)
  DECLARE_SERIAL_TX_ISR(1)
  #endif
  #if (HW_SERIAL1_RX_SUPPORT != ISR_NONE)
  DECLARE_SERIAL_RX_ISR(1)
  #endif
#endif

#if (HW_SERIAL2_SUPPORT != 0)
  HwSerial Serial2(HW_SERIAL_INIT_OBJ(2));
  #if (HW_SERIAL2_TX_SUPPORT != ISR_NONE)
  DECLARE_SERIAL_TX_ISR(2)
  #endif
  #if (HW_SERIAL2_RX_SUPPORT != ISR_NONE)
  DECLARE_SERIAL_RX_ISR(2)
  #endif
#endif

#if (HW_SERIAL3_SUPPORT != 0)
  HwSerial Serial3(HW_SERIAL_INIT_OBJ(3));
  #if (HW_SERIAL3_TX_SUPPORT != ISR_NONE)
  DECLARE_SERIAL_TX_ISR(3)
  #endif
  #if (HW_SERIAL3_RX_SUPPORT != ISR_NONE)
  DECLARE_SERIAL_RX_ISR(3)
  #endif
#endif

#define sbi(port, bit)    (port) |=  (1 << (bit))
#define cbi(port, bit)    (port) &= ~(1 << (bit))

// Actual interrupt handlers //////////////////////////////////////////////////////////////
#if (HW_SERIAL_RX_NORMAL_ISR != 0)
void HwSerial::_rx_complete_irq(void)
{
  volatile uint8_t * const ucsra          = GET_UART_REG_UCSRA(_hwS.Idx);
  volatile uint8_t *       rx_buffer      = GET_UART_RX_BUF(_hwS.Idx);
  volatile uint8_t         rx_buffer_size = GET_UART_RX_BUF_SIZE(_hwS.Idx);
  volatile uint8_t * const udr            = GET_UART_REG_UDR(_hwS.Idx);

  if (bit_is_clear(*ucsra, UPE0)) {
    // No Parity error, read byte and store it in the buffer if there is
    // room
    volatile uint8_t c = *udr;
    rx_buffer_index_t i = (unsigned int)(_rx_buffer_head + 1) % rx_buffer_size;

    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if (i != _rx_buffer_tail) {
      rx_buffer[_rx_buffer_head] = c;
      _rx_buffer_head = i;
    }
  } else {
    // Parity error, read byte but discard it
    *udr;
  };
}
#endif // HW_SERIAL_RX_NORMAL_ISR

#if (HW_SERIAL_TX_NORMAL_ISR != 0)
void HwSerial::_tx_udr_empty_irq(void)
{
  // If interrupts are enabled, there must be more data in the output
  // buffer. Send the next byte
  volatile uint8_t * const udr            = GET_UART_REG_UDR(_hwS.Idx);
  volatile uint8_t * const ucsra          = GET_UART_REG_UCSRA(_hwS.Idx);
  volatile uint8_t * const ucsrb          = GET_UART_REG_UCSRB(_hwS.Idx);
  volatile uint8_t *       tx_buffer      = GET_UART_TX_BUF(_hwS.Idx);
  volatile uint8_t         tx_buffer_size = GET_UART_TX_BUF_SIZE(_hwS.Idx);
  volatile uint8_t         c              = tx_buffer[_tx_buffer_tail];

  _tx_buffer_tail = (_tx_buffer_tail + 1) % tx_buffer_size;
  *udr = c;
  // clear the TXC bit -- "can be cleared by writing a one to its bit
  // location". This makes sure flush() won't return until the bytes
  // actually got written. Other r/w bits are preserved, and zeroes
  // written to the rest.
#ifdef MPCM0
  *ucsra = ((*ucsra) & ((1 << U2X0) | (1 << MPCM0))) | (1 << TXC0);
#else
  *ucsra = ((*ucsra) & ((1 << U2X0) | (1 << TXC0)));
#endif
  if (_tx_buffer_head == _tx_buffer_tail) {
    // Buffer empty, so disable interrupts
    cbi(*ucsrb, UDRIE0);
  }
}
#endif // HW_SERIAL_TX_NORMAL_ISR

// Constructor //////////////////////////////////////////////////////////////

HwSerial::HwSerial(uint8_t HwSerialIdx, uint8_t UseTx, uint8_t UseRx)
{
  _hwS.Idx        = HwSerialIdx;
  _hwS.UseTx      = UseTx;
  _hwS.UseRx      = UseRx;
  #if (HW_SERIAL_TX_FIFO_SIZE != 0)
  _tx_buffer_head = 0;
  _tx_buffer_tail = 0;
  #endif
  #if (HW_SERIAL_RX_FIFO_SIZE != 0)
  _rx_buffer_head = 0;
  _rx_buffer_tail = 0;
  #endif
}

// Public Methods //////////////////////////////////////////////////////////////

void HwSerial::init(unsigned long baud, uint8_t config)
{
  volatile uint8_t * const ucsra          = GET_UART_REG_UCSRA(_hwS.Idx);
  volatile uint8_t * const ubrrh          = GET_UART_REG_UBRRH(_hwS.Idx);
  volatile uint8_t * const ubrrl          = GET_UART_REG_UBRRL(_hwS.Idx);
  volatile uint8_t * const ucsrc          = GET_UART_REG_UCSRC(_hwS.Idx);
  volatile uint8_t * const ucsrb          = GET_UART_REG_UCSRB(_hwS.Idx);
  // Try u2x mode first
  uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
  *ucsra = 1 << U2X0;

  // hardcoded exception for 57600 for compatibility with the bootloader
  // shipped with the Duemilanove and previous boards and the firmware
  // on the 8U2 on the Uno and Mega 2560. Also, The baud_setting cannot
  // be > 4095, so switch back to non-u2x mode if the baud rate is too
  // low.
  if (((F_CPU == 16000000UL) && (baud == 57600)) || (baud_setting >4095))
  {
    *ucsra = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }

  // assign the baud_setting, a.k.a. ubrr (USART Baud Rate Register)
  *ubrrh = baud_setting >> 8;
  *ubrrl = baud_setting;

  _hwS.Written = 0;

  //set the data bits, parity, and stop bits
#if defined(__AVR_ATmega8__)
  config |= 0x80; // select UCSRC register (shared with UBRRH)
#endif
  *ucsrc = config;

  if(_hwS.UseRx)  sbi(*ucsrb, RXEN0);
  if(_hwS.UseTx)  sbi(*ucsrb, TXEN0);

  if(_hwS.UseRx)
  {
    sbi(*ucsrb, RXCIE0);
    cbi(*ucsrb, UDRIE0);
  }
}

#if (HW_SERIAL_TX_NORMAL_ISR != 0)
size_t HwSerial::write(uint8_t c)
{
  volatile uint8_t * const ucsra          = GET_UART_REG_UCSRA(_hwS.Idx);
  volatile uint8_t * const udr            = GET_UART_REG_UDR(_hwS.Idx);
  volatile uint8_t *       tx_buffer      = GET_UART_TX_BUF(_hwS.Idx);
  volatile uint8_t         tx_buffer_size = GET_UART_TX_BUF_SIZE(_hwS.Idx);
  volatile uint8_t * const ucsrb          = GET_UART_REG_UCSRB(_hwS.Idx);

  _hwS.Written = 1;
  // If the buffer and the data register is empty, just write the byte
  // to the data register and be done. This shortcut helps
  // significantly improve the effective datarate at high (>
  // 500kbit/s) bitrates, where interrupt overhead becomes a slowdown.
  if (_tx_buffer_head == _tx_buffer_tail && bit_is_set(*ucsra, UDRE0)) {
    // If TXC is cleared before writing UDR and the previous byte
    // completes before writing to UDR, TXC will be set but a byte
    // is still being transmitted causing flush() to return too soon.
    // So writing UDR must happen first.
    // Writing UDR and clearing TC must be done atomically, otherwise
    // interrupts might delay the TXC clear so the byte written to UDR
    // is transmitted (setting TXC) before clearing TXC. Then TXC will
    // be cleared when no bytes are left, causing flush() to hang
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      *udr = c;
#ifdef MPCM0
      *ucsra = ((*ucsra) & ((1 << U2X0) | (1 << MPCM0))) | (1 << TXC0);
#else
      *ucsra = ((*ucsra) & ((1 << U2X0) | (1 << TXC0)));
#endif
    }
    return 1;
  }
  tx_buffer_index_t i = (_tx_buffer_head + 1) % tx_buffer_size;

  // If the output buffer is full, there's nothing for it other than to
  // wait for the interrupt handler to empty it a bit
  while (i == _tx_buffer_tail) {
    if (bit_is_clear(SREG, SREG_I)) {
      // Interrupts are disabled, so we'll have to poll the data
      // register empty flag ourselves. If it is set, pretend an
      // interrupt has happened and call the handler to free up
      // space for us.
      if SIMU_UNLOCK_MACRO_TRUE((bit_is_set(*ucsra, UDRE0)))
	      _tx_udr_empty_irq();
    } else {
      // nop, the interrupt handler will free up space for us
    }
  }
  tx_buffer[_tx_buffer_head] = c;
  // make atomic to prevent execution of ISR between setting the
  // head pointer and setting the interrupt flag resulting in buffer
  // retransmission
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _tx_buffer_head = i;
    sbi(*ucsrb, UDRIE0);
  }
  SIMUSENDHWSBYTE(c); // Send char to simu
  return 1;
}
#endif // HW_SERIAL_TX_NORMAL_ISR

#if (HW_SERIAL_ENABLE_TX_SUPPORT != 0)
void HwSerial::enableTx(uint8_t On)
{
  volatile uint8_t * const ucsrb = GET_UART_REG_UCSRB(_hwS.Idx);
  if(On)
  {
    sbi(*ucsrb, TXEN0);
  }
  else
  {
    cbi(*ucsrb, UDRIE0);// disable Data Register Empty Interrupt
    cbi(*ucsrb, TXEN0);
  }
}

void HwSerial::resumeTx(void)
{
  volatile uint8_t * const ucsrb = GET_UART_REG_UCSRB(_hwS.Idx);
  sbi(*ucsrb, UDRIE0); // enable Data Register Empty Interrupt
}
#endif // HW_SERIAL_ENABLE_TX_SUPPORT

uint8_t HwSerial::available(void)
{
#if (HW_SERIAL_RX_SUPPORT != 0)
  volatile uint8_t rx_buffer_size = GET_UART_RX_BUF_SIZE(_hwS.Idx);
  return ((uint8_t)(rx_buffer_size + _rx_buffer_head - _rx_buffer_tail)) % rx_buffer_size;
#else
  return 0;
#endif
}

uint8_t HwSerial::read(void)
{
#if (HW_SERIAL_RX_SUPPORT != 0)
  volatile uint8_t *rx_buffer      = GET_UART_RX_BUF(_hwS.Idx);
  volatile uint8_t  rx_buffer_size = GET_UART_RX_BUF_SIZE(_hwS.Idx);
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    uint8_t c = rx_buffer[_rx_buffer_tail];
    _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) % rx_buffer_size;
    return c;
  }
#else
  return -1;
#endif
}

#if (HW_SERIAL_ENABLE_RX_SUPPORT != 0)
void HwSerial::enableRx(uint8_t On)
{
  volatile uint8_t * const ucsra = GET_UART_REG_UCSRA(_hwS.Idx);
  volatile uint8_t * const ucsrb = GET_UART_REG_UCSRB(_hwS.Idx);
  volatile uint8_t * const udr   = GET_UART_REG_UDR(_hwS.Idx);

  if(On)
  {
    sbi(*ucsrb, RXEN0); // enable RX
    sbi(*ucsrb, RXCIE0);// enable Interrupt
    while (*ucsra & (1 << RXCIE0)) *udr; // Flush RX buffer.
  }
  else
  {
    cbi(*ucsrb, RXCIE0);// disable Data Register Empty Interrupt
    cbi(*ucsrb, RXEN0);
  }
}
#endif // HW_SERIAL_ENABLE_RX_SUPPORT

#endif // whole file
