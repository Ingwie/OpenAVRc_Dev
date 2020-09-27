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

const pm_char STR_SUBTYPE_BAYANG_SPI[] PROGMEM = "BAYA""H8S3""X16A""IRDR""DHD4";

const static RfOptionSettingsvar_t RfOpt_BAYANG_Ser[] PROGMEM =
{
 /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED | BOOL2USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
 /*rfSubTypeMax*/4,
 /*rfOptionValue1Min*/0,
 /*rfOptionValue1Max*/0,
 /*rfOptionValue2Min*/0,
 /*rfOptionValue2Max*/0,
 /*rfOptionValue3Max*/3,    // RF POWER
};

#define BAYANG_BIND_COUNT		1000
#define BAYANG_PACKET_PERIOD	1000
#define BAYANG_PACKET_SIZE		15
#define BAYANG_RF_NUM_CHANNELS	4
#define BAYANG_RF_BIND_CHANNEL	0
#define BAYANG_RF_BIND_CHANNEL_X16_AH 10
#define BAYANG_ADDRESS_LENGTH	5

enum BAYANG_FLAGS
{
// flags going to packet[2]
 BAYANG_FLAG_RTH		  	= 0x01,
 BAYANG_FLAG_HEADLESS	  = 0x02,
 BAYANG_FLAG_FLIP	    	= 0x08,
 BAYANG_FLAG_VIDEO	  	= 0x10,
 BAYANG_FLAG_PICTURE		= 0x20,
// flags going to packet[3]
 BAYANG_FLAG_INVERTED	  = 0x80,			// inverted flight on Floureon H101
 BAYANG_FLAG_TAKE_OFF  	= 0x20,			// take off / landing on X16 AH
 BAYANG_FLAG_EMG_STOP 	= 0x04|0x08,	// 0x08 for VISUO XS809H-W-HD-G
};

enum BAYANG_OPTION_FLAGS
{
 BAYANG_OPTION_FLAG_TELEMETRY	= 0x01,
 BAYANG_OPTION_FLAG_ANALOGAUX	= 0x02,
};

enum BAYANG
{
 BAYANG	 = 0,
 H8S3D	 = 1,
 X16_AH  = 2,
 IRDRONE = 3,
 DHD_D4	 = 4,
};

#define bayang_sub_protocol g_model.rfSubType
#define bayang_autobind g_model.rfOptionBool2
#define bayang_telemetry g_model.rfOptionBool1
#define bayang_rfid_addr telem_save_data // shared

static void BAYANG_init()
{
 NRF24L01_SetTxRxMode(TX_EN);

 XN297_SetTXAddr((uint8_t *)"\x00\x00\x00\x00\x00", BAYANG_ADDRESS_LENGTH);

 NRF24L01_FlushTx();
 NRF24L01_FlushRx();
 NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);     	// Clear data ready, data sent, and retransmit
 NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);      	// No Auto Acknowldgement on all data pipes
 NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x01);  	// Enable data pipe 0 only
 NRF24L01_WriteReg(NRF24L01_11_RX_PW_P0, BAYANG_PACKET_SIZE);
 NRF24L01_SetBitrate(NRF24L01_BR_1M);             	// 1Mbps
 NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x00); 	// No retransmits
 NRF24L01_ManagePower();
 NRF24L01_Activate(0x73);							              // Activate feature register
 NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x00);		  	// Disable dynamic payload length on all pipes
 NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x01);
 NRF24L01_Activate(0x73);

 switch (bayang_sub_protocol)
  {
  case X16_AH:
  case IRDRONE:
   num_channel = BAYANG_RF_BIND_CHANNEL_X16_AH;
   break;
  default:
   num_channel = BAYANG_RF_BIND_CHANNEL;
   break;
  }
}

uint16_t convert_channel_10b(uint8_t num)
{
 int16_t val=FULL_CHANNEL_OUTPUTS(num)+1024;
 val = limit <int16_t>(0, val, 2048);
 return (val >> 1);
}

