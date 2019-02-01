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

#include "../OpenAVRc.h"


const static RfOptionSettingsvarstruct RfOpt_DDEVO_Ser[] PROGMEM =
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
//uint8_t num_channel; // defined in my misc.h uncomment to try Rich ;-)
static uint8_t *channel_used_ptr; // Todo share


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
{
  if (prev_num_channel != g_model.rfOptionValue1)
    {
      switch (g_model.rfOptionValue1)
        {
        case 5:
        case 6:
        case 11:
          //default:
          num_channel = 8;
          break;
        case 7:
          num_channel = 4;
          break;
        case 9:
        case 10:
          num_channel = 12;
          break;
        }
      if (prev_num_channel != num_channel)
        {
          prev_num_channel = num_channel;
          g_model.rfOptionValue1 = num_channel;
        }
    }
}

static void DEVO_scramble_pkt() // todo remove
{
#ifdef NO_SCRAMBLE
  return;
#else
  for(uint8_t i = 0; i < 15; i++)
    packet[i + 1] ^= temp_rfid_addr[i % 4];
#endif
}

const uint8_t zz_DEVOInitSequence[] PROGMEM =
{
  CYRF_1D_MODE_OVERRIDE, 0x38,      //FRC SEN (forces the synthesizer to start) + FRC AWAKE (force the oscillator to keep running at all times)
  CYRF_03_TX_CFG, 0x08 | TXPOWER_1, //Data Code Length = 32 chip codes + Data Mode = 8DR Mode + max-power(+4 dBm)
  CYRF_06_RX_CFG, 0x4A,             //LNA + FAST TURN EN + RXOW EN, enable low noise amplifier, fast turning, overwrite enable
  CYRF_0B_PWR_CTRL, 0x00,           //Reset power control
  CYRF_10_FRAMING_CFG, 0xA4,        //SOP EN + SOP LEN = 32 chips + LEN EN + SOP TH = 04h
  CYRF_11_DATA32_THOLD, 0x05,       //TH32 = 0x05
  CYRF_12_DATA64_THOLD, 0x0E,       //TH64 = 0Eh, set pn correlation threshold
  CYRF_1B_TX_OFFSET_LSB, 0x55,      //STRIM LSB = 0x55, typical configuration
  CYRF_1C_TX_OFFSET_MSB, 0x05,      //STRIM MSB = 0x05, typical configuration
  CYRF_32_AUTO_CAL_TIME, 0x3C,      //AUTO_CAL_TIME = 3Ch, typical configuration
  CYRF_35_AUTOCAL_OFFSET, 0x14,     //AUTO_CAL_OFFSET = 14h, typical configuration
  CYRF_39_ANALOG_CTRL, 0x01,        //ALL SLOW
  CYRF_1E_RX_OVERRIDE, 0x10,        //FRC RXDR (Force Receive Data Rate)
  CYRF_1F_TX_OVERRIDE, 0x00,        //Reset TX overrides
  CYRF_01_TX_LENGTH, 0x10,          //TX Length = 16 byte packet
  CYRF_27_CLK_OVERRIDE, 0x02,       //RXF, force receive clock
  CYRF_28_CLK_EN, 0x02,             //RXF, force receive clock enable
  CYRF_0F_XACT_CFG, 0x28,           //Force TX mode
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
      if (bind_counter > 0)
        bind_state = 0xC0;
      else
        bind_state = 0x80;
    }
  else // Allows Rx to be rebound, but bind is non-volatile.
    bind_state = 0x00;

  packet[10] = bind_state | (DEVO_PKTS_PER_CHANNEL - packet_count - 1);
  packet[11] = *(channel_used_ptr + 1);
  packet[12] = *(channel_used_ptr + 2);
  packet[13] = temp_rfid_addr[0];
  packet[14] = temp_rfid_addr[1];
  packet[15] = temp_rfid_addr[2]; // use temp_rfid_addr ?
}

static void DEVO_build_beacon_pkt(uint8_t upper)
{
  packet[0] = ((num_channel << 4) | 0x07);
  uint8_t enable = 0;
  uint8_t max = 8;
  if (upper)
    {
      packet[0] += 1;
      max = 4;
    }
  for(uint8_t i = 0; i < max; i++)
    {
      packet[i+1] = 0;
    }
  packet[9] = enable;
  DEVO_add_pkt_suffix();
}

static void DEVO_build_bind_pkt()
{
  packet[0] = (num_channel << 4) | 0x0a;
  packet[1] = bind_counter & 0xff;
  packet[2] = (bind_counter >> 8);
  packet[3] = *channel_used_ptr;
  packet[4] = *(channel_used_ptr + 1);
  packet[5] = *(channel_used_ptr + 2);
  packet[6] = temp_rfid_addr[0];
  packet[7] = temp_rfid_addr[1];
  packet[8] = temp_rfid_addr[2];
  packet[9] = temp_rfid_addr[3];
  DEVO_add_pkt_suffix();
  // The fixed-id portion is scrambled in the bind packet
  // I assume it is ignored
  packet[13] ^= temp_rfid_addr[0];
  packet[14] ^= temp_rfid_addr[1];
  packet[15] ^= temp_rfid_addr[2];
}

