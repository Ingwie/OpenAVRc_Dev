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


void menuGeneralDiagAna(uint8_t event)
{
#define ANAS_ITEMS_COUNT 2

  SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, ANAS_ITEMS_COUNT);

  STICK_SCROLL_DISABLE();

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
#if (NUM_STICKS+NUM_POTS) > 9
    coord_t y = MENU_HEADER_HEIGHT + 1 + (i/3)*FH;
    const uint8_t x_coord[] = {0, 70, 154};
    uint8_t x = x_coord[i%3];
    lcdDrawNumberNAtt(x, y, i+1, LEADING0|LEFT, 2);
    lcdDrawChar(x+2*FW-2, y, ':');
#else
    coord_t y = MENU_HEADER_HEIGHT + 1 + (i/2)*FH;
    uint8_t x = i&1 ? 64+5 : 0;
    lcdDrawStringWithIndex(x, y, PSTR("A"), i+1);
    lcdDrawChar(lcdNextPos, y, ':');
#endif
    lcd_outhex4(x+3*FW-1, y, anaIn(i));
    lcd_outdez8(x+10*FW-1, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
  }

  // Display raw BandGap result (debug)
  lcdDrawText(64+5, MENU_HEADER_HEIGHT+1+3*FH, STR_BG);
  lcdDrawNumberAttUnit(64+5+6*FW-3, 1+4*FH, BandGap, 0);

  lcdDrawTextLeft(6*FH-2, STR_BATT_CALIB);
  lcdPutsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, g_vbat10mV, (menuVerticalPosition==1 ? INVERS : 0)|PREC2);
  if (menuVerticalPosition==1) CHECK_INCDEC_GENVAR(event, g_eeGeneral.txVoltageCalibration, -127, 127);

}
