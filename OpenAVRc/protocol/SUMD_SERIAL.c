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

#include "../OpenAVRc.h"

const pm_char STR_SUMD_PROTOCOLS[] PROGMEM = " 6""10""14";

const static RfOptionSettingsvar_t RfOpt_Sumd_Ser[] PROGMEM = {
  /*rfProtoNeed*/0,
  /*rfSubTypeMax*/2, // 6,10&14mS
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/0,
};

static void SUMD_Reset()
{
  USART_DISABLE_TX(SUMD_USART);
}

#define CRC_POLYNOME 0x1021
static uint16_t crc16(uint16_t crc, uint8_t value) { // Todo : duplicated code

    crc = crc ^ (int16_t)value << 8;

    for (uint8_t i=0; i < 8; i++) {
        if (crc & 0x8000)
            crc = (crc << 1) ^ CRC_POLYNOME;
        else
            crc = (crc << 1);
    }
    return crc;
}

static uint16_t crc(uint8_t *data, uint8_t len) {

  uint16_t crc = 0;
  for (uint8_t i=0; i < len; i++)
      crc = crc16(crc, *data--);
  return crc;
}

#define SUMD_MAX_CHANNELS  16      // OAVRc max channels
#define SUMD_MAX_SIZE      (3 + 2*SUMD_MAX_CHANNELS + 2)   // 3 header bytes, 16bit channels, 16bit CRC
#define STICK_SCALE        4000  // +/-100
#define STICK_CENTER       12000
static void build_SUMD_data_ptk()
{
#if defined(X_ANY)
    Xany_scheduleTx_AllInstance();
#endif

    Usart0TxBufferCount = SUMD_MAX_SIZE;
    uint8_t sumdTxBufferCount = Usart0TxBufferCount;

    Usart0TxBuffer_p2M[--sumdTxBufferCount] = 0xA8;     // manufacturer id
    Usart0TxBuffer_p2M[--sumdTxBufferCount] = 0x01;     // 0x01 normal packet, 0x81 failsafe setting
    Usart0TxBuffer_p2M[--sumdTxBufferCount] = SUMD_MAX_CHANNELS;  //Model.num_channels ?

    for (uint8_t i=0; i < SUMD_MAX_CHANNELS; i++) {
        int16_t tempval = limit<int16_t>(0x1c20, (FULL_CHANNEL_OUTPUTS(i)<<2), 0x41a0); // X4 and limit
        tempval += STICK_CENTER;
        Usart0TxBuffer_p2M[--sumdTxBufferCount] = tempval >> 8;
        Usart0TxBuffer_p2M[--sumdTxBufferCount] = tempval;
    }

    uint16_t crc_val = crc(&Usart0TxBuffer_p2M[SUMD_MAX_SIZE-1], SUMD_MAX_SIZE-2);
    Usart0TxBuffer_p2M[--sumdTxBufferCount] = crc_val >> 8;
    Usart0TxBuffer_p2M[--sumdTxBufferCount] = crc_val;

#if !defined(SIMU)
    USART_TRANSMIT_BUFFER(SUMD_USART);
#endif
}

#define SUMD_FRAME_PERIOD        bind_counter_p2M
static uint16_t SUMD_SERIAL_cb()
{
 SUMD_FRAME_PERIOD = (6000U + g_model.rfSubType * 4000U);
  // Schedule next Mixer calculations.
 SCHEDULE_MIXER_END_IN_US(SUMD_FRAME_PERIOD);
 build_SUMD_data_ptk();
 heartbeat |= HEART_TIMER_PULSES;
 CALCULATE_LAT_JIT(); // Calculate latency and jitter.
 return SUMD_FRAME_PERIOD *2; // 10 mSec in specs, we allow 6,10&14 mS Frame.
}

static void SUMD_initialize()
{
// 115K2 8N1
  USART_SET_BAUD_115K2(SUMD_USART);
  USART_SET_MODE_8N1(SUMD_USART);
  USART_ENABLE_TX(SUMD_USART);
  Usart0TxBufferCount = 0;
  PROTO_Start_Callback( SUMD_SERIAL_cb);
}

const void *SUMD_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
  case PROTOCMD_BIND:
    SUMD_initialize();
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    SUMD_Reset();
    return 0;
  case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_Sumd_Ser),
                        STR_SUMD_PROTOCOLS,
                        STR_DUMMY,
                        STR_DUMMY,
                        STR_DUMMY,
                        STR_DUMMY,
                        STR_DUMMY,
                        STR_DUMMY);
    return 0;
  default:
    break;
  }
  return 0;
}

