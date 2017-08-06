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

#define TLM_MULTI 2 // usart 0 ????????

static uint32_t MULTI_fixed_id;

// Third row is number of subtypes -1 (max valid subtype)
#define NO_SUBTYPE  0
#define MM_RF_CUSTOM_SELECTED 0xff


const pm_char STR_SUBTYPE_FLYSKY[] PROGMEM =     "\004""Std\0""V9x9""V6x6""V912""CX20";

const pm_char STR_SUBTYPE_AFHDS2A[] PROGMEM =    "\010""PWM,IBUS""PPM,IBUS""PWM,SBUS""PPM,SBUS";

const pm_char STR_SUBTYPE_FRSKY[] PROGMEM =      "\007""D16\0   ""D8\0    ""D16 8ch""V8\0    ""LBT(EU)""LBT 8ch";

const pm_char STR_SUBTYPE_HISKY[] PROGMEM =      "\005""HiSky""HK310";

const pm_char STR_SUBTYPE_DSM[] PROGMEM =        "\006""2 22ms""2 11ms""X 22ms""X 11ms";

const pm_char STR_SUBTYPE_YD717[] PROGMEM =      "\007""YD717\0 ""SKYWLKR""Syma X2""XINXUN\0""NIHUI\0  ";

const pm_char STR_SUBTYPE_SYMAX[] PROGMEM =      "\003""Std""5c\0";

const pm_char STR_SUBTYPE_SLT[] PROGMEM =        "\005""SLT\0 ""Vista";

const pm_char STR_SUBTYPE_CX10[] PROGMEM =       "\007""Green\0 ""Blue\0  ""DM007\0 ""-\0     ""JC3015a""JC3015b""MK33041""Q242\0  ";

const pm_char STR_SUBTYPE_CG023[] PROGMEM =      "\005""CG023""YD829""H8 3d";

const pm_char STR_SUBTYPE_KN[] PROGMEM =         "\006""WLtoys""FeiLun";

const pm_char STR_SUBTYPE_MT99[] PROGMEM =       "\005""MT99\0""H7\0  ""YZ\0  ""LS\0  ""FY805";

const pm_char STR_SUBTYPE_MJXQ[] PROGMEM =       "\005""WLH08""X600\0""X800\0""H26D\0""E010\0""H26WH";

const pm_char STR_SUBTYPE_HONTAI[] PROGMEM =     "\007""Std\0   ""JJRC X1""X5C1cln";

const pm_char STR_SUBTYPE_Q2X2[] PROGMEM =       "\004""Q222""Q242""Q282";

const pm_char STR_SUBTYPE_Q303[] PROGMEM =       "\006""Q303\0 ""CX35\0 ""CX10D\0""CX10WD";

const pm_char STR_SUBTYPE_WK2x01[] PROGMEM =     "\006""WK2801""WK2401""W6_5_1""W6_6_1""W6_Hel""W6_HeI";

const pm_char STR_SUBTYPE_V2X2[] PROGMEM =       "\006""V2x2\0 ""JXD506";

const pm_char STR_SUBTYPE_BAYANG[] PROGMEM =     "\006""Bayang""H8S3D";

const pm_char STR_SUBTYPE_FY326[] PROGMEM =      "\005""FY326""FY319";

const mm_protocol_definition multi_protocols[] = {
  { MM_RF_PROTO_FLYSKY,     STR_SUBTYPE_FLYSKY,   4,  0             },
  { MM_RF_PROTO_HUBSAN,     NO_SUBTYPE,           0,  STR_MULTI_VIDFREQ   },
  { MM_RF_PROTO_FRSKY,      STR_SUBTYPE_FRSKY,    5,  STR_MULTI_RFTUNE    },
  { MM_RF_PROTO_HISKY,      STR_SUBTYPE_HISKY,    1,  0             },
  { MM_RF_PROTO_V2X2,       STR_SUBTYPE_V2X2,     1,  0             },
  { MM_RF_PROTO_DSM2,       STR_SUBTYPE_DSM,      3,  0             },
  { MM_RF_PROTO_YD717,      STR_SUBTYPE_YD717,    4,  0             },
  { MM_RF_PROTO_KN,         STR_SUBTYPE_KN,       1,  0             },
  { MM_RF_PROTO_SYMAX,      STR_SUBTYPE_SYMAX,    1,  0             },
  { MM_RF_PROTO_SLT,        STR_SUBTYPE_SLT,      1,  0             },
  { MM_RF_PROTO_CX10,       STR_SUBTYPE_CX10,     7,  0             },
  { MM_RF_PROTO_CG023,      STR_SUBTYPE_CG023,    2,  0             },
  { MM_RF_PROTO_BAYANG,     STR_SUBTYPE_BAYANG,   1,  STR_MULTI_TELEMETRY },
  { MM_RF_PROTO_MT99XX,     STR_SUBTYPE_MT99,     4,  0             },
  { MM_RF_PROTO_MJXQ,       STR_SUBTYPE_MJXQ,     5,  0             },
  { MM_RF_PROTO_FY326,      STR_SUBTYPE_FY326,    1,  0             },
  { MM_RF_PROTO_SFHSS,      NO_SUBTYPE,           0,  STR_MULTI_RFTUNE    },
  { MM_RF_PROTO_HONTAI,     STR_SUBTYPE_HONTAI,   2,  0             },
  { MM_RF_PROTO_OLRS,       NO_SUBTYPE,           0,  STR_MULTI_RFPOWER   },
  { MM_RF_PROTO_FS_AFHDS2A, STR_SUBTYPE_AFHDS2A,  3,  STR_MULTI_SERVOFREQ },
  { MM_RF_PROTO_Q2X2,       STR_SUBTYPE_Q2X2,     2,  0             },
  { MM_RF_PROTO_WK_2X01,    STR_SUBTYPE_WK2x01,   5,  0             },
  { MM_RF_PROTO_Q303,       STR_SUBTYPE_Q303,     3,  0             },
  { MM_RF_CUSTOM_SELECTED,  NO_SUBTYPE,           7,  STR_MULTI_OPTION    },

  //Sential and default for protocols not listed above (MM_RF_CUSTOM is 0xff()
  { 0xfe,                   NO_SUBTYPE,           0,  0             }
};

#undef NO_SUBTYPE

const mm_protocol_definition *getMultiProtocolDefinition (uint8_t protocol)
{
  const mm_protocol_definition *pdef;
  for (pdef = multi_protocols; pdef->protocol != 0xfe; pdef++) {
    if (pdef->protocol == protocol)
      return pdef;
  }
  // Return the empty last protocol
  return pdef;
}


static void MULTI_Reset()
{
#if defined(FRSKY)
  // Reset multi telemetry
#endif
}

static void MULTI_init()
{
#if defined(FRSKY)
  //telemetryPortInit(Multi); //todo we need to build 3 function/or one with 3 parameters : PPM/DSM_PPM mode - Multi mode - SPI mode
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

  setupPulsesMultimodule();


  heartbeat |= HEART_TIMER_PULSES;
  dt = TCNT1 - OCR1A; // Calculate latency and jitter.
  return 9000 *2; // 9 mSec loop
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