static void DEVO_build_data_pkt()
{
#if defined(X_ANY)
  //Xany_scheduleTx_AllInstance(); TODO channel_index check
#endif

  packet[0] = (num_channel << 4) | (0x0b + channel_index);
  uint8_t sign = 0x0b;
  for (uint8_t i = 0; i < 4; i++)
    {

      int16_t value = channelOutputs[i + (4* channel_index)];
      value = value + (value >> 1) + (value >> 4); // Range +/- 1600.

      if(value < 0)
        {
          value = -value;
          sign |= 1 << (7 - i);
        }
      packet[2 * i + 1] = value & 0xff;
      packet[2 * i + 2] = (value >> 8) & 0xff;
    }
  packet[9] = sign;
  channel_index++;
  if (channel_index * 4 >= num_channel)
    channel_index = 0;
  DEVO_add_pkt_suffix();
}

static void DEVO_set_bound_sop_code()
{
  // crc == 0 isn't allowed, so use 1 if the math results in 0.
  uint8_t crc = (temp_rfid_addr[0] + (temp_rfid_addr[1] >> 6) + temp_rfid_addr[2]);
  if(! crc)
    crc = 1;
  uint8_t sopidx = (0xff &((temp_rfid_addr[0] << 2) + temp_rfid_addr[1] + temp_rfid_addr[2])) % 10;
  CYRF_SetTxRxMode(TX_EN);
  CYRF_ConfigCRCSeed((crc << 8) + crc);
  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(sopidx);
  CYRF_WriteRegister(CYRF_03_TX_CFG, 0x08);
}

static void DEVO_BuildPacket()
{
  static uint8_t failsafe_pkt=0;  // TODO What this ?? USED ???????????????????????????? 8-{D

  switch(rfState8)
    {
    case DEVO_BIND:
      if((bind_counter) && (DEVO_AUTOBIND))
        bind_counter--;
      DEVO_build_bind_pkt();
      rfState8 = DEVO_BIND_SENDCH;
      break;
    case DEVO_BIND_SENDCH:
      if((bind_counter) && (DEVO_AUTOBIND))
        bind_counter--;
      DEVO_build_data_pkt();
      DEVO_scramble_pkt();
      if (bind_counter == 0)
        {
          rfState8 = DEVO_BOUND;
          ///BIND_DONE;
        }
      else
        rfState8 = DEVO_BIND;
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
      rfState8++;
      if (bind_counter)
        {
          bind_counter--;
          ///if (bind_counter == 0)
          ///BIND_DONE;
        }
      if (rfState8 == DEVO_BOUND_1)
        {
          SCHEDULE_MIXER_END_IN_US(24000); // Schedule next Mixer calculations.
        }
      break;
    case DEVO_BOUND_10:
      DEVO_build_beacon_pkt(num_channel > 8 ? failsafe_pkt : 0);
      failsafe_pkt = failsafe_pkt ? 0 : 1;
      DEVO_scramble_pkt();
      rfState8 = DEVO_BOUND_1;
      break;
    }
  packet_count++;
  if(packet_count == DEVO_PKTS_PER_CHANNEL)
    packet_count = 0;
}

static void DEVO_set_radio_channels()
{
  CYRF_FindBestChannels(channel_used, 3, 4, 4, 80);
  channel_used[3] = channel_used[0];
  channel_used[4] = channel_used[1];
}

uint16_t DEVO_cb()
{
  if (send_seq == 0)
    {
      send_seq = 1;
      DEVO_BuildPacket();
      CYRF_WriteDataPacket(packet);
      heartbeat |= HEART_TIMER_PULSES;
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      return 1200*2;
    }
  send_seq = 0;
  uint8_t i = 0;
  while (! (CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS) & 0x02))
    if(++i > DEVO_NUM_WAIT_LOOPS)
      return 1200*2;
  if (rfState8 == DEVO_BOUND)
    {
      /* exit binding state */
      rfState8 = DEVO_BOUND_3;
      DEVO_set_bound_sop_code();
    }
  if(packet_count == 0)
    {
      CYRF_WriteRegister(CYRF_03_TX_CFG, 0x08);
      CYRF_ManagePower();	// Keep transmit power in sync
      DEVO_check_num_chan(); // Check num channels
      channel_used_ptr = channel_used_ptr == &channel_used[2] ? channel_used : channel_used_ptr + 1;
      CYRF_ConfigRFChannel(*channel_used_ptr);
    }
  return 1200*2;
}

void DEVOInit(uint8_t bind)
{
  channel_index = 0;
  send_seq = 0;
  packet_count = 0;
  prev_num_channel = 8;
  num_channel = 8;
  // Check num channel
  DEVO_check_num_chan();
  // Load temp_rfid_addr + Model match
  loadrfidaddr_rxnum(0);

  CYRF_Reset();
  DDEVO_init();
  //CYRF_GetMfgData(temp_rfid_addr); prefer use temp_rfid_addr
  CYRF_SetTxRxMode(TX_EN);
  CYRF_ConfigCRCSeed(0x0000);
  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(0);
  DEVO_set_radio_channels();

  channel_used_ptr = channel_used;
  CYRF_ConfigRFChannel(*channel_used_ptr);

  if (bind || DEVO_AUTOBIND)
    {
      bind_counter = DEVO_BIND_COUNT;
      rfState8 = DEVO_BIND;
      if (DEVO_AUTOBIND)
        {
          PROTOCOL_SetBindState(1200); // 12 Sec
        }
      ///BIND_IN_PROGRESS;
    }
  else
    {
      rfState8 = DEVO_BOUND_1;
      bind_counter = 0;
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
      CYRF_Reset();
      PROTO_Stop_Callback();
      return 0;
    case PROTOCMD_BIND:
      DEVOInit(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_DDEVO_Ser),
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
