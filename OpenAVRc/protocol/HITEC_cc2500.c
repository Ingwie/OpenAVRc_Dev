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


const static RfOptionSettingsvar_t RfOpt_HITEC_Ser[] PROGMEM =
{
 /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
 /*rfSubTypeMax*/1,
 /*rfOptionValue1Min*/-128, // FREQFINE MIN
 /*rfOptionValue1Max*/127,  // FREQFINE MAX
 /*rfOptionValue2Min*/0,
 /*rfOptionValue2Max*/0,
 /*rfOptionValue3Max*/7,    // RF POWER
};

const pm_char STR_SUBTYPE_HITEC[] PROGMEM = "OPTI""MINI";

#define HITEC_BIND rfState8_p2M
#define HITEC_RF_CH_NUM channel_offset_p2M
#define HITEC_PACKET_LEN 13
#define HITEC_TX_ID_LEN	  2
#define HITEC_NUM_FREQUENCE		21
#define HITEC_BIND_NUM_FREQUENCE 14
#define HITEC_RX_BUFFER_SIZE (MAX_CHANNEL - HITEC_NUM_FREQUENCE)
#define HITEC_TELEMETRY (g_model.rfOptionBool1)
#define F5_COUNTER dp_crc_init_p2M
#define F5_FRAME packet_count_p2M

enum
{
 HITEC_START = 0x00,
 HITEC_CALIB = 0x01,
 HITEC_PREP  = 0x02,
 HITEC_DATA1 = 0x03,
 HITEC_DATA2 = 0x04,
 HITEC_DATA3 = 0x05,
 HITEC_DATA4 = 0x06,
 HITEC_RX1	 = 0x07,
 HITEC_RX2	 = 0x08,
};

enum
{
 HITEC_OPTIMA = 0x00,
 HITEC_MINIMA = 0x01,
};

const static uint8_t ZZ_HITECInitSequence[] PROGMEM =
{
 /* 00 */ 0x2F, 0x2E, 0x2F, 0x07, 0xD3, 0x91, 0xFF, 0x04,
 /* 08 */ 0x45, 0x00, 0x00, 0x12, 0x00, 0x5C, 0x85, 0xE8,
 /* 10 */ 0x3D, 0x3B, 0x73, 0x73, 0x7A, 0x01, 0x07, 0x30,
 /* 18 */ 0x08, 0x1D, 0x1C, 0xC7, 0x40, 0xB0, 0x87, 0x6B,
 /* 20 */ 0xF8, 0xB6, 0x10, 0xEA, 0x0A, 0x00, 0x11
};

// Generate RF channels
static void  HITEC_RF_channels()
{
//Normal hopping
 uint8_t idx = 0;
 uint32_t rnd;
 memcpy(&rnd, temp_rfid_addr_p2M, 4); // load id

 while (idx < HITEC_NUM_FREQUENCE)
  {
   uint8_t i;
   uint8_t count_0_47 = 0, count_48_93 = 0, count_94_140 = 0;

   rnd = rnd * 0x0019660D + 0x3C6EF35F; // Randomization
   // Use least-significant byte and make sure it's pair.
   uint8_t next_ch = ((rnd >> 8) % 141) & 0xFE;
   // Check that it's not duplicated and spread uniformly
   for (i = 0; i < idx; i++)
    {
     if(channel_used_p2M[i] == next_ch)
      break;
     if(channel_used_p2M[i] <= 47)
      count_0_47++;
     else if (channel_used_p2M[i] <= 93)
      count_48_93++;
     else
      count_94_140++;
    }
   if (i != idx)
    continue;
   if ((next_ch <= 47 && count_0_47 < 8) || (next_ch >= 48 && next_ch <= 93 && count_48_93 < 8) || (next_ch >= 94 && count_94_140 < 8))
    channel_used_p2M[idx++] = next_ch; //find hopping frequency
  }
}

