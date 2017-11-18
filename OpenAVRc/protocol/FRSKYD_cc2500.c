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

const static int8_t RfOpt_FrskyD_Ser[] PROGMEM = {
/*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
/*rfSubTypeMax*/0,
/*rfOptionValue1Min*/-128,
/*rfOptionValue1Max*/127,
/*rfOptionValue2Min*/0,
/*rfOptionValue2Max*/0,
/*rfOptionValue3Max*/0,
};

static uint8_t packet_number = 0;

const static uint8_t ZZ_frskyDInitSequence[] PROGMEM = {
  CC2500_17_MCSM1, 0x00, // Go to idle after tx & rx.
  CC2500_18_MCSM0, 0x18, // Auto calibrate when going from idle to tx/rx/fstxon
  CC2500_06_PKTLEN, 0x19,
  CC2500_07_PKTCTRL1, 0x05, // Address checking no broadcasts, Append status bytes to payload.
  CC2500_08_PKTCTRL0, 0x05, // No data whitening, FIFO for tx/rx, CRC generation and checking,
  // Variable packet length mode set by first byte after sync word (0x11).
  CC2500_0B_FSCTRL1, 0x08,
  CC2500_0D_FREQ2, 0x5c,
  CC2500_0E_FREQ1, 0x76, // V8 was 0x58
  CC2500_0F_FREQ0, 0x27, // V8 was 0x9d
  CC2500_10_MDMCFG4, 0xaa,
  CC2500_11_MDMCFG3, 0x39, // 31044 baud.
  CC2500_12_MDMCFG2, 0x11, // V8 was 0x93
  CC2500_13_MDMCFG1, 0x23,
  CC2500_14_MDMCFG0, 0x7a,
  CC2500_15_DEVIATN, 0x42, // 0x41 (+-28564Hz) for 1way, 0x42 for 2way.
  CC2500_19_FOCCFG, 0x16,
  CC2500_1A_BSCFG, 0x6c,
  CC2500_1B_AGCCTRL2, 0x03, // 0x03 for bind and normal.
  CC2500_1C_AGCCTRL1, 0x40,
  CC2500_1D_AGCCTRL0, 0x91,
  CC2500_21_FREND1, 0x56,
  CC2500_22_FREND0, 0x10,
  CC2500_23_FSCAL3, 0xA9, // Enable charge pump calibration, calibrate for each hop.
  CC2500_24_FSCAL2, 0x0a,
  CC2500_25_FSCAL1, 0x00,
  CC2500_26_FSCAL0, 0x11,
  CC2500_29_FSTEST, 0x59,
  CC2500_2C_TEST2, 0x88,
  CC2500_2D_TEST1, 0x31,
  CC2500_2E_TEST0, 0x0b,
  CC2500_03_FIFOTHR, 0x07};


static void FRSKYD_init(uint8_t bind)
{
  CC2500_Reset(); // 0x30

  uint_farptr_t pdata = pgm_get_far_address(ZZ_frskyDInitSequence);

  for (uint8_t i=0; i<(DIM(ZZ_frskyDInitSequence)/2); i++) { // Send init
    uint8_t add = pgm_read_byte_far(pdata);
    uint8_t dat = pgm_read_byte_far(++pdata);
    CC2500_WriteReg(add,dat);
    ++pdata;
  }

  CC2500_WriteReg(CC2500_0C_FSCTRL0, FREQFINE);
  CC2500_WriteReg(CC2500_09_ADDR, bind ? 0x03 : (frsky_id & 0xff));

//  CC2500_Strobe(CC2500_SIDLE); // Go to idle...
  CC2500_Strobe(CC2500_SFTX); // 3b
  CC2500_Strobe(CC2500_SFRX); // 3a
  CC2500_SetPower(bind ? TXPOWER_1 : TXPOWER_1);

  CC2500_WriteReg(CC2500_0A_CHANNR, 0x00);
  CC2500_Strobe(CC2500_SIDLE); // Go to idle...
}

