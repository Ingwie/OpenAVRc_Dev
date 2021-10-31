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

#define XTELEMETRY (g_model.rfOptionBool1)

#define CHAN_MULTIPLIER 100
#define CHAN_MAX_VALUE (100 * CHAN_MULTIPLIER)

const pm_char STR_SUBTYPE_SBUS[] PROGMEM = " 6ms""14ms";

const static RfOptionSettingsvar_t RfOpt_Sbus_Ser[] PROGMEM = {
  /*rfProtoNeed*/BOOL1USED,//can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
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

/*
static const char * const sbus_opts[] = {
  _tr_noop("Period (ms)"),  "6", "14", NULL,
  NULL
};

enum {
    PROTO_OPTS_PERIOD,
    LAST_PROTO_OPT,
};
//ctassert(LAST_PROTO_OPT <= NUM_PROTO_OPTS, too_many_protocol_opts);
*/

//#define SBUS_DATARATE             100000
#define SBUS_FRAME_PERIOD_MAX     14000   // 14ms
#define SBUS_CHANNELS             16
#define SBUS_PACKET_SIZE          25

static uint8_t packetSbus[SBUS_PACKET_SIZE];

//#define STICK_SCALE    869  // full scale at +-125
#define STICK_SCALE    800  // +/-100 gives 2000/1000 us
static void build_rcdata_pkt()
{
  int i;
	uint16_t channelsSbus[SBUS_CHANNELS];
  uint8_t sbusTxBufferCount = 24;

    for (i=0; i < SBUS_CHANNELS; i++) {
        if (i < SBUS_CHANNELS)//if (i < Model.num_channelsSbus)
            channelsSbus[i] = (uint16_t)(FULL_CHANNEL_OUTPUTS(i) * STICK_SCALE / CHAN_MAX_VALUE + 992);
        else
            channelsSbus[i] = 992;  // midpoint
    }

	packetSbus[0] = 0x0f;

  	packetSbus[1] = (uint8_t) ((channelsSbus[0] & 0x07FF));
  	packetSbus[2] = (uint8_t) ((channelsSbus[0] & 0x07FF)>>8 | (channelsSbus[1] & 0x07FF)<<3);
  	packetSbus[3] = (uint8_t) ((channelsSbus[1] & 0x07FF)>>5 | (channelsSbus[2] & 0x07FF)<<6);
  	packetSbus[4] = (uint8_t) ((channelsSbus[2] & 0x07FF)>>2);
  	packetSbus[5] = (uint8_t) ((channelsSbus[2] & 0x07FF)>>10 | (channelsSbus[3] & 0x07FF)<<1);
  	packetSbus[6] = (uint8_t) ((channelsSbus[3] & 0x07FF)>>7 | (channelsSbus[4] & 0x07FF)<<4);
  	packetSbus[7] = (uint8_t) ((channelsSbus[4] & 0x07FF)>>4 | (channelsSbus[5] & 0x07FF)<<7);
  	packetSbus[8] = (uint8_t) ((channelsSbus[5] & 0x07FF)>>1);
  	packetSbus[9] = (uint8_t) ((channelsSbus[5] & 0x07FF)>>9 | (channelsSbus[6] & 0x07FF)<<2);
  	packetSbus[10] = (uint8_t) ((channelsSbus[6] & 0x07FF)>>6 | (channelsSbus[7] & 0x07FF)<<5);
  	packetSbus[11] = (uint8_t) ((channelsSbus[7] & 0x07FF)>>3);
  	packetSbus[12] = (uint8_t) ((channelsSbus[8] & 0x07FF));
  	packetSbus[13] = (uint8_t) ((channelsSbus[8] & 0x07FF)>>8 | (channelsSbus[9] & 0x07FF)<<3);
  	packetSbus[14] = (uint8_t) ((channelsSbus[9] & 0x07FF)>>5 | (channelsSbus[10] & 0x07FF)<<6);
  	packetSbus[15] = (uint8_t) ((channelsSbus[10] & 0x07FF)>>2);
  	packetSbus[16] = (uint8_t) ((channelsSbus[10] & 0x07FF)>>10 | (channelsSbus[11] & 0x07FF)<<1);
  	packetSbus[17] = (uint8_t) ((channelsSbus[11] & 0x07FF)>>7 | (channelsSbus[12] & 0x07FF)<<4);
  	packetSbus[18] = (uint8_t) ((channelsSbus[12] & 0x07FF)>>4 | (channelsSbus[13] & 0x07FF)<<7);
  	packetSbus[19] = (uint8_t) ((channelsSbus[13] & 0x07FF)>>1);
  	packetSbus[20] = (uint8_t) ((channelsSbus[13] & 0x07FF)>>9 | (channelsSbus[14] & 0x07FF)<<2);
  	packetSbus[21] = (uint8_t) ((channelsSbus[14] & 0x07FF)>>6 | (channelsSbus[15] & 0x07FF)<<5);
  	packetSbus[22] = (uint8_t) ((channelsSbus[15] & 0x07FF)>>3);

	packetSbus[23] = 0x00; // flags
	packetSbus[24] = 0x00;


  for (i = 0; i < 24; i++) {
    uint16_t pulse = packetSbus[i];//limit(0, ((FULL_CHANNEL_OUTPUTS(i)*13)>>5)+512,1023);
    Usart0TxBuffer[--sbusTxBufferCount] = (i<<2) | ((pulse>>8)&0x03); // Encoded channel + upper 2 bits pulse width.
    Usart0TxBuffer[--sbusTxBufferCount] = pulse & 0xff; // Low byte
  }
  Usart0TxBufferCount = 24; // Indicates data to transmit.

}

// static uint8_t testrxframe[] = { 0x00, 0x0C, 0x14, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x03, 0x00, 0x00, 0x00, 0xF4 };

static enum {
    ST_DATA1,
    ST_DATA2,
} state;

static uint16_t mixer_runtime;
static uint16_t sbus_period;
static uint16_t SBUS_SERIAL_cb()//serial_cb()
{
    //if (sbus_period != Model.proto_opts[PROTO_OPTS_PERIOD] * 1000)
    //    sbus_period = Model.proto_opts[PROTO_OPTS_PERIOD] * 1000;
    if (sbus_period != g_model.rfSubType * 1000)
        sbus_period = g_model.rfSubType * 1000;

    switch (state) {
    case ST_DATA1:
        //CLOCK_RunMixer();    // clears mixer_sync, which is then set when mixer update complete
        state = ST_DATA2;
        return mixer_runtime;

    case ST_DATA2:
//        if (mixer_sync != MIX_DONE && mixer_runtime < 2000) mixer_runtime += 50;
        build_rcdata_pkt();
//        UART_Send(packetSbus, sizeof packetSbus);
        state = ST_DATA1;
        return sbus_period - mixer_runtime;
    }
#if !defined(SIMU)
    USART_TRANSMIT_BUFFER(SBUS_USART);
#endif
    heartbeat |= HEART_TIMER_PULSES;
    CALCULATE_LAT_JIT(); // Calculate latency and jitter.
    //return 22000U *2; // 22 mSec Frame.
    return sbus_period *2;//avoid compiler warning
}

static void SBUS_init()
{
// 100K 8E2
  USART_SET_BAUD_100K(SBUS_USART);
  USART_SET_MODE_8E2(SBUS_USART);
  USART_ENABLE_TX(SBUS_USART);
  Usart0TxBufferCount = 0;
#if defined(FRSKY)//HAS_EXTENDED_TELEMETRY
  if (XTELEMETRY) // telemetry on?
    {
      USART_ENABLE_RX(SBUS_USART);
      //frskyX_check_telemetry(packet_p2M, len);
    }
#endif
  state = ST_DATA1;

  //sbus_period = Model.proto_opts[PROTO_OPTS_PERIOD] ? (Model.proto_opts[PROTO_OPTS_PERIOD] * 1000) : SBUS_FRAME_PERIOD_MAX;
  sbus_period = g_model.rfSubType;
}


static uint16_t SBUS_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
  SBUS_SERIAL_cb();//build_rcdata_pkt();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

static uint16_t SBUS_cb()
{
  SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
  SBUS_SERIAL_cb();//build_rcdata_pkt();//CRSF_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 12000U *2;
}


static void SBUS_initialize(uint8_t bind)
{

  SBUS_init();
  if (bind) {
  PROTO_Start_Callback( SBUS_bind_cb);
  } else {
  PROTO_Start_Callback( SBUS_cb);
  }
}

const void *SBUS_SERIAL_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    SBUS_initialize(0);
    return 0;
  case PROTOCMD_BIND:
    SBUS_initialize(1);
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
                        STR_TELEMETRY,      //OptionBool 1
                        STR_DUMMY,      //OptionBool 2
                        STR_DUMMY       //OptionBool 3
                        );
    return 0;
  default:
    break;
  }
  return 0;
}
