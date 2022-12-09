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

// Multiprotocol inspired. Thanks a lot !


#include "../OpenAVRc.h"

// define pulses2MHz reusable values (13 bytes max)
#define HOTT_BIND_P2M                    BYTE_P2M(1)
#define HOTT_NUM_CHANNEL_P2M             BYTE_P2M(2)
#define HOTT_SEND_SEQ_P2M                BYTE_P2M(3)
#define HOTT_RF_CH_NUM_P2M               BYTE_P2M(4)
#define HOTT_HOP_FREQ_NO_P2M             BYTE_P2M(5)
#define HOTT_MIX_SWAP_P2M                BYTE_P2M(6)

//***********************************************//

#define HOTT_RXNUM_SAVE_AREA             g_model.points[NUM_POINTS-5] // 5 bytes used


const static RfOptionSettingsvar_t RfOpt_HOTT_Ser[] PROGMEM =
{
 /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
 /*rfSubTypeMax*/0,
 /*rfOptionValue1Min*/-128, // FREQFINE MIN
 /*rfOptionValue1Max*/127,  // FREQFINE MAX
 /*rfOptionValue2Min*/0,
 /*rfOptionValue2Max*/0,
 /*rfOptionValue3Max*/7,    // RF POWER
};

#define HOTT_TX_PACKET_LEN	50
#define HOTT_RX_PACKET_LEN	22
#define HOTT_PACKET_PERIOD	10000
#define HOTT_NUM_RF_CHANNELS 75
#define HOTT_COARSE	0

enum
{
 HOTT_START = 0x00,
 HOTT_CAL   = 0x01,
 HOTT_DATA1 = 0x02,
 HOTT_DATA2 = 0x03,
 HOTT_RX1   = 0x04,
 HOTT_RX2   = 0x05,
};

#ifdef HOTT_FW_TELEMETRY
 #define HOTT_SENSOR_TYPE 6
 #define HOTT_SENSOR_SEARCH_PERIOD 2000
 uint8_t HOTT_sensor_cur=0;
 uint8_t HOTT_sensor_pages=0;
 uint8_t HOTT_sensor_valid=false;
 uint8_t HOTT_sensor_ok[HOTT_SENSOR_TYPE];
 uint8_t HOTT_sensor_seq=0;
#endif

#define HOTT_FREQ0_VAL 0x6E

// Some important initialization parameters, all others are either default,
// or not important in the context of transmitter
// FIFOTHR  00
// SYNC1    D3
// SYNC0    91
// PKTLEN   32 - Packet length, 50 bytes
// PKTCTRL1 04 - APPEND_STATUS on=RSSI+LQI, all other are receive parameters - irrelevant
// PKTCTRL0 44 - whitening, use FIFO, use CRC, fixed packet length
// ADDR     00
// CHANNR   10
// FSCTRL1  09 - IF
// FSCTRL0  00 - zero freq offset
// FREQ2    5C - synthesizer frequencyfor 26MHz crystal
// FREQ1    6C
// FREQ0    B9
// MDMCFG4  2D -
// MDMCFG3  3B -
// MDMCFG2  73 - disable DC blocking, MSK, no Manchester code, 32 bits sync word
// MDMCFG1  A3 - FEC enable, 4 preamble bytes, CHANSPC_E - 03
// MDMCFG0  AA - CHANSPC_M - AA
// DEVIATN  47 -
// MCSM2    07 -
// MCSM1    00 - always use CCA, go to IDLE when done
// MCSM0    08 - disable autocalibration, PO_TIMEOUT - 64, no pin radio control, no forcing XTAL to stay in SLEEP
// FOCCFG   1D
const static uint8_t ZZ_HOTTInitSequence[] PROGMEM =
{
 /* 00 */ 0x2F, 0x2E, 0x2F, 0x00, 0xD3, 0x91, 0x32, 0x04,
 /* 08 */ 0x44, 0x00, 0x00, 0x09, 0x00, 0x5C, 0x6C, HOTT_FREQ0_VAL + HOTT_COARSE,
 /* 10 */ 0x2D, 0x3B, 0x73, 0xA3, 0xAA, 0x47, 0x07, 0x00,
 /* 18 */ 0x08, 0x1D, 0x1C, 0xC7, 0x09, 0xF0, 0x87, 0x6B,
 /* 20 */ 0xF0, 0xB6, 0x10, 0xEA, 0x0A, 0x00, 0x11
};

