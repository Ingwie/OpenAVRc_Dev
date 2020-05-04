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
      Ingwie : Modded (All is static now) to increase performances (More than X2 speed)
*/

#ifndef HW_SERIAL_H
#define HW_SERIAL_H

#include "Stream.h"
#if defined(SIMU)
 #include "targets/simu/simu_interface.h"
#endif

#if defined(U_CLI) || defined(TINY_DBG_UART_BT)
#if defined(XMODEM)
#define S1_TX_FF_SZ                  40
#define S1_RX_FF_SZ                  132//xmodem packet size
#else
#define S1_TX_FF_SZ                  16
#define S1_RX_FF_SZ                  128//nedeed by BT +INQ
#endif
#define HW_SERIAL1_TX_FIFO_SIZE      S1_TX_FF_SZ // Define here the fifo size (SHALL be a power of 2)
#define HW_SERIAL1_RX_FIFO_SIZE      S1_RX_FF_SZ // Define here the fifo size (SHALL be a power of 2)
#endif

typedef uint8_t tx_buffer_index_t;
typedef uint8_t rx_buffer_index_t;

// Define config for Serial.init(baud, config);
#define SERIAL_5N1 0x00
#define SERIAL_6N1 0x02
#define SERIAL_7N1 0x04
#define SERIAL_8N1 0x06
#define SERIAL_5N2 0x08
#define SERIAL_6N2 0x0A
#define SERIAL_7N2 0x0C
#define SERIAL_8N2 0x0E
#define SERIAL_5E1 0x20
#define SERIAL_6E1 0x22
#define SERIAL_7E1 0x24
#define SERIAL_8E1 0x26
#define SERIAL_5E2 0x28
#define SERIAL_6E2 0x2A
#define SERIAL_7E2 0x2C
#define SERIAL_8E2 0x2E
#define SERIAL_5O1 0x30
#define SERIAL_6O1 0x32
#define SERIAL_7O1 0x34
#define SERIAL_8O1 0x36
#define SERIAL_5O2 0x38
#define SERIAL_6O2 0x3A
#define SERIAL_7O2 0x3C
#define SERIAL_8O2 0x3E

class HwSerial : public Stream
{
  protected:
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;
    uint8_t  tx_buffer[HW_SERIAL1_TX_FIFO_SIZE];
    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    uint8_t  rx_buffer[HW_SERIAL1_RX_FIFO_SIZE];
  public:
    HwSerial();
    void            init(unsigned long baud) { init(baud, SERIAL_8N1); }
    void            init(unsigned long, uint8_t);
    virtual uint8_t available(void); // mandatory in any case to be "Stream compliant"
    virtual uint8_t read(void);      // mandatory in any case to be "Stream compliant"
    void            enableRx(uint8_t On);
FORCEINLINE void    _rx_complete_irq(void);
    void            enableTx(uint8_t On);
    void            resumeTx(void);
    virtual size_t  write(uint8_t);
    /*inline  size_t  write(unsigned long n) { return write((uint8_t)n); }
    inline  size_t  write(long n)          { return write((uint8_t)n); }
    inline  size_t  write(unsigned int n)  { return write((uint8_t)n); }
    inline  size_t  write(int n)           { return write((uint8_t)n); }*/
    using Print::write; // pull in write(str) and write(buf, size) from Print
    // Interrupt handlers - Not intended to be called externally
    void           _tx_udr_empty_irq(void);
};


  extern HwSerial Serial1;

#endif