void FRSKYD_generate_channels(void)
{
/*
 * Make sure adjacent channels in the array are spread across the band and are not repeated.
 */

  uint8_t chan_offset = ((SpiRFModule.fixed_id >> 16) & 0xFF) % 10; // 10 channel bases.
  uint8_t step = (((SpiRFModule.fixed_id >> 24) & 0xFF) % 11); // 11 sequences for now.

  step = step + 73; // 73 to 83.
  // Build channel array.
  for(uint8_t idx =0; idx <50; idx++) {
    uint16_t res = ((step * idx) + chan_offset) % 236; // 235 is the highest channel used.

    if(res == 0) res = 161; // Avoid binding channel 0.
    if(res == 1) res = 80; // Channel 1 probably indicates end of sequence in bind packet.
    if(idx == 47) res = 1; // Unused but sent to rx in bind packet, may indicate end of sequence.
    if(idx > 47) res = 0; // Unused but sent to rx in bind packet.
    channels_used[idx] = res;
   }
}

static void FRSKYD_build_bind_packet()
{
  static uint8_t bind_idx = 0;

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

  ++bind_idx;
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

  uint8_t num_chan = 8 + (g_model.PPMNCH *2); //TODO Why ?? use num_chan ??
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
  SCHEDULE_MIXER_END(18*16); // Schedule next Mixer calculations.
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0);
  FRSKYD_build_bind_packet();
  CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO
  CC2500_WriteData(packet, 18);
  CC2500_Strobe(CC2500_STX); // Tx
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}


static uint16_t FRSKYD_data_cb()
{
  static uint8_t start_tx_rx = 0;
  static uint8_t len;
  uint8_t rx_packet[21]; // Down-link packet is 20 bytes.

  SCHEDULE_MIXER_END(8.5*16); // Schedule next Mixer calculations.

    if(! start_tx_rx) {

      if((packet_number & 0x03) == 0) {
        CC2500_SetTxRxMode(TX_EN);
        CC2500_Strobe(CC2500_SIDLE); // Force idle if still receiving in error condition.
      } else if((packet_number & 0x03) == 3) {
        CC2500_SetTxRxMode(RX_EN);
      }

      if(packet_number & 0x1F) {
        CC2500_SetPower(TXPOWER_1); // TODO update power level.
        CC2500_WriteReg(CC2500_0C_FSCTRL0, FREQFINE);
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
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      return 8500 *2;
    }
}

static void FRSKYD_initialize(uint8_t bind)
{
  PROTO_Stop_Callback();

  frsky_id = SpiRFModule.fixed_id;// % 0x4000;
  FRSKYD_generate_channels();

  CC2500_Reset(); // 0x30

  if(bind) {
    FRSKYD_init(1);
    //PROTOCOL_SetBindState(0xFFFFFFFF);
    PROTO_Start_Callback(25000U *2, FRSKYD_bind_cb);
  } else {
    FRSKYD_init(0);
    FRSKYD_build_data_packet();
    // TELEMETRY_SetType(TELEM_FRSKY);
    PROTO_Start_Callback(25000U *2, FRSKYD_data_cb);
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
    FRSKYD_initialize(1);
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    CC2500_Reset();
    CC2500_SetTxRxMode(TXRX_OFF);
    CC2500_Strobe(CC2500_SIDLE);
    return 0;
    case PROTOCMD_GETOPTIONS:
          SetRfOptionSettings(pgm_get_far_address(RfOpt_FrskyD_Ser),
                        STR_DUMMY,       //Sub proto
                        STR_RFTUNE,      //Option 1 (int)
                        STR_DUMMY,       //Option 2 (int)
                        STR_DUMMY,       //Option 3 (uint 0 to 31)
                        STR_TELEMETRY,   //OptionBool 1
                        STR_DUMMY,       //OptionBool 2
                        STR_DUMMY        //OptionBool 3
                        );
    return 0;
	//case PROTOCMD_GETNUMOPTIONS:
    //return (void *)1L;
  //case PROTOCMD_DEFAULT_NUMCHAN:
    //return (void *)8L;
//        case PROTOCMD_CURRENT_ID: return Model.fixed_id ? (void *)((unsigned long)Model.fixed_id) : 0;

//        case PROTOCMD_TELEMETRYSTATE:
//            return (void *)(long)(Model.proto_opts[PROTO_OPTS_TELEM] == TELEM_ON ? PROTO_TELEM_ON : PROTO_TELEM_OFF);
  default:
    break;
  }
  return 0;
}

