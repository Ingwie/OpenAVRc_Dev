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


#ifndef PROTOCOL_FRSKY_H_
#define PROTOCOL_FRSKY_H_

static uint16_t frsky_id;
static uint8_t channels_used[50];
//static uint8_t channel_offset;

void FRSKY_generate_channels(void)
{
/*
 * Make sure adjacent channels in the array are spread across the band and are not repeated.
 */

  uint8_t chan_offset = ((SpiRFModule.fixed_id >> 16) & 0xFF) % 10; // 10 channel bases.
  uint8_t step = (((SpiRFModule.fixed_id >> 24) & 0xFF) % 11); // 11 sequences for now.

  step = step + 73; // 73 to 83.
  // Build channel array.
  for(uint8_t idx =0; idx <50; idx++) {
    uint16_t res = ((step * idx) + chan_offset) % 236; // 235 is the highest channel used.

    if(res == 0) res = 161; // Avoid binding channel 0.
    if(res == 1) res = 80; // Channel 1 probably indicates end of sequence in bind packet.
    if(idx == 47) res = 1; // Unused but sent to rx in bind packet, may indicate end of sequence.
    if(idx > 47) res = 0; // Unused but sent to rx in bind packet.
    channels_used[idx] = res;
   }
}


#endif /* PROTOCOL_FRSKY_H_ */
