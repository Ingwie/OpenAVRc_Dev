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
#include "popups.h"

void displayBox()
{
  lcdDrawFilledRect(10, 16, LCD_W-20, 40, SOLID, ERASE);
  lcdDrawRect(10, 16, LCD_W-20, 40);
  lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y, warningText);
  // could be a place for a warningInfoText
}

void displayPopup(const pm_char * pstr)
{
  warningText = pstr;
  displayBox();
  warningText = NULL;
  lcdRefresh();
}

void message(const pm_char *title, const pm_char *t, const char *last MESSAGE_SOUND_ARG)
{
  lcdClear();
  lcd_img(2, 0, asterisk_lbm, 0, 0);

#define MESSAGE_LCD_OFFSET   6*FW

#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_IT) || defined(TRANSLATIONS_CZ)
  lcdDrawTextAtt(MESSAGE_LCD_OFFSET, 0, STR_WARNING, DBLSIZE);
  lcdDrawTextAtt(MESSAGE_LCD_OFFSET, 2*FH, title, DBLSIZE);
#else
  lcdDrawTextAtt(MESSAGE_LCD_OFFSET, 0, title, DBLSIZE);
  lcdDrawTextAtt(MESSAGE_LCD_OFFSET, 2*FH, STR_WARNING, DBLSIZE);
#endif

  lcdDrawFilledRect(0, 0, LCD_W, 32);
  if (t) lcdDrawTextLeft(5*FH, t);
  if (last) {
    lcdDrawTextLeft(7*FH, last);
    AUDIO_ERROR_MESSAGE(sound);
  }

#undef MESSAGE_LCD_OFFSET

  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
}

void displayWarning(uint8_t event)
{
  warningResult = false;
  displayBox();
  if (warningInfoText) {
    lcdDrawSizedTextAtt(WARNING_LINE_X, WARNING_LINE_Y+FH, warningInfoText, warningInfoLength, WARNING_INFO_FLAGS);
  }
  lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y+2*FH, warningType == WARNING_TYPE_ASTERISK ? STR_EXIT : STR_POPUPS);
  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_BREAK:
#endif
  case EVT_KEY_BREAK(KEY_ENTER):
    if (warningType == WARNING_TYPE_ASTERISK)
      break;
    warningResult = true;
    // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_LONG:
    killEvents(event);
#endif
  case EVT_KEY_BREAK(KEY_EXIT):
    warningText = NULL;
    warningType = WARNING_TYPE_ASTERISK;
    break;
  }
}


#if defined(NAVIGATION_MENUS)
void (*popupMenuHandler)(const char *result);

const char * displayPopupMenu(uint8_t event)
{
  const char * result = NULL;

  uint8_t display_count = min<uint8_t>(popupMenuNoItems, POPUP_MENU_MAX_LINES);
  uint8_t y = (display_count >= 5 ? MENU_Y - FH - 1 : MENU_Y);
  lcdDrawFilledRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2, SOLID, ERASE);
  lcdDrawRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2);

  for (uint8_t i=0; i<display_count; i++) {
    lcdDrawTextAtt(MENU_X+6, i*(FH+1) + y + 2, popupMenuItems[i], popupMenuFlags);
    if (i == s_menu_item) lcdDrawFilledRect(MENU_X+1, i*(FH+1) + y + 1, MENU_W-2, 9);
  }

  if (popupMenuNoItems > display_count) {
    displayScrollbar(MENU_X+MENU_W-1, y+1, POPUP_MENU_MAX_LINES * (FH+1), popupMenuOffset, popupMenuNoItems, POPUP_MENU_MAX_LINES);
  }

  switch(event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LEFT
#endif
  case EVT_KEY_FIRST(KEY_MOVE_UP):
  case EVT_KEY_REPT(KEY_MOVE_UP):
    if (s_menu_item > 0) {
      s_menu_item--;
    }
#if defined(SDCARD)
    else if (popupMenuOffset > 0) {
      popupMenuOffset--;
      result = STR_UPDATE_LIST;
    }
#endif
    else {
      s_menu_item = display_count - 1;
#if defined(SDCARD)
      if (popupMenuNoItems > POPUP_MENU_MAX_LINES) {
        popupMenuOffset = popupMenuNoItems - display_count;
        result = STR_UPDATE_LIST;
      }
#endif
    }
    break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_RIGHT
#endif
  case EVT_KEY_FIRST(KEY_MOVE_DOWN):
  case EVT_KEY_REPT(KEY_MOVE_DOWN):
    if (s_menu_item < display_count - 1 && popupMenuOffset + s_menu_item + 1 < popupMenuNoItems) {
      s_menu_item++;
    }
#if defined(SDCARD)
    else if (popupMenuNoItems > popupMenuOffset + display_count) {
      popupMenuOffset++;
      result = STR_UPDATE_LIST;
    }
#endif
    else {
      s_menu_item = 0;
#if defined(SDCARD)
      if (popupMenuOffset) {
        popupMenuOffset = 0;
        result = STR_UPDATE_LIST;
      }
#endif
    }
    break;
    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_BREAK(KEY_ENTER):
    result = popupMenuItems[s_menu_item];
    // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LONG
    killEvents(event);
#endif
  case EVT_KEY_BREAK(KEY_EXIT):
    popupMenuNoItems = 0;
    s_menu_item = 0;
    popupMenuFlags = 0;
    popupMenuOffset = 0;
    break;
  }

  return result;
}
#endif
