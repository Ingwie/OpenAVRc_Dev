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
#include "Multidef.txt"
  MM_RF_PROTO_COUNT,
  MM_RF_PROTO_LAST = MM_RF_PROTO_COUNT - 1
};
#undef MULTIDEF

#define MULTIRFPROTOCOL rfOptionValue1
#define AUTOBINDMODE    rfOptionBool2
#define LOWPOWERMODE    rfOptionBool3

#define MULTI_CHANS           16
#define MULTI_CHAN_BITS       11

#define MM_PROTO_NAME_LEN 10
#define MM_SUB_PROTO_NAME_LEN 8

const static RfOptionSettingsvar_t RfOpt_Multi_Ser[] PROGMEM =
{
  /*rfProtoNeed*//*BOOL1USED |*/ BOOL2USED | BOOL3USED,
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
  uint8_t proto_num; // OpenAVRc protocol number
  uint8_t multi_num; // Multimudule protocol number
  const pm_char * protoNameString;
});

// To generate proto name strings.
// e.g. MULTIDEF(28,AFHDS2A,PWM_IBUS,PPM_IBUS,PWM_SBUS,PPM_SBUS,PWM_IB16,PPM_IB16,PWM_SB16,PPM_SB16)
#define MULTIDEF(num, name, ... ) const pm_char STR_MM_PROTO_##num[] PROGMEM = #name;  // Generates STR_MM_PROTO_28[] = "AFHDS2A".
#include "Multidef.txt"
#undef MULTIDEF

