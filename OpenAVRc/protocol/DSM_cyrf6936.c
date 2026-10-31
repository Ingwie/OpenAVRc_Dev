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
*                                                                         *
**************************************************************************
*/

// Multiprotocol inspired. Thanks a lot !
//
// V16:
// - strict V7 POWER1 bind/RF base;
// - bind packet, DSMX hopping, RF ID, channel maps and TX timing unchanged;
// - OptionBool1 = Telemetry;
// - OptionBool2 = Autobind;
// - telemetry RX is enabled only in X_22 / X_11 when Telemetry is ON;
// - CYRF RSSI -> FrSky Tx;
// - Spektrum 0x7F QoS: Rx RSSI only when the frame really carries a RSSI value
//   (Lemon-style signature); standard Spektrum fade counters are not faked as RSSI;
// - Spektrum 0x7F QoS receiver voltage -> OpenAVRc RxBt;
// - Spektrum 0x40 Vario: altitude -> OpenAVRc Alt, vertical speed -> OpenAVRc VitV;
// - V19: temporary DSM/CYRF work buffers reuse packet_p2M to reduce AVR stack use;
// - V20: Spektrum 0x18/0x20 voltage/current -> OpenAVRc VFAS/Curr;
// - V21: Spektrum 0x16/0x17 GPS -> OpenAVRc GPS data + GPS time RTC sync;
// - V22: Spektrum 0x26 binary GPS -> OpenAVRc GPS data;
// - V23: Spektrum 0x3A LiPoMon -> individual cells C1..C6;
// - V23: Spektrum 0x18 chargeUsed -> OpenAVRc Conso;
// - V24: telemetry RX enabled for DSM2 and DSMX;
// - V24: Spektrum 0x7E dBm_A/dBm_B -> OpenAVRc Rx RSSI;
// - synthetic cells from VFAS removed; Power kept from VFAS x Curr.
// - CALCULATE_LAT_JIT() retained in the telemetry path for Protocol debug.
//

#include "../OpenAVRc.h"

// define pulses2MHz reusable values (13 bytes max)
#define DSM_RFSTATE             BYTE_P2M(1)
#define DSM_CH_IDX_P2M          BYTE_P2M(2)
#define DSM_SOP_COL_P2M         BYTE_P2M(3)
#define DSM_NUM_CH_P2M          BYTE_P2M(4)
#define DSM_PREV_NUM_CHAN_P2M   BYTE_P2M(5)
#if defined(GPS)
#define DSM_GPS_ALT_HIGH_P2M    BYTE_P2M(6) // Spektrum 0x17 altitude high, BCD 0..99
#endif

// [DSM BIND FIX V5]
// WORD_P2M(1) maps to pulses2MHz bytes 140..141, which are ALSO
// temp_rfid_addr_p2M[0..1]. That corrupted the DSM transmitter ID as soon
// as the bind counter was written/decremented.
//
// DSM only uses BYTE_P2M(1..5) = bytes 128..132, so WORD_P2M(4)
// (bytes 134..135) is free and keeps the 4-byte RF ID at 140..143 intact.
#define DSM_BIND_COUNTER_16_P2M WORD_P2M(4)
//***********************************************//


//#define DSM_TELEMETRY Todo ...
// V13: do NOT define DSM_TELEMETRY globally; that would alter the bind flow.
// V24: telemetry RX is available in both DSM2 and DSMX modes.
// Keep the existing macro name to avoid touching the surrounding state machine.
#define DSM_TELEMETRY_ENABLED (g_model.rfOptionBool1 &&                                  \
                                 ((g_model.rfSubType==DSM2_22) || (g_model.rfSubType==DSM2_11) || \
                                  (g_model.rfSubType==DSMX_22) || (g_model.rfSubType==DSMX_11)))

#define DSM_BIND_CHANNEL 0x0d //13 This can be any odd channel

#define DSM_NUM_WAIT_LOOPS (100 / 5)   //each loop is ~5us.  Do not wait more than 100us TODO measure


const static RfOptionSettingsvar_t RfOpt_DSM_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED | BOOL2USED, // BOOL1=Telemetry, BOOL2=Autobind
  /*rfSubTypeMax*/4,
  /*rfOptionValue1Min*/4,    // Number of channels
  /*rfOptionValue1Max*/12,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,    // RF POWER
};

const pm_char STR_SUBTYPE_DSMSPI[] PROGMEM = "2_22""2_11""X_22""X_11""AUTO";

enum DSMOPTIONS
{
  DSM2_22	= 0,
  DSM2_11	= 1,
  DSMX_22	= 2,
  DSMX_11	= 3,
  DSM_AUTO = 4
};

// During binding, send one bind packet every 10 ms.
// [DSM BIND FIX 2026] Aligned with current DIY-Multiprotocol-TX-Module:
// 180 bind packets (~1.8 s), then up to ~4.2 s waiting for a bind response.
// Some receivers need the bind channel to become quiet before replying.
#define DSM_BIND_COUNT       180
#define DSM_BIND_COUNT_READ  600

enum
{
  DSM_BIND_WRITE=0,
  DSM_BIND_CHECK,
  DSM_BIND_READ,
  DSM_CHANSEL,
  DSM_CH1_WRITE_A,
  DSM_CH1_CHECK_A,
  DSM_CH2_WRITE_A,
  DSM_CH2_CHECK_A,
  DSM_CH2_READ_A,
  DSM_CH1_WRITE_B,
  DSM_CH1_CHECK_B,
  DSM_CH2_WRITE_B,
  DSM_CH2_CHECK_B,
  DSM_CH2_READ_B,
};

////////////////////////////////////////////////
uint8_t * ch_map = &pulses2MHz.pbyte[20]; // Use packet_p2M[20 to 34]


const uint8_t PROGMEM zzch_map_progmem[] =
{
//22+11ms for 4..7 channels
  1, 0, 2, 3, 0xff, 0xff, 0xff, 1,    0,    2,    3, 0xff, 0xff,    0xff, //4ch  - Guess
  1, 0, 2, 3, 4,    0xff, 0xff, 1,    0,    2,    3,    4, 0xff,    0xff, //5ch  - Guess
  1, 5, 2, 3, 0,    4,    0xff, 1,    5,    2,    3,    0,    4,    0xff, //6ch  - HP6DSM
  1, 5, 2, 4, 3,    6,    0,    1,    5,    2,    4,    3,    6,    0   , //7ch  - DX6i
//22ms for 8..12 channels
  1, 5, 2, 3, 6,    0xff, 0xff, 4,    0,    7,    0xff, 0xff, 0xff, 0xff, //8ch  - DX8/DX7
  1, 5, 2, 3, 6,    0xff, 0xff, 4,    0,    7,    8,    0xff, 0xff, 0xff, //9ch  - Guess
  1, 5, 2, 3, 6,    0xff, 0xff, 4,    0,    7,    8,    9,    0xff, 0xff, //10ch - Guess
  1, 5, 2, 3, 6,    10,   0xff, 4,    0,    7,    8,    9,    0xff, 0xff, //11ch - Guess
  1, 5, 2, 4, 6,    10,   0xff, 0,    7,    3,    8,    9, 11, 0xff,      //12ch - DX18
//11ms for 8..12 channels
  1, 5, 2, 3, 6,    7,    0xff, 1,    5,    2,    4,    0,    0xff, 0xff, //8ch  - DX7
  1, 5, 2, 3, 6,    7,    0xff, 1,    5,    2,    4,    0,    8,    0xff, //9ch  - Guess
  1, 5, 2, 3, 4,    8,    9,    1,    5,    2,    3,     0,   7,    6   , //10ch - DX18
};