static void HITEC_init()
{
 CC2500_Strobe(CC2500_SIDLE);

 uint_farptr_t pdata = pgm_get_far_address(ZZ_HITECInitSequence);

 for (uint8_t i = 0; i<(DIM(ZZ_HITECInitSequence)); ++i)
  {
   uint8_t dat = pgm_read_byte_far(pdata++);
   CC2500_WriteReg(i, dat);
  }

 CC2500_ManageFreq();
 CC2500_SetTxRxMode(TX_EN);
 CC2500_SetPower(TXPOWER_1);
}

static void HITEC_tune_chan()
{
 CC2500_Strobe(CC2500_SIDLE);
 if(HITEC_BIND)
  CC2500_WriteReg(CC2500_0A_CHANNR, channel_index_p2M*10);
 else
  CC2500_WriteReg(CC2500_0A_CHANNR, channel_used_p2M[channel_index_p2M]);
 CC2500_Strobe(CC2500_SFTX);
 CC2500_Strobe(CC2500_SCAL);
 CC2500_Strobe(CC2500_STX);
}

static void HITEC_change_chan_fast()
{
 CC2500_Strobe(CC2500_SIDLE);
 if(HITEC_BIND)
  CC2500_WriteReg(CC2500_0A_CHANNR, channel_index_p2M*10);
 else
  CC2500_WriteReg(CC2500_0A_CHANNR, channel_used_p2M[channel_index_p2M]);
 CC2500_WriteReg(CC2500_25_FSCAL1, calData[channel_index_p2M]);
}

static void HITEC_build_packet()
{
 F5_FRAME=1;
 F5_COUNTER=0;
 uint8_t offset;

 packet_p2M[1] = temp_rfid_addr_p2M[1];
 packet_p2M[2] = temp_rfid_addr_p2M[2];
 packet_p2M[3] = temp_rfid_addr_p2M[3];
 packet_p2M[22] = 0xEE;			// unknown always 0xEE
 if(HITEC_BIND)
  {
   packet_p2M[0] = 0x16;		// 22 bytes to follow
   memset(packet_p2M+5,0x00,14);
   switch(bind_idx_p2M)
    {
    case 0x72:			// first part of the hopping table
     for(uint8_t i=0; i<14; i++)
      packet_p2M[5+i]=channel_used_p2M[i]>>1;
     break;

    case 0x73:			// second part of the hopping table
     for(uint8_t i=0; i<7; i++)
      packet_p2M[5+i]=channel_used_p2M[i+14]>>1;
     break;

    case 0x74:
     packet_p2M[7]=0x55;	// unknown but bind does not complete if not there
     packet_p2M[8]=0x55;	// unknown but bind does not complete if not there
     break;

    case 0x7B:
     packet_p2M[5]=channel_used_p2M[13]>>1;	// if not there the Optima link is jerky...
     packet_p2M[14]=0x2A;
     packet_p2M[15]=0x46; // unknown but if 0x45 then 17=0x46, if 0x46 then 17=0x46 or 0x47, if 0x47 then 0x45 or 0x46
     packet_p2M[16]=0x2A;
     packet_p2M[17]=0x47;
     packet_p2M[18]=0x2A;
     break;
    }
   if(g_model.rfSubType==HITEC_MINIMA)
    packet_p2M[4] = bind_idx_p2M+0x10;
   else
    packet_p2M[4] = bind_idx_p2M;	// Optima: increments based on RX answer
   packet_p2M[19] = 0x08;		// packet sequence
   offset=20;				// packet_p2M[20] and [21]
  }
 else
  {
   packet_p2M[0] = 0x1A;		// 26 bytes to follow
   for(uint8_t i=0; i<9; i++)
    {
#define HITEC_MIN     0x1B87
#define HITEC_MAX     0x3905
     //7047 - 10822 - 14597
     //int32_t value = (FULL_CHANNEL_OUTPUTS(i)-(-RESX))*(HITEC_MAX-HITEC_MIN)/(RESX-(-RESX))+HITEC_MIN;
     uint16_t value = (int32_t)(FULL_CHANNEL_OUTPUTS(i)+RESX)*(HITEC_MAX-HITEC_MIN)/(2*RESX)+HITEC_MIN;

     packet_p2M[4+2*i] = value >> 8;
     packet_p2M[5+2*i] = value & 0xFF;
    }
   packet_p2M[23] = 0x80;		// packet sequence
   offset = 24;				// packet_p2M[24] and [25]
   packet_p2M[26] = 0x00;		// unknown always 0 and the RX doesn't seem to care about the value?
  }

 if(F5_FRAME)
  {
   // No idea what it is but Minima RXs are expecting these frames to work
   packet_p2M[offset] = 0xF5;
   packet_p2M[offset+1] = 0xDF;
   if((F5_COUNTER%9)==0)
    packet_p2M[offset+1] -= 0x04;	// every 8 packets send 0xDB
   F5_COUNTER++;
   F5_COUNTER%=59;					// every 6 0xDB packets wait only 4 to resend instead of 8
   F5_FRAME=0;					// alternate
   if(HITEC_BIND)
    packet_p2M[offset+1]++;			// when binding the values are 0xE0 and 0xDC
  }
 else
  {
   packet_p2M[offset] = 0x00;
   packet_p2M[offset+1] = 0x00;
   F5_FRAME = 1;	// alternate
  }
}

