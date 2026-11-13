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

// Deviation & Multiprotocol inspired. Thanks a lot !
// [DEVO V8 FULL STANDARD TELEMETRY]
// Standard DEVO telemetry mapped only where OpenAVRc has a real equivalent:
// - 0x30: RPM1
// - 0x31: Temp1 / Temp2
// - 0x32: GPS longitude
// - 0x33: GPS latitude
// - 0x34: altitude
// - 0x35: GPS speed
// - 0x36: GPS date/time
// - CYRF RSSI kept as link RSSI
//
// Not mapped:
// - DEVO Volt1 / Volt2 / Volt3: no unambiguous OpenAVRc equivalent
// - RPM2: OpenAVRc has one standard RPM field
// - Temp3 / Temp4: OpenAVRc has two standard temperature fields
// - Vario speed: no native DEVO vertical-speed field in 0x30..0x36
//
// [DEVO V7 BOOL MAPPING]
// - Bool1 = Telemetry
// - Bool2 = Autobind
// - Bool3 = Static ID
// - RF logic unchanged from V6.
//
// [DEVO V6 TELEMETRY MENU]
// - Added the Telemetry option; V7 later remapped it to Bool1.
// - The telemetry RX window is opened only when this option is ON.
//
// [DEVO V5 DEVIATION ALIGN]
// - Manual bind forces the runtime Fixed-ID bind flags like DeviationTX.
// - Autobind behavior remains unchanged.
// - Bind state is explicitly cleared when the bind countdown reaches zero.
// - CYRF RX_CTRL aligned to DeviationTX (0x80).
//
// [DEVO V4 TELEMETRY] Standard DEVO RX telemetry window added.
// V8 supersedes the temporary V4 voltage mappings: Volt1/2/3 are now ignored.
// CYRF telemetry RSSI remains exposed as OpenAVRc link RSSI.
//

/*
 * Channel Order :
 * Ch1 E
 * Ch2 A
 * Ch3 T
 * Ch4 R
 */

#include "../OpenAVRc.h"

// define pulses2MHz reusable values (13 bytes max)
#define DEVO_RFSTATE             BYTE_P2M(1)
#define DEVO_CH_IDX_P2M          BYTE_P2M(2)
#define DEVO_REC_SEQ_P2M         BYTE_P2M(3)
#define DEVO_XANY_HOLD_CNT_P2M   BYTE_P2M(4) // X-Any DEVO: 4 scheduler calls + 1 hold cycle
#define DEVO_PACKET_COUNT_P2M    BYTE_P2M(5)
#define DEVO_NUM_CHAN_P2M        BYTE_P2M(6)
#define DEVO_PREV_NUM_CHAN_P2M   BYTE_P2M(7)
#define DEVO_FAILSAFE_PKT_P2M    BYTE_P2M(8) // [DEVO V2] beacon upper/lower selector in shared P2M

// [DEVO V4 TELEMETRY]
// Last safe bytes before temp_rfid_addr_p2M (140..143).
// BYTE_P2M(11)=138, BYTE_P2M(12)=139.
#define DEVO_TELEM_STATE_P2M     BYTE_P2M(11)
#define DEVO_USE_FIXED_ID_P2M    BYTE_P2M(12) // runtime bind/fixed-id state

// [DEVO BIND FIX V1]
// WORD_P2M(1) maps to pulses2MHz bytes 140..141, which are ALSO
// temp_rfid_addr_p2M[0..1]. Writing the bind counter there corrupts the
// DEVO protocol ID, exactly like the old DSM overlap fixed previously.
// DEVO uses BYTE_P2M(1..7) = bytes 128..134, so WORD_P2M(3)
// (bytes 136..137) is free.
#define DEVO_BIND_COUNTER_16_P2M WORD_P2M(3)

// The real CYRF6936 manufacturer ID is 6 bytes and must remain available
// after packet_p2M is rebuilt, because DEVO uses it for bind, scrambling
// and the bound SOP/CRC. channel_used_p2M[0..4] are the 3 hopping channels
// plus their two repeated entries, so [5..10] is a free P2M workspace.
#define DEVO_CYRF_MFG_ID_P2M    (&channel_used_p2M[5])
//***********************************************//


const static RfOptionSettingsvar_t RfOpt_DEVO_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED,  //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/4, // Num channel must be 4,8 or 12 -> DEVO_check_num_chan()
  // [DEVO BIND FIX V1] MPM supports other counts too; RX701 uses 7 channels.
  /*rfOptionValue1Max*/12,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,
};

const pm_char DEVO_STR_NUMCH[] PROGMEM = INDENT"N Ch"; // TODO MAKE GLOBAL SHARED
const pm_char DEVO_STR_STATICID[] PROGMEM = INDENT"Static ID";


