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
#include "../Xany.h"
#include "menu_model.h"

enum menuModelXanyItems
{
  ITEM_MODEL_NAME_A,
  ITEM_MODEL_ACTIVE_A,
  ITEM_MODEL_CHID_A,
  ITEM_MODEL_REPEATNB_A,
  ITEM_MODEL_ABSAGLSENSOR_A,
  ITEM_MODEL_INPUT_A,
#if (X_ANY >= 2)
  ITEM_MODEL_SEPARATOR,
  ITEM_MODEL_NAME_B,
  ITEM_MODEL_ACTIVE_B,
  ITEM_MODEL_CHID_B,
  ITEM_MODEL_REPEATNB_B,
  ITEM_MODEL_ABSAGLSENSOR_B,
  ITEM_MODEL_INPUT_B,
#endif
#if (X_ANY >= 3)
  ITEM_MODEL_SEPARATOR1,
  ITEM_MODEL_NAME_C,
  ITEM_MODEL_ACTIVE_C,
  ITEM_MODEL_CHID_C,
  ITEM_MODEL_REPEATNB_C,
  ITEM_MODEL_ABSAGLSENSOR_C,
  ITEM_MODEL_INPUT_C,
#endif
#if (X_ANY >= 4)
  ITEM_MODEL_SEPARATOR2,
  ITEM_MODEL_NAME_D,
  ITEM_MODEL_ACTIVE_D,
  ITEM_MODEL_CHID_D,
  ITEM_MODEL_REPEATNB_D,
  ITEM_MODEL_ABSAGLSENSOR_D,
  ITEM_MODEL_INPUT_D,
#endif
};

