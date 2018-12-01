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


#ifdef MODULAR
  //Reduce size
  #define RANDOM_CHANNELS  0
#else
  #define RANDOM_CHANNELS  1
#endif

#define BIND_CHANNEL 0x0d //This can be any odd channel

#define NUM_WAIT_LOOPS (100 )// 5) //each loop is ~5us.  Do not wait more than 100us TODO : better wait function

const static RfOptionSettingsvarstruct RfOpt_DSM_Ser[] PROGMEM = {
  /*rfProtoNeed*/PROTO_NEED_SPI, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/0,
};


enum {
  DSM_OPT_TELEMETRY = 0,
  DSM_OPT_ORANGERX,
#ifndef MODULAR
  DSM_OPT_HIGHSPEED,
  DSM_OPT_FLOGFILTER,
#endif
  DSM_OPT_LAST,
};


#define ORANGERX_YES 1
#define ORANGERX_NO 0
#ifndef MODULAR
  #define HIGHSPEED_ON 1
  #define HIGHSPEED_OFF 0
  #define FLOGFILTER_ON 1
  #define FLOGFILTER_OFF 0
#endif

//During binding we will send DSM_BIND_COUNT/2 packets
//One packet each 10msec
#define DSM_BIND_COUNT 600

enum {
  DSM2_BIND = 0,
  DSM2_CHANSEL     = DSM_BIND_COUNT + 0,
  DSM2_CH1_WRITE_A = DSM_BIND_COUNT + 1,
  DSM2_CH1_CHECK_A = DSM_BIND_COUNT + 2,
  DSM2_CH2_WRITE_A = DSM_BIND_COUNT + 3,
  DSM2_CH2_CHECK_A = DSM_BIND_COUNT + 4,
  DSM2_CH2_READ_A  = DSM_BIND_COUNT + 5,
  DSM2_CH1_WRITE_B = DSM_BIND_COUNT + 6,
  DSM2_CH1_CHECK_B = DSM_BIND_COUNT + 7,
  DSM2_CH2_WRITE_B = DSM_BIND_COUNT + 8,
  DSM2_CH2_CHECK_B = DSM_BIND_COUNT + 9,
  DSM2_CH2_READ_B  = DSM_BIND_COUNT + 10,
};

static const uint8_t zzpncodes[5][9][8] PROGMEM = {
  /* Note these are in order transmitted (LSB 1st) */
  { /* Row 0 */
    /* Col 0 */ {0x03, 0xBC, 0x6E, 0x8A, 0xEF, 0xBD, 0xFE, 0xF8},
    /* Col 1 */ {0x88, 0x17, 0x13, 0x3B, 0x2D, 0xBF, 0x06, 0xD6},
    /* Col 2 */ {0xF1, 0x94, 0x30, 0x21, 0xA1, 0x1C, 0x88, 0xA9},
    /* Col 3 */ {0xD0, 0xD2, 0x8E, 0xBC, 0x82, 0x2F, 0xE3, 0xB4},
    /* Col 4 */ {0x8C, 0xFA, 0x47, 0x9B, 0x83, 0xA5, 0x66, 0xD0},
    /* Col 5 */ {0x07, 0xBD, 0x9F, 0x26, 0xC8, 0x31, 0x0F, 0xB8},
    /* Col 6 */ {0xEF, 0x03, 0x95, 0x89, 0xB4, 0x71, 0x61, 0x9D},
    /* Col 7 */ {0x40, 0xBA, 0x97, 0xD5, 0x86, 0x4F, 0xCC, 0xD1},
    /* Col 8 */ {0xD7, 0xA1, 0x54, 0xB1, 0x5E, 0x89, 0xAE, 0x86}
  },
  { /* Row 1 */
    /* Col 0 */ {0x83, 0xF7, 0xA8, 0x2D, 0x7A, 0x44, 0x64, 0xD3},
    /* Col 1 */ {0x3F, 0x2C, 0x4E, 0xAA, 0x71, 0x48, 0x7A, 0xC9},
    /* Col 2 */ {0x17, 0xFF, 0x9E, 0x21, 0x36, 0x90, 0xC7, 0x82},
    /* Col 3 */ {0xBC, 0x5D, 0x9A, 0x5B, 0xEE, 0x7F, 0x42, 0xEB},
    /* Col 4 */ {0x24, 0xF5, 0xDD, 0xF8, 0x7A, 0x77, 0x74, 0xE7},
    /* Col 5 */ {0x3D, 0x70, 0x7C, 0x94, 0xDC, 0x84, 0xAD, 0x95},
    /* Col 6 */ {0x1E, 0x6A, 0xF0, 0x37, 0x52, 0x7B, 0x11, 0xD4},
    /* Col 7 */ {0x62, 0xF5, 0x2B, 0xAA, 0xFC, 0x33, 0xBF, 0xAF},
    /* Col 8 */ {0x40, 0x56, 0x32, 0xD9, 0x0F, 0xD9, 0x5D, 0x97}
  },
  { /* Row 2 */
    /* Col 0 */ {0x40, 0x56, 0x32, 0xD9, 0x0F, 0xD9, 0x5D, 0x97},
    /* Col 1 */ {0x8E, 0x4A, 0xD0, 0xA9, 0xA7, 0xFF, 0x20, 0xCA},
    /* Col 2 */ {0x4C, 0x97, 0x9D, 0xBF, 0xB8, 0x3D, 0xB5, 0xBE},
    /* Col 3 */ {0x0C, 0x5D, 0x24, 0x30, 0x9F, 0xCA, 0x6D, 0xBD},
    /* Col 4 */ {0x50, 0x14, 0x33, 0xDE, 0xF1, 0x78, 0x95, 0xAD},
    /* Col 5 */ {0x0C, 0x3C, 0xFA, 0xF9, 0xF0, 0xF2, 0x10, 0xC9},
    /* Col 6 */ {0xF4, 0xDA, 0x06, 0xDB, 0xBF, 0x4E, 0x6F, 0xB3},
    /* Col 7 */ {0x9E, 0x08, 0xD1, 0xAE, 0x59, 0x5E, 0xE8, 0xF0},
    /* Col 8 */ {0xC0, 0x90, 0x8F, 0xBB, 0x7C, 0x8E, 0x2B, 0x8E}
  },
  { /* Row 3 */
    /* Col 0 */ {0xC0, 0x90, 0x8F, 0xBB, 0x7C, 0x8E, 0x2B, 0x8E},
    /* Col 1 */ {0x80, 0x69, 0x26, 0x80, 0x08, 0xF8, 0x49, 0xE7},
    /* Col 2 */ {0x7D, 0x2D, 0x49, 0x54, 0xD0, 0x80, 0x40, 0xC1},
    /* Col 3 */ {0xB6, 0xF2, 0xE6, 0x1B, 0x80, 0x5A, 0x36, 0xB4},
    /* Col 4 */ {0x42, 0xAE, 0x9C, 0x1C, 0xDA, 0x67, 0x05, 0xF6},
    /* Col 5 */ {0x9B, 0x75, 0xF7, 0xE0, 0x14, 0x8D, 0xB5, 0x80},
    /* Col 6 */ {0xBF, 0x54, 0x98, 0xB9, 0xB7, 0x30, 0x5A, 0x88},
    /* Col 7 */ {0x35, 0xD1, 0xFC, 0x97, 0x23, 0xD4, 0xC9, 0x88},
    /* Col 8 */ {0xE1, 0xD6, 0x31, 0x26, 0x5F, 0xBD, 0x40, 0x93}
  },
  { /* Row 4 */
    /* Col 0 */ {0xE1, 0xD6, 0x31, 0x26, 0x5F, 0xBD, 0x40, 0x93},
    /* Col 1 */ {0xDC, 0x68, 0x08, 0x99, 0x97, 0xAE, 0xAF, 0x8C},
    /* Col 2 */ {0xC3, 0x0E, 0x01, 0x16, 0x0E, 0x32, 0x06, 0xBA},
    /* Col 3 */ {0xE0, 0x83, 0x01, 0xFA, 0xAB, 0x3E, 0x8F, 0xAC},
    /* Col 4 */ {0x5C, 0xD5, 0x9C, 0xB8, 0x46, 0x9C, 0x7D, 0x84},
    /* Col 5 */ {0xF1, 0xC6, 0xFE, 0x5C, 0x9D, 0xA5, 0x4F, 0xB7},
    /* Col 6 */ {0x58, 0xB5, 0xB3, 0xDD, 0x0E, 0x28, 0xF1, 0xB0},
    /* Col 7 */ {0x5F, 0x30, 0x3B, 0x56, 0x96, 0x45, 0xF4, 0xA1},
    /* Col 8 */ {0x03, 0xBC, 0x6E, 0x8A, 0xEF, 0xBD, 0xFE, 0xF8}
  },
};

