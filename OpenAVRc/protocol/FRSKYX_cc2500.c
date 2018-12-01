/*
**************************************************************************
*                                                                        *
*                 ____                ___ _   _____                      *
*                / __ \___  ___ ___  / _ | | / / _ \____                 *
*               / /_/ / _ \/ -_) _ \/ __ | |/ /, _/ __/                 *
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

#define LBTMODE    (g_model.rfSubType == 0)
#define XTELEMETRY (g_model.rfOptionBool1)
#define X8MODE     (g_model.rfOptionBool2)

const pm_char STR_X8MODE[] PROGMEM = INDENT "X8-9mS";

const static RfOptionSettingsvarstruct RfOpt_FrskyX_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED | BOOL2USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/1,
  /*rfOptionValue1Min*/-128, // FREQFINE MIN
  /*rfOptionValue1Max*/127,  // FREQFINE MAX
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,    // RF POWER
};

const pm_char STR_SUBTYPE_FRSKYX[] PROGMEM = "XLBT""XFCC";

enum
{
  FRSKYX_DATA1,
  FRSKYX_DATA2,
  FRSKYX_DATA3,
  FRSKYX_DATA4,
  FRSKYX_DATA5,
};

// FCC,       EU  ,
static const uint8_t ZZ_FRSKYX_InitData_Start[] PROGMEM =
{
  CC2500_02_IOCFG0,  0x06,       0x06,
  CC2500_00_IOCFG2,  0x06,       0x06,
  CC2500_17_MCSM1,   0x0c,       0x0E,
  CC2500_18_MCSM0,   0x18,       0x18,
  CC2500_06_PKTLEN,  0x1E,       0x23,
  CC2500_07_PKTCTRL1,0x04,       0x04,
  CC2500_08_PKTCTRL0,0x01,       0x01,
  CC2500_3E_PATABLE, TXPOWER_1,  TXPOWER_1,
  CC2500_0B_FSCTRL1, 0x0A,       0x08,
  CC2500_0C_FSCTRL0, 0x00,       0x00,
  CC2500_0D_FREQ2,   0x5c,       0x5c,
  CC2500_0E_FREQ1,   0x76,       0x80,
  CC2500_0F_FREQ0,   0x27,       0x00,
  CC2500_10_MDMCFG4, 0x7B,       0x7B,
  CC2500_11_MDMCFG3, 0x61,       0xF8,
  CC2500_12_MDMCFG2, 0x13,       0x03,
  CC2500_13_MDMCFG1, 0x23,       0x23,
  CC2500_14_MDMCFG0, 0x7a,       0x7a,
  CC2500_15_DEVIATN, 0x51,       0x53
};

static void FRSKYX_init()
{
  channel_offset = 0;

  SCHEDULE_MIXER_END_IN_US(41000U); // Schedule next Mixer calculations.

  uint_farptr_t pdata = pgm_get_far_address(ZZ_FRSKYX_InitData_Start);

  for (uint8_t i=0; i<(DIM(ZZ_FRSKYX_InitData_Start)/3); i++)
    {
      // Send init EU or US
      uint8_t add = pgm_read_byte_far(pdata);
      uint8_t dat = LBTMODE ? pgm_read_byte_far(pdata+=2) : pgm_read_byte_far(++pdata);
      CC2500_WriteReg(add,dat);
      LBTMODE ? ++pdata : pdata+=2;
    }

  FRSKY_Init_Common_End();

  CC2500_ManageFreq();
  CC2500_SetPower(TXPOWER_1);

  //calibrate hop channels
  for (uint8_t c = 0; c < 48; c++)
    {
      CC2500_Strobe(CC2500_SIDLE);
      CC2500_WriteReg(CC2500_0A_CHANNR, channel_used[c]);
      CC2500_Strobe(CC2500_SCAL);
      _delay_us(900);
      calData[c] = CC2500_ReadReg(CC2500_25_FSCAL1); // VCO capacitance calibration
    }

  CC2500_Strobe(CC2500_SFTX); // 3b
  CC2500_Strobe(CC2500_SFRX); // 3a
  CC2500_Strobe(CC2500_SIDLE); // Go to idle...
}

