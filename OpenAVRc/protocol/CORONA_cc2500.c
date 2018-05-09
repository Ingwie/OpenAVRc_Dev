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
#define COR_V1                  0

const static int8_t RfOpt_corona_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/1,
  /*rfOptionValue1Min*/-128,
  /*rfOptionValue1Max*/127,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/0,
};

const pm_char STR_SUBTYPE_CORONA[] PROGMEM =     " V1 "" V2 ";

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
  for (uint8_t i = 0; i<4; ++i)
    {
      t_rf_id_addr[i] = g_eeGeneral.fixed_ID.ID_8[i]; /* Use packet[139 to 143 to store a copy of FixedID  */
    }

  // From dumps channels are anything between 0x00 and 0xC5 on V1.
  // But 0x00 and 0xB8 should be avoided on V2 since they are used for bind.
  // Below code make sure channels are between 0x02 and 0xA0, spaced with a minimum of 2 and not ordered (RX only use the 1st channel unless there is an issue).
  uint8_t order=t_rf_id_addr[3]&0x03;
  for(uint8_t i=0; i<CORONA_RF_NUM_CHANNELS+1; i++)
    {
      channel_used[i^order]=2+t_rf_id_addr[3-i]%39+(i<<5)+(i<<3);
    }
  // ID looks random but on the 15 V1 dumps they all show the same odd/even rule
  if(t_rf_id_addr[3]&0x01)
    {
      // If [3] is odd then [0] is odd and [2] is even
      t_rf_id_addr[0]|=0x01;
      t_rf_id_addr[2]&=0xFE;
    }
  else
    {
      // If [3] is even then [0] is even and [2] is odd
      t_rf_id_addr[0]&=0xFE;
      t_rf_id_addr[2]|=0x01;
    }
  t_rf_id_addr[1]=0xFE;			// Always FE in the dumps of V1 and V2


  CC2500_Strobe(CC2500_SIDLE);

  uint_farptr_t pdata = pgm_get_far_address(ZZ_coronaInitSequence);

  for (uint8_t i = 0; i<(DIM(ZZ_coronaInitSequence)); ++i)
    {
      uint8_t dat = pgm_read_byte_far(pdata++);
      CC2500_WriteReg(i, dat);
    }

  if(g_model.rfSubType!=COR_V1)
    {
      rfState = 400; // V2 send channel at startup while rfstate
      CC2500_WriteReg(CC2500_0A_CHANNR, CORONA_BIND_CHANNEL_V2);
      CC2500_WriteReg(CC2500_0E_FREQ1, 0x80);
      CC2500_WriteReg(CC2500_0F_FREQ0, 0x00 + CORONA_COARSE);
      CC2500_WriteReg(CC2500_15_DEVIATN, 0x50);
      CC2500_WriteReg(CC2500_17_MCSM1, 0x00);
      CC2500_WriteReg(CC2500_1B_AGCCTRL2, 0x67);
      CC2500_WriteReg(CC2500_1C_AGCCTRL1, 0xFB);
      CC2500_WriteReg(CC2500_1D_AGCCTRL0, 0xDC);
    }
  else
    {
      rfState = 0;
    }

  CC2500_WriteReg(CC2500_0C_FSCTRL0, FREQFINE);

  //not sure what they are doing to the PATABLE since basically only the first byte is used and it's only 8 bytes long. So I think they end up filling the PATABLE fully with 0xFF
  CC2500_WriteRegisterMulti(CC2500_3E_PATABLE,(const uint8_t *)"\x08\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 13);

  CC2500_SetTxRxMode(TX_EN);
  CC2500_SetPower(TXPOWER_1);
}