const uint8_t PROGMEM zzpncodes[5][8][8] =
{
  /* Note these are in order transmitted (LSB 1st) */
   /* Row 0 */
    /* Col 0 */ 0x03, 0xBC, 0x6E, 0x8A, 0xEF, 0xBD, 0xFE, 0xF8,
    /* Col 1 */ 0x88, 0x17, 0x13, 0x3B, 0x2D, 0xBF, 0x06, 0xD6,
    /* Col 2 */ 0xF1, 0x94, 0x30, 0x21, 0xA1, 0x1C, 0x88, 0xA9,
    /* Col 3 */ 0xD0, 0xD2, 0x8E, 0xBC, 0x82, 0x2F, 0xE3, 0xB4,
    /* Col 4 */ 0x8C, 0xFA, 0x47, 0x9B, 0x83, 0xA5, 0x66, 0xD0,
    /* Col 5 */ 0x07, 0xBD, 0x9F, 0x26, 0xC8, 0x31, 0x0F, 0xB8,
    /* Col 6 */ 0xEF, 0x03, 0x95, 0x89, 0xB4, 0x71, 0x61, 0x9D,
    /* Col 7 */ 0x40, 0xBA, 0x97, 0xD5, 0x86, 0x4F, 0xCC, 0xD1,
    /* Col 8    0xD7, 0xA1, 0x54, 0xB1, 0x5E, 0x89, 0xAE, 0x86*/
   /* Row 1 */
    /* Col 0 */ 0x83, 0xF7, 0xA8, 0x2D, 0x7A, 0x44, 0x64, 0xD3,
    /* Col 1 */ 0x3F, 0x2C, 0x4E, 0xAA, 0x71, 0x48, 0x7A, 0xC9,
    /* Col 2 */ 0x17, 0xFF, 0x9E, 0x21, 0x36, 0x90, 0xC7, 0x82,
    /* Col 3 */ 0xBC, 0x5D, 0x9A, 0x5B, 0xEE, 0x7F, 0x42, 0xEB,
    /* Col 4 */ 0x24, 0xF5, 0xDD, 0xF8, 0x7A, 0x77, 0x74, 0xE7,
    /* Col 5 */ 0x3D, 0x70, 0x7C, 0x94, 0xDC, 0x84, 0xAD, 0x95,
    /* Col 6 */ 0x1E, 0x6A, 0xF0, 0x37, 0x52, 0x7B, 0x11, 0xD4,
    /* Col 7 */ 0x62, 0xF5, 0x2B, 0xAA, 0xFC, 0x33, 0xBF, 0xAF,
    /* Col 8    0x40, 0x56, 0x32, 0xD9, 0x0F, 0xD9, 0x5D, 0x97 */
   /* Row 2 */
    /* Col 0 */ 0x40, 0x56, 0x32, 0xD9, 0x0F, 0xD9, 0x5D, 0x97,
    /* Col 1 */ 0x8E, 0x4A, 0xD0, 0xA9, 0xA7, 0xFF, 0x20, 0xCA,
    /* Col 2 */ 0x4C, 0x97, 0x9D, 0xBF, 0xB8, 0x3D, 0xB5, 0xBE,
    /* Col 3 */ 0x0C, 0x5D, 0x24, 0x30, 0x9F, 0xCA, 0x6D, 0xBD,
    /* Col 4 */ 0x50, 0x14, 0x33, 0xDE, 0xF1, 0x78, 0x95, 0xAD,
    /* Col 5 */ 0x0C, 0x3C, 0xFA, 0xF9, 0xF0, 0xF2, 0x10, 0xC9,
    /* Col 6 */ 0xF4, 0xDA, 0x06, 0xDB, 0xBF, 0x4E, 0x6F, 0xB3,
    /* Col 7 */ 0x9E, 0x08, 0xD1, 0xAE, 0x59, 0x5E, 0xE8, 0xF0,
    /* Col 8    0xC0, 0x90, 0x8F, 0xBB, 0x7C, 0x8E, 0x2B, 0x8E */
   /* Row 3 */
    /* Col 0 */ 0xC0, 0x90, 0x8F, 0xBB, 0x7C, 0x8E, 0x2B, 0x8E,
    /* Col 1 */ 0x80, 0x69, 0x26, 0x80, 0x08, 0xF8, 0x49, 0xE7,
    /* Col 2 */ 0x7D, 0x2D, 0x49, 0x54, 0xD0, 0x80, 0x40, 0xC1,
    /* Col 3 */ 0xB6, 0xF2, 0xE6, 0x1B, 0x80, 0x5A, 0x36, 0xB4,
    /* Col 4 */ 0x42, 0xAE, 0x9C, 0x1C, 0xDA, 0x67, 0x05, 0xF6,
    /* Col 5 */ 0x9B, 0x75, 0xF7, 0xE0, 0x14, 0x8D, 0xB5, 0x80,
    /* Col 6 */ 0xBF, 0x54, 0x98, 0xB9, 0xB7, 0x30, 0x5A, 0x88,
    /* Col 7 */ 0x35, 0xD1, 0xFC, 0x97, 0x23, 0xD4, 0xC9, 0x88,
    /* Col 8    0xE1, 0xD6, 0x31, 0x26, 0x5F, 0xBD, 0x40, 0x93 */
// Wrong values used by Orange TX/RX
//		/* Col 8 */ 0x88, 0xE1, 0xD6, 0x31, 0x26, 0x5F, 0xBD, 0x40
   /* Row 4 */
    /* Col 0 */ 0xE1, 0xD6, 0x31, 0x26, 0x5F, 0xBD, 0x40, 0x93,
    /* Col 1 */ 0xDC, 0x68, 0x08, 0x99, 0x97, 0xAE, 0xAF, 0x8C,
    /* Col 2 */ 0xC3, 0x0E, 0x01, 0x16, 0x0E, 0x32, 0x06, 0xBA,
    /* Col 3 */ 0xE0, 0x83, 0x01, 0xFA, 0xAB, 0x3E, 0x8F, 0xAC,
    /* Col 4 */ 0x5C, 0xD5, 0x9C, 0xB8, 0x46, 0x9C, 0x7D, 0x84,
    /* Col 5 */ 0xF1, 0xC6, 0xFE, 0x5C, 0x9D, 0xA5, 0x4F, 0xB7,
    /* Col 6 */ 0x58, 0xB5, 0xB3, 0xDD, 0x0E, 0x28, 0xF1, 0xB0,
    /* Col 7 */ 0x5F, 0x30, 0x3B, 0x56, 0x96, 0x45, 0xF4, 0xA1,
    /* Col 8    0x03, 0xBC, 0x6E, 0x8A, 0xEF, 0xBD, 0xFE, 0xF8 */
};

const uint8_t PROGMEM zzinit_vals[][2] =
{
  CYRF_02_TX_CTRL, 0x00,				// All TX interrupt disabled
  CYRF_05_RX_CTRL, 0x00,				// All RX interrupt disabled
  CYRF_28_CLK_EN, 0x02,					// Force receive clock enable
  CYRF_32_AUTO_CAL_TIME, 0x3c,			// Default init value
  CYRF_35_AUTOCAL_OFFSET, 0x14,			// Default init value
  CYRF_26_XTAL_CFG, 0x08,				// [DSM BIND FIX 2026] Start delay, as current MPM
  CYRF_06_RX_CFG, 0x4A,					// LNA enabled, RX override enabled, Fast turn mode enabled, RX is 1MHz below TX
  CYRF_1B_TX_OFFSET_LSB, 0x55,			// Default init value
  CYRF_1C_TX_OFFSET_MSB, 0x05,			// Default init value
  CYRF_39_ANALOG_CTRL, 0x01,			// All slow for synth setting time
  CYRF_01_TX_LENGTH, 0x10,				// 16 bytes packet
  CYRF_14_EOP_CTRL, 0x02,				// Set EOP Symbol Count to 2
  CYRF_12_DATA64_THOLD, 0x0a,			// 64 Chip Data PN corelator threshold, default datasheet value is 0x0E
  //Below is for bind only
  CYRF_03_TX_CFG, 0x38 | TXPOWER_1, // [DSM BIND FIX V7] POWER1 validated with AR6610T/CM703 bind
  CYRF_10_FRAMING_CFG, 0x4a,			// SOP disabled, no LEN field and SOP correlator of 0x0a but since SOP is disabled...
  CYRF_1F_TX_OVERRIDE, 0x04,			// Disable TX CRC, no ACK, use TX synthesizer
  CYRF_1E_RX_OVERRIDE, 0x14,			// Disable RX CRC, Force receive data rate, use RX synthesizer
};

const uint8_t PROGMEM zzdata_vals[][2] =
{
  CYRF_29_RX_ABORT, 0x20,				// Abort RX operation in case we are coming from bind
  CYRF_0F_XACT_CFG, 0x24,				// Force Idle
  CYRF_29_RX_ABORT, 0x00,				// Clear abort RX
  CYRF_03_TX_CFG, 0x28 | TXPOWER_1, // 64 chip codes, 8DR mode
  CYRF_10_FRAMING_CFG, 0xea,			// SOP enabled, SOP_CODE_ADR 64 chips, Packet len enabled, SOP correlator 0x0A
  CYRF_1F_TX_OVERRIDE, 0x00,			// CRC16 enabled, no ACK
  CYRF_1E_RX_OVERRIDE, 0x00,			// CRC16 enabled, no ACK
};

const uint8_t zzdatainitcode[] PROGMEM = {0xD7,0xA1,0x54,0xB1,0x5E,0x89,0xAE,0x86,0xc6,0x94,0x22,0xfe,0x48,0xe6,0x57,0x4e};
const uint8_t zzdatainitcodetele[] PROGMEM = {0x98,0x88,0x1B,0xE4,0x30,0x79,0x03,0x84,0xC9,0x2C,0x06,0x93,0x86,0xB9,0x9E,0xD7};