static uint16_t Xcrc(uint8_t *data, uint8_t len)
{
  static const uint16_t ZZCRCTable[] PROGMEM =
  {
    0x0000,0x1189,0x2312,0x329b,0x4624,0x57ad,0x6536,0x74bf,
    0x8c48,0x9dc1,0xaf5a,0xbed3,0xca6c,0xdbe5,0xe97e,0xf8f7,
    0x1081,0x0108,0x3393,0x221a,0x56a5,0x472c,0x75b7,0x643e,
    0x9cc9,0x8d40,0xbfdb,0xae52,0xdaed,0xcb64,0xf9ff,0xe876,
    0x2102,0x308b,0x0210,0x1399,0x6726,0x76af,0x4434,0x55bd,
    0xad4a,0xbcc3,0x8e58,0x9fd1,0xeb6e,0xfae7,0xc87c,0xd9f5,
    0x3183,0x200a,0x1291,0x0318,0x77a7,0x662e,0x54b5,0x453c,
    0xbdcb,0xac42,0x9ed9,0x8f50,0xfbef,0xea66,0xd8fd,0xc974,
    0x4204,0x538d,0x6116,0x709f,0x0420,0x15a9,0x2732,0x36bb,
    0xce4c,0xdfc5,0xed5e,0xfcd7,0x8868,0x99e1,0xab7a,0xbaf3,
    0x5285,0x430c,0x7197,0x601e,0x14a1,0x0528,0x37b3,0x263a,
    0xdecd,0xcf44,0xfddf,0xec56,0x98e9,0x8960,0xbbfb,0xaa72,
    0x6306,0x728f,0x4014,0x519d,0x2522,0x34ab,0x0630,0x17b9,
    0xef4e,0xfec7,0xcc5c,0xddd5,0xa96a,0xb8e3,0x8a78,0x9bf1,
    0x7387,0x620e,0x5095,0x411c,0x35a3,0x242a,0x16b1,0x0738,
    0xffcf,0xee46,0xdcdd,0xcd54,0xb9eb,0xa862,0x9af9,0x8b70,
    0x8408,0x9581,0xa71a,0xb693,0xc22c,0xd3a5,0xe13e,0xf0b7,
    0x0840,0x19c9,0x2b52,0x3adb,0x4e64,0x5fed,0x6d76,0x7cff,
    0x9489,0x8500,0xb79b,0xa612,0xd2ad,0xc324,0xf1bf,0xe036,
    0x18c1,0x0948,0x3bd3,0x2a5a,0x5ee5,0x4f6c,0x7df7,0x6c7e,
    0xa50a,0xb483,0x8618,0x9791,0xe32e,0xf2a7,0xc03c,0xd1b5,
    0x2942,0x38cb,0x0a50,0x1bd9,0x6f66,0x7eef,0x4c74,0x5dfd,
    0xb58b,0xa402,0x9699,0x8710,0xf3af,0xe226,0xd0bd,0xc134,
    0x39c3,0x284a,0x1ad1,0x0b58,0x7fe7,0x6e6e,0x5cf5,0x4d7c,
    0xc60c,0xd785,0xe51e,0xf497,0x8028,0x91a1,0xa33a,0xb2b3,
    0x4a44,0x5bcd,0x6956,0x78df,0x0c60,0x1de9,0x2f72,0x3efb,
    0xd68d,0xc704,0xf59f,0xe416,0x90a9,0x8120,0xb3bb,0xa232,
    0x5ac5,0x4b4c,0x79d7,0x685e,0x1ce1,0x0d68,0x3ff3,0x2e7a,
    0xe70e,0xf687,0xc41c,0xd595,0xa12a,0xb0a3,0x8238,0x93b1,
    0x6b46,0x7acf,0x4854,0x59dd,0x2d62,0x3ceb,0x0e70,0x1ff9,
    0xf78f,0xe606,0xd49d,0xc514,0xb1ab,0xa022,0x92b9,0x8330,
    0x7bc7,0x6a4e,0x58d5,0x495c,0x3de3,0x2c6a,0x1ef1,0x0f78
  };
  uint_farptr_t pdata = pgm_get_far_address(ZZCRCTable);
  uint16_t crc = 0;
  for(uint8_t i=0; i < len; i++)
    crc = (crc<<8) ^ pgm_read_word_far(pdata+2*(((crc>>8) ^ *data++) & 0xFF));
  return crc;
}

