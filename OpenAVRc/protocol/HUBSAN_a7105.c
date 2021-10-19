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
 *             Multiprotocol TX Module -                                  *
 *   https://github.com/pascallanger/DIY-Multiprotocol-TX-Module          *
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

/* Channel Order :
 * Ch1 A
 * Ch2 E
 * Ch3 T
 * Ch4 R
 * Ch5 Flight Modes H123D (-100% Sport Mode 1, 0% Sport Mode 2, +100% Acro).
 * Logical Switch L1 Light
 * Logical Switch L2 Video
 * Logical Switch L3 Picture
 * Logical Switch L4 Flip
 * Logical Switch L5 Headless
 * Logical Switch L6 RTH
 * Logical Switch L7 Stab
 * Logical Switch L8 GPS Hold
 * Logical Switch L9 Altitude Hold
 */

#include "../OpenAVRc.h"
// Memory Allocation.
//U8
#define hubsan_phase_p2M              pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-1]
#define hubsan_bind_phase_p2M         pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-2]
#define hubsan_handshake_counter_p2M  pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-3]

#define hubsan_vtx_freq_p2M           pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-6]
#define hubsan_h501_packet_p2M        pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-7]
#define hubsan_txState_p2M            pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-8]
#define hubsan_rfMode_p2M             pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-9]
#define hubsan_packet_count_p2M       pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-11]
#define hubsan_channel_num_p2M        pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-13]

uint32_t hubsan_id_data;

//#define VTX_STEP_SIZE "10"

const static RfOptionSettingsvar_t RfOpt_HUBSAN_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/2,
  /*rfOptionValue1Min*/-127,
  /*rfOptionValue1Max*/127,
  /*rfOptionValue2Min*/64, // 5640MHz
  /*rfOptionValue2Max*/94, // 5945MHz 30 Channels 10MHz steps.
  /*rfOptionValue3Max*/7,
};

const pm_char STR_SUBTYPE_HUBSAN_SPI[] PROGMEM = "H107""H301""H501";

enum Hubsan_sub_protocols
{
  H107 = 0, H301 = 1, H501 = 2,
};

enum
{
  HUBSAN_AILERON = 0, HUBSAN_ELEVATOR, HUBSAN_THROTTLE, HUBSAN_RUDDER,
};

enum
{
  // flags going to packet[9] (H107)
  HUBSAN_FLAG_VIDEO = 0x01,  // record video
  HUBSAN_FLAG_FLIP = 0x08,   // enable flips
  HUBSAN_FLAG_LED = 0x04     // enable LEDs
};

enum
{
  // flags going to packet[9] (H107 Plus series)
  HUBSAN_FLAG_HEADLESS = 0x08, // headless mode
};

enum
{
  // flags going to packet[9] (H301)
  FLAG_H301_VIDEO = 0x01,
  FLAG_H301_STAB = 0x02,
  FLAG_H301_LED = 0x10,
  FLAG_H301_RTH = 0x40,
};

enum
{
  // flags going to packet[13] (H107 Plus series)
  HUBSAN_FLAG_SNAPSHOT = 0x01,
  HUBSAN_FLAG_FLIP_PLUS = 0x80,
};

enum
{
  // flags going to packet[9] (H501S)
  FLAG_H501_VIDEO = 0x01,
  FLAG_H501_LED = 0x04,
  FLAG_H122D_FLIP = 0x08, //H122D
  FLAG_H501_RTH = 0x20,
  FLAG_H501_HEADLESS1 = 0x40,
  FLAG_H501_GPS_HOLD = 0x80,
};

enum
{
  // flags going to packet[11] (H122D & H123D)
  FLAG_H123D_FMODES = 0x03, //H123D 3 FMODES: Sport mode 1, Sport mode 2, Acro
  FLAG_H122D_OSD = 0x20, //H122D OSD
};

enum
{
  // flags going to packet[13] (H501S)
  FLAG_H501_SNAPSHOT = 0x01,
  FLAG_H501_HEADLESS2 = 0x02,
  FLAG_H501_ALT_HOLD = 0x08,
};

