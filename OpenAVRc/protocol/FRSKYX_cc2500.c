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
#include "frsky.h"

extern uint8_t Usart0RxBuffer[];


/*static const char * const frskyx_opts[] = {
  _tr_noop("Failsafe"), "Hold", "NoPulse", "RX", NULL,
  _tr_noop("AD2GAIN"),  "0", "2000", "655361", NULL,       // big step 10, little step 1
  _tr_noop("Freq-Fine"),  "-127", "127", NULL,
  _tr_noop("Format"),  "FCC", "EU", NULL,
  _tr_noop("RSSIChan"),  "None", "LastChan", NULL,
  NULL
};*/

const static int8_t RfOpt_FrskyX_Ser[] PROGMEM = {
/*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
/*rfSubTypeMax*/0,
/*rfOptionValue1Min*/-128,
/*rfOptionValue1Max*/127,
/*rfOptionValue2Min*/0,
/*rfOptionValue2Max*/0,
/*rfOptionValue3Max*/0,
};

bool frskyX_format_EU = 1; //0 US , 1 EU

enum {
  PROTO_OPTS_FAILSAFE,
  PROTO_OPTS_AD2GAIN,
  PROTO_OPTS_FREQFINE,
  PROTO_OPTS_FORMAT,
  PROTO_OPTS_RSSICHAN,
  LAST_PROTO_OPT,
};


#define FAILSAFE_HOLD    0
#define FAILSAFE_NOPULSE 1
#define FAILSAFE_RX      2

#define MAX_PACKET_SIZE 33

// Statics are not initialized on 7e so in initialize() if necessary
static uint8_t chanskip;
static uint8_t calData[48][3];
static uint8_t channr;
static uint8_t counter_rst;
static uint8_t ctr;
static uint8_t seq_last_sent;
static uint8_t seq_last_rcvd;
static uint8_t packet_size;
static uint8_t X_state;

// uint8_t ptr[4] = {0x01,0x12,0x23,0x30};
//uint8_t ptr[4] = {0x00,0x11,0x22,0x33};
enum {
  FRSKYX_DATA1,
  FRSKYX_DATA2,
  FRSKYX_DATA3,
  FRSKYX_DATA4,
  FRSKYX_DATA5
};

static const uint8_t hop_data[] = {
  0x02, 0xD4, 0xBB, 0xA2, 0x89,
  0x70, 0x57, 0x3E, 0x25, 0x0C,
  0xDE, 0xC5, 0xAC, 0x93, 0x7A,
  0x61, 0x48, 0x2F, 0x16, 0xE8,
  0xCF, 0xB6, 0x9D, 0x84, 0x6B,
  0x52, 0x39, 0x20, 0x07, 0xD9,
  0xC0, 0xA7, 0x8E, 0x75, 0x5C,
  0x43, 0x2A, 0x11, 0xE3, 0xCA,
  0xB1, 0x98, 0x7F, 0x66, 0x4D,
  0x34, 0x1B, 0x00, 0x1D, 0x03
};

// register, FCC, EU
static const uint8_t ZZ_frsky_init_data[] PROGMEM = {
  //{CC2500_02_IOCFG0,    0x06,  0x06},
  //{CC2500_00_IOCFG2,    0x06,  0x06},
  CC2500_17_MCSM1,     0x0c,  0x0E,
  CC2500_18_MCSM0,     0x18,  0x18,
  CC2500_06_PKTLEN,    0x1E,  0x23,
  CC2500_07_PKTCTRL1,  0x04,  0x04,
  CC2500_08_PKTCTRL0,  0x01,  0x01,
  CC2500_3E_PATABLE,   TXPOWER_1, TXPOWER_1,
  CC2500_0B_FSCTRL1,   0x0A,  0x08,
  CC2500_0C_FSCTRL0,   0x00,  0x00,
  CC2500_0D_FREQ2,     0x5c,  0x5c,
  CC2500_0E_FREQ1,     0x76,  0x80,
  CC2500_0F_FREQ0,     0x27,  0x00,
  CC2500_10_MDMCFG4,   0x7B,  0x7B,
  CC2500_11_MDMCFG3,   0x61,  0xF8,
  CC2500_12_MDMCFG2,   0x13,  0x03,
  CC2500_13_MDMCFG1,   0x23,  0x23,
  CC2500_14_MDMCFG0,   0x7a,  0x7a,
  CC2500_15_DEVIATN,   0x51,  0x53
};

// register, value
static const uint8_t ZZ_frsky_init_data_shared[] PROGMEM = {
  CC2500_19_FOCCFG,    0x16,
  CC2500_1A_BSCFG,     0x6c,
  CC2500_1B_AGCCTRL2,  0x43,
  CC2500_1C_AGCCTRL1,  0x40,
  CC2500_1D_AGCCTRL0,  0x91,
  CC2500_21_FREND1,    0x56,
  CC2500_22_FREND0,    0x10,
  CC2500_23_FSCAL3,    0xa9,
  CC2500_24_FSCAL2,    0x0A,
  CC2500_25_FSCAL1,    0x00,
  CC2500_26_FSCAL0,    0x11,
  CC2500_29_FSTEST,    0x59,
  CC2500_2C_TEST2,     0x88,
  CC2500_2D_TEST1,     0x31,
  CC2500_2E_TEST0,     0x0B,
  CC2500_03_FIFOTHR,   0x07,
  CC2500_09_ADDR,      0x00
};

