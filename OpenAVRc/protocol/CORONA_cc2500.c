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

#define CORONA_RF_NUM_CHANNELS	3
#define CORONA_ADDRESS_LENGTH	  4
#define CORONA_BIND_CHANNEL_V1	0xD1
#define CORONA_BIND_CHANNEL_V2	0xB8
#define CORONA_COARSE			      0x00
#define COR_V1                  1
#define proto_is_V1             channel_skip

const static RfOptionSettingsvarstruct RfOpt_corona_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/1,
  /*rfOptionValue1Min*/-128, // FREQFINE MIN
  /*rfOptionValue1Max*/127,  // FREQFINE MAX
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,    // RF POWER
};

const pm_char STR_SUBTYPE_CORONA[] PROGMEM =     "DSSS"" FSS";

const static uint8_t ZZ_coronaInitSequence[] PROGMEM =
{
  /* 00 */ 0x29, 0x2E, 0x06, 0x07, 0xD3, 0x91, 0xFF, 0x04,
  /* 08 */ 0x05, 0x00, CORONA_BIND_CHANNEL_V1, 0x06, 0x00, 0x5C, 0x4E, 0xC4 + CORONA_COARSE,
  /* 10 */ 0x5B, 0xF8, 0x03, 0x23, 0xF8, 0x47, 0x07, 0x30,
  /* 18 */ 0x18, 0x16, 0x6C, 0x43, 0x40, 0x91, 0x87, 0x6B,
  /* 20 */ 0xF8, 0x56, 0x10, 0xA9, 0x0A, 0x00, 0x11, 0x41,
  /* 28 */ 0x00, 0x59, 0x7F, 0x3F, 0x81, 0x35, 0x0B
};

static void corona_init()
{
  // From dumps channels are anything between 0x00 and 0xC5 on V1.
  // But 0x00 and 0xB8 should be avoided on V2 since they are used for bind.
  // Below code make sure channels are between 0x02 and 0xA0, spaced with a minimum of 2 and not ordered (RX only use the 1st channel unless there is an issue).

  uint8_t order = temp_rfid_addr[3]&0x03;
  bind_counter = 0;
  rfState16 = 0;
  channel_index = 0;

  for(uint8_t i=0; i<CORONA_RF_NUM_CHANNELS+1; i++)
    {
      channel_used[i^order]=2+temp_rfid_addr[3-i]%39+(i<<5)+(i<<3);
    }
  // ID looks random but on the 15 V1 dumps they all show the same odd/even rule
  if(temp_rfid_addr[3]&0x01)
    {
      // If [3] is odd then [0] is odd and [2] is even
      temp_rfid_addr[0]|=0x01;
      temp_rfid_addr[2]&=0xFE;
    }
  else
    {
      // If [3] is even then [0] is even and [2] is odd
      temp_rfid_addr[0]&=0xFE;
      temp_rfid_addr[2]|=0x01;
    }
  temp_rfid_addr[1]=0xFE;			// Always FE in the dumps of V1 and V2


  CC2500_Strobe(CC2500_SIDLE);

  uint_farptr_t pdata = pgm_get_far_address(ZZ_coronaInitSequence);

  for (uint8_t i = 0; i<(DIM(ZZ_coronaInitSequence)); ++i)
    {
      uint8_t dat = pgm_read_byte_far(pdata++);
      CC2500_WriteReg(i, dat);
    }

  if(!proto_is_V1)
    {
      rfState16 = 400; // V2 send channel at startup while rfstate
      CC2500_WriteReg(CC2500_0A_CHANNR, CORONA_BIND_CHANNEL_V2);
      CC2500_WriteReg(CC2500_0E_FREQ1, 0x80);
      CC2500_WriteReg(CC2500_0F_FREQ0, 0x00);
      CC2500_WriteReg(CC2500_15_DEVIATN, 0x50);
      CC2500_WriteReg(CC2500_17_MCSM1, 0x00);
      CC2500_WriteReg(CC2500_1B_AGCCTRL2, 0x67);
      CC2500_WriteReg(CC2500_1C_AGCCTRL1, 0xFB);
      CC2500_WriteReg(CC2500_1D_AGCCTRL0, 0xDC);
    }
  else
    {
      rfState16 = 0;
    }

  CC2500_ManageFreq();

  //not sure what they are doing to the PATABLE since basically only the first byte is used and it's only 8 bytes long. So I think they end up filling the PATABLE fully with 0xFF
  //CC2500_WriteRegisterMulti(CC2500_3E_PATABLE,(const uint8_t *)"\x08\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 13);

  CC2500_SetTxRxMode(TX_EN);
  CC2500_SetPower(TXPOWER_1);
}