enum
{
  BIND_1 =1,
  BIND_2,
  BIND_3,
  BIND_4,
  BIND_5,
  BIND_6,
  BIND_7,
  BIND_8,
  DATA_1,
  DATA_2,
  DATA_3,
  DATA_4,
  DATA_5,
};

#define HUBSAN_WAIT_WRITE 0x80
#define HUBSAN_AUTOBIND (g_model.rfOptionBool1)
#define HUBSAN_VTX_FREQ (g_model.rfOptionValue2)

void hubsan_update_crc()
{
  uint8_t sum = 0;
  for (uint8_t i = 0; i < 15; i++)
    sum += packet_p2M[i];
  packet_p2M[15] = (~sum) +1;
}

static void hubsan_build_bind_packet(uint8_t bind_state)
{
  if (hubsan_phase_p2M < BIND_7)
    hubsan_handshake_counter_p2M = 0;

  memset(packet_p2M, 0, 16);
  packet_p2M[0] = bind_state;
  packet_p2M[1] = hubsan_channel_num_p2M;
  packet_p2M[2] = temp_rfid_addr_p2M[3];
  packet_p2M[3] = temp_rfid_addr_p2M[2];
  packet_p2M[4] = temp_rfid_addr_p2M[1];
  packet_p2M[5] = temp_rfid_addr_p2M[0];

  if (hubsan_id_data == ID_NORMAL && g_model.rfSubType != H501)
  {
    packet_p2M[6] = 0x08;
    packet_p2M[7] = 0xe4;
    packet_p2M[8] = 0xea;
    packet_p2M[9] = 0x9e;
    packet_p2M[10] = 0x50;
    //const uint32_t txid = 0xdb042679;
    packet_p2M[11] = 0xDB;
    packet_p2M[12] = 0x04;
    packet_p2M[13] = 0x26;
    packet_p2M[14] = 0x79;
  }
  else
  { // ID_PLUS
    if (hubsan_phase_p2M >= BIND_3)
    {
      packet_p2M[7] = 0x62;
      packet_p2M[8] = 0x16;
    }
    if(hubsan_phase_p2M == BIND_7)
      packet_p2M[2] = hubsan_handshake_counter_p2M ++;
  }

  hubsan_update_crc();
}


