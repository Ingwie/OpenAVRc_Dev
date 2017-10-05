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

FORCEINLINE void sendStopPulses()
{
  PROTO_Cmds(PROTOCMD_RESET);
  TRACE("  ->  RESET Proto - %s -",  Protos[s_current_protocol].ProtoName);
  SIMU_SLEEP(500);
  PROTO_Stop_Callback();
  //s_current_protocol = 255;
}

void startPulses(enum ProtoCmds Command)
{
  IS_PROTO_NEED_SPI spi_enable_master_mode(); // Todo Call correct SPI changed soon ;-)
  PROTO_Stop_Callback();
  // Reset CS pin
#if defined(SPIMODULES)
  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();
#endif

  if (pulsesStarted()) {
    PROTO_Cmds(PROTOCMD_RESET);
    TRACE("  ->  RESET Proto - %s -",  Protos[s_current_protocol].ProtoName);
    SIMU_SLEEP(500);
  }
  //g_model.rfProtocol = limit((uint8_t)PROTOCOL_NONE, g_model.rfProtocol, (uint8_t)PROTOCOL_COUNT-2);
  s_current_protocol = g_model.rfProtocol;
  PROTO_Cmds = *Protos[g_model.rfProtocol].Cmds;
  TRACE("  ->  INIT Proto - %s -", Protos[g_model.rfProtocol].ProtoName);
  SIMU_SLEEP(500);
  PROTO_Cmds(PROTOCMD_GETOPTIONS);
  LimitRfOptionSettings();
  PROTO_Cmds(Command);
}

// This ISR should work for xmega.
ISR(TIMER1_COMPA_vect) // ISR for Protocol Callback, PPMSIM and PPM16 (Last 8 channels).
{
#if F_CPU > 16000000UL
  if (! timer_counts)
#endif
  {
    uint16_t half_us = timer_callback(); // Function pointer e.g. skyartec_cb().

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



void setupPulsesPPM(enum ppmtype proto)
{
  // Total frame length is a fixed 22.5msec (more than 9 channels is non-standard and requires this to be extended.)
  // Each channel's pulse is 0.7 to 1.7ms long, with a 0.3ms stop tail, making each complete cycle 1 to 2ms.

  // The pulse ISR is 2MHz that's why everything is multiplied by 2

  int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;   //range of 0.7..1.7msec

  uint16_t q = (g_model.PPMDELAY*50+300)*2; // Channel sync pulse.

  int32_t rest = 22500u*2 - q;

  // PPM 16 uses a fixed frame length of 22.5msec.
  if(proto == PPM || proto == PPMSIM) rest += (int32_t(g_model.PPMFRAMELENGTH))*1000;

  // PPM and PPM16 (Channels 1-8) use first half of array. PPMSIM and PPM16 (Channels 9-16) use last half.
  uint16_t *ptr = (proto == PPM || proto == PPM16FIRST) ? &pulses2MHz.pword[0] : &pulses2MHz.pword[PULSES_WORD_SIZE/2];

  uint8_t p;
  // Fix PPM16 to 16 channels (8+8), No modification by GUI.

  if(proto == PPM || proto == PPMSIM) p = 4 + (g_model.PPMNCH * 2); // Channels *2
  else if(proto == PPM16FIRST) p = 8;
  else p = 16; // PPM16 Channels 9-16.

  for (uint8_t i=(proto == PPM16LAST) ? 8 : 0; i<p; i++) { // Just do channels 1-8 unless PPM16 (9-16).
    int16_t v = limit((int16_t)-PPM_range, channelOutputs[i], (int16_t)PPM_range) + 2*PPM_CH_CENTER(i);
    rest -= v;
    *ptr++ = q;
    *ptr++ = v - q; // Total pulse width includes channel sync pulse.
  }

  *ptr++ = q;
  if (rest > 65535) rest = 65535; // Prevents overflows.
  if (rest < 9000)  rest = 9000;

  *ptr++ = rest - (PULSES_SETUP_TIME *2);
  *ptr = 0; // End array with (uint16_t) 0;
}


ISR(TIMER1_COMPB_vect) // Timer 1 compare "B" vector. Used for PPM commutation and maybe more ...
{
  ocr1b_function_ptr();
}


