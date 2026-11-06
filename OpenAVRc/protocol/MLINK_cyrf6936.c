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

 /*
   MLINK CYRF6936 protocol - V1 bind + servo channels only.
   Ported from the DIY Multiprotocol MLINK_cyrf6936 implementation.

   V1 goals:
    - native MLINK bind
    - 78-channel hopping sequence
    - 16 servo channels
    - no telemetry decoding
    - no failsafe
    - persistent MLINK working data stored in pulses2MHz

 */

#include "../OpenAVRc.h"

// -----------------------------------------------------------------------------
// pulses2MHz memory map used by MLINK
//
//   0..7     packet_p2M                  RF packet (8 bytes)
//   8..85    MLINK_HOP_P2M              78 hopping frequencies
//   86..101  MLINK_DATA_CODE_P2M        16-byte normal Data Code
//   102..117 MLINK_RX_SCRATCH_P2M       16-byte raw RX scratch
//   118..124 free
//   125..127 common OpenAVRc RF bytes
//   128..135 BYTE_P2M(1..8)             MLINK dynamic state
//   136..139 free
//   140..143 temp_rfid_addr_p2M          DO NOT TOUCH
//
// No persistent MLINK SRAM array is allocated.
// -----------------------------------------------------------------------------

#define MLINK_HOP_P2M              (&pulses2MHz.pbyte[8])
#define MLINK_DATA_CODE_P2M        (&pulses2MHz.pbyte[86])
#define MLINK_RX_SCRATCH_P2M       (&pulses2MHz.pbyte[102])

// define pulses2MHz reusable values (use only safe BYTE_P2M slots 1..12)
#define MLINK_RFSTATE_P2M          BYTE_P2M(1)
#define MLINK_PACKET_COUNT_P2M     BYTE_P2M(2)
#define MLINK_HOP_NO_P2M           BYTE_P2M(3)
#define MLINK_CRC_INIT_P2M         BYTE_P2M(4)
#define MLINK_UNK_6_2_P2M          BYTE_P2M(5)
#define MLINK_TOGGLE_P2M           BYTE_P2M(6)
#define MLINK_BIND_COUNT_L_P2M     BYTE_P2M(7)
#define MLINK_BIND_COUNT_H_P2M     BYTE_P2M(8)

// Fast private MPX coordinate scratch.
// Only pulses2MHz[118..121] are used.
// The high scratch bytes need only 5 data bits for the 13-bit low halves;
// bits 6 and 7 are reused as DONE / VALID flags.
#define MLINK_FAST_LAT_LO_L_P2M    pulses2MHz.pbyte[118]
#define MLINK_FAST_LAT_LO_H_P2M    pulses2MHz.pbyte[119]
#define MLINK_FAST_LON_LO_L_P2M    pulses2MHz.pbyte[120]
#define MLINK_FAST_LON_LO_H_P2M    pulses2MHz.pbyte[121]

#define MLINK_FAST_LO_H_DATA_MASK  0x1FU
#define MLINK_FAST_DONE_MASK       0x40U
#define MLINK_FAST_VALID_MASK      0x80U

// -----------------------------------------------------------------------------


#define MLINK_BIND_COUNT           696U       // ~20 s, as original implementation
#define MLINK_NUM_FREQ             78U
#define MLINK_HALF_FREQ            (MLINK_NUM_FREQ / 2U)
#define MLINK_BIND_CHANNEL         0x01U
#define MLINK_PACKET_SIZE          8U
#define MLINK_CRC8_POLY            0x31U


const static RfOptionSettingsvar_t RfOpt_MLINK_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED,
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,      // RF POWER
};


enum
{
  MLINK_BIND_TX = 0,
  MLINK_BIND_PREP_RX,
  MLINK_BIND_RX,

  MLINK_PREP_DATA,
  MLINK_SEND1,
  MLINK_SEND2,
  MLINK_SEND3,
  MLINK_CHECK3,
  MLINK_RX,
};


// CYRF6936 register configuration from the MLINK captures/original implementation.
static const uint8_t ZZ_MLINK_InitData_Start[] PROGMEM =
{
  CYRF_01_TX_LENGTH,        0x08, // 8-byte packet
  CYRF_02_TX_CTRL,          0x40, // Clear TX buffer
  CYRF_03_TX_CFG,           0x3C, // SDR, 64-chip data code
  CYRF_05_RX_CTRL,          0x00,
  CYRF_06_RX_CFG,           0x93, // AGC, overwrite, valid flag
  CYRF_0B_PWR_CTRL,         0x00,
  CYRF_0F_XACT_CFG,         0x04, // Transaction end state = IDLE
  CYRF_10_FRAMING_CFG,      0x00, // SOP disabled
  CYRF_11_DATA32_THOLD,     0x05,
  CYRF_12_DATA64_THOLD,     0x0F,
  CYRF_14_EOP_CTRL,         0x05,
  CYRF_15_CRC_SEED_LSB,     0x00,
  CYRF_16_CRC_SEED_MSB,     0x00,
  CYRF_1B_TX_OFFSET_LSB,    0x00,
  CYRF_1C_TX_OFFSET_MSB,    0x00,
  CYRF_1D_MODE_OVERRIDE,    0x00,
  CYRF_1E_RX_OVERRIDE,      0x14, // RX CRC disabled + forced RX data rate
  CYRF_1F_TX_OVERRIDE,      0x04, // TX CRC disabled
  CYRF_26_XTAL_CFG,         0x08,
  CYRF_29_RX_ABORT,         0x00,
  CYRF_32_AUTO_CAL_TIME,    0x3C,
  CYRF_35_AUTOCAL_OFFSET,   0x14,
  CYRF_39_ANALOG_CTRL,      0x03, // RX invert + slow synth
};