static void read_code(uint8_t *buf, uint8_t row, uint8_t col, uint8_t len)
{
  uint_farptr_t pncodes = pgm_get_far_address(zzpncodes);
  pncodes += (64*row + 8*col); // Find good adress

  for(uint8_t i=0; i<len; i++)
    {
      buf[i] = pgm_read_byte_far(pncodes+i);
    }
}

static uint8_t get_pn_row(uint8_t channel)
{
  return ((g_model.rfSubType == DSMX_11 || g_model.rfSubType == DSMX_22 )? (channel - 2) % 5 : channel % 5);
}

static void DSM_cyrf_config()
{
  uint_farptr_t init_vals = pgm_get_far_address(zzinit_vals);

  uint8_t reg, val;
  for(uint8_t i = 0; i<(sizeof(zzinit_vals)/2); i++)
    {
      reg = pgm_read_byte_far(init_vals++);
      val = pgm_read_byte_far(init_vals++);
      CYRF_WriteRegister(reg, val);
    }
  CYRF_WritePreamble(0x333304);
  CYRF_ConfigRFChannel(0x61);
}

static void DSM_build_bind_packet()
{
  uint8_t i;
  uint16_t sum = 384 - 0x10;//
  packet_p2M[0] = 0xff ^ temp_rfid_addr_p2M[0];
  packet_p2M[1] = 0xff ^ temp_rfid_addr_p2M[1];
  packet_p2M[2] = 0xff ^ temp_rfid_addr_p2M[2];
  packet_p2M[3] = 0xff ^ temp_rfid_addr_p2M[3];
  packet_p2M[4] = packet_p2M[0];
  packet_p2M[5] = packet_p2M[1];
  packet_p2M[6] = packet_p2M[2];
  packet_p2M[7] = packet_p2M[3];
  for(i = 0; i < 8; i++)
    sum += packet_p2M[i];
  packet_p2M[8] = sum >> 8;
  packet_p2M[9] = sum & 0xff;
  packet_p2M[10] = 0x01; //???
  packet_p2M[11] = DSM_NUM_CH_P2M;

  if (g_model.rfSubType==DSM2_22)
    packet_p2M[12]=DSM_NUM_CH_P2M<8?0x01:0x02;	// DSM2/1024 1 or 2 packets depending on the number of channels
  if(g_model.rfSubType==DSM2_11)
    packet_p2M[12]=0x12;					// DSM2/2048 2 packets
  if(g_model.rfSubType==DSMX_22)
    // [DSM BIND FIX V3]
    // Match the current MPM DSMX bind capability byte used with telemetry-capable RX.
    packet_p2M[12] = 0xb2;
  if(g_model.rfSubType==DSMX_11 || g_model.rfSubType==DSM_AUTO) // Force DSMX/1024 in mode Auto
    packet_p2M[12]=0xb2;					// DSMX/1024 2 packets

  packet_p2M[13] = 0x00; //???
  for(i = 8; i < 14; i++)
    sum += packet_p2M[i];
  packet_p2M[14] = sum >> 8;
  packet_p2M[15] = sum & 0xff;
}

static void DSM_initialize_bind_phase()
{
  uint_farptr_t dataadr = pgm_get_far_address(zzdatainitcode);

  // V19 P2M_OPT: packet_p2M is only a temporary workspace here.
  // DSM_build_bind_packet() overwrites it immediately afterwards.
  for(uint8_t i = 0; i<16; i++)
    {
      packet_p2M[i] = pgm_read_byte_far(dataadr++);
    }
  CYRF_ConfigRFChannel(DSM_BIND_CHANNEL); //This seems to be random?
  //64 SDR Mode is configured so only the 8 first values are needed but need to write 16 values...
  CYRF_ConfigDataCode(packet_p2M, 16);
  DSM_build_bind_packet();
}

static void DSM_cyrf_configdata()
{
  uint_farptr_t data_vals = pgm_get_far_address(zzdata_vals);

  uint8_t reg, val;
  for(uint8_t i=0; i<(sizeof(zzdata_vals)/2); i++)
    {
      reg = pgm_read_byte_far(data_vals++);
      val = pgm_read_byte_far(data_vals++);
      CYRF_WriteRegister(reg, val);
    }
}

static void DSM_update_channels()
{
  DSM_PREV_NUM_CHAN_P2M=g_model.rfOptionValue1;
  if(g_model.rfSubType==DSM_AUTO)
    // [DSM BIND FIX V2] AUTO is left unchanged in this validation build.
    // Validate first with explicit DSMX_22 (X_22). OpenAVRc currently has
    // no complete MPM-style AUTO bind-response negotiation enabled.
    DSM_NUM_CH_P2M=12;						// Force 12 channels in mode Auto
  else
    DSM_NUM_CH_P2M=g_model.rfOptionValue1;
  if(DSM_NUM_CH_P2M<4 || DSM_NUM_CH_P2M>12)
    DSM_NUM_CH_P2M=6;						// Default to 6 channels if invalid choice...

  // Create channel map based on number of channels and refresh rate
  uint8_t idx = DSM_NUM_CH_P2M - 4;
  if(DSM_NUM_CH_P2M>7 && DSM_NUM_CH_P2M<11 && (g_model.rfSubType==DSM2_11 || g_model.rfSubType==DSMX_11))
    idx+=5;								// In 11ms mode change index only for channels 8..10

  uint_farptr_t ch_map_progmem = pgm_get_far_address(zzch_map_progmem);
  ch_map_progmem += idx*14;

  for(uint8_t i=0; i<14; i++)
    {
      ch_map[i] = pgm_read_byte_far(ch_map_progmem+i);
    }
}

static void DSM_build_data_packet(uint8_t upper)
{
#if defined(X_ANY)
  if (!upper) Xany_scheduleTx_AllInstance();
#endif
  uint8_t bits = 11;

  if(DSM_PREV_NUM_CHAN_P2M!=g_model.rfOptionValue1)
    DSM_update_channels();

  if (g_model.rfSubType==DSMX_11 || g_model.rfSubType==DSMX_22 )
    {
      packet_p2M[0] = temp_rfid_addr_p2M[2];
      packet_p2M[1] = temp_rfid_addr_p2M[3];
    }
  else
    {
      packet_p2M[0] = (0xff ^ temp_rfid_addr_p2M[2]);
      packet_p2M[1] = (0xff ^ temp_rfid_addr_p2M[3]);
      if(g_model.rfSubType==DSM2_22)
        bits=10;						// Only DSM_22 is using a resolution of 1024
    }

  for (uint8_t i = 0; i < 7; i++)
    {
      uint8_t idx = ch_map[(upper?7:0) + i];//1,5,2,3,0,4
      uint16_t value = 0xffff;;
      if (idx != 0xff)
        {
          int16_t pvalue = FULL_CHANNEL_OUTPUTS(i);
          pvalue -= (pvalue>>2); // x-x/4 -> Range -1024..+1024 @ 125%
          pvalue += 1024;
          value=limit<uint16_t>(1, pvalue, 2046);

          if(bits==10)
            value>>=1;
          value |= (upper && i==0 ? 0x8000 : 0) | (idx << bits);
        }
      packet_p2M[i*2+2] = (value >> 8) & 0xff;
      packet_p2M[i*2+3] = (value >> 0) & 0xff;
    }
}

static void DSM_set_sop_data_crc()
{
  //The crc for channel '1' is NOT(rfid[0] << 8 + rfid[1])
  //The crc for channel '2' is (rfid[0] << 8 + rfid[1])
  uint16_t crc = (temp_rfid_addr_p2M[0] << 8) + temp_rfid_addr_p2M[1];
  if(DSM_RFSTATE==DSM_CH1_CHECK_A||DSM_RFSTATE==DSM_CH1_CHECK_B)
    CYRF_ConfigCRCSeed(crc);	//CH2
  else
    CYRF_ConfigCRCSeed(~crc);	//CH1

  uint8_t pn_row = get_pn_row(channel_used_p2M[DSM_CH_IDX_P2M]);

  // V19 P2M_OPT: the just-transmitted/received packet is no longer needed
  // when this function runs, so reuse packet_p2M as the 16-byte PN workspace.
  read_code(packet_p2M,pn_row,DSM_SOP_COL_P2M,8);					// pn_row between 0 and 4, DSM_SOP_COL_P2M between 1 and 7
  CYRF_ConfigSOPCode(packet_p2M);
  read_code(packet_p2M,pn_row,7 - DSM_SOP_COL_P2M,8);				// 7-DSM_SOP_COL_P2M between 0 and 6
  read_code(packet_p2M+8,pn_row,7 - DSM_SOP_COL_P2M + 1,8);			// 7-DSM_SOP_COL_P2M+1 between 1 and 7
  CYRF_ConfigDataCode(packet_p2M, 16);

  CYRF_ConfigRFChannel(channel_used_p2M[DSM_CH_IDX_P2M]);
  DSM_CH_IDX_P2M++;
  if(g_model.rfSubType == DSMX_11 || g_model.rfSubType == DSMX_22)
    DSM_CH_IDX_P2M %=23;
  else
    DSM_CH_IDX_P2M %=2;
}

