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


static uint8_t FRSKYD_state;
static uint32_t fixed_id;
static uint16_t bind_count;
#define TX_ADDR ((fixed_id >> 16) & 0xff)
#define TX_CHANNEL ((fixed_id >> 24) & 0xff)

enum {
  SKYARTEC_PKT1 = 0,
  SKYARTEC_PKT2,
  SKYARTEC_PKT3,
  SKYARTEC_PKT4,
  SKYARTEC_PKT5,
  SKYARTEC_PKT6
};

static void skyartec_init()
{
  CC2500_Reset();

  CC2500_WriteReg(CC2500_16_MCSM2, 0x07);
  CC2500_WriteReg(CC2500_17_MCSM1, 0x30);// Switch in idle FRSKYD_state after transmission
  CC2500_WriteReg(CC2500_1E_WOREVT1, 0x87);
  CC2500_WriteReg(CC2500_1F_WOREVT0, 0x6b);
  CC2500_WriteReg(CC2500_20_WORCTRL, 0xf8);
  CC2500_WriteReg(CC2500_2A_PTEST, 0x7f);
  CC2500_WriteReg(CC2500_2B_AGCTEST, 0x3f);
  CC2500_WriteReg(CC2500_0B_FSCTRL1, 0x09);
  CC2500_WriteReg(CC2500_0C_FSCTRL0, 0x00);
  CC2500_WriteReg(CC2500_0D_FREQ2, 0x5d);
  CC2500_WriteReg(CC2500_0E_FREQ1, 0x93);
  CC2500_WriteReg(CC2500_0F_FREQ0, 0xb1);
  CC2500_WriteReg(CC2500_10_MDMCFG4, 0x2d);
  CC2500_WriteReg(CC2500_11_MDMCFG3, 0x20);
  CC2500_WriteReg(CC2500_12_MDMCFG2, 0x73);
  CC2500_WriteReg(CC2500_13_MDMCFG1, 0x22);
  CC2500_WriteReg(CC2500_14_MDMCFG0, 0xf8);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0xcd);
  CC2500_WriteReg(CC2500_15_DEVIATN, 0x50);
  CC2500_WriteReg(CC2500_21_FREND1, 0xb6);
  CC2500_WriteReg(CC2500_22_FREND0, 0x10);
  CC2500_WriteReg(CC2500_18_MCSM0, 0x18);
  CC2500_WriteReg(CC2500_19_FOCCFG, 0x1d);
  CC2500_WriteReg(CC2500_1A_BSCFG, 0x1c);
  CC2500_WriteReg(CC2500_1B_AGCCTRL2, 0xc7);
  CC2500_WriteReg(CC2500_1C_AGCCTRL1, 0x00);
  CC2500_WriteReg(CC2500_1D_AGCCTRL0, 0xb2);
  CC2500_WriteReg(CC2500_23_FSCAL3, 0xea);
  CC2500_WriteReg(CC2500_24_FSCAL2, 0x0a);
  CC2500_WriteReg(CC2500_25_FSCAL1, 0x00);
  CC2500_WriteReg(CC2500_26_FSCAL0, 0x11);
  CC2500_WriteReg(CC2500_29_FSTEST, 0x59);
  CC2500_WriteReg(CC2500_2C_TEST2, 0x88);
  CC2500_WriteReg(CC2500_2D_TEST1, 0x31);
  CC2500_WriteReg(CC2500_2E_TEST0, 0x0b);
  CC2500_WriteReg(CC2500_07_PKTCTRL1, 0x05);
  CC2500_WriteReg(CC2500_08_PKTCTRL0, 0x05);
  CC2500_WriteReg(CC2500_09_ADDR, 0x43);
  CC2500_WriteReg(CC2500_06_PKTLEN, 0xff);
  CC2500_WriteReg(CC2500_04_SYNC1, 0x13);
  CC2500_WriteReg(CC2500_05_SYNC0, 0x18);
  CC2500_SetTxRxMode(TX_EN);
  CC2500_SetPower(TXPOWER_6);
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

static void send_data_packet()
{
  //13 c5 01 0259 0168 0000 0259 030c 021a 0489 f3 7e 0a

  NONATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
  packet[0] = 0x13;                //Length
  packet[1] = TX_ADDR;             //Tx Addr?
  packet[2] = 0x01;                //???

  // Each channel has a minimum of '0' and a maximum of 1280 (0x500).

  uint8_t num_chan = 8 + (g_model.ppmNCH *2);
  if(num_chan > 7) num_chan = 7;

  int16_t value;

  for(uint8_t i = 0; i < 7; i++) {
    if(i < num_chan) {
      value = channelOutputs[i];
      value /= 2;
      value = limit((int16_t)-640, value, (int16_t)+640);
      value += 0x280; // 640.
    } else {
      value = 0x280;
    }
    packet[3+2*i] = value >> 8;
    packet[4+2*i] = value & 0xff;
  }
  add_pkt_suffix();
  //for(uint16_t i = 0; i < 20; i++) printf("%02x ", packet[i]); printf("\n");
  CC2500_WriteReg(CC2500_04_SYNC1, ((fixed_id >> 0) & 0xff));
  CC2500_WriteReg(CC2500_05_SYNC0, ((fixed_id >> 8) & 0xff));
  CC2500_WriteReg(CC2500_09_ADDR, TX_ADDR);
  CC2500_WriteReg(CC2500_0A_CHANNR, TX_CHANNEL);
  CC2500_Strobe(CC2500_SFTX);
  CC2500_WriteData(packet, 20);
  CC2500_Strobe(CC2500_STX);
  }
}

