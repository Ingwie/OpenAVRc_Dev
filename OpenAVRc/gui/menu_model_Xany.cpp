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

/*
-- X-Any N°x --
Actif:  [x]              <-- Implémenté par Christophe: OK
Voie:   0-15           <-- Implémenté par Christophe: OK
Répétition: 0/1/2/3 <-- Implémenté par Christophe: OK
Sw:     0/4/8/16      <-- Implémenté par Christophe: OK
1010101010101010<-- Affichage de l'état des 0/4/8/16 inters Implémenté par Christophe: OK (si possible sur même ligne que ci-dessus)
0-360°: [x]           <-- Implémenté par Christophe: OK
      Cur: 1234  Min: 0003  Max: 4093  <-- A implémenter (1234=valeur liée=XanyInfo.Angle; min et max sont des valeurs de calibrations pour plus tard...)
Pot:    [x]             <-- A implémenter (valeur liée=g_model.Xany[xanynumber].PayloadCfg.Item.RotPot=0/1)
         0-255          <-- A implémenter (valeur liée= XanyInfo.RotPotValue; si possible sur même ligne que ci-dessus)
Tx Time: ----- ms    <-- A implémenter (des tirets si combinaison ValidMsg==0, sinon temps de transmission du message, valeur liée=XanyInfo.TxPeriodMs)
*/

enum menuModelXanyItems
{
  ITEM_MODEL_NAME_A,
  ITEM_MODEL_ACTIVE_A,
  ITEM_MODEL_CHID_A,
  ITEM_MODEL_REPEATNB_A,
  ITEM_MODEL_SWITCHS_A,
  ITEM_MODEL_INPUT_A,
  ITEM_MODEL_ABSAGLSENSOR_A,
  ITEM_MODEL_POT_A,
#if (X_ANY >= 2)
  ITEM_MODEL_SEPARATOR,
  ITEM_MODEL_NAME_B,
  ITEM_MODEL_ACTIVE_B,
  ITEM_MODEL_CHID_B,
  ITEM_MODEL_REPEATNB_B,
  ITEM_MODEL_SWITCHS_B,
  ITEM_MODEL_INPUT_B,
  ITEM_MODEL_ABSAGLSENSOR_B,
  ITEM_MODEL_POT_B,
#endif
#if (X_ANY >= 3)
  ITEM_MODEL_SEPARATOR1,
  ITEM_MODEL_NAME_C,
  ITEM_MODEL_ACTIVE_C,
  ITEM_MODEL_CHID_C,
  ITEM_MODEL_REPEATNB_C,
  ITEM_MODEL_SWITCHS_C,
  ITEM_MODEL_INPUT_C,
  ITEM_MODEL_ABSAGLSENSOR_C,
  ITEM_MODEL_POT_C,
#endif
#if (X_ANY >= 4)
  ITEM_MODEL_SEPARATOR2,
  ITEM_MODEL_NAME_D,
  ITEM_MODEL_ACTIVE_D,
  ITEM_MODEL_CHID_D,
  ITEM_MODEL_REPEATNB_D,
  ITEM_MODEL_SWITCHS_D,
  ITEM_MODEL_INPUT_D,
  ITEM_MODEL_ABSAGLSENSOR_D,
  ITEM_MODEL_POT_D,
#endif
};

