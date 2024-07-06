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

// Just to generate the enumeration of protocols.
// e.g. MULTIDEF(28,AFHDS2A,PWM_IBUS,PPM_IBUS,PWM_SBUS,PPM_SBUS,PWM_IB16,PPM_IB16,PWM_SB16,PPM_SB16)
#define MULTIDEF(num, name, ... ) MM_RF_PROTO_##num##_##name, // Generates MM_RF_PROTO_28_AFHDS2A

enum MultiModuleProtocols
{
  MM_RF_PROTO_FIRST = 0,
  MM_RF_PROTO_CUSTOM = MM_RF_PROTO_FIRST,
#include "Multi.txt"
  MM_RF_PROTO_COUNT,
  MM_RF_PROTO_LAST = MM_RF_PROTO_COUNT - 1
};
#undef MULTIDEF

#define MULTIRFPROTOCOL rfOptionValue1
#define CUSTOMPROTO     rfOptionBool1
#define AUTOBINDMODE    rfOptionBool2
#define LOWPOWERMODE    rfOptionBool3

#define MULTI_CHANS           16
#define MULTI_CHAN_BITS       11

#define NO_SUBTYPE  0
#define MM_RF_CUSTOM_SELECTED 0x0

#define MM_PROTO_NAME_LEN 10
#define MM_SUB_PROTO_NAME_LEN 8

const static RfOptionSettingsvar_t RfOpt_Multi_Ser[] PROGMEM =
{
  /*rfProtoNeed*/BOOL1USED | BOOL2USED | BOOL3USED,
  /*rfSubTypeMax*/15,
  /*rfOptionValue1Min*/-128,
  /*rfOptionValue1Max*/127,
  /*rfOptionValue2Min*/-127,
  /*rfOptionValue2Max*/127,
  /*rfOptionValue3Max*/0,
};


PACK(
struct mm_protocol_definition
{
  uint8_t proto_num;
  const pm_char * protoNameString;
});

// To generate proto name strings.
// e.g. MULTIDEF(28,AFHDS2A,PWM_IBUS,PPM_IBUS,PWM_SBUS,PPM_SBUS,PWM_IB16,PPM_IB16,PWM_SB16,PPM_SB16)
#define MULTIDEF(num, name, ... ) const pm_char STR_MM_PROTO_##num[] PROGMEM = #name;  // Generates STR_MM_PROTO_28[] = "AFHDS2A".
#include "Multi.txt"
#undef MULTIDEF

// To generate mm_proto_definition array.
// e.g. MULTIDEF(28,AFHDS2A,PWM_IBUS,PPM_IBUS,PWM_SBUS,PPM_SBUS,PWM_IB16,PPM_IB16,PWM_SB16,PPM_SB16)
#define MULTIDEF(num, name, ... ) {MM_RF_PROTO_##num##_##name, STR_MM_PROTO_##num},  // Generates {MM_RF_PROTO_28_AFHDS2A, STR_MM_PROTO_28}.
const mm_protocol_definition multi_protocols[] =
{
  { MM_RF_PROTO_CUSTOM, STR_MULTI_CUSTOM },
#include "Multi.txt"
  { 0xFE, STR_DUMMY },
};
#undef MULTIDEF

const mm_protocol_definition* getMultiProtocolDefinition(uint8_t proto_x)
{
  const mm_protocol_definition *pdef;
  // ToDo possible range check.
  return pdef = &multi_protocols[proto_x];
}

static void MULTI_Reset()
{
  USART_DISABLE_TX(MULTI_USART);
  USART_DISABLE_RX(MULTI_USART);
  parseTelemFunction = (p_parseTelemFunction)parseTelemFrskyByte;
}


