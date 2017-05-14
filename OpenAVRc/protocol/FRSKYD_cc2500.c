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
#include "frsky.h"

extern uint8_t frskyRxBuffer[];

static const char * const FRSKYD_opts[] = {
  _tr_noop("Freq-Fine"),  "-127", "+127", NULL,
  _tr_noop("Telemetry"),  _tr_noop("Off"), _tr_noop("On"), NULL,
  NULL
};

enum FRSKYD_opts {
  FRSKYD_OPT_FREQFINE =0,
  FRSKYD_OPT_TELEM,
  FRSKYD_OPT_LAST,
};
//ctassert(LAST_PROTO_OPT <= NUM_PROTO_OPTS, too_many_protocol_opts);


static uint8_t packet_number = 0;


static void FRSKYD_init(uint8_t bind)
{
  CC2500_SetTxRxMode(TX_EN);
  CC2500_WriteReg(CC2500_17_MCSM1, 0x00); // Go to idle after tx & rx.
//  CC2500_WriteReg(CC2500_17_MCSM1, 0x0C); // Stay in receive after packet reception, Idle state after transmission
  CC2500_WriteReg(CC2500_18_MCSM0, 0x18); // Auto calibrate when going from idle to tx/rx/fstxon

  CC2500_WriteReg(CC2500_06_PKTLEN, 0x19);
  CC2500_WriteReg(CC2500_07_PKTCTRL1, 0x05); // Address checking no broadcasts, Append status bytes to payload.
  CC2500_WriteReg(CC2500_08_PKTCTRL0, 0x05); // No data whitening, FIFO for tx/rx, CRC generation and checking,
  // Variable packet length mode set by first byte after sync word (0x11).
  CC2500_WriteReg(CC2500_3E_PATABLE, bind ? 0x50 : 0xFE);

  CC2500_WriteReg(CC2500_0B_FSCTRL1, 0x08);

  // static const s8 fine = 0; // Frsky rf deck = 0, Skyartec = -17.
  CC2500_WriteReg(CC2500_0C_FSCTRL0, (int8_t) 0); // TODO Model.proto_opts[PROTO_OPTS_FREQFINE]);
  CC2500_WriteReg(CC2500_0D_FREQ2, 0x5c);
  CC2500_WriteReg(CC2500_0E_FREQ1, 0x76); // V8 was 0x58
  CC2500_WriteReg(CC2500_0F_FREQ0, 0x27); // V8 was 0x9d
  CC2500_WriteReg(CC2500_10_MDMCFG4, 0xaa);
  CC2500_WriteReg(CC2500_11_MDMCFG3, 0x39); // 31044 baud.
  CC2500_WriteReg(CC2500_12_MDMCFG2, 0x11); // V8 was 0x93

  CC2500_WriteReg(CC2500_13_MDMCFG1, 0x23);
  CC2500_WriteReg(CC2500_14_MDMCFG0, 0x7a);
  CC2500_WriteReg(CC2500_15_DEVIATN, 0x42); // 0x41 (+-28564Hz) for 1way, 0x42 for 2way.

  CC2500_WriteReg(CC2500_19_FOCCFG, 0x16);
  CC2500_WriteReg(CC2500_1A_BSCFG, 0x6c);
  CC2500_WriteReg(CC2500_1B_AGCCTRL2, 0x03); // 0x03 for bind and normal.

  CC2500_WriteReg(CC2500_1C_AGCCTRL1, 0x40);
  CC2500_WriteReg(CC2500_1D_AGCCTRL0, 0x91);
  CC2500_WriteReg(CC2500_21_FREND1, 0x56);
  CC2500_WriteReg(CC2500_22_FREND0, 0x10);

  CC2500_WriteReg(CC2500_23_FSCAL3, 0xA9); // Enable charge pump calibration, calibrate for each hop.
  CC2500_WriteReg(CC2500_24_FSCAL2, 0x0a);
  CC2500_WriteReg(CC2500_25_FSCAL1, 0x00);
  CC2500_WriteReg(CC2500_26_FSCAL0, 0x11);
  CC2500_WriteReg(CC2500_29_FSTEST, 0x59);
  CC2500_WriteReg(CC2500_2C_TEST2, 0x88);
  CC2500_WriteReg(CC2500_2D_TEST1, 0x31);
  CC2500_WriteReg(CC2500_2E_TEST0, 0x0b);
  CC2500_WriteReg(CC2500_03_FIFOTHR, 0x07);
  CC2500_WriteReg(CC2500_09_ADDR, bind ? 0x03 : (frsky_id & 0xff));

  CC2500_Strobe(CC2500_SIDLE); // Go to idle...
  CC2500_Strobe(CC2500_SFTX); // 3b
  CC2500_Strobe(CC2500_SFRX); // 3a

  CC2500_WriteReg(CC2500_0A_CHANNR, 0x00);
}