#define DEVO_PKTS_PER_CHANNEL	4
#define DEVO_BIND_COUNT			0x1388
#define DEVO_NUM_WAIT_LOOPS (100 / 5) //each loop is ~5us.  Do not wait more than 100us

#define DEVO_TELEMETRY (g_model.rfOptionBool1) // [DEVO V7] OpenAVRc convention: telemetry on Bool1.
#define DEVO_AUTOBIND  (g_model.rfOptionBool2)
#define USE_FIXED_ID   (g_model.rfOptionBool3) // For Ladybird Quad.

//static uint8_t *hopping_channel_ptr; // ToDo ... share between protocols.
// [DEVO V2] DEVO_REC_SEQ_P2M is now the hopping index (0..2), avoiding a static AVR pointer.


enum
{
  DEVO_BIND,
  DEVO_BIND_SENDCH,
  DEVO_BOUND,
  DEVO_BOUND_1,
  DEVO_BOUND_2,
  DEVO_BOUND_3,
  DEVO_BOUND_4,
  DEVO_BOUND_5,
  DEVO_BOUND_6,
  DEVO_BOUND_7,
  DEVO_BOUND_8,
  DEVO_BOUND_9,
  DEVO_BOUND_10,
};

static void DEVO_check_num_chan()
{ // GUI changes result in 4, 8 or 12 channels.
  // [DEVO BIND FIX V1] Align with MPM capability and keep the exact channel
  // count requested by the model. This is required for 7-channel RX701.
  if (DEVO_PREV_NUM_CHAN_P2M != g_model.rfOptionValue1)
    {
      DEVO_NUM_CHAN_P2M = g_model.rfOptionValue1;
      if (DEVO_NUM_CHAN_P2M < 4)
        DEVO_NUM_CHAN_P2M = 4;
      else if (DEVO_NUM_CHAN_P2M > 12)
        DEVO_NUM_CHAN_P2M = 12;

      DEVO_PREV_NUM_CHAN_P2M = DEVO_NUM_CHAN_P2M;
      g_model.rfOptionValue1 = DEVO_NUM_CHAN_P2M;
    }
}

static void DEVO_scramble_pkt() // todo remove
{
#ifdef NO_SCRAMBLE
  return;
#else
  for(uint8_t i = 0; i < 15; i++)
    packet_p2M[i + 1] ^= DEVO_CYRF_MFG_ID_P2M[i % 4];
#endif
}

const uint8_t zz_DEVOInitSequence[] PROGMEM =
{
  CYRF_1D_MODE_OVERRIDE, 0x38,      // FRC SEN (forces the synthesizer to start) + FRC AWAKE (force the oscillator to keep running at all times)
  CYRF_03_TX_CFG, 0x08,             // Data Code Length = 32 chip codes + Data Mode = 8DR Mode.
  CYRF_06_RX_CFG, 0x4A,             // LNA + FAST TURN EN + RXOW EN, enable low noise amplifier, fast turning, overwrite enable
  CYRF_0B_PWR_CTRL, 0x00,           // Reset power control
  CYRF_10_FRAMING_CFG, 0xA4,        // SOP EN + SOP LEN = 32 chips + LEN EN + SOP TH = 04h
  CYRF_11_DATA32_THOLD, 0x05,       // TH32 = 0x05
  CYRF_12_DATA64_THOLD, 0x0E,       // TH64 = 0Eh, set pn correlation threshold
  CYRF_1B_TX_OFFSET_LSB, 0x55,      // STRIM LSB = 0x55, typical configuration
  CYRF_1C_TX_OFFSET_MSB, 0x05,      // STRIM MSB = 0x05, typical configuration
  CYRF_32_AUTO_CAL_TIME, 0x3C,      // AUTO_CAL_TIME = 3Ch, typical configuration
  CYRF_35_AUTOCAL_OFFSET, 0x14,     // AUTO_CAL_OFFSET = 14h, typical configuration
  CYRF_39_ANALOG_CTRL, 0x01,        // ALL SLOW
  CYRF_1E_RX_OVERRIDE, 0x10,        // FRC RXDR (Force Receive Data Rate)
  CYRF_1F_TX_OVERRIDE, 0x00,        // Reset TX overrides
  CYRF_01_TX_LENGTH, 0x10,          // TX Length = 16 byte packet_p2M
  CYRF_0F_XACT_CFG, 0x10,           // MPM DEVO init sequence
  CYRF_27_CLK_OVERRIDE, 0x02,       // RXF, force receive clock
  CYRF_28_CLK_EN, 0x02,             // RXF, force receive clock enable
  CYRF_0F_XACT_CFG, 0x28,           // MPM DEVO init sequence: Force TX mode
//  CYRF_0F_XACT_CFG, 0x28,         // Force TX mode
};