static uint16_t MULTI_cb()
{
  SCHEDULE_MIXER_END_IN_US(22000); // Schedule next Mixer calculations.

  if (Usart0TxBufferCount) return 1000 * 2; // return, if buffer is not empty

  Usart0TxBufferCount = MM_TX_PKT_LEN;
  uint8_t multiTxBufferCount = Usart0TxBufferCount;

  uint8_t proto_type = (uint8_t) g_model.MULTIRFPROTOCOL;
  uint8_t protoByte;
  int8_t optionValue = g_model.rfOptionValue2;
  uint8_t subType = g_model.rfSubType;

  // stream[0] = header. 0x55 for proto 0-31, 0x54 for 32-63 ... etc
  Usart0TxBuffer_p2M[--multiTxBufferCount] = (proto_type & 0x20) ? 0x54 : 0x55;

  /*
   stream [1] =
   protocol bits 4..0
   RangeCheck bit 5
   AutoBindBit bit 6
   BindBit bit 7
   */

  // A bind condition originating from the MM cannot be displayed on the TX screen as it creates a loop condition.
  // e.g. MM binding -> BIND_MODE -> MM binding.
  //if(mm_type1_packet_ptr->mm_is_binding && systemBolls.protoMode == NORMAL_MODE) systemBolls.protoMode = BIND_MODE;
  //else if(!mm_type1_packet_ptr->mm_is_binding && systemBolls.protoMode == BIND_MODE) systemBolls.protoMode = NORMAL_MODE;

  protoByte = proto_type & 0x1f;
  if (systemBolls.protoMode == BIND_MODE) protoByte |= 0x80;
  else if (systemBolls.rangeModeIsOn) protoByte |= 0x20;
  Usart0TxBuffer_p2M[--multiTxBufferCount] = protoByte;

  /*
   Stream[2] =
   RxNum value is 0..15 (bits 0..3)
   Type value is 0..7 <<4     (bit 4..6)
   Power value => 0x80 0=High/1=Low
   */

  Usart0TxBuffer_p2M[--multiTxBufferCount] = ((uint8_t)( (g_model.modelId & 0x0f)
    | ((subType & 7) << 4)
    | (g_model.LOWPOWERMODE << 7)));

  /* Stream[3]   = option_protocol
   option_protocol value is -128..127byte 3
   */
  Usart0TxBuffer_p2M[--multiTxBufferCount] = (uint8_t) optionValue;

  /*
   Stream[4] to [25] = Channels or failsafe depending on Steam[0]
   16 Channels on 11 bits (0..2047)
   */

  uint32_t bits = 0;
  uint8_t bitsavailable = 0;

#if defined(X_ANY)
  Xany_scheduleTx_AllInstance();
#endif

  // byte 4-25, channels 0..2047
  // Range for pulses (channelsOutputs) is [-1024:+1024] for [-100%;100%]
  // Multi uses [204;1843] as [-100%;100%]
  for (uint8_t i = 0; i < MULTI_CHANS; i++)
  {
    int16_t value = FULL_CHANNEL_OUTPUTS(i);

    // Scale to 80%
    value = value * 8 / 10 + 1024;
    bits |= ((uint32_t) limit < int16_t > (0, value, 2047)) << bitsavailable;
    bitsavailable += MULTI_CHAN_BITS;

    while (bitsavailable >= 8)
    {
      Usart0TxBuffer_p2M[--multiTxBufferCount] = ((uint8_t)(bits & 0xff));
      bits >>= 8;
      bitsavailable -= 8;
    }
  }

  /*
   Stream[26] = sub_protocol bits 6 & 7|RxNum bits 4 & 5|Telemetry_Invert 3|Future_Use 2|Disable_Telemetry 1|Disable_CH_Mapping 0
   sub_protocol is 0..255 (bits 0..5 + bits 6..7)
   RxNum value is 0..63 (bits 0..3 + bits 4..5)
   Telemetry_Invert   => 0x08 0=normal, 1=invert
   Future_Use     => 0x04 0=      , 1=
   Disable_Telemetry  => 0x02 0=enable, 1=disable
   Disable_CH_Mapping => 0x01 0=enable, 1=disable
   */

  Usart0TxBuffer_p2M[--multiTxBufferCount] = (proto_type & 0xC0)
      | (g_model.modelId & 0x30);

#if !defined(SIMU)
  USART_TRANSMIT_BUFFER(MULTI_USART);
#endif

  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 22000U * 2; // 22 mSec loop
}

static void MULTI_initialize()
{
// 100K 8E2
  parseTelemFunction = (p_parseTelemFunction)parseMultiByte;
  USART_SET_BAUD_100K(MULTI_USART);
  USART_SET_MODE_8E2(MULTI_USART);
  USART_ENABLE_TX(MULTI_USART);
  USART_ENABLE_RX(MULTI_USART);
  Usart0TxBufferCount = 0;

  if (g_model.AUTOBINDMODE) PROTOCOL_SetBindState(500); // 5 Seconds. Maximum bind time of MM is 10 seconds.
  // This is an auto-bind from the Transmitter. Some protocols like Hubsan are a hard coded auto-bind from the module.

  PROTO_Start_Callback(MULTI_cb);
}