static void DSM_calc_dsmx_channel()
{
  uint8_t idx = 0;

  // [DSM BIND FIX V2]
  // IMPORTANT: build the DSMX hopping ID from the SAME identity that is
  // transmitted in the bind packet and used for CRC/SOP.
  //
  // Previous OpenAVRc code used:
  //     ~g_eeGeneral.fixed_ID.ID_32
  // while bind/CRC/SOP use temp_rfid_addr_p2M[] after Model Match and,
  // possibly, the OrangeRX compatibility correction.
  //
  // On AVR, ID_32 is also little-endian relative to ID_8[], so the numeric
  // byte order did not match the DSM/MPM calculation.
  //
  // Current MPM calculation:
  //   ~((id[0]<<24) | (id[1]<<16) | (id[2]<<8) | id[3])
  //
  // Using temp_rfid_addr_p2M[] here guarantees that BIND, CRC/SOP and the
  // 23-channel DSMX hopping table all describe the exact same transmitter ID.
  uint32_t id =
      ~(((uint32_t)temp_rfid_addr_p2M[0] << 24) |
        ((uint32_t)temp_rfid_addr_p2M[1] << 16) |
        ((uint32_t)temp_rfid_addr_p2M[2] <<  8) |
        ((uint32_t)temp_rfid_addr_p2M[3] <<  0));

  uint32_t id_tmp = id;
  while(idx < 23)
    {
      uint8_t i;
      uint8_t count_3_27 = 0, count_28_51 = 0, count_52_76 = 0;
      id_tmp = id_tmp * 0x0019660D + 0x3C6EF35F;		// Randomization
      uint8_t next_ch = ((id_tmp >> 8) % 0x49) + 3;	// Use least-significant byte and must be larger than 3
      if ( (next_ch ^ temp_rfid_addr_p2M[3]) & 0x01 )
        continue;
      for (i = 0; i < idx; i++)
        {
          if(channel_used_p2M[i] == next_ch)
            break;
          if(channel_used_p2M[i] <= 27)
            count_3_27++;
          else if (channel_used_p2M[i] <= 51)
            count_28_51++;
          else
            count_52_76++;
        }
      if (i != idx)
        continue;
      if ((next_ch < 28 && count_3_27 < 8)
          ||(next_ch >= 28 && next_ch < 52 && count_28_51 < 7)
          ||(next_ch >= 52 && count_52_76 < 8))
        channel_used_p2M[idx++] = next_ch;
    }
}

// [DSM BIND FIX V6]
// Current MPM AUTO leaves the bind channel quiet and listens for the
// receiver's 10-byte bind response before starting normal DSM frames.
// OpenAVRc previously compiled this phase only with DSM_TELEMETRY enabled.
//
// Reuse packet_p2M as the temporary RX buffer: once bind TX is finished,
// the 16-byte bind packet is no longer needed. This adds no persistent SRAM.
static uint8_t DSM_Check_Bind_RX_packet(const uint8_t *buf)
{
  uint8_t result = 1;
  uint16_t sum = 384 - 0x10;

  // buf[1..4] = complemented TX CYRF ID
  // buf[5]    = receiver version
  // buf[6]    = number of channels
  // buf[7]    = maximum DSM type supported
  // buf[8]    = usually 0
  // buf[9..10]= checksum
  for(uint8_t i = 1; i < 9; i++)
    {
      sum += buf[i];
      if(i < 5 && buf[i] != (0xff ^ temp_rfid_addr_p2M[i-1]))
        result = 0;
    }

  // Keep the same checksum test as current MPM/OpenAVRc.
  if(buf[9] != (sum >> 8) && buf[10] != (uint8_t)sum)
    result = 0;

  return result;
}

// [DSM AUTO V25]
// Apply the receiver bind response directly inside OpenAVRc.
//
// Bind response fields (our buffer is shifted by one byte):
//   buf[6] = number of channels
//   buf[7] = DSM capability/type
//
// Known Spektrum bind-status values:
//   0x01        = DSM2 1024 / 22ms
//   0x02        = DSM2 1024 / legacy MC24 variant -> use DSM2_22 here
//   0x12        = DSM2 2048 / 11ms
//   0xA2        = DSMX 2048 / 22ms
//   0xB2        = DSMX 2048 / 11ms
//
// This lets AUTO select the four OpenAVRc air subtypes directly from
// the receiver bind response instead of forcing the refresh rate.
static void DSM_apply_auto_bind_response(const uint8_t *buf)
{
  uint8_t channels = buf[6] & 0x0F;
  const uint8_t type = buf[7];

  if(channels > 12U)
    channels = 12U;
  else if(channels < 4U)
    channels = 6U;

  switch(type)
    {
    case 0x01:
    case 0x02:
      g_model.rfSubType = DSM2_22;
      break;

    case 0x12:
      // 2048-bit DSM2 requires the 2-frame data format in this legacy driver.
      g_model.rfSubType = DSM2_11;
      break;

    case 0xA2:
      g_model.rfSubType = DSMX_22;
      break;

    case 0xB2:
      g_model.rfSubType = DSMX_11;
      break;

    default:
      // Unknown bind status: keep a conservative modern fallback.
      g_model.rfSubType = DSMX_22;
      break;
    }

  g_model.rfOptionValue1 = channels;

  // Rebuild the channel map now that AUTO has become a real subtype.
  DSM_update_channels();

  // AUTO was initialized before the subtype was known and therefore used
  // the DSM2 two-channel path. If DSMX has just been detected, rebuild the
  // complete 23-channel DSMX hopping table before normal traffic starts.
  if(g_model.rfSubType == DSMX_22 || g_model.rfSubType == DSMX_11)
    {
      DSM_CH_IDX_P2M = 0;
      DSM_calc_dsmx_channel();
    }
}

#if defined(DSM_TELEMETRY)
static uint8_t DSM_Check_RX_packet()
{
  uint8_t result=1;						// assume good packet

  uint16_t sum = 384 - 0x10;
  for(uint8_t i = 1; i < 9; i++)
    {
      sum += telem_save_data_buff[i];
      if(i<5)
        if(telem_save_data_buff[i] != (0xff ^ temp_rfid_addr_p2M[i-1]))
          result=0; 					// bad packet
    }
  if( telem_save_data_buff[9] != (sum>>8)  && telem_save_data_buff[10] != (uint8_t)sum )
    result=0;
  return result;
}
#endif


#if defined(FRSKY)
#if defined(GPS)
// Spektrum GPS 0x16/0x17 uses packed BCD with little-endian byte order
// for its multi-byte BCD fields.
static uint8_t DSM_bcd8(uint8_t bcd)
{
  const uint8_t lo = bcd & 0x0F;
  const uint8_t hi = (bcd >> 4) & 0x0F;
  if(lo > 9 || hi > 9)
    return 0xFF;
  return (uint8_t)(hi * 10U + lo);
}

static uint16_t DSM_bcd16_le(const uint8_t *p)
{
  const uint8_t lo = DSM_bcd8(p[0]);
  const uint8_t hi = DSM_bcd8(p[1]);
  if(lo == 0xFF || hi == 0xFF)
    return 0xFFFF;
  return (uint16_t)lo + (uint16_t)hi * 100U;
}
#endif

