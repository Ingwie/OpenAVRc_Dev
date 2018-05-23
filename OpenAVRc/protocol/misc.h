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

#define RX_TX_ADDR_OFFSET       PULSES_BYTE_SIZE - 5
#define CHANNEL_USED_OFFSET     50

uint8_t * packet = pulses2MHz.pbyte; //protocol global packet (Use 50 MAX)
uint8_t * channel_used = &pulses2MHz.pbyte[CHANNEL_USED_OFFSET]; //protocol global channel (Use 50 MAX -> 44 bytes free to use in SPI protocols)
uint8_t * t_rf_id_addr = &pulses2MHz.pbyte[RX_TX_ADDR_OFFSET];

#define RF_ID_ADDR_OFFSET_VAR   4
#define PULSES_WORD_OFFSET_VAR  50 // 100 bytes
#define PULSES_BYTE_OFFSET_VAR         PULSES_BYTE_SIZE - RF_ID_ADDR_OFFSET_VAR //139

/**< USE to store Protocols dynamic datas
  uint16_t pword[PULSES_WORD_SIZE];
  uint8_t  pbyte[PULSES_BYTE_SIZE];  // 144
 */
//U8
#define rfState8       pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-1]
#define channel_index  pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-2]
#define channel_offset pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-3]
#define rf_power       pulses2MHz.pbyte[PULSES_BYTE_OFFSET_VAR-4]
//U16
#define packet_period pulses2MHz.pword[PULSES_WORD_OFFSET_VAR]
#define bind_counter  pulses2MHz.pword[PULSES_WORD_OFFSET_VAR+1]
#define rfState16     pulses2MHz.pword[PULSES_WORD_OFFSET_VAR+2]


extern void PROTO_Start_Callback(uint16_t us, uint16_t (*cb)());
extern void PROTO_Stop_Callback();
extern uint32_t CLOCK_getms();
extern void CLOCK_delayms(uint32_t delay_ms);
extern void PROTOCOL_SetBindState(tmr10ms_t t10ms);

#endif
