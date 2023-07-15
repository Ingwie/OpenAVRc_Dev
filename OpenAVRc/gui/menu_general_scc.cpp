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

enum menuModelSccItems
{
  ITEM_SCC_CURRENT_CHANNEL,
  ITEM_SCC_SELECT_CHANNEL,
  ITEM_SCC_SBUS_SWEEP_CHECK,
  SCC_ITEMS_COUNT,
};

const pm_char CurChannelLbl[]    PROGMEM = "Actual Channel";
const pm_char NewChannelLbl[]    PROGMEM = "Select Channel";
const pm_char SBusSweepTestLbl[] PROGMEM = "SBus Sweep Test";
const pm_char STR_SCC[]          PROGMEM = "CHANNEL CHANGER";

coord_t y = MENU_HEADER_HEIGHT + 1;

void menuGeneralScc(uint8_t event)
{

 SIMPLE_MENU(STR_SCC, menuTabGeneral, e_Scc, SCC_ITEMS_COUNT+1);
 char    ChIdStr[3];
 uint8_t sub = menuVerticalPosition - 1;
 coord_t y = MENU_HEADER_HEIGHT + 1;
 y += FH;

  for (uint8_t i = 0; i < SCC_ITEMS_COUNT; ++i)
    {
      uint8_t k = i+menuVerticalOffset;
      uint8_t attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      switch(k)
        {
        case ITEM_SCC_CURRENT_CHANNEL :
          lcdDrawText(0, y, CurChannelLbl);
          if(!ReBuff.Scc.Sbus.CurChId)
          {
            // The Channel ID is unknown: display '?'
            ChIdStr[0] = '?'; ChIdStr[1] = ' '; ChIdStr[2] = 0;
            lcdDrawTextAtt(17*FW, y, (const pm_char *)ChIdStr, BSS);
          }
          else
          {
            // The Channel ID is known (between 1 and 16): display it as a number
            lcdDrawNumberNAtt(18*FW, y, (int16_t)ReBuff.Scc.Sbus.CurChId, 0);
          }
          if (attr) putEvent(event); // autojump to next/prev field
          break;

        case ITEM_SCC_SELECT_CHANNEL :
          lcdDrawText(0, y, NewChannelLbl);
          if(!ReBuff.Scc.Sbus.NewChId)
          {
            ChIdStr[0] = '?'; ChIdStr[1] = ' '; ChIdStr[2] = 0;
            lcdDrawTextAtt(17*FW, y, (const pm_char *)ChIdStr, BSS | attr);
          }
          else
          {
            lcdDrawNumberNAtt(18*FW, y, (int16_t)ReBuff.Scc.Sbus.NewChId, attr, 2);
          }
          if (attr)
          {
            CHECK_INCDEC_MODELVAR_ZERO(event, ReBuff.Scc.Sbus.NewChId, SBUS_CHANNEL_NB);
            if(!systemBolls.scc_sweep_on && (event == EVT_KEY_BREAK(KEY_ENTER)))
            {
              ReBuff.Scc.Sbus.CurChId = scc_setChId(ReBuff.Scc.Sbus.NewChId);
            }
          }
          break;

        case ITEM_SCC_SBUS_SWEEP_CHECK :
          ON_OFF_MENU_ITEM(systemBolls.scc_sweep_on, 17*FW, y, SBusSweepTestLbl, attr, event);
          if (!ReBuff.Scc.Sbus.NewChId)
            systemBolls.scc_sweep_on = 0; // The Current Channel ID SHALL not be 0 (Shall within [1-16])
          else
          {
            scc_sbusSweepEnable(systemBolls.scc_sweep_on);
          }
          break;
        }
      y += FH;
    }
}