static uint16_t crc(uint8_t *data, uint8_t len)
{
  static const uint16_t ZZCRCTable[] PROGMEM = {
    0x0000,0x1189,0x2312,0x329b,0x4624,0x57ad,0x6536,0x74bf,
    0x8c48,0x9dc1,0xaf5a,0xbed3,0xca6c,0xdbe5,0xe97e,0xf8f7,
    0x1081,0x0108,0x3393,0x221a,0x56a5,0x472c,0x75b7,0x643e,
    0x9cc9,0x8d40,0xbfdb,0xae52,0xdaed,0xcb64,0xf9ff,0xe876,
    0x2102,0x308b,0x0210,0x1399,0x6726,0x76af,0x4434,0x55bd,
    0xad4a,0xbcc3,0x8e58,0x9fd1,0xeb6e,0xfae7,0xc87c,0xd9f5,
    0x3183,0x200a,0x1291,0x0318,0x77a7,0x662e,0x54b5,0x453c,
    0xbdcb,0xac42,0x9ed9,0x8f50,0xfbef,0xea66,0xd8fd,0xc974,
    0x4204,0x538d,0x6116,0x709f,0x0420,0x15a9,0x2732,0x36bb,
    0xce4c,0xdfc5,0xed5e,0xfcd7,0x8868,0x99e1,0xab7a,0xbaf3,
    0x5285,0x430c,0x7197,0x601e,0x14a1,0x0528,0x37b3,0x263a,
    0xdecd,0xcf44,0xfddf,0xec56,0x98e9,0x8960,0xbbfb,0xaa72,
    0x6306,0x728f,0x4014,0x519d,0x2522,0x34ab,0x0630,0x17b9,
    0xef4e,0xfec7,0xcc5c,0xddd5,0xa96a,0xb8e3,0x8a78,0x9bf1,
    0x7387,0x620e,0x5095,0x411c,0x35a3,0x242a,0x16b1,0x0738,
    0xffcf,0xee46,0xdcdd,0xcd54,0xb9eb,0xa862,0x9af9,0x8b70,
    0x8408,0x9581,0xa71a,0xb693,0xc22c,0xd3a5,0xe13e,0xf0b7,
    0x0840,0x19c9,0x2b52,0x3adb,0x4e64,0x5fed,0x6d76,0x7cff,
    0x9489,0x8500,0xb79b,0xa612,0xd2ad,0xc324,0xf1bf,0xe036,
    0x18c1,0x0948,0x3bd3,0x2a5a,0x5ee5,0x4f6c,0x7df7,0x6c7e,
    0xa50a,0xb483,0x8618,0x9791,0xe32e,0xf2a7,0xc03c,0xd1b5,
    0x2942,0x38cb,0x0a50,0x1bd9,0x6f66,0x7eef,0x4c74,0x5dfd,
    0xb58b,0xa402,0x9699,0x8710,0xf3af,0xe226,0xd0bd,0xc134,
    0x39c3,0x284a,0x1ad1,0x0b58,0x7fe7,0x6e6e,0x5cf5,0x4d7c,
    0xc60c,0xd785,0xe51e,0xf497,0x8028,0x91a1,0xa33a,0xb2b3,
    0x4a44,0x5bcd,0x6956,0x78df,0x0c60,0x1de9,0x2f72,0x3efb,
    0xd68d,0xc704,0xf59f,0xe416,0x90a9,0x8120,0xb3bb,0xa232,
    0x5ac5,0x4b4c,0x79d7,0x685e,0x1ce1,0x0d68,0x3ff3,0x2e7a,
    0xe70e,0xf687,0xc41c,0xd595,0xa12a,0xb0a3,0x8238,0x93b1,
    0x6b46,0x7acf,0x4854,0x59dd,0x2d62,0x3ceb,0x0e70,0x1ff9,
    0xf78f,0xe606,0xd49d,0xc514,0xb1ab,0xa022,0x92b9,0x8330,
    0x7bc7,0x6a4e,0x58d5,0x495c,0x3de3,0x2c6a,0x1ef1,0x0f78
  };
  uint_farptr_t pdata = pgm_get_far_address(ZZCRCTable);
  uint16_t crc = 0;
  for(uint8_t i=0; i < len; i++)
    crc = (crc<<8) ^ pgm_read_word_far(pdata+2*(((crc>>8) ^ *data++) & 0xFF));
  return crc;
}

