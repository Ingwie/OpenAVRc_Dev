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

#define TX_ADDR temp_rfid_addr[2]
#define TX_CHANNEL temp_rfid_addr[3]

const static RfOptionSettingsvarstruct RfOpt_Skyartec_Ser[] PROGMEM = {
  /*rfProtoNeed*/PROTO_NEED_SPI, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/-128, // FREQFINE MIN
  /*rfOptionValue1Max*/127,  // FREQFINE MAX
  /*rfOptionValue2Min*/-128, // FREQCOARSE MIN
  /*rfOptionValue2Max*/127,  // FREQCOARSE MAX
  /*rfOptionValue3Max*/7,    // RF POWER
};


const static uint8_t ZZ_skyartecInitSequence[] PROGMEM = {
  CC2500_16_MCSM2, 0x07,
  CC2500_17_MCSM1, 0x30,// Switch in idle Skyartec_state after transmission
  CC2500_1E_WOREVT1, 0x87,
  CC2500_1F_WOREVT0, 0x6b,
  CC2500_20_WORCTRL, 0xf8,
  CC2500_2A_PTEST, 0x7f,
  CC2500_2B_AGCTEST, 0x3f,
  CC2500_0B_FSCTRL1, 0x09,
  CC2500_0C_FSCTRL0, 0x00,
  CC2500_0D_FREQ2, 0x5d,
  CC2500_0E_FREQ1, 0x93,
  CC2500_0F_FREQ0, 0xb1,
  CC2500_10_MDMCFG4, 0x2d,
  CC2500_11_MDMCFG3, 0x20,
  CC2500_12_MDMCFG2, 0x73,
  CC2500_13_MDMCFG1, 0x22,
  CC2500_14_MDMCFG0, 0xf8,
  CC2500_0A_CHANNR, 0xcd,
  CC2500_15_DEVIATN, 0x50,
  CC2500_21_FREND1, 0xb6,
  CC2500_22_FREND0, 0x10,
  CC2500_18_MCSM0, 0x18,
  CC2500_19_FOCCFG, 0x1d,
  CC2500_1A_BSCFG, 0x1c,
  CC2500_1B_AGCCTRL2, 0xc7,
  CC2500_1C_AGCCTRL1, 0x00,
  CC2500_1D_AGCCTRL0, 0xb2,
  CC2500_23_FSCAL3, 0xea,
  CC2500_24_FSCAL2, 0x0a,
  CC2500_25_FSCAL1, 0x00,
  CC2500_26_FSCAL0, 0x11,
  CC2500_29_FSTEST, 0x59,
  CC2500_2C_TEST2, 0x88,
  CC2500_2D_TEST1, 0x31,
  CC2500_2E_TEST0, 0x0b,
  CC2500_07_PKTCTRL1, 0x05,
  CC2500_08_PKTCTRL0, 0x05,
  CC2500_09_ADDR, 0x43,
  CC2500_06_PKTLEN, 0xff,
  CC2500_04_SYNC1, 0x13,
  CC2500_05_SYNC0, 0x18};


static void skyartec_init()
{
  uint_farptr_t pdata = pgm_get_far_address(ZZ_skyartecInitSequence);

  for (uint8_t i=0; i<(DIM(ZZ_skyartecInitSequence)/2); i++) {
    uint8_t add = pgm_read_byte_far(pdata);
    uint8_t dat = pgm_read_byte_far(++pdata);
    CC2500_WriteReg(add,dat);
    ++pdata;
  }

  CC2500_SetTxRxMode(TX_EN);
  CC2500_SetPower(TXPOWER_1);
  CC2500_Strobe(CC2500_SFTX);
  CC2500_Strobe(CC2500_SFRX);
  CC2500_Strobe(CC2500_SXOFF);
  CC2500_Strobe(CC2500_SIDLE);
}

static void add_pkt_suffix()
{
  uint8_t xor1 = 0;
  uint8_t xor2 = 0;
  for(uint8_t i = 3; i <= 16; i++)
    xor1 ^= packet[i];
  for(uint8_t i = 3; i <= 14; i++)
    xor2 ^= packet[i];

  uint8_t sum = packet[3] + packet[5] + packet[7] + packet[9] + packet[11] + packet[13];
  packet[17] = xor1;
  packet[18] = xor2;
  packet[19] = sum & 0xff;
}

