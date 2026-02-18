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

/*
  **************************************************************************************************
  Support for DIY receivers from this repository:
  https://github.com/stanekTM/RX_nRF24_Stanek
  
  The "Stanek" protocol with the nRF24L01+ transceiver is included.
  Setting the number of control channels 2 to 13ch.
  Telemetry monitors receiver voltage A1(A2) and "fake" RSSI.
  The nRF24L01+ transceiver does not contain real RSSI and is only a rough counting of lost packets.
  **************************************************************************************************
*/

#include "../OpenAVRc.h"

// Define pulses2MHz reusable values (13 bytes max)
#define STANEK_RFSTATE             BYTE_P2M(1)
#define STANEK_CH_IDX_P2M          BYTE_P2M(2)
#define STANEK_REC_SEQ_P2M         BYTE_P2M(3)
#define STANEK_PACKET_SIZE_P2M     BYTE_P2M(4)
#define STANEK_TELEM_SAVE_SEQ_P2M  BYTE_P2M(5)

#define STANEK_RF_STATE16_P2M      WORD_P2M(1)
#define STANEK_BIND_COUNTER_16_P2M WORD_P2M(2)
//***********************************************//

const static RfOptionSettingsvar_t RfOpt_STANEK_Ser[] PROGMEM =
{
  /*rfProtoNeed*/PROTO_NEED_SPI/* | BOOL1USED | BOOL2USED */, // Can be PROTO_NEED_SPI | BOOL1USED | BOOL2USED | BOOL3USED
  /*rfSubTypeMax*/      0,
  /*rfOptionValue1Min*/ 2,  // RC channel min.
  /*rfOptionValue1Max*/ 13, // RC channel max.
  /*rfOptionValue2Min*/ 0,
  /*rfOptionValue2Max*/ 0,
  /*rfOptionValue3Max*/ 3,  // RF power
};

#define STANEK_NUM_RC_CHANNELS  g_model.rfOptionValue1 // Num RC channels (2 to 13ch)

uint8_t STANEK_TX_RX_Address[] = "jirka"; // Setting a unique address (5 bytes number or character)

#define STANEK_RF_CHANNEL     76          // RF channel on 0 to 125 (2.4GHz to 2.525GHz)

#define STANEK_PACKET_PERIOD  3000        // In microseconds

//**********************************************************************************************************************************
// STANEK_init
//**********************************************************************************************************************************
static void STANEK_init()
{
  STANEK_CH_IDX_P2M = STANEK_RF_CHANNEL; // Initialize the RF channel
  
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    (uint8_t*)(&STANEK_TX_RX_Address), 5);
  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (uint8_t*)(&STANEK_TX_RX_Address), 5);
  
  NRF24L01_FlushTx();
  NRF24L01_FlushRx();
  
  NRF24L01_Initialize();
  
  NRF24L01_ManagePower();
  NRF24L01_SetTxRxMode(TX_EN); // Clear data ready, data sent, retransmit and enable CRC 16 bits, ready for TX
  
  NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00); // 0x00 Disable auto acknowledgement on all data pipes
  //                                             0x3F Enable auto acknowledgement on all data pipes
  //                                             0x01 Enable auto acknowledgement data pipe 0
  
  NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x3F); // 0x3F Enable all data pipes
  //                                                 0x01 Enable data pipe 0 only
  
  NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03); // 5 bytes RX/TX address field width
  
  NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x55); // 0x55 1500us (5 * 250us + 250us) delay, 5 * retries
  //                                                  0xFF 4000us (15 * 250us + 250us) delay, 15 * retries
  //                                                  0x00 Disable retransmits
  
  NRF24L01_SetBitrate(NRF24L01_BR_250K); // 250Kbps
  
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70); // Reset status
  
  NRF24L01_Activate(0x73); // Activate feature register
  
  NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x3F); // 0x3F Enable Dynamic Payload Length on all data pipes
  //                                             0x01 Enable Dynamic Payload Length on data pipe 0
  
  NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x04); // 0x04 Enable Dynamic Payload Length
  //                                               0x06 Enable Dynamic Payload Length, enable Payload with ACK
  //                                               0x07 Enable all features
  
  NRF24L01_Activate(0x73); // Activate feature register
}