const static uint8_t ZZ_HOTT_hop[][HOTT_NUM_RF_CHANNELS] PROGMEM =
{
 { 48, 37, 16, 62, 9, 50, 42, 22, 68, 0, 55, 35, 21, 74, 1, 56, 31, 20, 70, 11, 45, 32, 24, 71, 8, 54, 38, 26, 61, 13, 53, 30, 15, 65, 7, 52, 34, 28, 60, 3, 47, 39, 18, 69, 2, 49, 44, 23, 72, 5, 51, 43, 19, 64, 12, 46, 33, 17, 67, 6, 58, 36, 29, 73, 14, 57, 41, 25, 63, 4, 59, 40, 27, 66, 10 },
 { 50, 23, 5, 34, 67, 53, 22, 12, 39, 62, 51, 21, 10, 33, 63, 59, 16, 1, 43, 66, 49, 19, 8, 30, 71, 47, 24, 2, 35, 68, 45, 25, 14, 41, 74, 55, 18, 4, 32, 61, 54, 17, 11, 31, 72, 52, 28, 6, 38, 65, 46, 15, 9, 40, 60, 48, 26, 3, 37, 70, 58, 29, 0, 36, 64, 56, 20, 7, 42, 69, 57, 27, 13, 44, 73 },
 { 73, 51, 39, 18, 9, 64, 56, 34, 16, 12, 66, 58, 36, 25, 11, 61, 47, 40, 15, 8, 71, 50, 43, 20, 6, 62, 54, 42, 19, 3, 63, 46, 44, 29, 14, 72, 49, 33, 22, 5, 69, 57, 30, 21, 10, 70, 45, 35, 26, 7, 65, 59, 31, 28, 1, 67, 48, 32, 24, 0, 60, 55, 41, 17, 2, 74, 52, 38, 27, 4, 68, 53, 37, 23, 13 },
 { 52, 60, 40, 21, 14, 50, 72, 41, 23, 13, 59, 61, 39, 16, 6, 58, 66, 33, 17, 5, 55, 64, 43, 20, 12, 54, 74, 35, 29, 3, 46, 63, 37, 22, 10, 48, 65, 31, 27, 9, 49, 73, 38, 24, 11, 56, 70, 32, 15, 1, 51, 71, 44, 18, 8, 45, 67, 36, 25, 7, 57, 62, 34, 28, 2, 53, 69, 42, 19, 4, 47, 68, 30, 26, 0 },
 { 50, 16, 34, 6, 71, 51, 24, 40, 7, 68, 57, 27, 33, 14, 70, 55, 26, 30, 5, 74, 47, 28, 44, 11, 67, 49, 15, 32, 9, 61, 52, 22, 37, 13, 66, 59, 18, 42, 3, 62, 46, 29, 31, 12, 60, 48, 19, 38, 1, 72, 58, 17, 36, 4, 64, 53, 21, 39, 0, 63, 56, 20, 41, 2, 65, 45, 25, 35, 10, 69, 54, 23, 43, 8, 73 },
 { 55, 38, 12, 62, 23, 52, 44, 3, 66, 18, 54, 36, 10, 74, 16, 56, 42, 9, 70, 17, 58, 33, 5, 69, 20, 50, 40, 1, 63, 24, 53, 37, 13, 65, 15, 48, 34, 4, 61, 22, 57, 31, 6, 64, 26, 46, 35, 11, 72, 21, 47, 30, 7, 68, 29, 45, 32, 8, 60, 19, 49, 43, 2, 67, 27, 51, 39, 0, 71, 28, 59, 41, 14, 73, 25 },
 { 70, 32, 18, 10, 58, 69, 38, 22, 2, 54, 67, 36, 19, 12, 57, 62, 34, 20, 14, 52, 63, 41, 15, 3, 51, 73, 42, 28, 6, 48, 60, 43, 29, 5, 45, 64, 31, 17, 4, 56, 65, 35, 26, 13, 53, 61, 37, 23, 1, 49, 68, 40, 16, 9, 47, 71, 39, 25, 7, 50, 66, 33, 24, 8, 59, 72, 44, 27, 11, 46, 74, 30, 21, 0, 55 },
 { 6, 45, 71, 27, 44, 10, 46, 74, 22, 32, 0, 55, 69, 21, 33, 4, 50, 66, 18, 38, 7, 57, 62, 19, 36, 1, 48, 70, 20, 40, 8, 47, 68, 15, 43, 2, 58, 61, 26, 42, 3, 56, 72, 23, 34, 14, 54, 67, 16, 37, 5, 59, 64, 24, 30, 12, 52, 65, 25, 39, 13, 49, 73, 17, 31, 9, 53, 60, 28, 35, 11, 51, 63, 29, 41 },
 { 31, 65, 50, 20, 13, 37, 66, 45, 23, 5, 32, 69, 54, 19, 7, 39, 74, 52, 27, 1, 42, 64, 53, 22, 4, 43, 70, 58, 16, 3, 40, 71, 57, 17, 0, 35, 63, 56, 18, 9, 44, 72, 51, 21, 6, 33, 67, 46, 25, 11, 30, 73, 55, 15, 8, 36, 62, 48, 24, 10, 41, 60, 49, 29, 14, 34, 61, 47, 26, 2, 38, 68, 59, 28, 12 },
 { 67, 22, 49, 36, 13, 64, 28, 57, 37, 6, 65, 29, 46, 39, 3, 70, 26, 45, 35, 1, 62, 24, 58, 34, 10, 68, 19, 53, 33, 4, 66, 21, 52, 31, 7, 74, 18, 47, 32, 5, 61, 16, 51, 38, 8, 72, 23, 55, 30, 12, 73, 17, 59, 44, 0, 60, 15, 50, 43, 14, 63, 27, 48, 42, 11, 71, 20, 54, 41, 9, 69, 25, 56, 40, 2 },
 { 19, 38, 14, 66, 57, 18, 44, 7, 74, 48, 23, 30, 6, 71, 58, 26, 32, 5, 61, 46, 20, 34, 0, 68, 45, 24, 36, 1, 70, 50, 27, 33, 10, 63, 52, 16, 42, 9, 65, 51, 15, 41, 11, 64, 53, 22, 37, 3, 60, 56, 28, 35, 4, 67, 49, 17, 39, 13, 69, 54, 25, 43, 2, 73, 55, 21, 31, 8, 62, 47, 29, 40, 12, 72, 59 },
 { 4, 52, 64, 28, 44, 14, 46, 74, 16, 32, 11, 50, 68, 27, 36, 0, 47, 70, 26, 34, 13, 57, 61, 18, 38, 6, 56, 62, 19, 40, 5, 58, 67, 17, 31, 12, 54, 63, 22, 33, 3, 53, 72, 21, 41, 10, 48, 66, 15, 35, 7, 45, 60, 20, 37, 9, 51, 69, 25, 42, 2, 59, 71, 24, 39, 1, 55, 65, 23, 30, 8, 49, 73, 29, 43 },
 { 44, 66, 19, 1, 56, 35, 62, 20, 4, 54, 39, 70, 24, 5, 55, 31, 74, 26, 12, 58, 32, 60, 17, 10, 45, 37, 63, 22, 3, 50, 33, 64, 16, 7, 51, 34, 61, 21, 8, 48, 38, 68, 29, 0, 46, 36, 72, 28, 14, 49, 42, 69, 25, 6, 57, 43, 65, 18, 2, 52, 30, 71, 23, 13, 47, 41, 67, 15, 9, 53, 40, 73, 27, 11, 59 },
 { 12, 16, 36, 46, 69, 6, 20, 44, 58, 62, 11, 19, 34, 48, 71, 1, 18, 42, 50, 74, 3, 25, 31, 47, 65, 0, 24, 33, 45, 72, 2, 23, 35, 56, 64, 10, 22, 38, 49, 63, 7, 26, 37, 51, 70, 14, 21, 30, 53, 67, 5, 15, 40, 52, 66, 9, 17, 39, 55, 60, 13, 27, 41, 54, 73, 4, 28, 32, 57, 61, 8, 29, 43, 59, 68 },
 { 63, 42, 18, 2, 57, 71, 34, 22, 10, 48, 67, 36, 25, 4, 46, 60, 31, 28, 6, 47, 74, 37, 15, 0, 55, 65, 32, 24, 12, 56, 66, 40, 27, 14, 52, 62, 38, 19, 3, 50, 73, 33, 29, 11, 53, 61, 35, 16, 7, 58, 72, 41, 26, 5, 59, 69, 30, 20, 9, 51, 68, 44, 23, 1, 49, 70, 39, 17, 8, 54, 64, 43, 21, 13, 45 },
 { 52, 1, 71, 17, 36, 47, 7, 64, 26, 32, 53, 5, 60, 20, 42, 57, 2, 66, 18, 34, 56, 4, 63, 24, 35, 46, 13, 72, 22, 30, 48, 0, 67, 21, 39, 50, 3, 74, 16, 31, 59, 14, 61, 23, 37, 45, 6, 65, 19, 44, 51, 11, 62, 27, 41, 55, 9, 68, 15, 38, 58, 8, 70, 29, 40, 54, 10, 69, 28, 33, 49, 12, 73, 25, 43 }
};

