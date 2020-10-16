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

const static RfOptionSettingsvar_t RfOpt_FrskyD_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/-128, // FREQFINE MIN
  /*rfOptionValue1Max*/127,  // FREQFINE MAX
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,    // RF POWER
};

const static uint8_t ZZ_frskyDInitSequence[] PROGMEM =
{
  CC2500_17_MCSM1,      0x00, // Go to idle after tx & rx.
  CC2500_18_MCSM0,      0x18, // Auto calibrate when going from idle to tx/rx/fstxon
  CC2500_06_PKTLEN,     0x19,
  CC2500_07_PKTCTRL1,   0x05, // Address checking no broadcasts, Append status bytes to payload.
  CC2500_08_PKTCTRL0,   0x05, // No data whitening, FIFO for tx/rx, CRC generation and checking,
  CC2500_0B_FSCTRL1,    0x08,
  CC2500_0D_FREQ2,      0x5C,
  CC2500_0E_FREQ1,      0x76, // V8 was 0x58
  CC2500_0F_FREQ0,      0x27, // V8 was 0x9d
  CC2500_10_MDMCFG4,    0xAA,
  CC2500_11_MDMCFG3,    0x39, // 31044 baud.
  CC2500_12_MDMCFG2,    0x11, // V8 was 0x93
  CC2500_13_MDMCFG1,    0x23,
  CC2500_14_MDMCFG0,    0x7A,
  CC2500_15_DEVIATN,    0x42, // 0x41 (+-28564Hz) for 1way, 0x42 for 2way.
};

static void FRSKYD_init(uint8_t bind)
{
  packet_count_p2M = 0;

  uint_farptr_t pdata = pgm_get_far_address(ZZ_frskyDInitSequence);

  for (uint8_t i=0; i<(DIM(ZZ_frskyDInitSequence)/2); i++)   // Send init
    {
      uint8_t add = pgm_read_byte_far(pdata);
      uint8_t dat = pgm_read_byte_far(++pdata);
      CC2500_WriteReg(add,dat);
      ++pdata;
    }

  FRSKY_Init_Common_End();

  CC2500_ManageFreq();
  CC2500_SetPower(TXPOWER_1);

  CC2500_WriteReg(CC2500_09_ADDR, bind ? 0x03 : temp_rfid_addr_p2M[0]);
  CC2500_Strobe(CC2500_SFTX); // 3b
  CC2500_Strobe(CC2500_SFRX); // 3a
  CC2500_Strobe(CC2500_SIDLE); // Go to idle...
}

static void FRSKYD_build_bind_packet()
{
  packet_p2M[0] = 0x11; //Length (17)
  packet_p2M[1] = 0x03; //Packet type
  packet_p2M[2] = 0x01; //Packet type
  packet_p2M[3] = temp_rfid_addr_p2M[0];
  packet_p2M[4] = temp_rfid_addr_p2M[1];
  packet_p2M[5] = bind_idx_p2M; // Index into channels_used array.
  packet_p2M[6] =  channel_used_p2M[bind_idx_p2M++];
  packet_p2M[7] =  channel_used_p2M[bind_idx_p2M++];
  packet_p2M[8] =  channel_used_p2M[bind_idx_p2M++];
  packet_p2M[9] =  channel_used_p2M[bind_idx_p2M++];
  packet_p2M[10] = channel_used_p2M[bind_idx_p2M++];
  packet_p2M[11] = 0x00;
  packet_p2M[12] = 0x00;
  packet_p2M[13] = 0x00;
  packet_p2M[14] = 0x00;
  packet_p2M[15] = 0x00;
  packet_p2M[16] = 0x00;
  packet_p2M[17] = 0x01;

  if(bind_idx_p2M > 49)
    bind_idx_p2M = 0;
}