// V15 - Bridge the minimum DSM telemetry needed by OpenAVRc's FrSky display.
//
// Raw CYRF DSM telemetry packet:
//   [0] I2C address
//   [1] secondary ID
//   [2..15] sensor payload
//
// 0x7F = QoS / Flight Log:
//   [2..3]   A
//   [4..5]   B
//   [6..7]   L
//   [8..9]   R
//   [10..11] F (frame losses)
//   [12..13] H (holds)
//   [14..15] Rx voltage
//
// On normal Spektrum receivers A/B/L/R/F/H are counters, NOT RSSI.
// Some Lemon receivers use A as a 0..100 RSSI value and mark B/L/R/F as 0x8000.
// Only in that documented/supported case do we feed FrSky Rx RSSI.
static void DSM_bridge_telemetry_to_frsky(const uint8_t *buf, uint8_t len, uint8_t cyrf_rssi)
{
  frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;

  // Signal strength of the telemetry packet received locally by the transmitter.
  telemetryData.rssi[1].set(((uint16_t)cyrf_rssi * 100U + 15U) / 31U); // FrSky "Tx" = CYRF RSSI 0..31 -> 0..100%

  if(len < 16)
    return;

  const uint8_t sensorId = buf[0] & 0x7F;

  // V24 - Spektrum 0x7E RPM/Volts/Temperature frame also carries
  // receiver antenna RSSI in bytes 8 and 9 (dBm_A / dBm_B).
  // Spektrum definition:
  //   value < -1 : RSSI in dBm
  //   value == 0 or -1 : no RSSI data
  //   value > 0 : already a percentage/range value
  // Use the strongest valid antenna and feed OpenAVRc FrSky "Rx" RSSI.
  if(sensorId == 0x7E)
    {
      const int8_t rssiA = (int8_t)buf[8];
      const int8_t rssiB = (int8_t)buf[9];
      uint8_t rxPercent = 0U;
      bool haveRxRssi = false;

      // Newer Spektrum telemetry may put a 1..100 range value directly here.
      if(rssiA > 0 || rssiB > 0)
        {
          int16_t bestPercent = 0;
          if(rssiA > bestPercent) bestPercent = rssiA;
          if(rssiB > bestPercent) bestPercent = rssiB;
          if(bestPercent > 100) bestPercent = 100;
          rxPercent = (uint8_t)bestPercent;
          haveRxRssi = true;
        }
      else
        {
          // Legacy representation: negative dBm.  Pick the strongest antenna
          // (the value closest to zero), ignoring 0/-1 "no data" markers.
          int16_t bestDbm = -127;
          if(rssiA < -1) bestDbm = rssiA;
          if(rssiB < -1 && rssiB > bestDbm) bestDbm = rssiB;

          if(bestDbm != -127)
            {
              // Convert the useful RC range -100..-50 dBm to OpenAVRc 0..100%.
              // Values outside that range are saturated.
              if(bestDbm <= -100) rxPercent = 0U;
              else if(bestDbm >= -50) rxPercent = 100U;
              else rxPercent = (uint8_t)((bestDbm + 100) * 2);
              haveRxRssi = true;
            }
        }

      if(haveRxRssi)
        telemetryData.rssi[0].set(rxPercent); // FrSky "Rx" = receiver RSSI

      return;
    }


#if defined(GPS)
  // V22 - Spektrum 0x26 binary GPS, the default format sent by current oXs SRXL2.
  // Layout (big endian multi-byte values):
  //   [2..3]   altitude in metres with +1000m offset
  //   [4..7]   latitude  degrees * 1e7, signed
  //   [8..11]  longitude degrees * 1e7, signed
  //   [12..13] heading in 0.1 degree
  //   [14]     ground speed in km/h
  //   [15]     satellites
  if(sensorId == 0x26)
    {
      const uint16_t rawAlt = ((uint16_t)buf[2] << 8) | buf[3];
      const int32_t lat = (int32_t)(((uint32_t)buf[4] << 24) |
                                    ((uint32_t)buf[5] << 16) |
                                    ((uint32_t)buf[6] << 8)  |
                                     (uint32_t)buf[7]);
      const int32_t lon = (int32_t)(((uint32_t)buf[8] << 24) |
                                    ((uint32_t)buf[9] << 16) |
                                    ((uint32_t)buf[10] << 8) |
                                     (uint32_t)buf[11]);
      const uint16_t headingTenths = ((uint16_t)buf[12] << 8) | buf[13];
      const uint8_t groundSpeedKmh = buf[14];
      const uint8_t numSats = buf[15];

      // 0x26 has no explicit fix flag. Require a plausible satellite count
      // and a non-zero position so forced/invalid zero coordinates do not fix.
      if(numSats == 0U || numSats == 0xFFU || (lat == 0L && lon == 0L))
        {
          telemetryData.value.gpsFix = 0;
          telemetryData.value.gpsGetFirstData = 0;
          telemetryData.value.gpsLatitudeNS = 0;
          telemetryData.value.gpsLongitudeEW = 0;
          return;
        }

      // Valid geographic coordinates are far from INT32_MIN, so a 32-bit
      // absolute value is sufficient here and avoids AVR 64-bit arithmetic.
      uint32_t alat = (lat < 0) ? (uint32_t)(-lat) : (uint32_t)lat;
      uint32_t alon = (lon < 0) ? (uint32_t)(-lon) : (uint32_t)lon;

      const uint16_t latDeg = (uint16_t)(alat / 10000000UL);
      const uint16_t lonDeg = (uint16_t)(alon / 10000000UL);
      const uint32_t latRem = alat % 10000000UL;
      const uint32_t lonRem = alon % 10000000UL;
      // decimal-degree remainder -> minutes * 10000: rem * 60 * 10000 / 1e7 = rem * 6 / 100.
      // Truncate rather than round so 59.9999 minutes can never become 60.0000.
      const uint32_t latMin10000 = (latRem * 6UL) / 100UL;
      const uint32_t lonMin10000 = (lonRem * 6UL) / 100UL;

      telemetryData.value.gpsLatitude_bp =
        latDeg * 100U + (uint16_t)(latMin10000 / 10000UL);
      telemetryData.value.gpsLatitude_ap =
        (uint16_t)(latMin10000 % 10000UL);
      telemetryData.value.gpsLatitudeNS = (lat >= 0) ? 'N' : 'S';

      telemetryData.value.gpsLongitude_bp =
        lonDeg * 100U + (uint16_t)(lonMin10000 / 10000UL);
      telemetryData.value.gpsLongitude_ap =
        (uint16_t)(lonMin10000 % 10000UL);
      telemetryData.value.gpsLongitudeEW = (lon >= 0) ? 'E' : 'W';

      telemetryData.value.gpsFix = 1;
      telemetryData.value.gpsGetFirstData = 0;

      if(headingTenths != 0xFFFFU && headingTenths <= 3600U)
        {
          telemetryData.value.gpsCourse_bp = headingTenths / 10U;
          telemetryData.value.gpsCourse_ap = (headingTenths % 10U) * 10U;
        }

      if(groundSpeedKmh != 0xFFU)
        {
          // OpenAVRc internal GPS speed is 0.1 knot.
          telemetryData.value.gpsSpeed_bp =
            (uint16_t)(((uint32_t)groundSpeedKmh * 2500UL + 231UL) / 463UL);
          checkMaxGpsSpeed();
        }

      if(rawAlt != 0xFFFFU)
        {
          int32_t altM = (int32_t)rawAlt - 1000L;
          if(altM > 32767L) altM = 32767L;
          else if(altM < -32768L) altM = -32768L;
          processHubPacket(GPS_ALT_BP_ID, (uint16_t)(int16_t)altM);
        }

      return;
    }

  // V21 - Spektrum 0x16 GPS Location.
  // Payload bytes (buf[] includes I2C address + secondary ID):
  //   [2..3]   altitude low, packed BCD, 0.1 m
  //   [4..7]   latitude DDMM.mmmm, packed BCD
  //   [8..11]  longitude DDMM.mmmm, packed BCD (+ >99 flag)
  //   [12..13] course, packed BCD, 0.1 degree
  //   [14]      HDOP, packed BCD, 0.1
  //   [15]      GPS flags
  if(sensorId == 0x16)
    {
      const uint8_t gpsFlags = buf[15];
      const uint8_t fixValid = (gpsFlags & 0x08) ? 1U : 0U;

      // A valid Spektrum GPS fix is more explicit than OpenAVRc's historic
      // two-frame N/S + E/W heuristic, so use the receiver's FIX_VALID bit.
      if(!fixValid)
        {
          telemetryData.value.gpsFix = 0;
          telemetryData.value.gpsGetFirstData = 0;
          telemetryData.value.gpsLatitudeNS = 0;
          telemetryData.value.gpsLongitudeEW = 0;
          return;
        }

      const uint8_t latF0 = DSM_bcd8(buf[4]);
      const uint8_t latF1 = DSM_bcd8(buf[5]);
      const uint8_t latMin = DSM_bcd8(buf[6]);
      const uint8_t latDeg = DSM_bcd8(buf[7]);

      const uint8_t lonF0 = DSM_bcd8(buf[8]);
      const uint8_t lonF1 = DSM_bcd8(buf[9]);
      const uint8_t lonMin = DSM_bcd8(buf[10]);
      uint16_t lonDeg = DSM_bcd8(buf[11]);

      if(latF0 == 0xFF || latF1 == 0xFF || latMin == 0xFF || latDeg == 0xFF ||
         lonF0 == 0xFF || lonF1 == 0xFF || lonMin == 0xFF || lonDeg == 0xFF)
        return;

      if(gpsFlags & 0x04) // LONGITUDE_GREATER_99
        lonDeg += 100U;

      telemetryData.value.gpsLatitude_bp =
        (uint16_t)latDeg * 100U + latMin;
      telemetryData.value.gpsLatitude_ap =
        (uint16_t)latF0 + (uint16_t)latF1 * 100U;
      telemetryData.value.gpsLatitudeNS =
        (gpsFlags & 0x01) ? 'N' : 'S';

      telemetryData.value.gpsLongitude_bp =
        lonDeg * 100U + lonMin;
      telemetryData.value.gpsLongitude_ap =
        (uint16_t)lonF0 + (uint16_t)lonF1 * 100U;
      telemetryData.value.gpsLongitudeEW =
        (gpsFlags & 0x02) ? 'E' : 'W';

      telemetryData.value.gpsFix = 1;
      telemetryData.value.gpsGetFirstData = 0;

      // Course is packed BCD 0.1 degree.
      const uint16_t courseTenths = DSM_bcd16_le(&buf[12]);
      if(courseTenths != 0xFFFF && courseTenths <= 3600U)
        {
          telemetryData.value.gpsCourse_bp = courseTenths / 10U;
          telemetryData.value.gpsCourse_ap =
            (courseTenths % 10U) * 10U;
        }

      // Altitude low is 0.1 m. 0x17 supplies the high BCD digits.
      const uint16_t altLowDm = DSM_bcd16_le(&buf[2]);
      if(altLowDm != 0xFFFF && DSM_GPS_ALT_HIGH_P2M <= 99U)
        {
          int32_t altDm =
            (int32_t)DSM_GPS_ALT_HIGH_P2M * 10000L + altLowDm;
          if(gpsFlags & 0x80) // NEGATIVE_ALT
            altDm = -altDm;

          int32_t altM =
            (altDm >= 0) ? (altDm + 5L) / 10L
                         : (altDm - 5L) / 10L;
          if(altM > 32767L)
            altM = 32767L;
          else if(altM < -32768L)
            altM = -32768L;

          // Reuse OpenAVRc's native GPS altitude path: GPSAlt, relative
          // altitude, pilot position and distance handling stay centralized.
          processHubPacket(GPS_ALT_BP_ID, (uint16_t)(int16_t)altM);
        }

      return;
    }

  // V21 - Spektrum 0x17 GPS Status.
  //   [2..3] speed, packed BCD, 0.1 knot
  //   [4..7] UTC, packed BCD HH:MM:SS.S
  //   [8]    satellites, packed BCD
  //   [9]    altitude high, packed BCD
  if(sensorId == 0x17)
    {
      const uint16_t speedTenthsKnots = DSM_bcd16_le(&buf[2]);
      if(speedTenthsKnots != 0xFFFF && telemetryData.value.gpsFix)
        {
          // This is already the unit used internally by OpenAVRc:
          // tenths of a knot.
          telemetryData.value.gpsSpeed_bp = speedTenthsKnots;
          checkMaxGpsSpeed();
        }

      const uint8_t altHigh = DSM_bcd8(buf[9]);
      if(altHigh != 0xFF)
        DSM_GPS_ALT_HIGH_P2M = altHigh;

      // X-Bus 0x17 carries UTC time but no calendar date.
      // Byte order in the 32-bit packed BCD field is: 00, SS, MM, HH.
      const uint8_t sec = DSM_bcd8(buf[5]);
      const uint8_t minute = DSM_bcd8(buf[6]);
      const uint8_t hour = DSM_bcd8(buf[7]);

      if(hour < 24U && minute < 60U && sec < 60U)
        {
          // Same OpenAVRc path as the validated CRSF GPS time:
          // store seconds first, then H/M so GPS_HOUR_MIN_ID applies timezone
          // and adjustRTChour() sees a complete HH:MM:SS value.
          processHubPacket(GPS_SEC_ID, sec);
          processHubPacket(GPS_HOUR_MIN_ID,
                           ((uint16_t)minute << 8) | hour);
        }

      return;
    }
#endif

  // V23 - Spektrum 0x3A LiPo 6S Monitor.
  // [2..13] = C1..C6, big-endian, 0.01V; 0x7FFF = cell absent.
  // OpenAVRc stores each cell in 0.02V units.
  if(sensorId == 0x3A)
    {
      uint8_t count = 0U;
      uint8_t minCell = 255U;
      uint8_t minIdx = 0U;

      for(uint8_t i=0; i<6U; ++i)
        {
          const uint8_t p = (uint8_t)(2U + i*2U);
          const uint16_t rawCentiV =
            ((uint16_t)buf[p] << 8) | buf[p+1U];

          if(rawCentiV == 0x7FFFU || rawCentiV == 0xFFFFU)
            break;

          uint16_t cell02V = (rawCentiV + 1U) / 2U;
          if(cell02V > 255U) cell02V = 255U;

          telemetryData.value.cellVolts[count] = (uint8_t)cell02V;
          if((uint8_t)cell02V < minCell)
            {
              minCell = (uint8_t)cell02V;
              minIdx = count;
            }
          ++count;
        }

      telemetryData.value.cellsCount = count;
      if(count > 0U)
        {
          telemetryData.value.minCellVolts = minCell;
          telemetryData.value.minCellIdx = minIdx;
        }
      return;
    }

  // V20 - Spektrum 0x18 Receiver Capacity Meter.
  // Raw payload:
  //   [2..3] current A, signed big-endian, 0.01 A
  //   [4..5] charge used A, 0.1 mAh
  //   [6..7] voltage A, big-endian, 0.01 V
  //
  // OpenAVRc stores Curr and VFAS in 0.1 A / 0.1 V units.
  if(sensorId == 0x18)
    {
      const int16_t rawCurrentCentiA =
        (int16_t)(((uint16_t)buf[2] << 8) | buf[3]);
      const uint16_t rawChargeLow =
        ((uint16_t)buf[4] << 8) | buf[5];
      const uint16_t rawVoltageCentiV =
        ((uint16_t)buf[6] << 8) | buf[7];
      const uint32_t rawChargeTenthsMah =
        (uint32_t)rawChargeLow |
        ((uint32_t)(buf[15] & 0x0FU) << 16);

      if(rawCurrentCentiA != (int16_t)0x7FFF && rawCurrentCentiA >= 0)
        {
          const uint16_t currentDeciA =
            ((uint16_t)rawCurrentCentiA + 5U) / 10U;
          processHubPacket(CURRENT_ID, currentDeciA);
        }

      if(rawVoltageCentiV != 0xFFFF)
        {
          const uint16_t voltageDeciV =
            (rawVoltageCentiV + 5U) / 10U;
          processHubPacket(VFAS_ID, voltageDeciV);
        }

      if(rawChargeLow != 0xFFFFU)
        {
          uint32_t capacityMah = (rawChargeTenthsMah + 5UL) / 10UL;
          if(capacityMah > 65535UL) capacityMah = 65535UL;

          // Use the oXs capacity value directly. Mark it as external so
          // OpenAVRc does not integrate Curr a second time.
          telemetryData.value.currentConsumption = (uint16_t)capacityMah;
          telemetryData.value.openXsensor = 1;
        }

      // Preserve the already validated Power display even when openXsensor=1.
      telemetryData.value.power =
        ((telemetryData.value.current >> 1) *
         (telemetryData.value.vfas >> 1)) / 25;

      return;
    }

  // V20 - Spektrum 0x20 ESC.
  // Raw payload:
  //   [4..5] input voltage, big-endian, 0.01 V
  //   [8..9] motor current, big-endian, 0.01 A
  if(sensorId == 0x20)
    {
      const uint16_t rawVoltageCentiV =
        ((uint16_t)buf[4] << 8) | buf[5];
      const uint16_t rawCurrentCentiA =
        ((uint16_t)buf[8] << 8) | buf[9];

      if(rawVoltageCentiV != 0xFFFF)
        {
          const uint16_t voltageDeciV =
            (rawVoltageCentiV + 5U) / 10U;
          processHubPacket(VFAS_ID, voltageDeciV);
        }

      if(rawCurrentCentiA != 0xFFFF)
        {
          const uint16_t currentDeciA =
            (rawCurrentCentiA + 5U) / 10U;
          processHubPacket(CURRENT_ID, currentDeciA);
        }

      telemetryData.value.power =
        ((telemetryData.value.current >> 1) *
         (telemetryData.value.vfas >> 1)) / 25;

      return;
    }

  // V18 - Spektrum 0x40 Vario-S.
  // Raw payload:
  //   [2..3] altitude, signed big-endian, 0.1 m
  //   [4..5] vertical speed, signed big-endian, 0.1 m/s
  //
  // OpenAVRc:
  //   Alt  is stored in whole meters through BARO_ALT_BP_ID
  //   VitV is stored in cm/s through VARIO_ID
  if(sensorId == 0x40)
    {
      const int16_t rawAltDm =
        (int16_t)(((uint16_t)buf[2] << 8) | buf[3]);

      const int16_t rawVSpeedDmS =
        (int16_t)(((uint16_t)buf[4] << 8) | buf[5]);

      // 0x7FFF is the usual "no data" sentinel used by Spektrum sensors.
      if(rawAltDm != (int16_t)0x7FFF)
        {
          // Convert 0.1 m -> whole meters, with signed rounding.
          const int16_t altM =
            (rawAltDm >= 0) ? (rawAltDm + 5) / 10
                            : (rawAltDm - 5) / 10;

          processHubPacket(BARO_ALT_BP_ID, (uint16_t)altM);
        }

      if(rawVSpeedDmS != (int16_t)0x7FFF)
        {
          // Convert 0.1 m/s -> cm/s.
          int32_t vSpeedCms = (int32_t)rawVSpeedDmS * 10L;

          // Keep the value inside OpenAVRc's int16_t vario storage.
          if(vSpeedCms > 32767L)
            vSpeedCms = 32767L;
          else if(vSpeedCms < -32768L)
            vSpeedCms = -32768L;

          processHubPacket(VARIO_ID, (uint16_t)(int16_t)vSpeedCms);
        }

      return;
    }


  if(sensorId != 0x7F)
    return;

  uint16_t qosA = ((uint16_t)buf[2] << 8) | buf[3];
  uint16_t qosB = ((uint16_t)buf[4] << 8) | buf[5];
  uint16_t qosL = ((uint16_t)buf[6] << 8) | buf[7];
  uint16_t qosR = ((uint16_t)buf[8] << 8) | buf[9];
  uint16_t qosF = ((uint16_t)buf[10] << 8) | buf[11];

  // V16 - Standard Spektrum QoS/Flight Log receiver voltage.
  // buf[14..15] is big-endian and expressed in 0.01V.
  // OpenAVRc RxBt is stored in 0.1V units.
  uint16_t rxVoltageCentiV = ((uint16_t)buf[14] << 8) | buf[15];
  if(rxVoltageCentiV != 0xFFFF)
    {
      uint16_t rxVoltageDeciV = (rxVoltageCentiV + 5U) / 10U;
      if(rxVoltageDeciV <= 255U)
        telemetryData.rxBatt = (uint8_t)rxVoltageDeciV;
    }

  // LemonRX-style QoS frame: A is RSSI (0..100), remaining antenna/fade
  // fields carry 0x8000 sentinels. Do not interpret normal Spektrum fade
  // counters as a signal-strength percentage.
  if(qosB == 0x8000 &&
     qosL == 0x8000 &&
     qosR == 0x8000 &&
     qosF == 0x8000 &&
     qosA <= 100)
    {
      telemetryData.rssi[0].set((uint8_t)qosA); // FrSky "Rx"
    }
}
#endif