//static const uint8_t pn_bind[] = {0xD7, 0xA1, 0x54, 0xB1, 0x5E, 0x89, 0xAE, 0x86, 0xc6, 0x94, 0x22, 0xfe, 0x48, 0xe6, 0x57, 0x4e};
//static const uint8_t pn_bind_transmit[] = {0xD7, 0xA1, 0x54, 0xB1, 0x5E, 0x89, 0xAE, 0x86, 0xc6, 0x94, 0x22, 0xfe, 0x48, 0xe6, 0x57, 0x4e};
//static const uint8_t pn_bind_receive1[] = {0x98, 0x88, 0x1B, 0xE4, 0x30, 0x79, 0x03, 0x84, 0x06, 0x0C, 0x12, 0x18, 0x1E, 0x24, 0x71, 0x10};
//static const uint8_t pn_bind_receive2[] = {0x98, 0x88, 0x1B, 0xE4, 0x30, 0x79, 0x03, 0x84, 0xC9, 0x2C, 0x06, 0x93, 0x86, 0xB9, 0x9E, 0xD7};


//up to 7 channels
static const uint8_t ch_map7[] =  {1, 5, 2, 3, 0, 4, 6}; //DX6i
//from 8 channels up to 12 channels
static const uint8_t ch_map12[] = {1, 5, 2, 4, 6, 10, 0xff, 0, 7, 3, 8, 9, 11, 0xff}; //12ch - DX18
#ifndef MODULAR
// "High Speed" 11ms for 8...10 channels
static const uint8_t ch_map14[] = {1, 5, 2, 3, 4, 6, 8, 1, 5, 2, 3, 0, 7, 9};
#endif

//uint8_t channels[23]; Use packet[30 - 53] to save memory
uint8_t chidx;
uint8_t sop_col;
uint8_t data_col;
uint16_t dsm_state;
uint8_t crcidx;
uint8_t binding;
uint8_t num_channels;
uint16_t checksum;

#define OFFSETINPACKETFORPNCODE 20

#define OFFSETINPACKETFORCHANNEL 30
#define CHANNELS packet

static void CYRF_Send_pncodes_ConfigDataCode(uint8_t row, uint8_t col)
{
  uint_farptr_t pncodes = pgm_get_far_address(zzpncodes);
  pncodes += (72*row + 8*col); // Find good adress
  for (uint8_t i=0; i<8 ; i++) {
    packet[OFFSETINPACKETFORPNCODE+i] = pgm_read_byte_far(pncodes+i); // Load into packet to save ram
  }
  CYRF_ConfigDataCode(&packet[OFFSETINPACKETFORPNCODE], 8);
}

static void CYRF_Send_pncodes_ConfigSOPCode(uint8_t row, uint8_t col)
{
  uint_farptr_t pncodes = pgm_get_far_address(zzpncodes);
  pncodes += (72*row + 8*col); // Find good adress
  for (uint8_t i=0; i<8 ; i++) {
    packet[OFFSETINPACKETFORPNCODE+i] = pgm_read_byte_far(pncodes+i); // Load into packet to save ram
  }
    CYRF_ConfigSOPCode(&packet[OFFSETINPACKETFORPNCODE]);
}