static void HITEC_send_packet()
{
 CC2500_WriteData(packet_p2M, packet_p2M[0]+1);
 if(HITEC_BIND)
  {
   packet_p2M[19] >>= 1;	// packet sequence
   if( (packet_p2M[4] & 0xFE) == 0x82 )
    {
     // Minima
     packet_p2M[4] ^= 1;					// alternate 0x82 and 0x83
     if( packet_p2M[4] & 0x01 )
      for(uint8_t i=0; i<7; i++)	// 0x83
       packet_p2M[5+i]=channel_used_p2M[i+14]>>1;
     else
      for(uint8_t i=0; i<14; i++)	// 0x82
       packet_p2M[5+i]=channel_used_p2M[i]>>1;
    }
  }
 else
  packet_p2M[23] >>= 1;	// packet sequence
}

uint16_t HITEC_callback()
{
 switch(send_seq_p2M)
  {
  case HITEC_START:
   HITEC_init();
   bind_idx_p2M = 0x72;
   if(HITEC_BIND)
    {
     HITEC_RF_CH_NUM=HITEC_BIND_NUM_FREQUENCE;
     // if(g_model.rfSubType==HITEC_OPTIMA) CC2500_SetPower(TXPOWER_3); // More power to bind optima /// TODO needed ?
    }
   else
    {
     HITEC_RF_CH_NUM=HITEC_NUM_FREQUENCE;
     //Set TXID
     CC2500_WriteReg(CC2500_05_SYNC0,temp_rfid_addr_p2M[2]);
     CC2500_WriteReg(CC2500_04_SYNC1,temp_rfid_addr_p2M[3]);
    }
   channel_index_p2M = 0;
   HITEC_tune_chan();
   send_seq_p2M = HITEC_CALIB;
   return 2000*2;

  case HITEC_CALIB:
   SCHEDULE_MIXER_END_IN_US(2000*2); // Schedule next Mixer calculations.
   calData[channel_index_p2M]=CC2500_ReadReg(CC2500_25_FSCAL1);
   ++channel_index_p2M;
   if (channel_index_p2M < HITEC_RF_CH_NUM)
    HITEC_tune_chan();
   else
    {
     channel_index_p2M = 0;
     send_seq_p2M = HITEC_PREP;
    }
   return 2000*2;

   /* Work cycle: 22.5ms */
#define HITEC_PACKET_PERIOD	22500U
#define HITEC_PREP_TIMING	462U
#define HITEC_DATA_TIMING	2736U
#define HITEC_RX1_TIMING	4636U
  case HITEC_PREP:
   SCHEDULE_MIXER_END_IN_US(HITEC_PACKET_PERIOD); // Schedule next Mixer calculations.
   if (freq_fine_mem_p2M == g_model.rfOptionValue1) // No user frequency change
    {
     HITEC_change_chan_fast();
     ++channel_index_p2M;
     if(channel_index_p2M>=HITEC_RF_CH_NUM)
      channel_index_p2M = 0;
     CC2500_ManagePower();
     CC2500_SetTxRxMode(TX_EN);
     HITEC_build_packet();
     send_seq_p2M++;
    }
   else
    send_seq_p2M = HITEC_START;	// Restart the tune process if option is changed to get good tuned values
   return HITEC_PREP_TIMING*2;

  case HITEC_DATA1:
  case HITEC_DATA2:
  case HITEC_DATA3:
  case HITEC_DATA4:
   HITEC_send_packet();
   send_seq_p2M++;
   return HITEC_DATA_TIMING*2;

  case HITEC_RX1:
   CC2500_SetTxRxMode(RX_EN);
   CC2500_Strobe(CC2500_SRX);	// Turn RX ON
   send_seq_p2M++;
   return HITEC_RX1_TIMING*2;

  case HITEC_RX2:
   uint8_t len = CC2500_ReadReg(CC2500_3B_RXBYTES | CC2500_READ_BURST) & 0x7F;
   if(len && len<HITEC_RX_BUFFER_SIZE)
    {
     // use a part of the channels buffer [50]
     uint8_t * rxBuf = &pulses2MHz.pbyte[CHANNEL_USED_OFFSET + HITEC_NUM_FREQUENCE]; //Use 50 Channel MAX -> 29 free bytes
     // Something has been received
     CC2500_ReadData(rxBuf, len);
     if( (rxBuf[len-1] & 0x80) && rxBuf[0]==len-3 && rxBuf[1]==temp_rfid_addr_p2M[1] && rxBuf[2]==temp_rfid_addr_p2M[2] && rxBuf[3]==temp_rfid_addr_p2M[3])
      {
       //valid crc && length ok && tx_id ok
        if(HITEC_BIND)
         {
          if(len==13)	// Bind packets have a length of 13
           {
            // bind packet: 0A,00,E5,F2,7X,05,06,07,08,09,00
            uint8_t check = 1;
            for(uint8_t i=5; i<10; i++)
             if(rxBuf[i]!=i) check = 0;

            if(((rxBuf[4]&0xF0)==0x70) && check)
             {
              bind_idx_p2M = rxBuf[4]+1;
              if(bind_idx_p2M==0x7B) // in dumps the RX stops to reply at 0x7B
               {
                if (++num_channel_p2M>164) // the RX stops to reply at 0x7B so wait a little and exit
                 {
                  send_seq_p2M = HITEC_START; // stop bind datas
                  HITEC_BIND = 0;
                 }
               }
             }
           }
         }
        else if( len==15 && rxBuf[4]==0 && rxBuf[12]==0 && HITEC_TELEMETRY) // good packet and telemetry on ?
         {
          // Valid telemetry packets
          // no station:
          //		0C,1C,A1,2B,00,00,00,00,00,00,00,8D,00,64,8E	-> 00 8D=>RX battery voltage 0x008D/28=5.03V
          // with HTS-SS:
          //		0C,1C,A1,2B,00,11,AF,00,2D,00,8D,11,00,4D,96	-> 00 8D=>RX battery voltage 0x008D/28=5.03V
          //		0C,1C,A1,2B,00,12,00,00,00,00,00,12,00,52,93
          //		0C,1C,A1,2B,00,13,00,00,00,00,46,13,00,52,8B	-> 46=>temperature2 0x46-0x28=30°C
          //		0C,1C,A1,2B,00,14,00,00,00,00,41,14,00,2C,93	-> 41=>temperature1 0x41-0x28=25°C
          //		0C,1C,A1,2B,00,15,00,2A,00,0E,00,15,00,44,96	-> 2A 00=>rpm1=420, 0E 00=>rpm2=140
          //		0C,1C,A1,2B,00,16,00,00,00,00,00,16,00,2C,8E
          //		0C,1C,A1,2B,00,17,00,00,00,42,44,17,00,48,8D	-> 42=>temperature3 0x42-0x28=26°C,44=>temperature4 0x44-0x28=28°C
          //		0C,1C,A1,2B,00,18,00,00,00,00,00,18,00,50,92
#if defined(TODO_FRSKY)
          TX_RSSI = rxBuf[13];
          if(TX_RSSI >=128)
           TX_RSSI -= 128;
          else
           TX_RSSI += 128;
          TX_LQI = rxBuf[14]&0x7F;

          if(g_model.rfSubType==HITEC_OPTIMA)
           {
            switch(rxBuf[5])		// telemetry frame number
             {
             case 0x00:
              v_lipo1 = (rxBuf[10])<<5 | (rxBuf[11])>>3;	// calculation in float is volt=(rxBuf[10]<<8+rxBuf[11])/28
              break;
             case 0x11:
              v_lipo1 = (rxBuf[9])<<5 | (rxBuf[10])>>3;	// calculation in float is volt=(rxBuf[9]<<8+rxBuf[10])/28
              break;
             case 0x18:
              v_lipo2 =  (rxBuf[6])<<5 | (rxBuf[7])>>3;	// calculation in float is volt=(rxBuf[6]<<8+rxBuf[7])/10
              break;
             }
            telemetry_link=1;			// telemetry hub available
           }
#endif
         }
      }
    }
   CC2500_Strobe(CC2500_SFRX);	// Flush the RX FIFO buffer
   send_seq_p2M = HITEC_PREP;
   heartbeat |= HEART_TIMER_PULSES;
   CALCULATE_LAT_JIT(); // Calculate latency and jitter.
   return (HITEC_PACKET_PERIOD -HITEC_PREP_TIMING -4*HITEC_DATA_TIMING -HITEC_RX1_TIMING)*2;
  }
 return 0;
}

