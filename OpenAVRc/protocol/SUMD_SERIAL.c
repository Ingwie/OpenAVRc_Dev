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

#define CHAN_MULTIPLIER 100
#define CHAN_MAX_VALUE (100 * CHAN_MULTIPLIER)

const pm_char STR_SUMD_PROTOCOLS[] PROGMEM = " 6""14";

const static RfOptionSettingsvar_t RfOpt_Sumd_Ser[] PROGMEM = {
  /*rfProtoNeed*/0,
  /*rfSubTypeMax*/1,
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/0,
};

static void SUMD_Reset()
{
  USART_DISABLE_TX(SUMD_USART);
  USART_DISABLE_RX(SUMD_USART);
}


//#define SUMD_DATARATE             115200
#define SUMD_FRAME_PERIOD_STD     10000   // 10ms in spec
#define SUMD_MAX_CHANNELS         16      // Spec supports 32, deviation current max is 16
#define SUMD_MAX_SUMD_SIZE      (3 + 2*SUMD_MAX_CHANNELS + 2)   // 3 header bytes, 16bit channels, 16bit CRC


#define CRC_POLYNOME 0x1021
static uint16_t crc16(uint16_t crc, uint8_t value) { // Todo : duplicated code
    uint8_t i;

    crc = crc ^ (int16_t)value << 8;

    for (i=0; i < 8; i++) {
        if (crc & 0x8000)
            crc = (crc << 1) ^ CRC_POLYNOME;
        else
            crc = (crc << 1);
    }
    return crc;
}

static uint16_t crc(uint8_t *data, uint8_t len) {
  uint16_t crc = 0;
  for (int i=0; i < len; i++)
      crc = crc16(crc, *data++);
  return crc;
}


// #define STICK_SCALE    869  // full scale at +-125
#define STICK_SCALE     3200  // +/-100 gives 15200/8800
#define STICK_CENTER   12000
static void build_rcdata_pkt()
{
    uint16_t chanval;
    uint16_t crc_val = 0;
    uint8_t j = 0;
    uint16_t channelsSumd[SUMD_MAX_CHANNELS];
    uint8_t sumdTxBufferCount = SUMD_MAX_SUMD_SIZE;

    packet_p2M[j++] = 0xa8;     // manufacturer id
    packet_p2M[j++] = 0x01;     // 0x01 normal packet_p2M, 0x81 failsafe setting
    packet_p2M[j++] = SUMD_MAX_CHANNELS;//Model.num_channels;

    for (int i=0; i < SUMD_MAX_CHANNELS; i++) {
        chanval = (uint16_t)(FULL_CHANNEL_OUTPUTS(i) * STICK_SCALE / CHAN_MAX_VALUE + STICK_CENTER);
        packet_p2M[j++] = chanval >> 8;
        packet_p2M[j++] = chanval;
    }

    crc_val = crc(packet_p2M, j);
    packet_p2M[j++] = crc_val >> 8;
    packet_p2M[j++] = crc_val;

    for (uint8_t i = 0; i < SUMD_MAX_CHANNELS; i++) {
      uint16_t pulse = packet_p2M[i];//limit(0, ((FULL_CHANNEL_OUTPUTS(i)*13)>>5)+512,1023);
      Usart0TxBuffer_p2M[--sumdTxBufferCount] = (i<<2) | ((pulse>>8)&0x03); // Encoded channel + upper 2 bits pulse width.
      Usart0TxBuffer_p2M[--sumdTxBufferCount] = pulse & 0xff; // Low byte
    }
    Usart0TxBufferCount = SUMD_MAX_SUMD_SIZE; // Indicates data to transmit.

#if !defined(SIMU)
    USART_TRANSMIT_BUFFER(SUMD_USART);
#endif
}


static enum {
    ST_DATA1,
    ST_DATA2,
} state;

#define SUMD_PERIOD      bind_counter_p2M

static uint16_t SUMD_SERIAL_cb() {
    SUMD_PERIOD = (g_model.rfSubType == 0)?6000:14000;

    switch (state) {
    case ST_DATA1:
        state = ST_DATA2;

    case ST_DATA2:
        // Schedule next Mixer calculations.
        SCHEDULE_MIXER_END_IN_US(SUMD_PERIOD);
        build_rcdata_pkt();
        state = ST_DATA1;
        heartbeat |= HEART_TIMER_PULSES;
        CALCULATE_LAT_JIT(); // Calculate latency and jitter.
        return SUMD_PERIOD *2; // 6 or 14 mSec Frame.
    }
    return SUMD_PERIOD;   // avoid compiler warning
}

static void SUMD_initialize()
{
// 115K2 8N1
  USART_SET_BAUD_115K2(SUMD_USART);
  USART_SET_MODE_8N1(SUMD_USART);
  USART_ENABLE_TX(SUMD_USART);
  Usart0TxBufferCount = 0;
  state = ST_DATA1;
  //SUMD_PERIOD = Model.proto_opts[PROTO_OPTS_PERIOD] ? (Model.proto_opts[PROTO_OPTS_PERIOD] * 1000) : SUMD_FRAME_PERIOD_STD;
  SUMD_PERIOD = g_model.rfSubType?g_model.rfSubType:SUMD_FRAME_PERIOD_STD;
  PROTO_Start_Callback( SUMD_SERIAL_cb);
}

const void *SUMD_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    SUMD_initialize();
    return 0;
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

