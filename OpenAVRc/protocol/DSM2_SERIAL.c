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

#define TLM_MULTI 0 // usart 0


// DSM2 control bits
#define DSM2_CHANS                         6
#define FRANCE_BIT                         0x10
#define DSMX_BIT                           0x08
#define BAD_DATA                           0x47
#define DSM2_SEND_BIND                     0x80
#define DSM2_SEND_RANGECHECK               0x20

bool dsm2Bind = 0;
bool dsm2Range = 0;

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


static void DSM2_SERIAL_Reset()
{
#if defined(FRSKY)
 // Reset DSM2.
#endif
}


static uint16_t DSM_SERIAL_cb()
{
  // Schedule next Mixer calculations.
  SCHEDULE_MIXER_END(22*16);


  if (frskyTxBufferCount) return 1000 *2; // return, if buffer is not empty
  frskyTxBufferCount = 14; //
  uint8_t dsm2TxBufferCount = frskyTxBufferCount;

  uint8_t dsm2_header;

  if(s_current_protocol == PROTO_DSM2_LP45)
    dsm2_header = 0x00;
  else if(s_current_protocol == PROTO_DSM2_DSM2)
    dsm2_header = 0x10;
  else dsm2_header = 0x10 | DSMX_BIT; // PROTO_DSM2_DSMX

  if(dsm2Bind)
    dsm2_header |= DSM2_SEND_BIND;
  else if(dsm2Range)
    dsm2_header |= DSM2_SEND_RANGECHECK;
  else;

  frskyTxBuffer[--dsm2TxBufferCount] = dsm2_header;

  frskyTxBuffer[--dsm2TxBufferCount] = g_model.header.modelId[0]; // DSM2 Header. Second byte for model match.

  for (uint8_t i = 0; i < DSM2_CHANS; i++) {
    uint16_t pulse = limit(0, ((channelOutputs[i]*13)>>5)+512,1023);
    frskyTxBuffer[--dsm2TxBufferCount] = (i<<2) | ((pulse>>8)&0x03); // Encoded channel + upper 2 bits pulse width.
    frskyTxBuffer[--dsm2TxBufferCount] = pulse & 0xff; // Low byte
  }

#if !defined(SIMU)
  telemetryTransmitBuffer();
#endif

  heartbeat |= HEART_TIMER_PULSES;
  dt = TCNT1 - OCR1A; // Calculate latency and jitter.
  return 22000U *2; // 22 mSec Frame.
}


static void DSM2_SERIAL_initialize(uint8_t bind)
{
// 125K 8N1
#if defined(FRSKY) && defined(DSM2_SERIAL)
#if !defined(SIMU)

#undef BAUD
#define BAUD 125000

#include <util/setbaud.h>

  UBRRH_N(TLM_MULTI) = UBRRH_VALUE;
  UBRRL_N(TLM_MULTI) = UBRRL_VALUE;
  UCSRA_N(TLM_MULTI) &= ~(1 << U2X_N(TLM_MULTI)); // disable double speed operation.

  // Set 8N1 (leave TX and RX disabled for now)
  UCSRB_N(TLM_MULTI) = (0 << RXCIE_N(TLM_MULTI)) | (0 << TXCIE_N(TLM_MULTI)) | (0 << UDRIE_N(TLM_MULTI)) | (0 << RXEN_N(TLM_MULTI)) | (0 << TXEN_N(TLM_MULTI)) | (0 << UCSZ2_N(TLM_MULTI));
  UCSRC_N(TLM_MULTI) = (1 << UCSZ1_N(TLM_MULTI)) | (1 << UCSZ0_N(TLM_MULTI)); // Set 1 stop bit, No parity bit.
  while (UCSRA_N(TLM_MULTI) & (1 << RXC_N(TLM_MULTI))) UDR_N(TLM_MULTI); // Flush receive buffer.

  // These should be running right from power up on a FrSky enabled '9X.
  frskyTxBufferCount = 0; // TODO not driver code
  UCSRB_N(TLM_MULTI) |= (1 << TXEN_N(TLM_MULTI)); // Enable USART Tx.

#endif // SIMU
#endif // defined(DSM2_SERIAL)
#if defined(DSM2) || defined(PXX)
uint8_t moduleFlag[NUM_MODULES] = { 0 };
#endif

  if (bind) dsm2Bind = 1;
  else dsm2Bind = 0;
  CLOCK_StartTimer(25000U *2, DSM_SERIAL_cb();
}


const void *DSM2_SERIAL_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    dsm2Bind = 0;
    DSM2_SERIAL_initialize();
    return 0;
  case PROTOCMD_BIND:
    dsm2Bind = 1;
    DSM2_SERIAL_initialize();
    return 0;
  //case PROTOCMD_DEINIT:
  case PROTOCMD_RESET:
    CLOCK_StopTimer();
    DSM2_SERIAL_Reset();
    return 0;
  //case PROTOCMD_CHECK_AUTOBIND:
    //return (void *)1L; // Always Autobind

  //case PROTOCMD_NUMCHAN:
    //return (void *)7L;
  //case PROTOCMD_DEFAULT_NUMCHAN:
    //return (void *)7L;
//  case PROTOCMD_CURRENT_ID:
//    return Model.fixed_id ? (void *)((unsigned long)Model.fixed_id) : 0;
//  case PROTOCMD_TELEMETRYMULTI_state:
//    return (void *)(long)PROTO_TELEM_UNSUPPORTED;
  default:
    break;
  }
  return 0;
}