static void FRSKYX_initialize_data(uint8_t adr)
{
  CC2500_WriteReg(CC2500_18_MCSM0,0x08);
  CC2500_WriteReg(CC2500_09_ADDR, adr ? 0x03 : temp_rfid_addr[3]);
  CC2500_WriteReg(CC2500_07_PKTCTRL1,0x05);
}

static void frskyX_set_start(uint8_t ch)
{
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_25_FSCAL1, calData[ch]);
  CC2500_WriteReg(CC2500_0A_CHANNR, channel_used[ch]);
}

static void frskyX_build_bind_packet()
{
  packet[0] = LBTMODE ? 0x20 : 0x1D; // LBT (EU) or  FCC (US)
  packet[1] = 0x03;
  packet[2] = 0x01;
  packet[3] = temp_rfid_addr[3];
  packet[4] = temp_rfid_addr[2];
  packet[5] = bind_idx; // Index into channels_used array.
  packet[6] =  channel_used[bind_idx++];
  packet[7] =  channel_used[bind_idx++];
  packet[8] =  channel_used[bind_idx++];
  packet[9] =  channel_used[bind_idx++];
  packet[10] = channel_used[bind_idx++];
  packet[11] = 0x02;
  packet[12] = RXNUM;

  uint8_t end_packet = LBTMODE ? 31 : 28 ;
  memset(&packet[13], 0, end_packet - 13);
  uint16_t lcrc = Xcrc(&packet[3], end_packet-3);
  packet[end_packet++] = lcrc >> 8;
  packet[end_packet] = lcrc;
  if(bind_idx > 49)
    bind_idx = 0;
}

static uint16_t scaleForPXX(uint8_t chan)
{
  // 0-2047, 0 = 817, 1024 = 1500, 2047 = 2182
  int16_t value = ((channelOutputs[chan] + 2*PPM_CH_CENTER(chan) - 2*PPM_CENTER)* 3 / 4) + 1024;
  limit<uint16_t>(1, value, 2046);
  if (chan > 7)
    value|=2048;   // upper channels offset
  return value;
}