static void hubsan_build_packet()
{


  memset(packet_p2M, 0, 16);
  if (hubsan_vtx_freq_p2M != HUBSAN_VTX_FREQ || hubsan_packet_count_p2M == 100) // set vTX frequency (H107D)
  {
    hubsan_vtx_freq_p2M = HUBSAN_VTX_FREQ;
    packet_p2M[0] = 0x40; // VTX data packet
    packet_p2M[1] = (hubsan_vtx_freq_p2M / 10) + 50;
    packet_p2M[2] = (hubsan_vtx_freq_p2M % 10)+ 10;// 5640 - 5940 MHz
    packet_p2M[3] = 0x82;
    hubsan_packet_count_p2M ++;
  }
  else // 20 00 00 00 80 00 7d 00 84 02 64 db 04 26 79 7b
  {
    // ii : aileron   observed range: 0x45 - 0xc3 (smaller is right)69-195-50%
    // gg : elevator  observed range: 0x3e - 0xbc (smaller is up)62-188 -50%
    // cc : throttle  observed range: 0x00 - 0xFF (smaller is down)
    // ee : rudder    observed range: 0x34 - 0xcc (smaller is right)52-204-60%
    packet_p2M[0] = 0x20; // normal data packet
    packet_p2M[2] = convert_channel_8b(HUBSAN_THROTTLE); // Throttle
  }

  packet_p2M[4] = convert_channel_8b(HUBSAN_RUDDER); // Rudder
  packet_p2M[6] = 0xFF - convert_channel_8b(HUBSAN_ELEVATOR);// Elevator is reversed
  packet_p2M[8] = 0xFF - convert_channel_8b(HUBSAN_AILERON);// Aileron is reversed

  if(hubsan_id_data == ID_NORMAL && g_model.rfSubType == H107)
  { // H107/L/C/D, H102D
    if( hubsan_packet_count_p2M < 100)
    {
      packet_p2M[9] = 0x02 | HUBSAN_FLAG_LED | HUBSAN_FLAG_FLIP; // Sends default value for the 100 first packets
      hubsan_packet_count_p2M ++;
    }
    else
    {
      packet_p2M[9] = 0x02;
      if(getLogicalSwitch(4))
      packet_p2M[9] |= HUBSAN_FLAG_FLIP;
      if(getLogicalSwitch(1))
      packet_p2M[9] |= HUBSAN_FLAG_LED;
      if(getLogicalSwitch(2))
      packet_p2M[9] |= HUBSAN_FLAG_VIDEO; // H102D
    }
    packet_p2M[10] = 0x64;
    //const uint32_t txid = 0xdb042679;
    packet_p2M[11] = 0xDB;
    packet_p2M[12] = 0x04;
    packet_p2M[13] = 0x26;
    packet_p2M[14] = 0x79;
  }
  else if(g_model.rfSubType == H301)
  { // H301
    if( hubsan_packet_count_p2M < 100)
    {
      packet_p2M[9] = FLAG_H301_STAB; // sends default value for the 100 first packets
      hubsan_packet_count_p2M ++;
    }
    else
    {
      if(getLogicalSwitch(6))
      packet_p2M[9] |= FLAG_H301_RTH;
      if(getLogicalSwitch(1))
      packet_p2M[9] |= FLAG_H301_LED;
      if(getLogicalSwitch(7))
      packet_p2M[9] |= FLAG_H301_STAB;
      if(getLogicalSwitch(2))
      packet_p2M[9] |= FLAG_H301_VIDEO;
    }
    packet_p2M[10] = 0x18; // ?
    packet_p2M[12] = 0x5c;// ?
    packet_p2M[14] = 0xf6;// ?
  }
  else
  { // ID_PLUS && H501
    packet_p2M[3] = g_model.rfSubType == H501 ? 0x00:0x64;
    packet_p2M[5] = g_model.rfSubType == H501 ? 0x00:0x64;
    packet_p2M[7] = g_model.rfSubType == H501 ? 0x00:0x64;

    if(g_model.rfSubType == H501)
    { // H501S
      packet_p2M[9] = 0x02;
      if(getLogicalSwitch(6))
      packet_p2M[9] |= FLAG_H501_RTH;
      if(getLogicalSwitch(1))
      packet_p2M[9] |= FLAG_H501_LED;
      if(getLogicalSwitch(2))
      packet_p2M[9] |= FLAG_H501_VIDEO;
      if(getLogicalSwitch(5))
      packet_p2M[9] |= FLAG_H501_HEADLESS1;
      if(getLogicalSwitch(8))
      packet_p2M[9] |= FLAG_H501_GPS_HOLD;
      if(getLogicalSwitch(4))
      packet_p2M[9] |= FLAG_H122D_FLIP;// H122D specific -> flip

      // packet[10]= 0x1A;
      // packet[11] content 0x00 is default

      // H123D specific -> Flight modes
      packet_p2M[11] = 0x41;// Sport mode 1
  //    if(Channel_data[CH5]>CHANNEL_MAX_COMMAND)
      packet_p2M[11]=0x43;// Acro
 //     else if(Channel_data[CH5]>CHANNEL_MIN_COMMAND)
      packet_p2M[11]=0x42;// Sport mode 2

      // H122D specific -> OSD but useless...
      // packet[11]|= 0x80
      // | GET_FLAG(CHXX_SW,FLAG_H122D_OSD);

      if(getLogicalSwitch(5))
      packet_p2M[13] |= FLAG_H501_HEADLESS2;
      if(getLogicalSwitch(9))
      packet_p2M[13] |= FLAG_H501_ALT_HOLD;
      if(getLogicalSwitch(3))
      packet_p2M[13] |= FLAG_H501_SNAPSHOT;
    }
    else
    { // H107P/C+/D+
      packet_p2M[9] = 0x06;
      // FLIP|LIGHT|PICTURE|VIDEO|HEADLESS
      if(getLogicalSwitch(2))
      packet_p2M[9] |= HUBSAN_FLAG_VIDEO;
      if(getLogicalSwitch(5))
      packet_p2M[9] |= HUBSAN_FLAG_HEADLESS;

      packet_p2M[10]= 0x19;
      packet_p2M[12]= 0x5C;// ghost channel ?
      packet_p2M[13] = 0;
      if(getLogicalSwitch(4))
      packet_p2M[13] |= HUBSAN_FLAG_FLIP_PLUS;
      if(getLogicalSwitch(3))
      packet_p2M[13] |= HUBSAN_FLAG_SNAPSHOT;

      packet_p2M[14]= 0x49;// ghost channel ?
    }

    if(hubsan_packet_count_p2M < 100)
    { // set channels to neutral for first 100 packets.
      packet_p2M[2] = 0x80; // throttle neutral is at mid stick on plus series
      packet_p2M[4] = 0x80;
      packet_p2M[6] = 0x80;
      packet_p2M[8] = 0x80;
      packet_p2M[9] = 0x06;
      packet_p2M[13]= 0x00;
      hubsan_packet_count_p2M ++;
    }

    if(g_model.rfSubType == H501)
    { // H501S
      hubsan_h501_packet_p2M ++;
      if(hubsan_h501_packet_p2M == 10)
      {
        memset(packet_p2M, 0, 16);
        packet_p2M[0] = 0xe8;
      }
      else if(hubsan_h501_packet_p2M == 20)
      {
        memset(packet_p2M, 0, 16);
        packet_p2M[0] = 0xe9;
      }
      if(hubsan_h501_packet_p2M >= 20) hubsan_h501_packet_p2M = 0;
    }
  }
  hubsan_update_crc();
}