static void DDEVO_init()
{
  // Initialise CYRF chip.
  uint_farptr_t pdata = pgm_get_far_address(zz_DEVOInitSequence);
  uint8_t add;
  uint8_t dat;
  for (uint8_t i=0; i<(DIM(zz_DEVOInitSequence)/2); i++)
    {
      add = pgm_read_byte_far(pdata++);
      dat = pgm_read_byte_far(pdata++);
      CYRF_WriteRegister(add,dat);
    }
}

static void DEVO_set_protocol_id_from_mfg()
{
  // MPM DEVO identity:
  //   ((hop0 ^ mfg0 ^ mfg3) << 16) |
  //   ((hop1 ^ mfg1 ^ mfg4) <<  8) |
  //   ((hop2 ^ mfg2 ^ mfg5) <<  0)
  // then modulo 1,000,000.
  uint32_t protocol_id =
      ((uint32_t)(channel_used_p2M[0] ^ DEVO_CYRF_MFG_ID_P2M[0] ^ DEVO_CYRF_MFG_ID_P2M[3]) << 16) |
      ((uint32_t)(channel_used_p2M[1] ^ DEVO_CYRF_MFG_ID_P2M[1] ^ DEVO_CYRF_MFG_ID_P2M[4]) <<  8) |
      ((uint32_t)(channel_used_p2M[2] ^ DEVO_CYRF_MFG_ID_P2M[2] ^ DEVO_CYRF_MFG_ID_P2M[5]) <<  0);

  protocol_id %= 1000000UL;

  // Reuse OpenAVRc's existing 4-byte P2M RF-ID area for the DEVO
  // MProtocol_id. Only the low 24 bits are transmitted in the suffix.
  temp_rfid_addr_p2M[0] = (uint8_t)(protocol_id);
  temp_rfid_addr_p2M[1] = (uint8_t)(protocol_id >> 8);
  temp_rfid_addr_p2M[2] = (uint8_t)(protocol_id >> 16);
  temp_rfid_addr_p2M[3] = 0;
}

static void DEVO_add_pkt_suffix()
{
  uint8_t bind_state;

  if (DEVO_USE_FIXED_ID_P2M) // Runtime Fixed-ID state (DeviationTX behavior).
    {
      if (DEVO_BIND_COUNTER_16_P2M > 0)
        bind_state = 0xC0;
      else
        bind_state = 0x80;
    }
  else // Allows Rx to bind again, but current bind is non-volatile.
    bind_state = 0x00;

  packet_p2M[10] = bind_state | (DEVO_PKTS_PER_CHANNEL - DEVO_PACKET_COUNT_P2M - 1);
  packet_p2M[11] = channel_used_p2M[DEVO_REC_SEQ_P2M + 1];
  packet_p2M[12] = channel_used_p2M[DEVO_REC_SEQ_P2M + 2];
  packet_p2M[13] = temp_rfid_addr_p2M[0];
  packet_p2M[14] = temp_rfid_addr_p2M[1];
  packet_p2M[15] = temp_rfid_addr_p2M[2];
}

static void DEVO_build_beacon_pkt(uint8_t upper)
{
  packet_p2M[0] = ((DEVO_NUM_CHAN_P2M << 4) | 0x07);
  uint8_t enable = 0;
  uint8_t max = 8;
  if (upper)
    {
      packet_p2M[0] += 1;
      max = 4;
    }
  for(uint8_t i = 0; i < max; i++)
    {
      packet_p2M[i+1] = 0;
    }
  packet_p2M[9] = enable;
  DEVO_add_pkt_suffix();
}

static void DEVO_build_bind_pkt()
{
  packet_p2M[0] = (DEVO_NUM_CHAN_P2M << 4) | 0x0a;
  packet_p2M[1] = DEVO_BIND_COUNTER_16_P2M & 0xff;
  packet_p2M[2] = (DEVO_BIND_COUNTER_16_P2M >> 8);
  packet_p2M[3] = channel_used_p2M[DEVO_REC_SEQ_P2M];
  packet_p2M[4] = channel_used_p2M[DEVO_REC_SEQ_P2M + 1];
  packet_p2M[5] = channel_used_p2M[DEVO_REC_SEQ_P2M + 2];
  packet_p2M[6] = DEVO_CYRF_MFG_ID_P2M[0];
  packet_p2M[7] = DEVO_CYRF_MFG_ID_P2M[1];
  packet_p2M[8] = DEVO_CYRF_MFG_ID_P2M[2];
  packet_p2M[9] = DEVO_CYRF_MFG_ID_P2M[3];
  DEVO_add_pkt_suffix();
  // The fixed-id portion is scrambled in the bind packet_p2M
  // I assume it is ignored
  packet_p2M[13] ^= DEVO_CYRF_MFG_ID_P2M[0];
  packet_p2M[14] ^= DEVO_CYRF_MFG_ID_P2M[1];
  packet_p2M[15] ^= DEVO_CYRF_MFG_ID_P2M[2];
}

