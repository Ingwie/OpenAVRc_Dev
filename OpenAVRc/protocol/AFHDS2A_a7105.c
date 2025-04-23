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

// define pulses2MHz data. Undef at the end of this file.

#define packet                    pulses2MHz.spi.afhds2a.packet_p2M
#define hopping_array             pulses2MHz.spi.afhds2a.channel_used_p2M
#define phase                     pulses2MHz.spi.afhds2a.afhds2a_RF_STATE_P2M
#define hopping_index             pulses2MHz.spi.afhds2a.afhds2a_CH_IDX_P2M
#define wait_transmit             pulses2MHz.spi.afhds2a.afhds2a_REC_SEQ_P2M
#define bind4_phase               pulses2MHz.spi.afhds2a.afhds2a_BIND_IDX_P2M
#define bind_count                pulses2MHz.spi.afhds2a.afhds2a_PACKET_COUNT_P2M
#define packet_type               pulses2MHz.spi.afhds2a.packet_type
#define packet_counter            pulses2MHz.spi.afhds2a.RF_STATE16_P2M
#define temp_rfid_addr            pulses2MHz.spi.afhds2a.temp_rfid_addr_p2M

//***********************************************//

const static RfOptionSettingsvar_t RfOpt_AFHDS2A_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI | BOOL1USED, //can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/3,
  /*rfOptionValue1Min*/-127,
  /*rfOptionValue1Max*/127,
  /*rfOptionValue2Min*/0,
  /*rfOptionValue2Max*/70,
  /*rfOptionValue3Max*/7,
};

const pm_char STR_SUBTYPE_AFHDS2A_SPI[] PROGMEM = "IBPW""IBPP""SBPW""SBPP";

#define AFHDS2A_RX_ID          seed
#define AFHDS2A_RX_ID_STORAGE  (g_model.points[NUM_POINTS-5]) // Dirty : Use curves points to store RX ID (4 bytes)
#define AFHDS2A_NUM_WAIT_LOOPS (700 / 15) // Each loop is ~15us.  Do not wait more than 700us
#define AFHDS2A_TXPACKET_SIZE  38
#define AFHDS2A_RXPACKET_SIZE  37
#define AFHDS2A_NUMFREQ        16
#define AFHDS2A_WAIT_WRITE     0x80

enum AFHDS2A_sub
{
  PWM_IBUS,
  PPM_IBUS,
  PWM_SBUS,
  PPM_SBUS,
};

enum
{
  AFHDS2A_PACKET_STICKS,
  AFHDS2A_PACKET_SETTINGS,
  AFHDS2A_PACKET_FAILSAFE,
};

enum
{
  AFHDS2A_BIND1,
  AFHDS2A_BIND2,
  AFHDS2A_BIND3,
  AFHDS2A_BIND4,
  AFHDS2A_DATA_INIT,
  AFHDS2A_DATA,
};



static void AFHDS2A_calc_channels()
{
  uint8_t idx = 0;
  uint32_t rnd = g_eeGeneral.fixed_ID.ID_32;
  while (idx < AFHDS2A_NUMFREQ)
  {
    uint8_t i;
    uint8_t band_no = ((((idx << 1) | ((idx >> 1) & 0b01)) + temp_rfid_addr[0]) & 0b11);
    rnd = rnd * 0x0019660D + 0x3C6EF35F; // Randomization

    uint8_t next_ch = band_no * 41 + 1 + ((rnd >> idx) % 41); // Channel range: 1..164

    for (i = 0; i < idx; i++)
    {
      // Keep the distance 5 between the channels
      uint8_t distance;
      if (next_ch > hopping_array[i]) distance = next_ch - hopping_array[i];
      else distance = hopping_array[i] - next_ch;

      if (distance < 5) break;
    }

    if (i != idx) continue;

    hopping_array[idx++] = next_ch;
  }
}


static void AFHDS2A_build_bind_packet()
{
  uint8_t ch;
  memcpy(&packet[1], temp_rfid_addr, 4);
  memset(&packet[5], 0xff, 4);
  packet[10] = 0x00;
  for (ch = 0; ch < AFHDS2A_NUMFREQ; ch++)
  {
    packet[11 + ch] = hopping_array[ch];
  }
  memset(&packet[27], 0xff, 10);
  packet[37] = 0x00;
  switch (phase)
  {
    case AFHDS2A_BIND1:
      packet[0] = 0xbb;
      packet[9] = 0x01;
      break;
    case AFHDS2A_BIND2:
    case AFHDS2A_BIND3:
    case AFHDS2A_BIND4:
      packet[0] = 0xbc;
      if (phase == AFHDS2A_BIND4)
      {
        memcpy(&packet[5], &AFHDS2A_RX_ID, 4);
        memset(&packet[11], 0xff, 16);
      }
      packet[9] = phase - 1;
      if (packet[9] > 0x02)
        packet[9] = 0x02;
      packet[27] = 0x01;
      packet[28] = 0x80;
      break;
  }
}

