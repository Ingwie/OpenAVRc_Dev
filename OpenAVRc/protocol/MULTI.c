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


// Third row is number of subtypes -1 (max valid subtype)
#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)
#define MULTI_SEND_AUTOBIND                 (1 << 6)
#define MULTI_CHANS           16
#define MULTI_CHAN_BITS       11

#define NO_SUBTYPE  0
#define MM_RF_CUSTOM_SELECTED 0xff

const static RfOptionSettingsvarstruct RfOpt_Multi_Ser[] PROGMEM = {
  /*rfProtoNeed*/BOOL1USED | BOOL2USED | BOOL3USED,
  /*rfSubTypeMax*/15,
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/MULTI_RF_PROTO_LAST,
  /*rfOptionValue2Min*/-127,
  /*rfOptionValue2Max*/127,
  /*rfOptionValue3Max*/0,
};

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
  { MM_RF_PROTO_FRSKY,      STR_SUBTYPE_FRSKY,    5,  STR_RFTUNEFINE    },
  { MM_RF_PROTO_HISKY,      STR_SUBTYPE_HISKY,    1,  0             },
  { MM_RF_PROTO_V2X2,       STR_SUBTYPE_V2X2,     1,  0             },
  { MM_RF_PROTO_DSM2,       STR_SUBTYPE_DSM,      3,  0             },
  { MM_RF_PROTO_YD717,      STR_SUBTYPE_YD717,    4,  0             },
  { MM_RF_PROTO_KN,         STR_SUBTYPE_KN,       1,  0             },
  { MM_RF_PROTO_SYMAX,      STR_SUBTYPE_SYMAX,    1,  0             },
  { MM_RF_PROTO_SLT,        STR_SUBTYPE_SLT,      1,  0             },
  { MM_RF_PROTO_CX10,       STR_SUBTYPE_CX10,     7,  0             },
  { MM_RF_PROTO_CG023,      STR_SUBTYPE_CG023,    2,  0             },
  { MM_RF_PROTO_BAYANG,     STR_SUBTYPE_BAYANG,   1,  STR_TELEMETRY },
  { MM_RF_PROTO_MT99XX,     STR_SUBTYPE_MT99,     4,  0             },
  { MM_RF_PROTO_MJXQ,       STR_SUBTYPE_MJXQ,     5,  0             },
  { MM_RF_PROTO_FY326,      STR_SUBTYPE_FY326,    1,  0             },
  { MM_RF_PROTO_SFHSS,      NO_SUBTYPE,           0,  STR_RFTUNEFINE    },
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
  Usart0DisableTx();
  Usart0DisableRx();
}

static uint16_t MULTI_cb()
{
  SCHEDULE_MIXER_END_IN_US(22000); // Schedule next Mixer calculations.

  // Send datas
  if (Usart0TxBufferCount) return 1000 *2; // return, if buffer is not empty
  Usart0TxBufferCount = 26;
  uint8_t multiTxBufferCount = Usart0TxBufferCount;

  // Our enumeration starts at 0
  int8_t type = g_model.MULTIRFPROTOCOL + 1;
  int8_t subtype = g_model.rfSubType;
  int8_t optionValue = g_model.rfOptionValue2;

  uint8_t protoByte = 0;
  if (protoMode == BIND_MODE)
    protoByte |= MULTI_SEND_BIND;
  else if (rangeModeIsOn)
    protoByte |= MULTI_SEND_RANGECHECK;

  // rfProtocol
  if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2) {

    // Autobinding should always be done in DSMX 11ms
    if(g_model.AUTOBINDMODE && protoMode == BIND_MODE)
      subtype = MM_RF_DSM2_SUBTYPE_AUTO;

    // Multi module in DSM mode wants the number of channels to be used as option value
    optionValue = MULTI_CHANS;
  }

  if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DEVO || g_model.MULTIRFPROTOCOL == MM_RF_PROTO_WK_2X01) {
    if(g_model.AUTOBINDMODE) optionValue =0;
    else optionValue =1;
  }


  // 15  for Multimodule is FrskyX or D16 which we map as a subprotocol of 3 (FrSky)
  // all protos > frskyx are therefore also off by one
  if (type >= 15)
    type = type + 1;

  // 25 is again a FrSky protocol (FrskyV) so shift again
  if (type >= 25)
    type = type + 1;

  if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_FRSKY) {
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
  if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_FS_AFHDS2A)
    optionValue = optionValue | 0x80;

  // For custom protocol send unmodified type byte
  if (g_model.CUSTOMPROTO)
    type = g_model.MULTIRFPROTOCOL;


  // header, byte 0,  0x55 for proto 0-31 0x54 for 32-63
  if (type <= 31)
    Usart0TxBuffer[--multiTxBufferCount] = 0x55;
  else
    Usart0TxBuffer[--multiTxBufferCount] = 0x54;

  // protocol byte 1
  protoByte |= (type & 0x1f);
  if(g_model.MULTIRFPROTOCOL != MM_RF_PROTO_DSM2)
    protoByte |= (g_model.AUTOBINDMODE << 6);

//  sendByteMulti(protoByte);
  Usart0TxBuffer[--multiTxBufferCount] = protoByte;

  // byte 2, subtype, powermode, model id
  Usart0TxBuffer[--multiTxBufferCount] = ((uint8_t) ((g_model.modelId & 0x0f)
                                          | ((subtype & 0x7) << 4)
                                          | (g_model.LOWPOWERMODE << 7))
                                         );

  // byte 3
  Usart0TxBuffer[--multiTxBufferCount] = (uint8_t) optionValue;

  uint32_t bits = 0;
  uint8_t bitsavailable = 0;

#if defined(X_ANY)
  Xany_scheduleTx_AllInstance();
#endif
  // byte 4-25, channels 0..2047
  // Range for pulses (channelsOutputs) is [-1024:+1024] for [-100%;100%]
  // Multi uses [204;1843] as [-100%;100%]
  for (uint8_t i=0; i<MULTI_CHANS; i++) {
    int16_t value = channelOutputs[i] + 2*PPM_CH_CENTER(i) - 2*PPM_CENTER;

    // Scale to 80%
    value =  value*8/10 + 1024;
    bits |= ((uint32_t)limit<int16_t>(0, value, 2047)) << bitsavailable;
    bitsavailable += MULTI_CHAN_BITS;

    while (bitsavailable >= 8) {
      Usart0TxBuffer[--multiTxBufferCount] = ((uint8_t) (bits & 0xff));
      bits >>= 8;
      bitsavailable -= 8;
    }
  }

#if !defined(SIMU)
  Usart0TransmitBuffer();
#endif

  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 22000U *2; // 22 mSec loop
}


static void MULTI_initialize()
{
// 100K 8E2
  Usart0Set100000BAUDS();
  Usart0Set8E2();
  Usart0EnableTx();
  Usart0EnableRx();
  Usart0TxBufferCount = 0;

  if (g_model.AUTOBINDMODE) PROTOCOL_SetBindState(500); // 5 Sec

  PROTO_Start_Callback( MULTI_cb);
}

const void *MULTI_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    MULTI_initialize();
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    MULTI_Reset();
    return 0;
  case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_Multi_Ser),
                        STR_DUMMY,       //Sub proto
                        STR_DUMMY,       //Option 1 (int)
                        STR_DUMMY,       //Option 2 (int)
                        STR_DUMMY,       //Option 3 (uint 0 to 31)
                        STR_DUMMY,       //OptionBool 1
                        STR_DUMMY,       //OptionBool 2
                        STR_DUMMY        //OptionBool 3
                       );
    return 0;
  default:
    break;
  }
  return 0;
}