static void build_bind_packet()
{
  uint16_t sum = 384 - 0x10;
  packet[0] = checksum >> 8;
  packet[1] = checksum & 0xff;
  packet[2] = 0xff ^ cyrfmfg_id[2];
  packet[3] = (0xff ^ cyrfmfg_id[3]) + g_model.modelId;
  packet[4] = packet[0];
  packet[5] = packet[1];
  packet[6] = packet[2];
  packet[7] = packet[3];
  for(uint8_t i = 0; i < 8; i++)
    sum += packet[i];
  packet[8] = sum >> 8;
  packet[9] = sum & 0xff;
  packet[10] = 0x01; //???
  packet[11] = num_channels;
  if(Protos[g_model.rfProtocol].Protocol == PROTOCOL_DSMX) {
    packet[12] = num_channels < 8 /*&& Model.proto_opts[PROTOOPTS_TELEMETRY] == TELEM_OFF*/ ? 0xa2 : 0xb2;
  } else {
    packet[12] = num_channels < 8 ? 0x01 : 0x02;
  }
  packet[13] = 0x00; //???
  for(uint8_t i = 8; i < 14; i++)
    sum += packet[i];
  packet[14] = sum >> 8;
  packet[15] = sum & 0xff;
}

static void build_data_packet(uint8_t upper)
{
  const uint8_t *chmap;
#if defined(X_ANY)

#if (X_ANY >= 1)
  Xany_scheduleTx(0);
#endif
#if (X_ANY >= 2)
  Xany_scheduleTx(1);
#endif
#if (X_ANY >= 3)
  Xany_scheduleTx(2);
#endif
#if (X_ANY >= 4)
  Xany_scheduleTx(3);
#endif

#endif
  //if (binding && PROTOCOL_SticksMoved(0)) {
  //Don't turn off dialog until sticks are moved
  //  PROTOCOL_SetBindState(0);  //Turn off Bind dialog
  //binding = 0;
  //}
  if (Protos[g_model.rfProtocol].Protocol == PROTOCOL_DSMX) {
    packet[0] = cyrfmfg_id[2];
    packet[1] = cyrfmfg_id[3] + g_model.modelId;
  } else {
    packet[0] = (0xff ^ cyrfmfg_id[2]);
    packet[1] = (0xff ^ cyrfmfg_id[3]) + g_model.modelId;
  }
  uint8_t bits = Protos[g_model.rfProtocol].Protocol == PROTOCOL_DSMX ? 11 : 10;
  if(num_channels < 8) {
    chmap = ch_map7;
    /*#ifndef MODULAR
        } else if((num_channels < 11) && (Model.proto_opts[PROTOOPTS_HIGHSPEED] == HIGHSPEED_ON)) {
            chmap = ch_map14;
    #endif*/
  } else {
    chmap = ch_map12;
  }
  uint16_t max = 1 << bits;
  uint16_t pct_100 = (uint32_t)max * 100 / 150;
  for (uint8_t i = 0; i < 7; i++) {
    uint8_t idx = chmap[upper * 7 + i];
    int32_t value;
    if ((chmap[upper*7 + i] == 0xff) || ((num_channels > 7) && (chmap[upper*7 + i] > num_channels - 1))) {
      value = 0xffff;
    } else {
      //if (binding && Model.limits[idx].flags & CH_FAILSAFE_EN) {
      //    value = (int32_t)Model.limits[idx].failsafe * (pct_100 / 2) / 100 + (max / 2);
      //} else {
      value = (int32_t)channelOutputs[idx] * (pct_100 / 2) / CHAN_MAX_VALUE + (max / 2);
      //}
      if (value >= max)
        value = max - 1;
      else if (value < 0)
        value = 0;
      value = (upper && i == 0 ? 0x8000 : 0) | (chmap[upper * 7 + i] << bits) | value;
    }
    packet[i*2 + 2] = (value >> 8) & 0xff;
    packet[i*2 + 3] = (value >> 0) & 0xff;
  }
}

static const uint8_t init_vals[][2] = {
  //{CYRF_1D_MODE_OVERRIDE, 0x01},  //moved to CYRF_Reset()
  {CYRF_03_TX_CFG, 0x38 | TXPOWER_1},     //Data Code Length = 64 chip codes + Data Mode = SDR Mode + (todo,was)max-power(+4 dBm)
  {CYRF_06_RX_CFG, 0x4A},         //LNA + FAST TURN EN + RXOW EN, enable low noise amplifier, fast turning, overwrite enable
  {CYRF_12_DATA64_THOLD, 0x0a},   //TH64 = 0Ah, set pn correlation threshold (0Eh???)
  {CYRF_1B_TX_OFFSET_LSB, 0x55},  //STRIM LSB = 0x55, typical configuration
  {CYRF_1C_TX_OFFSET_MSB, 0x05},  //STRIM MSB = 0x05, typical configuration
  {CYRF_32_AUTO_CAL_TIME, 0x3c},  //AUTO_CAL_TIME = 3Ch, typical configuration
  {CYRF_35_AUTOCAL_OFFSET, 0x14}, //AUTO_CAL_OFFSET = 14h, typical configuration
  //{CYRF_39_ANALOG_CTRL, 0x01},    //ALL SLOW
  {CYRF_01_TX_LENGTH, 0x10},      //TX Length = 16 byte packet
  {CYRF_28_CLK_EN, 0x02},         //RXF, force receive clock enable ???
  {CYRF_0F_XACT_CFG, 0x28},       // Pascallanger - Not in Deviation

};

static void cyrf_startup_config()
{
  CYRF_Reset();
  for(uint8_t i = 0; i < sizeof(init_vals) / 2; i++)
    CYRF_WriteRegister(init_vals[i][0], init_vals[i][1]);
  //If using 64-SDR, set number of preamble repetitions to four for optimum performance
  CYRF_WritePreamble(0x333304);
}

static const uint8_t bind_vals[][2] = {
  {CYRF_03_TX_CFG, 0x38 | TXPOWER_1},  //Data Code Length = 64 chip codes + Data Mode = SDR Mode + max-power(+4 dBm)
  {CYRF_10_FRAMING_CFG, 0x4a}, //SOP LEN + SOP TH = 0Ah (0Eh???)
  {CYRF_1E_RX_OVERRIDE, 0x14}, //FRC RXDR + DIS RXCRC (disable rx checksum)
  {CYRF_1F_TX_OVERRIDE, 0x04}, //DIS TXCRC (disable tx checksum)
  {CYRF_14_EOP_CTRL, 0x02},    //EOP = 2 (set EOP sync = 2)
};

static void cyrf_bind_config()
{
  for(uint8_t i = 0; i < sizeof(bind_vals) / 2; i++)
    CYRF_WriteRegister(bind_vals[i][0], bind_vals[i][1]);
}

