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

#define SBUS_TELEMETRY (g_model.rfOptionBool1)
#define SBUS_AUTOBIND (g_model.rfOptionBool2)

#define CHAN_MULTIPLIER 100
#define CHAN_MAX_VALUE (100 * CHAN_MULTIPLIER)

const pm_char STR_SUBTYPE_SBUS[] PROGMEM = " 6""14";

const static RfOptionSettingsvar_t RfOpt_Sbus_Ser[] PROGMEM = {
  /*rfProtoNeed*/ 0,//can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/1,//2 subtypes, 6 et 14
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/0,
};

static void SBUS_Reset()
{
  USART_DISABLE_TX(SBUS_USART);
  USART_DISABLE_RX(SBUS_USART);
}

#define SBUS_FRAME_PERIOD_MAX     14000   // 14ms
#define SBUS_CHANNELS             16
#define SBUS_PACKET_SIZE          25

#define SBUS_PERIOD               bind_counter_p2M

//#define STICK_SCALE    869  // full scale at +-125
#define STICK_SCALE    800  // +/-100 gives 2000/1000 us
static void build_rcdata_pkt()
{
  uint8_t i;
  uint16_t channelsSbus[SBUS_CHANNELS];
  uint8_t sbusTxBufferCount = 24;

    for (i=0; i < SBUS_CHANNELS; i++) {
        if (i < SBUS_CHANNELS)//if (i < Model.num_channelsSbus)
            channelsSbus[i] = (uint16_t)(FULL_CHANNEL_OUTPUTS(i) * STICK_SCALE / CHAN_MAX_VALUE + 992);
        else
            channelsSbus[i] = 992;  // midpoint
    }

	packet_p2M[0] = 0x0f;

  	packet_p2M[1] = (uint8_t) ((channelsSbus[0] & 0x07FF));
  	packet_p2M[2] = (uint8_t) ((channelsSbus[0] & 0x07FF)>>8 | (channelsSbus[1] & 0x07FF)<<3);
  	packet_p2M[3] = (uint8_t) ((channelsSbus[1] & 0x07FF)>>5 | (channelsSbus[2] & 0x07FF)<<6);
  	packet_p2M[4] = (uint8_t) ((channelsSbus[2] & 0x07FF)>>2);
  	packet_p2M[5] = (uint8_t) ((channelsSbus[2] & 0x07FF)>>10 | (channelsSbus[3] & 0x07FF)<<1);
  	packet_p2M[6] = (uint8_t) ((channelsSbus[3] & 0x07FF)>>7 | (channelsSbus[4] & 0x07FF)<<4);
  	packet_p2M[7] = (uint8_t) ((channelsSbus[4] & 0x07FF)>>4 | (channelsSbus[5] & 0x07FF)<<7);
  	packet_p2M[8] = (uint8_t) ((channelsSbus[5] & 0x07FF)>>1);
  	packet_p2M[9] = (uint8_t) ((channelsSbus[5] & 0x07FF)>>9 | (channelsSbus[6] & 0x07FF)<<2);
  	packet_p2M[10] = (uint8_t) ((channelsSbus[6] & 0x07FF)>>6 | (channelsSbus[7] & 0x07FF)<<5);
  	packet_p2M[11] = (uint8_t) ((channelsSbus[7] & 0x07FF)>>3);
  	packet_p2M[12] = (uint8_t) ((channelsSbus[8] & 0x07FF));
  	packet_p2M[13] = (uint8_t) ((channelsSbus[8] & 0x07FF)>>8 | (channelsSbus[9] & 0x07FF)<<3);
  	packet_p2M[14] = (uint8_t) ((channelsSbus[9] & 0x07FF)>>5 | (channelsSbus[10] & 0x07FF)<<6);
  	packet_p2M[15] = (uint8_t) ((channelsSbus[10] & 0x07FF)>>2);
  	packet_p2M[16] = (uint8_t) ((channelsSbus[10] & 0x07FF)>>10 | (channelsSbus[11] & 0x07FF)<<1);
  	packet_p2M[17] = (uint8_t) ((channelsSbus[11] & 0x07FF)>>7 | (channelsSbus[12] & 0x07FF)<<4);
  	packet_p2M[18] = (uint8_t) ((channelsSbus[12] & 0x07FF)>>4 | (channelsSbus[13] & 0x07FF)<<7);
  	packet_p2M[19] = (uint8_t) ((channelsSbus[13] & 0x07FF)>>1);
  	packet_p2M[20] = (uint8_t) ((channelsSbus[13] & 0x07FF)>>9 | (channelsSbus[14] & 0x07FF)<<2);
  	packet_p2M[21] = (uint8_t) ((channelsSbus[14] & 0x07FF)>>6 | (channelsSbus[15] & 0x07FF)<<5);
  	packet_p2M[22] = (uint8_t) ((channelsSbus[15] & 0x07FF)>>3);

	packet_p2M[23] = 0x00; // flags
	packet_p2M[24] = 0x00;


  for (i = 0; i < 24; i++) {
    uint16_t pulse = packet_p2M[i];//limit(0, ((FULL_CHANNEL_OUTPUTS(i)*13)>>5)+512,1023);
    Usart0TxBuffer_p2M[--sbusTxBufferCount] = (i<<2) | ((pulse>>8)&0x03); // Encoded channel + upper 2 bits pulse width.
    Usart0TxBuffer_p2M[--sbusTxBufferCount] = pulse & 0xff; // Low byte
  }
  Usart0TxBufferCount = 24; // Indicates data to transmit.

#if !defined(SIMU)
  USART_TRANSMIT_BUFFER(SBUS_USART);
#endif
}

