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

#ifndef HW_SERIAL_H
#define HW_SERIAL_H

#include <inttypes.h>

#include "Stream.h"

/* Do NOT touch below */
#define ISR_NONE      0
#define ISR_NORMAL    1
#define ISR_CUSTOM    2
#ifdef  NO
#undef  NO
#define NO            0
#endif
#ifdef  YES
#undef  YES
#define YES           1
#endif
/* Do NOT touch above */

/************************************************/
/* vvv Start of User HwSerial configuration vvv */
/************************************************/
#if defined(TINY_DBG_UART_USB)
#define HW_SERIAL0_TX_SUPPORT        ISR_NORMAL  // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial0
#define HW_SERIAL0_RX_SUPPORT        ISR_NORMAL  // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial0
#define HW_SERIAL0_ENABLE_TX_SUPPORT NO          // Choose here between NO and YES for Serial0 Enable/Disable Tx support
#define HW_SERIAL0_ENABLE_RX_SUPPORT NO          // Choose here between NO and YES for Serial0 Enable/Disable Rx support
#define HW_SERIAL0_TX_FIFO_SIZE      16          // Define here the fifo size (SHALL be a power of 2)
#define HW_SERIAL0_RX_FIFO_SIZE      8           // Define here the fifo size (SHALL be a power of 2)
#else
#define HW_SERIAL0_TX_SUPPORT        ISR_NONE    // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial0
#define HW_SERIAL0_RX_SUPPORT        ISR_NONE    // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial0
#define HW_SERIAL0_ENABLE_TX_SUPPORT NO          // Choose here between NO and YES for Serial0 Enable/Disable Tx support
#define HW_SERIAL0_ENABLE_RX_SUPPORT NO          // Choose here between NO and YES for Serial0 Enable/Disable Rx support
#define HW_SERIAL0_TX_FIFO_SIZE      0          // Define here the fifo size (SHALL be a power of 2)
#define HW_SERIAL0_RX_FIFO_SIZE      0           // Define here the fifo size (SHALL be a power of 2)
#endif

#if defined(U_CLI) || defined(TINY_DBG_UART_BT)
#if defined(XMODEM)
#define S1_TX_FF_SZ                  32
#define S1_RX_FF_SZ                  128
#else
#define S1_TX_FF_SZ                  16
#define S1_RX_FF_SZ                  8
#endif
#define HW_SERIAL1_TX_SUPPORT        ISR_NORMAL  // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial1
#define HW_SERIAL1_RX_SUPPORT        ISR_NORMAL  // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial1
#define HW_SERIAL1_ENABLE_TX_SUPPORT NO          // Choose here between NO and YES for Serial1 Enable/Disable Tx support
#define HW_SERIAL1_ENABLE_RX_SUPPORT NO          // Choose here between NO and YES for Serial1 Enable/Disable Rx support
#define HW_SERIAL1_TX_FIFO_SIZE      S1_TX_FF_SZ // Define here the fifo size (SHALL be a power of 2)
#define HW_SERIAL1_RX_FIFO_SIZE      S1_RX_FF_SZ // Define here the fifo size (SHALL be a power of 2)
#else
#warning UART_BT NOT USED!!!
#define HW_SERIAL1_TX_SUPPORT        ISR_NONE    // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial1
#define HW_SERIAL1_RX_SUPPORT        ISR_NONE    // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial1
#define HW_SERIAL1_ENABLE_TX_SUPPORT NO          // Choose here between NO and YES for Serial1 Enable/Disable Tx support
#define HW_SERIAL1_ENABLE_RX_SUPPORT NO          // Choose here between NO and YES for Serial1 Enable/Disable Rx support
#define HW_SERIAL1_TX_FIFO_SIZE      0           // Define here the fifo size (SHALL be a power of 2)
#define HW_SERIAL1_RX_FIFO_SIZE      0           // Define here the fifo size (SHALL be a power of 2)
#endif

#define HW_SERIAL2_TX_SUPPORT        ISR_NONE    // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial2
#define HW_SERIAL2_RX_SUPPORT        ISR_NONE    // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial2
#define HW_SERIAL2_ENABLE_TX_SUPPORT NO          // Choose here between NO and YES for Serial2 Enable/Disable Tx support
#define HW_SERIAL2_ENABLE_RX_SUPPORT NO          // Choose here between NO and YES for Serial2 Enable/Disable Rx support
#define HW_SERIAL2_TX_FIFO_SIZE      0           // Define here the fifo size (SHALL be a power of 2)
#define HW_SERIAL2_RX_FIFO_SIZE      0           // Define here the fifo size (SHALL be a power of 2)

