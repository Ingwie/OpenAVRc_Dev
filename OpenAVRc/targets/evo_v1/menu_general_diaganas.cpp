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
#if defined(TX_CAPACITY_MEASUREMENT)
  #define ANAS_ITEMS_COUNT 4
#else
  #define ANAS_ITEMS_COUNT 2
#endif

  SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, ANAS_ITEMS_COUNT);

  STICK_SCROLL_DISABLE();

  for (uint8_t i=0; i<NUMBER_ANALOG; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + (i/2)*FH;
    const uint8_t x_coord[] = {0, 69, 0};
    uint8_t x = x_coord[i%2];
    lcdDrawNumberNAtt(x, y, i, LEADING0|LEFT, 2);
    lcdDrawChar(x+2*FW-2, y, ':');
    lcd_outhex4(x+3*FW-1, y, s_anaFilt[i]);
  }
}

