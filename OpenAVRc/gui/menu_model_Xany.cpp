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
  ITEM_MODEL_SWITCHS_A,
  ITEM_MODEL_ABS_ANGLE_SRC_A,
  ITEM_MODEL_PROP_SRC_A,
#if (X_ANY >= 2)
  ITEM_MODEL_SEPARATOR,
  ITEM_MODEL_NAME_B,
  ITEM_MODEL_ACTIVE_B,
  ITEM_MODEL_CHID_B,
  ITEM_MODEL_REPEATNB_B,
  ITEM_MODEL_SWITCHS_B,
  ITEM_MODEL_ABS_ANGLE_SRC_B,
  ITEM_MODEL_PROP_SRC_B,
#endif
#if (X_ANY >= 3)
  ITEM_MODEL_SEPARATOR1,
  ITEM_MODEL_NAME_C,
  ITEM_MODEL_ACTIVE_C,
  ITEM_MODEL_CHID_C,
  ITEM_MODEL_REPEATNB_C,
  ITEM_MODEL_SWITCHS_C,
  ITEM_MODEL_ABS_ANGLE_SRC_C,
  ITEM_MODEL_PROP_SRC_C,
#endif
#if (X_ANY >= 4)
  ITEM_MODEL_SEPARATOR2,
  ITEM_MODEL_NAME_D,
  ITEM_MODEL_ACTIVE_D,
  ITEM_MODEL_CHID_D,
  ITEM_MODEL_REPEATNB_D,
  ITEM_MODEL_SWITCHS_D,
  ITEM_MODEL_ABS_ANGLE_SRC_D,
  ITEM_MODEL_PROP_SRC_D,
#endif
};

const pm_uchar zz_XanyCh[] PROGMEM = {
#if defined (LCDROT180)
#include "../bitmaps/XanyCh.lbmi"
#else
#include "../bitmaps/XanyCh.lbm"
#endif
};

void menuModelXany(uint8_t event)
{
#define NUM_LINE_PER_XANY 8
#define MODEL_XANY_MAX_LINES  NUM_X_ANY*NUM_LINE_PER_XANY
#define XANYLINES LABEL(NUMBER),0,0,0,0,0,1
#define XANY1LINES 0, XANYLINES
#if (X_ANY >= 2)
#define XANY2LINES , LABEL(NUMBER) , XANYLINES
#else
#define XANY2LINES
#endif
#if (X_ANY >= 3)
#define XANY3LINES , LABEL(NUMBER) , XANYLINES
#else
#define XANY3LINES
#endif
#if (X_ANY >= 4)
#define XANY4LINES , LABEL(NUMBER) , XANYLINES
#else
#define XANY4LINES
#endif

  MENU(STR_X_ANY, menuTabModel, e_Xany, MODEL_XANY_MAX_LINES, {XANY1LINES XANY2LINES XANY3LINES XANY4LINES});
  uint8_t      AngleSrcIdx2SubImgIdx[] = {0, 1, 6, 7};
  uint8_t      PropSrcIdx2SubImgIdx[]  = {0, 2, 3, 4, 5, 0, 0, 0, 6, 7};
  uint8_t      AngleSrcIdx = 0, PropSrcIdx = 0, SwitchSrcIdx = 0, ExpoPerCentIdx = 0;
  uint8_t      sub = menuVerticalPosition - 1;
  XanyInfoSt_t XanyInfo;

  coord_t y = MENU_HEADER_HEIGHT + 1;

  for (uint8_t i=0; i<LCD_LINES-1; ++i)
    {
      uint8_t k = i+menuVerticalOffset;
      uint8_t xanynumber = k / NUM_LINE_PER_XANY;
      uint8_t ValidMsg = Xany_operation(xanynumber, XANY_OP_READ_INFO, &XanyInfo);
      uint8_t attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);

      switch(k)
        {
        case ITEM_MODEL_NAME_A :
#if (X_ANY >= 2)
        case ITEM_MODEL_NAME_B :
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_NAME_C :
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_NAME_D :
#endif
          lcdDrawStringWithIndex(0, y, STR_NUMBER, xanynumber + 1, ValidMsg?attr:attr|BLINK);
          if (ValidMsg)
          {
            lcdDrawNumberNAtt(19*FW, y, XanyInfo.TxPeriodMs, 0);
            lcdDrawText(19*FW, y, STR_MS);
          }
          break;

        case ITEM_MODEL_ACTIVE_A :
#if (X_ANY >= 2)
        case ITEM_MODEL_ACTIVE_B :
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_ACTIVE_C :
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_ACTIVE_D :
#endif
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].Active, 8*FW, y, STR_ACTIVED, attr, event);
          if (!ValidMsg)
            g_model.Xany[xanynumber].Active = 0;
          break;

        case ITEM_MODEL_CHID_A :
#if (X_ANY >= 2)
        case ITEM_MODEL_CHID_B :
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_CHID_C :
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_CHID_D :
#endif
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[xanynumber].ChId+1, attr);
          if (attr)
          {
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].ChId, NUM_CHNOUT);
#if (PCM_PROTOCOL == YES) &&  defined(X_ANY)
            Pcm_updateXanyChMap();
#endif
          }
          break;

        case ITEM_MODEL_REPEATNB_A :
#if (X_ANY >= 2)
        case ITEM_MODEL_REPEATNB_B :
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_REPEATNB_C :
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_REPEATNB_D :
#endif
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[xanynumber].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].RepeatNb, 3);
          break;

        case ITEM_MODEL_SWITCHS_A :