static void send_bind_packet()
{
  //0b 7d 01 01 b2 c5 4a 2f 00 00 c5 d6
  packet[0] = 0x0b;       //Length
  packet[1] = 0x7d;
  packet[2] = 0x01;
  packet[3] = 0x01;
  packet[4] = (fixed_id >> 24) & 0xff;
  packet[5] = (fixed_id >> 16) & 0xff;
  packet[6] = (fixed_id >> 8)  & 0xff;
  packet[7] = (fixed_id >> 0)  & 0xff;
  packet[8] = 0x00;
  packet[9] = 0x00;
  packet[10] = TX_ADDR;
  uint8_t bxor = 0;
  for(uint8_t i = 3; i < 11; i++)  bxor ^= packet[i];
  packet[11] = bxor;
  CC2500_WriteReg(CC2500_04_SYNC1, 0x7d);
  CC2500_WriteReg(CC2500_05_SYNC0, 0x7d);
  CC2500_WriteReg(CC2500_09_ADDR, 0x7d);
  CC2500_WriteReg(CC2500_0A_CHANNR, 0x7d);
  CC2500_Strobe(CC2500_SFTX);
  CC2500_WriteData(packet, 12);
  CC2500_Strobe(CC2500_STX);
}


static uint16_t skyartec_cb()
{
  heartbeat |= HEART_TIMER_PULSES;
  // SCHEDULE_MIXER_END((uint16_t) (6 *2) *8); // Todo

  if (FRSKYD_state != SKYARTEC_PKT6) {
    send_data_packet();
    FRSKYD_state++;
  } else {
    if (--bind_count) send_bind_packet();
    FRSKYD_state = SKYARTEC_PKT1;
  }
  dt = TCNT1 - OCR1A; // Calculate latency and jitter.
  return 12000 *2;
}


static void skyartec_initialize()
{
  CLOCK_StopTimer();
  skyartec_init();
  fixed_id = 0xb2c54a2f;
// if (Model.fixed_id) {
//   fixed_id ^= Model.fixed_id + (Model.fixed_id << 16);
// } else {
  uint32_t partnum = CC2500_ReadReg(0xF0);
  uint32_t vernum = CC2500_ReadReg(0xF1);
  fixed_id ^= partnum << 24;
  fixed_id ^= vernum << 16;
  fixed_id ^= (vernum << 4 | partnum >> 4) << 8;
  fixed_id ^= (partnum << 4 | vernum >> 4) << 8;
// }
  if (0 == (fixed_id & 0xff000000))
    fixed_id |= 0xb2;
  if (0 == (fixed_id & 0x00ff0000))
    fixed_id |= 0xc5;
  if (0 == (fixed_id & 0x0000ff00))
    fixed_id |= 0x4a;
  if (0 == (fixed_id & 0x000000ff))
    fixed_id |= 0x2f;
  bind_count = 10000;
  FRSKYD_state = SKYARTEC_PKT1;

  CLOCK_StartTimer(25000U *2, &skyartec_cb);
}

const void *SKYARTEC_Cmds(enum ProtoCmds cmd)
{
  switch(cmd) {
  case PROTOCMD_INIT:
    skyartec_initialize();
    return 0;
  case PROTOCMD_DEINIT:
  case PROTOCMD_RESET:
    CLOCK_StopTimer();
    //return (void *)(CC2500_Reset() ? 1L : -1L);
    return (void *) 1L;
  case PROTOCMD_CHECK_AUTOBIND:
    return (void *)1L; // Always Autobind
  case PROTOCMD_BIND:
    skyartec_initialize();
    return 0;
  case PROTOCMD_NUMCHAN:
    return (void *)7L;
  case PROTOCMD_DEFAULT_NUMCHAN:
    return (void *)7L;
//  case PROTOCMD_CURRENT_ID:
//    return Model.fixed_id ? (void *)((unsigned long)Model.fixed_id) : 0;
  case PROTOCMD_TELEMETRYSTATE:
    return (void *)(long)PROTO_TELEM_UNSUPPORTED;
  default:
    break;
  }
  return 0;
}