static void AFHDS2A_build_packet(uint8_t type)
{
  memcpy(&packet[1], temp_rfid_addr, 4);
  memcpy(&packet[5], &AFHDS2A_RX_ID, 4);

  switch (type)
  {
    case AFHDS2A_PACKET_STICKS:
      packet[0] = 0x58;
      // 16 channels + RX_LQI on channel 17
      for (uint8_t ch = 0; ch < 16; ch++)
      {
        int16_t value = (FULL_CHANNEL_OUTPUTS(ch)) / 2; // +-1280 to +-640
        value += PPM_CENTER; // + 1500 offset
        value = limit((int16_t) - 860, value, (int16_t) + 2140);

        if(ch < 14)
        {
        packet[9 + ch * 2] = value & 0xFF;
        packet[10 + ch * 2] = (value >> 8) & 0xFF;
        }
        else
        {
//          if(ch == 16)  //CH17=RX_LQI
//            val = 2000 - 10*RX_LQI;
          packet[10 + (ch-14)*6] |= (value) << 4;
          packet[12 + (ch-14)*6] |= (value) & 0xF0;
          packet[14 + (ch-14)*6] |= (value>>4) & 0xF0;
        }
      }
      {
        uint8_t next_hop = (hopping_index +1)&0x0F;
        packet[34] |= next_hop << 4;
        packet[36] |= next_hop ? 0x80 : 0x90;
      }
      break;


    case AFHDS2A_PACKET_FAILSAFE:
      packet[0] = 0x56;
      for (uint8_t ch = 0; ch < 16; ch++)
      { // Failsafe values
        uint16_t val;

#ifdef FAILSAFE_ENABLE
  val = Failsafe_data[protocol==PROTO_AFHDS2A?CH_AETR[ch]:ch];  // No remapping for BS receivers
  if(val!=FAILSAFE_CHANNEL_HOLD && val!=FAILSAFE_CHANNEL_NOPULSES)
    val = (((val<<2)+val)>>3)+860;
  else
#endif
        val = 0x0FFF;
        if (ch < 14)
        {
          packet[9 + ch * 2] = val;
          packet[10 + ch * 2] = (val >> 8) & 0x0F;
        }
        else
        {
          packet[10 + (ch - 14) * 6] |= (val) << 4;
          packet[12 + (ch - 14) * 6] |= (val) & 0xF0;
          packet[14 + (ch - 14) * 6] |= (val >> 4) & 0xF0;
        }
      }
      break;

    case AFHDS2A_PACKET_SETTINGS:
      packet[0] = 0xaa;
      packet[9] = 0xfd;
      packet[10] = 0xff;
      uint16_t val_hz = 5 * (g_model.rfOptionValue2 & 0x7f) + 50; // value should be between 0 and 70 which gives a value between 50 and 400Hz
      if (val_hz < 50 || val_hz > 400) val_hz = 50;	// default is 50Hz
      packet[11] = val_hz;
      packet[12] = val_hz >> 8;
      if (g_model.rfSubType == PPM_IBUS || g_model.rfSubType == PPM_SBUS)
      packet[13] = 0x01;	// PPM output enabled
      else
      packet[13] = 0x00;  // PWM
      packet[14] = 0x00;
      for (uint8_t i = 15; i < 37; i++)
        packet[i] = 0xff;
      packet[18] = 0x05;  // ?
      packet[19] = 0xdc;  // ?
      packet[20] = 0x05;  // ?
      if (g_model.rfSubType == PWM_SBUS || g_model.rfSubType == PPM_SBUS)
      packet[21] = 0xdd;	// SBUS output enabled
      else
      packet[21] = 0xde;	// IBUS
      break;

  }
  packet[37] = 0x00;
}

#if defined(FRSKY) // telemetry

enum
{ // telemetry sensors ID
  AFHDS2A_SENSOR_RX_VOLTAGE = 0x00,
//  AFHDS2A_SENSOR_RX_ERR_RATE  = 0xfe,
  AFHDS2A_SENSOR_RX_RSSI = 0xfc,
  AFHDS2A_SENSOR_RX_NOISE = 0xfb,
//  AFHDS2A_SENSOR_RX_SNR       = 0xfa,
  AFHDS2A_SENSOR_A3_VOLTAGE = 0x03,
};

