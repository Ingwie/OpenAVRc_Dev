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
#include "menu_model.h"


enum LogicalSwitchFields {
  LS_FIELD_FUNCTION,
  LS_FIELD_V1,
  LS_FIELD_V2,
  LS_FIELD_ANDSW,
  LS_FIELD_COUNT,
  LS_FIELD_LAST = LS_FIELD_COUNT-1
};

#define CSW_1ST_COLUMN  (4*FW-3)
#define CSW_2ND_COLUMN  (8*FW-3)
#define CSW_3RD_COLUMN  (13*FW-6)
#define CSW_4TH_COLUMN  (18*FW+2)


void menuModelLogicalSwitches(uint8_t event)
{
  INCDEC_DECLARE_VARS(EE_MODEL);

  MENU(STR_MENULOGICALSWITCHES, menuTabModel, e_LogicalSwitches, NUM_LOGICAL_SWITCH+1, {0, NAVIGATION_LINE_BY_LINE|LS_FIELD_LAST/*repeated...*/});

  int8_t    sub = menuVerticalPosition - 1;
  horzpos_t horz = menuHorizontalPosition;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    uint8_t attr = (sub==k ? ((s_editMode>0) ? BLINK|INVERS : INVERS)  : 0);
    uint8_t attr1 = (horz==1 ? attr : 0);
    uint8_t attr2 = (horz==2 ? attr : 0);
    LogicalSwitchData * cs = lswAddress(k);

    // CSW name
    uint8_t sw = SWSRC_SW1+k;
    lcdPutsSwitches(0, y, sw, (getSwitch(sw) ? BOLD : 0) | ((sub==k && CURSOR_ON_LINE()) ? INVERS : 0));

    // CSW func
    lcdDrawTextAtIndex(CSW_1ST_COLUMN, y, STR_VCSWFUNC, cs->func, horz==0 ? attr : 0);

    // CSW params
    uint8_t cstate = lswFamily(cs->func);
    int8_t v1_min=0, v1_max=MIXSRC_LAST_TELEM, v2_min=0, v2_max=MIXSRC_LAST_TELEM;
#define v1_val cs->v1

    if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      lcdPutsSwitches(CSW_2ND_COLUMN, y, cs->v1, attr1);
      lcdPutsSwitches(CSW_3RD_COLUMN, y, cs->v2, attr2);
      v1_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES;
      v1_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      v2_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES;
      v2_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
      INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
    } else if (cstate == LS_FAMILY_COMP) {
      putsMixerSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      putsMixerSource(CSW_3RD_COLUMN, y, cs->v2, attr2);
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
      INCDEC_ENABLE_CHECK(isSourceAvailable);
    } else if (cstate == LS_FAMILY_TIMER) {
      lcdDrawNumberAttUnit(CSW_2ND_COLUMN, y, lswTimerValue(cs->v1), LEFT|PREC1|attr1);
      lcdDrawNumberAttUnit(CSW_3RD_COLUMN, y, lswTimerValue(cs->v2), LEFT|PREC1|attr2);
      v1_min = v2_min = -128;
      v1_max = v2_max = 122;
      INCDEC_SET_FLAG(EE_MODEL);
      INCDEC_ENABLE_CHECK(NULL);
    } else {
      putsMixerSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      if (horz == 1) {
        INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
        INCDEC_ENABLE_CHECK(isSourceAvailableInCustomSwitches);
      } else {
        INCDEC_SET_FLAG(EE_MODEL);
        INCDEC_ENABLE_CHECK(NULL);
      }
#if   defined(FRSKY)
      if (v1_val >= MIXSRC_FIRST_TELEM) {
        lcdPutsTelemetryChannelValue(CSW_3RD_COLUMN, y, v1_val - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT|attr2);
        v2_max = maxTelemValue(v1_val - MIXSRC_FIRST_TELEM + 1);
        if (cstate == LS_FAMILY_OFS) {
          v2_min = -128;
          v2_max -= 128;
        } else {
          v2_max = min((uint8_t)127, (uint8_t)v2_max);
          v2_min = -v2_max;
        }
        if (cs->v2 > v2_max) {
          cs->v2 = v2_max;
          eeDirty(EE_MODEL);
        }
      } else {
        lcdDrawNumberAttUnit(CSW_3RD_COLUMN, y, cs->v2, LEFT|attr2);
        {
          v2_min = -LIMIT_EXT_PERCENT;
          v2_max = +LIMIT_EXT_PERCENT;
        }
      }
#else
      if (v1_val >= MIXSRC_FIRST_TELEM) {
        lcdPutsTelemetryChannelValue(CSW_3RD_COLUMN, y, v1_val - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT|attr2);
        v2_min = -128;
        v2_max = 127;
      } else {
        lcdDrawNumberAttUnit(CSW_3RD_COLUMN, y, cs->v2, LEFT|attr2);
        v2_min = -LIMIT_EXT_PERCENT;
        v2_max = +LIMIT_EXT_PERCENT;
      }
#endif
    }

    // CSW AND switch
    uint8_t andsw = cs->andsw;
    if (andsw > SWSRC_LAST_SWITCH) {
      andsw += SWSRC_SW1-SWSRC_LAST_SWITCH-1;
    }
    lcdPutsSwitches(CSW_4TH_COLUMN, y, andsw, horz==LS_FIELD_ANDSW ? attr : 0);


    if ((s_editMode>0 || p1valdiff) && attr) {
      switch (horz) {
      case LS_FIELD_FUNCTION: {
        CHECK_INCDEC_MODELVAR_ZERO(event, cs->func, LS_FUNC_MAX);
        uint8_t new_cstate = lswFamily(cs->func);
        if (cstate != new_cstate) {
          cs->v1 = cs->v2 = (new_cstate==LS_FAMILY_TIMER ? -119/*1.0*/ : 0);
        }
        break;
      }
      case LS_FIELD_V1:
        cs->v1 = CHECK_INCDEC_PARAM(event, v1_val, v1_min, v1_max);
        break;
      case LS_FIELD_V2:
        cs->v2 = CHECK_INCDEC_PARAM(event, cs->v2, v2_min, v2_max);
        if (checkIncDec_Ret) TRACE("v2=%d", cs->v2);
        break;
      case LS_FIELD_ANDSW:
        CHECK_INCDEC_MODELVAR_ZERO(event, cs->andsw, MAX_LS_ANDSW);
        break;
      }
    }
  }
}