static void Skyartec_send_data_packet()
{
  //13 c5 01 0259 0168 0000 0259 030c 021a 0489 f3 7e 0a

  packet[0] = 0x13;                //Length
  packet[1] = TX_ADDR;             //Tx Addr?
  packet[2] = 0x01;                //???

  // Each channel has a minimum of '0' and a maximum of 1280 (0x500).

  for(uint8_t i = 0; i < 7; i++) {

      int16_t value = channelOutputs[i];//* 0x280 / 0x500 + 0x280; // 0X500 = +125%
      value /= 2;
      //value = limit((int16_t)-640, value, (int16_t)+640);
      value += PPM_CH_CENTER(i) -  PPM_CENTER + 0x280; // Center + 640 (offset).

    packet[3+2*i] = value >> 8;
    packet[4+2*i] = value & 0xff;
  }
  add_pkt_suffix();

  CC2500_ManageFreq();
  CC2500_ManagePower();
  CC2500_WriteReg(CC2500_04_SYNC1, temp_rfid_addr[0]);
  CC2500_WriteReg(CC2500_05_SYNC0, temp_rfid_addr[1]);
  CC2500_WriteReg(CC2500_09_ADDR, TX_ADDR);
  CC2500_WriteReg(CC2500_0A_CHANNR, TX_CHANNEL);
  CC2500_WriteData(packet, packet[0]+1);
}

static void Skyartec_send_bind_packet()
{
  //0b 7d 01 01 b2 c5 4a 2f 00 00 c5 d6
  packet[0] = 0x0b;       //Length
  packet[1] = 0x7d;
  packet[2] = 0x01;
  packet[3] = 0x01;
  packet[4] = temp_rfid_addr[3];
  packet[5] = temp_rfid_addr[2];
  packet[6] = temp_rfid_addr[1];
  packet[7] = temp_rfid_addr[0];
  packet[8] = 0x00;
  packet[9] = 0x00;
  packet[10] = TX_ADDR;
  uint8_t bxor = 0;
  for(uint8_t i = 3; i < 11; i++)  bxor ^= packet[i];
  packet[11] = bxor;
  CC2500_ManageFreq();
  CC2500_WriteReg(CC2500_04_SYNC1, 0x7d);
  CC2500_WriteReg(CC2500_05_SYNC0, 0x7d);
  CC2500_WriteReg(CC2500_09_ADDR, 0x7d);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0x7d);
  CC2500_WriteData(packet, 12);
}

static uint16_t SKYARTEC_bind_cb()
{
  SCHEDULE_MIXER_END_IN_US(18000); // Schedule next Mixer calculations.
  Skyartec_send_bind_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 18000U *2;
}

static uint16_t SKYARTEC_cb()
{
  SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
  Skyartec_send_data_packet();
  heartbeat |= HEART_TIMER_PULSES;
  CALCULATE_LAT_JIT(); // Calculate latency and jitter.
  return 12000U *2;
}


static void SKYARTEC_initialize(uint8_t bind)
{
  freq_fine_mem = 0;

  loadrfidaddr_rxnum(2);
  CC2500_Reset();
  skyartec_init();
  if (bind) {
  PROTO_Start_Callback(25000U *2, SKYARTEC_bind_cb);
  } else {
  PROTO_Start_Callback(25000U *2, SKYARTEC_cb);
  }
}

const void *SKYARTEC_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    SKYARTEC_initialize(0);
    return 0;
  case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    CC2500_Reset();
    return 0;
  case PROTOCMD_BIND:
    SKYARTEC_initialize(1);
    return 0;
  case PROTOCMD_GETOPTIONS:
          SetRfOptionSettings(pgm_get_far_address(RfOpt_Skyartec_Ser),
                        STR_DUMMY,      //Sub proto
                        STR_RFTUNEFINE,      //Option 1 (int)
                        STR_RFTUNECOARSE,      //Option 2 (int)
                        STR_RFPOWER,    //Option 3 (uint 0 to 31)
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