void initialize_bind_state()
{
  cyrf_bind_config();
  CYRF_WriteRegister(CYRF_00_CHANNEL, BIND_CHANNEL); //This seems to be random?
  //In 64-SDR mode, only the first eight bytes are used, but all sixteen bytes must be writed.
  CYRF_Send_pncodes_ConfigDataCode(0,8); //first eight bytes
  CYRF_Send_pncodes_ConfigDataCode(0,8); //last eight bytes, unused, so can be any
  build_bind_packet();
}

static const uint8_t transfer_vals[][2] = {
  //{CYRF_29_RX_ABORT, 0x20},    //RX abort anable                   (RX mode abort in time Rx bind responce)
  //{CYRF_0F_XACT_CFG, 0x28},    //Force end dsm_state = Synth Mode (TX) (RX mode abort in time Rx bind responce)
  //{CYRF_29_RX_ABORT, 0x00},    //Clear RX abort                    (RX mode abort in time Rx bind responce)
  {CYRF_03_TX_CFG, 0x28 | TXPOWER_1},  //Data Code Length = 64 chip codes + Data Mode = 8DR Mode + (todo,was)max-power(+4 dBm)
  {CYRF_10_FRAMING_CFG, 0xea}, //SOP EN + SOP LEN = 64 chips + LEN EN + SOP TH = 0Ah (0Eh???)
  {CYRF_1E_RX_OVERRIDE, 0x00}, //Reset RX overrides
  {CYRF_1F_TX_OVERRIDE, 0x00}, //Reset TX overrides
};

static void cyrf_transfer_config()
{
  for(uint8_t i = 0; i < sizeof(transfer_vals) / 2; i++)
    CYRF_WriteRegister(transfer_vals[i][0], transfer_vals[i][1]);
}

static uint8_t get_pn_row(uint8_t channel)
{
  return Protos[g_model.rfProtocol].Protocol == PROTOCOL_DSMX ? (channel - 2) % 5 : channel % 5;
}

static void set_sop_data_crc()
{
  uint8_t pn_row = get_pn_row(CHANNELS[OFFSETINPACKETFORCHANNEL+chidx]);
  //printf("Ch: %d Row: %d SOP: %d Data: %d\n", ch[chidx], pn_row, sop_col, data_col);
  CYRF_WriteRegister(CYRF_00_CHANNEL, CHANNELS[OFFSETINPACKETFORCHANNEL+chidx]);
  CYRF_ConfigCRCSeed(crcidx ? ~checksum : checksum);
  CYRF_Send_pncodes_ConfigSOPCode(pn_row,sop_col);
  //In 64-8DR mode, all sixteen bytes are used
  CYRF_Send_pncodes_ConfigDataCode(pn_row,data_col); //first eight bytes
  /*if((pn_row == 3) && (data_col == 7) && (Model.proto_opts[PROTOOPTS_ORANGERX] == ORANGERX_YES)) {
      CYRF_Send_pncodes_ConfigDataCode((const uint8_t *)"\x88\xE1\xD6\x31\x26\x5F\xBD\x40", 8); //last eight bytes
  } else {*/
  CYRF_Send_pncodes_ConfigDataCode(pn_row,data_col + 1); //last eight bytes
  //}
  /* setup for next iteration */
  if(Protos[g_model.rfProtocol].Protocol == PROTOCOL_DSMX)
    chidx = (chidx + 1) % 23;
  else
    chidx = (chidx + 1) % 2;
  crcidx = !crcidx;
}

static void calc_dsmx_channel()
{
  uint8_t idx = 0;
  uint32_t id = ~(((uint32_t)cyrfmfg_id[0] << 24) | ((uint32_t)cyrfmfg_id[1] << 16) | ((uint32_t)cyrfmfg_id[2] << 8) | (cyrfmfg_id[3] << 0));
  uint32_t id_tmp = id;
  while(idx < 23) {
    uint8_t i;
    uint8_t count_3_27 = 0, count_28_51 = 0, count_52_76 = 0;
    id_tmp = id_tmp * 0x0019660D + 0x3C6EF35F; // Randomization
    uint8_t next_ch = ((id_tmp >> 8) % 0x49) + 3;  // Use least-significant byte and must be larger than 3
    if (((next_ch ^ id) & 0x01) == 0)
      continue;
    for (i = 0; i < idx; i++) {
      if(CHANNELS[OFFSETINPACKETFORCHANNEL+i] == next_ch)
        break;
      if(CHANNELS[OFFSETINPACKETFORCHANNEL+i] <= 27)
        count_3_27++;
      else if (CHANNELS[OFFSETINPACKETFORCHANNEL+i] <= 51)
        count_28_51++;
      else
        count_52_76++;
    }
    if (i != idx)
      continue;
    if ((next_ch < 28 && count_3_27 < 8)
        ||(next_ch >= 28 && next_ch < 52 && count_28_51 < 7)
        ||(next_ch >= 52 && count_52_76 < 8)) {
      CHANNELS[OFFSETINPACKETFORCHANNEL+idx++] = next_ch;
    }
  }
}

/*static uint8_t bcd_to_int(uint8_t data)
{
  uint8_t value = 0, multi = 1;
  while (data) {
    value += (data & 15U) * multi;
    multi *= 10;
    data >>= 4;
  }
  return value;
}*/

/*static uint32_t pkt32_to_coord(uint8_t *ptr)
{
  // (decimal, format DD MM.MMMM)
  return bcd_to_int(ptr[3]) * 3600000
         + bcd_to_int(((uint32_t)ptr[2] << 16) | ((uint32_t)ptr[1] << 8) | ptr[0]) * 6;
}*/