void menuModelXany(uint8_t event)
{
#define NUM_LINE_PER_XANY 7
#define MODEL_XANY_MAX_LINES  NUM_X_ANY*NUM_LINE_PER_XANY

#define MODEL_XANY_2ND_COLUMN  (10*FW)

  SIMPLE_MENU(STR_X_ANY, menuTabModel, e_Xany, MODEL_XANY_MAX_LINES);

  uint8_t      sub = menuVerticalPosition - 1, ValidMsg;
  int8_t       editMode = s_editMode;
  XanyInfoSt_t XanyInfo;

  coord_t y = MENU_HEADER_HEIGHT + 1;

  for (uint8_t i=0; i<LCD_LINES-1; ++i)
    {
      uint8_t k = i+menuVerticalOffset;

      LcdFlags blink = ((editMode>0) ? BLINK|INVERS : INVERS);
      LcdFlags attr = (sub == k ? blink : 0);

      switch(k)
        {
        case ITEM_MODEL_NAME_A :
          lcdDrawStringWithIndex(0, y, STR_NUMBER, 1, attr);
          break;

        case ITEM_MODEL_ACTIVE_A :
          ON_OFF_MENU_ITEM(g_model.Xany[0].Active, MODEL_XANY_2ND_COLUMN, y, STR_ACTIVED, attr, event);
          break;

        case ITEM_MODEL_CHID_A :
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[0].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[0].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_A :
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[0].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[0].RepeatNb, 3);
          break;

        case ITEM_MODEL_ABSAGLSENSOR_A :
          ON_OFF_MENU_ITEM(g_model.Xany[0].PayloadCfg.Item.AbsAngleSensor, MODEL_XANY_2ND_COLUMN, y, STR_ANGLE_SENSOR, attr, event);
          break;

        case ITEM_MODEL_INPUT_A :
          ValidMsg = Xany_operation(0, XANY_OP_READ_INFO, &XanyInfo);
          /* If ValidMsg == 0, this means the combination is not supported */
          if (ValidMsg)
            {
              lcd_outbin(XanyInfo.SwNb,4*FW,y,XanyInfo.SwValue,attr);
            }
          else
            {
              lcdDrawTextAtt(0, y, STR_INCOMPATIBLE, attr ? attr : BLINK);
            }
          break;
#if (X_ANY >= 2)
        case ITEM_MODEL_SEPARATOR :
          break;

        case ITEM_MODEL_NAME_B :
          lcdDrawStringWithIndex(0, y, STR_NUMBER, 2, attr);
          break;

        case ITEM_MODEL_ACTIVE_B :
          ON_OFF_MENU_ITEM(g_model.Xany[1].Active, MODEL_XANY_2ND_COLUMN, y, STR_ACTIVED, attr, event);
          break;

        case ITEM_MODEL_CHID_B :
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[1].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[1].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_B :
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[1].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[1].RepeatNb, 3);
          break;

        case ITEM_MODEL_ABSAGLSENSOR_B :
          ON_OFF_MENU_ITEM(g_model.Xany[1].PayloadCfg.Item.AbsAngleSensor, MODEL_XANY_2ND_COLUMN, y, STR_ANGLE_SENSOR, attr, event);
          break;

        case ITEM_MODEL_INPUT_B :
          ValidMsg = Xany_operation(1, XANY_OP_READ_INFO, &XanyInfo);
          /* If ValidMsg == 0, this means the combination is not supported */
          if (ValidMsg)
            {
              lcd_outbin(XanyInfo.SwNb,4*FW,y,XanyInfo.SwValue,attr);
            }
          else
            {
              lcdDrawTextAtt(0, y, STR_INCOMPATIBLE, attr ? attr : BLINK);
            }
          break;
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_SEPARATOR1 :
          break;

        case ITEM_MODEL_NAME_C :
          lcdDrawStringWithIndex(0, y, STR_NUMBER, 3, attr);
          break;

        case ITEM_MODEL_ACTIVE_C :
          ON_OFF_MENU_ITEM(g_model.Xany[2].Active, MODEL_XANY_2ND_COLUMN, y, STR_ACTIVED, attr, event);
          break;

        case ITEM_MODEL_CHID_C :
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[2].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[2].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_C :
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[2].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[2].RepeatNb, 3);
          break;

        case ITEM_MODEL_ABSAGLSENSOR_C :
          ON_OFF_MENU_ITEM(g_model.Xany[2].PayloadCfg.Item.AbsAngleSensor, MODEL_XANY_2ND_COLUMN, y, STR_ANGLE_SENSOR, attr, event);
          break;

        case ITEM_MODEL_INPUT_C :
          ValidMsg = Xany_operation(2, XANY_OP_READ_INFO, &XanyInfo);
          /* If ValidMsg == 0, this means the combination is not supported */
          if (ValidMsg)
            {
              lcd_outbin(XanyInfo.SwNb,4*FW,y,XanyInfo.SwValue,attr);
            }
          else
            {
              lcdDrawTextAtt(0, y, STR_INCOMPATIBLE, attr ? attr : BLINK);
            }
          break;
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_SEPARATOR2 :
          break;

        case ITEM_MODEL_NAME_D :
          lcdDrawStringWithIndex(0, y, STR_NUMBER, 4, attr);
          break;

        case ITEM_MODEL_ACTIVE_D :
          ON_OFF_MENU_ITEM(g_model.Xany[3].Active, MODEL_XANY_2ND_COLUMN, y, STR_ACTIVED, attr, event);
          break;

        case ITEM_MODEL_CHID_D :
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[3].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[3].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_D :
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[3].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[3].RepeatNb, 3);
          break;

        case ITEM_MODEL_ABSAGLSENSOR_D :
          ON_OFF_MENU_ITEM(g_model.Xany[3].PayloadCfg.Item.AbsAngleSensor, MODEL_XANY_2ND_COLUMN, y, STR_ANGLE_SENSOR, attr, event);
          break;

        case ITEM_MODEL_INPUT_D :
          ValidMsg = Xany_operation(3, XANY_OP_READ_INFO, &XanyInfo);
          /* If ValidMsg == 0, this means the combination is not supported */
          if (ValidMsg)
            {
              lcd_outbin(XanyInfo.SwNb,4*FW,y,XanyInfo.SwValue,attr);
            }
          else
            {
              lcdDrawTextAtt(0, y, STR_INCOMPATIBLE, attr ? attr : BLINK);
            }
          break;
#endif
        }
      y += FH;
    }
}