// Bind Data Code.
static const uint8_t ZZ_MLINK_BindDataCode[] PROGMEM =
{
  0x6F, 0xBE, 0x32, 0x01,
  0xDB, 0xF1, 0x2B, 0x01,
  0xE3, 0x5C, 0xFA, 0x02,
  0x97, 0x93, 0xF9, 0x02
};


static uint16_t MLINK_cb();


// -----------------------------------------------------------------------------
// Small P2M helpers
// -----------------------------------------------------------------------------

static void MLINK_set_bind_counter(uint16_t value)
{
  MLINK_BIND_COUNT_L_P2M = (uint8_t)value;
  MLINK_BIND_COUNT_H_P2M = (uint8_t)(value >> 8);
}


static uint16_t MLINK_get_bind_counter()
{
  return (uint16_t)MLINK_BIND_COUNT_L_P2M |
         ((uint16_t)MLINK_BIND_COUNT_H_P2M << 8);
}


static uint8_t MLINK_dec_bind_counter()
{
  uint16_t value = MLINK_get_bind_counter();

  if (value)
    --value;

  MLINK_set_bind_counter(value);
  return (value == 0U);
}


// -----------------------------------------------------------------------------
// Frequency / ID generation
// -----------------------------------------------------------------------------

static void MLINK_shuffle_freqs(uint32_t seed, uint8_t *hop)
{
  srandom(seed);

  for (uint8_t i = 0U; i < MLINK_HALF_FREQ; ++i)
  {
    const uint8_t r = (uint8_t)(random() % MLINK_HALF_FREQ);
    const uint8_t tmp = hop[r];
    hop[r] = hop[i];
    hop[i] = tmp;
  }
}


static void MLINK_generate_channels(void)
{
  // Both halves initially contain the same odd RF channels: 3,5,...79.
  for (uint8_t i = 0U; i < MLINK_HALF_FREQ; ++i)
  {
    const uint8_t ch = (uint8_t)((i << 1) + 3U);
    MLINK_HOP_P2M[i] = ch;
    MLINK_HOP_P2M[i + MLINK_HALF_FREQ] = ch;
  }

  // MPM MProtocol_id equivalent: OpenAVRc fixed 32-bit transmitter ID.
  uint32_t id = g_eeGeneral.fixed_ID.ID_32;

  // Data Code part 1 = original ID, MSB first.
  MLINK_DATA_CODE_P2M[0] = (uint8_t)(id >> 24);
  MLINK_DATA_CODE_P2M[1] = (uint8_t)(id >> 16);
  MLINK_DATA_CODE_P2M[2] = (uint8_t)(id >> 8);
  MLINK_DATA_CODE_P2M[3] = (uint8_t)id;

  MLINK_shuffle_freqs(id, &MLINK_HOP_P2M[0]);

  // Data Code / hopping part 2.
  id ^= 0x6FBE3201UL;

  MLINK_DATA_CODE_P2M[4] = (uint8_t)(id >> 24);
  MLINK_DATA_CODE_P2M[5] = (uint8_t)(id >> 16);
  MLINK_DATA_CODE_P2M[6] = (uint8_t)(id >> 8);
  MLINK_DATA_CODE_P2M[7] = (uint8_t)id;

  MLINK_shuffle_freqs(id, &MLINK_HOP_P2M[MLINK_HALF_FREQ]);

  MLINK_CRC_INIT_P2M = MLINK_DATA_CODE_P2M[7];
  MLINK_UNK_6_2_P2M = 0x3A;

  // Last 8 bytes are the reverse of the first 8 bytes.
  for (uint8_t i = 0U; i < 8U; ++i)
    MLINK_DATA_CODE_P2M[i + 8U] = MLINK_DATA_CODE_P2M[7U - i];
}


// -----------------------------------------------------------------------------
// CYRF6936 init
// -----------------------------------------------------------------------------

static void MLINK_init(void)
{
  uint_farptr_t pdata = pgm_get_far_address(ZZ_MLINK_InitData_Start);

  for (uint8_t i = 0U; i < (DIM(ZZ_MLINK_InitData_Start) / 2U); ++i)
  {
    const uint8_t reg = pgm_read_byte_far(pdata++);
    const uint8_t val = pgm_read_byte_far(pdata++);
    CYRF_WriteRegister(reg, val);
  }

  CYRF_WritePreamble(0x333304UL);
  CYRF_SetTxRxMode(TX_EN);
}


// -----------------------------------------------------------------------------
// CRC8 MLINK
//
// Polynomial = 0x31.
// Every source byte is reflected before entering the CRC.
// CRC output is reflected again.
// -----------------------------------------------------------------------------

static uint8_t MLINK_crc8_update(uint8_t crc, uint8_t data)
{
  crc ^= data;

  for (uint8_t bit = 0U; bit < 8U; ++bit)
  {
    if (crc & 0x80U)
      crc = (uint8_t)((crc << 1) ^ MLINK_CRC8_POLY);
    else
      crc <<= 1;
  }

  return crc;
}


static uint8_t MLINK_crc8_packet(const uint8_t *data, uint8_t init)
{
  uint8_t crc = init;

  for (uint8_t i = 0U; i < (MLINK_PACKET_SIZE - 1U); ++i)
    crc = MLINK_crc8_update(crc, bit_reverse(data[i]));

  return bit_reverse(crc);
}