/*NOINLINE static void parse_telemetry_packet()
{
    static uint8_t altitude; // byte from first GPS packet
#if HAS_EXTENDED_TELEMETRY
    static const uint8_t update0a[] = { TELEM_DSM_PBOX_VOLT1, TELEM_DSM_PBOX_VOLT2,
                                   TELEM_DSM_PBOX_CAPACITY1, TELEM_DSM_PBOX_CAPACITY2,
                                   TELEM_DSM_PBOX_ALARMV1, TELEM_DSM_PBOX_ALARMV2,
                                   TELEM_DSM_PBOX_ALARMC1, TELEM_DSM_PBOX_ALARMC2, 0};
    static const uint8_t update15[] = { TELEM_DSM_JETCAT_STATUS, TELEM_DSM_JETCAT_THROTTLE,
                                   TELEM_DSM_JETCAT_PACKVOLT, TELEM_DSM_JETCAT_PUMPVOLT,
                                   TELEM_DSM_JETCAT_RPM, TELEM_DSM_JETCAT_TEMPEGT,
                                   TELEM_DSM_JETCAT_OFFCOND, 0};
    static const uint8_t update20[] = { TELEM_DSM_ESC_RPM, TELEM_DSM_ESC_VOLT1,
                                   TELEM_DSM_ESC_TEMP1, TELEM_DSM_ESC_AMPS1,
                                   TELEM_DSM_ESC_TEMP2, TELEM_DSM_ESC_AMPS2,
                                   TELEM_DSM_ESC_VOLT2, TELEM_DSM_ESC_THROTTLE,
                                   TELEM_DSM_ESC_OUTPUT, 0};
    static const uint8_t update18[] = { TELEM_DSM_RXPCAP_AMPS, TELEM_DSM_RXPCAP_CAPACITY, TELEM_DSM_RXPCAP_VOLT, 0};
    static const uint8_t update34[] = { TELEM_DSM_FPCAP_AMPS, TELEM_DSM_FPCAP_CAPACITY, TELEM_DSM_FPCAP_TEMP, 0};
#endif
    static const uint8_t update16[] = { TELEM_GPS_ALT, TELEM_GPS_LAT, TELEM_GPS_LONG, TELEM_GPS_HEADING, 0};
    static const uint8_t update17[] = { TELEM_GPS_SPEED, TELEM_GPS_TIME, TELEM_GPS_SATCOUNT, 0};
    static const uint8_t update7f[] = { TELEM_DSM_FLOG_FADESA, TELEM_DSM_FLOG_FADESB,
                                   TELEM_DSM_FLOG_FADESL, TELEM_DSM_FLOG_FADESR,
                                   TELEM_DSM_FLOG_FRAMELOSS, TELEM_DSM_FLOG_HOLDS,
                                   TELEM_DSM_FLOG_VOLT1, 0};
    static const uint8_t update7e[] = { TELEM_DSM_FLOG_RPM1, TELEM_DSM_FLOG_VOLT2, TELEM_DSM_FLOG_TEMP1, 0};
    static const uint8_t update03[] = { TELEM_DSM_AMPS1, 0};
    static const uint8_t update11[] = { TELEM_DSM_AIRSPEED, 0};
    static const uint8_t update12[] = { TELEM_DSM_ALTITUDE, TELEM_DSM_ALTITUDE_MAX, 0};
    static const uint8_t update14[] = { TELEM_DSM_GFORCE_X, TELEM_DSM_GFORCE_Y, TELEM_DSM_GFORCE_Z,
                                   TELEM_DSM_GFORCE_XMAX, TELEM_DSM_GFORCE_YMAX, TELEM_DSM_GFORCE_ZMAX,
                                   TELEM_DSM_GFORCE_ZMIN, 0};
    static const uint8_t update40[] = { TELEM_DSM_VARIO_ALTITUDE, TELEM_DSM_VARIO_CLIMBRATE1,
                                   TELEM_DSM_VARIO_CLIMBRATE2, TELEM_DSM_VARIO_CLIMBRATE3,
                                   TELEM_DSM_VARIO_CLIMBRATE4, TELEM_DSM_VARIO_CLIMBRATE5,
                                   TELEM_DSM_VARIO_CLIMBRATE6, 0};
    const uint8_t *update = &update7f[7];
    u32 idx = 0;

#define data_type  packet[0]
#define end_byte   packet[15]
#define LSB_1st    ((data_type >= 0x15 && data_type <= 0x18) || (data_type == 0x34))

    static uint16_t pktTelem[8];

    // Convert 8bit packet into 16bit equivalent
    if (LSB_1st) {
        for(u32 i = 1; i < 8; ++i) {
            pktTelem[i] = (packet[i*2+1] << 8) | packet[i*2];
        }
    } else {
        for(u32 i = 1; i < 8; ++i) {
            pktTelem[i] = (packet[i*2] << 8) | packet[i*2+1];
        }
    }
    switch(data_type) {
        case 0x7f: //TM1000 Flight log
        case 0xff: //TM1100 Flight log
#ifndef MODULAR
            //=================================================================
            // Filter out "Telemetry Range Warning" signals
            //=================================================================
            // if 'Holds'>=255 -> Telemetry Range Warning
            // if 'A' or 'B' or 'L' or 'R' not equal to its previous value
            // and >=511 (usually =65535) -> Telemetry Range Warning
            // therefore A, B, L, R, F, H -> not valid (must be filtered).
            // Fades A is an 8-bit value, B, R, and L are 16-bit values.
            //=================================================================
            if (Model.proto_opts[PROTOOPTS_FLOGFILTER] == FLOGFILTER_ON) {
                if(pktTelem[6] > 15) { //holds
                    break;
                } else if(pktTelem[6] > (uint16_t)Telemetry.value[TELEM_DSM_FLOG_HOLDS]) {
                    update = update7f; //refresh "Flight Log" in case "Hold" dsm_state
                    break;
                }
                //if(pktTelem[1] > 255) //fadesA - unknown if it's right for third party Rx, so will use generic condition
                if((pktTelem[1] != 0xFFFF) && (pktTelem[1] > (uint16_t)Telemetry.value[TELEM_DSM_FLOG_FADESA] + 510)) //fadesA
                    break;
                if((pktTelem[2] != 0xFFFF) && (pktTelem[2] > (uint16_t)Telemetry.value[TELEM_DSM_FLOG_FADESB] + 510)) //fadesB
                    break;
                if((pktTelem[3] != 0xFFFF) && (pktTelem[3] > (uint16_t)Telemetry.value[TELEM_DSM_FLOG_FADESL] + 510)) //fadesL
                    break;
                if((pktTelem[4] != 0xFFFF) && (pktTelem[4] > (uint16_t)Telemetry.value[TELEM_DSM_FLOG_FADESR] + 510)) //fadesR
                    break;
                if(pktTelem[5] > (uint16_t)Telemetry.value[TELEM_DSM_FLOG_FRAMELOSS] + 510) //frLoss
                    break;
            }
#endif
            update = update7f;
            break;
        case 0x03: //High Current sensor
            update = update03;
            break;
        case 0x11: //AirSpeed sensor
            update = update11;
            break;
        case 0x12: //Altimeter sensor
            update = update12;
            break;
        case 0x14: //G-Force sensor
            update = update14;
            break;
#if HAS_EXTENDED_TELEMETRY
        case 0x18: //RX Pack Cap sensor (SPMA9604)
            update = update18;
            break;
        case 0x34: //Flight Pack Cap sensor (SPMA9605)
            update = update34;
            break;
#endif
        case 0x40: //Variometer sensor (SPMA9589)
            update = update40;
            break;
    }
    if (*update) {
        while (*update) {
            Telemetry.value[*update] = pktTelem[++idx];
            if (pktTelem[idx] != 0xffff)
                TELEMETRY_SetUpdated(*update);
            update++;
        }
        return;
    }
    switch(data_type) {
#if HAS_EXTENDED_TELEMETRY
        case 0x0a: //Powerbox sensor
            update = update0a;
            Telemetry.value[TELEM_DSM_PBOX_VOLT1] = pktTelem[1]; //In 1/100 of Volts
            Telemetry.value[TELEM_DSM_PBOX_VOLT2] = pktTelem[2]; //In 1/100 of Volts
            Telemetry.value[TELEM_DSM_PBOX_CAPACITY1] = pktTelem[3]; //In mAh
            Telemetry.value[TELEM_DSM_PBOX_CAPACITY2] = pktTelem[4]; //In mAh
            Telemetry.value[TELEM_DSM_PBOX_ALARMV1] = end_byte & 0x01; //0 = disable, 1 = enable
            Telemetry.value[TELEM_DSM_PBOX_ALARMV2] = end_byte & 0x02; //0 = disable, 1 = enable
            Telemetry.value[TELEM_DSM_PBOX_ALARMC1] = end_byte & 0x04; //0 = disable, 1 = enable
            Telemetry.value[TELEM_DSM_PBOX_ALARMC2] = end_byte & 0x08; //0 = disable, 1 = enable
            break;
        case 0x15: //JetCat sensor
            update = update15;
            Telemetry.value[TELEM_DSM_JETCAT_STATUS]   = packet[2];
            Telemetry.value[TELEM_DSM_JETCAT_THROTTLE] = bcd_to_int(packet[3]);   //up to 159% (the upper nibble is 0-f, the lower nibble 0-9)
            Telemetry.value[TELEM_DSM_JETCAT_PACKVOLT] = bcd_to_int(pktTelem[2]); //In 1/100 of Volts
            Telemetry.value[TELEM_DSM_JETCAT_PUMPVOLT] = bcd_to_int(pktTelem[3]); //In 1/100 of Volts (low voltage)
            Telemetry.value[TELEM_DSM_JETCAT_RPM]      = bcd_to_int(pktTelem[4] & 0x0fff); //RPM up to 999999
            Telemetry.value[TELEM_DSM_JETCAT_TEMPEGT]  = bcd_to_int(pktTelem[6]); //EGT temp up to 999C
            Telemetry.value[TELEM_DSM_JETCAT_OFFCOND]  = end_byte;
            break;
        case 0x20: //Electronic Speed Control
            update = update20;
            Telemetry.value[TELEM_DSM_ESC_RPM]   = pktTelem[1] * 10; //In rpm, 0-655340 (0xFFFF --> No data)
            Telemetry.value[TELEM_DSM_ESC_VOLT1] = pktTelem[2];      //Batt in 1/100 of Volts (Volt2) (0-655.34V) (0xFFFF --> No data)
            Telemetry.value[TELEM_DSM_ESC_TEMP1] = pktTelem[3];      //FET Temp in 1/10 of C degree (0-999.8C) (0xFFFF --> No data)
            Telemetry.value[TELEM_DSM_ESC_AMPS1] = pktTelem[4];      //In 1/100 Amp (0-655.34A) (0xFFFF --> No data)
            Telemetry.value[TELEM_DSM_ESC_TEMP2] = pktTelem[5];      //BEC Temp in 1/10 of C degree (0-999.8C) (0xFFFF --> No data)
            Telemetry.value[TELEM_DSM_ESC_AMPS2] = packet[12];       //BEC current in 1/10 Amp (0-25.4A) (0xFF ----> No data)
            Telemetry.value[TELEM_DSM_ESC_VOLT2] = packet[13] * 5;   //BEC voltage in 0.05V (0-12.70V) (0xFF ----> No data)
            Telemetry.value[TELEM_DSM_ESC_THROTTLE] = packet[14]* 5; //Throttle % in 0.5% (0-127%) (0xFF ----> No data)
            Telemetry.value[TELEM_DSM_ESC_OUTPUT] = end_byte * 5;    //Power Output % in 0.5% (0-127%) (0xFF ----> No data)
            break;
#endif //HAS_EXTENDED_TELEMETRY
        case 0x7e: //TM1000
        case 0xfe: //TM1100
            update = update7e;
            Telemetry.value[TELEM_DSM_FLOG_RPM1]  = (pktTelem[1] == 0xffff || pktTelem[1] < 200) ?  0 : (120000000 / 2 / pktTelem[1]);
            Telemetry.value[TELEM_DSM_FLOG_VOLT2] =  pktTelem[2];
            Telemetry.value[TELEM_DSM_FLOG_TEMP1] = (pktTelem[3] == 0x7fff) ? 0 : (pktTelem[3] - 32) * 5 / 9; //Convert to C
            break;
        case 0x16: //GPS sensor (always second GPS packet)
            update = update16;
            Telemetry.gps.altitude  = (bcd_to_int((altitude << 24) | ((u32)pktTelem[1] << 8)))
                                                                   * ((end_byte & 0x80) ? -1 : 1);
                                                                  	  //In m * 1000 (16Bit decimal, 1 unit is 0.1m)
                                                                  	  //1 = below sea level, 0 = above sea level
            Telemetry.gps.latitude  =  pkt32_to_coord(&packet[4]) * ((end_byte & 0x01) ? 1 : -1); //1 = N(+), 0 = S(-)
            Telemetry.gps.longitude = (pkt32_to_coord(&packet[8]) + ((end_byte & 0x04) ? 360000000 : 0)) //1 = +100 degrees
                                                                  * ((end_byte & 0x02) ? 1 : -1); //1 = E(+), 0 = W(-)
            Telemetry.gps.heading = bcd_to_int(pktTelem[6]); //In degrees (16Bit decimal, 1 unit is 0.1 degree)
            break;
        case 0x17: //GPS sensor (always first GPS packet)
            update = update17;
            Telemetry.gps.velocity = bcd_to_int(pktTelem[1]) * 5556 / 108; //In m/s * 1000
            //uint8_t ssec  = bcd_to_int(packet[4]);
            uint8_t sec   = bcd_to_int(packet[5]);
            uint8_t min   = bcd_to_int(packet[6]);
            uint8_t hour  = bcd_to_int(packet[7]);
            uint8_t day   = 0;
            uint8_t month = 0;
            uint8_t year  = 0; // + 2000
            Telemetry.gps.time = ((year & 0x3F) << 26)
                               | ((month & 0x0F) << 22)
                               | ((day & 0x1F) << 17)
                               | ((hour & 0x1F) << 12)
                               | ((min & 0x3F) << 6)
                               | ((sec & 0x3F) << 0);
            Telemetry.gps.satcount = bcd_to_int(packet[8]);
            altitude = packet[9];
            break;
    }
    idx = 0;
    while (*update) {
        TELEMETRY_SetUpdated(*update);
        update++;
    }
}*/