#if (X_ANY >= 2)
        case ITEM_MODEL_SWITCHS_B :
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_SWITCHS_C :
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_SWITCHS_D :
#endif
          if(g_model.Xany[xanynumber].PayloadCfg.SwitchSrcIdx >= X_ANY_SW_SRC_NB) g_model.Xany[xanynumber].PayloadCfg.SwitchSrcIdx = 0;
          SwitchSrcIdx = g_model.Xany[xanynumber].PayloadCfg.SwitchSrcIdx;
          lcdDrawTextAtIndex(FW/2, y, STR_SWITCHES_VALUES, SwitchSrcIdx, attr);
          if (attr) SwitchSrcIdx = checkIncDecModelZero(event, SwitchSrcIdx, X_ANY_SW_SRC_NB - 1);
          g_model.Xany[xanynumber].PayloadCfg.SwitchSrcIdx = SwitchSrcIdx;
          if (ValidMsg && SwitchSrcIdx)
          {
            lcd_outbin(XanyInfo.SwNb,14*FW-3*XanyInfo.SwNb,y,XanyInfo.SwValue, INVERS);
          }
          break;

        case ITEM_MODEL_ABS_ANGLE_SRC_A :
#if (X_ANY >= 2)
        case ITEM_MODEL_ABS_ANGLE_SRC_B :
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_ABS_ANGLE_SRC_C :
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_ABS_ANGLE_SRC_D :
#endif
          if(g_model.Xany[xanynumber].PayloadCfg.AngleSrcIdx >= X_ANY_ANGLE_SRC_NB) g_model.Xany[xanynumber].PayloadCfg.AngleSrcIdx = 0;
          AngleSrcIdx = g_model.Xany[xanynumber].PayloadCfg.AngleSrcIdx;
          lcdDrawText(0, y, STR_ANGLE_SENSOR);
          AngleSrcIdx = selectMenuSubImg(4*FW+FW/3, y, AngleSrcIdx, 0, X_ANY_ANGLE_SRC_NB - 1, pgm_get_far_address(zz_XanyCh), AngleSrcIdx2SubImgIdx[AngleSrcIdx], attr, event);
          g_model.Xany[xanynumber].PayloadCfg.AngleSrcIdx = AngleSrcIdx;

          if(ValidMsg && AngleSrcIdx)
          {
            lcdDrawNumberNAtt(12*FW, y, XanyInfo.AngleValue, INVERS | UNSIGN | LEADING0, 4);
          }
          break;

        case ITEM_MODEL_PROP_SRC_A :
#if (X_ANY >= 2)
        case ITEM_MODEL_PROP_SRC_B :
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_PROP_SRC_C :
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_PROP_SRC_D :
#endif
          PropSrcIdx = g_model.Xany[xanynumber].PayloadCfg.PropSrcIdx;
          if IS_IN_RANGE(PropSrcIdx, X_ANY_PROP_SRC_P1, X_ANY_PROP_SRC_P3)
          {
            /* Use strings for P1 to P3 */
            PropSrcIdx  = selectMenuItem(5*FW+FW/3, y, STR_PROP, STR_VSRCRAW, PropSrcIdx - X_ANY_PROP_SRC_P1 + NUM_STICKS + 1, 0, X_ANY_PROP_SRC_P3 - X_ANY_PROP_SRC_P1 + NUM_STICKS + 2, menuHorizontalPosition==0 ? attr : 0, s_editMode>0 ? event : 0);
            PropSrcIdx += X_ANY_PROP_SRC_P1 -NUM_STICKS - 1;
          }
          else
          {
            /* Use bitmaps for all other possibilities */
            lcdDrawText(0, y, STR_PROP);
            PropSrcIdx = selectMenuSubImg(5*FW, y, PropSrcIdx, 0, X_ANY_PROP_SRC_NB - 1, pgm_get_far_address(zz_XanyCh), PropSrcIdx2SubImgIdx[PropSrcIdx], menuHorizontalPosition==0 ? attr : 0, s_editMode>0 ? event : 0);
          }
          g_model.Xany[xanynumber].PayloadCfg.PropSrcIdx = PropSrcIdx;
          if(ValidMsg && PropSrcIdx)
          {
            lcdDrawNumberNAtt(12*FW, y, XanyInfo.PropValue, INVERS | UNSIGN | LEADING0, 3);
            lcdDrawText(13*FW, y, STR_EXPO);
            ExpoPerCentIdx = g_model.Xany[xanynumber].Param.Expo.PerCentIdx;
            lcdDrawTextAtIndex(17*FW+FW/2, y, STR_XANY_EXPO, ExpoPerCentIdx, menuHorizontalPosition==1 ? attr : 0);
            if (menuHorizontalPosition==1 && (s_editMode>0))
            {
              CHECK_INCDEC_MODELVAR_ZERO(event, ExpoPerCentIdx, X_ANY_EXPO_PER_CENT_NB - 1);
              g_model.Xany[xanynumber].Param.Expo.PerCentIdx = ExpoPerCentIdx;
            }
          }
          else if (menuHorizontalPosition==1)
          {
           REPEAT_LAST_CURSOR_MOVE(); // Next field field if not valid
          }
          break;

#if (X_ANY >= 2)
        case ITEM_MODEL_SEPARATOR :
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_SEPARATOR1 :
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_SEPARATOR2 :
#endif
          break;
        }
      y += FH;
    }
}