static void FRSKYD_build_data_packet()
{
#if defined(X_ANY)
  Xany_scheduleTx_AllInstance();
#endif
  packet_p2M[0] = 0x11; // Length
  packet_p2M[1] = temp_rfid_addr_p2M[0];
  packet_p2M[2] = temp_rfid_addr_p2M[1];
  packet_p2M[3] = packet_count_p2M;
#if defined(FRSKY)
  packet_p2M[4] = receive_seq_p2M; // acknowledge last packet value
#else
  packet_p2M[4] = 0x00;
#endif
  packet_p2M[5] = 0x01;
  // packet 6 to 9 contain LS byte of channels 1 to 4.
  packet_p2M[10] = 0; // Low nibble = channel 1, High nibble = channel 2.
  packet_p2M[11] = 0;
  // packet 12 to 15 contain LS byte of channels 5 to 8.
  packet_p2M[16] = 0;
  packet_p2M[17] = 0;

  for(uint8_t i = 0; i < 8; i++)
    {
      // 0x08CA / 1.5 = 1500 (us). Probably because they use 12MHz clocks.
      // 0x05DC -> 1000us 5ca
      // 0x0BB8 -> 2000us bca
      int16_t value = FULL_CHANNEL_OUTPUTS(i);
      value -= (value>>2); // x-x/4
      value = limit((int16_t)-(640 + (640>>1)), value, (int16_t)+(640 + (640>>1)));
      value += 0x08CA;

      if(i < 4)
        {
          packet_p2M[6+i] = value & 0xff;
          packet_p2M[10+(i>>1)] |= ((value >> 8) & 0x0f) << (4 *(i & 0x01));
        }
      else
        {
          packet_p2M[8+i] = value & 0xff;
          packet_p2M[16+((i-4)>>1)] |= ((value >> 8) & 0x0f) << (4 * ((i-4) & 0x01));
        }
    }
}

static uint16_t FRSKYD_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0);
  //CC2500_WriteReg(CC2500_23_FSCAL3, 0x89) // Todo have a try
  FRSKYD_build_bind_packet();
  CC2500_WriteData(packet_p2M, 18);
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

#if defined(FRSKY)

#define HUB_MAX_BYTES 6

static void frskyD_send_HUB_telemetry(uint8_t offset, uint8_t len)
{
  parseTelemFrskyByte(START_STOP);		// start
  parseTelemFrskyByte(USRPKT);				// user frame
  parseTelemFrskyByte(len);           // num of hub bytes
  parseTelemFrskyByte(packet_p2M[7]);     // sequence

  uint8_t lastcount = offset + len;

  for (uint8_t i=offset; i < lastcount; ++i)
    {
      if ((packet_p2M[i] == START_STOP) || (packet_p2M[i] == BYTESTUFF))
        {
          parseTelemFrskyByte(BYTESTUFF);
          packet_p2M[i] ^= STUFF_MASK;
        }
      parseTelemFrskyByte(packet_p2M[i]);
    }

  parseTelemFrskyByte(START_STOP);		// stop
}

static void frskyD_check_telemetry(uint8_t len)
{

  /*
  *  pkt 0 = length not counting appended status bytes
  *  pkt 1,2 = fixed_id
  *  pkt 3 = A1 : 52mV per count; 4.5V = 0x56
  *  pkt 4 = A2 : 13.4mV per count; 3.0V = 0xE3 on D6FR
  *  pkt 5 = RSSI
  *  pkt 6 = number of stream bytes
  *  pkt 7 = sequence number increments mod 32 when packet containing stream data acknowledged
  *  pkt 8-(8+(pkt[6]-1)) = stream data
  *  pkt len-2 = downlink RSSI
  *  pkt len-1 = crc status (bit7 set indicates good), link quality indicator (bits6-0)
  */

  // only process packets with the required id and packet length and valid crc
  if (!(packet_p2M[len-1] & 0x80)
      || packet_p2M[0] != len - 3
      || packet_p2M[1] != temp_rfid_addr_p2M[0]
      || packet_p2M[2] != temp_rfid_addr_p2M[1]) return;

  frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
  // frskyStreaming gets decremented every 10ms, FRSKY_TIMEOUT_FIRST value is detected to play connection prompt.

  telemetryData.rssi[0].set(packet_p2M[5] & 0x7f);

  telemetryData.rssi[1].set(packet_p2M[len-2] & 0x7f);

  //Get voltage A1 (52mv/count)
  telemetryData.analog[TELEM_ANA_A1].set(packet_p2M[3], g_model.telemetry.channels[TELEM_ANA_A1].type);

  //Get voltage A2 (~13.2mv/count) (Docs say 1/4 of A1)
  telemetryData.analog[TELEM_ANA_A2].set(packet_p2M[4], g_model.telemetry.channels[TELEM_ANA_A2].type);

  if(packet_p2M[6] && packet_p2M[6]<11)
    {
      if ((packet_p2M[7] & 0x1F) == (receive_seq_p2M & 0x1F)) // Is it the expected frame ?
        {
          uint8_t topBit = 0 ;
          if ((receive_seq_p2M & 0x80) && (receive_seq_p2M & 0x1F) != telem_save_seq_p2M)
            {
              topBit = 0x80 ;
            }
          receive_seq_p2M = ((receive_seq_p2M+1)%32) | topBit ;	// Request next telemetry frame

          uint8_t numbyte = 0;
          if(packet_p2M[6]>HUB_MAX_BYTES)
            {
              numbyte = packet_p2M[6] - HUB_MAX_BYTES;      // size of the second frame
              frskyD_send_HUB_telemetry(8, HUB_MAX_BYTES);
            }
          else
            {
              frskyD_send_HUB_telemetry(8, packet_p2M[6]);  // only 1 frame
            }

          if (numbyte)                                  // the the second frame
            {
              frskyD_send_HUB_telemetry(8+HUB_MAX_BYTES, numbyte);
            }
        }
      else
        {
          // incorrect sequence
          telem_save_seq_p2M = packet_p2M[7] & 0x1F;
          receive_seq_p2M |= 0x80;
        }
    }
}
#endif

