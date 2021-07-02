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

const static RfOptionSettingsvar_t RfOpt_AFHDS2A_Ser[] PROGMEM =
{
 /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
 /*rfSubTypeMax*/3,
 /*rfOptionValue1Min*/-127,
 /*rfOptionValue1Max*/127,
 /*rfOptionValue2Min*/0,
 /*rfOptionValue2Max*/70,
 /*rfOptionValue3Max*/7,
};

const pm_char STR_SUBTYPE_AFHDS2A_SPI[] PROGMEM = "IBPW""IBPP""SBPW""SBPP";

#define AFHDS2A_RX_ID          seed
#define AFHDS2A_RX_ID_STORAGE  (g_model.points[NUM_POINTS-5]) // Dirty : Use curves points to store RX ID (4 bytes)
#define AFHDS2A_NUM_WAIT_LOOPS (700 / 15) //each loop is ~15us.  Do not wait more than 700us
#define AFHDS2A_TXPACKET_SIZE	  38
#define AFHDS2A_RXPACKET_SIZE	  37
#define AFHDS2A_NUMFREQ	   		  16
#define AFHDS2A_WAIT_WRITE      0x80

enum AFHDS2A_sub
{
 PWM_IBUS,
 PPM_IBUS,
 PWM_SBUS,
 PPM_SBUS,
};

enum
{
 AFHDS2A_PACKET_STICKS,
 AFHDS2A_PACKET_SETTINGS,
 AFHDS2A_PACKET_FAILSAFE,
};

enum
{
 AFHDS2A_BIND1,
 AFHDS2A_BIND2,
 AFHDS2A_BIND3,
 AFHDS2A_BIND4,
 AFHDS2A_DATA,
};

static void AFHDS2A_calc_channels()
{
 uint8_t idx = 0;
 uint32_t rnd = g_eeGeneral.fixed_ID.ID_32;
 while (idx < AFHDS2A_NUMFREQ)
  {
   uint8_t i;
   uint8_t count_1_42 = 0, count_43_85 = 0, count_86_128 = 0, count_129_168 = 0;
   rnd = rnd * 0x0019660D + 0x3C6EF35F; // Randomization

   uint8_t next_ch = ((rnd >> (idx%32)) % 0xa8) + 1;
   // Keep the distance 2 between the channels - either odd or even
   if (((next_ch ^ temp_rfid_addr_p2M[0]) & 0x01 )== 0)
    continue;
   // Check that it's not duplicate and spread uniformly
   for (i = 0; i < idx; i++)
    {
     if(channel_used_p2M[i] == next_ch)
      break;
     if(channel_used_p2M[i] <= 42)
      count_1_42++;
     else if (channel_used_p2M[i] <= 85)
      count_43_85++;
     else if (channel_used_p2M[i] <= 128)
      count_86_128++;
     else
      count_129_168++;
    }
   if (i != idx)
    continue;
   if ((next_ch <= 42 && count_1_42 < 5)
       ||(next_ch >= 43 && next_ch <= 85 && count_43_85 < 5)
       ||(next_ch >= 86 && next_ch <=128 && count_86_128 < 5)
       ||(next_ch >= 129 && count_129_168 < 5))
    channel_used_p2M[idx++] = next_ch;
  }
}

static void AFHDS2A_build_bind_packet()
{
 uint8_t ch;
 memcpy( &packet_p2M[1], temp_rfid_addr_p2M, 4);
 memset( &packet_p2M[5], 0xff, 4);
 packet_p2M[10]= 0x00;
 for(ch=0; ch<AFHDS2A_NUMFREQ; ch++)
  {
   packet_p2M[11+ch] = channel_used_p2M[ch];
  }
 memset( &packet_p2M[27], 0xff, 10);
 packet_p2M[37] = 0x00;
 switch(rfState8_p2M)
  {
  case AFHDS2A_BIND1:
   packet_p2M[0] = 0xbb;
   packet_p2M[9] = 0x01;
   break;
  case AFHDS2A_BIND2:
  case AFHDS2A_BIND3:
  case AFHDS2A_BIND4:
   packet_p2M[0] = 0xbc;
   if(rfState8_p2M == AFHDS2A_BIND4)
    {
     memcpy(&packet_p2M[5], &AFHDS2A_RX_ID, 4);
     memset(&packet_p2M[11], 0xff, 16);
    }
   packet_p2M[9] = rfState8_p2M-1;
   if(packet_p2M[9] > 0x02)
    packet_p2M[9] = 0x02;
   packet_p2M[27]= 0x01;
   packet_p2M[28]= 0x80;
   break;
  }
}

