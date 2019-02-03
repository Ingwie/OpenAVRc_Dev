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

#define FLYSKY_BIND_COUNT 2500

const static RfOptionSettingsvarstruct RfOpt_FLYSKY_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/4,
  /*rfOptionValue1Min*/-127,
  /*rfOptionValue1Max*/127,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,
};

const pm_char STR_SUBTYPE_FLYSKY_SPI[] PROGMEM = "STD.""V9X9""V6X6""V912""CX20";

enum Flysky_sub_protocols
{
  Flysky	= 0,
  V9X9	= 1,
  V6X6	= 2,
  V912	= 3,
  CX20	= 4
};

enum
{
  // flags going to byte 10
  FLAG_V9X9_VIDEO = 0x40,
  FLAG_V9X9_CAMERA= 0x80,
  // flags going to byte 12
  FLAG_V9X9_FLIP   = 0x10,
  FLAG_V9X9_LED   = 0x20,
};

enum
{
  // flags going to byte 13
  FLAG_V6X6_HLESS1= 0x80,
  // flags going to byte 14
  FLAG_V6X6_VIDEO = 0x01,
  FLAG_V6X6_YCAL  = 0x02,
  FLAG_V6X6_XCAL  = 0x04,
  FLAG_V6X6_RTH   = 0x08,
  FLAG_V6X6_CAMERA= 0x10,
  FLAG_V6X6_HLESS2= 0x20,
  FLAG_V6X6_LED   = 0x40,
  FLAG_V6X6_FLIP  = 0x80,
};

enum
{
  // flags going to byte 14
  FLAG_V912_TOPBTN= 0x40,
  FLAG_V912_BTMBTN= 0x80,
};

const uint8_t ZZV912_X17_SEQ[] PROGMEM =  { 0x14, 0x31, 0x40, 0x49, 0x49, 0x49, 0x49, 0x49, 0x49, 0x49, };

const uint8_t ZZ_FLYSKY_channels[] PROGMEM =
{
  0x12, 0x34, 0x56, 0x78,
  0x18, 0x27, 0x36, 0x45,
  0x41, 0x82, 0x36, 0x57,
  0x84, 0x13, 0x65, 0x72,
  0x87, 0x64, 0x15, 0x32,
  0x76, 0x84, 0x13, 0x52,
  0x71, 0x62, 0x84, 0x35,
  0x71, 0x86, 0x43, 0x52
};

static void FLYSKY_apply_extension_flags()
{
  switch(g_model.rfSubType)
    {
    case V9X9:
      if(getLogicalSwitch(1))
        packet[12] |= FLAG_V9X9_FLIP;
      if(getLogicalSwitch(2))
        packet[12] |= FLAG_V9X9_LED;
      if(getLogicalSwitch(3))
        packet[10] |= FLAG_V9X9_CAMERA;
      if(getLogicalSwitch(4))
        packet[10] |= FLAG_V9X9_VIDEO;
      break;

    case V6X6:
      packet[13] = 0x03; // 3 = 100% rate (0=40%, 1=60%, 2=80%)
      packet[14] = 0x00;
      if(getLogicalSwitch(1))
        packet[14] |= FLAG_V6X6_FLIP;
      if(getLogicalSwitch(2))
        packet[14] |= FLAG_V6X6_LED;
      if(getLogicalSwitch(3))
        packet[14] |= FLAG_V6X6_CAMERA;
      if(getLogicalSwitch(4))
        packet[14] |= FLAG_V6X6_VIDEO;
      if(getLogicalSwitch(5))
        {
          packet[13] |= FLAG_V6X6_HLESS1;
          packet[14] |= FLAG_V6X6_HLESS2;
        }
      if(getLogicalSwitch(6))
        packet[14] |= FLAG_V6X6_RTH;
      if(getLogicalSwitch(7))
        packet[14] |= FLAG_V6X6_XCAL;
      if(getLogicalSwitch(8))
        packet[14] |= FLAG_V6X6_YCAL;
      packet[15] = 0x10; // unknown
      packet[16] = 0x10; // unknown
      packet[17] = 0xAA; // unknown
      packet[18] = 0xAA; // unknown
      packet[19] = 0x60; // unknown, changes at irregular interval in stock TX
      packet[20] = 0x02; // unknown
      break;

    case V912:
      packet_count++;
      if( packet_count > 9)
        packet_count = 0;
      packet[12] |= 0x20; // bit 6 is always set ?
      packet[13] = 0x00;  // unknown
      packet[14] = 0x00;
      if(getLogicalSwitch(1))
        packet[14]  = FLAG_V912_BTMBTN;
      if(getLogicalSwitch(2))
        packet[14] |= FLAG_V912_TOPBTN;
      packet[15] = 0x27; // [15] and [16] apparently hold an analog channel with a value lower than 1000
      packet[16] = 0x03; // maybe it's there for a pitch channel for a CP copter ?
      packet[17] = pgm_read_byte_far(pgm_get_far_address(ZZV912_X17_SEQ)+packet_count) ; // not sure what [17] & [18] are for
      if(packet_count == 0)                    // V912 Rx does not even read those bytes... [17-20]
        packet[18] = 0x02;
      else
        packet[18] = 0x00;
      packet[19] = 0x00; // unknown
      packet[20] = 0x00; // unknown
      break;

    case CX20:
      packet[19] = 0x00; // unknown
      packet[20] = (channel_index<<4)|0x0A;
      break;
    default:
      break;
    }
}

