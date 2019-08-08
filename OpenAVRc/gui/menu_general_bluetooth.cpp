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

enum menuGeneralBTItems
{
  ITEM_BT_TYPE,
  ITEM_BT_NAME,
  ITEM_BT_ROLE,
  ITEM_BT_PEER,
  ITEM_BT_AUTOCONNECT,
  ITEM_BT_END
};
#define BT_Tab 0,0,0,0,0,0


#define STR_BLUETOOTH PSTR("Bluetooth")
#define STR_BTTYPE PSTR("HC-05","HM10")



void menuGeneralBluetooth(uint8_t event)
{
  MENU(STR_BLUETOOTH, menuTabGeneral, e_Bluetooth, ITEM_BT_END, {BT_Tab});

  coord_t y = MENU_HEADER_HEIGHT + 1;
  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<LCD_LINES-1; ++i)
    {
      uint8_t k = i+menuVerticalOffset;
      uint8_t attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);

      switch(k)
        {
        case ITEM_BT_TYPE :

          break;
        case ITEM_BT_NAME :
          lcdDrawTextLeft(y, STR_NAME);
          break;

        case ITEM_BT_ROLE :
          break;
        case ITEM_BT_PEER :
          break;
        case ITEM_BT_AUTOCONNECT :
          break;
        }
      y += FH;
    }
}