// RX is read as 16 raw bytes because CYRF receive invert is enabled.
// Useful MLINK bytes are at offsets 0,2,...14.
static uint8_t MLINK_crc8_raw_rx(const uint8_t *raw, uint8_t init)
{
  uint8_t crc = init;

  for (uint8_t i = 0U; i < (MLINK_PACKET_SIZE - 1U); ++i)
    crc = MLINK_crc8_update(crc, bit_reverse(raw[i << 1]));

  return bit_reverse(crc);
}


// -----------------------------------------------------------------------------
// Packet generation
// -----------------------------------------------------------------------------

static void MLINK_build_bind_packet(void)
{
  const uint8_t p_c = (uint8_t)(MLINK_PACKET_COUNT_P2M >> 1);

  memset(packet_p2M, (p_c < 0x16U) ? 0x00 : 0xFF, MLINK_PACKET_SIZE - 1U);

  packet_p2M[0] = 0x0F; // Bind
  packet_p2M[1] = p_c;

  switch (p_c)
  {
    case 0x00:
      packet_p2M[2] = 0x40;
      packet_p2M[4] = 0x01;
      packet_p2M[5] = 0x03;
      packet_p2M[6] = 0xE3;
      break;

    case 0x05:
      packet_p2M[6] = MLINK_CRC_INIT_P2M;
      break;

    case 0x06:
      packet_p2M[2] = MLINK_UNK_6_2_P2M;
      for (uint8_t i = 0U; i < 4U; ++i)
        packet_p2M[i + 3U] = MLINK_HOP_P2M[i];
      break;

    case 0x15:
      packet_p2M[6] = 0x51;
      break;

    case 0x16:
      packet_p2M[2] = 0x51;
      packet_p2M[3] = 0xEC;
      packet_p2M[4] = 0x05;
      break;

    case 0x1A:
      packet_p2M[1] = 0xFF;
      memset(&packet_p2M[2], 0x00, 5);
      break;

    default:
      break;
  }

  if (p_c >= 0x01U && p_c <= 0x04U)
  {
    // Data Code: 5 bytes per bind packet.
    const uint8_t base = (uint8_t)((p_c - 1U) * 5U);

    for (uint8_t i = 0U; i < 5U; ++i)
      if ((uint8_t)(i + base) < 16U)
        packet_p2M[i + 2U] = MLINK_DATA_CODE_P2M[i + base];
  }
  else if (p_c >= 0x07U && p_c <= 0x15U)
  {
    // Hopping frequencies: 5 bytes per bind packet.
    const uint8_t base = (uint8_t)(5U * (p_c - 6U) - 1U);

    for (uint8_t i = 0U; i < 5U; ++i)
      if ((uint8_t)(i + base) < MLINK_NUM_FREQ)
        packet_p2M[i + 2U] = MLINK_HOP_P2M[i + base];
  }
  else if (p_c > 0x19U)
  {
    packet_p2M[1] = 0xFF;
    memset(&packet_p2M[2], 0x00, MLINK_PACKET_SIZE - 3U);
  }

  packet_p2M[7] = MLINK_crc8_packet(packet_p2M, 0xFFU);
}


// OpenAVRc FULL_CHANNEL_OUTPUTS() uses RESX = +/-100%.
// MLINK normal channel encoding is:
//   -100% -> 426
//       0% -> 1937
//   +100% -> 3448
// Extended limits are intentionally not clipped, matching MPM "nolimit".
static uint16_t MLINK_scale_channel(uint8_t chan)
{
  const int32_t input = FULL_CHANNEL_OUTPUTS(chan);
  int32_t value = 1937L + ((input * 1511L) / RESX);

  // Keep the arithmetic safe if an abnormal model value is encountered.
  if (value < 0L)
    value = 0L;
  else if (value > 0xFFFFL)
    value = 0xFFFFL;

  return (uint16_t)value;
}


static void MLINK_data_frame(void)
{
#if defined(X_ANY)
  Xany_scheduleTx_AllInstance();
#endif

  // The first logical packet of each hop is where the new mixer values should
  // be ready. On each 5th hop SEND1 is replaced by the RX window and SEND2
  // becomes the first logical channel packet.
  if (MLINK_RFSTATE_P2M == MLINK_SEND1 ||
      ((MLINK_HOP_NO_P2M % 5U) == 0U && MLINK_RFSTATE_P2M == MLINK_SEND2))
  {
    SCHEDULE_MIXER_END_IN_US(21000U);
  }

  if (MLINK_HOP_NO_P2M == 0U)
    MLINK_TOGGLE_P2M = 1U;

  // Select the three channels carried by this packet.
  if (MLINK_RFSTATE_P2M == MLINK_SEND1 ||
      (((MLINK_HOP_NO_P2M % 5U) == 0U) && MLINK_RFSTATE_P2M == MLINK_SEND2))
  {
    if ((MLINK_HOP_NO_P2M & 1U) == 0U)
      packet_p2M[0] = 0x09; // 10,8,6
    else
      packet_p2M[0] = 0x01; // 11,9,7
  }
  else if (MLINK_RFSTATE_P2M == MLINK_SEND2)
  {
    if (MLINK_TOGGLE_P2M)
      packet_p2M[0] = 0x02; // x,15,13
    else
      packet_p2M[0] = 0x0A; // x,14,12

    MLINK_TOGGLE_P2M ^= 1U;
  }
  else
  {
    // MLINK_SEND3
    if ((MLINK_HOP_NO_P2M & 1U) == 0U)
      packet_p2M[0] = 0x88; // 4,2,0
    else
      packet_p2M[0] = 0x80; // 5,3,1
  }

  uint8_t start = (uint8_t)(4U + 6U * (packet_p2M[0] & 0x03U));
  if ((packet_p2M[0] & 0x08U) == 0U)
    ++start;

  // Three 16-bit channel values, MSB first.
  for (uint8_t i = 0U; i < 3U; ++i)
  {
    const uint16_t value = (start < 16U) ? MLINK_scale_channel(start) : 0U;

    packet_p2M[i * 2U + 1U] = (uint8_t)(value >> 8);
    packet_p2M[i * 2U + 2U] = (uint8_t)value;

    start -= 2U;
  }

  // Data CRC init = reflected (hop index + protocol CRC offset).
  const uint8_t crcInit = bit_reverse((uint8_t)(MLINK_HOP_NO_P2M + MLINK_CRC_INIT_P2M));
  packet_p2M[7] = MLINK_crc8_packet(packet_p2M, crcInit);
}