static void frskyX_data_frame()
{
  //0x1D 0xB3 0xFD 0x02 0x56 0x07 0x15 0x00 0x00 0x00 0x04 0x40 0x00 0x04 0x40 0x00 0x04 0x40 0x00 0x04 0x40 0x08 0x00 0x00 0x00 0x00 0x00 0x00 0x96 0x12
  uint16_t chan_0 ;
  uint16_t chan_1 ;

#if defined(X_ANY)

#if (X_ANY >= 1)
  Xany_scheduleTx(0);
#endif
#if (X_ANY >= 2)
  Xany_scheduleTx(1);
#endif
#if (X_ANY >= 3)
  Xany_scheduleTx(2);
#endif
#if (X_ANY >= 4)
  Xany_scheduleTx(3);
#endif

#endif
  /* data frames sent every 9ms; failsafe every 9 seconds
  #ifdef FAILSAFE_ENABLE
  	static uint16_t failsafe_count=0;
  	static uint8_t FS_flag=0,failsafe_chan=0;
  	if (FS_flag == 0  &&  failsafe_count > FRX_FAILSAFE_TIME  &&  channel_offset == 0  &&  IS_FAILSAFE_VALUES_on)
  	{
  		FS_flag = 0x10;
  		failsafe_chan = 0;
  	} else if (FS_flag & 0x10 && failsafe_chan < (X8MODE ? 8-1:16-1))
  	{
  		FS_flag = 0x10 | ((FS_flag + 2) & 0x0F);	//10, 12, 14, 16, 18, 1A, 1C, 1E - failsafe packet
  		failsafe_chan ++;
  	} else if (FS_flag & 0x10)
  	{
  		FS_flag = 0;
  		failsafe_count = 0;
  	}
  	failsafe_count++;
  #endif */

  packet[0] = LBTMODE ? 0x20 : 0x1D ;	// LBT or FCC
  packet[1] = temp_rfid_addr[3];
  packet[2] = temp_rfid_addr[2];
  packet[3] = 0x02;
  //
  packet[4] = (channel_skip<<6)|channel_index;
  packet[5] = channel_skip>>2;
  packet[6] = RXNUM;
  //packet[7] = FLAGS 00 - standard packet
  //10, 12, 14, 16, 18, 1A, 1C, 1E - failsafe packet
  //20 - range check packet
  /*#ifdef FAILSAFE_ENABLE
  	packet[7] = FS_flag;
  #else*/
  packet[7] = 0;
  //#endif
  packet[8] = 0;
  //
  uint8_t startChan = channel_offset;

  for(uint8_t i = 0; i <12 ; i+=3)
    {
      //12 bytes of channel data
      /*#ifdef FAILSAFE_ENABLE
      	if( (FS_flag & 0x10) && ((failsafe_chan & 0x07) == (startChan & 0x07)) )
      		chan_0 = frskyX_scaleForPXX_FS(failsafe_chan);
      	else
      #endif */
      chan_0 = scaleForPXX(startChan);
      startChan++;
      //
      /*#ifdef FAILSAFE_ENABLE
      	if( (FS_flag & 0x10) && ((failsafe_chan & 0x07) == (startChan & 0x07)) )
      		chan_1 = frskyX_scaleForPXX_FS(failsafe_chan);
      	else
      #endif*/
      chan_1 = scaleForPXX(startChan);
      startChan++;
      //
      packet[9+i] = chan_0;	//3 bytes*4
      packet[9+i+1]=(((chan_0>>8) & 0x0F)|(chan_1 << 4));
      packet[9+i+2]=chan_1>>4;
    }
  packet[21] = (receive_seq << 4) | send_seq ;//8 at start

  if(X8MODE)			// in X8 mode send only 8ch every 9ms
    channel_offset = 0 ;
  else
    channel_offset^=0x08;

  uint8_t end_packet = LBTMODE ? 31 : 28;

  for (uint8_t i=22; i<end_packet; i++)
    packet[i]=0;
  /*#if defined SPORT_POLLING
  	uint8_t idxs=0;
  	if(ok_to_send)
  		for (uint8_t i=23;i<end_packet;i++)
  		{//
  			if(sport_index==sport_idx)
  			{//no new data
  				ok_to_send=false;
  				break;
  			}
  			packet[i]=SportData[sport_index];
  			sport_index= (sport_index+1)& (MAX_SPORT_BUFFER-1);
  			idxs++;
  		}
  	packet[22]= idxs;
  	#ifdef DEBUG_SERIAL
  		for(uint8_t i=0;i<idxs;i++)
  		{
  			Serial.print(packet[23+i],HEX);
  			Serial.print(" ");
  		}
  		Serial.println(" ");
  	#endif
  #endif // SPORT_POLLING */

  uint16_t lcrc = Xcrc(&packet[3], end_packet-3);
  packet[end_packet++]=lcrc>>8;//high byte
  packet[end_packet]=lcrc;//low byte
}

