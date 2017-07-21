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

#define TLM_MULTI 0 // usart 0 ????????

static uint32_t MULTI_fixed_id;

static void MULTI_Reset()
{
#if !defined(SIMU)
   UCSRB_N(TLM_MULTI) = 0 |  (0 << RXEN_N(TLM_MULTI)) | (0 << TXEN_N(TLM_MULTI)); // disable RX disable TX
#endif
}

static void MULTI_init()
{
#if !defined(SIMU)

#undef BAUD
#define BAUD 9600 // Set real speed
#include <util/setbaud.h>

  UBRRH_N(TLM_MULTI) = UBRRH_VALUE;
  UBRRL_N(TLM_MULTI) = UBRRL_VALUE;
  UCSRA_N(TLM_MULTI) &= ~(1 << U2X_N(TLM_MULTI)); // disable double speed operation if needed .

  UCSRB_N(TLM_MULTI) = 0 | (0 << RXCIE_N(TLM_MULTI)) | (0 << TXCIE_N(TLM_MULTI))    // set 8N1
  | (0 << UDRIE_N(TLM_MULTI)) | (1 << RXEN_N(TLM_MULTI)) | (1 << TXEN_N(TLM_MULTI)) // enable RX enable TX
  | (0 << UCSZ2_N(TLM_MULTI));

  UCSRC_N(TLM_MULTI) = 0 | (1 << UCSZ1_N(TLM_MULTI)) | (1 << UCSZ0_N(TLM_MULTI));

#endif

}

static uint16_t MULTI_bind_cb()
{
  //send bind command
  return 18000U *2;
}

static uint16_t MULTI_cb()
{
  // Send datas
  /*
  packet[X] = blabla ....
  packet[X] = blabla ....
  packet[X] = blabla ....

  // Send without interrupt
  for i = 0 to packet lengt do
      UDR_N(TLM_MULTI) = JQ6500_Data[i]; // Send Datas
  while UDRE(TLM_MULTI) ..... I don't remenber*/
  // Send datas

  heartbeat |= HEART_TIMER_PULSES;
  dt = TCNT1 - OCR1A; // Calculate latency and jitter.
  return 9000 *2; // 9 mSec
}


static void MULTI_initialize(uint8_t bind)
{
  CLOCK_StopTimer();
  MULTI_init();
  MULTI_fixed_id = SpiRFModule.fixed_id;
  if (bind) {
  CLOCK_StartTimer(25000U *2, MULTI_bind_cb);
  } else {
  CLOCK_StartTimer(25000U *2, MULTI_cb);
  }
}

const void *MULTI_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    MULTI_initialize(0);
    return 0;
  //case PROTOCMD_DEINIT:
  case PROTOCMD_RESET:
    CLOCK_StopTimer();
    MULTI_Reset();
    return 0;
  //case PROTOCMD_CHECK_AUTOBIND:
    //return (void *)1L; // Always Autobind
  case PROTOCMD_BIND:
    MULTI_initialize(1);
    return 0;
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

