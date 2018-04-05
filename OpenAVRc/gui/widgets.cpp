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

void drawStick(coord_t centrex, int16_t xval, int16_t yval)
{
#define BOX_CENTERY   (LCD_H-9-BOX_WIDTH/2)
#define MARKER_WIDTH  5
  lcd_square(centrex-BOX_WIDTH/2, BOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);
  lcdDrawSolidVerticalLine(centrex, BOX_CENTERY-1, 3);
  lcdDrawSolidHorizontalLine(centrex-1, BOX_CENTERY, 3);
  lcd_square(centrex + (xval/((2*RESX)/(BOX_WIDTH-MARKER_WIDTH))) - MARKER_WIDTH/2, BOX_CENTERY - (yval/((2*RESX)/(BOX_WIDTH-MARKER_WIDTH))) - MARKER_WIDTH/2, MARKER_WIDTH, ROUND);
#undef BOX_CENTERY
#undef MARKER_WIDTH
}

void menu_lcd_onoff(coord_t x, coord_t y, uint8_t value, LcdFlags attr)
{
#if defined(GRAPHICS)
  if (value)
    lcdDrawChar(x+1, y, '#');
  if (attr)
    lcdDrawFilledRect(x, y, 7, 7);
  else
    lcd_square(x, y, 7);
#else
  /* ON / OFF version */
  lcdDrawTextAtIndex(x, y, STR_OFFON, value, attr ? INVERS:0) ;
#endif
}

void displayScreenIndex(uint8_t index, uint8_t count, uint8_t attr)
{
  lcdDrawNumberAttUnit(LCD_W, 0, count, attr);
  coord_t x = 1+LCD_W-FW*(count>9 ? 3 : 2);
  lcdDrawCharAtt(x, 0, '/', attr);
  lcdDrawNumberAttUnit(x, 0, index+1, attr);
}

void displayScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible)
{
  lcdDrawSolidVerticalLineStip(x, y, h, DOTTED);
  coord_t yofs = (h * offset) / count;
  coord_t yhgt = (h * visible) / count;
  if (yhgt + yofs > h)
    yhgt = h - yofs;
  lcdDrawSolidVerticalLineStip(x, y + yofs, yhgt, SOLID, FORCE);
}

void title(const pm_char * s)
{
  lcdDrawTextAtt(0, 0, s, INVERS);
}

select_menu_value_t selectMenuItem(coord_t x, coord_t y, const pm_char *label, const pm_char *values, select_menu_value_t value, select_menu_value_t min, select_menu_value_t max, LcdFlags attr, uint8_t event)
{
  lcdDrawTextColumnLeft(x, y, label);
  if (values) lcdDrawTextAtIndex(x, y, values, value-min, attr);
  if (attr) value = checkIncDec(event, value, min, max, (menuVerticalPositions[0] == 0) ? EE_MODEL : EE_GENERAL);
  return value;
}

uint8_t onoffMenuItem(uint8_t value, coord_t x, coord_t y, const pm_char *label, LcdFlags attr, uint8_t event )
{
#if defined(GRAPHICS)
  menu_lcd_onoff(x, y, value, attr);
  return selectMenuItem(x, y, label, NULL, value, 0, 1, attr, event);
#else
  return selectMenuItem(x, y, label, STR_OFFON, value, 0, 1, attr, event);
#endif
}

int8_t switchMenuItem(coord_t x, coord_t y, int8_t value, LcdFlags attr, uint8_t event)
{
  lcdDrawTextColumnLeft(x, y, STR_SWITCH);
  lcdPutsSwitches(x,  y, value, attr);
  if (attr) CHECK_INCDEC_MODELSWITCH(event, value, SWSRC_FIRST_IN_MIXES, SWSRC_LAST_IN_MIXES, isSwitchAvailableInMixes);
  return value;
}

void displaySlider(coord_t x, coord_t y, uint8_t value, uint8_t max, uint8_t attr)
{
  lcdDrawChar(x+(value*4*FW)/max, y, '$');
  lcdDrawSolidHorizontalLine(x, y+3, 5*FW-1, FORCE);
  if (attr && (!(attr & BLINK) || !BLINK_ON_PHASE)) lcdDrawFilledRect(x, y, 5*FW-1, FH-1);
}

#if   defined(GVARS)
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)
{
  uint16_t delta = GV_GET_GV1_VALUE(max);
  bool invers = (attr & INVERS);

  //TRACE("gvarMenuItem(val=%d min=%d max=%d)", value, min, max);

  if (invers && event == EVT_KEY_LONG(KEY_ENTER)) {
    s_editMode = !s_editMode;
    value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode) : delta);
    eeDirty(EE_MODEL);
  }
  if (GV_IS_GV_VALUE(value, min, max)) {
    if (attr & LEFT)
      attr -= LEFT; /* because of ZCHAR */
    else
      x -= 2*FW+FWNUM;

    int8_t idx = (int16_t) GV_INDEX_CALC_DELTA(value, delta);
    if (invers) {
      idx = checkIncDec(event, idx, -MAX_GVARS, MAX_GVARS-1, EE_MODEL|NO_DBLKEYS);   // disable double keys
    }
    if (idx < 0) {
      value = (int16_t) GV_CALC_VALUE_IDX_NEG(idx, delta);
      idx = -idx;
      lcdDrawCharAtt(x-6, y, '-', attr);
    } else {
      value = (int16_t) GV_CALC_VALUE_IDX_POS(idx, delta);
      idx++;
    }
    lcdDrawStringWithIndex(x, y, STR_GV, idx, attr);
  } else {
    lcdDrawNumberAttUnit(x, y, value, attr);
    if (invers) value = checkIncDec(event, value, min, max, EE_MODEL);
  }
  return value;
}
#else
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)
{
  lcdDrawNumberAttUnit(x, y, value, attr);
  if (attr&INVERS) value = checkIncDec(event, value, min, max, EE_MODEL);
  return value;
}
#endif

#if defined(SDCARD)
char statusLineMsg[STATUS_LINE_LENGTH];
tmr10ms_t statusLineTime = 0;
uint8_t statusLineHeight = 0;

void showStatusLine()
{
  statusLineTime = get_tmr10ms();
}

#define STATUS_LINE_DELAY (3 * 100) /* 3s */
void drawStatusLine()
{
  if (statusLineTime) {
    if ((tmr10ms_t)(get_tmr10ms() - statusLineTime) <= (tmr10ms_t)STATUS_LINE_DELAY) {
      if (statusLineHeight < FH) statusLineHeight++;
    } else if (statusLineHeight) {
      statusLineHeight--;
    } else {
      statusLineTime = 0;
    }

    lcdDrawFilledRect(0, LCD_H-statusLineHeight, LCD_W, FH, SOLID, ERASE);
    lcdDrawTextAtt(5, LCD_H+1-statusLineHeight, statusLineMsg, BSS);
    lcdDrawFilledRect(0, LCD_H-statusLineHeight, LCD_W, FH, SOLID);
  }
}
#endif
