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
//SPI Def
#define RFPOWER         rfOptionValue3
#define RXNUM           g_model.modelId


#define RF_ID_ADDR_OFFSET_VAR   4
#define RX_TX_ADDR_OFFSET       (PULSES_BYTE_SIZE - RF_ID_ADDR_OFFSET_VAR)
#define MAX_PACKET              50
#define CHANNEL_USED_OFFSET     MAX_PACKET
#define MAX_CHANNEL             75
#define FREE_BYTE_OFFSET        (MAX_PACKET + MAX_CHANNEL)
#define PULSES_BYTE_OFFSET_VAR  FREE_BYTE_OFFSET           // 125
#define PULSES_WORD_OFFSET_VAR  (RX_TX_ADDR_OFFSET/2)      // 140/2

#if defined(SPIMODULES) || (SERIAL_PROTOCOL!=NO)
  uint8_t * packet_p2M = pulses2MHz.pbyte; //protocol global packet (Use 40 MAX)
#endif

#define Usart0TxBuffer_p2M    packet_p2M // [USART0_TX_PACKET_SIZE] bytes used

#if defined(SPIMODULES)
  uint8_t * channel_used_p2M = &pulses2MHz.pbyte[CHANNEL_USED_OFFSET]; //protocol global channel (Use 50 MAX -> 54 bytes free to use in SPI protocols)
  uint8_t * temp_rfid_addr_p2M = &pulses2MHz.pbyte[RX_TX_ADDR_OFFSET];
  #if defined(PROTO_HAS_CC2500) || defined(PROTO_HAS_CYRF6936) || defined(PROTO_HAS_NRF24L01)
    //uint8_t * telem_save_data_buff = &pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR+10]; // used in [9] FrskyX & [10] DSM telemetry
    uint8_t telem_save_data_buff[22]; // used in [9] FrskyX & [10] DSM telemetry
  #endif
  #if defined(PROTO_HAS_CC2500)
    uint8_t calData[75]; // used in FrskyX , Hitec & Hott protocol
  #endif
  #if defined(PROTO_HAS_CC2500) || defined(PROTO_HAS_A7105)
    uint32_t seed; // used in FrskyV or AFHDS2A
  #endif
#endif

/**< USE to store Protocols dynamic datas
  uint16_t pword[PULSES_WORD_SIZE];  // 72
  uint8_t  pbyte[PULSES_BYTE_SIZE];  // 144
 */
// General shared values
#define FREQ_FINE_MEM_P2M      pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR] // Used with CC2500 & A7105
#define RF_SETUP_P2M           pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR] // Only used with NRF24L01 register 06
#define RF_POWER_P2M           pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR+1]
#define RF_POWER_MEM_P2M       pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR+2]

// Macro to define protocols static datas
#define BYTE_P2M(x)            pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR+2+(x)]
#define WORD_P2M(x)            pulses2MHz.pword[PULSES_WORD_OFFSET_VAR+1-(x)]


uint8_t bit_reverse(uint8_t a);
uint16_t convert_channel_10b(uint8_t num);
uint8_t convert_channel_8b(uint8_t num);
uint8_t convert_channel_6b(uint8_t num);
void inline PROTO_Change_Callback(uint16_t (*cb)());
void PROTO_Start_Callback( uint16_t (*cb)());
void PROTO_Stop_Callback();
void PROTOCOL_SetBindState(tmr10ms_t t10ms);
uint16_t PROTOCOL_GetElapsedTime();
#if defined(SPIMODULES)
extern uint16_t RFPowerOut;
void loadrfidaddr();
void loadrfidaddr_rxnum(uint8_t addrnum);
#endif

#endif
