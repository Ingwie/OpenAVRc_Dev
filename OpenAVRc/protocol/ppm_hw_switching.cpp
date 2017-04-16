/*
**************************************************************************
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code named                            *
*             OpenTx - https://github.com/opentx/opentx                  *
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
#include "interface.h"
#include "misc.h"
#if defined(SIMU)
#include "../targets/simu/simu_interface.h"
#endif

#define PULSES_SETUP_TIME 500 // 0.5ms

/*
 * 16 Bit Timer running @ 16MHz has a resolution of 0.5us.
 * This should give a PPM resolution of 2048.
*/
static uint16_t PPM_HW_cb()
{
  static int16_t PPM_Range;
  static uint32_t FrameLen;
  static uint16_t SyncPulse;
  static int8_t state = -1;


  if (state == -2) {
    // Need to prevent next toggle.
    // Also need to read pin and store before disconnecting switching output.
    if(PINB & (1<<OUT_B_PPM)) PORTB |= (1<<OUT_B_PPM);
    else PORTB &= ~(1<<OUT_B_PPM);

    TCCR1A &= ~(0b11<<COM1B0);
    state++;

    if (FrameLen > 65535) return 65535; // 32.7675ms
    if (FrameLen > 4500 *2)  return FrameLen;
    else return 4500 *2; // 4.5ms
  }
  else if (state == -1) {
    // Set the idle level.
    if (g_model.pulsePol) {
      TCCR1A = (TCCR1A | (1<<COM1B1)) & ~(1<<COM1B0); // Set idle level.
    }
    else {
      TCCR1A |= (0b11<<COM1B0);
    }
    TCCR1C = 1<<FOC1B; // Strobe FOC1B.
    TCCR1A = (TCCR1A | (1<<COM1B0)) & ~(1<<COM1B1); // Toggle OC1B on next match.

    PPM_Range = g_model.extendedLimits ? 640 *2 : 512 *2;
    SyncPulse = ((g_model.ppmDelay * 50) + 300) *2; // Lets call it what it is "Channel Sync Pulse".

    FrameLen = 45 + g_model.ppmFrameLength; // (22.5ms +/- 0.5ms steps) *2.
    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END((uint16_t) FrameLen *8);

    FrameLen = (uint32_t) FrameLen * 500 *2; // Convert to 0.5us counts.
    FrameLen -= PULSES_SETUP_TIME *2;
    state++;

    heartbeat |= HEART_TIMER_PULSES;
    return PULSES_SETUP_TIME *2;
  }
  else if (! (state & 1 )) {
    FrameLen -= SyncPulse;
    state++;

    if(state > 2 * (8 + (g_model.ppmNCH *2)) ) {
      state = -2;
    }
    return SyncPulse;
  }
  else {
    uint16_t ServoPulse = 2* PPM_CH_CENTER(state >> 1);
    ServoPulse += limit((int16_t)-PPM_Range, (channelOutputs[state >> 1] ), (int16_t)+PPM_Range);
    FrameLen -= (ServoPulse - SyncPulse);
    state ++;
    return (ServoPulse - SyncPulse);
  }
}


static void PPM_HW_initialize()
{

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    OCR1B = TCNT1 + (25000U *2);
  }
  // Setup Timer 1.
  // Normal mode (0), OVF @ TOP (0xFFFF), F_CPU/8.
  TCCR1B = (0b10 << CS10);

  TIFR1 |= 1<<OCF1B; // Reset Flag.
  TIMSK1 |= 1<<OCIE1B; // Enable Output Compare interrupt.
}


const void * PPM_SWITCHING_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
    case PROTOCMD_INIT: PPM_HW_initialize(); return 0;
    case PROTOCMD_DEINIT:
    case PROTOCMD_RESET:
      CLOCK_StopTimer();
      TIMSK1 &= ~(1<<OCIE1B); // Disable Output Compare interrupt.
      TIFR1 |= 1<<OCF1B; // Reset Flag.
      return (void *) 1L;
//        case PROTOCMD_CHECK_AUTOBIND: return 0;
//        case PROTOCMD_BIND:  ppm_hw_initialize(); return 0;
//        case PROTOCMD_NUMCHAN: return (void *) 16L;
//        case PROTOCMD_DEFAULT_NUMCHAN: return (void *) 8L;
/*        case PROTOCMD_GETOPTIONS:
            if (Model.proto_opts[CENTER_PW] == 0) {
                Model.proto_opts[CENTER_PW] = 1100;
                Model.proto_opts[DELTA_PW] = 500;
                Model.proto_opts[NOTCH_PW] = 400;
                Model.proto_opts[PERIOD_PW] = 22500;
            }
            return ppm_opts;
*/
//        case PROTOCMD_TELEMETRYSTATE: return (void *)(long) PROTO_TELEM_UNSUPPORTED;
    default: break;
  }
  return 0;
}


ISR(TIMER1_COMPB_vect) // PPM switching vector.
{
  uint16_t half_us = PPM_HW_cb();

  if(! half_us) {
    PPM_SWITCHING_Cmds(PROTOCMD_DEINIT);
    return;
  }

  dt = TCNT1 - OCR1B; // Calculate latency and jitter.
  if(dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if(dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;

  OCR1B += half_us;
}