#ifdef FRSKYxx
uint8_t hubsan_check_integrity()
{
  if( (packet_p2M[0] & 0xFE) != 0xE0 )
    return 0;
  uint8_t sum = 0;
  for(uint8_t i = 0; i < 15; i++)
    sum += packet_p2M[i];
  return  packet_p2M[15] == (~sum) +1;
}
#endif


uint16_t HUBSAN_cb()
{
  heartbeat |= HEART_TIMER_PULSES;
#ifdef FRSKYxx
  //static uint8_t rfMode = 0;
#endif

  uint16_t delay;
  uint8_t i;

  A7105_AdjustLOBaseFreq();

  switch (hubsan_phase_p2M)
  {
    case BIND_1:
    hubsan_bind_phase_p2M ++;
    if(hubsan_bind_phase_p2M >= 20 && g_model.rfSubType != H501) // deviation ==20, multi >=20
    {
      if(hubsan_id_data == ID_NORMAL)
        hubsan_id_data = ID_PLUS;
      else
        hubsan_id_data = ID_NORMAL;
        A7105_WriteID(hubsan_id_data);
        hubsan_bind_phase_p2M = 0;
    }
    case BIND_3:
    case BIND_5:
    case BIND_7:
    hubsan_build_bind_packet(hubsan_phase_p2M == BIND_7 ? 9 : (hubsan_phase_p2M == BIND_5 ? 1 : hubsan_phase_p2M));
    A7105_Strobe(A7105_STANDBY);
    A7105_WriteData(16, hubsan_channel_num_p2M);
    hubsan_phase_p2M |= HUBSAN_WAIT_WRITE;
    return 3000 *2;

    case BIND_1 | HUBSAN_WAIT_WRITE:
    case BIND_3 | HUBSAN_WAIT_WRITE:
    case BIND_5 | HUBSAN_WAIT_WRITE:
    case BIND_7 | HUBSAN_WAIT_WRITE:
    // wait for completion
    for(i = 0; i< 20; i++)
    if(! (A7105_ReadReg(A7105_00_MODE) & 0x01))
      break;
    A7105_SetTxRxMode(RX_EN);
    A7105_Strobe(A7105_RX);
    hubsan_phase_p2M &= ~HUBSAN_WAIT_WRITE;

    if(hubsan_id_data == ID_PLUS)
    {
      if(hubsan_phase_p2M == BIND_7 && packet_p2M[2] == 9)
      {
        hubsan_phase_p2M = DATA_1;
        A7105_WriteReg(A7105_1F_CODE_I, 0x0F);
        PROTOCOL_SetBindState(0);
        return 4500 *2;
      }
    }
    hubsan_phase_p2M ++;
    return 4500 *2; //7.5msec elapsed since last write


    case BIND_2:
    case BIND_4:
    case BIND_6:

    A7105_SetTxRxMode(TX_EN);
    if(A7105_ReadReg(A7105_00_MODE) & 0x01)
    {
      hubsan_phase_p2M = BIND_1;
      return 4500*2; // No signal, restart binding procedure.  12msec elapsed since last write
    }

    A7105_ReadData(16);
    hubsan_phase_p2M ++;

    if (hubsan_phase_p2M == BIND_5)
    A7105_WriteID(((uint32_t)packet_p2M[2] << 24) | ((uint32_t)packet_p2M[3] << 16) | ((uint32_t)packet_p2M[4] << 8) | packet_p2M[5]);
    return 500*2;  // 8 msec elapsed time since last write;

    case BIND_8:
    A7105_SetTxRxMode(TX_EN);
    if(A7105_ReadReg(A7105_00_MODE) & 0x01)
    {
      hubsan_phase_p2M = BIND_7;
      return 15000 *2; //22.5msec elapsed since last write
    }

    A7105_ReadData(16);
    if(packet_p2M[1] == 9 && hubsan_id_data == ID_NORMAL)
    {
      hubsan_phase_p2M = DATA_1;
      A7105_WriteReg(A7105_1F_CODE_I, 0x0F);
      PROTOCOL_SetBindState(0); // BIND_DONE;
      return 28000U *2; // 35.5msec elapsed since last write
    }
    else
    {
      hubsan_phase_p2M = BIND_7;
      return 15000 *2; //22.5 msec elapsed since last write
    }


    case DATA_1:
    case DATA_2:
    case DATA_3:
    case DATA_4:
    case DATA_5:

    if(hubsan_txState_p2M == 0)
    { // send packet
#ifdef MULTI_SYNC
      telemetry_set_input_sync(10000);
#endif
#ifdef FRSKYxx
      hubsan_rfMode_p2M = A7105_TX;
#endif
      if( hubsan_phase_p2M == DATA_1)
        A7105_ManagePower(); //Keep transmit power in sync

      hubsan_build_packet();
      A7105_Strobe(A7105_STANDBY);
      uint8_t ch;
      if((hubsan_phase_p2M == DATA_5 && hubsan_id_data == ID_NORMAL) && g_model.rfSubType == H107)
        ch = hubsan_channel_num_p2M + 0x23;
      else
        ch = hubsan_channel_num_p2M;

      A7105_WriteData(16, ch);

      if (hubsan_phase_p2M == DATA_5)
        hubsan_phase_p2M = DATA_1;
      else
        hubsan_phase_p2M ++;
      delay = 3000;
    }
    else
    {
#ifdef FRSKYxx
      if(hubsan_rfMode_p2M == A7105_TX)
      { // switch to rx mode 3ms after packet sent
        for( i=0; i<10; i++)
        {
          if( !(A7105_ReadReg(A7105_00_MODE) & 0x01))
          { // wait for tx completion
            A7105_SetTxRxMode(RX_EN);
            A7105_Strobe(A7105_RX);
            hubsan_rfMode_p2M = A7105_RX;
            break;
          }
        }
      }
      if(hubsan_rfMode_p2M == A7105_RX)
      { // check for telemetry frame
        for( i=0; i<10; i++)
        {
          if( !(A7105_ReadReg(A7105_00_MODE) & 0x01))
          { // data received
            A7105_ReadData(16);
            if(hubsan_check_integrity())
            {
              // Hubsan Lipo voltage, 8 bits real value *10. e.g. (0x2A =42 -> 4.2V).
              // Set Frsky range to 13.1V
              telemetryData.analog[TELEM_ANA_A1].set(packet_p2M[13]*2, g_model.telemetry.channels[TELEM_ANA_A1].type);
              // telemetry_link =1;
              frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
            }
            A7105_Strobe(A7105_RX);
            // Read TX RSSI
            int16_t temp=256-(A7105_ReadReg(A7105_1D_RSSI_THOLD)*8)/5; // Value from A7105 is between 8 for maximum signal strength to 160 or less.
            if(temp<0) temp=0;
            else if(temp>255) temp=255;
            temp = temp / 2;
            telemetryData.rssi[1].set((uint8_t) temp);
            telemetryData.rssi[0].set(packet_p2M[13]); // Shows raw Lipo voltage.
            break;
          }
        }
      }
#endif
      delay = 1000;
    }
    if (++hubsan_txState_p2M == 8)
    { // 3ms + 7*1ms
      SCHEDULE_MIXER_END_IN_US(10000U); // Schedule next Mixer calculations. Soon as possible
      A7105_SetTxRxMode(TX_EN);
      hubsan_txState_p2M = 0;
    }
    CALCULATE_LAT_JIT(); // Calculate latency and jitter.
    return delay *2;
  }
  return 0;
}