// -----------------------------------------------------------------------------
// Telemetry
//
// Auto detection is stateless at RF level:
//   0x03 = RX-5
//   0x13 = RX-9-DR
//
// RX-9-DR sensor records use the Multiplex Sensor Bus (MSB) 3-byte format:
//   byte 0: high nibble = sensor address, low nibble = value class
//   byte 1: value low byte, bit 0 = alarm
//   byte 2: value high byte
// The signed sensor value is stored shifted left by one bit.
// -----------------------------------------------------------------------------

#if defined(FRSKY)

static uint8_t MLINK_limit_percent(uint16_t value)
{
  return (value > 100U) ? 100U : (uint8_t)value;
}


static int16_t MLINK_sensor_value(const uint8_t *sensor)
{
  // Remove the alarm bit while preserving the signed two's-complement value.
  int16_t raw = (int16_t)(((uint16_t)sensor[2] << 8) | sensor[1]);
  raw = (int16_t)(raw & (int16_t)0xFFFE);
  return (int16_t)(raw / 2);
}



// -----------------------------------------------------------------------------
// Extra oXs/OpenAVRc private fields carried on class 15.
// These are independent of GPS and are intercepted before the standard MSB
// class decoder.
//
// address 6  / class 15 : TEMP2, biased by +1000 degC
// address 7  / class 15 : AccX, biased by +8192, unit 0.001 g
// address 12 / class 15 : AccY, biased by +8192, unit 0.001 g
// address 13 / class 15 : AccZ, biased by +8192, unit 0.001 g
// -----------------------------------------------------------------------------
static uint8_t MLINK_decode_oxs_extra(uint8_t address,
                                      uint8_t valueClass,
                                      uint16_t value)
{
  if(valueClass != 15U)
    return 0U;

  if(address == 6U)
  {
    const int16_t temp2 = (int16_t)((int32_t)value - 1000L);
    processHubPacket(TEMP2_ID, (uint16_t)temp2);
    return 1U;
  }

  if(address == 7U || address == 12U || address == 13U)
  {
    const int16_t acc = (int16_t)((int32_t)value - 8192L);

    if(address == 7U)
      processHubPacket(ACCEL_X_ID, (uint16_t)acc);
    else if(address == 12U)
      processHubPacket(ACCEL_Y_ID, (uint16_t)acc);
    else
      processHubPacket(ACCEL_Z_ID, (uint16_t)acc);

    return 1U;
  }

  return 0U;
}


#if defined(GPS)

// oXs/OpenAVRc FAST private MPX extension.
static void MLINK_fast_update_min_cell(void)
{
  uint8_t minCell = 255U;
  uint8_t minIdx = 0U;
  for(uint8_t i=0U; i<telemetryData.value.cellsCount && i<6U; ++i)
  {
    const uint8_t v = telemetryData.value.cellVolts[i];
    if(v && v < minCell)
    {
      minCell = v;
      minIdx = i;
    }
  }
  if(minCell != 255U)
  {
    telemetryData.value.minCellVolts = minCell;
    telemetryData.value.minCellIdx = minIdx;
  }
}

static void MLINK_fast_store_cell_pair(uint8_t firstCell, uint16_t packed)
{
  const uint8_t c0 = (uint8_t)((packed & 0x7FU) + 100U);
  const uint8_t c1 = (uint8_t)(((packed >> 7) & 0x7FU) + 100U);

  telemetryData.value.cellVolts[firstCell] = c0;
  telemetryData.value.cellVolts[firstCell + 1U] = c1;

  const uint8_t count = (uint8_t)(firstCell + 2U);
  if(telemetryData.value.cellsCount < count)
    telemetryData.value.cellsCount = count;

  MLINK_fast_update_min_cell();
}