static void FRSKYX_initialize_data(uint8_t adr)
{
  CC2500_WriteReg(CC2500_0C_FSCTRL0, FREQFINE);  // Frequency offset hack
  CC2500_WriteReg(CC2500_18_MCSM0,    0x08);
  CC2500_WriteReg(CC2500_09_ADDR, adr ? 0x03 : g_eeGeneral.fixed_ID.ID_8[0]);
  CC2500_WriteReg(CC2500_07_PKTCTRL1,0x05);
}


static void set_start(uint8_t ch)
{
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_23_FSCAL3, calData[ch][0]);
  CC2500_WriteReg(CC2500_24_FSCAL2, calData[ch][1]);
  CC2500_WriteReg(CC2500_25_FSCAL1, calData[ch][2]);
  CC2500_WriteReg(CC2500_0A_CHANNR, (ch == 47) ? 0 : channels_used[ch]);
}

#define RXNUM g_model.modelId

static void frskyX_build_bind_packet()
{
  packet[0] = frskyX_format_EU ? 0x20 : 0x1D;// LBT (EU) or  FCC (US)
  packet[1] = 0x03;
  packet[2] = 0x01;
  packet[3] = g_eeGeneral.fixed_ID.ID_8[0];
  packet[4] = g_eeGeneral.fixed_ID.ID_8[1];
  packet[5] = X_state *5; // Index into channels_used array.
  packet[6] =  channels_used[ (packet[5]) +0];
  packet[7] =  channels_used[ (packet[5]) +1];
  packet[8] =  channels_used[ (packet[5]) +2];
  packet[9] =  channels_used[ (packet[5]) +3];
  packet[10] = channels_used[ (packet[5]) +4];
  packet[11] = 0x02;
  packet[12] = RXNUM;

  memset(&packet[13], 0, packet_size-15);

  uint16_t lcrc = crc(&packet[3], packet_size-5);
  packet[packet_size-2] = lcrc >> 8;
  packet[packet_size-1] = lcrc;

  ++X_state;
  if(X_state > 9) X_state = 0;
}


static uint16_t scaleForPXX(uint8_t chan, uint8_t failsafe)
{
// 0-2047, 0 = 817, 1024 = 1500, 2047 = 2182
  int16_t value = channelOutputs[chan];
  value = limit((int16_t)-1024, value, (int16_t)+1024);
  (chan > 7) ? value += 2048 : value += 1024;   // upper channels offset 2048, lower 1014
  return value;
}

#define FAILSAFE_TIMEOUT 1032

static void frskyX_data_frame()
{
  //0x1D 0xB3 0xFD 0x02 0x56 0x07 0x15 0x00 0x00 0x00 0x04 0x40 0x00 0x04 0x40 0x00 0x04 0x40 0x00 0x04 0x40 0x08 0x00 0x00 0x00 0x00 0x00 0x00 0x96 0x12
  // channel packing: H (0)7-4, L (0)3-0; H (1)3-0, L (0)11-8; H (1)11-8, L (1)7-4 etc
  uint16_t chan_0;
  uint16_t chan_1;
  static uint8_t failsafe_chan;
  static uint16_t failsafe_count;
  static uint8_t FS_flag;
  uint8_t startChan = 0;


  // data frames sent every 9ms; failsafe every 9 seconds
  if (FS_flag == 0  &&  failsafe_count > FAILSAFE_TIMEOUT  &&  channel_offset == 0 ) { // &&  Model.proto_opts[PROTO_OPTS_FAILSAFE] != FAILSAFE_RX) {
    FS_flag = 0x10;
    failsafe_chan = 0;
  } else if (FS_flag & 0x10 && failsafe_chan < (/*Model.num_channels-1*/16)) {
    FS_flag = 0x10 | ((FS_flag + 2) & 0x0f);
    failsafe_chan += 1;
  } else if (FS_flag & 0x10) {
    FS_flag = 0;
    failsafe_count = 0;
  }
  failsafe_count += 1;


  packet[0] = frskyX_format_EU ? 0x20 : 0x1D;
  packet[1] = g_eeGeneral.fixed_ID.ID_8[0];
  packet[2] = g_eeGeneral.fixed_ID.ID_8[1];

  packet[3] = 0x02;
  packet[4] = (ctr << 6) + channr;  //*64
  packet[5] = counter_rst;
  packet[6] = RXNUM;

  // packet[7] is FLAGS
  // 00 - standard packet
  // 10, 12, 14, 16, 18, 1A, 1C, 1E - failsafe packet
  packet[7] = 0;    // may be replaced by failsafe below
  packet[8] = 0;

  startChan = channel_offset;

  for(uint8_t i = 0; i < 12 ; i += 3) {    // 12 bytes of channel data
    if (FS_flag & 0x10 && (((failsafe_chan & 0x7) | channel_offset) == startChan)) {
      packet[7] = FS_flag;
      chan_0 = scaleForPXX(failsafe_chan, 1);
    } else {
      chan_0 = scaleForPXX(startChan, 0);
    }
    startChan++;

    if (FS_flag & 0x10 && (((failsafe_chan & 0x7) | channel_offset) == startChan)) {
      packet[7] = FS_flag;
      chan_1 = scaleForPXX(failsafe_chan, 1);
    } else {
      chan_1 = scaleForPXX(startChan, 0);
    }
    startChan++;

    packet[9+i]   = chan_0;
    packet[9+i+1] = (((chan_0 >> 8) & 0x0F) | (chan_1 << 4));
    packet[9+i+2] = chan_1 >> 4;
  }

  packet[21] = seq_last_sent << 4 | seq_last_rcvd;
  if (seq_last_sent < 8 && seq_last_rcvd < 8)
    seq_last_sent = (seq_last_sent + 1) % 4;
  else if (seq_last_rcvd == 0)
    seq_last_sent = 1;

  channel_offset ^= 0x08;

  memset(&packet[22], 0, packet_size-24);

  uint16_t lcrc = crc(&packet[3], packet_size-5);
  packet[packet_size-2] = lcrc >> 8;
  packet[packet_size-1] = lcrc;
}