static uint8_t get_chan_num(uint8_t idx)
{
  const uint8_t multiplier = 0x23;
  /*
  The multiplier appears to vary with Txid.
  e.g.
  Txid	multiplier
  0x0766	0x2d
  0x101c	0x9b
  0x1ee9	0x0a
  0x2dd7	0x1e
  0x25c2	0x23
  0xadac	0x7d
  */

  unsigned int ret = ((idx * multiplier) % 235) + channel_offset;
//   if(idx == 3 || idx == 23 ret++; rick
//   if(ret ==0x5a || ret == 0xdc) ret++; rick
  if(idx == 47) return 1; // rick
  if(idx > 47) return 0;
  return (uint8_t) ret;
}


static void FRSKYD_build_bind_packet()
{
  static uint8_t bind_idx =0;

  packet[0] = 0x11; //Length (17)
  packet[1] = 0x03; //Packet type
  packet[2] = 0x01; //Packet type
  packet[3] = frsky_id & 0xff;
  packet[4] = frsky_id >> 8;
  packet[5] = bind_idx *5; // Index into channels_used array.
  packet[6] =  channels_used[ (packet[5]) +0];
  packet[7] =  channels_used[ (packet[5]) +1];
  packet[8] =  channels_used[ (packet[5]) +2];
  packet[9] =  channels_used[ (packet[5]) +3];
  packet[10] = channels_used[ (packet[5]) +4];
  packet[11] = 0x00;
  packet[12] = 0x00;
  packet[13] = 0x00;
  packet[14] = 0x00;
  packet[15] = 0x00;
  packet[16] = 0x00;
  packet[17] = 0x01;

  bind_idx ++;
  if(bind_idx > 9) bind_idx = 0;
}


static void FRSKYD_build_data_packet()
{
  packet[0] = 0x11; // Length
  packet[1] = frsky_id & 0xff;
  packet[2] = frsky_id >> 8;
  packet[3] = packet_number;
#if HAS_EXTENDED_TELEMETRY
  packet[4] = sequence; // acknowledge last hub packet
#else
  packet[4] = 0x00;
#endif
  packet[5] = 0x01;
  // packet 6 to 9 contain LS byte of channels 1 to 4.
  packet[10] = 0; // Low nibble = channel 1, High nibble = channel 2.
  packet[11] = 0;
  // packet 12 to 15 contain LS byte of channels 5 to 8.
  packet[16] = 0;
  packet[17] = 0;

  uint8_t num_chan = 8 + (g_model.ppmNCH *2);
  if(num_chan > 8) num_chan = 8;

  for(uint8_t i = 0; i < 8; i++) {
    int16_t value;
    if(i < num_chan) {
      // 0x08CA / 1.5 = 1500 (us). Probably because they use 12MHz clocks.
      // 0x05DC -> 1000us 5ca
      // 0x0BB8 -> 2000us bca

      value = channelOutputs[i];
      value -= (value>>2); // x-x/4
      value = limit((int16_t)-(640 + (640>>1)), value, (int16_t)+(640 + (640>>1)));
      value += 0x08CA;
    } else value = 0x8C9;

    if(i < 4) {
      packet[6+i] = value & 0xff;
      packet[10+(i>>1)] |= ((value >> 8) & 0x0f) << (4 *(i & 0x01));
    } else {
      packet[8+i] = value & 0xff;
      packet[16+((i-4)>>1)] |= ((value >> 8) & 0x0f) << (4 * ((i-4) & 0x01));
    }
  }
}


static uint16_t FRSKYD_bind_cb()
{
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0);
  FRSKYD_build_bind_packet();
  CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO
  CC2500_WriteData(packet, 18);
  CC2500_Strobe(CC2500_STX); // Tx
  heartbeat |= HEART_TIMER_PULSES;
  dt = TCNT1 - OCR1A; // Calculate latency and jitter.
  return 18000U *2;
}