static uint16_t HITEC_cb()
{
 uint16_t time = HITEC_callback();
 heartbeat |= HEART_TIMER_PULSES;
 CALCULATE_LAT_JIT(); // Calculate latency and jitter.
 return time;
}

static void HITEC_initialize(uint8_t bind)
{
 loadrfidaddr_rxnum(3);
 CC2500_Reset();
 HITEC_RF_channels();
 HITEC_BIND = bind; // store bind state
 send_seq_p2M = HITEC_START;
 PROTO_Start_Callback(HITEC_cb);
}

const void *HITEC_Cmds(enum ProtoCmds cmd)
{
 switch(cmd)
  {
  case PROTOCMD_INIT:
   HITEC_initialize(0);
   return 0;
  case PROTOCMD_RESET:
   PROTO_Stop_Callback();
   CC2500_Reset();
   return 0;
  case PROTOCMD_BIND:
   HITEC_initialize(1);
   return 0;
  case PROTOCMD_GETOPTIONS:
   SetRfOptionSettings(pgm_get_far_address(RfOpt_HITEC_Ser),
                       STR_SUBTYPE_HITEC,      //Sub proto
                       STR_RFTUNEFINE,      //Option 1 (int)
                       STR_DUMMY,      //Option 2 (int)
                       STR_RFPOWER,      //Option 3 (uint 0 to 31)
                       STR_TELEMETRY,      //OptionBool 1
                       STR_DUMMY,      //OptionBool 2
                       STR_DUMMY       //OptionBool 3
                      );
   return 0;
  default:
   break;
  }
 return 0;
}