static void DEVO_build_data_pkt()
{
#if defined(X_ANY)
  /*
   * DEVO receivers refresh their PWM outputs close to 20 ms. With 7/8 DEVO
   * channels, X-Any is normally scheduled every 4.8 ms; Repeat=3 therefore
   * keeps one symbol for only 19.2 ms. The relative phase then slowly drifts
   * and some symbols can be missed.
   *
   * Keep the normal four X-Any scheduler calls, then hold the fourth symbol
   * for one extra DEVO channel cycle. This gives about 24 ms per X-Any symbol
   * without changing Xany.cpp or the behaviour of normal DEVO channels.
   *
   * The counter uses the shared P2M byte previously reserved for the old
   * DEVO_SEND_SEQ state. The held excursions must survive mixer updates, so
   * only these X_ANY values need persistent storage.
   */
  static int16_t XanyDevoHold[X_ANY];

  if (DEVO_CH_IDX_P2M == 0 && DEVO_BIND_COUNTER_16_P2M == 0)
    {
      if (DEVO_XANY_HOLD_CNT_P2M < 4)
        {
          Xany_scheduleTx_AllInstance();
          for (uint8_t x = 0; x < X_ANY; x++)
            if (g_model.Xany[x].Active)
              XanyDevoHold[x] = channelOutputs[g_model.Xany[x].ChId];
          DEVO_XANY_HOLD_CNT_P2M++;
        }
      else
        {
          // Fifth cycle: keep the last scheduled symbol, do not advance X-Any.
          DEVO_XANY_HOLD_CNT_P2M = 0;
        }
    }
#endif

  packet_p2M[0] = (DEVO_NUM_CHAN_P2M << 4) | (0x0b + DEVO_CH_IDX_P2M);
  uint8_t sign = 0x0b;
  for (uint8_t i = 0; i < 4; i++)
    {
      uint8_t ch = i + (4 * DEVO_CH_IDX_P2M);
      int16_t value = FULL_CHANNEL_OUTPUTS(ch);
#if defined(X_ANY)
      uint8_t isXany = 0;

      // A configured X-Any channel uses the held X-Any excursion, never the
      // mixer value, so the extra hold cycle cannot be overwritten.
      for (uint8_t x = 0; x < X_ANY; x++)
        if (g_model.Xany[x].Active && g_model.Xany[x].ChId == ch)
          {
            value = XanyDevoHold[x];
            isXany = 1;
            break;
          }

      if (isXany)
        value <<= 1; // X-Any needs the full ~1024..1976 us DEVO PWM range.
      else
#endif
        value = value + (value >> 1) + (value >> 4); // Normal DEVO range +/-1600.

      if(value < 0)
        {
          value = -value;
          sign |= 1 << (7 - i);
        }
      packet_p2M[2 * i + 1] = value & 0xff;
      packet_p2M[2 * i + 2] = (value >> 8) & 0xff;
    }
  packet_p2M[9] = sign;
  DEVO_CH_IDX_P2M++;
  if (DEVO_CH_IDX_P2M * 4 >= DEVO_NUM_CHAN_P2M)
    DEVO_CH_IDX_P2M = 0;
  DEVO_add_pkt_suffix();
}


// [DEVO V8 TELEMETRY HELPERS]
#if defined(FRSKY) && defined(GPS)
static uint16_t DEVO_text_to_int(const uint8_t *ptr, uint8_t len)
{
  uint16_t value = 0;

  for(uint8_t i = 0; i < len; i++)
    {
      if(ptr[i] < '0' || ptr[i] > '9')
        break;
      value = value * 10U + (uint16_t)(ptr[i] - '0');
    }

  return value;
}

static void DEVO_float_to_ints(const uint8_t *ptr, uint16_t *value, uint16_t *decimal)
{
  *value = 0;
  *decimal = 0;
  uint8_t seen_decimal = 0;

  for(uint8_t i = 0; i < 7; i++)
    {
      if(ptr[i] == 0)
        break;

      if(ptr[i] == '.')
        {
          seen_decimal = 1;
          continue;
        }

      if(ptr[i] < '0' || ptr[i] > '9')
        break;

      if(seen_decimal)
        *decimal = (*decimal * 10U) + (uint16_t)(ptr[i] - '0');
      else
        *value = (*value * 10U) + (uint16_t)(ptr[i] - '0');
    }
}
#endif

