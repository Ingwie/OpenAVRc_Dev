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

#include "OpenAVRc.h"

#ifdef SIMU
  #define SIMUSENDHWSBYTE(x) SendHwSByte(x); return 1
#else
  #define SIMUSENDHWSBYTE(x)
#endif

#include "Serial1.h"

#define TLM_USART1 1

#define SERIAL1_TX_ISR() Serial1._tx_udr_empty_irq()

#define DECLARE_SERIAL_TX_ISR(UartIdx)\
ISR(USART##UartIdx##_UDRE_vect)\
{\
  SERIAL##UartIdx##_TX_ISR();\
}

#define SERIAL1_RX_ISR() Serial1._rx_complete_irq()

#define DECLARE_SERIAL_RX_ISR(UartIdx)\
ISR(USART##UartIdx##_RX_vect)\
{\
  SERIAL##UartIdx##_RX_ISR();\
}


  HwSerial Serial1;
  DECLARE_SERIAL_TX_ISR(1)
  DECLARE_SERIAL_RX_ISR(1)


#define sbi(port, bit)    (port) |=  (1 << (bit))
#define cbi(port, bit)    (port) &= ~(1 << (bit))

// Actual interrupt handlers //////////////////////////////////////////////////////////////
void HwSerial::_rx_complete_irq(void)
{
  if (bit_is_clear(UCSRA_N(TLM_USART1), UPE_N(TLM_USART1))) {
    // No Parity error, read byte and store it in the buffer if there is
    // room
    uint8_t c = UDR_N(TLM_USART1);
    rx_buffer_index_t i = (_rx_buffer_head + 1) % HW_SERIAL1_RX_FIFO_SIZE;

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
    UDR_N(TLM_USART1);
  };
}

void HwSerial::_tx_udr_empty_irq(void)
{
  if (_tx_buffer_head != _tx_buffer_tail) {
  // there is data in the buffer. Send the next byte
  ++_tx_buffer_tail %= HW_SERIAL1_TX_FIFO_SIZE;
  UDR_N(TLM_USART1) = tx_buffer[_tx_buffer_tail];
  } else {
    // Buffer empty, so disable interrupts
    cbi(UCSRB_N(TLM_USART1), UDRIE_N(TLM_USART1));
  }
}

// Constructor //////////////////////////////////////////////////////////////

HwSerial::HwSerial()
{
  _tx_buffer_head = 0;
  _tx_buffer_tail = 0;
  _rx_buffer_head = 0;
  _rx_buffer_tail = 0;
}

// Public Methods //////////////////////////////////////////////////////////////

void HwSerial::init(unsigned long baud, uint8_t config)
{
  // Try u2x mode first
  uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
  UCSRA_N(TLM_USART1) = (1 << U2X_N(TLM_USART1));

  // hardcoded exception for 57600 for compatibility with the bootloader
  // shipped with the Duemilanove and previous boards and the firmware
  // on the 8U2 on the Uno and Mega 2560. Also, The baud_setting cannot
  // be > 4095, so switch back to non-u2x mode if the baud rate is too
  // low.
  if (((F_CPU == 16000000UL) && (baud == 57600)) || (baud_setting >4095))
  {
    UCSRA_N(TLM_USART1) = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }

  // assign the baud_setting, a.k.a. ubrr (USART Baud Rate Register)
  UBRRH_N(TLM_USART1)= baud_setting >> 8;
  UBRRL_N(TLM_USART1) = baud_setting;

  //set the data bits, parity, and stop bits
#if defined(__AVR_ATmega8__)
  conf ig |= 0x80; // select UCSRC register (shared with UBRRH)
#endif
  UCSRC_N(TLM_USART0) = config;

  sbi(UCSRB_N(TLM_USART1), RXEN_N(TLM_USART1));
  sbi(UCSRB_N(TLM_USART1), TXEN_N(TLM_USART1));
  sbi(UCSRB_N(TLM_USART1), RXCIE_N(TLM_USART1));
  cbi(UCSRB_N(TLM_USART1), UDRIE_N(TLM_USART1));
}

size_t HwSerial::write(uint8_t c)
{
  SIMUSENDHWSBYTE(c); // Send char to simu

  uint8_t rxIsrIsNotActive = bit_is_clear(UCSRB_N(TLM_USART1),UDRIE_N(TLM_USART1));

  /*if ((_tx_buffer_head == _tx_buffer_tail) && rxIsrIsNotActive) // if buffer is empty and isr is off
  {
    UDR_N(TLM_USART1) = c; // just send data ...
    sbi(UCSRB_N(TLM_USART1), UDRIE_N(TLM_USART1)); // enable isr and return
    return 1;
  }*/

  uint8_t newpos = (_tx_buffer_head +1) % HW_SERIAL1_TX_FIFO_SIZE;

  while (_tx_buffer_tail == newpos) {_delay_us(10);} // Wait if buffer is full (I realy don't love that !)
 tx_buffer[newpos] = c; // load the buffer
 _tx_buffer_head = newpos;

 if (rxIsrIsNotActive) // if the driver is not running
  {
   ++_tx_buffer_tail %= HW_SERIAL1_TX_FIFO_SIZE;
   UDR_N(TLM_USART1) = tx_buffer[_tx_buffer_tail];
   sbi(UCSRB_N(TLM_USART1), UDRIE_N(TLM_USART1));
  }
 return 1;
}

void HwSerial::enableTx(uint8_t On)
{
  if(On)
  {
    sbi(UCSRB_N(TLM_USART1), TXEN_N(TLM_USART1));
  }
  else
  {
    cbi(UCSRB_N(TLM_USART1), UDRIE_N(TLM_USART1));// disable Data Register Empty Interrupt
    cbi(UCSRB_N(TLM_USART1), TXEN_N(TLM_USART1));
  }
}

void HwSerial::resumeTx(void)
{
  sbi(UCSRB_N(TLM_USART1), UDRIE_N(TLM_USART1)); // enable Data Register Empty Interrupt
}

uint8_t HwSerial::available(void)
{
  return ((uint8_t)(HW_SERIAL1_RX_FIFO_SIZE + _rx_buffer_head - _rx_buffer_tail)) % HW_SERIAL1_RX_FIFO_SIZE;
}

uint8_t HwSerial::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    uint8_t c = rx_buffer[_rx_buffer_tail];
    _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) % HW_SERIAL1_RX_FIFO_SIZE;
    return c;
  }
}

void HwSerial::enableRx(uint8_t On)
{
  if(On)
  {
    sbi(UCSRB_N(TLM_USART1), RXEN_N(TLM_USART1)); // enable RX
    sbi(UCSRB_N(TLM_USART1), RXCIE_N(TLM_USART1));// enable Interrupt
    while (UCSRA_N(TLM_USART1) & (1 << RXCIE_N(TLM_USART1))) UDR_N(TLM_USART1); // Flush RX buffer.
  }
  else
  {
    cbi(UCSRB_N(TLM_USART1), RXCIE_N(TLM_USART1));// disable Data Register Empty Interrupt
    cbi(UCSRB_N(TLM_USART1), RXEN_N(TLM_USART1));
  }
}