static void AFHDS2A_build_packet(uint8_t type)
{
 memcpy(&packet_p2M[1], temp_rfid_addr_p2M, 4);
 memcpy(&packet_p2M[5], &AFHDS2A_RX_ID, 4);

 switch(type)
  {
  case AFHDS2A_PACKET_STICKS:
   packet_p2M[0] = 0x58;
   for(uint8_t ch=0; ch<14; ch++)
    {
     int16_t value = (FULL_CHANNEL_OUTPUTS(ch))/2; // +-1280 to +-640
     value += PPM_CENTER; // + 1500 offset
     value = limit((int16_t)-860, value, (int16_t)+2140);
     packet_p2M[9 +  ch*2] = value&0xFF;
     packet_p2M[10 + ch*2] = (value>>8)&0xFF;
    }
   break;
  case AFHDS2A_PACKET_FAILSAFE:
   packet_p2M[0] = 0x56;
   for(uint8_t ch=0; ch<14; ch++)
    {
     // no values
     packet_p2M[9 + ch*2] = 0xff;
     packet_p2M[10+ ch*2] = 0xff;
    }
   break;
  case AFHDS2A_PACKET_SETTINGS:
   packet_p2M[0] = 0xaa;
   packet_p2M[9] = 0xfd;
   packet_p2M[10]= 0xff;
   uint16_t val_hz=5*(g_model.rfOptionValue2 & 0x7f)+50;	// value should be between 0 and 70 which gives a value between 50 and 400Hz
   if(val_hz<50 || val_hz>400)
    val_hz=50;	// default is 50Hz
   packet_p2M[11]= val_hz;
   packet_p2M[12]= val_hz >> 8;
   if(g_model.rfSubType == PPM_IBUS || g_model.rfSubType == PPM_SBUS)
    packet_p2M[13] = 0x01;	// PPM output enabled
   else
    packet_p2M[13] = 0x00;  // PWM
   packet_p2M[14]= 0x00;
   for(uint8_t i=15; i<37; i++)
    packet_p2M[i] = 0xff;
   packet_p2M[18] = 0x05;		// ?
   packet_p2M[19] = 0xdc;		// ?
   packet_p2M[20] = 0x05;		// ?
   if(g_model.rfSubType == PWM_SBUS || g_model.rfSubType == PPM_SBUS)
    packet_p2M[21] = 0xdd;	// SBUS output enabled
   else
    packet_p2M[21] = 0xde;	// IBUS
   break;
  }
 packet_p2M[37] = 0x00;
}


#if defined(FRSKY) // telemetry
// telemetry sensors ID
enum
{
 AFHDS2A_SENSOR_RX_VOLTAGE   = 0x00,
 //AFHDS2A_SENSOR_RX_ERR_RATE  = 0xfe,
 AFHDS2A_SENSOR_RX_RSSI      = 0xfc,
 AFHDS2A_SENSOR_RX_NOISE     = 0xfb,
 AFHDS2A_SENSOR_RX_SNR       = 0xfa,
};