#if defined(FRSKY)
static void frskyX_check_telemetry(uint8_t *pkt, uint8_t len)
{
/*
Telemetry frames(RF) SPORT info 15 bytes
  SPORT frame 6+3 bytes
  [00] PKLEN  0E 0E 0E 0E
  [01] TXID1  DD DD DD DD
  [02] TXID2  6D 6D 6D 6D
  [03] CONST  02 02 02 02
  [04] RS/RB  2C D0 2C CE // D0,CE = 2*RSSI; ....2C = RX battery voltage(5V from Bec)
  [05] SEQ    03 10 21 32 // TX/RX telemetry hand-shake sequence number
  [06] NO.BT  00 00 06 03 // No.of valid SPORT frame bytes in the frame
  [07] STRM1  00 00 7E 00
  [08] STRM2  00 00 1A 00
  [09] STRM3  00 00 10 00
  [10] STRM4  03 03 03 03
  [11] STRM5  F1 F1 F1 F1
  [12] STRM6  D1 D1 D0 D0
  [13] CHKSUM1
  [14] CHKSUM2
  [15] RSSI
  [16] LQI

  The sequence byte contains 2 nibbles. The low nibble normally contains a 2-bit
  sequence number (0-3) that is the sequence of sending packets. The high nibble
  contains the "next expected" packet sequence to be received.
  Bit 2 of this nibble (bit 6 of the byte) is set to request a re-transmission of a missed packet.
  Bit 3 of the nibbles is used to indicate/acknowledge startup synchronisation.  // only process packets with the required id and packet length and good crc*/

  if ( pkt[0] == len - 3
       && pkt[1] == temp_rfid_addr[3]
       && pkt[2] == temp_rfid_addr[2]
       && (Xcrc(&pkt[3], len-7) == (uint16_t)(pkt[len-4] << 8 | pkt[len-3])))
    {
      if(frskyStreaming < FRSKY_TIMEOUT10ms -5)
        frskyStreaming +=5;
      // frskyStreaming gets decremented every 10ms, however we can only add to it every 4 *9ms, so we add 5.

      if (pkt[4] & 0x80)     // distinguish RSSI from VOLT1
        {
          telemetryData.rssi[0].set(pkt[4] & 0x7f);
        }
      /*else
        {
          telemetryData.analog[0].set(pkt[4] * 10,UNIT_VOLTS);      // In 1/100 of Volts
        } TODO use*/


      telemetryData.rssi[1].set(pkt[len-2] & 0x7f);


      if (((pkt[5] & 0x0f) == 0x08) || ((pkt[5] >> 4) == 0x08))     // restart
        {
          receive_seq = 8;
          send_seq = 0;
          parseTelemFrskyByte(START_STOP); // reset
        }
      else
        {
          if ((pkt[5] & 0x03) == (receive_seq & 0x03))
            {
              receive_seq = (receive_seq + 1) % 4;

              if(pkt[6]>0 && pkt[6]<=10)//
                {
                  for (uint8_t i=0; i < pkt[6]; i++)
                    {
                      parseTelemFrskyByte(pkt[7+i]);
                    }
                }
              // process any saved data from out-of-sequence packet if
              // it's the next expected packet
              if (telem_save_seq == receive_seq)
                {
                  receive_seq = (receive_seq + 1) % 4;
                  for (uint8_t i=0; i < telem_save_data[0]; i++)
                    {
                      parseTelemFrskyByte(telem_save_data[1+i]);
                    }
                }
              telem_save_seq = 0xFF;
            }
          else
            {
              receive_seq = (receive_seq & 0x03) | 0x04;  // incorrect sequence - request resend
              // if this is the packet after the expected one, save the sport data
              if ((pkt[5] & 0x03) == ((receive_seq+1) % 4) && pkt[6] <= 6)
                {
                  telem_save_seq = (receive_seq+1) % 4;
                  memcpy(telem_save_data, &pkt[6], pkt[6]+1);
                }
            }

        }
    }
}
#endif

