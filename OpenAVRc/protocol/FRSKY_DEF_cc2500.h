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

#ifndef FRSKY_DEF_CC2500_H_INCLUDED
#define FRSKY_DEF_CC2500_H_INCLUDED

const uint8_t ZZ_FRSKY_InitRegister_Start[] PROGMEM =
{
  CC2500_02_IOCFG0,
  CC2500_00_IOCFG2,
  CC2500_17_MCSM1,
  CC2500_18_MCSM0,
  CC2500_06_PKTLEN,
  CC2500_07_PKTCTRL1,
  CC2500_08_PKTCTRL0,
  CC2500_3E_PATABLE,
  CC2500_0B_FSCTRL1,
  CC2500_0C_FSCTRL0,
  CC2500_0D_FREQ2,
  CC2500_0E_FREQ1,
  CC2500_0F_FREQ0,
  CC2500_10_MDMCFG4,
  CC2500_11_MDMCFG3,
  CC2500_12_MDMCFG2,
  CC2500_13_MDMCFG1,
  CC2500_14_MDMCFG0,
  CC2500_15_DEVIATN,
};

const uint8_t ZZ_FRSKY_Common_End[] PROGMEM =
{
  CC2500_19_FOCCFG,   0x16,
  CC2500_1A_BSCFG,    0x6C,
  CC2500_1B_AGCCTRL2, 0x43,
  CC2500_1C_AGCCTRL1, 0x40,
  CC2500_1D_AGCCTRL0, 0x91,
  CC2500_21_FREND1,   0x56,
  CC2500_22_FREND0,   0x10,
  CC2500_23_FSCAL3,   0xA9,
  CC2500_24_FSCAL2,   0x0A,
  CC2500_25_FSCAL1,   0x00,
  CC2500_26_FSCAL0,   0x11,
  CC2500_29_FSTEST,   0x59,
  CC2500_2C_TEST2,    0x88,
  CC2500_2D_TEST1,    0x31,
  CC2500_2E_TEST0,    0x0B,
  CC2500_03_FIFOTHR,  0x07,
  CC2500_09_ADDR,     0x00,
};

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

void FRSKY_Init_Channels(void)
{ // Multiprotocole function
	uint8_t val;
	uint8_t channel = g_eeGeneral.fixed_ID.ID_8[0]&0x07;
	uint8_t channel_spacing = g_eeGeneral.fixed_ID.ID_8[1];
	//Filter bad tables
	if(channel_spacing<0x02) channel_spacing+=0x02;
	if(channel_spacing>0xE9) channel_spacing-=0xE7;
	if(channel_spacing%0x2F==0) channel_spacing++;

	channel_used[0]=channel;
	for(uint8_t i=1;i<50;i++)
	{
		channel=(channel+channel_spacing) % 0xEB;
		val=channel;
		if((val==0x00) || (val==0x5A) || (val==0xDC))
			val++;
		channel_used[i]=i>46?0:val;
	}
}

void FRSKY_generate_channels()
{
/*
 * Make sure adjacent channels in the array are spread across the band and are not repeated.
 */

  uint8_t chan_offset = g_eeGeneral.fixed_ID.ID_8[2] % 10; // 10 channel bases.
  uint8_t step = g_eeGeneral.fixed_ID.ID_8[3] % 11; // 11 sequences for now.

  step = step + 73; // 73 to 83.
  // Build channel array.
  for(uint8_t idx =0; idx <50; idx++) {
    uint16_t res = ((step * idx) + chan_offset) % 236; // 235 is the highest channel used.

    if(res == 0) res = 161; // Avoid binding channel 0.
    if(res == 1) res = 80; // Channel 1 probably indicates end of sequence in bind packet.
    if(idx == 47) res = 1; // Unused but sent to rx in bind packet, may indicate end of sequence.
    if(idx > 47) res = 0; // Unused but sent to rx in bind packet.
    channel_used[idx] = res;
   }
}

#endif // FRSKY_DEF_CC2500_H_INCLUDED
