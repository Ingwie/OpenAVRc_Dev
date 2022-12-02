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

// Deviation & Multiprotocol inspired. Thanks a lot !
#include "../OpenAVRc.h"

// define pulses2MHz reusable values (13 bytes max)
#define J6PRO_RFSTATE   BYTE_P2M(1)
//***********************************************//

const static RfOptionSettingsvar_t RfOpt_J6PRO_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/0,
  /*rfOptionValue1Min*/0,
  /*rfOptionValue1Max*/0,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/0,
  /*rfOptionValue3Max*/7,
};

#define J6PRO_NUM_WAIT_LOOPS (100 / 5) //each loop is ~5us.  Do not wait more than 100us
#define J6PRO_AUTOBIND (g_model.rfOptionBool1)


const uint8_t zzJ6PRO_data_code[] PROGMEM =
  { // Default Data Code.
  0x02, 0xf9, 0x93, 0x97, 0x02, 0xfa, 0x5c, 0xe3,
  0x01, 0x2b, 0xf1, 0xdb, 0x01, 0x32, 0xbe, 0x6f
  };

enum J6PRO_State
{
  J6PRO_BIND,
  J6PRO_BIND_01,
  J6PRO_BIND_03_START,
  J6PRO_BIND_03_CHECK,
  J6PRO_BIND_05_1,
  J6PRO_BIND_05_2,
  J6PRO_BIND_05_3,
  J6PRO_BIND_05_4,
  J6PRO_BIND_05_5,
  J6PRO_BIND_05_6,
//  J6PRO_BIND_05_7, // JF-M Transmitter sends 7 packets.
  J6PRO_CHANSEL,
  J6PRO_CHAN_1,
  J6PRO_CHAN_2,
  J6PRO_CHAN_3,
  J6PRO_CHAN_4,
};

void J6PRO_build_bind_packet()
{
  packet_p2M[0] = 0x01;  //Packet type
  packet_p2M[1] = 0x01;  //FIXME: What is this ? Model number maybe ?
  packet_p2M[2] = 0x56;  //FIXME: What is this ? Session Id ?
  packet_p2M[3] = temp_rfid_addr_p2M[0];
  packet_p2M[4] = temp_rfid_addr_p2M[1];
  packet_p2M[5] = temp_rfid_addr_p2M[2];
  packet_p2M[6] = temp_rfid_addr_p2M[3];
  packet_p2M[7] = RXNUM;
  packet_p2M[8] = 0xfe;
}

void J6PRO_build_data_packet()
{
#if defined(X_ANY)
  Xany_scheduleTx_AllInstance();
#endif

  packet_p2M[13] = 0;
  packet_p2M[14] = 0;
  packet_p2M[15] = 0;

  packet_p2M[0] = 0xaa; // FIXME what is this ?

  // conversion range 0 - 511.5 - 1023

  for(uint8_t upper=0; upper<3; upper++)
  {
    for(uint8_t lower=0; lower<4; lower++)
    {
      int16_t value = (FULL_CHANNEL_OUTPUTS((upper << 2) | lower)) /2;
      value = limit( (int16_t)-511, value, (int16_t) +511 );
      value += 511;
      packet_p2M[(upper << 2) | (lower +1) ] = value & 0xff; // Lower 8 bits in packet_p2M[1] to [12].
      packet_p2M[13 + upper] |= (value >> 8) << (lower * 2); // Upper 2 bits in packet_p2M[13] to [15].
    }
  }
}

const uint8_t zz_J6PROInitSequence[] PROGMEM =
{
  /* Initialise CYRF chip */
  CYRF_28_CLK_EN, 0x02,          // RXF, force receive clock enable
  CYRF_32_AUTO_CAL_TIME, 0x3c,   // AUTO_CAL_TIME = 3Ch, typical configuration
  CYRF_35_AUTOCAL_OFFSET, 0x14,  // AUTO_CAL_OFFSET = 14h, typical configuration
  CYRF_1C_TX_OFFSET_MSB, 0x05,   // STRIM MSB = 0x05, typical configuration
  CYRF_1B_TX_OFFSET_LSB, 0x55,   // STRIM LSB = 0x55, typical configuration
  CYRF_0F_XACT_CFG, 0x24,        // No Transaction mode, Force Idle State, ACK TO (don't care).
  CYRF_03_TX_CFG, 0x28,          // Data Code Length = 64 chip codes + Data Mode = 8DR Mode.
  CYRF_06_RX_CFG, 0x4a,          // LNA + FAST TURN EN + RXOW EN, enable low noise amplifier, fast turning, overwrite enable.
  CYRF_12_DATA64_THOLD, 0x0e,    // TH64 = 0Eh, set pn correlation threshold
  CYRF_10_FRAMING_CFG, 0xee,     // SOP EN + SOP LEN = 64 chips + LEN EN + SOP TH = 0Eh
  CYRF_1F_TX_OVERRIDE, 0x00,     // Reset TX overrides
  CYRF_1E_RX_OVERRIDE, 0x00,     // Reset RX overrides
};