// static uint8_t testrxframe[] = { 0x00, 0x0C, 0x14, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x03, 0x00, 0x00, 0x00, 0xF4 };

static enum {
    ST_DATA1,
    ST_DATA2,
} state;


static uint16_t SBUS_SERIAL_cb()//serial_cb()
{
    SBUS_PERIOD = (g_model.rfSubType == 0)?6000U:14000U;
    switch (state)
    {
      case ST_DATA1:
          state = ST_DATA2;
      case ST_DATA2:
          // Schedule next Mixer calculations.
          SCHEDULE_MIXER_END_IN_US(SBUS_PERIOD);
          build_rcdata_pkt();
          state = ST_DATA1;
          heartbeat |= HEART_TIMER_PULSES;
          CALCULATE_LAT_JIT(); // Calculate latency and jitter.
          return SBUS_PERIOD *2; // 6 or 14 mSec Frame.
    }
    return SBUS_PERIOD;//avoid compiler warning
}


static void SBUS_SERIAL_initialize()
{
// 100K 8E2
  USART_SET_BAUD_100K(SBUS_USART);
  USART_SET_MODE_8E2(SBUS_USART);
  USART_ENABLE_TX(SBUS_USART);
  Usart0TxBufferCount = 0;
#if defined(TODO_FRSKY)
  if (SBUS_TELEMETRY) // telemetry on?
    {
      USART_ENABLE_RX(SBUS_USART);
    }
#endif
  state = ST_DATA1;
  PROTO_Start_Callback( SBUS_SERIAL_cb);
}

const void *SBUS_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    SBUS_SERIAL_initialize();
    return 0;
  case PROTOCMD_BIND:
    SBUS_SERIAL_initialize();
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    SBUS_Reset();
    return 0;
  case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_Sbus_Ser),
                        STR_SUBTYPE_SBUS,      //Sub proto
                        STR_DUMMY,      //Option 1 (int)
                        STR_DUMMY,      //Option 2 (int)
                        STR_DUMMY,      //Option 3 (uint 0 to 31)
                        STR_DUMMY,  //OptionBool 1
                        STR_DUMMY,   //OptionBool 2
                        STR_DUMMY       //OptionBool 3
                        );
    return 0;
  default:
    break;
  }
  return 0;
}