static uint16_t dsm2_cb()
{
#define CH1_CH2_DELAY 4010*2  // Time between write of channel 1 and channel 2
#define WRITE_DELAY   1550*2  // Time after write to verify write complete
#define READ_DELAY     600*2  // Time before write to check read dsm_state, and switch channels.
  // Telemetry read+processing =~200us and switch channels =~300us

  heartbeat |= HEART_TIMER_PULSES; // Todo better placeto find

  if(dsm_state < DSM2_CHANSEL) {
    //Binding
    dsm_state += 1;
    if(dsm_state & 1) {
      //Send packet on even states
      //Note dsm_state has already incremented, so this is actually 'even' dsm_state
      CYRF_WriteDataPacket(packet);
      return 8500*2;
    } else {
      //Check status on odd states
      CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS);
      return 1500*2;
    }
  } else if(dsm_state < DSM2_CH1_WRITE_A) {
    //Select channels and configure for writing data
    //CYRF_FindBestChannels(ch, 2, 10, 1, 79);
    cyrf_transfer_config();
    CYRF_SetTxRxMode(TX_EN);
    chidx = 0;
    crcidx = 0;
    dsm_state = DSM2_CH1_WRITE_A;
    set_sop_data_crc();
    return 10000U*2;
  } else if(dsm_state == DSM2_CH1_WRITE_A || dsm_state == DSM2_CH1_WRITE_B
            || dsm_state == DSM2_CH2_WRITE_A || dsm_state == DSM2_CH2_WRITE_B) {
    if (dsm_state == DSM2_CH1_WRITE_A || dsm_state == DSM2_CH1_WRITE_B)
      build_data_packet(dsm_state == DSM2_CH1_WRITE_B);
    CYRF_WriteDataPacket(packet);
    dsm_state++;
    return WRITE_DELAY;
  } else if(dsm_state == DSM2_CH1_CHECK_A || dsm_state == DSM2_CH1_CHECK_B) {
    uint32_t i = 0;
    uint8_t reg;
    while (! ((reg = CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS)) & 0x02)) {
      if (++i >= NUM_WAIT_LOOPS)
        break;
    }
    if (0) { /*(Model.proto_opts[PROTOOPTS_TELEMETRY] == TELEM_ON)*/
      // reset cyrf6936 in case TX mode and RX mode freezed
      if (((reg & 0x22) == 0x20) || (CYRF_ReadRegister(CYRF_02_TX_CTRL) & 0x80)) {
        cyrf_startup_config();
        cyrf_transfer_config();
        CYRF_SetTxRxMode(TX_EN);
        //printf(" Rst CYRF\n");
      }
    }
    set_sop_data_crc();
    dsm_state++;
    return CH1_CH2_DELAY - WRITE_DELAY;
  } else if(dsm_state == DSM2_CH2_CHECK_A || dsm_state == DSM2_CH2_CHECK_B) {
    uint32_t i = 0;
    while (! (CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS) & 0x02)) {
      if(++i > NUM_WAIT_LOOPS)
        break;
    }
    if (dsm_state == DSM2_CH2_CHECK_A) {
      //Keep transmit power in sync
      CYRF_WriteRegister(CYRF_03_TX_CFG, 0x28 | TXPOWER_1); //Data Code Length = 64 chip codes + Data Mode = 8DR Mode + tx_power
    }
    if (1) { /*(Model.proto_opts[PROTOOPTS_TELEMETRY] == TELEM_OFF)*/
      set_sop_data_crc();
      if (dsm_state == DSM2_CH2_CHECK_A) {
        if(num_channels < 8) {
          dsm_state = DSM2_CH1_WRITE_A;
          CALCULATE_LAT_JIT(); // Calculate latency and jitter.
          SCHEDULE_MIXER_END_IN_US(22000);
          return 22000U *2 - CH1_CH2_DELAY - WRITE_DELAY;
        }
        dsm_state = DSM2_CH1_WRITE_B;
      } else {
        dsm_state = DSM2_CH1_WRITE_A;
      }
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      SCHEDULE_MIXER_END_IN_US(11000);
      return 11000U*2 - CH1_CH2_DELAY - WRITE_DELAY;
    } else {
      dsm_state++;
      CYRF_SetTxRxMode(RX_EN); //Receive mode
      CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x80); //Prepare to receive
      return 11000*2 - CH1_CH2_DELAY - WRITE_DELAY - READ_DELAY;
    }
  } else if(dsm_state == DSM2_CH2_READ_A || dsm_state == DSM2_CH2_READ_B) {
    //Read telemetry if needed
    uint8_t rx_state = CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);
    if((rx_state & 0x03) == 0x02) {  // RXC=1, RXE=0 then 2nd check is required (debouncing)
      rx_state |= CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);
    }
    if((rx_state & 0x07) == 0x02) { // good data (complete with no errors)
      CYRF_WriteRegister(CYRF_07_RX_IRQ_STATUS, 0x80); // need to set RXOW before data read
      CYRF_ReadDataPacketLen(packet, CYRF_ReadRegister(CYRF_09_RX_COUNT));
#if defined(FRSKY)
      telemetryData.rssi[1].value = CYRF_ReadRegister(CYRF_13_RSSI) & 0x1F; // RSSI of the received telemetry signal
      // TODO parse_telemetry_packet();
#endif
    }
    if (dsm_state == DSM2_CH2_READ_A && num_channels < 8) {
      dsm_state = DSM2_CH2_READ_B;
      //Reseat RX mode just in case any error
      CYRF_WriteRegister(CYRF_0F_XACT_CFG, (CYRF_ReadRegister(CYRF_0F_XACT_CFG) | 0x20));  // Force end dsm_state
      uint8_t i = 0;
      while (CYRF_ReadRegister(CYRF_0F_XACT_CFG) & 0x20) {
        if(++i > NUM_WAIT_LOOPS)
          break;
      }
      CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x80);  //Prepare to receive
      return 11000U*2;
    }
    if (dsm_state == DSM2_CH2_READ_A)
      dsm_state = DSM2_CH1_WRITE_B;
    else
      dsm_state = DSM2_CH1_WRITE_A;
    CYRF_SetTxRxMode(TX_EN); //Write mode
    set_sop_data_crc();
    return READ_DELAY;
  }
  return 0;
}

