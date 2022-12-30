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

MenuHandlerFunc menuHandlers[5];
uint8_t menuEvent = 0;
uint8_t menuVerticalPositions[4];
uint8_t menuLevel = 0;
uint8_t shared_u8 = 0xFF; // Reusable shared value
uint8_t editNameCursorPos = 0;

void popMenu()
{
  ASSERT(menuLevel>0);
  menuLevel = menuLevel-1;
  menuEvent = EVT_ENTRY_UP;
}

void chainMenu(MenuHandlerFunc newMenu)
{
#if defined(BLUETOOTH)
  if (newMenu == menuGeneralBluetooth)
  {
    (memclear((void*)&ReBuff, sizeof(ReBuff)));
  }
#endif
#if defined(SCC)
  if (newMenu == menuGeneralScc)
  {
    scc_init(); // Will initialize the part of the ReBuff used by SCC (Reusable Buffer)
    ReBuff.Scc.Sbus.CurChId = scc_getCurChId(); // When entering in SCC menu, ask for the current Channel ID (in case SBUS device is connected)
  }
#endif
  menuHandlers[menuLevel] = newMenu;
  menuEvent = EVT_ENTRY;
}

void pushMenu(MenuHandlerFunc newMenu)
{
  killEvents(KEY_ENTER);

  if (menuLevel == 0) {
    if (newMenu == menuGeneralSetup)
      menuVerticalPositions[0] = 1;
    if (newMenu == menuModelSelect)
      menuVerticalPositions[0] = 0;
  } else {
    menuVerticalPositions[menuLevel] = menuVerticalPosition;
  }

  menuLevel++;

  ASSERT(menuLevel < DIM(menuHandlers));

  menuHandlers[menuLevel] = newMenu;
  menuEvent = EVT_ENTRY;
}

void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active, uint8_t msk, uint8_t range)
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
        v = checkIncDec(event, abs(v),(range == RANGE_NUMBER)? 27 : 0, (range == RANGE_NUMBER)? 36 : (range == RANGE_UPPER)? (LEN_STD_CHARS-2) : ZCHAR_MAX, 0);
        if (c <= 0) v = -v;
      }

      switch (event) {
      case EVT_ROTARY_BREAK:
        if (s_editMode == EDIT_MODIFY_FIELD) {
          s_editMode = EDIT_MODIFY_STRING;
          cur = 0;
        } else if (cur<size-1)
          cur++;
        else
          s_editMode = 0;
        break;

      case EVT_KEY_BREAK(KEY_LEFT):
        if (cur>0) cur--;
        break;
      case EVT_KEY_BREAK(KEY_RIGHT):
        if (cur<size-1) cur++;
        break;

      case EVT_ROTARY_LONG:
        if (v==0) {
          s_editMode = 0;
          killEvents(event);
          break;
        }
        // no break

      case EVT_KEY_LONG(KEY_LEFT):
      case EVT_KEY_LONG(KEY_RIGHT):
        if (v>=-26 && v<=26) {
          v = -v; // toggle case
          if (event==EVT_KEY_LONG(KEY_LEFT))
            killEvents(KEY_LEFT);
        }
        break;
      }

      if (range == RANGE_UPPER)
      {
        v = abs(v);
      }
      if (c != v) {
        name[cur] = v;
        eeDirty(msk);
      }

      lcdDrawCharAtt(x+editNameCursorPos*FW, y, idx2char(v), INVERS|FIXEDWIDTH);
    } else {
      cur = 0;
      checkIncDec_Ret = 0;
    }
    editNameCursorPos = cur;
  }
}

void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active, uint8_t msk, uint8_t range)
{
  lcdDrawTextLeft(y, label);
  editName(x, y, name, size, event, active, msk, range);
}