static void AFHDS2A_update_telemetry()
{
  // 0    1234   5678   9           10         11       12
  // AA | TXID | rx_id | sensor id | sensor # | value 16 bit big endian | sensor id ......
  // AC | TXID | rx_id | sensor id | sensor # | length | bytes | sensor id ......
  // Max 7 sensors per packet.

  uint8_t linkOk = 0;

//  if (packet[0] != 0xAA) return; // 0xAA Normal telemetry, 0xAC Extended telemetry not decoded here --- yet

  for (uint8_t sensor = 0; sensor < 7; sensor++)
  {
    uint8_t index = 9 + (4 * sensor);

    switch (packet[index])
    {
      case AFHDS2A_SENSOR_RX_VOLTAGE:
        telemetryData.analog[TELEM_ANA_A1].set( packet[index + 2],
            g_model.telemetry.channels[TELEM_ANA_A1].type);
        linkOk = 1;
        break;

      case AFHDS2A_SENSOR_A3_VOLTAGE:
        telemetryData.analog[TELEM_ANA_A2].set( packet[index + 3] << 5 | (packet[index + 2] >> 3),
            g_model.telemetry.channels[TELEM_ANA_A2].type);
        linkOk = 1;
        break;

//    case AFHDS2A_SENSOR_RX_ERR_RATE:
//      if(packet[index+2]<=100)  RX_LQI=packet[index+2];
//      break;

      case AFHDS2A_SENSOR_RX_RSSI:
        telemetryData.rssi[1].set(-packet[index + 2]);
        linkOk = 1;
        break;

      case 0xff: // end of data
        break;
    }
  }

  if (linkOk)
    frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
  // frskyStreaming gets decremented every 10ms, FRSKY_TIMEOUT_FIRST value is detected to play connection prompt.
}
#endif

static uint16_t AFHDS2A_cb()
{
  heartbeat |= HEART_TIMER_PULSES;

  uint8_t data_rx;

  A7105_AdjustLOBaseFreq();

  switch (phase)
  {
    case AFHDS2A_BIND1:
    case AFHDS2A_BIND2:
    case AFHDS2A_BIND3:
      AFHDS2A_build_bind_packet();
      data_rx = A7105_ReadReg(A7105_00_MODE); // Check if something has been received...
      A7105_WriteData(AFHDS2A_TXPACKET_SIZE, bind_count % 2 ? 0x0d : 0x8c);

      if (!(data_rx & 0b00100011)) // removed FECF check due to issues with fs-x6b ->  & (1<<5 | 1<<6)
      { // RX+CRCF Ok
        A7105_ReadData(AFHDS2A_RXPACKET_SIZE);
        if (packet[0] == 0xbc && packet[9] == 0x01)
        {
          memcpy(&AFHDS2A_RX_ID, &packet[5], 4);
          memcpy(&AFHDS2A_RX_ID_STORAGE, &AFHDS2A_RX_ID, 4); // Store RX number in EEPROM.
          eeDirty(EE_MODEL); // Save in EEPROM.
          phase = AFHDS2A_BIND4;
          bind_count++;
          break;
        }
      }
      bind_count++;
      phase |= AFHDS2A_WAIT_WRITE;
      return 1500 * 2;

    case AFHDS2A_BIND1 | AFHDS2A_WAIT_WRITE:
    case AFHDS2A_BIND2 | AFHDS2A_WAIT_WRITE:
    case AFHDS2A_BIND3 | AFHDS2A_WAIT_WRITE:
      // Wait for TX completion
      wait_transmit = 0;
      while ((A7105_ReadReg(A7105_00_MODE) & 0x01)) // Wait 700 us max
        if (++wait_transmit > AFHDS2A_NUM_WAIT_LOOPS) break;

      A7105_ManagePower();
      A7105_SetTxRxMode((bind_count & 0x40) ? TXRX_OFF : RX_EN); // Turn LNA off time to time since we are in near range and we want to prevent swamping.
      A7105_Strobe(A7105_RX);
      phase &= ~AFHDS2A_WAIT_WRITE;
      phase++;
      if (phase > AFHDS2A_BIND3)
      phase = AFHDS2A_BIND1;
      return 2350 * 2;

    case AFHDS2A_BIND4:
      AFHDS2A_build_bind_packet();
      A7105_WriteData(AFHDS2A_TXPACKET_SIZE, bind_count % 2 ? 0x0d : 0x8c);
      bind_count++;

      if (++bind4_phase >= 4)
      {
        hopping_index = 1;
        phase = AFHDS2A_DATA_INIT;
        PROTOCOL_SetBindState(0); // BIND_DONE;
      }
      break;

    case AFHDS2A_DATA_INIT:
      packet_counter = 0;
      packet_type = AFHDS2A_PACKET_STICKS;
      phase = AFHDS2A_DATA;

    case AFHDS2A_DATA:
      if (!(packet_counter % 3))
      {
        SCHEDULE_MIXER_END_IN_US(12000); // Schedule next Mixer calculations.
      }

      AFHDS2A_build_packet(packet_type);
      data_rx = A7105_ReadReg(A7105_00_MODE); // Check if something has been received...
      A7105_WriteData(AFHDS2A_TXPACKET_SIZE,
      hopping_array[hopping_index++]);

      if (hopping_index >= AFHDS2A_NUMFREQ)
      {
        hopping_index = 0;
      }

      if (!(packet_counter & 0x03ff))
      { // Send settings every 3.9s
        packet_type = AFHDS2A_PACKET_SETTINGS;
      }
      else
      {
        packet_type = AFHDS2A_PACKET_STICKS; // ToDo : check for settings changes
      }

      if (!(data_rx & 0b00100011)) // removed FECF check due to issues with fs-x6b ->  & (1<<5 | 1<<6)
      { // RX+CRCF Ok
        A7105_ReadData(AFHDS2A_RXPACKET_SIZE);
        if (packet[0] == 0xAA && packet[9] == 0xFC)
        packet_type = AFHDS2A_PACKET_SETTINGS;	// RX is asking for settings.

#if defined(FRSKY) // telemetry
       else
         if(packet[0] == 0xAA && packet[9]!=0xFD  && (memcmp(&packet[1], temp_rfid_addr, 4) == 0) )
           { // Normal telemetry packet.
             // Ignore packets which contain the RX configuration: AA FD FF 32 00 01 00 FF FF FF 05 DC 05 DE FA FF FF FF FF FF FF FF FF FF FF FF FF FF FF
            // Read TX RSSI
            int16_t temp=256-(A7105_ReadReg(A7105_1D_RSSI_THOLD)*8)/5;  // value from A7105 is between 8 for maximum signal strength to 160 or less
            limit<int16_t>(0, temp, 255);
            telemetryData.rssi[0].set(temp);

            if (g_model.rfOptionBool1) // Telemetry on ?
             AFHDS2A_update_telemetry();
          }
#endif
      }

      packet_counter++;
      phase |= AFHDS2A_WAIT_WRITE;
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      return 1500 * 2;

    case AFHDS2A_DATA | AFHDS2A_WAIT_WRITE:
      // Wait for TX completion
      wait_transmit = 0;
      while ((A7105_ReadReg(A7105_00_MODE) & 0x01)) // wait 700 us max
        if (++wait_transmit > AFHDS2A_NUM_WAIT_LOOPS) break;

      A7105_ManagePower();
      A7105_SetTxRxMode(RX_EN);
      A7105_Strobe(A7105_RX);
      phase &= ~AFHDS2A_WAIT_WRITE;
      CALCULATE_LAT_JIT(); // Calculate latency and jitter.
      return 2350 * 2;
  }
  return 3850 * 2;
}