// [DEVO V8 TELEMETRY]
// Standard Walkera DEVO telemetry packet types:
//   0x30 = Volt1, Volt2, Volt3, RPM1, RPM2
//   0x31 = Temp1..Temp4
//   0x32 = GPS longitude
//   0x33 = GPS latitude
//   0x34 = altitude
//   0x35 = GPS speed
//   0x36 = GPS date/time
//
// Volt1/2/3 are deliberately NOT mapped: their DEVO meaning is not
// sufficiently defined to assign RxBt/VFAS safely in OpenAVRc.
static void DEVO_parse_telemetry_packet(uint8_t len, uint8_t cyrf_rssi)
{
  if(len < 16)
    return;

  // DEVO telemetry is scrambled with the same manufacturer ID.
  DEVO_scramble_pkt();

  // Reject telemetry from another model / protocol ID.
  const uint32_t rx_id =
      ((uint32_t)packet_p2M[15] << 16) |
      ((uint32_t)packet_p2M[14] << 8) |
      packet_p2M[13];

  const uint32_t protocol_id =
      ((uint32_t)temp_rfid_addr_p2M[2] << 16) |
      ((uint32_t)temp_rfid_addr_p2M[1] << 8) |
      temp_rfid_addr_p2M[0];

  if(((packet_p2M[0] & 0xF0) != 0x30) ||
     (rx_id != (protocol_id & 0x00FFFFFFUL)))
    return;

#if defined(FRSKY)
  frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;

  // CYRF RSSI is the strength of the telemetry packet received by the TX.
  // Standard DEVO has no separate receiver-reported RSSI value.
  uint8_t rssiValue = (uint8_t)(cyrf_rssi + (cyrf_rssi << 1)); // 0..93, like MultiModule DEVO
  telemetryData.rssi[1].set(rssiValue);
  telemetryData.rssi[0].set(rssiValue);

  switch(packet_p2M[0])
    {
    case 0x30: // Voltages + RPM1/RPM2
      // Volt1 / Volt2 / Volt3 intentionally ignored.
      // OpenAVRc exposes one standard RPM field, therefore use DEVO RPM1.
      if(packet_p2M[7] != 0x00 && packet_p2M[7] != 0xFF)
        {
          telemetryData.value.rpm = (uint16_t)packet_p2M[7] * 120U;
          checkMaxRpm();
        }
      // packet_p2M[9] is DEVO RPM2: no second OpenAVRc RPM destination.
      break;

    case 0x31: // Temperature packet
      // DeviationTX stores DEVO temperature as raw value with +20 offset.
      if(packet_p2M[1] != 0xFF)
        {
          telemetryData.value.temperature1 = (int16_t)packet_p2M[1] - 20;
          checkMaxTemperature1();
        }

      if(packet_p2M[2] != 0xFF)
        {
          telemetryData.value.temperature2 = (int16_t)packet_p2M[2] - 20;
          checkMaxTemperature2();
        }

      // packet[3] / packet[4] = Temp3 / Temp4:
      // OpenAVRc has only Temp1 and Temp2 standard fields.
      break;

#if defined(GPS)
    case 0x32: // Longitude: dddmm.mmmm + E/W
      {
        uint16_t bp = DEVO_text_to_int(&packet_p2M[1], 3) * 100U;
        uint32_t minutes =
            (uint32_t)DEVO_text_to_int(&packet_p2M[4], 2) * 10000UL +
            (uint32_t)DEVO_text_to_int(&packet_p2M[7], 4);

        uint16_t whole_minutes = (uint16_t)(minutes / 10000UL);
        bp += whole_minutes;

        telemetryData.value.gpsLongitude_bp = bp;
        telemetryData.value.gpsLongitude_ap =
            (uint16_t)(minutes - ((uint32_t)whole_minutes * 10000UL));
        telemetryData.value.gpsLongitudeEW = packet_p2M[11];
      }
      break;

    case 0x33: // Latitude: ddmm.mmmm + N/S
      {
        uint16_t bp = DEVO_text_to_int(&packet_p2M[1], 2) * 100U;
        uint32_t minutes =
            (uint32_t)DEVO_text_to_int(&packet_p2M[3], 2) * 10000UL +
            (uint32_t)DEVO_text_to_int(&packet_p2M[6], 4);

        uint16_t whole_minutes = (uint16_t)(minutes / 10000UL);
        bp += whole_minutes;

        telemetryData.value.gpsLatitude_bp = bp;
        telemetryData.value.gpsLatitude_ap =
            (uint16_t)(minutes - ((uint32_t)whole_minutes * 10000UL));
        telemetryData.value.gpsLatitudeNS = packet_p2M[10];
      }
      break;

    case 0x34: // Altitude
      {
        uint16_t altitude_bp, altitude_ap;
        DEVO_float_to_ints(&packet_p2M[1], &altitude_bp, &altitude_ap);

        // MultiModule maps DEVO 0x34 to FrSky barometric altitude.
        // OpenAVRc's standard altitude field is integer metres here.
        processHubPacket(BARO_ALT_BP_ID, altitude_bp);
        (void)altitude_ap;
      }
      break;

    case 0x35: // GPS speed
      {
        uint16_t speed_bp, speed_ap;
        DEVO_float_to_ints(&packet_p2M[7], &speed_bp, &speed_ap);

        processHubPacket(GPS_SPEED_BP_ID, speed_bp);
        processHubPacket(GPS_SPEED_AP_ID, speed_ap);
      }
      break;

    case 0x36: // GPS date/time: HHMMSSDDMMYY
      if(packet_p2M[1] != 0)
        {
          uint8_t hour  = (uint8_t)DEVO_text_to_int(&packet_p2M[1], 2);
          uint8_t min   = (uint8_t)DEVO_text_to_int(&packet_p2M[3], 2);
          uint8_t sec   = (uint8_t)DEVO_text_to_int(&packet_p2M[5], 2);
          uint8_t day   = (uint8_t)DEVO_text_to_int(&packet_p2M[7], 2);
          uint8_t month = (uint8_t)DEVO_text_to_int(&packet_p2M[9], 2);
          uint16_t year = 2000U + DEVO_text_to_int(&packet_p2M[11], 2);

          processHubPacket(GPS_DAY_MONTH_ID,
                           (uint16_t)day | ((uint16_t)month << 8));
          processHubPacket(GPS_YEAR_ID, year);
          processHubPacket(GPS_HOUR_MIN_ID,
                           (uint16_t)hour | ((uint16_t)min << 8));
          processHubPacket(GPS_SEC_ID, sec);
        }
      break;
#endif // GPS

    default:
      break;
    }

#else
  (void)cyrf_rssi;
#endif // FRSKY
}