static uint16_t FRSKYX_send_data_packet()
{
  switch(rfState8)
    {
    case FRSKYX_DATA1:
      if (X8MODE)
        {
          SCHEDULE_MIXER_END_IN_US(9000); // Schedule is possible on fast systems.
        }
      else if (!channel_offset)
        {
          SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
        }
      frskyX_data_frame();
      CC2500_ManageFreq();
      CC2500_ManagePower();
      CC2500_SetTxRxMode(TX_EN);
      frskyX_set_start(channel_index);
      CC2500_Strobe(CC2500_SFRX);
      channel_index = (channel_index+channel_skip)%47;
      CC2500_Strobe(CC2500_SIDLE);
      CC2500_WriteData(packet, packet[0]+1);
      //
//			frskyX_data_frame();
      ++rfState8;
      return 5200;
    case FRSKYX_DATA2:
      CC2500_SetTxRxMode(RX_EN);
      CC2500_Strobe(CC2500_SIDLE);
      ++rfState8;
      return 200;
    case FRSKYX_DATA3:
      CC2500_Strobe(CC2500_SRX);
      ++rfState8;
      return 3100;
    case FRSKYX_DATA4:

      uint8_t len = CC2500_ReadReg(CC2500_3B_RXBYTES | CC2500_READ_BURST) & 0x7F;
      if (len && (len<=(0x0E + 3)))				//Telemetry frame is 17
        {
          CC2500_ReadData(packet, len);
#if defined(FRSKY)
          if (XTELEMETRY) // telemetry on?
            {
              frskyX_check_telemetry(packet, len);
            }
#endif
        }
      if (send_seq != 8)
        send_seq = (send_seq + 1) % 4;

      rfState8 = FRSKYX_DATA1;
      return 500;
    }
  return 1;
}

static void FRSKYX_send_bind_packet()
{
  frskyX_set_start(47);
  CC2500_Strobe(CC2500_SFRX);
  frskyX_build_bind_packet();
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteData(packet, packet[0]+1);
}

static uint16_t FRSKYX_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(9000); // Schedule next Mixer calculations.
  FRSKYX_send_bind_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 9000U *2;
}

static uint16_t FRSKYX_cb()
{
  uint16_t time = FRSKYX_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return time*2;
}

static void FRSKYX_initialize(uint8_t bind)
{
  freq_fine_mem = 0;
  channel_index = 0;
  channel_skip = 0;
  send_seq = 0x08 ;
  receive_seq = 0 ;

  loadrfidaddr();
  CC2500_Reset();

  FRSKY_generate_channels();

#if defined (SIMU)
#define srandom(x) srand(x)
#define random() rand()
#endif

  while (!channel_skip)
    {
      srandom(g_eeGeneral.fixed_ID.ID_32 & 0xfefefefe);
      channel_skip = random()%47;
    }

  FRSKYX_init();

  if (bind)
    {
      FRSKYX_initialize_data(1);
      CC2500_SetTxRxMode(TX_EN);
      PROTO_Start_Callback( FRSKYX_bind_cb);
    }
  else
    {
      FRSKYX_initialize_data(0);
      rfState8 = FRSKYX_DATA1;
      PROTO_Start_Callback( FRSKYX_cb);
    }
}

const void *FRSKYX_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
    {
    case PROTOCMD_INIT:
      FRSKYX_initialize(0);
      return 0;
    case PROTOCMD_BIND:
      bind_idx = 0;
      FRSKYX_initialize(1);
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      CC2500_Reset();
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_FrskyX_Ser),
                          STR_SUBTYPE_FRSKYX,       //Sub proto
                          STR_RFTUNEFINE,      //Option 1 (int)
                          STR_DUMMY,       //Option 2 (int)
                          STR_RFPOWER,     //Option 3 (uint 0 to 31)
                          STR_TELEMETRY,   //OptionBool 1
                          STR_X8MODE,      //OptionBool 2
                          STR_DUMMY        //OptionBool 3
                         );
      return 0;
    default:
      break;
    }
  return 0;
}
