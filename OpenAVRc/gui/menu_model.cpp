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


uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcdDrawTextLeft(y, str);
  lcdDrawNumberAttUnit(MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}

uint8_t s_copyMode = 0;
int8_t s_copySrcRow;
int8_t s_copyTgtOfs;
uint8_t s_currIdx;
uint8_t editNameCursorPos = 0;


void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active)
{

  uint8_t mode = 0;
  if ((active) && (s_editMode <= 0)) {
    mode = INVERS;
  }

  lcdDrawSizedTextAtt(x, y, name, size, ZCHAR | FIXEDWIDTH | mode);

  if (active) {

    uint8_t cur = editNameCursorPos;
    if (s_editMode > 0) {

      int8_t c = name[cur];
      int8_t v = c;

      if (p1valdiff || IS_ROTARY_RIGHT(event) || IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP)
          || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP)) {
        v = checkIncDec(event, abs(v), 0, ZCHAR_MAX, 0);
        if (c <= 0) v = -v;
      }

      switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_BREAK:
        if (s_editMode == EDIT_MODIFY_FIELD) {
          s_editMode = EDIT_MODIFY_STRING;
          cur = 0;
        } else if (cur<size-1)
          cur++;
        else
          s_editMode = 0;
        break;
#endif

      case EVT_KEY_BREAK(KEY_LEFT):
        if (cur>0) cur--;
        break;
      case EVT_KEY_BREAK(KEY_RIGHT):
        if (cur<size-1) cur++;
        break;

#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_LONG:
        if (v==0) {
          s_editMode = 0;
          killEvents(event);
          break;
        }
        // no break
#endif

      case EVT_KEY_LONG(KEY_LEFT):
      case EVT_KEY_LONG(KEY_RIGHT):
        if (v>=-26 && v<=26) {
          v = -v; // toggle case
          if (event==EVT_KEY_LONG(KEY_LEFT))
            killEvents(KEY_LEFT);
        }
        break;
      }

      if (c != v) {
        name[cur] = v;
        eeDirty(EE_MODEL);
      }

      lcdDrawCharAtt(x+editNameCursorPos*FW, y, idx2char(v), ERASEBG|INVERS|FIXEDWIDTH);
    } else {
      cur = 0;
    }
    editNameCursorPos = cur;
  }
}

void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active)
{
  lcdDrawTextLeft(y, label);
  editName(x, y, name, size, event, active);
}
