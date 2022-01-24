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

#if (SERIAL_PROTOCOL==DSM)
  #define IS_DSM2_SERIAL_PROTOCOL(protocol)  (protocol==PROTOCOL_DSM_SERIAL)
#else
  #define IS_DSM2_SERIAL_PROTOCOL(protocol)  (0)
#endif

#if (SERIAL_PROTOCOL==MULTIMODULE)
  #define IS_MULTIMODULE_PROTOCOL(protocol)  (protocol==PROTOCOL_MULTI)
#else
  #define IS_MULTIMODULE_PROTOCOL(protocol)  (0)
#endif

#if (SERIAL_PROTOCOL==CRSF)
  #define IS_CRSF_PROTOCOL(protocol)  (protocol==PROTOCOL_CRSF)
#else
  #define IS_CRSF_PROTOCOL(protocol)  (0)
#endif

#if (SERIAL_PROTOCOL==SBUS)
  #define IS_SBUS_PROTOCOL(protocol)  (protocol==PROTOCOL_SBUS)
#else
  #define IS_SBUS_PROTOCOL(protocol)  (0)
#endif

#if (SERIAL_PROTOCOL==SUMD)
  #define IS_SUMD_PROTOCOL(protocol)  (protocol==PROTOCOL_SUMD)
#else
  #define IS_SUMD_PROTOCOL(protocol)  (0)
#endif

#if defined(SPIMODULES)
 #if (SERIAL_PROTOCOL==MULTIMODULE)
  #define LASTPROTOMENU1 PROTOCOL_MULTI+1
 #elif (SERIAL_PROTOCOL==CRSF)
  #define LASTPROTOMENU1 PROTOCOL_CRSF+1
 #elif (SERIAL_PROTOCOL==SBUS)
  #define LASTPROTOMENU1 PROTOCOL_SBUS+1
 #elif (SERIAL_PROTOCOL==SUMD)
  #define LASTPROTOMENU1 PROTOCOL_SUMD+1
 #elif (SERIAL_PROTOCOL==DSM)
  #define LASTPROTOMENU1 PROTOCOL_DSM_SERIAL+1
 #else
  #define LASTPROTOMENU1 PROTOCOL_PPMSIM+1
 #endif
 #define IS_SPIMODULES_PROTOCOL(protocol)  (protocol>=LASTPROTOMENU1)
#else
 #define IS_SPIMODULES_PROTOCOL(protocol)  (0)
 #define LASTPROTOMENU1 PROTOCOL_COUNT-1
#endif

#endif
