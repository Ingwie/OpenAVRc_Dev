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

const static int8_t RfOpt_FrskyV_Ser[] PROGMEM = {
/*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
/*rfSubTypeMax*/0,
/*rfOptionValue1Min*/-128,
/*rfOptionValue1Max*/127,
/*rfOptionValue2Min*/0,
/*rfOptionValue2Max*/0,
/*rfOptionValue3Max*/0,
};

static uint32_t seed;
static uint8_t dp_crc_init;

const static uint8_t ZZ_frskyVInitSequence[] PROGMEM = {
  CC2500_17_MCSM1, 0x0C, // Stay in receive after packet reception, Idle state after transmission
  CC2500_18_MCSM0, 0x18, // Auto calibrate when going from idle to tx/rx/fstxon
  CC2500_06_PKTLEN, 0xFF,
  CC2500_07_PKTCTRL1, 0x04,
  CC2500_08_PKTCTRL0, 0x05,
  CC2500_0B_FSCTRL1, 0x08,
  CC2500_0D_FREQ2, 0x5C,
  CC2500_0E_FREQ1, 0x58,
  CC2500_0F_FREQ0, 0x9D,
  CC2500_10_MDMCFG4, 0xAA,
  CC2500_11_MDMCFG3, 0x10, // 26977 baud.
  CC2500_12_MDMCFG2, 0x93,
  CC2500_13_MDMCFG1, 0x23,
  CC2500_14_MDMCFG0, 0x7A,
  CC2500_15_DEVIATN, 0x41, // 0x41 (+-28564Hz) for 1way, 0x42 for 2way.
  CC2500_19_FOCCFG, 0x16,
  CC2500_1A_BSCFG, 0x6C,
  CC2500_1B_AGCCTRL2, 0x43,
  CC2500_1C_AGCCTRL1, 0x40,
  CC2500_1D_AGCCTRL0, 0x91,
  CC2500_21_FREND1, 0x56,
  CC2500_22_FREND0, 0x10, // power index 0
  CC2500_23_FSCAL3, 0xA9, // Enable charge pump calibration, calibrate for each hop.
  CC2500_24_FSCAL2, 0x0A,
  CC2500_25_FSCAL1, 0x00,
  CC2500_26_FSCAL0, 0x11,
  CC2500_29_FSTEST, 0x59,
  CC2500_2C_TEST2, 0x88,
  CC2500_2D_TEST1, 0x31,
  CC2500_2E_TEST0, 0x0B,
  CC2500_03_FIFOTHR, 0x07,
  CC2500_09_ADDR, 0x00 // address 0
  };


static void FRSKYV_init(uint8_t bind)
{
  CC2500_Reset(); // 0x30

  uint_farptr_t pdata = pgm_get_far_address(ZZ_frskyVInitSequence);

  for (uint8_t i=0; i<(DIM(ZZ_frskyVInitSequence)/2); i++) { // Send init sequence.
    uint8_t add = pgm_read_byte_far(pdata);
    uint8_t dat = pgm_read_byte_far(++pdata);
    CC2500_WriteReg(add,dat);
    ++pdata;
  }


  CC2500_SetTxRxMode(TX_EN);
  CC2500_WriteReg(CC2500_0C_FSCTRL0, FREQFINE);
//  CC2500_Strobe(CC2500_SIDLE); // Go to idle...
  CC2500_Strobe(CC2500_SFTX); // 3b
  CC2500_Strobe(CC2500_SFRX); // 3a
  CC2500_SetPower(bind ? TXPOWER_1 : TXPOWER_1);
  CC2500_Strobe(CC2500_SIDLE); // Go to idle...

}

static uint8_t FRSKYV_crc8(uint8_t result, uint8_t *data, uint8_t len)
{
  for(uint8_t i = 0; i < len; i++) {
    result = result ^ data[i];
    for(uint8_t j = 0; j < 8; j++) {
      if(result & 0x80) result = (result << 1) ^ 0x07;
      else result = result << 1;
    }
  }
  return result;
}