// Telemetry
/* SPORT details serial
  100K 8E2 normal-multiprotocol
  -every 12ms-
  1  2  3  4  5  6  7  8  9  CRC DESCR
  7E 98 10 05 F1 20 23 0F 00 A6 SWR_ID
  7E 98 10 01 F1 33 00 00 00 C9 RSSI_ID
  7E 98 10 04 F1 58 00 00 00 A1 BATT_ID
  7E BA 10 03 F1 E2 00 00 00 18 ADC2_ID
  7E BA 10 03 F1 E2 00 00 00 18 ADC2_ID
  7E BA 10 03 F1 E2 00 00 00 18 ADC2_ID
  7E BA 10 03 F1 E2 00 00 00 18 ADC2_ID
  7E BA 10 03 F1 E2 00 00 00 18 ADC2_ID
  7E BA 10 03 F1 E2 00 00 00 18 ADC2_ID


  Telemetry frames(RF) SPORT info 15 bytes
  SPORT frame 6+3 bytes
  [00] PKLEN  0E 0E 0E 0E
  [01] TXID1  DD DD DD DD
  [02] TXID2  6D 6D 6D 6D
  [03] CONST  02 02 02 02
  [04] RS/RB  2C D0 2C CE // D0,CE = 2*RSSI; ....2C = RX battery voltage(5V from Bec)
  [05] ?????  03 10 21 32 // TX/RX telemetry hand-shake bytes
  [06] NO.BT  00 00 06 03 // No.of valid SPORT frame bytes in the frame
  [07] STRM1  00 00 7E 00
  [08] STRM2  00 00 1A 00
  [09] STRM3  00 00 10 00
  [10] STRM4  03 03 03 03
  [11] STRM5  F1 F1 F1 F1
  [12] STRM6  D1 D1 D0 D0
  [13] CHKSUM1
  [14] CHKSUM2
  [15] RSSI
  [16] LQI
*/

//#define START_STOP              0x7e
//#define BYTESTUFF               0x7d
//#define STUFF_MASK              0x20
//#define FRSKY_SPORT_PACKET_SIZE    8
#define TELEM_PKT_SIZE            17

// FrSky PRIM IDs (1 byte)
//#define DATA_FRAME                0x10

#define SPORT_DATA_uint8_t(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_uint32_t(packet)  (*((uint32_t *)(packet+4)))
#define HUB_DATA_uint16_t(packet)    (*((uint16_t *)(packet+4)))


#if HAS_EXTENDED_TELEMETRY

#include "frsky_d_telem._c"

static uint8_t sport_crc(uint8_t *data)
{
  uint16_t crc = 0;
  for (uint8_t i=1; i < FRSKY_SPORT_PACKET_SIZE-1; ++i) {
    crc += data[i];
    crc += crc >> 8;
    crc &= 0x00ff;
  }
  return 0x00ff - crc;
}

static void serial_echo(uint8_t *packet)
{
  static uint8_t outbuf[FRSKY_SPORT_PACKET_SIZE+2] = {0x7e};

  memcpy(outbuf+1, packet, FRSKY_SPORT_PACKET_SIZE);
  outbuf[FRSKY_SPORT_PACKET_SIZE+1] = sport_crc(outbuf+2);
  UART_Send(outbuf, FRSKY_SPORT_PACKET_SIZE+2);
}


