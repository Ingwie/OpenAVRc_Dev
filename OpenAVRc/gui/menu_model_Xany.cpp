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
#include "menu_model.h"

enum menuModelXanyItems
{
  ITEM_MODEL_NAME_A,
  ITEM_MODEL_ACTIVE_A,
  ITEM_MODEL_CHID_A,
  ITEM_MODEL_REPEATNB_A,
  ITEM_MODEL_ABSAGLSENSOR_A,
  ITEM_MODEL_SEPARATOR,
  ITEM_MODEL_NAME_B,
  ITEM_MODEL_ACTIVE_B,
  ITEM_MODEL_CHID_B,
  ITEM_MODEL_REPEATNB_B,
  ITEM_MODEL_ABSAGLSENSOR_B,
};

void menuModelXany(uint8_t event)
{
#define NUM_LINE_PER_XANY 6
#define MODEL_XANY_MAX_LINES  NUM_X_ANY*NUM_LINE_PER_XANY

#define MODEL_XANY_2ND_COLUMN  (10*FW)


  MENU_TAB({ 0,0,0,0,0,0,0,0,0,0});

  MENU_CHECK(menuTabModel, e_Xany, MODEL_XANY_MAX_LINES);

  TITLE(PSTR("X ANY"));

  uint8_t sub = menuVerticalPosition - 1;
  int8_t editMode = s_editMode;

  coord_t y = MENU_HEADER_HEIGHT + 1;

  for (uint8_t i=0; i<MODEL_XANY_MAX_LINES-5; ++i)
    {
      uint8_t k = i+menuVerticalOffset;

      LcdFlags blink = ((editMode>0) ? BLINK|INVERS : INVERS);
      LcdFlags attr = (sub == k ? blink : 0);

      switch(k)
        {
        case ITEM_MODEL_NAME_A :
          lcdDrawStringWithIndex(0, y, PSTR("Numero "), 1,attr);
          break;

        case ITEM_MODEL_ACTIVE_A :
          ON_OFF_MENU_ITEM(g_model.Xany[0].Active, MODEL_XANY_2ND_COLUMN, y, PSTR("Actif"), attr, event);
          break;

        case ITEM_MODEL_CHID_A :
          lcdDrawStringWithIndex(0, y, PSTR("Voie "), g_model.Xany[0].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[0].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_A :
          lcdDrawStringWithIndex(0, y, PSTR("Nb repetition  "), g_model.Xany[0].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[0].RepeatNb, 3);
          break;

        case ITEM_MODEL_ABSAGLSENSOR_A :
          ON_OFF_MENU_ITEM(g_model.Xany[0].AbsAglSensor, MODEL_XANY_2ND_COLUMN, y, PSTR("Angle"), attr, event);
          break;

        case ITEM_MODEL_SEPARATOR :
          break;

        case ITEM_MODEL_NAME_B :
          lcdDrawStringWithIndex(0, y, PSTR("Numero "), 2,attr);
          break;

        case ITEM_MODEL_ACTIVE_B :
          ON_OFF_MENU_ITEM(g_model.Xany[1].Active, MODEL_XANY_2ND_COLUMN, y, PSTR("Actif"), attr, event);
          break;

        case ITEM_MODEL_CHID_B :
          lcdDrawStringWithIndex(0, y, PSTR("Voie "), g_model.Xany[1].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[1].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_B :
          lcdDrawStringWithIndex(0, y, PSTR("Nb repetition  "), g_model.Xany[1].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[1].RepeatNb, 3);
          break;

        case ITEM_MODEL_ABSAGLSENSOR_B :
          ON_OFF_MENU_ITEM(g_model.Xany[1].AbsAglSensor, MODEL_XANY_2ND_COLUMN, y, PSTR("Angle"), attr, event);
          break;
        }
      y += FH;
    }
}
