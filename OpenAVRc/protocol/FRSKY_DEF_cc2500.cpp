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

#include "FRSKY_DEF_cc2500.h"

void FRSKY_Init_Common_End()
{
  uint_farptr_t pdata = pgm_get_far_address(ZZ_FRSKY_Common_End);

  for (uint8_t i=0; i<(DIM(ZZ_FRSKY_Common_End)/2); ++i) { // Send init
    uint8_t reg = pgm_read_byte_far(pdata);
    uint8_t dat = pgm_read_byte_far(++pdata);
    CC2500_WriteReg(reg,dat);
    ++pdata;
  }
}

void FRSKY_generate_channels()
{
/*
 * Make sure adjacent channels in the array are spread across the band and are not repeated.
 */

  uint16_t firstId = temp_rfid_addr_p2M[0] + temp_rfid_addr_p2M[3];
  uint16_t secondID = temp_rfid_addr_p2M[1] + temp_rfid_addr_p2M[2];

  uint8_t chan_offset = firstId % 10; // 10 channel bases.
  uint8_t step = secondID % 11; // 11 sequences for now.

  step = step + 73; // 73 to 83.
  // Build channel array.
  for(uint8_t idx =0; idx <50; idx++) {
    uint16_t res = ((step * idx) + chan_offset) % 236; // 235 is the highest channel used.

		if((res==0x00) || (res==0x5A) || (res==0xDC))
    {
      ++res;
    }
    if(idx > 46) res = 0; // Unused but sent to rx in bind packet_p2M.
    channel_used_p2M[idx] = res;
   }
}