const static uint16_t ZZ_HOTT_hop_val[] PROGMEM = { 0xC06B, 0xC34A, 0xDB24, 0x8E09, 0x272E, 0x217F, 0x155B, 0xEDE8, 0x1D31, 0x0986, 0x56F7, 0x6454, 0xC42D, 0x01D2, 0xC253, 0x1180 };

static void HOTT_TXID_init()
{
 uint_farptr_t pdata = pgm_get_far_address(ZZ_HOTT_hop_val);
 pdata += (HOTT_NUM_CHANNEL_P2M * 2);
 uint16_t hott_hop_val = pgm_read_word_far(pdata);
 memcpy(&packet_p2M[0],&hott_hop_val,2);

 pdata = pgm_get_far_address(ZZ_HOTT_hop);
 pdata += (HOTT_NUM_CHANNEL_P2M * HOTT_NUM_RF_CHANNELS);
 for(uint8_t i=0; i<HOTT_NUM_RF_CHANNELS; i++)
  channel_used_p2M[i] = pgm_read_byte_far(pdata++);
 memset(&packet_p2M[30],0xFF,9);
 packet_p2M[39]=0x07;									// unknown and constant
 if(HOTT_BIND_P2M)
  {
   memset(&packet_p2M[40],0xFA,5);
   memcpy(&packet_p2M[45],temp_rfid_addr_p2M,4);
   packet_p2M[45+4] = RXNUM; //  model match like
  }
 else
  {
   memcpy(&packet_p2M[40],temp_rfid_addr_p2M,4);
   packet_p2M[40+4] = RXNUM; //  model match like
   memcpy(&packet_p2M[45],&HOTT_RXNUM_SAVE_AREA,5); // get HOTT RX ID
  }
}