static void processSportPacket(uint8_t *packet)
{
//    uint8_t  instance = (packet[0] & 0x1F) + 1;    // all instances of same sensor write to same telemetry value
  uint8_t  prim = packet[1];
  uint16_t id = *((uint16_t *)(packet+2));

  if (!PPMin_Mode())
    serial_echo(packet);   // echo to trainer port

  if (prim != DATA_FRAME)
    return;

  if ((id >> 8) == 0) {
    // The old FrSky IDs
    processHubPacket(id, HUB_DATA_uint16_t(packet));
    return;
  }

  // rx telemetry in stream data
  // 8 bit voltages are raw ADC with 3.3V reference => 13.2mV/bit
  // Convert to two decimal places (hundredths of volts)
  switch(id) {
  case ADC1_ID:    // put in VOLT3 so ADC2_ID (external input) stays consistent with frsky2way
    set_telemetry(TELEM_FRSKY_VOLT3, SPORT_DATA_uint8_t(packet) * 132 / 100);
    break;
  case ADC2_ID:    // put in VOLT2 for consistency with frsky2way
    set_telemetry(TELEM_FRSKY_VOLT2, ((int32_t)SPORT_DATA_uint8_t(packet) * 132 * Model.proto_opts[PROTO_OPTS_AD2GAIN]) / 10000);
    break;
  case BATT_ID:
    set_telemetry(TELEM_FRSKY_VOLTA, SPORT_DATA_uint8_t(packet) * 132 / 100);
    break;
  }

  int32_t data = SPORT_DATA_S32(packet);
  switch (id & 0xfff0) {
  case ALT_FIRST_ID & 0xfff0:
    if (ground_level == 0) ground_level = data;
    set_telemetry(TELEM_FRSKY_ALTITUDE, data - ground_level);
    break;
  case VARIO_FIRST_ID & 0xfff0:
    set_telemetry(TELEM_FRSKY_VARIO, data);
    break;
  case CURR_FIRST_ID & 0xfff0:
    set_telemetry(TELEM_FRSKY_CURRENT, data);
    if (discharge_time == 0) discharge_time = CLOCK_getms();
    discharge_dAms += data * (CLOCK_getms() - discharge_time);
    discharge_time = CLOCK_getms();
    set_telemetry(TELEM_FRSKY_DISCHARGE, discharge_dAms / 36000);
    break;;
  case VFAS_FIRST_ID & 0xfff0:
    set_telemetry(TELEM_FRSKY_VOLTA, data);
    break;

  case CELLS_FIRST_ID & 0xfff0: {
    uint8_t cells_count = (data & 0xF0) >> 4;
    uint8_t cell_index  = (data & 0x0F);
    update_cell(cell_index, ((data & 0x000FFF00) >> 8) / 5);
    if (cell_index+1 < cells_count)
      update_cell(cell_index+1, ((data & 0xFFF00000) >> 20) / 5);
    update_min_cell(cells_count);
    break;
  }

  case T1_FIRST_ID & 0xfff0:
    set_telemetry(TELEM_FRSKY_TEMP1, data);
    break;
  case T2_FIRST_ID & 0xfff0:
    set_telemetry(TELEM_FRSKY_TEMP2, data);
    break;
  case RPM_FIRST_ID & 0xfff0:
    set_telemetry(TELEM_FRSKY_RPM, data);
    break;
  case FUEL_FIRST_ID & 0xfff0:
    set_telemetry(TELEM_FRSKY_FUEL, data);
    break;

  case GPS_LONG_LATI_FIRST_ID & 0xfff0: {
    uint32_t gps_long_lati_b1w = (data & 0x3fffffff) / 10000;
    uint32_t gps_long_lati_a1w = (data & 0x3fffffff) % 10000;
    int32_t bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
    int32_t ap = gps_long_lati_a1w;
    int32_t deg = bp / 100;
    int32_t min = bp % 100;
    if (data & (1 << 31)) {
      Telemetry.gps.longitude = (deg * 60 + min) * 60 * 1000 + ap * 6;
      if (data & (1 << 30))
        Telemetry.gps.longitude = -Telemetry.gps.longitude;   // west negative
      TELEMETRY_SetUpdated(TELEM_GPS_LONG);
    } else {
      Telemetry.gps.latitude = (deg * 60 + min) * 60 * 1000 + ap * 6;
      if (data & (1 << 30))
        Telemetry.gps.latitude = -Telemetry.gps.latitude;   // south negative
      TELEMETRY_SetUpdated(TELEM_GPS_LAT);
    }
    break;
  }

  case GPS_ALT_FIRST_ID & 0xfff0:
    Telemetry.gps.altitude = data * 10;
    TELEMETRY_SetUpdated(TELEM_GPS_ALT);
    break;
  case GPS_SPEED_FIRST_ID & 0xfff0:
    Telemetry.gps.velocity = data * 5556 / 10800;
    TELEMETRY_SetUpdated(TELEM_GPS_SPEED);
    break;
  case GPS_COURS_FIRST_ID & 0xfff0:
    // = TELEM_GPS_HEADING;
    break;
  case GPS_TIME_DATE_FIRST_ID & 0xfff0:
    if (data & 0x000000ff) {
      fr_gps.year =      (uint16_t) ((data & 0xff000000) >> 24);
      fr_gps.day_month = (uint16_t) ((data & 0x00ffff00) >> 8);
    } else {
      fr_gps.hour_min = (uint16_t) ((data & 0xffff0000) >> 16);
      fr_gps.second =   (uint16_t) ((data & 0x0000ff00) >> 8);
      Telemetry.gps.time = ( (uint32_t)fr_gps.year & 0x3f)            << 26
                           | (((uint32_t)fr_gps.day_month >> 8) & 0x0f) << 22
                           | ( (uint32_t)fr_gps.day_month & 0x1f)       << 17
                           | (((uint32_t)fr_gps.hour_min >> 8) & 0x1f)  << 12
                           | ( (uint32_t)fr_gps.hour_min & 0x3f)        << 6
                           | ( (uint32_t)fr_gps.second & 0x3f);
      TELEMETRY_SetUpdated(TELEM_GPS_TIME);
    }
    break;
  } // switch
// not sure about these
//    case A3_FIRST_ID & 0xfff0:
//    case A4_FIRST_ID & 0xfff0:
//    case AIR_SPEED_FIRST_ID & 0xfff0:
//    case ACCX_FIRST_ID & 0xfff0:
//    case ACCY_FIRST_ID & 0xfff0:
//    case ACCZ_FIRST_ID & 0xfff0:
}


