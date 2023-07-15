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

#include "OpenAVRc.h"

#define SCC_PIN                            J, 2

#define CMD_SET_ID_FOLLOWED_BY_CMD_GET_ID  0
#define CMD_GET_ID                         1

#define SCC_INTER_CMD_DELAY_MS             100
#define SCC_REQ_TO_RESP_TIMEOUT_MS         100

#define CODE_GET_ID                        0xE0 /* Not used in this sketch */
#define CODE_SET_ID                        0xB2

#define CMD_HEADER_BYTE                    0x9F
#define RESP_HEADER_BYTE                   0x05

#define SBUS_VAL_MIN                       0
#define SBUS_VAL_MAX                       2047
#define SBUS_VAL_NEUTRAL                   ((SBUS_VAL_MIN + SBUS_VAL_MAX) / 2)
#define SBUS_VAL_STEP                      20

#define RX_DELAY_CENTERING                 112                     //     For
#define RX_DELAY_INTRA_BIT                 232                     //  9600 bauds
#define END_TX_RX_BIT_MASK                 (1 << (8 + 1))

#define TX_DELAY_SBUS_CFG_MODE             (RX_DELAY_INTRA_BIT + 2)
#define TX_DELAY_SBUS_FRAME_MODE           19 //This value has been tuned to obtain exactly 10us (bit time @ 100 000 Bauds)

const uint8_t GetIdMsg[] PROGMEM = {0x9F ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x80 ,0x00 ,0xE0 ,0x1F};

/* PRIVATE FUNCTION PROTOTYPES */
static uint8_t     reverseByteBitOrder(uint8_t Byte);
static uint8_t     calculate_sbus_checksum(uint8_t const* b);
static uint8_t     validate_sbus_checksum(uint8_t const* b);
static inline void serialRxMode(void);
static inline void serialTxMode(void);
static size_t      serialWrite(uint8_t b);
static size_t      serialWrite(uint8_t *buf, uint8_t bufLen);
static void        recv(void);
static inline void tunedDelay(uint16_t delay);

/* PUBLIC FUNCTIONS */

/**
* \file   scc.cpp
* \fn     void scc_init(void)
* \brief  Initialize the SCC module
* \param  Void
* \return Void
*/
void scc_init(void)
{
  memset((void*)&ReBuff.Scc, 0, sizeof(SccSt_t));
  ReBuff.Scc.Sbus.SweepValue = SBUS_VAL_NEUTRAL;
  PIN_MODE(SCC_PIN, INPUT);
  DIGITAL_WRITE(SCC_PIN, LOW); // No Pullup (and Logic is inverted -> Idle state = 0 when set as output)
  PIN_MODE(SCC_PIN, OUTPUT);   // Set pin as Output (LOW)
#if !defined(SIMU)
  ENABLE_PCINT_PORT(SCC_PIN);  // Enable PCINT at Port Level (Port containing the SCC_PIN)
#endif
}

/**
* \file   scc.cpp
* \fn     void scc_end(void)
* \brief  Terminate the SCC module by deactivating Rx interrupt and by setting the SCC pin in output mode
* \param  Void
* \return Void
*/
void scc_end(void)
{
  serialTxMode();
#if !defined(SIMU)
  DISABLE_PCINT_PORT(SCC_PIN); // Disable PCINT at Port Level (Port containing the SCC_PIN)
#endif
  systemBolls.scc_sweep_on = 0;
  ReBuff.Scc.Sbus.CurChId  = 0; // ID will be unknown at the next entry in SCC menu
}