static void HOTT_rf_init()
{
 CC2500_Strobe(CC2500_SIDLE);

 uint_farptr_t pdata = pgm_get_far_address(ZZ_HOTTInitSequence);
 for (uint8_t i = 0; i < 39; ++i)
  CC2500_WriteReg(i, pgm_read_byte_far(pdata++));

 CC2500_ManageFreq();

 CC2500_SetTxRxMode(TX_EN);
 CC2500_SetPower(1);
}

static void HOTT_init()
{
 loadrfidaddr();
 CC2500_Reset();
 srandom(g_eeGeneral.fixed_ID.ID_32 & 0xfefefefe);
 HOTT_NUM_CHANNEL_P2M = random()%16;
 HOTT_TXID_init();
 HOTT_rf_init();
#ifdef HOTT_FW_TELEMETRY
 HoTT_SerialRX_val=0;
 HoTT_SerialRX=false;
 HOTT_sensor_cur=3;
 HOTT_sensor_pages=0;
 HOTT_sensor_valid=false;
 HOTT_sensor_seq=0;
 for(uint8_t i=0; i<HOTT_SENSOR_TYPE; i++)
  HOTT_sensor_ok[i]=false;	// no sensors detected
 packet_count=0;
 state=HOTT_SENSOR_SEARCH_PERIOD;
#endif
 HOTT_SEND_SEQ_P2M = HOTT_START;
}

static void HOTT_tune_chan()
{
 CC2500_Strobe(CC2500_SIDLE);
 CC2500_WriteReg(CC2500_0A_CHANNR, (HOTT_RF_CH_NUM_P2M+1)*3);
 CC2500_Strobe(CC2500_SCAL);
}

static void HOTT_tune_chan_fast()
{
 CC2500_Strobe(CC2500_SIDLE);
 CC2500_WriteReg(CC2500_0A_CHANNR, (HOTT_RF_CH_NUM_P2M+1)*3);
 CC2500_WriteReg(CC2500_25_FSCAL1, calData[HOTT_RF_CH_NUM_P2M]);
}

static void HOTT_tune_freq()
{
 uint8_t freqMem = FREQ_FINE_MEM_P2M;
 CC2500_ManageFreq();
 if (freqMem != FREQ_FINE_MEM_P2M)
  {
   CC2500_WriteReg(CC2500_0F_FREQ0, HOTT_FREQ0_VAL + HOTT_COARSE);
   HOTT_SEND_SEQ_P2M = HOTT_START;								// Restart the tune process if option is changed to get good tuned values
  }
}

