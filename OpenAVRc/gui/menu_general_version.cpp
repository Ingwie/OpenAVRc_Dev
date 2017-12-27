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


#include "../OpenAVRc.h"
#include "menu_general.h"


void menuGeneralVersion(uint8_t event)
{
#define VERS_ITEMS_COUNT 2

  MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, VERS_ITEMS_COUNT, {0, 3});

  uint8_t sub = menuVerticalPosition - 1;
  uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
  uint8_t attr = (sub == menuVerticalOffset ? blink : 0);

  lcdDrawTextLeft(MENU_HEADER_HEIGHT+FH, vers_stamp);

#define ID_OFFSET 4 + 3*FW

  lcdDrawTextLeft(7*FH, STR_RFID);
  for (uint8_t j=0; j<4; j++) {
    uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
    switch (j) {
    case 0:
      lcd_outhex(2, ID_OFFSET + 0*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[3], rowattr);
      if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[3], 0, 0xFF);
      break;
    case 1:
      lcd_outhex(2, ID_OFFSET + 2*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[2], rowattr);
      if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[2], 0, 0xFF);
      break;
    case 2:
      lcd_outhex(2, ID_OFFSET + 4*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[1], rowattr);
      if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[1], 0, 0xFF);
      break;
    case 3:
      lcd_outhex(2, ID_OFFSET + 6*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[0], rowattr);
      if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[0], 0, 0xFF);
      break;
    }
  }
}
