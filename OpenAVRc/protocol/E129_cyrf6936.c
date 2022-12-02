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
 *             Multiprotocol TX Module -                                  *
 *   https://github.com/pascallanger/DIY-Multiprotocol-TX-Module          *
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
 * Models: Eachine E129 / E130, Hobbyking Firefox C129 and Twister Ninja 250.
 *
 *
 * Channel Order :
 * Ch1 A
 * Ch2 E
 * Ch3 T
 * Ch4 R
 * Ch5 Trim A
 * Ch6 Trim E
 * Ch7 Trim R
 * Logical Switch L3 Take Off / Land
 * Logical Switch L4 Emergency Stop
 *
 * Trims can be done to some extent on the AETR channels directly
 * but if you push them too far you won't be able to arm like explained below.
 * In this case use the associated trim TrimA/E/R instead.
 *
 * Take off with a none spring throttle is easier by putting both sticks down outwards
 * (like on the original radio) in Mode 1/2, not sure about other modes.
 *
 * Calibration is the same as the original radio with both sticks down and to the left in Mode 1/2,
 * not sure about other modes.
 *
 */

#include "../OpenAVRc.h"

// define pulses2MHz reusable values (13 bytes max)
#define E129_RF_STATE_P2M          BYTE_P2M(1)

#define E129_BIND_COUNTER_16_P2M   WORD_P2M(1)
//***********************************************//


#include "iface_rf2500.h"

#define E129_FORCE_ID

#define E129_BIND_CH 0x2D // 45
#define E129_PAYLOAD_SIZE 16

const static RfOptionSettingsvar_t RfOpt_E129_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/9,// Number of channels
  /*rfOptionValue1Max*/9,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,// RF Power max
};

uint8_t bit_reverse(uint8_t a)
{
  a = ((a & 0xf0) >> 4) | ((a & 0x0f) << 4);
  a = ((a & 0xcc) >> 2) | ((a & 0x33) << 2);
  a = ((a & 0xaa) >> 1) | ((a & 0x55) << 1);
  return a;
}

uint16_t convert_channel_10b(uint8_t num)
{
  int16_t value = FULL_CHANNEL_OUTPUTS(num)+ 0x3ff; // Add offset before division.
  value /=2;
  value = limit( (int16_t) 0, value, (int16_t) 0x3FF );
  return (unsigned) value;
}

uint8_t convert_channel_6b(uint8_t num)
{
  int8_t value = FULL_CHANNEL_OUTPUTS(num)/32;
  value += 0x1F;
  value = limit( (int8_t) 0, value, (int8_t) 0x3F);
  return (unsigned) value;
}

static void E129_build_bind_packet()
{
  // Build the bind packet
  memset(packet_p2M, 0, E129_PAYLOAD_SIZE);
  packet_p2M[0] = 0x0F; // Packet length
  packet_p2M[1] = 0xA4;
  packet_p2M[2] = bit_reverse(temp_rfid_addr_p2M[2]);
  packet_p2M[3] = bit_reverse(temp_rfid_addr_p2M[3]);
  packet_p2M[4] = bit_reverse(temp_rfid_addr_p2M[0]);
  packet_p2M[5] = bit_reverse(temp_rfid_addr_p2M[1]);
  for (uint8_t i = 0; i < 4; i++)
    packet_p2M[6 + i] = channel_used_p2M[i] - 2;

  packet_p2M[14] = 0x00; // Check
  for (uint8_t i = 0; i < 14; i++)
    packet_p2M[14] += packet_p2M[i];
}

enum
{
  E129_AILERON = 0,
  E129_ELEVATOR,
  E129_THROTTLE,
  E129_RUDDER,
  E129_TRIMA,
  E129_TRIME,
  E129_TRIMR,
  E129_FLAG1,
  E129_FLAG2,
};

void E129_build_data_packet()
{
  uint8_t pre_fab_pkt[E129_PAYLOAD_SIZE];

  pre_fab_pkt[0] = 0x0F; // Packet length
  pre_fab_pkt[1] = 0xA6; // Packet type
  pre_fab_pkt[2] = 0xF7; // High rate 0xF7, low rate 0xF4
  pre_fab_pkt[3] = 0x00; // Mode: short press=0x20->0x00->0x20->..., long press=0x10->0x30->0x10->...

  pre_fab_pkt[4] = 0;
  if(getLogicalSwitch(1)) pre_fab_pkt[4] |= 0x20;// Take off/Land 0x20.
  if(getLogicalSwitch(2)) pre_fab_pkt[4] |= 0x04;// Emergency stop 0x04.

  uint16_t val;
  uint8_t trim;

  val = convert_channel_10b(E129_AILERON);
  trim = convert_channel_6b(E129_TRIMA); // 6 bit right aligned.
  pre_fab_pkt[5] = (trim <<2 ) | (val >>8);// Trim (0x00..0x1F..0x3E) << 2 | channel >> 8
  pre_fab_pkt[6] = val;// channel (0x000...0x200...0x3FF)

  val = convert_channel_10b(E129_ELEVATOR);
  trim = convert_channel_6b(E129_TRIME);
  pre_fab_pkt[7] = (trim <<2) | (val >>8);
  pre_fab_pkt[8] = val;

  val = convert_channel_10b(E129_THROTTLE);
  pre_fab_pkt[9] = (0x1F <<2) | (val >>8);// No Trim.
  pre_fab_pkt[10] = val;

  val = convert_channel_10b(E129_RUDDER);
  trim = convert_channel_6b(E129_TRIMR);
  pre_fab_pkt[11] = (trim <<2) | (val >>8);
  pre_fab_pkt[12] = val;

  pre_fab_pkt[13] = 0x00; // ?

  pre_fab_pkt[14] = 0x00; // LRC

  for (uint8_t i = 0; i < 14; i++)
    pre_fab_pkt[14] += pre_fab_pkt[i];

  pre_fab_pkt[15] = 0x00; // CRC ?

  memcpy(packet_p2M, pre_fab_pkt, E129_PAYLOAD_SIZE);
  RF2500_BuildPayload(packet_p2M);
}

