/*
**************************************************************************
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code named                            *
*             OpenTx - https://github.com/opentx/opentx                  *
*                                                                        *
*                Only AVR code here for lisibility ;-)                   *
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
  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, 1);

  lcdDrawTextLeft(MENU_HEADER_HEIGHT+FH, vers_stamp);

#if defined(COPROCESSOR)
  if (Coproc_valid == 1) {
    lcdDrawTextLeft(6*FH, PSTR("CoPr:"));
    lcd_outdez8(10*FW, 6*FH, Coproc_read);
  } else {
    lcdDrawTextLeft(6*FH, PSTR("CoPr: ---"));
  }
#endif
}