static uint8_t FRSKYV_crc8_le(void)
{
/*
  How Tx Id relates to data packet initial crc value.
  ID      crc start value
  0x0000  0x0E
  0x0001  0xD7
  0x0002  0xBB
  0x0003  0x62
  0x1257  0xA6
  0x1258  0x7D
  0x1259  0xA4
  0x1E2D  0x89
  0x1E2E  0xE5
  0x1E2F  0x3C
  0x3210  0x1F
  0x3FFF  0x45
*/

  uint8_t result = 0xD6;

    result = result ^ (frsky_id >> 8);
    for(uint8_t j = 0; j < 8; j++) {
      if(result & 0x01) result = (result >> 1) ^ 0x83;
      else result = result >> 1;
    }

    result = result ^ (frsky_id & 0xFF);
    for(uint8_t j = 0; j < 8; j++) {
      if(result & 0x01) result = (result >> 1) ^ 0x83;
      else result = result >> 1;
    }

  return result;
}


static void FRSKYV_build_bind_packet()
{
/*
  Channels in bind packets for
  Tx Id 0x30DD
  0x00 = 0x0D,0x11,0x16,0x1B,0x20 // 0x0d -> 0x11 = step of 4.
  0x05 = 0x25,0x2A,0x2F,0x34,0x39
  0x0A = 0x3E,0x43,0x48,0x4D,0x52
  0x0F = 0x57,0x5C,0x61,0x66,0x6B
  0x14 = 0x70,0x75,0x7A,0x7F,0x84
  0x19 = 0x89,0x8E,0x93,0x98,0x9D
  0x1E = 0xA2,0xA7,0xAC,0xB1,0xB6
  0x23 = 0xBB,0xC0,0xC5,0xCA,0xCF
  0x28 = 0xD4,0xD9,0xDE,0xE3,0xE9
  0x2D = 0xED,0xF2,0xF7,0xFC,0x01
*/
// Highest observed channel is 0xFC.
// Steps between channels are almost always 5.

  static uint8_t bind_idx =0;

  packet[0] = 0x0E; //Length
  packet[1] = 0x03; //Packet type
  packet[2] = 0x01; //Packet type
  packet[3] = frsky_id & 0xFF;
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
  packet[14] = FRSKYV_crc8(0x93, packet, 14);

  ++bind_idx;
  if(bind_idx > 9) bind_idx = 0;
}


static void FRSKYV_build_data_packet()
{
  static uint8_t V_state =0;
  uint8_t ofsetChan = 0;

  packet[0] = 0x0E;
  packet[1] = frsky_id & 0xFF;
  packet[2] = frsky_id >> 8;
  packet[3] = seed & 0xFF;
  packet[4] = seed >> 8;

  // Appears to be a bitmap relating to the number of channels sent e.g.
  // 0x0F -> first 4 channels, 0x70 -> channels 5,6,7, 0xF0 -> channels 5,6,7,8
  if(V_state == 0 || V_state == 2) packet[5] = 0x0F;
  else if(V_state == 1 || V_state == 3) { ofsetChan = 4; packet[5] = 0xF0;}
  else packet[5] = 0x00;


  for(uint8_t i = 0; i < 4; i++) {
      // 0x08CA / 1.5 = 1500 (us). Probably because they use 12MHz clocks.
      // 0x05DC -> 1000us 5ca
      // 0x0BB8 -> 2000us bca

      int16_t value = channelOutputs[i + ofsetChan];
      value -= (value>>2); // x-x/4
      value = limit((int16_t)-(640 + (640>>1)), value, (int16_t)+(640 + (640>>1)));
      value += 0x08CA;

      packet[6 + (i*2)] = value & 0xFF;
      packet[7 + (i*2)] = (value >> 8) & 0xFF;
  }

  packet[14] = FRSKYV_crc8(dp_crc_init, packet, 14);
  ++V_state;
  if(V_state > 4) V_state =0;
  // Potentially if we had only four channels we could send them every 9ms.
}