static void DSM_init()
{
#if defined(GPS)
  DSM_GPS_ALT_HIGH_P2M = 0;
#endif

  // [DSM BIND FIX V4]
  // Match current DIY-Multiprotocol-TX-Module DSM identity generation:
  //   1) read the real CYRF6936 manufacturer ID;
  //   2) keep the first 4 bytes used by DSM;
  //   3) apply Model Match with a direct XOR on byte 3.
  //
  // IMPORTANT: temp_rfid_addr_p2M[] is only 4 bytes in OpenAVRc, while
  // CYRF_GetMfgData() writes 6 bytes. V19 reuses packet_p2M as the temporary
  // 6-byte buffer instead of consuming stack RAM.
  CYRF_GetMfgData(packet_p2M);

  temp_rfid_addr_p2M[0] = packet_p2M[0];
  temp_rfid_addr_p2M[1] = packet_p2M[1];
  temp_rfid_addr_p2M[2] = packet_p2M[2];
  temp_rfid_addr_p2M[3] = packet_p2M[3];

  // MPM: cyrfmfg_id[3] ^= RX_num;
  // OpenAVRc's old generic helper used RXNUM*3, which is NOT what MPM DSM does.
  temp_rfid_addr_p2M[3] ^= RXNUM;

  //Calc DSM_SOP_COL_P2M
  DSM_SOP_COL_P2M = (temp_rfid_addr_p2M[0] + temp_rfid_addr_p2M[1] + temp_rfid_addr_p2M[2] + 2) & 0x07;
  //Fix for OrangeRX using wrong pncodes by preventing access to "Col 8"
  if(DSM_SOP_COL_P2M==0)
    {
      temp_rfid_addr_p2M[temp_rfid_addr_p2M[3]%3]^=0x01;					//Change a bit so DSM_SOP_COL_P2M will be different from 0
      DSM_SOP_COL_P2M = (temp_rfid_addr_p2M[0] + temp_rfid_addr_p2M[1] + temp_rfid_addr_p2M[2] + 2) & 0x07;
    }
  //Hopping frequencies
  if (g_model.rfSubType == DSMX_11 || g_model.rfSubType == DSMX_22)
    DSM_calc_dsmx_channel();
  else
    {
      // V19 P2M_OPT: packet_p2M[0..9] is free during DSM2 channel search.
      CYRF_FindBestChannels(packet_p2M, 10, 5, 3, 75);
      //
      srandom(g_eeGeneral.fixed_ID.ID_32 & 0xfefefefe);
      uint8_t idx = random() % 10;
      channel_used_p2M[0] = packet_p2M[idx];

      while(1)
        {
          idx = random() % 10;
          if (packet_p2M[idx] != channel_used_p2M[0])
            break;
        }
      channel_used_p2M[1] = packet_p2M[idx];
    }
  //
  DSM_cyrf_config();
  CYRF_SetTxRxMode(TX_EN);
  //
  DSM_update_channels();
  //
}