static void corona_send_data_packet()
{
  if(rfState==0 || g_model.rfSubType==COR_V1)
    {
      // Build standard packet
      packet[0] = 0x10;		// 17 bytes to follow

      //Channels
      memset(packet+9, 0x00, 4);

      for(uint8_t i=0; i<8; i++)
        {
          // Channel values are packed
          packet[i+1] = channelOutputs[i];
          packet[9 + (i>>1)] |= (i&0x01)?(channelOutputs[i]>>4)&0xF0:(channelOutputs[i]>>8)&0x0F;
        }

      //TX ID
      for(uint8_t i=0; i<CORONA_ADDRESS_LENGTH; i++)
        packet[i+13] = t_rf_id_addr[i];

      packet[17] = 0x00;

      // Tune frequency if it has been changed
      //if ( prev_option != option )
      {
        CC2500_WriteReg(CC2500_0C_FSCTRL0, FREQFINE); //TODO buil a global cc2500 frqfine function
        //  prev_option = option ;
      }
      // Packet period is based on hopping
      switch(channel_index)
        {
        case 0:
          packet_period=g_model.rfSubType==COR_V1?4991:4248;
          break;
        case 1:
          packet_period=g_model.rfSubType==COR_V1?4991:4345;
          break;
        case 2:
          packet_period=g_model.rfSubType==COR_V1?12520:13468;
          if(g_model.rfSubType!=COR_V1)
            packet[17] = 0x03;
          break;
        }
      // Set channel
      CC2500_WriteReg(CC2500_0A_CHANNR, channel_used[channel_index]);
      channel_index++;
      channel_index%=CORONA_RF_NUM_CHANNELS;
      // Update power
      CC2500_SetPower(TXPOWER_1);
    }
  else
    {
      // Send identifier packet for 2.65sec. This is how the RX learns the hopping table after a bind. Why it's not part of the bind like V1 is a mistery...
      rfState--;
      packet[0]=0x07;		// 8 bytes to follow
      // Send hopping freq
      for(uint8_t i=0; i<CORONA_RF_NUM_CHANNELS; i++)
        packet[i+1]=channel_used[i];
      // Send TX ID
      for(uint8_t i=0; i<CORONA_ADDRESS_LENGTH; i++)
        packet[i+4]=t_rf_id_addr[i];
      packet[8]=0;
      packet_period=6647;
      // Set channel
      CC2500_WriteReg(CC2500_0A_CHANNR, 0x00);
    }
  // Send packet
  CC2500_WriteData(packet, packet[0]+2);
}

static void corona_send_bind_packet()
{
  {
    // Build bind packets
    if(g_model.rfSubType==COR_V1)
      {
        // V1
        if(bind_counter&1)
          {
            // Send TX ID
            packet[0]=0x04;		// 5 bytes to follow
            for(uint8_t i=0; i<CORONA_ADDRESS_LENGTH; i++)
              packet[i+1]=t_rf_id_addr[i];
            packet[5]=0xCD;		// Unknown but seems to be always the same value for V1
            packet_period=3689;
          }
        else
          {
            // Send hopping freq
            packet[0]=0x03;		// 4 bytes to follow
            for(uint8_t i=0; i<CORONA_RF_NUM_CHANNELS+1; i++)
              packet[i+1]=channel_used[i];
            // Not sure what the last byte (+1) is for now since only the first 3 channels are used...
            packet_period=3438;
          }
      }
    else
      {
        // V2
        packet[0]=0x04;		// 5 bytes to follow
        for(uint8_t i=0; i<CORONA_ADDRESS_LENGTH; i++)
          packet[i+1]=t_rf_id_addr[i];
        packet[5]=0x00;		// Unknown but seems to be always the same value for V2
        packet_period=26791;
      }
  }
  // Send packet
  CC2500_WriteData(packet, packet[0]+2);
}

static uint16_t CORONA_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
  corona_send_bind_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

static uint16_t CORONA_cb()
{
  SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
  corona_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 12000U *2;
}


static void CORONA_initialize(uint8_t bind)
{
  PROTO_Stop_Callback();
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
      CC2500_SetTxRxMode(TXRX_OFF);
      CC2500_Strobe(CC2500_SIDLE);
      return 0;
    case PROTOCMD_BIND:
      CORONA_initialize(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_corona_Ser),
                          STR_SUBTYPE_CORONA,      //Sub proto
                          STR_RFTUNE,      //Option 1 (int)
                          STR_DUMMY,      //Option 2 (int)
                          STR_DUMMY,      //Option 3 (uint 0 to 31)
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

