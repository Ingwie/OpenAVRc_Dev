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

#define TLM_DSM_SERIAL TLM_USART0 // usart 0


// DSM2 control bits
#define DSM2_CHANS                         6
#define FRANCE_BIT                         0x10
#define DSMX_BIT                           0x08
#define BAD_DATA                           0x47
#define DSM2_SEND_BIND                     0x80
#define DSM2_SEND_RANGECHECK               0x20

#define DSMBIND    rf_power_p2M

// DSM2 protocol pulled from th9x - Thanks thus!!!

//http://www.rclineforum.de/forum/board49-zubeh-r-elektronik-usw/fernsteuerungen-sender-und-emp/neuer-9-kanal-sender-f-r-70-au/Beitrag_3897736#post3897736
//(dsm2(LP4DSM aus den RTF (Ready To Fly) Sendern von Spektrum.
//http://www.rcgroups.com/forums/showpost.php?p=18554028&postcount=237
// /home/thus/txt/flieger/PPMtoDSM.c
/*
  125000 Baud 8n1      _ xxxx xxxx - ---
#define DSM2_CHANNELS      6                // Max number of DSM2 Channels transmitted
#define DSM2_BIT (8*2)

bind:
  DSM2_Header = 0x80,0
static byte DSM2_Channel[DSM2_CHANNELS*2] = {
                ch
  0x00,0xAA,     0 0aa
  0x05,0xFF,     1 1ff
  0x09,0xFF,     2 1ff
  0x0D,0xFF,     3 1ff
  0x13,0x54,     4 354
  0x14,0xAA      5 0aa
};

normal:
  DSM2_Header = 0,0;
  DSM2_Channel[i*2]   = (byte)(i<<2) | highByte(pulse);
  DSM2_Channel[i*2+1] = lowByte(pulse);

*/

// DSM2=SERIAL mode


enum SubProtoDsm {
  Sub_LP45 = 0,
  Sub_DSM2,
  Sub_DSMX
};

const static RfOptionSettingsvar_t RfOpt_Dsm_Ser[] PROGMEM = {
  /*rfProtoNeed*/0,
  /*rfSubTypeMax*/2,
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/0,
};

static void DSM2_SERIAL_Reset()
{
  USART_DISABLE_TX(DSM_USART);
}


static uint16_t DSM_SERIAL_cb()
{
  // Schedule next Mixer calculations.
  SCHEDULE_MIXER_END_IN_US(22000);

  Usart0TxBufferCount = 0;

  uint8_t dsmTxBufferCount = 14;

  uint8_t dsm_header;

  if(g_model.rfSubType == Sub_LP45)
    dsm_header = 0x00;
  else if(g_model.rfSubType == Sub_DSM2)
    dsm_header = 0x10;
  else dsm_header = 0x10 | DSMX_BIT; // PROTO_DSM2_DSMX

  if(DSMBIND)
    dsm_header |= DSM2_SEND_BIND;
  else if(systemBolls.rangeModeIsOn)
    dsm_header |= DSM2_SEND_RANGECHECK;

  Usart0TxBuffer_p2M[--dsmTxBufferCount] = dsm_header;

  Usart0TxBuffer_p2M[--dsmTxBufferCount] = g_model.modelId; // DSM2 Header. Second byte for model match.

#if defined(X_ANY)
  Xany_scheduleTx_AllInstance();
#endif

  for (uint8_t i = 0; i < DSM2_CHANS; i++) {
    uint16_t pulse = limit(0, ((FULL_CHANNEL_OUTPUTS(i)*13)>>5)+512,1023);
    Usart0TxBuffer_p2M[--dsmTxBufferCount] = (i<<2) | ((pulse>>8)&0x03); // Encoded channel + upper 2 bits pulse width.
    Usart0TxBuffer_p2M[--dsmTxBufferCount] = pulse & 0xff; // Low byte
  }
  Usart0TxBufferCount = 14; // Indicates data to transmit.

#if !defined(SIMU)
  USART_TRANSMIT_BUFFER(DSM_USART);
#endif

  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 22000U *2; // 22 mSec Frame.
}


static void DSM_SERIAL_initialize()
{
// 125K 8N1
  USART_SET_BAUD_125K(DSM_USART);
  USART_SET_MODE_8N1(DSM_USART);
  USART_ENABLE_TX(DSM_USART);
  Usart0TxBufferCount = 0;
  PROTO_Start_Callback( DSM_SERIAL_cb);
}


const void *DSM_SERIAL_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    DSMBIND = 0;
    DSM_SERIAL_initialize();
    return 0;
  case PROTOCMD_BIND:
    DSMBIND = 1;
    DSM_SERIAL_initialize();
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    DSM2_SERIAL_Reset();
    return 0;
  case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_Dsm_Ser),
                        STR_DSM_PROTOCOLS,
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