static uint16_t DSM_cb()
{
  heartbeat |= HEART_TIMER_PULSES;

#define DSM_CH1_CH2_DELAY	4010*2			// Time between write of channel 1 and channel 2
#define DSM_WRITE_DELAY		1950*2			// Time after write to verify write complete
#define DSM_READ_DELAY		600*2				// Time before write to check read DSM_RFSTATE, and switch channels. Was 400 but 600 seems what the 328p needs to read a packet
  // [DSM BIND FIX V6] Needed by AUTO bind-response listening even when
  // full DSM telemetry support is not compiled.
  uint8_t rx_phase;
  uint8_t len; // V13: runtime telemetry RX for X_22/X_11
  uint8_t waitcpt = 0;

  switch(DSM_RFSTATE)
    {
    case DSM_BIND_WRITE:
      // [DSM BIND FIX V6]
      // The counter is decremented for manual bind and autobind (V1 fix).
      if(DSM_BIND_COUNTER_16_P2M-- == 0)
        {
          // V25: after ~1.8 s of bind packets, leave the bind channel quiet
          // and listen for the receiver response whenever AUTO is selected or
          // telemetry is enabled. This matches the telemetry-capable MPM flow.
          if(g_model.rfSubType == DSM_AUTO || DSM_TELEMETRY_ENABLED)
            DSM_RFSTATE = DSM_BIND_CHECK;
          else
            DSM_RFSTATE = DSM_CHANSEL;
        }
      CYRF_WriteDataPacket(packet_p2M);
      return 10000U*2;

    case DSM_BIND_CHECK:
      {
        // 64 SDR mode: only the first 8 bytes are significant, but OpenAVRc's
        // CYRF_ConfigDataCode() writes the full 16-byte register file.
        uint_farptr_t dataadr = pgm_get_far_address(zzdatainitcodetele);
        // V19 P2M_OPT: bind TX is finished; reuse packet_p2M for the RX data code.
        for(uint8_t i = 0; i < 16; i++)
          packet_p2M[i] = pgm_read_byte_far(dataadr++);

        CYRF_ConfigDataCode(packet_p2M, 16);
        CYRF_SetTxRxMode(RX_EN);
        CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x87); // Prepare to receive

        DSM_BIND_COUNTER_16_P2M = DSM_BIND_COUNT_READ;
        DSM_RFSTATE = DSM_BIND_READ;
        return 2000U*2;
      }

    case DSM_BIND_READ:
      // Listen for the 10-byte receiver bind response, exactly as current MPM.
      rx_phase = CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);

      // RXC=1, RXE=0: read IRQ a second time (MPM debounce).
      if((rx_phase & 0x03) == 0x02)
        rx_phase |= CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);

      if((rx_phase & 0x07) == 0x02)
        {
          // Complete packet, no RX error.
          CYRF_WriteRegister(CYRF_07_RX_IRQ_STATUS, 0x80); // RXOW before read

          if(CYRF_ReadRegister(CYRF_09_RX_COUNT) == 10)
            {
              // Reuse the old TX bind packet as a temporary RX buffer.
              // Read response bytes into [1..10], matching the MPM layout.
              CYRF_ReadDataPacketLen(packet_p2M + 1, 10);

              if(DSM_Check_Bind_RX_packet(packet_p2M))
                {
                  // V25: in AUTO, use the receiver response instead of falling
                  // through the old forced DSM2 path.
                  if(g_model.rfSubType == DSM_AUTO)
                    DSM_apply_auto_bind_response(packet_p2M);

                  DSM_RFSTATE = DSM_CHANSEL;
                  return 2000U*2;
                }
            }

          // Packet was not the expected bind response: re-arm RX.
          CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20);
          CYRF_SetTxRxMode(RX_EN);
          CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x00);
          CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x83);
        }
      else if((rx_phase & 0x02) != 0x02)
        {
          // RX error: abort and re-arm, as current MPM.
          CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20);
          CYRF_SetTxRxMode(RX_EN);
          CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x00);
          CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x83);
        }

      if(--DSM_BIND_COUNTER_16_P2M == 0)
        {
          // V25: no bind response means AUTO cannot know the receiver type.
          // Do not keep the old broken AUTO/DSM2 path. Use DSMX 22ms as a
          // conservative fallback; explicit DSM2 remains available manually.
          if(g_model.rfSubType == DSM_AUTO)
            {
              g_model.rfSubType = DSMX_22;
              DSM_update_channels();
              DSM_CH_IDX_P2M = 0;
              DSM_calc_dsmx_channel();
            }

          DSM_RFSTATE = DSM_CHANSEL;
          return 7000U*2;
        }

      return 7000U*2;

    case DSM_CHANSEL:
      DSM_cyrf_configdata();
      CYRF_SetTxRxMode(TX_EN);
      DSM_CH_IDX_P2M = 0;
      DSM_RFSTATE = DSM_CH1_WRITE_A;
      DSM_set_sop_data_crc();
      return 10000*2;

    case DSM_CH1_WRITE_A:
    case DSM_CH1_WRITE_B:
    case DSM_CH2_WRITE_A:
    case DSM_CH2_WRITE_B:
      DSM_build_data_packet(DSM_RFSTATE == DSM_CH1_WRITE_B||DSM_RFSTATE == DSM_CH2_WRITE_B);	// build lower or upper channels
      CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS);		// clear IRQ flags
      CYRF_WriteDataPacket(packet_p2M);
      DSM_RFSTATE++;										// change from WRITE to CHECK mode
      return DSM_WRITE_DELAY;

    case DSM_CH1_CHECK_A:
    case DSM_CH1_CHECK_B:
    case DSM_CH2_CHECK_A:
    case DSM_CH2_CHECK_B:

      while (!(CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS) & 0x02))
        {
          if (++waitcpt > DSM_NUM_WAIT_LOOPS)
            break;
        }

      if(DSM_RFSTATE==DSM_CH1_CHECK_A || DSM_RFSTATE==DSM_CH1_CHECK_B)
        {
#if defined(DSM_TELEMETRY)
          // reset cyrf6936 if freezed after switching from TX to RX
          if (((CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS) & 0x22) == 0x20) || (CYRF_ReadRegister(CYRF_02_TX_CTRL) & 0x80))
            {
              CYRF_Reset();
              DSM_cyrf_config();
              DSM_cyrf_configdata();
              CYRF_SetTxRxMode(TX_EN);
            }