static void HOTT_prep_data_packet()
{
 static uint8_t upper=0;

 packet_p2M[2] = HOTT_HOP_FREQ_NO_P2M;

 packet_p2M[3] = upper;									// used for failsafe and upper channels (only supporting 16 channels)
#ifdef FAILSAFE_ENABLE
 static uint8_t failsafe_count=0;
 if(IS_FAILSAFE_VALUES_on && IS_BIND_DONE)
  {
   failsafe_count++;
   if(failsafe_count>=3)
    {
     FAILSAFE_VALUES_off;
     failsafe_count=0;
    }
  }
 else
  failsafe_count=0;
#endif

// Channels value are PPM*2, -100%=1100µs, +100%=1900µs, order TAER
 uint16_t val;
 for(uint8_t i=4; i<28; i+=2)
  {
   uint8_t ch=(i-4)>>1;
   if(upper && ch >= 8)
    ch+=4;										// when upper swap CH9..CH12 by CH13..16
   val = (FULL_CHANNEL_OUTPUTS(ch)); // +-1280
   val += PPM_CENTER*2; // + 1500 offset
#ifdef FAILSAFE_ENABLE
   if(failsafe_count==1)
    {
     // first failsafe packet
     packet_p2M[3] |= 0x40;
     uint16_t fs=Failsafe_data[ch];
     if( fs == FAILSAFE_CHANNEL_HOLD || fs == FAILSAFE_CHANNEL_NOPULSES)
      val|=0x8000;						// channel hold flag
     else
      {
       val=(((fs<<2)+fs)>>2)+860*2;		// value range 860<->2140 *2 <-> -125%<->+125%
       val|=0x4000;						// channel specific position flag
      }
    }
   else if(failsafe_count==2)
    {
     // second failsafe packet=timing?
     packet_p2M[3] |= 0x50;
     if(i==4)
      val=2;
     else
      val=0;
    }
#endif
   packet_p2M[i] = val;
   packet_p2M[i+1] = val>>8;
  }
 upper ^= 0x01;										// toggle between CH9..CH12 and CH13..16

 packet_p2M[28] = 0x80;									// no sensor
 packet_p2M[29] = 0x02;									// 0x02 when bind starts then when RX replies cycle in sequence 0x1A/22/2A/0A/12, 0x02 during normal packets, 0x01->text config menu, 0x0A->no more RX telemetry
#ifdef HOTT_FW_TELEMETRY
 if(HOTT_BIND_P2M)
  {
   static uint8_t prev_SerialRX_val=0;
   if(HoTT_SerialRX)
    {
     //Text mode
     uint8_t sensor=HoTT_SerialRX_val&0xF0;
     if((sensor&0x80) && sensor!=0xF0 && (HoTT_SerialRX_val&0x0F) >= 0x07)
      {
       //Valid Text query
       if(sensor==0x80) HoTT_SerialRX_val&=0x0F;	// RX only
       if(prev_SerialRX_val!=HoTT_SerialRX_val)
        {
         prev_SerialRX_val=HoTT_SerialRX_val;
         packet_p2M[28] = HoTT_SerialRX_val;			// send the button being pressed only once
        }
       else
        packet_p2M[28] = HoTT_SerialRX_val | 0x0F;	// no button pressed
      }
     else
      packet_p2M[28] = 0x0F;							// RX, no button pressed
     if(sub_protocol == HOTT_SYNC)
      packet_p2M[29] = ((HOTT_sensor_seq+1)<<3) | 1;	// Telemetry packet sequence
     else
      packet_p2M[29] = 0x01;							// 0x01->Text config menu
    }
   else
    {
     packet_p2M[28] = 0x89+HOTT_sensor_cur;				// 0x89/8A/8B/8C/8D/8E during normal packets
     if(sub_protocol == HOTT_SYNC)
      packet_p2M[29] = ((HOTT_sensor_seq+1)<<3) | 2;	// Telemetry packet sequence
    }
   //debugln("28=%02X,29=%02X",packet_p2M[28],packet_p2M[29]);
  }
#endif

 CC2500_WriteReg(CC2500_06_PKTLEN, HOTT_TX_PACKET_LEN);
 CC2500_WriteRegisterMulti(CC2500_3F_TXFIFO, packet_p2M, HOTT_TX_PACKET_LEN);
 HOTT_HOP_FREQ_NO_P2M %= HOTT_NUM_RF_CHANNELS;
 HOTT_RF_CH_NUM_P2M =  channel_used_p2M[HOTT_HOP_FREQ_NO_P2M];
}