static void DSM_initialize(uint8_t bind)
{
  PROTO_Stop_Callback();
  cyrf_startup_config();
  CYRF_GetMfgData(cyrfmfg_id);
  cyrfmfg_id[0] ^= g_eeGeneral.fixed_ID.ID_8[0];
  cyrfmfg_id[1] ^= g_eeGeneral.fixed_ID.ID_8[1];
  cyrfmfg_id[2] ^= g_eeGeneral.fixed_ID.ID_8[2];
  cyrfmfg_id[3] ^= g_eeGeneral.fixed_ID.ID_8[3];

  if (Protos[g_model.rfProtocol].Protocol == PROTOCOL_DSMX) {
    calc_dsmx_channel();
  } else {
    if (0) { /*(RANDOM_CHANNELS)*/
      /*uint8_t tmpch[10];
      CYRF_FindBestChannels(tmpch, 10, 5, 3, 75);
      uint8_t idx = rand32() % 10;
      CHANNELS[OFFSETINPACKETFORCHANNEL+0] = tmpch[idx];
      while(1) {
         idx = rand32() % 10;
         if (tmpch[idx] != CHANNELS[OFFSETINPACKETFORCHANNEL+0])
             break;
      }
      CHANNELS[OFFSETINPACKETFORCHANNEL+1] = tmpch[idx];*/
    } else {
      CHANNELS[OFFSETINPACKETFORCHANNEL+0] = (cyrfmfg_id[0] + cyrfmfg_id[2] + cyrfmfg_id[4]
                     + g_eeGeneral.fixed_ID.ID_8[0] + g_eeGeneral.fixed_ID.ID_8[2]) % 39 + 1;
      CHANNELS[OFFSETINPACKETFORCHANNEL+1] = (cyrfmfg_id[1] + cyrfmfg_id[3] + cyrfmfg_id[5]
                     + g_eeGeneral.fixed_ID.ID_8[1] + g_eeGeneral.fixed_ID.ID_8[1]) % 40 + 40;
    }
    //printf("DSM2 Channels: %02x %02x\n", CHANNELS[OFFSETINPACKETFORCHANNEL+0], CHANNELS[OFFSETINPACKETFORCHANNEL+1]);
  }
  checksum = ~((cyrfmfg_id[0] << 8) + cyrfmfg_id[1]);
  crcidx = 0;
  sop_col = (cyrfmfg_id[0] + cyrfmfg_id[1] + cyrfmfg_id[2] + 2) & 0x07;
  data_col = 7 - sop_col;
  num_channels = 12;

  if (bind) {
    dsm_state = DSM2_BIND;
    //PROTOCOL_SetBindState((DSM_BIND_COUNT > 200 ? DSM_BIND_COUNT / 2 : 200) * 10); //msecs
    initialize_bind_state();
    binding = 1;
  } else {
    dsm_state = DSM2_CHANSEL;
    binding = 0;
  }
  CYRF_SetTxRxMode(TX_EN);
  PROTO_Start_Callback( dsm2_cb);
}