#endif
          DSM_set_sop_data_crc();
          DSM_RFSTATE++;										// change from CH1_CHECK to CH2_WRITE
          return DSM_CH1_CH2_DELAY - DSM_WRITE_DELAY;
        }
      if (DSM_RFSTATE == DSM_CH2_CHECK_A)
        {
          CYRF_ManagePower();	//Keep transmit power in sync
        }
      // V24: receive telemetry in DSM2 and DSMX modes.
      if(DSM_TELEMETRY_ENABLED)
        {
          DSM_RFSTATE++;                                      // CH2_CHECK -> CH2_READ
          CYRF_SetTxRxMode(RX_EN);                            // Receive mode
          CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x87);         // Prepare to receive
          CALCULATE_LAT_JIT();                               // Protocol debug
          return 11000*2 - DSM_CH1_CH2_DELAY - DSM_WRITE_DELAY - DSM_READ_DELAY;
        }

      // No telemetry
      DSM_set_sop_data_crc();
      if (DSM_RFSTATE == DSM_CH2_CHECK_A)
        {
          if(DSM_NUM_CH_P2M > 7 || g_model.rfSubType==DSM2_11 || g_model.rfSubType==DSMX_11)
            DSM_RFSTATE = DSM_CH1_WRITE_B;				//11ms mode or upper to transmit change from CH2_CHECK_A to CH1_WRITE_A
          else
            {
              //Normal mode 22ms
              DSM_RFSTATE = DSM_CH1_WRITE_A;				// change from CH2_CHECK_A to CH1_WRITE_A (ie no upper)
              CALCULATE_LAT_JIT(); // Calculate latency and jitter.
              SCHEDULE_MIXER_END_IN_US((22000U*2 - DSM_CH1_CH2_DELAY - DSM_WRITE_DELAY)/2);
              return 22000U*2 - DSM_CH1_CH2_DELAY - DSM_WRITE_DELAY ;
            }
        }
      else
        DSM_RFSTATE = DSM_CH1_WRITE_A;					// change from CH2_CHECK_B to CH1_WRITE_A (upper already transmitted so transmit lower)
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      SCHEDULE_MIXER_END_IN_US((11000*2 - DSM_CH1_CH2_DELAY - DSM_WRITE_DELAY)/2);
      return 11000*2 - DSM_CH1_CH2_DELAY - DSM_WRITE_DELAY;


    case DSM_CH2_READ_A:
    case DSM_CH2_READ_B:
      // V24: receive one DSM telemetry frame during the normal DSM2/DSMX RX window.
      rx_phase = CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);
      if((rx_phase & 0x03) == 0x02)
        rx_phase |= CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);

      if((rx_phase & 0x07) == 0x02)
        {
          CYRF_WriteRegister(CYRF_07_RX_IRQ_STATUS, 0x80);   // RXOW before read
          len = CYRF_ReadRegister(CYRF_09_RX_COUNT);
          if(len > 16)
            len = 16;

          // Keep the raw packet available for later S.Port sensor conversion.
          CYRF_ReadDataPacketLen(packet_p2M, len);

          uint8_t dsm_rssi = CYRF_ReadRegister(CYRF_13_RSSI) & 0x1F;
#if defined(FRSKY)
          DSM_bridge_telemetry_to_frsky(packet_p2M, len, dsm_rssi);
#else
          (void)dsm_rssi;
#endif
        }

      CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20);

      if (DSM_RFSTATE == DSM_CH2_READ_A &&
          (g_model.rfSubType==DSM2_22 || g_model.rfSubType==DSMX_22) &&
          DSM_NUM_CH_P2M < 8)
        {
          // 22 ms mode with <8 channels: keep listening through the second 11 ms half.
          CYRF_SetTxRxMode(RX_EN);
          CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x00);
          CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x87);
          DSM_RFSTATE = DSM_CH2_READ_B;
          CALCULATE_LAT_JIT();                               // Protocol debug
          return 11000*2;
        }

      if (DSM_RFSTATE == DSM_CH2_READ_A)
        DSM_RFSTATE = DSM_CH1_WRITE_B;
      else
        DSM_RFSTATE = DSM_CH1_WRITE_A;

      CYRF_SetTxRxMode(TX_EN);
      CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x00);
      DSM_set_sop_data_crc();
      CALCULATE_LAT_JIT();                                   // Protocol debug
      return DSM_READ_DELAY;

    }
  return 0;
}

static void DSM_initialize(uint8_t bind)
{
  CYRF_Reset();
  DSM_init();

  if(bind || (g_model.rfOptionBool2)) // Bind or autobind
    {
      DSM_initialize_bind_phase();
      DSM_RFSTATE = DSM_BIND_WRITE;
      DSM_BIND_COUNTER_16_P2M=DSM_BIND_COUNT;
      if (g_model.rfOptionBool2)
        {
          PROTOCOL_SetBindState(400); // 4 Sec
        }
    }
  else
    DSM_RFSTATE = DSM_CHANSEL;//

  PROTO_Start_Callback(DSM_cb);
}

const void *DSM_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
    {
    case PROTOCMD_INIT:
      DSM_initialize(0);
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      CYRF_Reset();
      CYRF_SetTxRxMode(TXRX_OFF);
      return 0;
    case PROTOCMD_BIND:
      DSM_initialize(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_DSM_Ser),
                          STR_SUBTYPE_DSMSPI,//Sub proto
                          STR_NUMCH,         //Num channels STR_CH
                          STR_DUMMY,         //Option 2 (int)
                          STR_RFPOWER,       //Rf power
                          STR_TELEMETRY,     //OptionBool 1
                          STR_AUTOBIND,      //OptionBool 2
                          STR_DUMMY          //OptionBool 3
                         );
      return 0;
    default:
      break;
    }
  return 0;
}