static uint16_t FRSKYD_data_cb()
{
  static uint8_t start_tx_rx = 0;
  static uint8_t len;
  uint8_t rx_packet[21]; // Down-link packet is 20 bytes.

    if(! start_tx_rx) {

      if((packet_number & 0x03) == 0) {
        CC2500_SetTxRxMode(TX_EN);
        CC2500_Strobe(CC2500_SIDLE); // Force idle if still receiving in error condition.
      } else if((packet_number & 0x03) == 3) {
        CC2500_SetTxRxMode(RX_EN);
      }

      if(packet_number & 0x1F) {
        CC2500_SetPower(TXPOWER_1); // TODO update power level.
        CC2500_WriteReg(CC2500_0C_FSCTRL0, (int8_t) 0); // TODO Update fine frequency value.
      }

      CC2500_WriteReg(CC2500_0A_CHANNR, channels_used[packet_number %47]);
      start_tx_rx =1;
      return 500 *2;
    } else {

      switch(packet_number & 0x03) {

      case 0: // Tx data
        FRSKYD_build_data_packet(); // 38.62us 16MHz AVR.
        CC2500_Strobe(CC2500_SFTX);
        CC2500_WriteData(packet, 18);
        CC2500_Strobe(CC2500_STX);
        break;

      case 1: // Tx data
        FRSKYD_build_data_packet(); // 38.62us 16MHz AVR.
        CC2500_Strobe(CC2500_SFTX);
        CC2500_WriteData(packet, 18);
        CC2500_Strobe(CC2500_STX);

        // Process previous telemetry packet
        len = CC2500_ReadReg(CC2500_3B_RXBYTES | CC2500_READ_BURST);
        if(len > 0x14) break; // 20 bytes
        CC2500_ReadData(rx_packet, len);

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

        // Packet checks: sensible length, good CRC, matching fixed id
        if(len != rx_packet[0] + 3 || rx_packet[0] < 5 || !(rx_packet[len-1] & 0x80)) break;
        else if(rx_packet[1] != (frsky_id & 0xff)) break;
        else if(rx_packet[2] != frsky_id >>8) break;
#if defined(FRSKY)
        memcpy(frskyRxBuffer, rx_packet, len);
        if(frskyStreaming < FRSKY_TIMEOUT10ms -5) frskyStreaming +=5;
        // frskyStreaming gets decremented every 10ms, however we can only add to it every 4 *9ms, so we add 5.
#endif
        break;

      case 2: // Tx data
        FRSKYD_build_data_packet(); // 38.62us 16MHz AVR.
        CC2500_Strobe(CC2500_SFTX);
        CC2500_WriteData(packet, 18);
        CC2500_Strobe(CC2500_STX);
        break;

      case 3: // Rx data
        CC2500_Strobe(CC2500_SFRX);
        CC2500_Strobe(CC2500_SRX);
        break;
      }

      packet_number ++;
      if(packet_number > 187) packet_number =0;
      start_tx_rx =0;
      heartbeat |= HEART_TIMER_PULSES;
      dt = TCNT1 - OCR1A; // Calculate latency and jitter.
      return 8500 *2;
    }
}

static void FRSKYD_initialize(uint8_t bind)
{
  CLOCK_StopTimer();

  frsky_id = SpiRFModule.fixed_id % 0x4000;

  // Build channel array.
  channel_offset = frsky_id % 5;
  for(uint8_t x=0; x<50; x++)	channels_used[x] = get_chan_num(x);

  CC2500_Reset(); // 0x30

  if(bind) {
    FRSKYD_init(1);
    PROTOCOL_SetBindState(0xFFFFFFFF);
    CLOCK_StartTimer(25000U *2, FRSKYD_bind_cb);
  } else {
    FRSKYD_init(0);
    FRSKYD_build_data_packet();
    // TELEMETRY_SetType(TELEM_FRSKY);
    CLOCK_StartTimer(25000U *2, FRSKYD_data_cb);
  }
}


const void * FRSKYD_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    FRSKYD_initialize(0);
    return 0;
  //case PROTOCMD_CHECK_AUTOBIND:
    //return 0; // Never Autobind
  case PROTOCMD_BIND:
    SpiRFModule.mode = BIND_MODE; // TODO
    FRSKYD_initialize(1);
    return 0;
  case PROTOCMD_RESET:
    CLOCK_StopTimer();
    return 0;
  //case PROTOCMD_NUMCHAN:
    //return (void *)8L;
  //case PROTOCMD_DEFAULT_NUMCHAN:
    //return (void *)8L;
//        case PROTOCMD_CURRENT_ID: return Model.fixed_id ? (void *)((unsigned long)Model.fixed_id) : 0;
  //case PROTOCMD_GETOPTIONS:
    //return FRSKYD_opts;
//        case PROTOCMD_TELEMETRYSTATE:
//            return (void *)(long)(Model.proto_opts[PROTO_OPTS_TELEM] == TELEM_ON ? PROTO_TELEM_ON : PROTO_TELEM_OFF);
//        case PROTOCMD_RESET:
//        case PROTOCMD_DEINIT:
//            CLOCK_StopTimer();
//            return (void *)(CC2500_Reset() ? 1L : -1L);
  default:
    break;
  }
  return 0;
}