static void DEVO_set_bound_sop_code()
{
  // crc == 0 isn't allowed, so use 1 if the math results in 0.
  uint8_t crc = (DEVO_CYRF_MFG_ID_P2M[0] + (DEVO_CYRF_MFG_ID_P2M[1] >> 6) + DEVO_CYRF_MFG_ID_P2M[2]);
  if(! crc)
    crc = 1;
  uint8_t sopidx = (0xff &((DEVO_CYRF_MFG_ID_P2M[0] << 2) + DEVO_CYRF_MFG_ID_P2M[1] + DEVO_CYRF_MFG_ID_P2M[2])) % 10;

  CYRF_SetTxRxMode(TX_EN);
  CYRF_ConfigCRCSeed((crc << 8) + crc);
  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(sopidx);

  // [DEVO V2 POWER FIX]
  // CYRF_03_TX_CFG bits 2:0 are PA power. Writing 0x08 here, as the old
  // OpenAVRc code did, resets them to CYRF power 0 just when bind finishes.
  // Keep the current PA bits while restoring DEVO 8DR/data-code mode.
  uint8_t txCfg = CYRF_ReadRegister(CYRF_03_TX_CFG);
  CYRF_WriteRegister(CYRF_03_TX_CFG, (txCfg & 0x07) | 0x08);
}