typedef enum {
  X_state_DATA_IDLE,
  X_state_DATA_START,
  X_state_DATA_IN_FRAME,
  X_state_DATA_XOR,
} SportX_states;
static SportX_states dataX_state = X_state_DATA_IDLE;   // file scope so can be reset on loss of rx packet sync

static void frsky_parse_sport_stream(uint8_t data)
{
  static uint8_t sportRxBufferCount;
  static uint8_t sportRxBuffer[FRSKY_SPORT_PACKET_SIZE];   // Receive buffer. 8 bytes (full packet)

  switch (dataX_state) {
  case X_state_DATA_START:
    if (data == START_STOP) {
      dataX_state = X_state_DATA_IN_FRAME ;
      sportRxBufferCount = 0;
    } else {
      if (sportRxBufferCount < FRSKY_SPORT_PACKET_SIZE)
        sportRxBuffer[sportRxBufferCount++] = data;
      dataX_state = X_state_DATA_IN_FRAME;
    }
    break;

  case X_state_DATA_IN_FRAME:
    if (data == BYTESTUFF) {
      dataX_state = X_state_DATA_XOR; // XOR next byte
    } else if (data == START_STOP) {
      dataX_state = X_state_DATA_IN_FRAME ;
      sportRxBufferCount = 0;
    } else if (sportRxBufferCount < FRSKY_SPORT_PACKET_SIZE) {
      sportRxBuffer[sportRxBufferCount++] = data;
    }
    break;

  case X_state_DATA_XOR:
    if (sportRxBufferCount < FRSKY_SPORT_PACKET_SIZE) {
      sportRxBuffer[sportRxBufferCount++] = data ^ STUFF_MASK;
    }
    dataX_state = X_state_DATA_IN_FRAME;
    break;

  case X_state_DATA_IDLE:
    if (data == START_STOP) {
      sportRxBufferCount = 0;
      dataX_state = X_state_DATA_START;
    }
    break;
  } // switch

  if (sportRxBufferCount >= FRSKY_SPORT_PACKET_SIZE) {
    processSportPacket(sportRxBuffer);
    dataX_state = X_state_DATA_IDLE;
  }
}

#endif // HAS_EXTENDED_TELEMETRY

/*static void frsky_check_telemetry(uint8_t *pkt, uint8_t len)
{
  // only process packets with the required id and packet length and good crc
    if (len == TELEM_PKT_SIZE
        && pkt[0] == TELEM_PKT_SIZE - 3
        && pkt[1] == g_eeGeneral.fixed_ID.ID_8[0]
        && pkt[2] == g_eeGeneral.fixed_ID.ID_8[1]
        && crc(&pkt[3], TELEM_PKT_SIZE-7) == (pkt[TELEM_PKT_SIZE-4] << 8 | pkt[TELEM_PKT_SIZE-3])
       ) {
        if (pkt[4] & 0x80) {   // distinguish RSSI from VOLT1
            Telemetry.value[TELEM_FRSKY_RSSI] = pkt[4] & 0x7f;
            TELEMETRY_SetUpdated(TELEM_FRSKY_RSSI);
        } else {
            Telemetry.value[TELEM_FRSKY_VOLT1] = pkt[4] * 10;      // In 1/100 of Volts
            TELEMETRY_SetUpdated(TELEM_FRSKY_VOLT1);
        }

        Telemetry.value[TELEM_FRSKY_LQI] = pkt[len-1] & 0x7f;
        TELEMETRY_SetUpdated(TELEM_FRSKY_LQI);

        Telemetry.value[TELEM_FRSKY_LRSSI] = (int8_t)pkt[len-2] / 2 - 70;  // Value in dBm
        TELEMETRY_SetUpdated(TELEM_FRSKY_LRSSI);

        if ((pkt[5] >> 4 & 0x0f) == 0x08) {   // restart or somesuch
            seq_last_sent = 8;
            seq_last_rcvd = 0;
  #if HAS_EXTENDED_TELEMETRY
            dataX_state = X_state_DATA_IDLE;    // reset sport decoder
  #endif
        } else {
            if ((pkt[5] >> 4 & 0x03) == (seq_last_rcvd + 1) % 4) {
                seq_last_rcvd = (seq_last_rcvd + 1) % 4;
            }
  #if HAS_EXTENDED_TELEMETRY
            else
                dataX_state = X_state_DATA_IDLE;    // reset sport decoder if sequence number wrong
  #endif
        }

  #if HAS_EXTENDED_TELEMETRY
        if (pkt[6] <= 6)
            for (uint8_t i=0; i < pkt[6]; i++)
                frsky_parse_sport_stream(pkt[7+i]);
  #endif
    }
}*/

