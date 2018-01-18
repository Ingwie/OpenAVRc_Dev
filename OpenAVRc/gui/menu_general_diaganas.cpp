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


#include "menu_general.h"

#define TR_ANAIN_POT_BAT "Pt1""Pt2""Pt3""Bat"
const pm_char STR_TR_ANAIN_POT_BAT[] PROGMEM = TR_ANAIN_POT_BAT;


void menuGeneralDiagAna(uint8_t event)
{
#define ANAS_ITEMS_COUNT 2

  SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, ANAS_ITEMS_COUNT);

  STICK_SCROLL_DISABLE();

  uint8_t numLoop = NUM_STICKS+NUM_POTS;

  for (uint8_t i=0; i<numLoop+1; i++) { // Add battery input
    coord_t y = MENU_HEADER_HEIGHT + 1 + (i/2)*FH;
    uint8_t x = i&1 ? 65 : 2;
    if (i<4) {
      putsMixerSource(x, y, MIXSRC_Rud + pgm_read_byte_near(modn12x3 + 4*g_eeGeneral.stickMode + i), (anaIn(i) < 1 || anaIn(i) > 0x7FD) ? INVERS : 0);
    } else {
      lcdDrawSizedTextAtt(x, y, STR_TR_ANAIN_POT_BAT+3*(i-4), 3, (anaIn(i) < 1 || anaIn(i) > 0x7FD) ? INVERS : 0);
    }
    lcdDrawChar(x+18, y, ':');
    lcd_outhex(4, x+3*FW+4, y, anaIn(i), 0);
    if (i<numLoop)
      lcd_outdez8(x+10*FW+2, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
  }
  lcdDrawTextLeft(6*FH-2, STR_BATT_CALIB);
  lcdPutsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, g_vbat10mV, (menuVerticalPosition==1 ? INVERS : 0)|PREC2);
  if (menuVerticalPosition==1)
    CHECK_INCDEC_GENVAR(event, g_eeGeneral.txVoltageCalibration, -127, 127);

}