static void MLINK_fast_publish_coord(uint8_t longitude, uint32_t packed)
{
  if(packed & 0x02000000UL)
    packed |= 0xFC000000UL; // sign extend 26-bit value

  const int32_t coordE5 = (int32_t)packed;
  const uint8_t negative = (coordE5 < 0);
  uint32_t v = negative ? (uint32_t)(-(int64_t)coordE5) : (uint32_t)coordE5;

  const uint16_t deg = (uint16_t)(v / 100000UL);
  const uint32_t frac = v % 100000UL;
  const uint32_t minScaled = frac * 60UL;
  const uint16_t bp = (uint16_t)(deg * 100U + (uint16_t)(minScaled / 100000UL));
  const uint16_t ap = (uint16_t)((minScaled % 100000UL) / 10UL);

  if(longitude)
  {
    telemetryData.value.gpsLongitude_bp = bp;
    telemetryData.value.gpsLongitude_ap = ap;
    telemetryData.value.gpsLongitudeEW = negative ? 'W' : 'E';
    MLINK_FAST_LON_LO_H_P2M |= MLINK_FAST_DONE_MASK;
  }
  else
  {
    telemetryData.value.gpsLatitude_bp = bp;
    telemetryData.value.gpsLatitude_ap = ap;
    telemetryData.value.gpsLatitudeNS = negative ? 'S' : 'N';
    MLINK_FAST_LAT_LO_H_P2M |= MLINK_FAST_DONE_MASK;
  }

  if((MLINK_FAST_LAT_LO_H_P2M & MLINK_FAST_DONE_MASK) &&
     (MLINK_FAST_LON_LO_H_P2M & MLINK_FAST_DONE_MASK))
    manageGpsFix();
}

static uint8_t MLINK_decode_oxs_fast_extension(uint8_t address,
                                                uint8_t valueClass,
                                                uint16_t value)
{
  if((address == 14U || address == 15U) &&
     (valueClass == 14U || valueClass == 15U))
  {
    if(valueClass == 14U)
    {
      if(address == 14U)
      {
        MLINK_FAST_LAT_LO_L_P2M = (uint8_t)value;
        MLINK_FAST_LAT_LO_H_P2M =
          (uint8_t)((MLINK_FAST_LAT_LO_H_P2M & MLINK_FAST_DONE_MASK) |
                    (((uint8_t)(value >> 8)) & MLINK_FAST_LO_H_DATA_MASK) |
                    MLINK_FAST_VALID_MASK);
      }
      else
      {
        MLINK_FAST_LON_LO_L_P2M = (uint8_t)value;
        MLINK_FAST_LON_LO_H_P2M =
          (uint8_t)((MLINK_FAST_LON_LO_H_P2M & MLINK_FAST_DONE_MASK) |
                    (((uint8_t)(value >> 8)) & MLINK_FAST_LO_H_DATA_MASK) |
                    MLINK_FAST_VALID_MASK);
      }
      return 1U;
    }

    if(address == 14U)
    {
      if(!(MLINK_FAST_LAT_LO_H_P2M & MLINK_FAST_VALID_MASK)) return 1U;
      const uint16_t low = (uint16_t)MLINK_FAST_LAT_LO_L_P2M |
                           ((uint16_t)(MLINK_FAST_LAT_LO_H_P2M &
                                      MLINK_FAST_LO_H_DATA_MASK) << 8);
      MLINK_FAST_LAT_LO_H_P2M &= (uint8_t)~MLINK_FAST_VALID_MASK;
      MLINK_fast_publish_coord(0U, ((uint32_t)(low & 0x1FFFU)) |
                                  ((uint32_t)(value & 0x1FFFU) << 13));
    }
    else
    {
      if(!(MLINK_FAST_LON_LO_H_P2M & MLINK_FAST_VALID_MASK)) return 1U;
      const uint16_t low = (uint16_t)MLINK_FAST_LON_LO_L_P2M |
                           ((uint16_t)(MLINK_FAST_LON_LO_H_P2M &
                                      MLINK_FAST_LO_H_DATA_MASK) << 8);
      MLINK_FAST_LON_LO_H_P2M &= (uint8_t)~MLINK_FAST_VALID_MASK;
      MLINK_fast_publish_coord(1U, ((uint32_t)(low & 0x1FFFU)) |
                                  ((uint32_t)(value & 0x1FFFU) << 13));
    }
    return 1U;
  }

  if(address != 10U)
    return 0U;

  switch(valueClass)
  {
    case 11U:
    {
      if(value < 1440U)
      {
        const uint8_t utcHour = (uint8_t)(value / 60U);
        telemetryData.value.min = (uint8_t)(value % 60U);

        // Preserve the currently validated M-Link behaviour.
        int16_t localHour = (int16_t)utcHour + (int16_t)g_eeGeneral.timezone;
        while(localHour < 0) localHour += 24;
        while(localHour >= 24) localHour -= 24;
        telemetryData.value.hour = (uint8_t)localHour;
      }
      return 1U;
    }
    case 12U:
      if(value < 60U) telemetryData.value.sec = (uint8_t)value;
      return 1U;
    case 13U:
      MLINK_fast_store_cell_pair(0U, value);
      return 1U;
    case 14U:
      MLINK_fast_store_cell_pair(2U, value);
      return 1U;
    case 15U:
      MLINK_fast_store_cell_pair(4U, value);
      return 1U;
    default:
      return 0U;
  }
}

#endif // GPS