static uint16_t FRSKYX_bind_cb()
{
  SCHEDULE_MIXER_END(18*16); // Schedule next Mixer calculations.
  frskyX_build_bind_packet();
  set_start(47);
  CC2500_SetPower(TXPOWER_1);
  CC2500_Strobe(CC2500_SFRX);
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_Strobe(CC2500_SFTX);
  CC2500_WriteData(packet, packet[0]+1);
  CC2500_Strobe(CC2500_STX);
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

static uint16_t FRSKYX_cb()
{
  uint8_t len;

  switch(X_state) {

  case FRSKYX_DATA1:
    SCHEDULE_MIXER_END(9*16); // Schedule next Mixer calculations.
    CC2500_WriteReg(CC2500_0C_FSCTRL0, FREQFINE);
    frskyX_data_frame();
    CC2500_SetTxRxMode(TX_EN);
    CC2500_Strobe(CC2500_SIDLE); // Force idle if still receiving in error condition.
    set_start(channr);
    CC2500_SetPower(TXPOWER_1);
    CC2500_Strobe(CC2500_SIDLE);
    CC2500_Strobe(CC2500_SFTX);
    CC2500_WriteData(packet, packet[0]+1);
    CC2500_Strobe(CC2500_STX);
    channr = (channr + chanskip) % 47;
    ++X_state;
    heartbeat |= HEART_TIMER_PULSES;
    CALCULATE_LAT_JIT(); // Calculate latency and jitter.
    return 5500*2;

  case FRSKYX_DATA2:
    CC2500_SetTxRxMode(RX_EN);
    CC2500_Strobe(CC2500_SIDLE);
    ++X_state;
    return 200*2;

  case FRSKYX_DATA3:
    CC2500_Strobe(CC2500_SRX);
    ++X_state;
    return 3000*2;

  case FRSKYX_DATA4:
    len = CC2500_ReadReg(CC2500_3B_RXBYTES | CC2500_READ_BURST) & 0x7F;
    if (len && len < MAX_PACKET_SIZE) {
      CC2500_ReadData(packet, len);
      //frsky_check_telemetry(packet, len);

       uint8_t good = (
        packet[0] == len - 3
        && packet[1] == g_eeGeneral.fixed_ID.ID_8[0]
        && packet[2] == g_eeGeneral.fixed_ID.ID_8[1]
       );

#if defined(FRSKY)
      if (good) {
        memcpy(Usart0RxBuffer, packet, len);
        if(frskyStreaming < FRSKY_TIMEOUT10ms -5) frskyStreaming +=5;
      }
      // frskyStreaming gets decremented every 10ms, however we can only add to it every 4 *9ms, so we add 5.
#endif


    } else {
      // restart sequence on missed packet - might need count or timeout instead of one missed
      seq_last_sent = 0;
      seq_last_rcvd = 8;
    }
    X_state = FRSKYX_DATA1;
    return 300*2;

  default :
    return 0;
  }
}

static void FRSKYX_init()
{
  CC2500_Strobe(CC2500_SIDLE);

  uint_farptr_t pdata = pgm_get_far_address(ZZ_frsky_init_data);

  for (uint8_t i=0; i<(DIM(ZZ_frsky_init_data)/3); i++) { // Send init EU or US
    uint8_t add = pgm_read_byte_far(pdata);
    uint8_t dat = frskyX_format_EU ? pgm_read_byte_far(pdata+=2) : pgm_read_byte_far(++pdata);
    CC2500_WriteReg(add,dat);
    ++pdata;
  }

  pdata = pgm_get_far_address(ZZ_frsky_init_data_shared);

    for (uint8_t i=0; i<(DIM(ZZ_frsky_init_data_shared)/2); i++) { // Send init shared
    uint8_t add = pgm_read_byte_far(pdata);
    uint8_t dat = pgm_read_byte_far(++pdata);
    CC2500_WriteReg(add,dat);
    ++pdata;
  }

  SCHEDULE_MIXER_END(41*16); // Schedule next Mixer calculations.
  //calibrate hop channels
  for (uint8_t c = 0; c < 47; c++) {
    CC2500_Strobe(CC2500_SIDLE);
    CC2500_WriteReg(CC2500_0A_CHANNR, channels_used[c]);
    CC2500_Strobe(CC2500_SCAL);
    _delay_us(900);
    calData[c][0] = CC2500_ReadReg(CC2500_23_FSCAL3);
    calData[c][1] = CC2500_ReadReg(CC2500_24_FSCAL2);
    calData[c][2] = CC2500_ReadReg(CC2500_25_FSCAL1);
  }
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0);
  CC2500_Strobe(CC2500_SCAL);
  _delay_us(900);
  calData[47][0] = CC2500_ReadReg(CC2500_23_FSCAL3); // Charge pump current calibration
  calData[47][1] = CC2500_ReadReg(CC2500_24_FSCAL2); // VCO current calibration
  calData[47][2] = CC2500_ReadReg(CC2500_25_FSCAL1); // VCO capacitance calibration
}