void J6PRO_cyrf_init()
{
  // Initialise CYRF chip.
  uint_farptr_t pdata = pgm_get_far_address(zz_J6PROInitSequence);
  uint8_t add;
  uint8_t dat;

  for (uint8_t i = 0; i < (DIM(zz_J6PROInitSequence) / 2); i++)
  {
    add = pgm_read_byte_far(pdata++);
    dat = pgm_read_byte_far(pdata++);
    CYRF_WriteRegister(add, dat);
  }
}

void J6PRO_cyrf_bindinit()
{
  /* Use when binding */
  /* As binding does not use "Listen Before Talk", Use 10mW maximum power output. */
#if (CYRF6936PA_GAIN >= 20 )
  CYRF_SetPower(TXPOWER_4);
#else
  CYRF_SetPower(TXPOWER_7); // No PA.
#endif
  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(19); // Bind SOP code.
  CYRF_ConfigCRCSeed(0x0000);
  J6PRO_build_bind_packet();
}

void J6PRO_cyrf_datainit()
{
  /* Use when already bound */
  uint8_t sopidx =
      (0xff & (temp_rfid_addr_p2M[0] + temp_rfid_addr_p2M[1] + temp_rfid_addr_p2M[2] + temp_rfid_addr_p2M[3] - 0xfe)) % 19;
  uint16_t crc =
      (0xff & (temp_rfid_addr_p2M[1] - RXNUM + 0xfe)) |
      ((0xff & (temp_rfid_addr_p2M[2] + temp_rfid_addr_p2M[3] - RXNUM + 0xfe)) << 8);

  CYRF_PROGMEM_Config_DEVO_J6PRO_sopcodes(sopidx);
  CYRF_ConfigCRCSeed(crc);
}

void J6PRO_set_radio_channels()
{
  //FIXME: Query free channels
  //lowest channel is 0x08, upper channel is 0x4d?
  CYRF_FindBestChannels(channel_used_p2M, 3, 5, 8, 77);
  channel_used_p2M[3] = channel_used_p2M[0];
}

uint16_t J6PRO_cb()
{
  heartbeat |= HEART_TIMER_PULSES;

  switch (J6PRO_RFSTATE)
  {
    case J6PRO_BIND:
    J6PRO_cyrf_bindinit();
    J6PRO_RFSTATE = J6PRO_BIND_01;
    /* FALLTHROUGH */
    //no break because we want to send the 1st bind packet now
    case J6PRO_BIND_01:
    CYRF_ConfigRFChannel(0x52);
    CYRF_SetTxRxMode(TX_EN);
    CYRF_WriteDataPacketLen(packet_p2M, 0x09); // 2ms for packet to egress.
    J6PRO_RFSTATE = J6PRO_BIND_03_START;
    return 3000*2; // 3msec
    case J6PRO_BIND_03_START:
    {
      uint8_t i = J6PRO_NUM_WAIT_LOOPS;

      while(i--)
      {
        if((CYRF_ReadRegister(CYRF_02_TX_CTRL) & 0x80) == 0x00)
        {
          (void) CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS); // For debugging.
          break;
        }
      }
    }
    CYRF_ConfigRFChannel(0x53);
    CYRF_SetTxRxMode(RX_EN);
    CYRF_WriteRegister(CYRF_07_RX_IRQ_STATUS, 0x80); // Clear RXOW IRQ if set.
    (void) CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS);
    CYRF_WriteRegister(CYRF_05_RX_CTRL, 0x82); // Prepare to receive
    J6PRO_RFSTATE = J6PRO_BIND_03_CHECK;
    return 30000U*2; // 30msec

    case J6PRO_BIND_03_CHECK:
    {
      uint8_t rx = CYRF_ReadRegister(CYRF_05_RX_CTRL);
      if((rx & 0x80) == 0x00)
      {
        rx = CYRF_ReadRegister(CYRF_0A_RX_LENGTH);
        if(rx == 0x0f)
        {
          rx = CYRF_ReadRegister(CYRF_09_RX_COUNT);
          if(rx == 0x0f)
          {
            //Expected and actual length are both 15
            CYRF_ReadDataPacketLen(packet_p2M, rx);
            if (packet_p2M[0] == 0x03 &&
                packet_p2M[3] == temp_rfid_addr_p2M[0] &&
                packet_p2M[4] == temp_rfid_addr_p2M[1] &&
                packet_p2M[5] == temp_rfid_addr_p2M[2] &&
                packet_p2M[6] == temp_rfid_addr_p2M[3] &&
                packet_p2M[7] == RXNUM &&
                packet_p2M[8] == 0xfe)
            {
              /* Bind done */
              packet_p2M[0] = 0x05;
              (void) CYRF_ReadRegister(CYRF_07_RX_IRQ_STATUS); // For debugging.
              CYRF_ConfigRFChannel(0x54);
              CYRF_SetTxRxMode(TX_EN);
              J6PRO_RFSTATE = J6PRO_BIND_05_1;
              return 2000*2; // 2msec
            }
          }
        }
      }
      J6PRO_RFSTATE = J6PRO_BIND_01;
      return 500*2;
    }
    case J6PRO_BIND_05_1:
    case J6PRO_BIND_05_2:
    case J6PRO_BIND_05_3:
    case J6PRO_BIND_05_4:
    case J6PRO_BIND_05_5:
    case J6PRO_BIND_05_6:
//    case J6PRO_BIND_05_7: // JF-M Transmitter sends 7 packets.
    CYRF_WriteDataPacketLen(packet_p2M, 0x0f);
    (void) CYRF_ReadRegister(CYRF_04_TX_IRQ_STATUS); // For debugging.
    ++J6PRO_RFSTATE;
    return 30000U*2; // was 4.6msec
    case J6PRO_CHANSEL:
    CYRF_SetTxRxMode(TX_EN);
    PROTOCOL_SetBindState(0);
    J6PRO_cyrf_datainit();
    J6PRO_RFSTATE = J6PRO_CHAN_1;
    /* FALLTHROUGH */
    case J6PRO_CHAN_1:
    //Keep transmit power updated
    CYRF_ManagePower(); // Keep transmit power in sync
    J6PRO_build_data_packet();
    SCHEDULE_MIXER_END_IN_US(24550); // Schedule next Mixer calculations.
    /* FALLTHROUGH */
    //return 3400;
    case J6PRO_CHAN_2:
    //return 3500;
    case J6PRO_CHAN_3:
    //return 3750
    case J6PRO_CHAN_4:
    CYRF_ConfigRFChannel(channel_used_p2M[J6PRO_RFSTATE - J6PRO_CHAN_1]);
    CYRF_WriteDataPacket(packet_p2M); // Longer data packet takes 2.7ms to egress.
    if (J6PRO_RFSTATE == J6PRO_CHAN_4)
    {
      J6PRO_RFSTATE = J6PRO_CHAN_1;
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      return 13900*2;
    }
    ++J6PRO_RFSTATE;
    CALCULATE_LAT_JIT(); // Calculate latency and jitter.
    return 4550*2; // was 3.55ms
  }
  return 0;
}

void J6PRO_Init(uint8_t bind)
{
  CYRF_Reset();

  // Load temp_rfid_addr_p2M.
  loadrfidaddr();

  J6PRO_cyrf_init();
  uint_farptr_t dataadr = pgm_get_far_address(zzJ6PRO_data_code);
  uint8_t codedata[16];
  for(uint8_t i = 0; i<16; i++)
    codedata[i] = pgm_read_byte_far(dataadr++);

  CYRF_ConfigDataCode(codedata, 16);
  CYRF_WritePreamble(0x333302); // Default Preamble 0x3333, Count 0x02.
  J6PRO_set_radio_channels();

  if (bind || J6PRO_AUTOBIND)
  {
    J6PRO_RFSTATE = J6PRO_BIND;
    PROTOCOL_SetBindState(1200); // 12 Sec
  }
  else
  {
    J6PRO_RFSTATE = J6PRO_CHANSEL;
  }
  PROTO_Start_Callback(J6PRO_cb);
}


const void *J6PRO_Cmds(enum ProtoCmds cmd)
{
  switch (cmd)
  {
  case PROTOCMD_INIT:
    J6PRO_Init(0);
    return 0;
  case PROTOCMD_RESET:
    CYRF_Reset();
    PROTO_Stop_Callback();
    return 0;
  case PROTOCMD_BIND:
    J6PRO_Init(1);
    return 0;
  case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_J6PRO_Ser),
                        STR_DUMMY,      //Sub proto
                        STR_DUMMY,      //Option 1 (int)
                        STR_DUMMY,      //Option 2 (int)
                        STR_RFPOWER,    //Option 3 (uint 0 to 31)
                        STR_AUTOBIND,   //OptionBool 1
                        STR_DUMMY,      //OptionBool 2
                        STR_DUMMY       //OptionBool 3
                       );
    return 0;

  default:
    break;
  }
  return 0;
}