static void MLINK_decode_sensor(const uint8_t *sensor)
{
  const uint8_t address = (uint8_t)(sensor[0] >> 4);
  const uint8_t valueClass = (uint8_t)(sensor[0] & 0x0FU);
  const int16_t value = MLINK_sensor_value(sensor);

  // Our class-9 private values are encoded as positive biased 14-bit values.
  if(value >= 0 && MLINK_decode_oxs_extra(address, valueClass, (uint16_t)value))
    return;

#if defined(GPS)
  if(value >= 0 && MLINK_decode_oxs_fast_extension(address, valueClass, (uint16_t)value))
    return;
#endif

  switch (valueClass)
  {
    case 1: // Voltage, 0.1 V
      if (value >= 0)
      {
        if (address == 0U)
        {
          // Standard MSB address 0 = receiver voltage.
          // OpenAVRc analog voltage uses hundredths of a volt.
          telemetryData.analog[0].set((uint16_t)value * 10U, UNIT_VOLTS);
        }
        else
        {
          // External/main battery voltage -> VFAS, in 0.1 V.
          processHubPacket(VFAS_ID, (uint16_t)value);
        }
      }
      break;

    case 2: // Current, 0.1 A
      if (value >= 0)
        processHubPacket(CURRENT_ID, (uint16_t)value);
      break;

    case 3: // Vario, 0.1 m/s -> OpenAVRc uses cm/s
    {
      int32_t cmPerSec = (int32_t)value * 10L;
      if (cmPerSec < -32768L) cmPerSec = -32768L;
      if (cmPerSec >  32767L) cmPerSec =  32767L;
      processHubPacket(VARIO_ID, (uint16_t)(int16_t)cmPerSec);
      break;
    }

#if defined(GPS)
    case 4: // Ground speed, 0.1 km/h -> OpenAVRc internal 0.1 knot
      if(address == 4U && value >= 0)
      {
        telemetryData.value.gpsSpeed_bp =
          (uint16_t)(((uint32_t)(uint16_t)value * 250UL + 231UL) / 463UL);
        telemetryData.value.gpsSpeed_ap = 0U;
        checkMaxGpsSpeed();
      }
      break;
#endif

    case 5: // RPM: MSB V2 uses +value*100 rpm, or -value*10 rpm
    {
      uint32_t rpm;

      if (value >= 0)
        rpm = (uint32_t)(uint16_t)value * 100UL;
      else
        rpm = (uint32_t)(-(int32_t)value) * 10UL;

      // processHubPacket(RPM_ID) expects pulse frequency scaled by blade count.
      const uint8_t blades = (uint8_t)(g_model.telemetry.blades + 2U);
      uint32_t hubValue = (rpm * blades + 30UL) / 60UL;
      if (hubValue > 65535UL) hubValue = 65535UL;

      processHubPacket(RPM_ID, (uint16_t)hubValue);
      break;
    }

    case 6: // Temperature, 0.1 degC -> OpenAVRc TEMP1 is whole degC
      processHubPacket(TEMP1_ID, (uint16_t)(int16_t)(value / 10));
      break;

#if defined(GPS)
    case 7: // Direction, 0.1 degree
      if(address == 15U && value >= 0)
      {
        telemetryData.value.gpsCourse_bp = (uint16_t)value / 10U;
        telemetryData.value.gpsCourse_ap = ((uint16_t)value % 10U) * 10U;
      }
      break;
#endif

    case 8: // Altitude, 1 m
      // oXs default MPX address 8 = RELATIVEALT (barometric relative altitude).
      if(address == 8U)
      {
        telemetryData.value.baroAltitude = value;
        setMinMaxAltitude();
      }
#if defined(GPS)
      // Address 14 = GPS altitude. Keep it separate from barometric Alt.
      else if(address == 14U)
      {
        telemetryData.value.gpsAltitude = value;
      }
#endif
      break;

    case 10: // Link Quality Indicator, 1 %
      if (value >= 0)
        telemetryData.rssi[0].set(MLINK_limit_percent((uint16_t)value));
      break;

    default:
      // V2 intentionally keeps the same sensor-class scope as the original
      // MLINK implementation. Extra MSB classes can be added after validation.
      break;
  }
}


static void MLINK_decode_telemetry(uint8_t *buf)
{
  // A valid RF telemetry packet means the link is alive.
  frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;

  // Strength of the telemetry packet received locally by the transmitter.
  const uint8_t cyrfRssi = CYRF_ReadRegister(CYRF_13_RSSI) & 0x1FU;
  telemetryData.rssi[1].set((uint8_t)(((uint16_t)cyrfRssi * 100U + 15U) / 31U));

  if (buf[0] == 0x03U)
  {
    // RX-5 example:
    //   03 15 23 00 00 01 02 xx
    //
    // The original MLINK implementation identifies buf[2] as receiver RSSI
    // and buf[5] as a loss/LQI-related value.
    telemetryData.rssi[0].set(MLINK_limit_percent((uint16_t)buf[2] << 1));
    return;
  }

  if (buf[0] == 0x13U)
  {
    // RX-9-DR: two complete MSB sensor records per RF packet.
    MLINK_decode_sensor(&buf[1]);
    MLINK_decode_sensor(&buf[4]);
    return;
  }

  // Unknown valid frame: do not force a receiver type.
}

#endif // FRSKY


// -----------------------------------------------------------------------------
// Normal data state machine
// -----------------------------------------------------------------------------