/**
* \file   scc.cpp
* \fn     uint8_t scc_sendCmd(uint8_t ChId)
* \brief  Send a SCC command to change the Channel ID, or send a command to get the current Channel ID
* \param  ChId: The channel ID (0 to request the current Id, 1 to 16 to define the new channel ID
* \return: The current channel ID (1 to 16 if OK, 0 if no answer)
*/
uint8_t scc_sendCmd(uint8_t ChId)
{
  uint8_t  RetId = 0;
  uint8_t  Cmd = CMD_GET_ID;
  uint16_t Start10MsTick;

  ReBuff.Scc.Sbus.TxDelay = TX_DELAY_SBUS_CFG_MODE; // Tx delay for 9600 bauds
  if(ChId) Cmd = CMD_SET_ID_FOLLOWED_BY_CMD_GET_ID;
  if(Cmd == CMD_SET_ID_FOLLOWED_BY_CMD_GET_ID)
  {
    ReBuff.Scc.Sbus.Msg.Content.HeaderByte = CMD_HEADER_BYTE;
    ReBuff.Scc.Sbus.Msg.Content.ChIdx      = reverseByteBitOrder(ChId - 1);
    memcpy_P((void*)&ReBuff.Scc.Sbus.Msg.Content.DoNotTouch, GetIdMsg + 5, sizeof(ReBuff.Scc.Sbus.Msg.Content.DoNotTouch));
    ReBuff.Scc.Sbus.Msg.Content.CmdCode    = CODE_SET_ID;
    ReBuff.Scc.Sbus.Msg.Content.Checksum   = calculate_sbus_checksum((uint8_t const*)&ReBuff.Scc.Sbus.Msg);
    serialTxMode();
    serialWrite((uint8_t*)&ReBuff.Scc.Sbus.Msg, SCC_SBUS_MSG_LEN);
    Start10MsTick = GET_10MS_TICK();
    while((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(SCC_INTER_CMD_DELAY_MS)) // Wait before requesting the current channel
    {
      // Put here prio task if needed
      YIELD_TO_TASK(PRIO_TASK_LIST());
    }
  }
  /* Request for the current ID */
  memcpy_P((void*)&ReBuff.Scc.Sbus.Msg, GetIdMsg, SCC_SBUS_MSG_LEN);
  ReBuff.Scc.Sbus.MsgIdx = 0; // Flush Rx
  serialTxMode();
  serialWrite((uint8_t*)&ReBuff.Scc.Sbus.Msg, SCC_SBUS_MSG_LEN);
  serialRxMode();
  /* Wait for response */
  Start10MsTick = GET_10MS_TICK();
  ReBuff.Scc.Sbus.MsgIdx = 0;
  while(((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(SCC_REQ_TO_RESP_TIMEOUT_MS)) && (ReBuff.Scc.Sbus.MsgIdx < SCC_SBUS_MSG_LEN)) // Wait for answer
  {
    // Put here prio task if needed
    YIELD_TO_TASK(PRIO_TASK_LIST());
  }
  if(ReBuff.Scc.Sbus.MsgIdx >= SCC_SBUS_MSG_LEN)
  {
    if(!validate_sbus_checksum((uint8_t const*)&ReBuff.Scc.Sbus.Msg))
    {
      RetId = reverseByteBitOrder(ReBuff.Scc.Sbus.Msg.Content.ChIdx) + 1;
    }
  }
  return(RetId);
}

/**
* \file   scc.cpp
* \fn     void scc_sbusSweepEnable(uint8_t On)
* \brief  Put the SCC pin in output mode
* \param  Void
* \return Void
*/
void scc_sbusSweepEnable(uint8_t On)
{
  if(On)
  {
    serialTxMode();
  }
}

/**
* \file   scc.cpp
* \fn     void scc_sbusSweepProcess(void)
* \brief  Generate a SBUS frame on the SCC pin every xx ms by sweeping the current channel (for check purpose)
* \param  Void
* \return Void
*/
void scc_sbusSweepTestProcess(void)
{
  uint32_t        bits = 0;
  uint8_t         bitsavailable = 0, SbusByte;
  uint16_t        value;

  ReBuff.Scc.Sbus.TxDelay = TX_DELAY_SBUS_FRAME_MODE; // Tx delay for 100 000 bauds
  // Generate SBUS frame
  serialWrite(0x0F); // Header
  for (uint8_t ChIdx = 0; ChIdx < SBUS_CHANNEL_NB; ChIdx++)
  {
    if(ChIdx == (ReBuff.Scc.Sbus.NewChId - 1))
    {
      ReBuff.Scc.Sbus.SweepValue += (ReBuff.Scc.Sbus.SweepDirUp? +SBUS_VAL_STEP: -SBUS_VAL_STEP);
      if(ReBuff.Scc.Sbus.SweepValue >= SBUS_VAL_MAX)
      {
        ReBuff.Scc.Sbus.SweepValue = SBUS_VAL_MAX;
        ReBuff.Scc.Sbus.SweepDirUp = 0; // Max reached -> Change direction
      }
      if(ReBuff.Scc.Sbus.SweepValue <= SBUS_VAL_MIN)
      {
        ReBuff.Scc.Sbus.SweepValue = SBUS_VAL_MIN;
        ReBuff.Scc.Sbus.SweepDirUp = 1; // Min reached -> Change direction
      }
      value = ReBuff.Scc.Sbus.SweepValue;
    }
    else value = SBUS_VAL_NEUTRAL; // All the other channels are set to neutral
    bits |= (uint32_t)value << bitsavailable;
    bitsavailable += 11; // 11 bits per channel
    while (bitsavailable >= 8)
    {
      SbusByte = ((uint8_t) (bits & 0xff));
      serialWrite(SbusByte);
      bits >>= 8;
      bitsavailable -= 8;
    }
  }
  serialWrite(0x00); // Flags
  serialWrite(0x00); // Trailer
}

/* PRIVATE FUNCTIONS */

/**
* \file   scc.cpp
* \fn     uint8_t reverseByteBitOrder(uint8_t Byte)
* \brief  Reverse all the bits in a byte
* \param  Byte: Byte to be reversed
* \return The reversed byte
*/
static uint8_t reverseByteBitOrder(uint8_t Byte)
{
  Byte = ((Byte >> 1) & 0x55) | ((Byte << 1) & 0xaa);
  Byte = ((Byte >> 2) & 0x33) | ((Byte << 2) & 0xcc);
  Byte = ((Byte >> 4) & 0x0f) | ((Byte << 4) & 0xf0);
  return(Byte);
}

/**
* \file   scc.cpp
* \fn     uint8_t calculate_sbus_checksum(uint8_t const* b)
* \brief  Caculate the checksum for a SCC SBUS command message
* \param  b: Pointer of the command message
* \return The computed checksum
*/
static uint8_t calculate_sbus_checksum(uint8_t const* b)
{
  uint8_t n = 15; // 2nd to 16th byte
  uint8_t checksum = 0;
  do checksum -= reverseByteBitOrder(*++b); while (--n);
  return reverseByteBitOrder(checksum); // Checksum is 17th byte
}

/**
* \file   scc.cpp
* \fn     uint8_t validate_sbus_checksum(uint8_t const* b)
* \brief  Validate the checksum a SCC SBUS answer message
* \param  b: Pointer of the answer message
* \return 0: checksum OK, <> 0: bad checksum
*/
static uint8_t validate_sbus_checksum(uint8_t const* b)
{
  uint8_t n = 16;  // 2nd to 17th byte
  uint8_t checksum = 0;
  do checksum += reverseByteBitOrder(*++b); while (--n);
  return checksum; // Valid checksum will return 0
}

/**
* \file   scc.cpp
* \fn     void serialRxMode(void)
* \brief  Configure the SCC pin in Rx mode and enable the associated pin change interrupt
* \param  void
* \return void
*/
static inline void serialRxMode(void)
{
  PIN_MODE(SCC_PIN, INPUT);
#if !defined(SIMU)
  ENABLE_PCINT_PIN(SCC_PIN);
#endif
}

/**
* \file   scc.cpp
* \fn     void serialTxMode(void)
* \brief  Configure the SCC pin in Tx mode and disable the associated pin change interrupt
* \param  void
* \return void
*/
static inline void serialTxMode(void)
{
#if !defined(SIMU)
  DISABLE_PCINT_PIN(SCC_PIN);
#endif
  PIN_MODE(SCC_PIN, OUTPUT);
  DIGITAL_WRITE(SCC_PIN, LOW); // No Pullup (and Logic is inverted -> Idle state = 0)
}

/**
* \file   scc.cpp
* \fn     size_t serialWrite(uint8_t b)
* \brief  Write one byte on the SCC SBUS
* \param  b: The byte to be written
* \return The number of byte written (1)
*/
static size_t serialWrite(uint8_t b)
{
  uint16_t dataParStop  = 0;
  uint8_t  OddParity    = 0;

  /*          in dataParStop           */
  /* |<----------------------------->| */
  /* (Bit0)...(Bit 7)(Par)(Stop)(Stop): interrupt are re-enabled after the (Par) bit */
  dataParStop = b; // Keep the 8 bits
  // Compute parity
  for(uint8_t Idx = 0; Idx < 8; Idx++)
  {
    OddParity ^= (b & 1); /* Result is 0 if bit at 1 are even */
    b >>= 1;
  }
  /* Set parity bit */
  dataParStop |= OddParity << 8;

  /* Apply logic */
  dataParStop = ~dataParStop;

  uint8_t oldSREG = SREG;
  cli();  // turn off interrupts for a clean txmit

  // Write the start bit according to the logic
  DIGITAL_WRITE(SCC_PIN, HIGH);
  tunedDelay(ReBuff.Scc.Sbus.TxDelay);

  // Write each of the data bits, parity and stop bits
  for (uint16_t mask = 0x01; mask != END_TX_RX_BIT_MASK; mask <<= 1)
  {
    if (dataParStop & mask)         // choose bit
      DIGITAL_WRITE(SCC_PIN, HIGH); // send 1
    else
      DIGITAL_WRITE(SCC_PIN, LOW);  // send 0

    tunedDelay(ReBuff.Scc.Sbus.TxDelay);
  }
  DIGITAL_WRITE(SCC_PIN, LOW);  // restore pin to natural state according to the logic
  SREG = oldSREG;               // turn interrupts back on
  tunedDelay(ReBuff.Scc.Sbus.TxDelay); // First  stop bit
  tunedDelay(ReBuff.Scc.Sbus.TxDelay); // Second stop bit

  return 1;
}

/**
* \file   scc.cpp
* \fn     size_t serialWrite(uint8_t *buf, uint8_t bufLen)
* \brief  Write the content of a buffer on the SCC SBUS
* \param  buf:     Pointer on the buffre to write
* \param  bufLen:  Number of bytes to write
* \return: The number of bytes written (bufLen)
*/
static size_t serialWrite(uint8_t *buf, uint8_t bufLen)
{
  uint8_t Ret = bufLen;
  while(bufLen--)
  {
    serialWrite(*buf++);
  }
  return(Ret);
}

/**
* \file   scc.cpp
* \fn     void recv(void)
* \brief  Receive a SCC SBUS answer message
* \param  void
* \return The SCC SBUS answer message with the length in Scc.Sbus.MsgIdx
*/
static void recv(void)
{
  uint8_t  RxByte = 0, NotRxBitMask;

  if (DIGITAL_READ(SCC_PIN))
  {
    tunedDelay(RX_DELAY_CENTERING + RX_DELAY_INTRA_BIT); // After this delay, we are in the center of the first data bit
    // Read each of the 8 bits
    for (uint16_t RxBitMask = 0x1; RxBitMask != (END_TX_RX_BIT_MASK >> 1); RxBitMask <<= 1)
    {
      NotRxBitMask = ~RxBitMask;
      if (DIGITAL_READ(SCC_PIN))
      {
        RxByte |= RxBitMask;   // parity shift here as > 7 bit, so it is just discarded
      }
      else // else clause added to ensure function timing is ~balanced
      {
        RxByte &= NotRxBitMask;// parity shift here as > 7 bit, so it is just discarded
      }
      tunedDelay(RX_DELAY_INTRA_BIT);
    }
    tunedDelay(RX_DELAY_INTRA_BIT); // Parity is not checked
    // Stop bit
    tunedDelay(RX_DELAY_INTRA_BIT);
    // No need to wait for 2 Stop bits
    // Apply reversed polarity
    RxByte = ~RxByte;
    // if buffer full, return
    if (ReBuff.Scc.Sbus.MsgIdx < SCC_SBUS_MSG_LEN)
    {
      ReBuff.Scc.Sbus.Msg.ByteTbl[ReBuff.Scc.Sbus.MsgIdx++] = RxByte; // save new byte
    }
  }
}

/**
* \file   scc.cpp
* \fn     void tunedDelay(uint16_t delay)
* \brief  create a delay proportionally to delay
* \param  delay: The delay
* \return void
*/
static inline void tunedDelay(uint16_t delay)
{
#if !defined(SIMU)
uint8_t tmp = 0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+w" (delay), "+a" (tmp)
    : "0" (delay)
    );
#endif
}

/* Interrupt Sub-Routine */
ISR(PCINT1_vect) // PCINT1_vect for pin J2 on ATmega2560
{
  recv();
}
