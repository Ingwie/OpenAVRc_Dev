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
#define DEVO_SEND_SEQ_P2M        BYTE_P2M(4)
#define DEVO_PACKET_COUNT_P2M    BYTE_P2M(5)
#define DEVO_NUM_CHAN_P2M        BYTE_P2M(6)
#define DEVO_PREV_NUM_CHAN_P2M   BYTE_P2M(7)

#define DEVO_BIND_COUNTER_16_P2M WORD_P2M(1)
//***********************************************//


const static RfOptionSettingsvar_t RfOpt_DEVO_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED | BOOL2USED,  //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/4, // Num channel must be 4,8 or 12 -> DEVO_check_num_chan()
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

#define DEVO_AUTOBIND (g_model.rfOptionBool1)
#define USE_FIXED_ID  (g_model.rfOptionBool2) // For Ladybird Quad.

static uint8_t *hopping_channel_ptr; // ToDo ... share between protocols.


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
  if (DEVO_PREV_NUM_CHAN_P2M != g_model.rfOptionValue1)
    {
      switch (g_model.rfOptionValue1)
        {
        case 5:
        case 6:
        case 11:
          DEVO_NUM_CHAN_P2M = 8;
          break;
        case 7:
          DEVO_NUM_CHAN_P2M = 4;
          break;
        case 9:
        case 10:
          DEVO_NUM_CHAN_P2M = 12;
          break;
        }
      if (DEVO_PREV_NUM_CHAN_P2M != DEVO_NUM_CHAN_P2M)
        {
          DEVO_PREV_NUM_CHAN_P2M = DEVO_NUM_CHAN_P2M;
          g_model.rfOptionValue1 = DEVO_NUM_CHAN_P2M;
        }
    }
}