/* Full telemetry
packet_p2M[0] = TX RSSI value
packet_p2M[1] = TX LQI value
packet_p2M[2] = frame number
packet_p2M[3-7] telemetry data

The frame number takes the following values: 0x00, 0x11, 0x12, ..., 0x1C. The frames can be present or not, they also do not have to follow each others.
Here is a description of the telemetry data for each frame number:
- frame 0x00
data byte 0 -> 0x00				unknown
data byte 1 -> 0x00				unknown
data byte 2 -> 0x00				unknown
data byte 3 -> RX Batt Volt_H
data byte 4 -> RX Batt Volt_L => RX Batt=(Volt_H*256+Volt_L)/28
- frame 0x11
data byte 0 -> 0xAF				start of frame
data byte 1 -> 0x00				unknown
data byte 2 -> 0x2D				station type 0x2D=standard station nitro or electric, 0xAC=advanced station
data byte 3 -> RX Batt Volt_H
data byte 4 -> RX Batt Volt_L => RX Batt=(Volt_H*256+Volt_L)/28
- frame 0x12
data byte 0 -> Lat_sec_H		GPS : latitude second
data byte 1 -> Lat_sec_L		signed int : 1/100 of second
data byte 2 -> Lat_deg_min_H	GPS : latitude degree.minute
data byte 3 -> Lat_deg_min_L	signed int : +=North, - = south
data byte 4 -> Time_second		GPS Time
- frame 0x13
data byte 0 -> 					GPS Longitude second
data byte 1 -> 					signed int : 1/100 of second
data byte 2 -> 					GPS Longitude degree.minute
data byte 3 -> 					signed int : +=Est, - = west
data byte 4 -> Temp2			Temperature2=Temp2-40°C
- frame 0x14
data byte 0 -> Speed_H
data byte 1 -> Speed_L			GPS Speed=Speed_H*256+Speed_L km/h
data byte 2 -> Alti_sea_H
data byte 3 -> Alti_sea_L		GPS Altitude=Alti_sea_H*256+Alti_sea_L m
data byte 4 -> Temp1			Temperature1=Temp1-40°C
- frame 0x15
data byte 0 -> FUEL
data byte 1 -> RPM1_L
data byte 2 -> RPM1_H			RPM1=RPM1_H*256+RPM1_L
data byte 3 -> RPM2_L
data byte 4 -> RPM2_H			RPM2=RPM2_H*256+RPM2_L
- frame 0x16
data byte 0 -> Date_year		GPS Date
data byte 1 -> Date_month
data byte 2 -> Date_day
data byte 3 -> Time_hour		GPS Time
data byte 4 -> Time_min
- frame 0x17
data byte 0 -> COURSEH
data byte 1 -> COURSEL			GPS heading = COURSEH*256+COURSEL in degrees
data byte 2 -> Count			GPS satellites
data byte 3 -> Temp3			Temperature3=Temp2-40°C
data byte 4 -> Temp4			Temperature4=Temp3-40°C
- frame 0x18
data byte 0 -> Volt_L			Volt=(Volt_H*256+Volt_L)/10 V
data byte 1 -> Volt_H
data byte 2 -> AMP_L
data byte 3 -> AMP_H			Amp=(AMP1_*256+AMP_L -180)/14 in signed A
- frame 0x19					Servo sensor
data byte 0 -> AMP_Servo1		Amp=AMP_Servo1/10 in A
data byte 1 -> AMP_Servo2		Amp=AMP_Servo2/10 in A
data byte 2 -> AMP_Servo3		Amp=AMP_Servo3/10 in A
data byte 3 -> AMP_Servo4		Amp=AMP_Servo4/10 in A
- frame 0x1A
data byte 2 -> ASpeed_H			Air speed=ASpeed_H*256+ASpeed_L km/h
data byte 3 -> ASpeed_L
- frame 0x1B					Variometer sensor
data byte 0 -> Alti1H
data byte 1 -> Alti1L			Altitude unfiltered
data byte 2 -> Alti2H
data byte 3 -> Alti2L			Altitude filtered
- frame 0x1C					Unknown
- frame 0x22					Unknown
*/
