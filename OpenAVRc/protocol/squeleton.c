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


const static RfOptionSettingsvar_t RfOpt_Squeleton_Ser[] PROGMEM = {
/*rfProtoNeed*/PROTO_NEED_SPI, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
/*rfSubTypeMax*/0,
/*rfOptionValue1Min*/-128,
/*rfOptionValue1Max*/127,
/*rfOptionValue2Min*/0,
/*rfOptionValue2Max*/0,
/*rfOptionValue3Max*/0,
};


const static uint8_t ZZ_SqueletonInitSequence[] PROGMEM = {
};

static void Squeleton_init()
{
}


static void Squeleton_send_data_packet()
{
}

static void Squeleton_send_bind_packet()
{
}

static uint16_t Squeleton_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
  Squeleton_send_bind_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

static uint16_t Squeleton_cb()
{
  SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
  Squeleton_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 12000U *2;
}


static void Squeleton_initialize(uint8_t bind)
{
  Squeleton_init();
  if (bind) {
  PROTO_Start_Callback( Squeleton_bind_cb);
  } else {
  PROTO_Start_Callback( Squeleton_cb);
  }
}

const void *Squeleton_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    Squeleton_initialize(0);
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    return 0;
  case PROTOCMD_BIND:
    Squeleton_initialize(1);
    return 0;
  case PROTOCMD_GETOPTIONS:
          SetRfOptionSettings(pgm_get_far_address(RfOpt_Squeleton_Ser),
                        STR_DUMMY,      //Sub proto
                        STR_DUMMY,      //Option 1 (int)
                        STR_DUMMY,      //Option 2 (int)
                        STR_DUMMY,      //Option 3 (uint 0 to 31)
                        STR_DUMMY,      //OptionBool 1
                        STR_DUMMY,      //OptionBool 2
                        STR_DUMMY       //OptionBool 3
                        );
    return 0;
  default:
    break;
  }
  return 0;
}

