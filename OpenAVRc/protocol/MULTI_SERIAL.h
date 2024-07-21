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

#ifndef MULTI_SERIAL_H
#define MULTI_SERIAL_H

struct mm_t1_pkt
  {
    union
    { // The unnamed unions can be accessed e.g. ->input_signal_detected.
      uint8_t flags;
      struct
      {
        unsigned input_signal_detected :1;// Input signal detected
        unsigned serial_mode_enabled :1;// Serial mode enabled
        unsigned protocol_is_valid :1;// Protocol is valid
        unsigned mm_is_binding :1;// Module is in binding mode
        unsigned w_a_b_e_t_l_t_p :1;// Module waits a bind event to load the protocol !
        unsigned protocol_supports_failsafe :1;// Current protocol supports failsafe
        unsigned p_s_d_c_m :1;// Current protocol supports disable channel mapping
        unsigned data_buffer_almost_full :1;// Data buffer is almost full
      };
    };
    uint8_t ver_major;
    uint8_t ver_minor;
    uint8_t ver_revision;
    uint8_t ver_patchlevel;
    uint8_t channel_Order;
    uint8_t prev_valid_proto;
    uint8_t next_valid_proto;
    char proto_name[7];
    union
    {
      uint8_t options;
      struct
      {
        unsigned qty_sub_proto :4;// Number of sub protocols
        unsigned option_text_displayed :4;// Option text to be displayed
      };
    };
  char sub_proto_name[8];
  } __attribute__((__packed__));



#define MM_TX_PKT_LEN 27
#define MM_TYPE_01_PKT_LEN 24

typedef struct
{
  uint8_t mm_tx_buffer[MM_TX_PKT_LEN]; // Reserved aka Usart0TxBuffer_p2M
  uint8_t mm_rx_buffer[MM_TYPE_01_PKT_LEN];
  struct mm_t1_pkt mm_type1_packet;
} mmSt_t;


#endif // MULTI_SERIAL_H