static uint16_t MLINK_send_data_packet(void)
{
  switch (MLINK_RFSTATE_P2M)
  {
    case MLINK_PREP_DATA:
      CYRF_ConfigDataCode(MLINK_DATA_CODE_P2M, 16);
      MLINK_CRC_INIT_P2M = (uint8_t)(MLINK_CRC_INIT_P2M + 0xEDU);
      MLINK_HOP_NO_P2M = 0U;
      MLINK_TOGGLE_P2M = 0U;

      CYRF_ConfigRFChannel(MLINK_HOP_P2M[MLINK_HOP_NO_P2M]);
      CYRF_SetTxRxMode(TX_EN);

      // Apply the model RF power immediately, then CYRF_ManagePower() keeps it
      // synchronized with range mode / menu changes.
      CYRF_SetPower(g_model.rfOptionValue3);

      MLINK_RFSTATE_P2M = MLINK_SEND1;
      // FALLTHROUGH

    case MLINK_SEND1:
      MLINK_data_frame();
      CYRF_WriteDataPacketLen(packet_p2M, MLINK_PACKET_SIZE);
      MLINK_RFSTATE_P2M = MLINK_SEND2;
      return 5991U * 2U; // 4880 + 1111 us

    case MLINK_SEND2:
      MLINK_data_frame();
      CYRF_WriteDataPacketLen(packet_p2M, MLINK_PACKET_SIZE);
      MLINK_RFSTATE_P2M = MLINK_SEND3;

      if ((MLINK_HOP_NO_P2M % 5U) == 0U)
        return 5634U * 2U; // 4617 + 1017 us

      return 6039U * 2U;   // 4617 + 1422 us

    case MLINK_SEND3:
      MLINK_data_frame();
      CYRF_WriteDataPacketLen(packet_p2M, MLINK_PACKET_SIZE);
      MLINK_RFSTATE_P2M = MLINK_CHECK3;
      return 4611U * 2U;

    case MLINK_CHECK3:
      ++MLINK_HOP_NO_P2M;
      if (MLINK_HOP_NO_P2M >= MLINK_NUM_FREQ)
        MLINK_HOP_NO_P2M = 0U;

      CYRF_ConfigRFChannel(MLINK_HOP_P2M[MLINK_HOP_NO_P2M]);

      // Keep the original MLINK RX window every five frequencies even though
      // telemetry is not decoded in V1. It is part of the observed RF timing.
      if ((MLINK_HOP_NO_P2M % 5U) == 0U)
      {
        CYRF_SetTxRxMode(RX_EN);
        CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x82);
        MLINK_RFSTATE_P2M = MLINK_RX;
        return 9882U * 2U; // 8038 + 2434 + 410 - 1000 us
      }

      CYRF_ManagePower();
      MLINK_RFSTATE_P2M = MLINK_SEND1;
      return 4470U * 2U;

    case MLINK_RX:
    {
      const uint8_t status = CYRF_ReadRegister(CYRF_05_RX_CTRL);

      if ((status & 0x80U) == 0U)
      {
        const uint8_t len = CYRF_ReadRegister(CYRF_09_RX_COUNT);

        if (len == MLINK_PACKET_SIZE)
        {
          // RX invert mode: useful bytes are raw[0], raw[2], ... raw[14].
          CYRF_ReadDataPacketLen(MLINK_RX_SCRATCH_P2M, MLINK_PACKET_SIZE * 2U);

          // Normal telemetry CRC init is reflected protocol CRC offset.
          const uint8_t crcInit = bit_reverse(MLINK_CRC_INIT_P2M);

          if (MLINK_RX_SCRATCH_P2M[14] ==
              MLINK_crc8_raw_rx(MLINK_RX_SCRATCH_P2M, crcInit))
          {
            // Compact the 8 useful bytes in place. This does not require any
            // extra SRAM and keeps the bind/servo packet buffer untouched.
            for (uint8_t i = 0U; i < MLINK_PACKET_SIZE; ++i)
              MLINK_RX_SCRATCH_P2M[i] = MLINK_RX_SCRATCH_P2M[i << 1];

#if defined(FRSKY)
            MLINK_decode_telemetry(MLINK_RX_SCRATCH_P2M);
#endif
          }
        }
      }

      CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20);
      CYRF_WriteRegister(CYRF_0F_XACT_CFG, 0x24);
      CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x00);
      CYRF_SetTxRxMode(TX_EN);

      // Original MLINK timing: after the telemetry window resume at SEND2.
      MLINK_RFSTATE_P2M = MLINK_SEND2;
      return 1000U * 2U;
    }

    default:
      MLINK_RFSTATE_P2M = MLINK_PREP_DATA;
      return 1000U * 2U;
  }
}


// -----------------------------------------------------------------------------
// Bind
// -----------------------------------------------------------------------------

static void MLINK_send_bind_packet(void)
{
  MLINK_build_bind_packet();
  CYRF_WriteDataPacketLen(packet_p2M, MLINK_PACKET_SIZE);
}