static uint16_t FRSKYD_data_cb()
{
  uint8_t packet_count_and_3 = packet_count_p2M & 0x03;

  if(!start_tx_rx_p2M)
    {

      if(packet_count_and_3 == 0)
        {
          CC2500_SetTxRxMode(TX_EN);
          CC2500_Strobe(CC2500_SIDLE); // Force idle if still receiving in error condition.
        }
      else if(packet_count_and_3 == 3)
        {
          CC2500_SetTxRxMode(RX_EN);
        }

      if(packet_count_p2M & 0x1F)
        {
          CC2500_ManagePower();
          CC2500_ManageFreq();
        }

      CC2500_WriteReg(CC2500_0A_CHANNR, channel_used_p2M[packet_count_p2M %47]);
      //CC2500_WriteReg(CC2500_23_FSCAL3, 0x89) // Todo have a try
      start_tx_rx_p2M = 1;
      return 500 *2;
    }
  else
    {

      switch(packet_count_and_3)
        {

        case 0: // Tx data
        case 2: // Tx data
          SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
          FRSKYD_build_data_packet(); // 38.62us 16MHz AVR.
          CC2500_WriteData(packet_p2M, 18);
          break;

        case 1: // Tx data
          FRSKYD_build_data_packet(); // 38.62us 16MHz AVR.
          CC2500_WriteData(packet_p2M, 18);

          // Process previous telemetry packet
          uint8_t len;
          len = CC2500_ReadReg(CC2500_3B_RXBYTES | CC2500_READ_BURST);
          if((!len) || len > 0x14)
            break; // 20 bytes max
          CC2500_ReadData(packet_p2M, len);
#if defined(FRSKY)
          if (g_model.rfOptionBool1) // telemetry on?
            {
              frskyD_check_telemetry(len);
            }
#endif
          break;

        case 3: // Rx data
          CC2500_Strobe(CC2500_SFRX);
          CC2500_Strobe(CC2500_SRX);
          break;
        }

      packet_count_p2M ++;
      if(packet_count_p2M > 187)
        packet_count_p2M =0;
      start_tx_rx_p2M = 0;
      heartbeat |= HEART_TIMER_PULSES;
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      return 8500 *2;
    }
}

static void FRSKYD_initialize(uint8_t bind)
{
  freq_fine_mem_p2M = 0;
  receive_seq_p2M = 8;
  telem_save_seq_p2M = 0;

  loadrfidaddr_rxnum(0);
  FRSKY_generate_channels();
  CC2500_Reset(); // 0x30

  if(bind)
    {
      FRSKYD_init(1);
      CC2500_SetTxRxMode(TX_EN);
      PROTO_Start_Callback( FRSKYD_bind_cb);
    }
  else
    {
      FRSKYD_init(0);
      PROTO_Start_Callback( FRSKYD_data_cb);
    }
}

const void * FRSKYD_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
    {
    case PROTOCMD_INIT:
      FRSKYD_initialize(0);
      return 0;
    case PROTOCMD_BIND:
      bind_idx_p2M = 0;
      FRSKYD_initialize(1);
      return 0;
    case PROTOCMD_RESET:
      PROTO_Stop_Callback();
      CC2500_Reset();
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_FrskyD_Ser),
                          STR_DUMMY,       //Sub proto
                          STR_RFTUNEFINE,      //Option 1 (int)
                          STR_DUMMY,       //Option 2 (int)
                          STR_RFPOWER,    //Option 3 (uint 0 to 31)
                          STR_TELEMETRY,   //OptionBool 1
                          STR_DUMMY,       //OptionBool 2
                          STR_DUMMY        //OptionBool 3
                         );
      return 0;
    default:
      break;
    }
  return 0;
}