static void DEVO_scramble_pkt() // todo remove
{
#ifdef NO_SCRAMBLE
  return;
#else
  for(uint8_t i = 0; i < 15; i++)
    packet_p2M[i + 1] ^= temp_rfid_addr_p2M[i % 4];
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
  CYRF_27_CLK_OVERRIDE, 0x02,       // RXF, force receive clock
  CYRF_28_CLK_EN, 0x02,             // RXF, force receive clock enable
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

static void DEVO_add_pkt_suffix()
{
  uint8_t bind_state;

  if (USE_FIXED_ID) // Makes bind permanent. Need bind link etc to reset.
    {
      if (DEVO_BIND_COUNTER_16_P2M > 0)
        bind_state = 0xC0;
      else
        bind_state = 0x80;
    }
  else // Allows Rx to bind again, but current bind is non-volatile.
    bind_state = 0x00;

  packet_p2M[10] = bind_state | (DEVO_PKTS_PER_CHANNEL - DEVO_PACKET_COUNT_P2M - 1);
  packet_p2M[11] = *(hopping_channel_ptr + 1);
  packet_p2M[12] = *(hopping_channel_ptr + 2);
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
  packet_p2M[3] = *hopping_channel_ptr;
  packet_p2M[4] = *(hopping_channel_ptr + 1);
  packet_p2M[5] = *(hopping_channel_ptr + 2);
  packet_p2M[6] = temp_rfid_addr_p2M[0];
  packet_p2M[7] = temp_rfid_addr_p2M[1];
  packet_p2M[8] = temp_rfid_addr_p2M[2];
  packet_p2M[9] = temp_rfid_addr_p2M[3];
  DEVO_add_pkt_suffix();
  // The fixed-id portion is scrambled in the bind packet_p2M
  // I assume it is ignored
  packet_p2M[13] ^= temp_rfid_addr_p2M[0];
  packet_p2M[14] ^= temp_rfid_addr_p2M[1];
  packet_p2M[15] ^= temp_rfid_addr_p2M[2];
}

static void DEVO_build_data_pkt()
{
#if defined(X_ANY)
  //Xany_scheduleTx_AllInstance(); TODO DEVO_CH_IDX_P2M check
#endif

  packet_p2M[0] = (DEVO_NUM_CHAN_P2M << 4) | (0x0b + DEVO_CH_IDX_P2M);
  uint8_t sign = 0x0b;
  for (uint8_t i = 0; i < 4; i++)
    {
      int16_t value = FULL_CHANNEL_OUTPUTS(i + (4* DEVO_CH_IDX_P2M));
      value = value + (value >> 1) + (value >> 4); // Range +/- 1600.

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

static void DEVO_set_bound_sop_code()
{
  // crc == 0 isn't allowed, so use 1 if the math results in 0.
  uint8_t crc = (temp_rfid_addr_p2M[0] + (temp_rfid_addr_p2M[1] >> 6) + temp_rfid_addr_p2M[2]);
  if(! crc)
    crc = 1;
  uint8_t sopidx = (0xff &((temp_rfid_addr_p2M[0] << 2) + temp_rfid_addr_p2M[1] + temp_rfid_addr_p2M[2])) % 10;

  CYRF_ConfigCRCSeed((crc << 8) + crc);
  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(sopidx);
  CYRF_WriteRegister(CYRF_03_TX_CFG, 0x08);
}

static void DEVO_BuildPacket()
{
  static uint8_t failsafe_pkt=0;  // TODO What this ?? USED ???????????????????????????? 8-{D

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
      DEVO_build_beacon_pkt(DEVO_NUM_CHAN_P2M > 8 ? failsafe_pkt : 0);
      failsafe_pkt = failsafe_pkt ? 0 : 1;
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
  if (DEVO_SEND_SEQ_P2M == 0)
    {
      DEVO_SEND_SEQ_P2M = 1;
      DEVO_BuildPacket();
      CYRF_WriteDataPacket(packet_p2M);
      heartbeat |= HEART_TIMER_PULSES;
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      return 1200*2;
    }
  DEVO_SEND_SEQ_P2M = 0;
  uint8_t i = 0;
  while (! (CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS) & 0x02))
    if(++i > DEVO_NUM_WAIT_LOOPS)
      return 1200*2;
  if (DEVO_RFSTATE == DEVO_BOUND)
    {
      /* exit binding state */
      DEVO_RFSTATE = DEVO_BOUND_3;
      DEVO_set_bound_sop_code();
    }
  if(DEVO_PACKET_COUNT_P2M == 0)
    {
      CYRF_ManagePower();	// Keep transmit power in sync
      DEVO_check_num_chan(); // Check num channels
      hopping_channel_ptr = hopping_channel_ptr == &channel_used_p2M[2] ? &channel_used_p2M[0] : hopping_channel_ptr + 1;
      CYRF_ConfigRFChannel(*hopping_channel_ptr);
    }
  return 1200*2;
}

void DEVOInit(uint8_t bind)
{
  DEVO_CH_IDX_P2M = 0;
  DEVO_SEND_SEQ_P2M = 0;
  DEVO_PACKET_COUNT_P2M = 0;

  if(g_model.rfOptionValue1 < 5)
    DEVO_NUM_CHAN_P2M = 4;
  else if (g_model.rfOptionValue1 < 9)
    DEVO_NUM_CHAN_P2M = 8;
  else DEVO_NUM_CHAN_P2M = 12;

  DEVO_PREV_NUM_CHAN_P2M = DEVO_NUM_CHAN_P2M;

  // Load temp_rfid_addr_p2M + Model match.
  loadrfidaddr_rxnum(0);

  CYRF_Reset();
  DDEVO_init();

  CYRF_ConfigCRCSeed(0x0000);
  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(0);
  DEVO_set_radio_channels();

  hopping_channel_ptr = &channel_used_p2M[0];
  CYRF_ConfigRFChannel(*hopping_channel_ptr);

  CYRF_SetTxRxMode(TX_EN);

  if (bind || DEVO_AUTOBIND)
    {
      DEVO_BIND_COUNTER_16_P2M = DEVO_BIND_COUNT;
      DEVO_RFSTATE = DEVO_BIND;
      if (DEVO_AUTOBIND)
        {
          PROTOCOL_SetBindState(1200); // 12 Sec
        }
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
                          STR_AUTOBIND,      //OptionBool 1
                          DEVO_STR_STATICID,      //OptionBool 2
                          STR_DUMMY       //OptionBool 3
                         );
      return 0;
    default:
      break;
    }
  return 0;
}
