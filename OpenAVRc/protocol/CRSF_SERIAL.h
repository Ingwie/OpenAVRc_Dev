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

#ifndef CRSF_SERIAL_H
#define CRSF_SERIAL_H

#define CRSF_CHAN_PACKET_SIZE     26

typedef struct
{
  uint8_t crsf_tx_buffer[CRSF_CHAN_PACKET_SIZE]; // Reserved aka Usart0TxBuffer_p2M
volatile  uint8_t crsf_rx_state;
  uint8_t crsf_freq_rate;
  uint8_t crsf_40ms_flag;
  uint8_t crsf_40ms_flipflop;
  uint16_t crsf_rate_period;
  uint8_t crsf_rx_buffer[15];
  int32_t crsf_timing_correct;
  int16_t crsf_timing_offset;
  uint8_t crsf_module_responding;
} crsfSt_t;

// Frame Type
#define XF_TYPE_GPS              0x02
#define XF_TYPE_VARIO            0x07
#define XF_TYPE_BATTERY          0x08
#define XF_TYPE_BARO_ALT         0x09
#define XF_TYPE_HEARTBEAT        0x0b
#define XF_TYPE_VTX              0x0F
#define XF_TYPE_VTX_TELEM        0x10
#define XF_TYPE_LINK             0x14
#define XF_TYPE_CHANNELS         0x16
#define XF_TYPE_RX_ID            0x1C
#define XF_TYPE_TX_ID            0x1D
#define XF_TYPE_ATTITUDE         0x1E
#define XF_TYPE_FLIGHT_MODE      0x21
#define XF_TYPE_PING_DEVICES     0x28
#define XF_TYPE_DEVICE_INFO      0x29
#define XF_TYPE_REQUEST_SETTINGS 0x2A
#define XF_TYPE_SETTINGS_ENTRY   0x2B
#define XF_TYPE_SETTINGS_READ    0x2C
#define XF_TYPE_SETTINGS_WRITE   0x2D
#define XF_TYPE_ELRS_INFO        0x2E
#define XF_TYPE_COMMAND_ID       0x32
#define XF_TYPE_RADIO_ID         0x3A

// Frame Subtype
#define CRSF_UART_SYNC                      0xC8
#define CRSF_SUBCOMMAND                     0x10
#define CRSF_COMMAND_MODEL_SELECT_ID        0x05

#endif // CRSF_SERIAL_H