#define HW_SERIAL3_TX_SUPPORT        ISR_NONE    // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial3
#define HW_SERIAL3_RX_SUPPORT        ISR_NONE    // Choose here between ISR_NONE, ISR_NORMAL and ISR_CUSTOM for Serial3
#define HW_SERIAL3_ENABLE_TX_SUPPORT NO          // Choose here between NO and YES for Serial3 Enable/Disable Tx support
#define HW_SERIAL3_ENABLE_RX_SUPPORT NO          // Choose here between NO and YES for Serial3 Enable/Disable Rx support
#define HW_SERIAL3_TX_FIFO_SIZE      0           // Define here the fifo size (SHALL be a power of 2)
#define HW_SERIAL3_RX_FIFO_SIZE      0           // Define here the fifo size (SHALL be a power of 2)
/**********************************************/
/* ^^^ End of User HwSerial configuration ^^^ */
/**********************************************/

#define HW_SERIAL0_SUPPORT           (HW_SERIAL0_TX_SUPPORT || HW_SERIAL0_RX_SUPPORT)
#define HW_SERIAL1_SUPPORT           (HW_SERIAL1_TX_SUPPORT || HW_SERIAL1_RX_SUPPORT)
#define HW_SERIAL2_SUPPORT           (HW_SERIAL2_TX_SUPPORT || HW_SERIAL2_RX_SUPPORT)
#define HW_SERIAL3_SUPPORT           (HW_SERIAL3_TX_SUPPORT || HW_SERIAL3_RX_SUPPORT)

/* Set unused fifo size to zero */
#if (HW_SERIAL0_TX_SUPPORT != ISR_NORMAL)
#undef  HW_SERIAL0_TX_FIFO_SIZE
#define HW_SERIAL0_TX_FIFO_SIZE  0
#endif
#if (HW_SERIAL0_RX_SUPPORT != ISR_NORMAL)
#undef  HW_SERIAL0_RX_FIFO_SIZE
#define HW_SERIAL0_RX_FIFO_SIZE  0
#endif

#if (HW_SERIAL1_TX_SUPPORT != ISR_NORMAL)
#undef  HW_SERIAL1_TX_FIFO_SIZE
#define HW_SERIAL1_TX_FIFO_SIZE  0
#endif
#if (HW_SERIAL1_RX_SUPPORT != ISR_NORMAL)
#undef  HW_SERIAL1_RX_FIFO_SIZE
#define HW_SERIAL1_RX_FIFO_SIZE  0
#endif

#if (HW_SERIAL2_TX_SUPPORT != ISR_NORMAL)
#undef  HW_SERIAL2_TX_FIFO_SIZE
#define HW_SERIAL2_TX_FIFO_SIZE  0
#endif
#if (HW_SERIAL2_RX_SUPPORT != ISR_NORMAL)
#undef  HW_SERIAL2_RX_FIFO_SIZE
#define HW_SERIAL2_RX_FIFO_SIZE  0
#endif

#if (HW_SERIAL3_TX_SUPPORT != ISR_NORMAL)
#undef  HW_SERIAL3_TX_FIFO_SIZE
#define HW_SERIAL3_TX_FIFO_SIZE  0
#endif
#if (HW_SERIAL3_RX_SUPPORT != ISR_NORMAL)
#undef  HW_SERIAL3_RX_FIFO_SIZE
#define HW_SERIAL3_RX_FIFO_SIZE  0
#endif

#define HW_SERIAL_TX_SUPPORT          (HW_SERIAL0_TX_SUPPORT || HW_SERIAL1_TX_SUPPORT || HW_SERIAL2_TX_SUPPORT || HW_SERIAL3_TX_SUPPORT)
#define HW_SERIAL_RX_SUPPORT          (HW_SERIAL0_RX_SUPPORT || HW_SERIAL1_RX_SUPPORT || HW_SERIAL2_RX_SUPPORT || HW_SERIAL3_RX_SUPPORT)

#define HW_SERIAL_ENABLE_TX_SUPPORT   (HW_SERIAL0_ENABLE_TX_SUPPORT || HW_SERIAL1_ENABLE_TX_SUPPORT || HW_SERIAL2_ENABLE_TX_SUPPORT || HW_SERIAL3_ENABLE_TX_SUPPORT)
#define HW_SERIAL_ENABLE_RX_SUPPORT   (HW_SERIAL0_ENABLE_RX_SUPPORT || HW_SERIAL1_ENABLE_RX_SUPPORT || HW_SERIAL2_ENABLE_RX_SUPPORT || HW_SERIAL3_ENABLE_RX_SUPPORT)

#define HW_SERIAL_TX_FIFO_SIZE        (HW_SERIAL0_TX_FIFO_SIZE || HW_SERIAL1_TX_FIFO_SIZE || HW_SERIAL2_TX_FIFO_SIZE || HW_SERIAL3_TX_FIFO_SIZE)
#define HW_SERIAL_RX_FIFO_SIZE        (HW_SERIAL0_RX_FIFO_SIZE || HW_SERIAL1_RX_FIFO_SIZE || HW_SERIAL2_RX_FIFO_SIZE || HW_SERIAL3_RX_FIFO_SIZE)