static void DEVO_BuildPacket()
{
  //static uint8_t failsafe_pkt=0;  // TODO What this ?? USED ???????????????????????????? 8-{D
  // [DEVO V2] Stored in DEVO_FAILSAFE_PKT_P2M instead of static AVR SRAM.

  switch(DEVO_RFSTATE)
    {
    case DEVO_BIND:
      if(DEVO_BIND_COUNTER_16_P2M)
        DEVO_BIND_COUNTER_16_P2M--;
      DEVO_build_bind_pkt();
      DEVO_RFSTATE = DEVO_BIND_SENDCH;
      break;
    case DEVO_BIND_SENDCH:
      if(DEVO_BIND_COUNTER_16_P2M)
        DEVO_BIND_COUNTER_16_P2M--;
      DEVO_build_data_pkt();
      DEVO_scramble_pkt();
      if (DEVO_BIND_COUNTER_16_P2M == 0)
        {
          DEVO_RFSTATE = DEVO_BOUND;
          PROTOCOL_SetBindState(0); // [DEVO V5] Bind done, like DeviationTX.
          /* Bind done */
        }
      else
        DEVO_RFSTATE = DEVO_BIND;
      break;
    case DEVO_BOUND:
    case DEVO_BOUND_1:
    case DEVO_BOUND_2:
    case DEVO_BOUND_3:
    case DEVO_BOUND_4:
    case DEVO_BOUND_5:
    case DEVO_BOUND_6:
    case DEVO_BOUND_7:
    case DEVO_BOUND_8:
    case DEVO_BOUND_9:
      DEVO_build_data_pkt();
      DEVO_scramble_pkt();
      DEVO_RFSTATE++;
      if (DEVO_RFSTATE == DEVO_BOUND_1)
        {
          SCHEDULE_MIXER_END_IN_US(24000); // Schedule next Mixer calculations.
        }
      break;
    case DEVO_BOUND_10:
      DEVO_build_beacon_pkt(DEVO_NUM_CHAN_P2M > 8 ? DEVO_FAILSAFE_PKT_P2M : 0);
      DEVO_FAILSAFE_PKT_P2M = DEVO_FAILSAFE_PKT_P2M ? 0 : 1;
      DEVO_scramble_pkt();
      DEVO_RFSTATE = DEVO_BOUND_1;
      break;
    }
  DEVO_PACKET_COUNT_P2M++;
  if(DEVO_PACKET_COUNT_P2M == DEVO_PKTS_PER_CHANNEL)
    DEVO_PACKET_COUNT_P2M = 0;
}

static void DEVO_set_radio_channels()
{
  CYRF_FindBestChannels(channel_used_p2M, 3, 4, 4, 80);
  channel_used_p2M[3] = channel_used_p2M[0];
  channel_used_p2M[4] = channel_used_p2M[1];
}

uint16_t DEVO_cb()
{
  // [DEVO V4 TELEMETRY]
  // Keep the complete DEVO period at 2.4ms:
  //   TX 900us + RX window 1300us + return-to-TX 200us = 2400us
  // or, when no RX slot is opened:
  //   TX 900us + remaining 1500us = 2400us.
  if (DEVO_TELEM_STATE_P2M == 0)
    {
      DEVO_BuildPacket();
      CYRF_WriteDataPacket(packet_p2M);
      DEVO_TELEM_STATE_P2M = 1;

      heartbeat |= HEART_TIMER_PULSES;
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      return 900*2;
    }

  if (DEVO_TELEM_STATE_P2M == 1)
    {
      uint8_t i = 0;
      uint8_t tx_irq = 0;

      while (! ((tx_irq = CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS)) & 0x02))
        {
          if(++i > DEVO_NUM_WAIT_LOOPS)
            break;
        }

      // Same recovery principle as current MultiModule DEVO telemetry:
      // if the CYRF is stuck in TX, reinitialise it and continue next cycle.
      if (((tx_irq & 0x22) == 0x20) ||
          (CYRF_ReadRegister(CYRF_02_TX_CTRL) & 0x80))
        {
          CYRF_Reset();
          DDEVO_init();
          DEVO_set_bound_sop_code();
          CYRF_ConfigRFChannel(channel_used_p2M[DEVO_REC_SEQ_P2M]);
          DEVO_TELEM_STATE_P2M = 0;
          return 1500*2;
        }

      if (DEVO_RFSTATE == DEVO_BOUND)
        {
          /* exit binding state */
          DEVO_RFSTATE = DEVO_BOUND_3;
          DEVO_set_bound_sop_code();
        }

      // Open a telemetry RX window only between normal bound packets.
      // Never listen during the bind countdown, and keep the fourth packet
      // slot for the normal channel hop exactly as before.
      if (DEVO_TELEMETRY &&
          (DEVO_PACKET_COUNT_P2M != 0) &&
          (DEVO_BIND_COUNTER_16_P2M == 0))
        {
          CYRF_SetTxRxMode(RX_EN);
          CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x80); // DeviationTX DEVO telemetry RX
          DEVO_TELEM_STATE_P2M = 2;
          return 1300*2;
        }

      if(DEVO_PACKET_COUNT_P2M == 0)
        {
          CYRF_ManagePower();      // Keep transmit power in sync
          DEVO_check_num_chan();   // Check num channels
          DEVO_REC_SEQ_P2M++;
          if (DEVO_REC_SEQ_P2M >= 3)
            DEVO_REC_SEQ_P2M = 0;
          CYRF_ConfigRFChannel(channel_used_p2M[DEVO_REC_SEQ_P2M]);
        }

      DEVO_TELEM_STATE_P2M = 0;
      return 1500*2;
    }

  // DEVO_TELEM_STATE_P2M == 2 : end of telemetry RX window.
  uint8_t rx_state = CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);

  if((rx_state & 0x03) == 0x02)
    rx_state |= CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);

  if((rx_state & 0x07) == 0x02)
    {
      CYRF_WriteRegister(CYRF_07_RX_IRQ_STATUS, 0x80); // RXOW before read

      uint8_t len = CYRF_ReadRegister(CYRF_09_RX_COUNT);
      if(len > 16)
        len = 16;

      CYRF_ReadDataPacketLen(packet_p2M, len);
      uint8_t cyrf_rssi = CYRF_ReadRegister(CYRF_13_RSSI) & 0x1F;
      DEVO_parse_telemetry_packet(len, cyrf_rssi);
    }

  CYRF_SetTxRxMode(TX_EN);
  DEVO_TELEM_STATE_P2M = 0;
  return 200*2;
}