static void AFHDS2A_update_telemetry()
{
 // AA | TXID | rx_id | sensor id | sensor # | value 16 bit big endian | sensor id ......
 // max 7 sensors per packet
 uint8_t linkOk = 0;
 for(uint8_t sensor=0; sensor<7; sensor++)
  {
   // Send FrSky telemetry to TX
   uint8_t index = 9+(4*sensor);
   switch(packet_p2M[index])
    {
    case AFHDS2A_SENSOR_RX_VOLTAGE:
     //v_lipo1 = packet_p2M[index+3]<<8 | packet_p2M[index+2];
     telemetryData.analog[TELEM_ANA_A1].set(packet_p2M[index+2], g_model.telemetry.channels[TELEM_ANA_A1].type);
     linkOk = 1;
     break;
    //case AFHDS2A_SENSOR_RX_ERR_RATE:
    //RX_LQI=packet_p2M[index+2];
    //break;
    case AFHDS2A_SENSOR_RX_RSSI:
     telemetryData.rssi[1].set(-packet_p2M[index+2]);
     linkOk = 1;
     break;
    case 0xff:
     return;
     //default:
     // unknown sensor ID
     break;
    }
  }
 if (linkOk)   frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
 // frskyStreaming gets decremented every 10ms, FRSKY_TIMEOUT_FIRST value is detected to play connection prompt.
}
#endif

static uint16_t AFHDS2A_cb()
{
 heartbeat |= HEART_TIMER_PULSES;

 uint8_t data_rx;

 A7105_AdjustLOBaseFreq();

 switch(rfState8_p2M)
  {
  case AFHDS2A_BIND1:
  case AFHDS2A_BIND2:
  case AFHDS2A_BIND3:
   AFHDS2A_build_bind_packet();
   A7105_WriteData(AFHDS2A_TXPACKET_SIZE, packet_count_p2M%2 ? 0x0d : 0x8c);
   if(!(A7105_ReadReg(A7105_00_MODE) & (1<<5))) // CRCF Ok
    {
     A7105_ReadData(AFHDS2A_RXPACKET_SIZE);
     if(packet_p2M[0] == 0xbc && packet_p2M[9] == 0x01)
      {
       memcpy(&AFHDS2A_RX_ID, &packet_p2M[5], 4);
       memcpy(&AFHDS2A_RX_ID_STORAGE, &AFHDS2A_RX_ID, 4); // Store RX number in eeprom
       eeDirty(EE_MODEL); // save eeprom
       rfState8_p2M = AFHDS2A_BIND4;
       packet_count_p2M++;
       break;
      }
    }
   packet_count_p2M++;
   rfState8_p2M |= AFHDS2A_WAIT_WRITE;
   return 1700*2;
  case AFHDS2A_BIND1|AFHDS2A_WAIT_WRITE:
  case AFHDS2A_BIND2|AFHDS2A_WAIT_WRITE:
  case AFHDS2A_BIND3|AFHDS2A_WAIT_WRITE:
   //Wait for TX completion
   receive_seq_p2M = 0;
   while (!(A7105_ReadReg(A7105_00_MODE) & 0x01)) // wait 700 us max
    if(++receive_seq_p2M > AFHDS2A_NUM_WAIT_LOOPS)
     break;
   A7105_SetTxRxMode(TXRX_OFF);					// Turn LNA off since we are in near range and we want to prevent swamping
   A7105_Strobe(A7105_RX);
   rfState8_p2M &= ~AFHDS2A_WAIT_WRITE;
   rfState8_p2M++;
   if(rfState8_p2M > AFHDS2A_BIND3)
    rfState8_p2M = AFHDS2A_BIND1;
   return 2150*2;
  case AFHDS2A_BIND4:
   AFHDS2A_build_bind_packet();
   A7105_WriteData(AFHDS2A_TXPACKET_SIZE, packet_count_p2M%2 ? 0x0d : 0x8c);
   packet_count_p2M++;
   bind_idx_p2M++;
   if(bind_idx_p2M>=4)
    {
     rfState16_p2M=0;
     send_seq_p2M = AFHDS2A_PACKET_STICKS;
     channel_index_p2M=1;
     rfState8_p2M = AFHDS2A_DATA;
     eeDirty(EE_MODEL); // Save RX ID in eeprom (if it work)
    }
   break;
  case AFHDS2A_DATA:
   if(!(rfState16_p2M % 3))
    {
     SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
    }

   AFHDS2A_build_packet(send_seq_p2M);
   data_rx = A7105_ReadReg(A7105_00_MODE);		// Check if something has been received...

   A7105_WriteData(AFHDS2A_TXPACKET_SIZE, channel_used_p2M[channel_index_p2M++]);
   if(channel_index_p2M >= AFHDS2A_NUMFREQ)
    {
     channel_index_p2M = 0;
    }
   if(!(rfState16_p2M % 1313))
    {
     send_seq_p2M = AFHDS2A_PACKET_SETTINGS;
    }
   else
    {
     send_seq_p2M = AFHDS2A_PACKET_STICKS;		// todo : check for settings changes
    }
   if(!(A7105_ReadReg(A7105_00_MODE) & (1<<5 ) && !(data_rx & 1))) // RX+CRCF Ok
    {
     A7105_ReadData(AFHDS2A_RXPACKET_SIZE);
     if(packet_p2M[0] == 0xaa)
      {
       if(packet_p2M[9] == 0xfc)
        {
         send_seq_p2M = AFHDS2A_PACKET_SETTINGS;	// RX is asking for settings
        }
#if defined(FRSKY) // telemetry
       else
        {
         // Read TX RSSI
         int16_t temp=256-(A7105_ReadReg(A7105_1D_RSSI_THOLD)*8)/5;		// value from A7105 is between 8 for maximum signal strength to 160 or less
         limit<int16_t>(0, temp, 255);
         telemetryData.rssi[0].set(temp);
         if (g_model.rfOptionBool1) // telemetry on?
          {
           AFHDS2A_update_telemetry();
          }
        }
#endif
      }
    }
   ++rfState16_p2M;
   rfState8_p2M |= AFHDS2A_WAIT_WRITE;
   CALCULATE_LAT_JIT(); // Calculate latency and jitter.
   return 1700*2;
  case AFHDS2A_DATA|AFHDS2A_WAIT_WRITE:
   //Wait for TX completion
   receive_seq_p2M = 0;
   while (!(A7105_ReadReg(A7105_00_MODE) & 0x01)) // wait 700 us max
    if(++receive_seq_p2M > AFHDS2A_NUM_WAIT_LOOPS)
     break;
   A7105_ManagePower();
   A7105_SetTxRxMode(RX_EN);
   A7105_Strobe(A7105_RX);
   rfState8_p2M &= ~AFHDS2A_WAIT_WRITE;
   CALCULATE_LAT_JIT(); // Calculate latency and jitter.
   return 2150*2;
  }
 return 3850*2;
}