static void BAYANG_send_packet(uint8_t bind)
{
 uint8_t i;
 if (bind)
  {
   if (bayang_telemetry)
    {
     packet[0]= 0xA3; // telemetry is enabled
    }
   else
    {
     packet[0] = 0xA4;
    }
   for(i = 0 ; i < 5; i++)
    packet[i+1] = bayang_rfid_addr[i];
   for(i = 0; i < 4; i++)
    packet[i+6] = channel_used[i];
   switch (bayang_sub_protocol)
    {
    case X16_AH:
     packet[10] = 0x00;
     packet[11] = 0x00;
     break;
    case IRDRONE:
     packet[10] = 0x30;
     packet[11] = 0x01;
     break;
    case DHD_D4:
     packet[10] = 0xC8;
     packet[11] = 0x99;
     break;
    default:
     packet[10] = bayang_rfid_addr[0];	// txid[0]
     packet[11] = bayang_rfid_addr[1];	// txid[1]
     break;
    }
  }
 else
  {
   uint16_t val;
   uint8_t dyntrim = 1;
   switch (bayang_sub_protocol)
    {
    case X16_AH:
    case IRDRONE:
     packet[0] = 0xA6;
     break;
    default:
     packet[0] = 0xA5;
     break;
    }

   packet[1] = 0xFA;
   //Flags packet[2]
   packet[2] = 0x00;
   if(FULL_CHANNEL_OUTPUTS(4)>0)
    packet[2] = BAYANG_FLAG_FLIP;
   if(FULL_CHANNEL_OUTPUTS(5)>0)
    packet[2] |= BAYANG_FLAG_RTH;
   if(FULL_CHANNEL_OUTPUTS(6)>0)
    packet[2] |= BAYANG_FLAG_PICTURE;
   if(FULL_CHANNEL_OUTPUTS(7)>0)
    packet[2] |= BAYANG_FLAG_VIDEO;
   if(FULL_CHANNEL_OUTPUTS(8)>0)
    {
     packet[2] |= BAYANG_FLAG_HEADLESS;
     dyntrim = 0;
    }
   //Flags packet[3]
   packet[3] = 0x00;
   if(FULL_CHANNEL_OUTPUTS(9)>0)
    packet[3] = BAYANG_FLAG_INVERTED;
   if(FULL_CHANNEL_OUTPUTS(10)>0)
    dyntrim = 0;
   if(FULL_CHANNEL_OUTPUTS(11)>0)
    packet[3] |= BAYANG_FLAG_TAKE_OFF;
   if(FULL_CHANNEL_OUTPUTS(12)>0)
    packet[3] |= BAYANG_FLAG_EMG_STOP;
   //Aileron
   val = convert_channel_10b(3);
   packet[4] = (val>>8) + (dyntrim ? ((val>>2) & 0xFC) : 0x7C);
   packet[5] = val & 0xFF;
   //Elevator
   val = convert_channel_10b(1);
   packet[6] = (val>>8) + (dyntrim ? ((val>>2) & 0xFC) : 0x7C);
   packet[7] = val & 0xFF;
   //Throttle
   val = convert_channel_10b(2);
   packet[8] = (val>>8) + 0x7C;
   packet[9] = val & 0xFF;
   //Rudder
   val = convert_channel_10b(0);
   packet[10] = (val>>8) + (dyntrim ? ((val>>2) & 0xFC) : 0x7C);
   packet[11] = val & 0xFF;
  }
 switch (bayang_sub_protocol)
  {
  case H8S3D:
   packet[12] = bayang_rfid_addr[2];	// txid[2]
   packet[13] = 0x34;
   break;
  case X16_AH:
   packet[12] = 0;
   packet[13] = 0;
   break;
  case IRDRONE:
   packet[12] = 0xE0;
   packet[13] = 0x2E;
   break;
  case DHD_D4:
   packet[12] = 0x37;	//0x17 during bind
   packet[13] = 0xED;
   break;
  default:
   packet[12] = bayang_rfid_addr[2];	// txid[2]
   packet[13] = 0x0A;
   break;
  }
 packet[14] = 0;
 for (uint8_t i = 0; i < BAYANG_PACKET_SIZE-1; i++)
  packet[14] += packet[i];

 NRF24L01_WriteReg(NRF24L01_05_RF_CH, bind ? num_channel:channel_used[channel_index++]);
 channel_index %= BAYANG_RF_NUM_CHANNELS;

// clear packet status bits and TX FIFO
 NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);
 NRF24L01_FlushTx();

 XN297_WritePayload(packet, BAYANG_PACKET_SIZE);

 NRF24L01_SetTxRxMode(TXRX_OFF);
 NRF24L01_SetTxRxMode(TX_EN);

// Power on, TX mode, 2byte CRC
// Why CRC0? xn297 does not interpret it - either 16-bit CRC or nothing
 XN297_Configure(_BV(NRF24L01_00_EN_CRC) | _BV(NRF24L01_00_CRCO) | _BV(NRF24L01_00_PWR_UP));

 if (bayang_telemetry)
  {
   // switch radio to rx as soon as packet is sent
   while (!(NRF24L01_ReadReg(NRF24L01_07_STATUS) & _BV(NRF24L01_07_TX_DS)));
   NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x03);
  }

 NRF24L01_ManagePower();
}