static void FRSKYX_initialize(uint8_t bind)
{
  PROTO_Stop_Callback();
  CC2500_Reset();
  packet_size = frskyX_format_EU ? 33 : 30;
  channel_offset = 0;
  channr = 0;
  chanskip = 0;
  ctr = 0;
  seq_last_sent = 0;
  seq_last_rcvd = 8;
#if HAS_EXTENDED_TELEMETRY
  Telemetry.value[TELEM_FRSKY_MIN_CELL] = TELEMETRY_GetMaxValue(TELEM_FRSKY_MIN_CELL);
  //UART_SetDataRate(57600);    // set for s.port compatibility
#endif


  //for(uint8_t x = 0; x < 50; x ++) { channels_used[x] = hop_data[x]; }

  // Build channel array. (V code for test)
  channel_offset = (g_eeGeneral.fixed_ID.ID_8[1] << 8 | g_eeGeneral.fixed_ID.ID_8[0]) % 5;
  uint8_t chan_num;
  for(uint8_t x = 0; x < 50; x ++) {
    chan_num = (x*5) + 3 + channel_offset;
    channels_used[x] = (chan_num ? chan_num : 1); // Avoid binding channel 0.
  }


#if defined (SIMU)
#define srandom(x) srand(x)
#define random() rand()
#endif

  while (!chanskip) {
    srandom(g_eeGeneral.fixed_ID.ID_32 & 0xfefefefe);
    chanskip = random()%47;
  }
  while((chanskip - ctr) % 4) {
    ctr = (ctr+1) % 4;
  }
  counter_rst = (chanskip - ctr) >> 2;

  FRSKYX_init();

  CC2500_SetTxRxMode(TX_EN);  // enable PA

  if (bind) {
    //PROTOCOL_SetBindState(0xFFFFFFFF);
    FRSKYX_initialize_data(1);
    X_state = 0;
    PROTO_Start_Callback(25000U *2, FRSKYX_bind_cb);

  } else {
    //PROTOCOL_SetBindState(0);
    FRSKYX_initialize_data(0);
    channr = 0;
    X_state = FRSKYX_DATA1;
    PROTO_Start_Callback(25000U *2, FRSKYX_cb);
  }
}

const void *FRSKYX_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    FRSKYX_initialize(0);
    return 0;
  case PROTOCMD_BIND:
    FRSKYX_initialize(1);
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    CC2500_Reset();
    CC2500_SetTxRxMode(TXRX_OFF);
    CC2500_Strobe(CC2500_SIDLE);
    return 0;
    case PROTOCMD_GETOPTIONS:
          SetRfOptionSettings(pgm_get_far_address(RfOpt_FrskyX_Ser),
                        STR_DUMMY,       //Sub proto
                        STR_RFTUNE,      //Option 1 (int)
                        STR_DUMMY,       //Option 2 (int)
                        STR_DUMMY,       //Option 3 (uint 0 to 31)
                        STR_TELEMETRY,   //OptionBool 1
                        STR_DUMMY,       //OptionBool 2
                        STR_DUMMY        //OptionBool 3
                        );
    return 0;

  /*case PROTOCMD_CHECK_AUTOBIND:
    return 0; //Never Autobind
  case PROTOCMD_NUMCHAN:
    return (void *)12L;
  case PROTOCMD_DEFAULT_NUMCHAN:
    return (void *)8L;
  case PROTOCMD_CURRENT_ID:
    return Model.frsky_id ? (void *)((unsigned long)Model.frsky_id) : 0;
  case PROTOCMD_GETOPTIONS:
    if (!Model.proto_opts[PROTO_OPTS_AD2GAIN]) Model.proto_opts[PROTO_OPTS_AD2GAIN] = 100;  // if not set, default to no gain
    return frskyx_opts;
  case PROTOCMD_TELEMETRYX_state:
    return (void *)1L;
  case PROTOCMD_TELEMETRYTYPE:
    return (void *)(long) TELEM_FRSKY;
  case PROTOCMD_DEINIT:
  #if HAS_EXTENDED_TELEMETRY
    UART_SetDataRate(0);  // restore data rate to default
  #endif*/
  default:
    break;
  }
  return 0;
}
