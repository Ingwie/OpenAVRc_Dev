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


#ifndef misc_h
#define misc_h

// PPM Def
#define PPMFRAMELENGTH  rfOptionValue1 // 0=22.5ms  (10ms-30ms) 0.5ms increments
#define PPMDELAY        rfOptionValue2
#define PPMNCH          rfSubType
#define PULSEPOL        rfOptionBool1
//Multi Def
#define MULTIRFPROTOCOL rfOptionValue1
#define CUSTOMPROTO     rfOptionBool1
#define AUTOBINDMODE    rfOptionBool2
#define LOWPOWERMODE    rfOptionBool3
//SPI Def
#define RFPOWER         rfOptionValue3
#define RXNUM           g_model.modelId


#define RF_ID_ADDR_OFFSET_VAR   4
#define RX_TX_ADDR_OFFSET       (PULSES_BYTE_SIZE - RF_ID_ADDR_OFFSET_VAR)
#define MAX_PACKET              40
#define CHANNEL_USED_OFFSET     MAX_PACKET
#define MAX_CHANNEL             50
#define FREE_BYTE_OFFSET        (MAX_PACKET + MAX_CHANNEL)
#define PULSES_WORD_OFFSET_VAR  (FREE_BYTE_OFFSET / 2) // 45 (90 bytes)
#define PULSES_BYTE_OFFSET_VAR  RX_TX_ADDR_OFFSET      // 140


#if defined(SPIMODULES)
  uint8_t * packet = pulses2MHz.pbyte; //protocol global packet (Use 40 MAX)
  uint8_t * channel_used = &pulses2MHz.pbyte[CHANNEL_USED_OFFSET]; //protocol global channel (Use 50 MAX -> 54 bytes free to use in SPI protocols)
  uint8_t * temp_rfid_addr = &pulses2MHz.pbyte[RX_TX_ADDR_OFFSET];
  #if defined(PROTO_HAS_CYRF6936)
  //uint8_t * cyrfmfg_id = &pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-30]; // -30 to -25 cyrfmfg_id[6] used in DSM, DEVO
  #endif
  #if defined(PROTO_HAS_CC2500) || defined(PROTO_HAS_CYRF6936) || defined(PROTO_HAS_NRF24L01)
    uint8_t * telem_save_data = &pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-40]; // used in [9] FrskyX & [10] DSM telemetry
  #endif
  #if defined(PROTO_HAS_CC2500)
    uint8_t calData[48]; // used in FrskyX protocol
    uint32_t seed; // used in FrskyV telemetry
  #endif
#endif

/**< USE to store Protocols dynamic datas
  uint16_t pword[PULSES_WORD_SIZE];  // 72
  uint8_t  pbyte[PULSES_BYTE_SIZE];  // 144
 */

//U8
#define rfState8           pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-1]
#define channel_index      pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-2]
#define channel_offset     pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-3]
#define rf_power           pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-4]
#define rf_power_mem       pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-5]
#define channel_skip       pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-6]
#define receive_seq        pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-7]
#define send_seq           pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-8]
#define bind_idx           pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-9]
#define dp_crc_init        pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-10]
#define packet_count       pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-11]
#define telem_save_seq     pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-12]
#define start_tx_rx        pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-13]
#define num_channel        pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-13] // Only used in DEVO
#define freq_fine_mem      pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-14] // Only used with CC2500
#define prev_num_channel   pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-14] // Only used in DSM DEVO

//U16
#define rfState16          pulses2MHz.pword[PULSES_WORD_OFFSET_VAR]    // Only used in Corona
#define bind_counter       pulses2MHz.pword[PULSES_WORD_OFFSET_VAR+1]


void PROTO_Start_Callback( uint16_t (*cb)());
void PROTO_Stop_Callback();
uint32_t CLOCK_getms();
void CLOCK_delayms(uint32_t delay_ms);
void PROTOCOL_SetBindState(tmr10ms_t t10ms);
#if defined(SPIMODULES)
extern uint16_t RFPowerOut;
void loadrfidaddr();
void loadrfidaddr_rxnum(uint8_t addrnum);
#endif

#endif
