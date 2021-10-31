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


#ifndef PULSES_COMMON_H
#define PULSES_COMMON_H

#define SCHEDULE_MIXER_END_IN_US(delay_us) nextMixerEndTime = getTmr64uS() + US_TO_64US_TICK(delay_us) - US_TO_64US_TICK(400) // 400 uS

#define IS_WAIT_PUPIL_STATE()       ((g_model.rfProtocol == (PROTOCOL_PPM16)) || (g_model.rfProtocol == (PROTOCOL_PPMSIM)))

#define IS_PPM_PROTOCOL(protocol)          (protocol<=PROTOCOL_PPMSIM)

#if defined(DSM2_SERIAL) // Todo check in needed in per10ms
  #define IS_DSM2_SERIAL_PROTOCOL(protocol)  (protocol==PROTOCOL_DSM_SERIAL)
#else
  #define IS_DSM2_SERIAL_PROTOCOL(protocol)  (0)
#endif

#if defined(CRSF_SERIAL)
  #define IS_CRSF_SERIAL_PROTOCOL(protocol)  (protocol==PROTOCOL_CRSF_SERIAL)
#else
  #define IS_CRSF_SERIAL_PROTOCOL(protocol)  (0)
#endif

#if defined(SBUS_SERIAL)
  #define IS_SBUS_SERIAL_PROTOCOL(protocol)  (protocol==PROTOCOL_SBUS_SERIAL)
#else
  #define IS_SBUS_SERIAL_PROTOCOL(protocol)  (0)
#endif

#if defined(SUMD_SERIAL)
  #define IS_SUMD_SERIAL_PROTOCOL(protocol)  (protocol==PROTOCOL_SUMD_SERIAL)
#else
  #define IS_SUMD_SERIAL_PROTOCOL(protocol)  (0)
#endif

#if defined(MULTIMODULE)
  #define IS_MULTIMODULE_PROTOCOL(protocol)  (protocol==PROTOCOL_MULTI)
#else
  #define IS_MULTIMODULE_PROTOCOL(protocol)  (0)
#endif

#if defined(SPIMODULES)
  #if defined(MULTIMODULE)
    #define LASTPROTOMENU1 PROTOCOL_MULTI+1
  #else
    #if defined(DSM2_SERIAL)
      #define LASTPROTOMENU1 PROTOCOL_DSM_SERIAL+1
    #else
      #define LASTPROTOMENU1 PROTOCOL_PPMSIM+1
    #endif
    #if defined(SBUS_SERIAL)
      #define LASTPROTOMENU1 PROTOCOL_SBUS_SERIAL+1
    #else
      #define LASTPROTOMENU1 PROTOCOL_PPMSIM+1
    #endif
  #endif
  #define IS_SPIMODULES_PROTOCOL(protocol)  (protocol>=LASTPROTOMENU1)
#else
  #define IS_SPIMODULES_PROTOCOL(protocol)  (0)
  #define LASTPROTOMENU1 PROTOCOL_COUNT-1
#endif

#endif
