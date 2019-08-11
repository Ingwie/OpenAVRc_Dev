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
  ITEM_BT_NAME,
  ITEM_BT_ROLE,
  ITEM_BT_PEER,
  ITEM_BT_AUTOCONNECT,
  ITEM_BT_RESCANN,
  ITEM_BT_END
};
#define BT_Tab 0
#define BT_2ND_COLUMN 10*FW

#define STR_BLUETOOTH      PSTR("Bluetooth")
#define STR_BT_SLAVE_EXT   PSTR("_S")
#define STR_BT_MASTER_EXT  PSTR("_M")
#define STR_BT_ROLE        PSTR("Role")
#define STR_BT_M_S         PSTR("\006""Master""Slave\0")
#define STR_BT_PAIR        PSTR("Pair")
#define STR_AUTOCON        PSTR("Auto-con.")
#define STR_RESCANN        PSTR("Re-Scan")


/*     TEMP VALUES       */
uint8_t Master_Slave = 0;// 0 = Master
uint8_t Autocon = 0;// 0 = ON
#define STR_BT_NAME PSTR("RC-NAVY")

/*   Eeprom value
#define BLUETOOTH_FIELDS \
uint8_t BTParams;        \
char BTName[6];          \
char BTSlaveName[6];     \
uint8_t BTSlaveMac[6];   \
*/

void menuGeneralBluetooth(uint8_t event)
{
  MENU(STR_BLUETOOTH, menuTabGeneral, e_Bluetooth, ITEM_BT_END+1, {BT_Tab});

  if (warningResult) {
    warningResult = false;
    // Call RE SCANN BT here
  }
  g_eeGeneral.BTSlaveName[0] = 'T'; // To remove
  g_eeGeneral.BTSlaveName[1] = 'E';
  g_eeGeneral.BTSlaveName[2] = 'S';
  g_eeGeneral.BTSlaveName[3] = 'T';

  uint8_t addExt = 0; // used to add _M or _S
  coord_t y = MENU_HEADER_HEIGHT + 1;
  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<LCD_LINES-1; ++i)
    {
      uint8_t k = i+menuVerticalOffset;
      uint8_t attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);

      switch(k)
        {
        case ITEM_BT_NAME :
          lcdDrawTextLeft(y, STR_NAME);
          lcdDrawTextAtt(BT_2ND_COLUMN,y,STR_BT_NAME,attr);
          addExt = 1;
          break;

        case ITEM_BT_ROLE :
          lcdDrawTextLeft(y, STR_BT_ROLE);
          lcdDrawTextAtIndex(BT_2ND_COLUMN, y, STR_BT_M_S, Master_Slave, attr);
          if (attr)
            Master_Slave = checkIncDecGen(event, Master_Slave, 0, 1);
          break;

        case ITEM_BT_PEER :
          lcdDrawTextLeft(y, STR_BT_PAIR);
          lcdDrawSizedTextAtt(BT_2ND_COLUMN, y, g_eeGeneral.BTSlaveName, sizeof(g_eeGeneral.BTSlaveName), BSS|attr);
          addExt = 2;
          break;

        case ITEM_BT_AUTOCONNECT :
          Autocon = onoffMenuItem(Autocon, BT_2ND_COLUMN, y, STR_AUTOCON, attr, event);
          break;

        case ITEM_BT_RESCANN :
          lcdDrawTextAtt(7*FW,y,STR_RESCANN,attr);
          if (event==EVT_KEY_BREAK(KEY_ENTER))
            {
              POPUP_CONFIRMATION(STR_RESCANN);
              s_editMode = 0;
            }
          break;
        }

      if (addExt)
      {
        lcdDrawTextAtt(lcdNextPos,y,((Master_Slave ^ (addExt & 0x1))? STR_BT_MASTER_EXT : STR_BT_SLAVE_EXT),attr);
        addExt = 0;
      }
      y += FH;
    }
}