void menuModelXany(uint8_t event)
{
#define NUM_LINE_PER_XANY 9
#define MODEL_XANY_MAX_LINES  NUM_X_ANY*NUM_LINE_PER_XANY

#define MODEL_XANY_2ND_COLUMN  (10*FW)

  SIMPLE_MENU(STR_X_ANY, menuTabModel, e_Xany, MODEL_XANY_MAX_LINES);

  uint8_t      sub = menuVerticalPosition - 1, num_switchs;
  int8_t       editMode = s_editMode;
  XanyInfoSt_t XanyInfo;

  coord_t y = MENU_HEADER_HEIGHT + 1;

  for (uint8_t i=0; i<LCD_LINES-1; ++i)
    {
      uint8_t k = i+menuVerticalOffset;
      uint8_t xanynumber = k / NUM_LINE_PER_XANY;
      uint8_t ValidMsg = Xany_operation(xanynumber, XANY_OP_READ_INFO, &XanyInfo);
      LcdFlags blink = ((editMode>0) ? BLINK|INVERS : INVERS);
      LcdFlags attr = (sub == k ? blink : 0);

      switch(k)
        {
        case ITEM_MODEL_NAME_A :
          lcdDrawStringWithIndex(0, y, STR_NUMBER, xanynumber + 1, ValidMsg?attr:attr|BLINK);
          break;

        case ITEM_MODEL_ACTIVE_A :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].Active, MODEL_XANY_2ND_COLUMN, y, STR_ACTIVED, attr, event);
          break;

        case ITEM_MODEL_CHID_A :
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[xanynumber].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_A :
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[xanynumber].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].RepeatNb, 3);
          break;

        case ITEM_MODEL_SWITCHS_A :
          num_switchs = (g_model.Xany[xanynumber].PayloadCfg.Item.Switches < 3) ? g_model.Xany[xanynumber].PayloadCfg.Item.Switches*4 : 16;
          lcdDrawStringWithIndex(0, y, STR_SWITCHES, num_switchs, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].PayloadCfg.Item.Switches, 3);
          break;

        case ITEM_MODEL_INPUT_A :
          /* If ValidMsg == 0, this means the combination is not supported */
          if (ValidMsg)
            {
              lcd_outbin(XanyInfo.SwNb,12*FW-3*XanyInfo.SwNb,y,XanyInfo.SwValue, INVERS);
            }
          break;

        case ITEM_MODEL_ABSAGLSENSOR_A :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].PayloadCfg.Item.AbsAngleSensor, MODEL_XANY_2ND_COLUMN, y, STR_ANGLE_SENSOR, attr, event);
          break;

        case ITEM_MODEL_POT_A :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].PayloadCfg.Item.RotPot, MODEL_XANY_2ND_COLUMN, y, STR_POTS, attr, event);
          break;
#if (X_ANY >= 2)
        case ITEM_MODEL_SEPARATOR :
          ++xanynumber;
          break;

        case ITEM_MODEL_NAME_B :
          lcdDrawStringWithIndex(0, y, STR_NUMBER, xanynumber + 1, ValidMsg?attr:attr|BLINK);
          break;

        case ITEM_MODEL_ACTIVE_B :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].Active, MODEL_XANY_2ND_COLUMN, y, STR_ACTIVED, attr, event);
          break;

        case ITEM_MODEL_CHID_B :
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[xanynumber].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_B :
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[xanynumber].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].RepeatNb, 3);
          break;

        case ITEM_MODEL_SWITCHS_B :
          num_switchs = (g_model.Xany[xanynumber].PayloadCfg.Item.Switches < 3) ? g_model.Xany[xanynumber].PayloadCfg.Item.Switches*4 : 16;
          lcdDrawStringWithIndex(0, y, STR_SWITCHES, num_switchs, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].PayloadCfg.Item.Switches, 3);
          break;

        case ITEM_MODEL_INPUT_B :
          /* If ValidMsg == 0, this means the combination is not supported */
          if (ValidMsg)
            {
              lcd_outbin(XanyInfo.SwNb,12*FW-3*XanyInfo.SwNb,y,XanyInfo.SwValue, INVERS);
            }
          break;

        case ITEM_MODEL_ABSAGLSENSOR_B :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].PayloadCfg.Item.AbsAngleSensor, MODEL_XANY_2ND_COLUMN, y, STR_ANGLE_SENSOR, attr, event);
          break;

        case ITEM_MODEL_POT_B :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].PayloadCfg.Item.RotPot, MODEL_XANY_2ND_COLUMN, y, STR_POTS, attr, event);
          break;