static void AFHDS2A_initialize(uint8_t bind)
{
 A7105_Init();
 loadrfidaddr_rxnum(0);
 AFHDS2A_calc_channels();
 memcpy(&AFHDS2A_RX_ID, &AFHDS2A_RX_ID_STORAGE, 4); // load RX number stored in eeprom
 send_seq_p2M = AFHDS2A_PACKET_STICKS;

 if(bind)
  {
   rfState8_p2M = AFHDS2A_BIND1;
  }
 else
  {
   rfState8_p2M = AFHDS2A_DATA;
  }

 PROTO_Start_Callback(AFHDS2A_cb);
}

const void *AFHDS2A_Cmds(enum ProtoCmds cmd)
{
 switch(cmd)
  {
  case PROTOCMD_INIT:
   AFHDS2A_initialize(0);
   return 0;
  case PROTOCMD_RESET:
   A7105_Reset();
   PROTO_Stop_Callback();
   return 0;
  case PROTOCMD_BIND:
   AFHDS2A_initialize(1);
   return 0;
  case PROTOCMD_GETOPTIONS:
   SetRfOptionSettings(pgm_get_far_address(RfOpt_AFHDS2A_Ser),
                       STR_SUBTYPE_AFHDS2A_SPI,      //Sub proto
                       STR_RFTUNEFINE,      //Option 1 (int)
                       STR_MULTI_SERVOFREQ, //Option 2 (int)
                       STR_RFPOWER,         //Option 3 (uint 0 to 31)
                       STR_TELEMETRY,       //OptionBool 1
                       STR_DUMMY,           //OptionBool 2
                       STR_DUMMY            //OptionBool 3
                      );
   return 0;
  default:
   break;
  }
 return 0;
}