static uint16_t corona_send_data_packet()
{
  uint16_t packet_period = 1; // unused value

  if(!rfState16) // V1 or V2&identifier sended
    {
      // Build standard packet
      packet[0] = 0x10;		// 17 bytes to follow

      //Channels
      memset(packet+9, 0x00, 4);

      for(uint8_t i=0; i<8; i++)
        {
          // Channel values are packed
          // Compute value +-1280 to range 860<-1500->2140 -125%<-0->+125%
          int16_t value = channelOutputs[i];
          value /= 2; // +-1280 to +-640
          value += PPM_CH_CENTER(i); // Center value
          value = limit((int16_t)-860, value, (int16_t)+2140);

          packet[i+1] = value;
          packet[9 + (i>>1)] |= (i&0x01)?(value>>4)&0xF0:(value>>8)&0x0F;
        }

      //TX ID
      for(uint8_t i=0; i<CORONA_ADDRESS_LENGTH; i++)
        {
          packet[i+13] = temp_rfid_addr[i];
        }

      packet[17] = 0x00;

      // Tune frequency if it has been changed
      CC2500_ManageFreq();
      // Packet period is based on hopping
      switch(channel_index)
        {
        case 0:
          packet_period = 4000;
          SCHEDULE_MIXER_END_IN_US(20000); // Schedule next Mixer calculations.
          break;
        case 1:
          packet_period = 4000;
          break;
        case 2:
          packet_period = 12000;

          if(!proto_is_V1)
            {
              packet[17] = 0x03;
            }
          break;
        }
      // Set channel
      CC2500_WriteReg(CC2500_0A_CHANNR, channel_used[channel_index]);
      channel_index++;
      channel_index%=CORONA_RF_NUM_CHANNELS;
      // Update power
      CC2500_ManagePower();
    }
  else
    {
      // V2 : Send identifier packet for 2.65sec. This is how the RX learns the hopping table after a bind. Why it's not part of the bind like V1 is a mistery...
      if(--rfState16&1) SCHEDULE_MIXER_END_IN_US(13000); // Dec state & send Schedule next Mixer calculations.
      packet[0] = 0x07;		// 8 bytes to follow
      // Send hopping freq
      for(uint8_t i=0; i<CORONA_RF_NUM_CHANNELS; i++)
        {
          packet[i+1]=channel_used[i];
        }
      // Send TX ID
      for(uint8_t i=0; i<CORONA_ADDRESS_LENGTH; i++)
        {
          packet[i+4]=temp_rfid_addr[i];
        }
      packet[8]=0;
      packet_period=6500;
      // Set channel
      CC2500_WriteReg(CC2500_0A_CHANNR, 0x00);
    }
  // Send packet
  CC2500_WriteData(packet, packet[0]+2);
  return packet_period;
}

static uint16_t corona_send_bind_packet()
{
  uint16_t packet_period;

  // Build bind packets
  if(proto_is_V1)
    {
      // V1
      if(bind_counter++&1)
        {
          SCHEDULE_MIXER_END_IN_US(14000); // Schedule next Mixer calculations.
          // Send TX ID
          packet[0]=0x04;		// 5 bytes to follow
          for(uint8_t i=0; i<CORONA_ADDRESS_LENGTH; i++)
            packet[i+1]=temp_rfid_addr[i];
          packet[5]=0xCD;		// Unknown but seems to be always the same value for V1
        }
      else
        {
          // Send hopping freq
          packet[0]=0x03;		// 4 bytes to follow
          for(uint8_t i=0; i<CORONA_RF_NUM_CHANNELS+1; i++)
            packet[i+1]=channel_used[i];
          // Not sure what the last byte (+1) is for now since only the first 3 channels are used...
        }
      packet_period=3500;
    }
  else
    {
      // V2
      SCHEDULE_MIXER_END_IN_US(25000); // Schedule next Mixer calculations.
      packet[0]=0x04;		// 5 bytes to follow
      for(uint8_t i=0; i<CORONA_ADDRESS_LENGTH; i++)
        packet[i+1]=temp_rfid_addr[i];
      packet[5]=0x00;		// Unknown but seems to be always the same value for V2
      packet_period=25000;
    }
  // Send packet
  CC2500_WriteData(packet, packet[0]+2);
  return packet_period;
}

static uint16_t CORONA_bind_cb()
{
  uint16_t time = corona_send_bind_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return time*2;
}

static uint16_t CORONA_cb()
{
  uint16_t time = corona_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return time*2;
}


static void CORONA_initialize(uint8_t bind)
{
  freq_fine_mem = 0;

  loadrfidaddr_rxnum(3);
  CC2500_Reset();

  (g_model.rfSubType==COR_V1)?proto_is_V1=1:proto_is_V1=0;

  corona_init();

  if (bind)
    {
      PROTO_Start_Callback(25000U *2, CORONA_bind_cb);
    }
  else
    {
      PROTO_Start_Callback(25000U *2, CORONA_cb);
    }
}

const void *CORONA_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
    {
    case PROTOCMD_INIT:
      CORONA_initialize(0);
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      CC2500_Reset();
      return 0;
    case PROTOCMD_BIND:
      CORONA_initialize(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_corona_Ser),
                          STR_SUBTYPE_CORONA,      //Sub proto
                          STR_RFTUNEFINE,     //Option 1 (int)
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

