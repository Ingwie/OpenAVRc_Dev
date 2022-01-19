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
}

#define SBUS_CHANNELS             16
#define SBUS_PACKET_SIZE          25
#define SBUS_PERIOD               bind_counter_p2M

static void build_SBUS_data_ptk()
{
  Usart0TxBufferCount = SBUS_PACKET_SIZE; // Indicates data to transmit.

  uint16_t * channelsSbus = &pulses2MHz.pword[CHANNEL_USED_OFFSET/2]; // re use channel_used_p2M memory
  uint8_t SbusTxBufferCount = Usart0TxBufferCount;

 for (uint8_t i=0; i < SBUS_CHANNELS; i++)
  {
   int16_t tempval = FULL_CHANNEL_OUTPUTS(i) >> 1; // Div 2
   tempval += tempval >> 2; // Add div 4 -> 0.625 total

   channelsSbus[i] = (int16_t) tempval + 992;
  }

  Usart0TxBuffer_p2M[--SbusTxBufferCount] = 0x0f;

 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[0] & 0x07FF));
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[0] & 0x07FF)>>8 | (channelsSbus[1] & 0x07FF)<<3);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[1] & 0x07FF)>>5 | (channelsSbus[2] & 0x07FF)<<6);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[2] & 0x07FF)>>2);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[2] & 0x07FF)>>10 | (channelsSbus[3] & 0x07FF)<<1);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[3] & 0x07FF)>>7 | (channelsSbus[4] & 0x07FF)<<4);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[4] & 0x07FF)>>4 | (channelsSbus[5] & 0x07FF)<<7);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[5] & 0x07FF)>>1);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[5] & 0x07FF)>>9 | (channelsSbus[6] & 0x07FF)<<2);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[6] & 0x07FF)>>6 | (channelsSbus[7] & 0x07FF)<<5);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[7] & 0x07FF)>>3);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[8] & 0x07FF));
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[8] & 0x07FF)>>8 | (channelsSbus[9] & 0x07FF)<<3);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[9] & 0x07FF)>>5 | (channelsSbus[10] & 0x07FF)<<6);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[10] & 0x07FF)>>2);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[10] & 0x07FF)>>10 | (channelsSbus[11] & 0x07FF)<<1);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[11] & 0x07FF)>>7 | (channelsSbus[12] & 0x07FF)<<4);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[12] & 0x07FF)>>4 | (channelsSbus[13] & 0x07FF)<<7);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[13] & 0x07FF)>>1);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[13] & 0x07FF)>>9 | (channelsSbus[14] & 0x07FF)<<2);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[14] & 0x07FF)>>6 | (channelsSbus[15] & 0x07FF)<<5);
 	Usart0TxBuffer_p2M[--SbusTxBufferCount] = (uint8_t) ((channelsSbus[15] & 0x07FF)>>3);

	Usart0TxBuffer_p2M[--SbusTxBufferCount] = 0x00; // flags
	Usart0TxBuffer_p2M[--SbusTxBufferCount] = 0x00;

#if !defined(SIMU)
  USART_TRANSMIT_BUFFER(SBUS_USART);
#endif
}

static uint16_t SBUS_SERIAL_cb()//serial_cb()
{
 SBUS_PERIOD = (g_model.rfSubType == 0)?6000U:14000U;
 // Schedule next Mixer calculations.
 SCHEDULE_MIXER_END_IN_US(SBUS_PERIOD);
 build_SBUS_data_ptk();
 heartbeat |= HEART_TIMER_PULSES;
 CALCULATE_LAT_JIT(); // Calculate latency and jitter.
 return SBUS_PERIOD *2; // 6 or 14 mSec Frame.
}

static void SBUS_SERIAL_initialize()
{
// 100K 8E2
  USART_SET_BAUD_100K(SBUS_USART);
  USART_SET_MODE_8E2(SBUS_USART);
  USART_ENABLE_TX(SBUS_USART);
  Usart0TxBufferCount = 0;
  PROTO_Start_Callback(SBUS_SERIAL_cb);
}

const void *SBUS_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
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
