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
#include "../protocol/common.h"
#include "../protocol/interface.h"
#include "../protocol/misc.c"
#include "../spi.h"

#if defined(DSM2) || defined(PXX) || defined(SPIMODULES)
uint8_t moduleFlag = { 0 };
#endif


uint16_t nextMixerEndTime = 0;
uint8_t s_current_protocol = 255;
uint8_t s_pulses_paused = 0;
uint16_t dt;
uint16_t B3_comp_value;
#if F_CPU > 16000000UL
static volatile uint32_t timer_counts; // Could be uint16_t for mega2560.
#else
static volatile uint16_t timer_counts; // Is uint16_t for mega2560.
#endif

void startPulses(enum ProtoCmds Command)
{
spi_enable_master_mode(); // Todo check if Proto need SPI
CLOCK_StopTimer();
// Reset CS pin
RF_CS_CC2500_INACTIVE();
RF_CS_CYRF6936_INACTIVE();

if (s_current_protocol != 255) PROTO_Cmds(PROTOCMD_RESET);
PROTO_Cmds = *Protos[PROTO_CMD_ID].Cmds;
s_current_protocol = PROTO_CMD_ID;
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
  if (timer_counts > 65535)
  {
    OCR1A += 32000;
    timer_counts -= 32000; // 16ms @ 16MHz counter clock.
  }
  else
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



#if defined(MULTIMODULE)

#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)
#define MULTI_SEND_AUTOBIND                 (1 << 6)
#define MULTI_CHANS           16
#define MULTI_CHAN_BITS       11
void setupPulsesMultimodule()
{

  if (frskyTxBufferCount) return; // return, if buffer is not empty
  frskyTxBufferCount = 26;
  uint8_t multiTxBufferCount = frskyTxBufferCount;

  // Our enumeration starts at 0
  int type = g_model.moduleData.rfProtocol + 1;
  int subtype = g_model.moduleData.subType;
  int8_t optionValue = g_model.moduleData.optionValue;

  uint8_t protoByte = 0;
  if (moduleFlag == MODULE_BIND)
    protoByte |= MULTI_SEND_BIND;
  else if (moduleFlag == MODULE_RANGECHECK)
    protoByte |= MULTI_SEND_RANGECHECK;

  // rfProtocol
  if (g_model.moduleData.rfProtocol == MM_RF_PROTO_DSM2){

    // Autobinding should always be done in DSMX 11ms
    if(g_model.moduleData.autoBindMode && moduleFlag == MODULE_BIND)
      subtype = MM_RF_DSM2_SUBTYPE_AUTO;

    // Multi module in DSM mode wants the number of channels to be used as option value
    optionValue = 16; //NUM_CHANNELS(EXTERNAL_MODULE);

  }

  // 15  for Multimodule is FrskyX or D16 which we map as a subprotocol of 3 (FrSky)
  // all protos > frskyx are therefore also off by one
  if (type >= 15)
    type = type + 1;

  // 25 is again a FrSky protocol (FrskyV) so shift again
  if (type >= 25)
     type = type + 1;

  if (g_model.moduleData.rfProtocol == MM_RF_PROTO_FRSKY) {
    if(subtype == MM_RF_FRSKY_SUBTYPE_D8) {
      //D8
      type = 3;
      subtype = 0;
    } else if (subtype == MM_RF_FRSKY_SUBTYPE_V8) {
      //V8
      type = 25;
      subtype = 0;
    } else {
      type = 15;
      if (subtype == MM_RF_FRSKY_SUBTYPE_D16_8CH) // D16 8ch
        subtype = 1;
      else if (subtype == MM_RF_FRSKY_SUBTYPE_D16)
        subtype = 0;  // D16
      else if (subtype == MM_RF_FRSKY_SUBTYPE_D16_LBT)
        subtype = 2;
      else
        subtype = 3; // MM_RF_FRSKY_SUBTYPE_D16_LBT_8CH
    }
  }

  // Set the highest bit of option byte in AFHDS2A protocol to instruct MULTI to passthrough telemetry bytes instead
  // of sending Frsky D telemetry
  if (g_model.moduleData.rfProtocol == MM_RF_PROTO_FS_AFHDS2A)
    optionValue = optionValue | 0x80;

  // For custom protocol send unmodified type byte
  if (g_model.moduleData.customProto)
    type = g_model.moduleData.rfProtocol;


  // header, byte 0,  0x55 for proto 0-31 0x54 for 32-63
  if (type <= 31)
	frskyTxBuffer[--multiTxBufferCount] = 0x55;
//    sendByteMulti(0x55);
  else
//    sendByteMulti(0x54);
	frskyTxBuffer[--multiTxBufferCount] = 0x54;


  // protocol byte 1
  protoByte |= (type & 0x1f);
  if(g_model.moduleData.rfProtocol != MM_RF_PROTO_DSM2)
    protoByte |= (g_model.moduleData.autoBindMode << 6);

//  sendByteMulti(protoByte);
  frskyTxBuffer[--multiTxBufferCount] = protoByte;

  // byte 2, subtype, powermode, model id
  frskyTxBuffer[--multiTxBufferCount] = ((uint8_t) ((g_model.moduleData.rxnum & 0x0f)
                           | ((subtype & 0x7) << 4)
                           | (g_model.moduleData.lowPowerMode << 7))
                );

  // byte 3
//  sendByteMulti((uint8_t) optionValue);
  frskyTxBuffer[--multiTxBufferCount] = (uint8_t) optionValue;

  uint32_t bits = 0;
  uint8_t bitsavailable = 0;

  // byte 4-25, channels 0..2047
  // Range for pulses (channelsOutputs) is [-1024:+1024] for [-100%;100%]
  // Multi uses [204;1843] as [-100%;100%]
  for (int i=0; i<MULTI_CHANS; i++) {
    int channel = i;//g_model.moduleData[port].channelsStart+i;
    int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;

    // Scale to 80%
    value =  value*8/10 + 1024;
    bits |= ((uint32_t) limit(0, value, 2047)) << bitsavailable;
    bitsavailable += MULTI_CHAN_BITS;
    while (bitsavailable >= 8) {
//      sendByteMulti((uint8_t) (bits & 0xff));
	  frskyTxBuffer[--multiTxBufferCount] = ((uint8_t) (bits & 0xff));
      bits >>= 8;
      bitsavailable -= 8;
    }
  }

#if !defined(SIMU)
  telemetryTransmitBuffer();
#endif

}

#endif