enum
{
  E129_SEND_0 = 0,
  E129_SEND_ODD = 1,
  E129_SEND_2 = 2,
  E129_SEND_4 = 4,
  E129_SEND_6 = 6,
  E129_SEND_7 = 7,
};

uint16_t E129_data_cb(void)
{
  if ((E129_RF_STATE_P2M&1) == E129_SEND_ODD)
  { // Odd numbered states.
    RF2500_SendPayload();// ~625us
    if (E129_RF_STATE_P2M == E129_SEND_7) E129_RF_STATE_P2M = E129_SEND_0;
    else E129_RF_STATE_P2M ++;
    return 1260 *2;
  }
  else
  {
    RF2500_RFChannel(channel_used_p2M[E129_RF_STATE_P2M >> 1]);

    if (E129_RF_STATE_P2M == E129_SEND_0)
    {
      SCHEDULE_MIXER_END_IN_US((5200) *4);
      E129_build_data_packet(); // Build new packet.
    }
    else if (E129_RF_STATE_P2M == E129_SEND_4)
    {
      CYRF_ManagePower(); // Keep transmit power in sync.
    }
  }

  RF2500_SendPayload();
  E129_RF_STATE_P2M ++;
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return (5200-1260) *2;
}

uint16_t E129_bind_cb(void)
{
  if ((E129_RF_STATE_P2M&1) == E129_SEND_ODD)
  { // Odd numbered states.
    RF2500_SendPayload();// ~625us
    if (E129_RF_STATE_P2M == E129_SEND_7) E129_RF_STATE_P2M = E129_SEND_0;
    else E129_RF_STATE_P2M ++;
    return 1260 *2;
  }
  else
  {
    RF2500_RFChannel(E129_BIND_CH);

    if (E129_RF_STATE_P2M == E129_SEND_0)
    SCHEDULE_MIXER_END_IN_US((5200) *4);

    if (E129_RF_STATE_P2M == E129_SEND_6)
    {
      if(E129_BIND_COUNTER_16_P2M-- == 0)
      {
        PROTOCOL_SetBindState(0); // BIND_DONE;
        RF2500_SetTXAddr(temp_rfid_addr_p2M);// 4 bytes of address
        PROTO_Change_Callback(E129_data_cb);// Not coming back here again.
      }
    }

    RF2500_SendPayload();
    E129_RF_STATE_P2M ++;
    heartbeat |= HEART_TIMER_PULSES;
    return (5200-1260) *2;
  }
}

void E129_init()
{
  CYRF_Reset();
  // Load temp_rfid_addr_p2M.
  loadrfidaddr();

  // RF2500 emu init
  RF2500_Init(E129_PAYLOAD_SIZE, true);	// 16 bytes, Scrambled.

#ifdef E129_FORCE_ID
  temp_rfid_addr_p2M[0] = 0xC1;
  temp_rfid_addr_p2M[1] = 0x22;
  temp_rfid_addr_p2M[2] = 0x05;
  temp_rfid_addr_p2M[3] = 0xA3;
  channel_used_p2M[0] = 0x3C; // 60
  channel_used_p2M[1] = 0x49; // 73
  channel_used_p2M[2] = 0x4B; // 75
  channel_used_p2M[3] = 0x41; // 65
#else
  // ... Find clear channels and select four. calc_fh_channels(4);
  CYRF_FindBestChannels(channel_used_p2M, 4, 2, 60, 75);
#endif

// Freq hopping
  for (uint8_t i = 0; i < 4; i++) // Avoid bind channel #45 0x2D
    if (channel_used_p2M[i] == E129_BIND_CH) channel_used_p2M[i]++;

  E129_build_bind_packet();
  RF2500_SetTXAddr((uint8_t*) "\xE2\x32\xE0\xC8");  // 4 bytes of bind address
  RF2500_BuildPayload(packet_p2M);

  RF2500_SetPower();
  PROTOCOL_SetBindState(200); // 2 Sec
  E129_BIND_COUNTER_16_P2M= 96; // ~2 sec

  E129_RF_STATE_P2M= E129_SEND_0;
  PROTO_Start_Callback(E129_bind_cb);
}

const void* E129_Cmds(enum ProtoCmds cmd)
{
  switch (cmd)
  {
  case PROTOCMD_INIT:
    E129_init();
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    CYRF_Reset();
    return 0;
  case PROTOCMD_BIND:
    E129_init();
    return 0;
  case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_E129_Ser), STR_DUMMY, //Sub proto
        STR_DUMMY,      //Option 1 (int)
        STR_DUMMY,      //Option 2 (int)
        STR_RFPOWER,    //Rf power
        STR_DUMMY,      //OptionBool 1
        STR_DUMMY,      //OptionBool 2
        STR_DUMMY       //OptionBool 3
        );
    return 0;
  default:
    break;
  }
  return 0;
}