static void BAYANG_check_rx(void)
{
 if (NRF24L01_ReadReg(NRF24L01_07_STATUS) & _BV(NRF24L01_07_RX_DR))
  {
   // data received from model
   XN297_ReadPayload(packet, BAYANG_PACKET_SIZE);
   NRF24L01_WriteReg(NRF24L01_07_STATUS, 255);

   NRF24L01_FlushRx();
   uint8_t check = packet[0];
   for (uint8_t i = 1; i < BAYANG_PACKET_SIZE-1; i++)
    check += packet[i];
   // decode data , check sum is ok as well, since there is no crc
   if (packet[0] == 0x85 && packet[14] == check)
    {

     frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
     telemetryData.analog[TELEM_ANA_A1].set((packet[3]<<7) + (packet[4]>>2), g_model.telemetry.channels[TELEM_ANA_A1].type);
     telemetryData.analog[TELEM_ANA_A2].set((packet[5]<<7) + (packet[6]>>2), g_model.telemetry.channels[TELEM_ANA_A2].type);
     telemetryData.rssi[0].set(packet[7]);

     receive_seq++;
    }
  }
}

static void BAYANG_initialize_channels()
{
 channel_used[0] = 0;
 channel_used[1] = (temp_rfid_addr[0]&0x1F)+0x10;
 channel_used[2] = channel_used[1]+0x20;
 channel_used[3] = channel_used[2]+0x20;
 channel_index = 0;
}

static uint16_t BAYANG_callback()
{

 SCHEDULE_MIXER_END_IN_US(BAYANG_PACKET_PERIOD);
 if (!bind_counter) //if(IS_BIND_DONE)
  {
   if(!packet_count)
    BAYANG_send_packet(0);
   packet_count++;
   if (bayang_telemetry)
    {
     rfState16++;
     if (rfState16 > 500)
      {
       telemetryData.rssi[1].set(receive_seq);
       rfState16 = 0;
       receive_seq = 0;
      }
     if (packet_count > 1)
      BAYANG_check_rx();

     packet_count %= 5;
    }
   else
    packet_count %= 2;
  }
 else
  {
   if (!bind_counter)
    {
     XN297_SetTXAddr(bayang_rfid_addr, BAYANG_ADDRESS_LENGTH);
     XN297_SetRXAddr(bayang_rfid_addr, BAYANG_ADDRESS_LENGTH);
    }
   else
    {
     if(!packet_count)
      BAYANG_send_packet(1);
     packet_count++;
     packet_count %= 4;
     bind_counter--;
    }
  }
 heartbeat |= HEART_TIMER_PULSES;
 CALCULATE_LAT_JIT(); // Calculate latency and jitter.
 return BAYANG_PACKET_PERIOD *2;
}

static void BAYANG_initialize(uint8_t bind)
{
 loadrfidaddr_rxnum(0);
 bayang_rfid_addr[0] = temp_rfid_addr[0];
 bayang_rfid_addr[1] = temp_rfid_addr[1];
 bayang_rfid_addr[2] = temp_rfid_addr[2];
 bayang_rfid_addr[3] = temp_rfid_addr[3];
 bayang_rfid_addr[4] = temp_rfid_addr[0];

 channel_index = 0;
 receive_seq = 0;

 BAYANG_initialize_channels();
 BAYANG_init();
 if (bind || bayang_autobind)
  {
   bind_counter = BAYANG_BIND_COUNT * 4;
  }
 PROTO_Start_Callback(BAYANG_callback);
}

const void *BAYANG_Cmds(enum ProtoCmds cmd)
{
 switch(cmd)
  {
  case PROTOCMD_INIT:

   BAYANG_initialize(0);
   return 0;
  case PROTOCMD_RESET:
   PROTO_Stop_Callback();
   NRF24L01_Reset();
   return 0;
  case PROTOCMD_BIND:
   BAYANG_initialize(1);
   return 0;
  case PROTOCMD_GETOPTIONS:
   SetRfOptionSettings(pgm_get_far_address(RfOpt_BAYANG_Ser),
                       STR_SUBTYPE_BAYANG_SPI,      //Sub proto
                       STR_DUMMY,      //Option 1 (int)
                       STR_DUMMY,      //Option 2 (int)
                       STR_RFPOWER,    //Option 3 (uint 0 to 31)
                       STR_TELEMETRY,      //OptionBool 1
                       STR_AUTOBIND,      //OptionBool 2
                       STR_DUMMY       //OptionBool 3
                      );
   return 0;
  default:
   break;
  }
 return 0;
}