//**********************************************************************************************************************************
// STANEK_get_telemetry
//**********************************************************************************************************************************
//static void STANEK_get_telemetry()
FORCEINLINE void STANEK_get_telemetry()
{
  // Calculate TX RSSI based on past 250 expected telemetry packets.
  // Cannot use full second count because STANEK_REC_SEQ_P2M is not large enough
  if (++STANEK_RF_STATE16_P2M > 250)
  {
#if defined(FRSKY)
    telemetryData.rssi[1].set(STANEK_REC_SEQ_P2M);
#endif
    STANEK_REC_SEQ_P2M = 0;
    STANEK_RF_STATE16_P2M = 0;
  }
  
  // Process received telemetry packet
  //if (NRF24L01_ReadReg(NRF24L01_07_STATUS) & _BV(NRF24L01_07_RX_DR))
  if (NRF24L01_NOP() & _BV(NRF24L01_07_RX_DR))
  {
    // Read telemetry data
    NRF24L01_ReadPayload(telem_save_data_buff, 3);
#if defined(FRSKY)
    frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;
    
    telemetryData.rssi[0].set(telem_save_data_buff[0]); // Packet rate 0 to 255 where 255 is 100% packet rate
    telemetryData.analog[TELEM_ANA_A1].set(telem_save_data_buff[1], g_model.telemetry.channels[TELEM_ANA_A1].type); // Directly from analog input of receiver, but reduced to 8 bit depth (0 to 255)
    telemetryData.analog[TELEM_ANA_A2].set(telem_save_data_buff[2], g_model.telemetry.channels[TELEM_ANA_A2].type); // Directly from analog input of receiver, but reduced to 8 bit depth (0 to 255)
#endif
    STANEK_REC_SEQ_P2M++;
  }
  else
  {
    // If no telemetry packet was received then delay by the typical telemetry packet processing time.
    // This is done to try to keep the STANEK_send_packet process timing more consistent. Since the SPI payload read takes some time
    _delay_us(50);
  }
  
  NRF24L01_SetTxRxMode(TX_EN);
  NRF24L01_FlushRx();
}
//**********************************************************************************************************************************
// STANEK_send_packet
//**********************************************************************************************************************************
static void STANEK_send_packet()
{
  STANEK_get_telemetry();
  
  int16_t hold_value;
  uint8_t payload_index = 0;
  
  for (uint8_t x = 0; x < STANEK_NUM_RC_CHANNELS; x++)
  {
    // Valid channel values are 1000 to 2000
    hold_value = (FULL_CHANNEL_OUTPUTS(x)) / 2; // +-1024 to +-512
    hold_value += PPM_CENTER; // + 1500 offset
    hold_value = limit<int16_t>(1000, hold_value, 2000);

    packet_p2M[payload_index] = hold_value & 0xFF;
    payload_index++;
    packet_p2M[payload_index] = hold_value >> 8;
    payload_index++;
  }
  
  STANEK_PACKET_SIZE_P2M = STANEK_NUM_RC_CHANNELS * 2; // For one control channel with a value of 1000 to 2000 we need 2 bytes(packets)
  
  // Set RF channel and send data
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, STANEK_CH_IDX_P2M);
  NRF24L01_ManagePower();
  NRF24L01_WritePayload(packet_p2M, STANEK_PACKET_SIZE_P2M);
}

//**********************************************************************************************************************************
// STANEK_manage_time
//**********************************************************************************************************************************
static uint16_t STANEK_manage_time()
{
  uint16_t packet_period;
  
  // Switch radio to RX as soon as packet is sent.
  // Calculate transmit time based on packet size and data rate of 250Kbs per sec
  uint16_t rx_delay = /* Variable time air */(4 * 8 * STANEK_PACKET_SIZE_P2M) + /* Fixed */432; // 560us -> 1200us
  
  if (!STANEK_TELEM_SAVE_SEQ_P2M)
  {
    STANEK_BIND_COUNTER_16_P2M = PROTOCOL_GetElapsedTime(); // Use STANEK_BIND_COUNTER_16_P2M as memory only here
    packet_period = rx_delay + STANEK_BIND_COUNTER_16_P2M;
    STANEK_TELEM_SAVE_SEQ_P2M = 1; // Indicate to switch to RX mode next time
  }
  else
  {
    // Increase packet period by 100us for each channel over 6
    packet_period = limit<uint16_t>(0, (uint8_t)(STANEK_NUM_RC_CHANNELS - 6), 10);
    packet_period *= 100;
    packet_period += STANEK_PACKET_PERIOD;
    packet_period -= rx_delay + STANEK_BIND_COUNTER_16_P2M; // Remove RX time
    STANEK_TELEM_SAVE_SEQ_P2M = 0; // Reset switch to RX
  }
  
  return packet_period;
}

//**********************************************************************************************************************************
// STANEK_cb
//**********************************************************************************************************************************
static uint16_t STANEK_cb()
{
  if (STANEK_TELEM_SAVE_SEQ_P2M) // We need to switch to RX mode to read telemetry
  {
    NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x7F); // 0x7F RX mode with 16 bit CRC no IRQ
    //                                              0x0F RX mode with 16 bit CRC
  }
  else
  {
    if (++STANEK_RFSTATE >= 4)
    {
      STANEK_RFSTATE = 0;
      SCHEDULE_MIXER_END_IN_US(12000); // Schedule next mixer calculations
    }
    
    STANEK_send_packet();
  }
  
  uint16_t protocol_period = STANEK_manage_time();
  
  heartbeat |= HEART_TIMER_PULSES;
  
  CALCULATE_LAT_JIT(); // Calculate latency and jitter
  
  return protocol_period * 2; // From 3ms to 4ms
}

//**********************************************************************************************************************************
// STANEK_Cmds
//**********************************************************************************************************************************
const void *STANEK_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
  {
    case PROTOCMD_INIT:
    STANEK_init();
    PROTO_Start_Callback(STANEK_cb);
    return 0;
   case PROTOCMD_RESET:
    PROTO_Stop_Callback();
    NRF24L01_Reset();
    return 0;
   case PROTOCMD_BIND:
    STANEK_init();
    PROTO_Start_Callback(STANEK_cb);
    return 0;
   case PROTOCMD_GETOPTIONS:
    SetRfOptionSettings(pgm_get_far_address(RfOpt_STANEK_Ser),
    STR_DUMMY,   // Sub protocol
    STR_NUMCH,   // Option 1 (int) num RC channels (2 to 13ch)
    STR_DUMMY,   // Option 2 (int)
    STR_RFPOWER, // Option 3 (uint 0 to 31) RF power
    STR_DUMMY,   // OptionBool 1
    STR_DUMMY,   // OptionBool 2
    STR_DUMMY);  // OptionBool 3
    return 0;
   default:
    break;
  }
  return 0;
}