static void FLYSKY_build_packet(uint8_t init)
{
  //channel order AIL;ELE;THR;RUD;CH5;CH6;CH7;CH8
  packet[0] = init ? 0xaa : 0x55;
  packet[1] = temp_rfid_addr[3];
  packet[2] = temp_rfid_addr[2];
  packet[3] = temp_rfid_addr[1];
  packet[4] = temp_rfid_addr[0];

  for (uint8_t i=0; i<8; ++i)
    {
// Compute value +-1024 to range 1000<-1497->1994 -100%<-0->+00%
      int16_t value = (channelOutputs[i] + PPM_CH_CENTER(i) - PPM_CENTER)/2; //Round to +-512 1024/497 = 2.06
      value = limit((int16_t)-497, value, (int16_t)+497);
      value +=  1497; // (offset).
      if((g_model.rfSubType == CX20) && (i==1)/*Elevator*/)
        {
          value=3000-value;
        }
      packet[5 + i*2]=(value&0xFF);
      packet[6 + i*2]=(value>>8)&0xFF;
    }
  FLYSKY_apply_extension_flags();
}

static uint16_t FLYSKY_cb()
{
  heartbeat |= HEART_TIMER_PULSES;

  if (send_seq == rfState8)
    {
      send_seq = 0;
      A7105_AdjustLOBaseFreq();
      SCHEDULE_MIXER_END_IN_US(12000U); // Schedule next Mixer calculations. Soon as possible
    }

  if(bind_idx || bind_counter)
    {
      if (!send_seq)
        {
          FLYSKY_build_packet(1);
        }
      A7105_WriteData(21, 1);
      --bind_counter;
    }
  else
    {
      if (!send_seq)
        {
          FLYSKY_build_packet(0);
        }
      A7105_WriteData(21, channel_used[channel_index & 0x0F]);
      A7105_ManagePower();
    }

  ++channel_index;
  ++send_seq;

  uint16_t packet_period;
  if(g_model.rfSubType == CX20)
    {
      packet_period = 3984*2;
    }
  else
    {
      packet_period = 1510*2;	//1460 on deviation but not working with the latest V911 bricks... Turnigy 9X v2 is 1533, Flysky TX for 9XR/9XR Pro is 1510, V911 TX is 1490.
    }

  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return packet_period;
}


static void FLYSKY_initialize(uint8_t bind)
{
  uint8_t chanrow;
  uint8_t chanoffset;
  uint8_t temp;

  A7105_Init();

  // limit offset to 9 as higher values don't work with some RX (ie V912)
  // limit offset to 9 as CX20 repeats the same channels after that
  if ((temp_rfid_addr[3]&0xF0) > 0x90)
    {
      temp_rfid_addr[3]=temp_rfid_addr[3]-0x70;
    }
  // Build frequency hop table
  chanrow = temp_rfid_addr[3] & 0x0F;
  chanoffset = temp_rfid_addr[3]/16;

  for(uint8_t i=0; i<16; i++)
    {
      temp = pgm_read_byte_far(pgm_get_far_address(ZZ_FLYSKY_channels) +(chanrow>>1)*4+(i>>2));

      if(i&0x02)
        {
          temp &= 0x0F;
        }
      else
        {
          temp >>= 4;
        }

      temp *= 0x0A;

      if(i&0x01)
        {
          temp += 0x50;
        }

      if(g_model.rfSubType==CX20)
        {
          if(temp==0x0A)
            {
              temp += 0x37;
            }
          if(temp==0xA0)
            {
              if (chanoffset<4)
                {
                  temp = 0x37;
                }
              else if (chanoffset<9)
                {
                  temp = 0x2D;
                }
              else
                {
                  temp = 0x29;
                }
            }
          rfState8 = 3; // 12ms refresh rate value
        }
      else
        {
          rfState8 = 8; // 12ms refresh rate value
        }
      channel_used[((chanrow&1)?15-i:i)]=temp-chanoffset;
    }
  channel_index = 0;
  packet_count = 0;
  send_seq = 0;

  if(bind)
    {
      bind_idx = 0xFF; // Keep in Bind mode memory
    }
  else
    {
      bind_counter = FLYSKY_BIND_COUNT;
      PROTOCOL_SetBindState(250); // 2.5 Sec
    }
  PROTO_Start_Callback(FLYSKY_cb);
}

const void *FLYSKY_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
    {
    case PROTOCMD_INIT:
      FLYSKY_initialize(0);
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      return 0;
    case PROTOCMD_BIND:
      FLYSKY_initialize(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_FLYSKY_Ser),
                          STR_SUBTYPE_FLYSKY_SPI, //Sub proto
                          STR_RFTUNEFINE, //Option 1 (int)
                          STR_DUMMY,      //Option 2 (int)
                          STR_RFPOWER,    //Option 3 (uint 0 to 31)
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