void HUBSAN_initialise(uint8_t bind)
{
  const uint8_t allowed_ch[] = //progmem todo
      { 0x14, 0x1e, 0x28, 0x32, 0x3c, 0x46, 0x50, 0x5a, 0x64, 0x6e, 0x78, 0x82 };

  A7105_Init();

  loadrfidaddr_rxnum(0);

  uint32_t ida = ((uint32_t) temp_rfid_addr_p2M[3] << 24)
      | ((uint32_t) temp_rfid_addr_p2M[2] << 16)
      | ((uint32_t) temp_rfid_addr_p2M[1] << 8)
      | temp_rfid_addr_p2M[0];

  hubsan_channel_num_p2M = allowed_ch[ ida % sizeof(allowed_ch)];
  hubsan_id_data = ID_NORMAL;

  if (HUBSAN_AUTOBIND || g_model.rfSubType == H107)
  {
    PROTOCOL_SetBindState(1200); // 12 Sec // BIND_IN_PROGRESS;
    hubsan_phase_p2M = BIND_1;
    hubsan_bind_phase_p2M = 0;
  }
  else
  {
    hubsan_phase_p2M = DATA_1;
    A7105_WriteID(ida);
    A7105_WriteReg(A7105_1F_CODE_I, 0x0F);
  }
  hubsan_packet_count_p2M = 0;

  hubsan_vtx_freq_p2M = 0;
  hubsan_h501_packet_p2M = 0;
  hubsan_txState_p2M = 0;
  hubsan_rfMode_p2M =0;

  A7105_ManagePower(); // Keep transmit power in sync
  PROTO_Start_Callback(HUBSAN_cb);
}


const void* HUBSAN_Cmds(enum ProtoCmds cmd)
{
  switch (cmd)
  {
  case PROTOCMD_INIT:
    HUBSAN_initialise(0);
    return 0;
  case PROTOCMD_RESET:
    A7105_Reset();
    PROTO_Stop_Callback();
    return 0;
  case PROTOCMD_BIND:
    HUBSAN_initialise(1);
    return 0;
  case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_HUBSAN_Ser),
        STR_SUBTYPE_HUBSAN_SPI, // Sub proto
        STR_RFTUNEFINE,    // Option 1 (int)
        STR_MULTI_VIDFREQ, // Option 2 (int)
        STR_RFPOWER,       // Option 3 (uint 0 to 31)
        STR_AUTOBIND,      // OptionBool 1
        STR_DUMMY,         // OptionBool 2
        STR_DUMMY          // OptionBool 3
        );
    return 0;
  default:
    break;
  }
  return 0;
}