static uint16_t MLINK_bind_cb(void)
{
  uint16_t time = 1000U * 2U;

  switch (MLINK_RFSTATE_P2M)
  {
    case MLINK_BIND_TX:
      bind_tx:
      if (MLINK_dec_bind_counter() || MLINK_PACKET_COUNT_P2M >= (0x1BU * 2U))
      {
        // Bind finished: switch to the normal MLINK callback.
        PROTOCOL_SetBindState(0);
        MLINK_RFSTATE_P2M = MLINK_PREP_DATA;
        PROTO_Change_Callback(MLINK_cb);
        time = 22720U * 2U;
        break;
      }

      MLINK_send_bind_packet();

      if (MLINK_PACKET_COUNT_P2M == 0U ||
          MLINK_PACKET_COUNT_P2M > (0x19U * 2U))
      {
        MLINK_RFSTATE_P2M = MLINK_BIND_PREP_RX;
        time = 4700U * 2U;
        break;
      }

      ++MLINK_PACKET_COUNT_P2M;

      if (MLINK_PACKET_COUNT_P2M & 1U)
        time = 6000U * 2U;
      else
        time = 22720U * 2U;
      break;


    case MLINK_BIND_PREP_RX:
      // Wait at most ~200 us for the current TX packet to finish.
      for (uint8_t i = 0U; i < 200U; ++i)
      {
        if ((CYRF_ReadRegister(CYRF_02_TX_CTRL) & 0x80U) == 0U)
          break;
        _delay_us(1);
      }

      CYRF_SetTxRxMode(RX_EN);
      CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x82);
      MLINK_RFSTATE_P2M = MLINK_BIND_RX;

      if (MLINK_PACKET_COUNT_P2M > (0x19U * 2U))
        time = 28712U * 2U;
      else
        time = (28712U - 4700U) * 2U;
      break;


    case MLINK_BIND_RX:
    {
      const uint8_t status = CYRF_ReadRegister(CYRF_05_RX_CTRL);

      if ((status & 0x80U) == 0U)
      {
        const uint8_t len = CYRF_ReadRegister(CYRF_09_RX_COUNT);

        if (len == MLINK_PACKET_SIZE)
        {
          // RX invert mode returns useful bytes at offsets 0,2,...14.
          CYRF_ReadDataPacketLen(MLINK_RX_SCRATCH_P2M, MLINK_PACKET_SIZE * 2U);

          if (MLINK_RX_SCRATCH_P2M[14] == MLINK_crc8_raw_rx(MLINK_RX_SCRATCH_P2M, 0xFFU))
          {
            const uint8_t type = MLINK_RX_SCRATCH_P2M[0];

            if (type == 0x7FU)
            {
              MLINK_PACKET_COUNT_P2M = 3U; // Start sending bind payload.
            }
            else if (MLINK_PACKET_COUNT_P2M > (0x19U * 2U))
            {
              if (type == 0x8FU)
                ++MLINK_PACKET_COUNT_P2M;
              else if (type == 0x9FU)
                MLINK_PACKET_COUNT_P2M = 0x80U; // End bind.
              else
                MLINK_PACKET_COUNT_P2M = 0U;    // Restart bind.
            }
          }
        }
      }
      else
      {
        MLINK_PACKET_COUNT_P2M = 0U;
      }

      CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x20);
      CYRF_WriteRegister(CYRF_0F_XACT_CFG, 0x24);
      CYRF_WriteRegister(CYRF_29_RX_ABORT, 0x00);
      CYRF_SetTxRxMode(TX_EN);
      MLINK_RFSTATE_P2M = MLINK_BIND_TX;

      if (MLINK_PACKET_COUNT_P2M)
      {
        time = 18136U * 2U;
        break;
      }

      // Original implementation immediately resumes bind TX if no valid reply.
      goto bind_tx;
    }

    default:
      MLINK_RFSTATE_P2M = MLINK_BIND_TX;
      time = 1000U * 2U;
      break;
  }

  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT();
  return time;
}


static uint16_t MLINK_cb(void)
{
  const uint16_t time = MLINK_send_data_packet();

  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT();
  return time;
}


// -----------------------------------------------------------------------------
// Protocol initialization
// -----------------------------------------------------------------------------

static void MLINK_initialize(uint8_t bind)
{
  CYRF_Reset();
  loadrfidaddr();

  MLINK_generate_channels();
  MLINK_init();

  MLINK_PACKET_COUNT_P2M = 0U;
  MLINK_HOP_NO_P2M = 0U;
  MLINK_TOGGLE_P2M = 0U;

  // Clear packed FAST GPS scratch flags/data-high bits.
  MLINK_FAST_LAT_LO_H_P2M = 0U;
  MLINK_FAST_LON_LO_H_P2M = 0U;

  if (bind)
  {
    // Load bind Data Code through P2M scratch; no persistent SRAM buffer.
    uint_farptr_t pdata = pgm_get_far_address(ZZ_MLINK_BindDataCode);
    for (uint8_t i = 0U; i < 16U; ++i)
      MLINK_RX_SCRATCH_P2M[i] = pgm_read_byte_far(pdata++);

    CYRF_ConfigDataCode(MLINK_RX_SCRATCH_P2M, 16);
    CYRF_ConfigRFChannel(MLINK_BIND_CHANNEL);
    CYRF_SetTxRxMode(TX_EN);

    MLINK_PACKET_COUNT_P2M = 0U;
    MLINK_set_bind_counter(MLINK_BIND_COUNT);
    MLINK_RFSTATE_P2M = MLINK_BIND_TX;

    // Keep the OpenAVRc bind indication active for approximately 20 seconds.
    PROTOCOL_SetBindState(2000);
    PROTO_Start_Callback(MLINK_bind_cb);
  }
  else
  {
    MLINK_RFSTATE_P2M = MLINK_PREP_DATA;
    PROTO_Start_Callback(MLINK_cb);
  }
}


// -----------------------------------------------------------------------------
// OpenAVRc protocol interface
// -----------------------------------------------------------------------------

const void *MLINK_Cmds(enum ProtoCmds cmd)
{
  switch (cmd)
  {
    case PROTOCMD_INIT:
      MLINK_initialize(0);
      return 0;

    case PROTOCMD_BIND:
      MLINK_initialize(1);
      return 0;

    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      CYRF_Reset();
      CYRF_SetTxRxMode(TXRX_OFF);
      return 0;

    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_MLINK_Ser),
                          STR_DUMMY,    // Sub proto
                          STR_DUMMY,    // Option 1
                          STR_DUMMY,    // Option 2
                          STR_RFPOWER,  // Option 3
                          STR_TELEMETRY,// Bool 1
                          STR_DUMMY,    // Bool 2
                          STR_DUMMY);   // Bool 3
      return 0;

    default:
      break;
  }

  return 0;
}