const void *DSM_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    DSM_initialize(0);
    return 0;
  //case PROTOCMD_DEINIT:
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    CYRF_Reset();
    CYRF_SetTxRxMode(TXRX_OFF);
    return 0;
  //case PROTOCMD_CHECK_AUTOBIND: return 0; //Never Autobind
  case PROTOCMD_BIND:
    DSM_initialize(1);
    return 0;
  case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_DSM_Ser),
                        STR_DUMMY,      //Sub proto
                        STR_DUMMY,      //Option 1 (int)
                        STR_DUMMY,      //Option 2 (int)
                        STR_DUMMY,      //Option 3 (uint 0 to 31)
                        STR_DUMMY,      //OptionBool 1
                        STR_DUMMY,      //OptionBool 2
                        STR_DUMMY       //OptionBool 3
                       );
    return 0;
  //case PROTOCMD_NUMCHAN: return (void *)12L;
  //case PROTOCMD_DEFAULT_NUMCHAN: return (void *)7L;
  //case PROTOCMD_CURRENT_ID: return Model.fixed_id ? (void *)((unsigned long)Model.fixed_id) : 0;
  //case PROTOCMD_GETOPTIONS: return dsm_opts;
  //case PROTOCMD_TELEMETRYSTATE:
  //   return (void *)(long)(Model.proto_opts[PROTOOPTS_TELEMETRY] == TELEM_ON ? PROTO_TELEM_ON : PROTO_TELEM_OFF);
  // case PROTOCMD_TELEMETRYTYPE:
  //   return (void *)(long) TELEM_DSM;
  default:
    break;
  }
  return 0;
}

