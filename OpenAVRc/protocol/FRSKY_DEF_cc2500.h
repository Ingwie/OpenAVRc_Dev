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

#include "../OpenAVRc.h"

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

void FRSKY_Init_Common_End();
void FRSKY_generate_channels();

#endif // FRSKY_DEF_CC2500_H_INCLUDED
