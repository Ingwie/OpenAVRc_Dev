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

#ifndef SPIRF_H
#define SPIRF_H

#define MAX_PACKET              50
#define MAX_CHANNEL             75

typedef union{ // Place any SPI structs here. Note 144 bytes maximum.
  struct afhds2a_spi
  { // 144 bytes Maximum e.g. PULSES_BYTE_SIZE
    uint8_t packet_P2M[MAX_PACKET]; // 50
    uint8_t channel_used_p2M[MAX_CHANNEL]; // 75
    uint8_t unused1[3]; // &127
    uint8_t afhds2a_RF_STATE_P2M;
    uint8_t afhds2a_CH_IDX_P2M;
    uint8_t afhds2a_REC_SEQ_P2M;
    uint8_t afhds2a_BIND_IDX_P2M;
    uint8_t afhds2a_PACKET_COUNT_P2M; //&132
    uint8_t unused2[5];
    uint16_t AFHDS2A_RF_STATE16_P2M; //&138
    uint8_t temp_rfid_addr_p2M[4]; // &140
  } __attribute__((__packed__));
} spiU_t;

#endif // SPIRF_H