#if defined(HW_SERIAL0_TX_CUSTOM_ISR) || defined(HW_SERIAL1_TX_CUSTOM_ISR) || defined(HW_SERIAL2_TX_CUSTOM_ISR) || defined(HW_SERIAL3_TX_CUSTOM_ISR)
#define HW_SERIAL_TX_CUSTOM_ISR
#endif

#define HW_SERIAL_TX_NORMAL_ISR       ((HW_SERIAL0_TX_SUPPORT == ISR_NORMAL) || (HW_SERIAL1_TX_SUPPORT == ISR_NORMAL) || (HW_SERIAL2_TX_SUPPORT == ISR_NORMAL) || (HW_SERIAL3_TX_SUPPORT == ISR_NORMAL))
#define HW_SERIAL_RX_NORMAL_ISR       ((HW_SERIAL0_RX_SUPPORT == ISR_NORMAL) || (HW_SERIAL1_RX_SUPPORT == ISR_NORMAL) || (HW_SERIAL2_RX_SUPPORT == ISR_NORMAL) || (HW_SERIAL3_RX_SUPPORT == ISR_NORMAL))

#define HW_SERIAL_INIT_OBJ(Idx)       Idx, (HW_SERIAL##Idx##_TX_SUPPORT != ISR_NONE), (HW_SERIAL##Idx##_RX_SUPPORT != ISR_NONE)

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

typedef struct {
    uint8_t
        Idx       :2,
        UseTx     :1,
        UseRx     :1,
        Written   :1,// Has any byte been written to the UART since init()
        Rfu       :3;
}HwSerialSt_t;

class HwSerial : public Stream
{
  protected:
    HwSerialSt_t               _hwS;
    #if (HW_SERIAL_TX_FIFO_SIZE != 0)
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;
    #endif
    #if (HW_SERIAL_RX_FIFO_SIZE != 0)
    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    #endif
  public:
    HwSerial(uint8_t HwSerialIdx, uint8_t UseTx, uint8_t UseRx);
    void            init(unsigned long baud) { init(baud, SERIAL_8N1); }
    void            init(unsigned long, uint8_t);
    virtual uint8_t available(void); // mandatory in any case to be "Stream compliant"
    virtual uint8_t read(void);      // mandatory in any case to be "Stream compliant"
    #ifdef HW_SERIAL_ENABLE_RX_SUPPORT
    void            enableRx(uint8_t On);
    #endif
    #ifdef HW_SERIAL_RX_NORMAL_ISR
    inline void    _rx_complete_irq(void);
    #endif
    #ifdef HW_SERIAL_ENABLE_TX_SUPPORT
    void            enableTx(uint8_t On);
    void            resumeTx(void);
    #endif
    #ifdef HW_SERIAL_TX_NORMAL_ISR
    virtual size_t  write(uint8_t);
    inline  size_t  write(unsigned long n) { return write((uint8_t)n); }
    inline  size_t  write(long n)          { return write((uint8_t)n); }
    inline  size_t  write(unsigned int n)  { return write((uint8_t)n); }
    inline  size_t  write(int n)           { return write((uint8_t)n); }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    // Interrupt handlers - Not intended to be called externally
    void           _tx_udr_empty_irq(void);
    #endif
};

#ifdef HW_SERIAL0_SUPPORT
  extern HwSerial Serial0;
  #if (HW_SERIAL0_TX_SUPPORT == ISR_CUSTOM)
  extern void Serial0_customTxIsr();
  #endif
  #if (HW_SERIAL0_RX_SUPPORT == ISR_CUSTOM)
  extern void Serial0_customRxIsr();
  #endif
#endif

#ifdef HW_SERIAL1_SUPPORT
  extern HwSerial Serial1;
  #if (HW_SERIAL1_TX_SUPPORT == ISR_CUSTOM)
  extern void Serial1_customTxIsr();
  #endif
  #if (HW_SERIAL1_RX_SUPPORT == ISR_CUSTOM)
  extern void Serial1_customRxIsr();
  #endif
#endif

#ifdef HW_SERIAL2_SUPPORT
  extern HwSerial Serial2;
  #if (HW_SERIAL2_TX_SUPPORT == ISR_CUSTOM)
  extern void Serial2_customTxIsr();
  #endif
  #if (HW_SERIAL2_RX_SUPPORT == ISR_CUSTOM)
  extern void Serial2_customRxIsr();
  #endif
#endif

#ifdef HW_SERIAL3_SUPPORT
  extern HwSerial Serial3;
  #if (HW_SERIAL3_TX_SUPPORT == ISR_CUSTOM)
  extern void Serial3_customTxIsr();
  #endif
  #if (HW_SERIAL3_RX_SUPPORT == ISR_CUSTOM)
  extern void Serial3_customRxIsr();
  #endif
#endif

#endif