// To generate mm_proto_definition array.
// e.g. MULTIDEF(28,AFHDS2A,PWM_IBUS,PPM_IBUS,PWM_SBUS,PPM_SBUS,PWM_IB16,PPM_IB16,PWM_SB16,PPM_SB16)
#define MULTIDEF(num, name, ... ) {MM_RF_PROTO_##num##_##name, num, STR_MM_PROTO_##num},  // Generates {MM_RF_PROTO_28_AFHDS2A, 28, STR_MM_PROTO_28}.
const mm_protocol_definition multi_protocols[] =
{
  { 0x00, 0x00, STR_DUMMY },
#include "Multidef.txt"
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

struct mm_t1_pkt  *mm_type1_packet_ptr = &pulses2MHz.mm_st.mm_type1_packet;
#define mm_type1_packet  pulses2MHz.mm_st.mm_type1_packet
#define l_buffer  pulses2MHz.mm_st.mm_rx_buffer
#define heartbeat_p2m pulses2MHz.mm_st.heartbeat
#define write_ptr_p2m pulses2MHz.mm_st.write_ptr
#define state_p2m pulses2MHz.mm_st.state
#define length_p2m pulses2MHz.mm_st.length
#define pkt_type_p2m pulses2MHz.mm_st.pkt_type

static uint16_t MULTI_cb()
{
  SCHEDULE_MIXER_END_IN_US(22000); // Schedule next Mixer calculations.

  if (Usart0TxBufferCount) return 1000 * 2; // return, if buffer is not empty

  if (heartbeat_p2m)
    {
      if (!--heartbeat_p2m) memclear(&mm_type1_packet, MM_TYPE_01_PKT_LEN); //clear data buffer if connexion is lost
    }

  Usart0TxBufferCount = MM_TX_PKT_LEN;
  uint8_t multiTxBufferCount = Usart0TxBufferCount;

  uint8_t proto_type = (uint8_t) g_model.MULTIRFPROTOCOL;
  const mm_protocol_definition *pdef = getMultiProtocolDefinition(proto_type);
  proto_type = pdef->multi_num; // switch openAVRc protocol number to Multimodule protocol number

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

  protoByte = proto_type & 0x1F;
  if (systemBolls.protoMode == BIND_MODE)
    protoByte |= 0x80;
  else if (systemBolls.rangeModeIsOn)
    protoByte |= 0x20;

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
   option_protocol value is -128..127 byte 3.
   1<<7 seems like a special case to enable iBus telemetry.
   */
  if (proto_type == MM_RF_PROTO_28_AFHDS2A  &&  IS_USR_PROTO_IBUS())
    optionValue |= 0x80;
    Usart0TxBuffer_p2M[--multiTxBufferCount] = optionValue;

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

enum MPSTATE
{
  RESET,
  M_FOUND,
  P_FOUND,
  TYPE_FOUND,
  LEN_FOUND,
};

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

  state_p2m = RESET;
  heartbeat_p2m = 0;

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

NOINLINE void parseMultiByte(uint8_t data)
{

  enum PKTTYPE
  {
    MM_STATUS       = 0x01,
    FRSKY_SPORT_TLM = 0x02,
    FRSKY_HUB_TLM   = 0x03,
    SPEKTRUM_TLM    = 0x04,
    DSM_BIND_DATA   = 0x05,
    FLYSKY_TLM_AA   = 0x06,
    INPUT_SYNC      = 0x08,
    HITEC_TLM       = 0x0A,
    SCANNER_TLM     = 0x0B,
    FLYSKY_TLM_AC   = 0x0C,
    RX_CHLS_FDW     = 0x0D,
    HOTT_TLM        = 0x0E,
    MLINK_TLM       = 0x0F,
    CONFIG_TLM      = 0x10,
    PROTO_LIST      = 0x11,
  };

  switch (state_p2m)
  {
    case RESET: // Reset
      write_ptr_p2m = 0;
      length_p2m = 0;
      if (data == 'M') state_p2m = M_FOUND;
      break;

    case M_FOUND:
      if (data == 'P') state_p2m = P_FOUND;
      else state_p2m = RESET;
      break;

    case P_FOUND:
      pkt_type_p2m = data;
      if (data == MM_STATUS) state_p2m = TYPE_FOUND; // Multi Module Status.
      else if (data == FRSKY_SPORT_TLM) state_p2m = TYPE_FOUND; // FrSky S Port Telemetry packet.
      else if (data == FRSKY_HUB_TLM) state_p2m = TYPE_FOUND; // FrSky Hub Telemetry packet.
      else if (data == FLYSKY_TLM_AA) state_p2m = TYPE_FOUND; // Flysky 0xAA Telemetry. Length =29 data[0] = RSSI value, data[1-28] telemetry sensor values.
      else if (data == FLYSKY_TLM_AC) state_p2m = TYPE_FOUND; // Flysky 0xAC Telemetry. Length =29 data[0] = RSSI value, data[1-28] telemetry sensor values.
      else state_p2m = RESET;
      break;

    case TYPE_FOUND:
      if (data <= MM_RX_PKT_MAX_LEN)
      {
        length_p2m = data;
        state_p2m = LEN_FOUND;
      }
      else state_p2m = RESET;
      break;

    case LEN_FOUND:
      // store packet minus header.
      if (write_ptr_p2m < length_p2m)
      {
        l_buffer[write_ptr_p2m] = data;
        write_ptr_p2m++;

        if (write_ptr_p2m == length_p2m)
        {
          // load protocol informations if menuModelSetup is shown
          if (menuHandlers[menuLevel] == menuModelSetup && pkt_type_p2m == MM_STATUS && length_p2m == MM_TYPE_01_PKT_LEN)
          {
            memcpy(&mm_type1_packet, &l_buffer, MM_TYPE_01_PKT_LEN);
            heartbeat_p2m = 50; // 1 seconde
            state_p2m = RESET;
            break;
          }
          else if(pkt_type_p2m == FLYSKY_TLM_AA && length_p2m == MM_TYPE_06_PKT_LEN )
          {
            // iBus ...  extract and store rssi and replace with 0xAA so we have 0xAA plus 7 sensors of 4 bytes.
            telemetryData.rssi[0].set(l_buffer[0]);
            l_buffer[0] = 0xAA;
            LoadAFHDS2ATelemBuffer(l_buffer);
            state_p2m = RESET;
            break;
          }
          else if(pkt_type_p2m == FLYSKY_TLM_AC && length_p2m == MM_TYPE_0C_PKT_LEN )
          {
            // iBus ...  extract and store rssi and replace with 0xAC so we have 0xAC plus 4 sensors of 7 bytes.
            telemetryData.rssi[0].set(l_buffer[0]);
            l_buffer[0] = 0xAC;
            LoadAFHDS2ATelemBuffer(l_buffer);
            state_p2m = RESET;
            break;
          }
          else if ((pkt_type_p2m == FRSKY_HUB_TLM || pkt_type_p2m == FRSKY_SPORT_TLM) && length_p2m == FRSKY_TLM_PKT_SIZE)
          {
            LoadTelemBuffer(l_buffer);
            state_p2m = RESET;
            break;
          }
          else
          {
            state_p2m = RESET;
            break;
          }
        }

        break;
      }
  }
}

const char * const optionsstr[] PROGMEM =
{ STR_NULL, STR_MULTI_OPTION, STR_RFTUNEFINE, STR_MULTI_VIDFREQ,
    STR_MULTI_FIXEDID, STR_TELEMETRY, STR_MULTI_SERVOFREQ, STR_MULTI_MAX_THROW,
    STR_MULTI_RFCHAN, STR_MULTI_RFPOWER, STR_MULTI_OUTPUT, };

#undef mm_type1_packet
#undef l_buffer
#undef heartbeat_p2m
#undef write_ptr_p2m
#undef state_p2m
#undef length_p2m
#undef pkt_type_p2m

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