static void AFHDS2A_initialize(uint8_t bind)
{
  A7105_Init();
  loadrfidaddr_rxnum(0);
  AFHDS2A_calc_channels();

  memcpy(&AFHDS2A_RX_ID, &AFHDS2A_RX_ID_STORAGE, 4); // Load RX number stored in EEPROM.
  hopping_index = 0;

  if (bind)
  {
    bind_count = 0;
    bind4_phase = 0;
    phase = AFHDS2A_BIND1;
  }
  else
  {
    phase = AFHDS2A_DATA_INIT;
  }

  PROTO_Start_Callback(AFHDS2A_cb);
}

const void* AFHDS2A_Cmds(enum ProtoCmds cmd)
{
  switch (cmd)
  {
    case PROTOCMD_INIT:
      AFHDS2A_initialize(0);
      return 0;
    case PROTOCMD_RESET:
      A7105_Reset();
      PROTO_Stop_Callback();
      return 0;
    case PROTOCMD_BIND:
      AFHDS2A_initialize(1);
      return 0;
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_AFHDS2A_Ser),
          STR_SUBTYPE_AFHDS2A_SPI,      //Sub proto
          STR_RFTUNEFINE,      //Option 1 (int)
          STR_MULTI_SERVOFREQ, //Option 2 (int)
          STR_RFPOWER,         //Option 3 (uint 0 to 31)
          STR_TELEMETRY,       //OptionBool 1
          STR_DUMMY,           //OptionBool 2
          STR_DUMMY            //OptionBool 3
          );
      return 0;
    default:
      break;
  }
  return 0;
}

#undef packet
#undef hopping_array
#undef phase
#undef hopping_index
#undef wait_transmit
#undef bind4_phase
#undef bind_count
#undef packet_type
#undef packet_counter
#undef temp_rfid_addr

