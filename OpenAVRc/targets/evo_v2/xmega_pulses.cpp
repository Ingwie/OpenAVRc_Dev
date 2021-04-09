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

uint16_t nextMixerEndTime = 0;
uint8_t s_current_protocol = 255;
uint16_t dt;


FORCEINLINE uint8_t pulsesStarted()
{
  return (s_current_protocol != 255);
}


FORCEINLINE void sendStopPulses()
{
  PROTO_Cmds(PROTOCMD_RESET);
  TRACE("  ->  RESET Proto - %s -",  Protos[s_current_protocol].ProtoName);
  SIMU_SLEEP(100);
  PROTO_Stop_Callback();

#if defined (MULTIMODULE)
  USART_DISABLE_TX(MULTI_USART);
  USART_DISABLE_RX(MULTI_USART);
#endif
#if defined (DSM2_SERIAL)
  USART_DISABLE_TX(DSM_USART);
#endif
#if defined(SPIMODULES)
  USART_DISABLE_TX(RF_USART);
  USART_DISABLE_RX(RF_USART);
  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();
  RF_CS_NRF24L01_INACTIVE();
  RF_CS_A7105_INACTIVE();
#endif
}


void startPulses(enum ProtoCmds Command)
{
  PROTO_Stop_Callback();

#if defined(SPIMODULES)
  RFPowerOut = 0;
#endif
#if defined(FRSKY)
  telemetryResetValue();
#endif

  if (pulsesStarted()) {
    sendStopPulses();
  }

  if (g_model.rfProtocol > (PROTOCOL_COUNT-1)) g_model.rfProtocol = PROTOCOL_PPM;
  s_current_protocol = g_model.rfProtocol;

  if(IS_PPM_PROTOCOL(g_model.rfProtocol)) {
    // Do not do setup_rf_tc() as pulses use PWM mode.
  }
#if defined (DSM2_SERIAL)
  else  if(IS_DSM2_SERIAL_PROTOCOL(g_model.rfProtocol)) {
    DSM_USART_PORT.PIN3CTRL = PORT_OPC_PULLUP_gc; // Pullup TXD.
    DSM_USART_PORT.DIRSET = USART_TXD_PIN_bm;
    setup_rf_tc();
  }
#endif
#if defined (MULTIMODULE)
  else if(IS_MULTIMODULE_PROTOCOL(g_model.rfProtocol)) {
    MULTI_USART_PORT.PIN3CTRL = PORT_OPC_PULLUP_gc; // Pullup TXD.
    MULTI_USART_PORT.DIRSET = USART_TXD_PIN_bm;
    MULTI_USART_PORT.PIN2CTRL = PORT_OPC_PULLUP_gc; // Pullup RXD.
    MULTI_USART_PORT.DIRCLR = USART_RXD_PIN_bm;
    setup_rf_tc();
  }
#endif
#if defined(SPIMODULES)
  else if(IS_SPIMODULES_PROTOCOL(g_model.rfProtocol)) {
    rf_usart_mspi_init();
    setup_rf_tc();
  }
#endif
  PROTO_Cmds = *Protos[g_model.rfProtocol].Cmds;
  TRACE("  ->  INIT Proto - %s -", Protos[g_model.rfProtocol].ProtoName);
  SIMU_SLEEP(100);
  PROTO_Cmds(PROTOCMD_GETOPTIONS);
  LimitRfOptionSettings();
  PROTO_Cmds(Command);
}


void setupPulsesPPM(enum ppmtype proto)
{
  // Total frame length is a fixed 22.5msec (more than 9 channels is non-standard and requires this to be extended.)
  // Each channel's pulse is 0.7 to 1.7ms long, with a 0.3ms channel sync pulse, making each complete cycle 1 to 2ms.

  // The pulse ISR is 2MHz that's why everything is multiplied by 2.

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

  *ptr++ = rest - (PULSES_SETUP_TIME_US *2);
  *ptr = 0; // End array with (uint16_t) 0;
}


ISR(RF_TIMER_COMPA_VECT) // ISR for Protocol Callback.
{
  timer_counts = timer_callback(); // Function pointer e.g. skyartec_cb().

    if(! timer_counts) {
      PROTO_Cmds(PROTOCMD_RESET);
      return;
    }

  RF_TIMER_COMPA_REG += timer_counts;

  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;
}


