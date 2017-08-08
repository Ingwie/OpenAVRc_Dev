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


//#include "../protocol/common.h"
//#include "../protocol/interface.h"
#include "../OpenAVRc.h"
#include "../protocol/misc.c"
#include "../spi.h"

#if defined(DSM2) || defined(PXX) || defined(SPIMODULES)
uint8_t moduleFlag = { 0 };
#endif


uint16_t nextMixerEndTime = 0;
uint8_t s_current_protocol = 255;
uint16_t dt;
uint16_t B3_comp_value;
#if F_CPU > 16000000UL
static volatile uint32_t timer_counts; // Could be uint16_t for mega2560.
#else
static volatile uint16_t timer_counts; // Is uint16_t for mega2560.
#endif

FORCEINLINE bool pulsesStarted()
{
  return (s_current_protocol != 255);
}

FORCEINLINE void pausePulses()
{
  PROTO_Cmds(PROTOCMD_RESET);
  TRACE("  ->  RESET Proto - %s -",  Protos[s_current_protocol].ProtoName);
  SIMU_SLEEP(1000);
  CLOCK_StopTimer();
  s_current_protocol = 255;
}

FORCEINLINE void resumePulses()
{
  startPulses(PROTOCMD_INIT);
}

void startPulses(enum ProtoCmds Command)
{
  spi_enable_master_mode(); // Todo check if Proto need SPI
  CLOCK_StopTimer();
// Reset CS pin
  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();

  if (pulsesStarted()) {
    PROTO_Cmds(PROTOCMD_RESET);
    TRACE("  ->  RESET Proto - %s -",  Protos[s_current_protocol].ProtoName);
    SIMU_SLEEP(500);
  }
  PROTO_CMD_ID = limit((uint8_t)1, PROTO_CMD_ID, (uint8_t)(DIM(Protos)-1)); // verify limits do not use PPM_BB
  s_current_protocol = PROTO_CMD_ID;
  PROTO_Cmds = *Protos[PROTO_CMD_ID].Cmds;
  TRACE("  ->  INIT Proto - %s -", Protos[PROTO_CMD_ID].ProtoName);
  SIMU_SLEEP(500);
  PROTO_Cmds(Command);
}

// This ISR should work for xmega.
ISR(TIMER1_COMPA_vect) // Protocol Callback ISR.
{
#if F_CPU > 16000000UL
  if (! timer_counts)
#endif
  {
    uint16_t half_us = timer_callback(); // e.g. skyartec_cb().

    if(! half_us) {
      PROTO_Cmds(PROTOCMD_DEINIT);
      return;
    }

    timer_counts = HALF_MICRO_SEC_COUNTS(half_us);
  }

#if F_CPU > 16000000UL
  if (timer_counts > 65535) {
    OCR1A += 32000;
    timer_counts -= 32000; // 16ms @ 16MHz counter clock.
  } else
#endif
  {
    OCR1A += timer_counts;
    timer_counts = 0;
  }

  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
}


void setupPulsesPPM(uint8_t proto)
{
  // Total frame length is a fixed 22.5msec (more than 9 channels is non-standard and requires this to be extended.)
  // Each channel's pulse is 0.7 to 1.7ms long, with a 0.3ms stop tail, making each compelte cycle 1 to 2ms.

  int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;   //range of 0.7..1.7msec

  uint16_t *ptr = (proto == PROTO_PPM ? (uint16_t *)pulses2MHz : (uint16_t *) &pulses2MHz[PULSES_SIZE/2]);

  //The pulse ISR is 2mhz that's why everything is multiplied by 2
  uint8_t p = (proto == PROTO_PPM16 ? 16 : 8) + (g_model.ppmNCH * 2); //Channels *2
  uint16_t q = (g_model.ppmDelay*50+300)*2; // Stoplen *2
  int32_t rest = 22500u*2 - q;

  rest += (int32_t(g_model.ppmFrameLength))*1000;
  for (uint8_t i=(proto==PROTO_PPM16) ? p-8 : 0; i<p; i++) {
    int16_t v = limit((int16_t)-PPM_range, channelOutputs[i], (int16_t)PPM_range) + 2*PPM_CH_CENTER(i);
    rest -= v;
    *ptr++ = q;
    *ptr++ = v - q; // total pulse width includes stop phase
  }

  *ptr++ = q;
  if (rest > 65535) rest = 65535; // prevents overflows.
  if (rest < 9000)  rest = 9000;

  if (proto == PROTO_PPM) {
    *ptr++ = rest - SETUP_PULSES_DURATION;
    pulses2MHzRPtr = pulses2MHz;
  } else {
    *ptr++ = rest;
    B3_comp_value = rest - SETUP_PULSES_DURATION; // 500us before end of sync pulse.
  }

  *ptr = 0;
}