void DEVOInit(uint8_t bind)
{
  DEVO_CH_IDX_P2M = 0;
  DEVO_REC_SEQ_P2M = 0;
  DEVO_XANY_HOLD_CNT_P2M = 0;
  DEVO_PACKET_COUNT_P2M = 0;
  DEVO_FAILSAFE_PKT_P2M = 0;
  DEVO_TELEM_STATE_P2M = 0;
  DEVO_USE_FIXED_ID_P2M = USE_FIXED_ID ? 1 : 0;

  DEVO_NUM_CHAN_P2M = g_model.rfOptionValue1;
  if (DEVO_NUM_CHAN_P2M < 4)
    DEVO_NUM_CHAN_P2M = 4;
  else if (DEVO_NUM_CHAN_P2M > 12)
    DEVO_NUM_CHAN_P2M = 12;

  DEVO_PREV_NUM_CHAN_P2M = DEVO_NUM_CHAN_P2M;
  g_model.rfOptionValue1 = DEVO_NUM_CHAN_P2M;

  // Load temp_rfid_addr_p2M + Model match.
  // [DEVO BIND FIX V1] For Static ID keep the existing OpenAVRc model ID
  // in temp_rfid_addr_p2M; otherwise MPM derives the protocol ID from the
  // real CYRF manufacturer ID and the three selected hopping channels.
  if (USE_FIXED_ID)
    loadrfidaddr_rxnum(0);

  CYRF_Reset();
  DDEVO_init();

  // MPM reads the real six-byte CYRF6936 manufacturer ID after CYRF init.
  // Keep it in free P2M channel storage, not in a new RAM buffer.
  CYRF_GetMfgData(DEVO_CYRF_MFG_ID_P2M);

  CYRF_ConfigCRCSeed(0x0000);
  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(0);
  DEVO_set_radio_channels();

  if (!USE_FIXED_ID)
    DEVO_set_protocol_id_from_mfg();

  //hopping_channel_ptr = &channel_used_p2M[0];
  DEVO_REC_SEQ_P2M = 0;
  CYRF_ConfigRFChannel(channel_used_p2M[DEVO_REC_SEQ_P2M]);

  CYRF_SetTxRxMode(TX_EN);

  if (bind || DEVO_AUTOBIND)
    {
      DEVO_BIND_COUNTER_16_P2M = DEVO_BIND_COUNT;
      DEVO_RFSTATE = DEVO_BIND;

      // [DEVO V5] DeviationTX forces Fixed-ID bind flags for an explicit
      // manual bind. Keep Autobind unchanged: it follows the model option.
      // The protocol ID itself was already loaded or derived above.
      if (bind)
        DEVO_USE_FIXED_ID_P2M = 1;

      PROTOCOL_SetBindState(1200); // 12 Sec

      /* Bind in Progress */
    }
  else
    {
      DEVO_RFSTATE = DEVO_BOUND_1;
      DEVO_BIND_COUNTER_16_P2M = 0;
      DEVO_set_bound_sop_code();
    }
  PROTO_Start_Callback( DEVO_cb);
}

const void *DEVO_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
    {
    case PROTOCMD_INIT:
      DEVOInit(0);
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      CYRF_Reset();
      return 0;
    case PROTOCMD_BIND:
      DEVOInit(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_DEVO_Ser),
                          STR_DUMMY,      //Sub proto
                          DEVO_STR_NUMCH,      //Option 1 (int)
                          STR_DUMMY,      //Option 2 (int)
                          STR_RFPOWER,       //Rf power
                          STR_TELEMETRY,         //OptionBool 1 - [DEVO V7]
                          STR_AUTOBIND,          //OptionBool 2
                          DEVO_STR_STATICID      //OptionBool 3
                         );
      return 0;
    default:
      break;
    }
  return 0;
}