#endif
#if (X_ANY >= 3)
        case ITEM_MODEL_SEPARATOR1 :
          ++xanynumber;
          break;

        case ITEM_MODEL_NAME_C :
          lcdDrawStringWithIndex(0, y, STR_NUMBER, xanynumber + 1, ValidMsg?attr:attr|BLINK);
          break;

        case ITEM_MODEL_ACTIVE_C :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].Active, MODEL_XANY_2ND_COLUMN, y, STR_ACTIVED, attr, event);
          break;

        case ITEM_MODEL_CHID_C :
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[xanynumber].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_C :
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[xanynumber].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].RepeatNb, 3);
          break;

        case ITEM_MODEL_SWITCHS_C :
          num_switchs = (g_model.Xany[xanynumber].PayloadCfg.Item.Switches < 3) ? g_model.Xany[xanynumber].PayloadCfg.Item.Switches*4 : 16;
          lcdDrawStringWithIndex(0, y, STR_SWITCHES, num_switchs, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].PayloadCfg.Item.Switches, 3);
          break;

        case ITEM_MODEL_INPUT_C :
          /* If ValidMsg == 0, this means the combination is not supported */
          if (ValidMsg)
            {
              lcd_outbin(XanyInfo.SwNb,12*FW-3*XanyInfo.SwNb,y,XanyInfo.SwValue, INVERS);
            }
          break;

        case ITEM_MODEL_ABSAGLSENSOR_C :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].PayloadCfg.Item.AbsAngleSensor, MODEL_XANY_2ND_COLUMN, y, STR_ANGLE_SENSOR, attr, event);
          break;

        case ITEM_MODEL_POT_C :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].PayloadCfg.Item.RotPot, MODEL_XANY_2ND_COLUMN, y, STR_POTS, attr, event);
          break;
#endif
#if (X_ANY >= 4)
        case ITEM_MODEL_SEPARATOR2 :
          ++xanynumber;
          break;

        case ITEM_MODEL_NAME_D :
          lcdDrawStringWithIndex(0, y, STR_NUMBER, xanynumber + 1, ValidMsg?attr:attr|BLINK);
          break;

        case ITEM_MODEL_ACTIVE_D :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].Active, MODEL_XANY_2ND_COLUMN, y, STR_ACTIVED, attr, event);
          break;

        case ITEM_MODEL_CHID_D :
          lcdDrawStringWithIndex(0, y, STR_CHANNEL, g_model.Xany[xanynumber].ChId+1, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].ChId, NUM_CHNOUT);
          break;

        case ITEM_MODEL_REPEATNB_D :
          lcdDrawStringWithIndex(0, y, STR_NB_REPEAT, g_model.Xany[xanynumber].RepeatNb, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].RepeatNb, 3);
          break;

        case ITEM_MODEL_SWITCHS_D :
          num_switchs = (g_model.Xany[xanynumber].PayloadCfg.Item.Switches < 3) ? g_model.Xany[xanynumber].PayloadCfg.Item.Switches*4 : 16;
          lcdDrawStringWithIndex(0, y, STR_SWITCHES, num_switchs, attr);
          if (attr)
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.Xany[xanynumber].PayloadCfg.Item.Switches, 3);
          break;

        case ITEM_MODEL_INPUT_D :
          /* If ValidMsg == 0, this means the combination is not supported */
          if (ValidMsg)
            {
              lcd_outbin(XanyInfo.SwNb,12*FW-3*XanyInfo.SwNb,y,XanyInfo.SwValue, INVERS);
            }
          break;

        case ITEM_MODEL_ABSAGLSENSOR_D :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].PayloadCfg.Item.AbsAngleSensor, MODEL_XANY_2ND_COLUMN, y, STR_ANGLE_SENSOR, attr, event);
          break;

        case ITEM_MODEL_POT_D :
          ON_OFF_MENU_ITEM(g_model.Xany[xanynumber].PayloadCfg.Item.RotPot, MODEL_XANY_2ND_COLUMN, y, STR_POTS, attr, event);
          break;
#endif
        }
      y += FH;
    }
}