const void* MULTI_Cmds(enum ProtoCmds cmd)
{
  switch (cmd)
  {
    case PROTOCMD_INIT:
      MULTI_initialize();
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      MULTI_Reset();
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_Multi_Ser), STR_DUMMY, //Sub proto
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


struct mm_t1_pkt  *mm_type1_packet_ptr = &pulses2MHz.mm_st.mm_type1_packet;
#define mm_type1_packet  pulses2MHz.mm_st.mm_type1_packet
#define l_buffer  pulses2MHz.mm_st.mm_rx_buffer

NOINLINE void parseMultiByte(uint8_t data)
{
  enum MPSTATE
  {
    RESET,
    M_FOUND,
    P_FOUND,
    TYPE_FOUND,
    LEN_FOUND,
  };

  static uint8_t write_ptr;
  static uint8_t state = RESET;
  static uint8_t length;

  switch (state)
  {
    case RESET: // Reset
      write_ptr = 0;
      length = 0;
      if (data == 'M') state = M_FOUND;
      break;

    case M_FOUND:
      if (data == 'P') state = P_FOUND;
      else state = RESET;
      break;

    case P_FOUND:
      if (data == 0x01) state = TYPE_FOUND; // Multi Module Status.
      else if (data == 0x02) state = TYPE_FOUND; // FrSky S Port Telemetry packet.
      else if (data == 0x03) state = TYPE_FOUND; // FrSky Hub Telemetry packet.
      else state = RESET;
      break;

    case TYPE_FOUND:
      if (data <= MM_TYPE_01_PKT_LEN)
      {
        length = data;
        state = LEN_FOUND;
      }
      else state = RESET;
      break;

    case LEN_FOUND:
      // store packet minus header.
      if (write_ptr < length)
      {
        l_buffer[write_ptr] = data;
        write_ptr++;

        if (write_ptr == length)
        {
          if (length == MM_TYPE_01_PKT_LEN)
          {
            memcpy(&mm_type1_packet, &l_buffer, MM_TYPE_01_PKT_LEN);
            state = RESET;
            break;
          }
          else if (length == TELEM_RX_PACKET_SIZE)
          {
            LoadTelemBuffer(l_buffer);
            state = RESET;
            break;
          }
        break;
        }
      }
  }
}

// ToDo
// Add missing option strings. Temporary fix until translations are done.
const pm_char STR_MM_OPT_0[] PROGMEM = "";
//const pm_char STR_MM_OPT_1[] PROGMEM = ""; STR_MULTI_OPTION
//const pm_char STR_MM_OPT_2[] PROGMEM = ""; STR_RFTUNEFINE       INDENT "Freq.fine"
//const pm_char STR_MM_OPT_3[] PROGMEM = ""; STR_MULTI_VIDFREQ    INDENT "Vid. freq."
const pm_char STR_MM_OPT_4[] PROGMEM = "ID type";
//const pm_char STR_MM_OPT_5[] PROGMEM = ""; STR_TELEMETRY       "Telemetry"
//const pm_char STR_MM_OPT_6[] PROGMEM = ""; STR_MULTI_SERVOFREQ
const pm_char STR_MM_OPT_7[] PROGMEM = "Max throw";
const pm_char STR_MM_OPT_8[] PROGMEM = "Pick RF ch";
//const pm_char STR_MM_OPT_9[] PROGMEM = "";  STR_MULTI_RFPOWER   INDENT "RF Power"
const pm_char STR_MM_OPT_10[] PROGMEM = "Output";

const char *optionsstr[] =
{ STR_MM_OPT_0, STR_MULTI_OPTION, STR_RFTUNEFINE, STR_MULTI_VIDFREQ,
    STR_MM_OPT_4, STR_TELEMETRY, STR_MULTI_SERVOFREQ, STR_MM_OPT_7,
    STR_MM_OPT_8, STR_MULTI_RFPOWER, STR_MM_OPT_10, };

/*
 OPTION_NONE   0 Hidden field
 OPTION_OPTION 1 "Option:"   value=-128..0(default)..127
 OPTION_RFTUNE 2 "RF freq tune:" value=-128..0(default)..127
 OPTION_VIDFREQ  3 "Video freq:" value=-128..0(default)..127
 OPTION_FIXEDID  4 "ID type:"    value="Auto":0(default), "Fixed":1
 OPTION_TELEM  5 "Telem:"    value="Off":0(default), "On":1, "Off+Aux":2, "On+Aux":3
 OPTION_SRVFREQ  6 "Servo freq(Hz):" value="50":0(default).."400":70 => display=50+5*option with option=0..70
 OPTION_MAXTHR 7 "Max throw:"  value="Disabled":0, "Enabled":1
 OPTION_RFCHAN 8 "Select RF chan:" value=-128..0(default)..127
 OPTION_RFPOWER  9 "RF power:"   "1.6mW":0(default),"2.0mW":1,"2.5mW":2,"3.2mW":3,"4.0mW":4,"5.0mW":5,"6.3mW":6,"7.9mW":7,"10mW\0":8,"13mW\0":9,"16mW\0":10,"20mW\0":11,"25mW\0":12,"32mW\0":13,"40mW\0":14,"50mW\0":15
 OPTION_WBUS   10  "Output:"   "WBUS":0(default),"PPM":1
 */