static uint16_t FRSKYV_data_cb()
{
  SCHEDULE_MIXER_END(9*16); // Schedule next Mixer calculations.

  // Build next packet.
  seed = (uint32_t) (seed * 0xAA) % 0x7673; // Prime number 30323.
  FRSKYV_build_data_packet(); // 16MHz AVR = 127us.

  /* TODO Update options which don't need to be every 9ms. */
  static uint8_t option = 0;
  if(option == 0) CC2500_SetTxRxMode(TX_EN); // Keep Power Amp activated.
  else if(option == 64) CC2500_SetPower(TXPOWER_1); // TODO update power level.
  else if(option == 128) CC2500_WriteReg(CC2500_0C_FSCTRL0, FREQFINE);
  else if(option == 196) CC2500_Strobe(CC2500_SIDLE); // MCSM1 register setting puts CC2500 back into idle after TX.

  CC2500_WriteReg(CC2500_0A_CHANNR, channels_used[ ( (seed & 0xFF) % 50) ] ); // 16MHz AVR = 38us.
  CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO.
  CC2500_WriteData(packet, 15);
  CC2500_Strobe(CC2500_STX); // 8.853ms before we start again with the idle strobe.

  // Wait for transmit to finish. Timing is tight. Only 581uS between packet being emitted and idle strobe.
  // while( 0x0F != CC2500_Strobe(CC2500_SNOP)) { _delay_us(5); }
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 9000 *2;
}

static uint16_t FRSKYV_bind_cb()
{
  SCHEDULE_MIXER_END(18*16); // Schedule next Mixer calculations.
  FRSKYV_build_bind_packet();
  CC2500_Strobe(CC2500_SIDLE);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0);
  CC2500_Strobe(CC2500_SFTX); // Flush Tx FIFO
  CC2500_WriteData(packet, 15);
  CC2500_Strobe(CC2500_STX); // Tx
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}


static void FRSKYV_initialise(uint8_t bind)
{
  PROTO_Stop_Callback();

  frsky_id = SpiRFModule.fixed_id & 0x7FFF;

  // Build channel array.
  channel_offset = frsky_id % 5;
  uint8_t chan_num;
  for(uint8_t x = 0; x < 50; x ++) {
    chan_num = (x*5) + 3 + channel_offset;
	channels_used[x] = (chan_num ? chan_num : 1); // Avoid binding channel 0.
  }

  dp_crc_init = FRSKYV_crc8_le();

  if(bind) {
    FRSKYV_init(1);
    //PROTOCOL_SetBindState(0xFFFFFFFF);
    PROTO_Start_Callback(25000U *2, FRSKYV_bind_cb);
  } else {
    FRSKYV_init(0);
    seed = 2UL;
    FRSKYV_build_data_packet();
    PROTO_Start_Callback(25000U *2, FRSKYV_data_cb);
  }
}



const void * FRSKYV_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    FRSKYV_initialise(0);
    return 0;
//        case PROTOCMD_DEINIT:
//        case PROTOCMD_RESET:
//            PROTO_Stop_Callback();
//            return (void *)(CC2500_Reset() ? 1L : -1L);
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    CC2500_Reset();
    CC2500_SetTxRxMode(TXRX_OFF);
    CC2500_Strobe(CC2500_SIDLE);
    return 0;
  //case PROTOCMD_CHECK_AUTOBIND:
    //return 0; //Never Autobind.
  case PROTOCMD_BIND:
    FRSKYV_initialise(1);
    return 0;
  case PROTOCMD_GETOPTIONS:
          SetRfOptionSettings(pgm_get_far_address(RfOpt_FrskyV_Ser),
                        STR_DUMMY,       //Sub proto
                        STR_RFTUNE,      //Option 1 (int)
                        STR_DUMMY,       //Option 2 (int)
                        STR_DUMMY,       //Option 3 (uint 0 to 31)
                        STR_TELEMETRY,   //OptionBool 1
                        STR_DUMMY,       //OptionBool 2
                        STR_DUMMY        //OptionBool 3
                        );
    return 0;
  //case PROTOCMD_NUMCHAN:
    //return (void *)8L;
  //case PROTOCMD_DEFAULT_NUMCHAN:
    //return (void *)8L;
//        case PROTOCMD_CURRENT_ID: return Model.fixed_id ? (void *)((unsigned long)Model.fixed_id % 0x4000) : 0;
  //case PROTOCMD_TELEMETRYSTATE:
    //return (void *)(long) PROTO_TELEM_UNSUPPORTED;
  default:
    break;
  }
  return 0;
}