static uint16_t HOTT_cb()
{
 switch(HOTT_SEND_SEQ_P2M)
  {
  case HOTT_START:
   SCHEDULE_MIXER_END_IN_US(4000); // Schedule next Mixer calculations.
   HOTT_RF_CH_NUM_P2M = 0;
   HOTT_tune_chan();
   HOTT_SEND_SEQ_P2M = HOTT_CAL;
   return 2000*2;
  case HOTT_CAL:
   calData[HOTT_RF_CH_NUM_P2M]=CC2500_ReadReg(CC2500_25_FSCAL1);
   if (++HOTT_RF_CH_NUM_P2M < HOTT_NUM_RF_CHANNELS)
    HOTT_tune_chan();
   else
    {
     HOTT_HOP_FREQ_NO_P2M = 0;
     HOTT_RF_CH_NUM_P2M = channel_used_p2M[HOTT_HOP_FREQ_NO_P2M];
     CC2500_SetTxRxMode(RX_EN);
     HOTT_SEND_SEQ_P2M = HOTT_DATA1;
    }
   return 2000*2;

  /* Work cycle: 10ms */
  case HOTT_DATA1:
   if (HOTT_MIX_SWAP_P2M ^= 1) SCHEDULE_MIXER_END_IN_US(20000); // Schedule next Mixer calculations.
//Set RF freq, setup LBT and prep packet
#ifdef MULTI_SYNC
   telemetry_set_input_sync(HOTT_PACKET_PERIOD);
#endif
//Clear all
   CC2500_Strobe(CC2500_SIDLE);
   CC2500_Strobe(CC2500_SNOP);
   CC2500_Strobe(CC2500_SFTX);
   CC2500_Strobe(CC2500_SFRX);
   CC2500_WriteReg(CC2500_04_SYNC1, 0xD3);
   CC2500_WriteReg(CC2500_05_SYNC0, 0x91);
//Set RF freq
   HOTT_tune_freq();
   HOTT_tune_chan_fast();
//Setup LBT
   CC2500_WriteReg(CC2500_1B_AGCCTRL2, 0xFF);
   CC2500_WriteReg(CC2500_1C_AGCCTRL1, 0x0C);
   CC2500_Strobe(CC2500_SRX);
//Prep packet
   HOTT_prep_data_packet();
//Listen
   CC2500_WriteReg(CC2500_17_MCSM1, 0x10);		//??
   CC2500_WriteReg(CC2500_18_MCSM0, 0x18);		//??
   CC2500_Strobe(CC2500_SRX);					//??
   HOTT_SEND_SEQ_P2M++;		//HOTT_DATA2
   heartbeat |= HEART_TIMER_PULSES;
   CALCULATE_LAT_JIT(); // Calculate latency and jitter.
   return 1095*2;
  case HOTT_DATA2:
//LBT
   if((CC2500_ReadReg(CC2500_38_PKTSTATUS | CC2500_READ_BURST)&0x10)==0)
    {
     //Channel is busy
     CC2500_SetPower(1);										// Reduce to low power before transmitting
    }
   CC2500_WriteReg(CC2500_17_MCSM1, 0x00);		//??
   CC2500_WriteReg(CC2500_18_MCSM0, 0x08);		//??
   CC2500_ManagePower();
//Send packet
   CC2500_SetTxRxMode(TX_EN);
   CC2500_Strobe(CC2500_STX);
   HOTT_SEND_SEQ_P2M++;		//HOTT_RX1
   return 3880*2;
  case HOTT_RX1:
//Clear all
   CC2500_Strobe(CC2500_SIDLE);
   CC2500_Strobe(CC2500_SFTX);
   CC2500_Strobe(CC2500_SFRX);
//RX
   if(packet_p2M[29] & 0xF8)
    {
     // Sync telemetry
     CC2500_WriteReg(CC2500_04_SYNC1, 0x2C);
     CC2500_WriteReg(CC2500_05_SYNC0, 0x6E);
    }
   CC2500_SetTxRxMode(RX_EN);
   CC2500_WriteReg(CC2500_1B_AGCCTRL2, 0xC7);
   CC2500_WriteReg(CC2500_1C_AGCCTRL1, 0x09);
   CC2500_WriteReg(CC2500_06_PKTLEN, HOTT_RX_PACKET_LEN);
   CC2500_Strobe(CC2500_SRX);
   HOTT_SEND_SEQ_P2M++;		//HOTT_RX2
   return 4025*2;
  case HOTT_RX2:
//Telemetry
   uint8_t len = CC2500_ReadReg(CC2500_3B_RXBYTES | CC2500_READ_BURST) & 0x7F;
   if (len==HOTT_RX_PACKET_LEN+2)
    {
     CC2500_ReadData(telem_save_data_buff, len);
     if((telem_save_data_buff[HOTT_RX_PACKET_LEN+1]&0x80) && memcmp(temp_rfid_addr_p2M,telem_save_data_buff,4)==0)
      {
       // CRC OK and TX ID matches
       if(HOTT_BIND_P2M)
        {
         //GR-16:  D4 20 F2 E6 F6 31 BD 01 00 90 00 FF 03 00 9E 1B 00 00 00 00 00 00
         //GR-12L: D4 20 F2 E6 F6 6E EE 01 00 B1 00 FF 03 00 0E 08 10 00 02 00 00 00
         //Vector: D4 20 F2 E6 F6 00 00 3A 01 A1 00 00 1A 24 35 1A 00 24 00 00 00 1A
         //        -----TXID----- -----RXID----- ---------------Unknown-------------
         memcpy(&HOTT_RXNUM_SAVE_AREA, &telem_save_data_buff[5],5); // store HOTT RX ID
         eeDirty(EE_MODEL);
         HOTT_BIND_P2M = 0;
         HOTT_TXID_init();
        }
#ifdef HOTT_FW_TELEMETRY
//#if defined(FRSKY)
//          if (g_model.rfOptionBool1) // telemetry on? // TODO ....
       else
        {
         //Telemetry
         // [0..4] = TXID
         // [5..9] = RXID
         // [10] = holds warnings issued by hott devices
         // [11] = telmetry pages. For sensors 0x00 to 0x04, for config mennu 0x00 to 0x12.
         // Normal telem page 0 = 0x55, 0x32, 0x38, 0x55, 0x64, 0x32, 0xD0, 0x07, 0x00, 0x55
         //   Page 0 [12] = [21] = [15]
         //   Page 0 [13] = RX_Voltage Cur*10 in V
         //   Page 0 [14] = Temperature-20 in °C
         //   Page 0 [15] = RX_RSSI CC2500 formated (a<128:a/2-71dBm, a>=128:(a-256)/2-71dBm)
         //   Page 0 [16] = RX_LQI in %
         //   Page 0 [17] = RX_Voltage Min*10 in V
         //   Page 0 [18,19] = [19]<<8+[18]=max lost packet time in ms, max value seems 2s=0x7D0
         //   Page 0 [20] = rx events
         //
         // Config menu consists of the different telem pages put all together
         //   Page X [12] = seems like all the telem pages with the same value are going together to make the full config menu text. Seen so far 'a', 'b', 'c', 'd'
         //   Page X [13..21] = 9 ascii chars to be displayed, char is highlighted when ascii|0x80
         //   Screen display is 21 characters large which means that once the first 21 chars are filled go to the begining of the next line
         //   Menu commands are sent through TX packets:
         //     packet_p2M[28]= 0xXF=>no key press, 0xXD=>down, 0xXB=>up, 0xX9=>enter, 0xXE=>right, 0xX7=>left with X=0 or D
         //     packet_p2M[29]= 0xX1/0xX9 with X=0 or X counting 0,1,1,2,2,..,9,9
         // Reduce telemetry to 14 bytes
         telem_save_data_buff[0]= telem_save_data_buff[HOTT_RX_PACKET_LEN];
         telem_save_data_buff[1]= TX_LQI;
         uint8_t hott_warnings = telem_save_data_buff[10];						// save warnings from hott devices
         bool send_telem=true;
         HOTT_sensor_seq++;											// Increment RX sequence counter
         if(packet_p2M[29] & 1)
          {
           //Text mode
           HOTT_sensor_seq %= 19;									// 19 pages in Text mode
           HOTT_sensor_pages = 0;
           HOTT_sensor_valid = false;
           telem_save_data_buff[10] = 0x80;									// Marking telem Text mode
           telem_save_data_buff[12] = 0;
           for(uint8_t i=0; i<HOTT_SENSOR_TYPE; i++)
            telem_save_data_buff[12] |= HOTT_sensor_ok[i]<<i;				// Send detected sensors
          }
         else
          {
           //Binary sensor
           HOTT_sensor_seq %= 5;									// 5 pages in binary mode per sensor
           if(state==0 && HOTT_sensor_ok[0]==false && HOTT_sensor_ok[1]==false && HOTT_sensor_ok[2]==false && HOTT_sensor_ok[3]==false && HOTT_sensor_ok[4]==false && HOTT_sensor_ok[5]==false)
            HOTT_sensor_seq=0;									// No sensors always ask page 0
           if(state)
            state--;
           if( telem_save_data_buff[11]==1 )									// Page 1
            {
             if( telem_save_data_buff[12] == (HOTT_sensor_cur+9)<<4 )
              {
               //Requested sensor is sending: 0x90/A0/B0/C0/D0/E0
               HOTT_sensor_pages = 0;							// Sensor first page received
               HOTT_sensor_valid = true;						// Data from the expected sensor is being received
               HOTT_sensor_ok[(telem_save_data_buff[12]>>4)-9]=true;
              }
             else
              {
               HOTT_sensor_valid = false;
               HOTT_sensor_pages = 0x1E;						// Switch to next sensor
              }
            }
           if(telem_save_data_buff[11])
            {
             //Page != 0
             if(HOTT_sensor_valid)								// Valid
              {
               telem_save_data_buff[10] = HOTT_sensor_cur+9;				// Mark telem with sensor ID
               HOTT_sensor_pages |= 1<<telem_save_data_buff[11];			// Page received
              }
             else
              send_telem=false;								// Do not send
            }
           else
            telem_save_data_buff[10]=0;									// Mark telem with sensor 0=RX
          }
         debug("T%d=",send_telem);
         for(uint8_t i=10; i < HOTT_RX_PACKET_LEN; i++)
          {
           telem_save_data_buff[i-8]=telem_save_data_buff[i];
           debug(" %02X",telem_save_data_buff[i]);
          }
         telem_save_data_buff[14] = hott_warnings;								// restore saved warnings from hott devices
         debugln("");
         if(send_telem)
          telemetry_link=2;
         if((HOTT_sensor_pages&0x1E) == 0x1E)						// All 4 pages received from the sensor
          {
           //Next sensor
           uint8_t loop=0;
           do
            {
             HOTT_sensor_cur++;									// Switch to next sensor
             HOTT_sensor_cur %= HOTT_SENSOR_TYPE;
             loop++;
            }
           while(HOTT_sensor_ok[HOTT_sensor_cur]==false && loop<HOTT_SENSOR_TYPE+1 && state==0);
           HOTT_sensor_valid=false;
           HOTT_sensor_pages=0;
           HOTT_sensor_seq=0;
           debugln("Sensor:%02X",HOTT_sensor_cur+9);
          }
        }
       pps_counter++;
#endif
      }
    }
#ifdef HOTT_FW_TELEMETRY
   packet_count++;
   if(packet_count>=100)
    {
     TX_LQI=pps_counter;
     if(pps_counter==0)
      {
       // lost connection with RX, power cycle? research sensors again.
       HOTT_sensor_cur=3;
       HOTT_sensor_seq=0;
       HOTT_sensor_valid=false;
       for(uint8_t i=0; i<HOTT_SENSOR_TYPE; i++)
        HOTT_sensor_ok[i]=false;	// no sensors detected
       state=HOTT_SENSOR_SEARCH_PERIOD;
      }
     pps_counter=packet_count=0;
    }
#endif
   HOTT_SEND_SEQ_P2M=HOTT_DATA1;
   return 1000*2;
  }
 return 0;
}


static void HOTT_initialize(uint8_t bind)
{
 HOTT_BIND_P2M = bind;
 HOTT_init();
 PROTO_Start_Callback( HOTT_cb);
}

const void *HOTT_Cmds(enum ProtoCmds cmd)
{
 switch(cmd)
  {
  case PROTOCMD_INIT:
   HOTT_initialize(0);
   return 0;
  case PROTOCMD_RESET:
   PROTO_Stop_Callback();
   return 0;
  case PROTOCMD_BIND:
   HOTT_initialize(1);
   return 0;
  case PROTOCMD_GETOPTIONS:
   SetRfOptionSettings(pgm_get_far_address(RfOpt_HOTT_Ser),
                       STR_DUMMY,      //Sub proto
                       STR_RFTUNEFINE, //Option 1 (int)
                       STR_DUMMY,      //Option 2 (int)
                       STR_RFPOWER,    //Option 3 (uint 0 to 31)
                       STR_TELEMETRY,  //OptionBool 1
                       STR_DUMMY,      //OptionBool 2
                       STR_DUMMY       //OptionBool 3
                      );
   return 0;
  default:
   break;
  }
 return 0;
}
