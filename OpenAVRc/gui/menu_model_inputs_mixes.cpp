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
#include "widgets.h"


#define EXPO_ONE_2ND_COLUMN (7*FW+3*FW+2)
#define EXPO_ONE_FM_WIDTH   (5*FW)

#if defined(FLIGHT_MODES)
void displayFlightModes(coord_t x, coord_t y, FlightModesType value);
FlightModesType editFlightModes(coord_t x, coord_t y, uint8_t event, FlightModesType value, uint8_t attr)
{
  lcdDrawTextColumnLeft(x, y, STR_FLMODE);

  uint8_t posHorz = menuHorizontalPosition;

  for (uint8_t p=0; p<MAX_FLIGHT_MODES; p++) {
    lcdDrawCharAtt(x, y, '0'+p, ((posHorz==p) && attr) ? BLINK|INVERS : ((value & (1<<p)) ? 0 : INVERS));
    x += FW;
  }

  if (attr) {
    if (s_editMode && ((event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff))) {
      s_editMode = 0;
      value ^= (1<<posHorz);
      eeDirty(EE_MODEL);
    }
  }

  return value;
}
#else
#define displayFlightModes(...)
#endif

int16_t expoFn(int16_t x)
{
  ExpoData *ed = expoAddress(s_currIdx);
  int16_t anas[NUM_STICKS] = {0};
  anas[ed->chn] = x;
  applyExpos(anas, e_perout_mode_inactive_flight_mode);
  return anas[ed->chn];
}

void DrawFunction(FnFuncP fn, uint8_t offset)
{
  lcdDrawSolidVerticalLineStip(X0-offset, 0, LCD_H, 0xee);
  lcdDrawSolidHorizontalLineStip(X0-WCHART-offset, Y0, WCHART*2, 0xee);

  coord_t prev_yv = (coord_t)-1;

  for (int8_t xv=-WCHART; xv<=WCHART; xv++) {
    coord_t yv = (LCD_H-1) - (((uint16_t)RESX + fn(xv * (RESX/WCHART))) / 2 * (LCD_H-1) / RESX);
    if (prev_yv != (coord_t)-1) {
      if (abs((int8_t)yv-prev_yv) <= 1) {
        lcdDrawPoint(X0+xv-offset-1, prev_yv, FORCE);
      } else {
        uint8_t tmp = (prev_yv < yv ? 0 : 1);
        lcdDrawSolidVerticalLine(X0+xv-offset-1, yv+tmp, prev_yv-yv);
      }
    }
    prev_yv = yv;
  }
}


uint8_t getExpoMixCount(uint8_t expo)
{
  uint8_t count = 0;

  for(int8_t i=(expo ? MAX_EXPOS-1 : MAX_MIXERS-1); i>=0; i--) {
    uint8_t ch = (expo ? EXPO_VALID(expoAddress(i)) : mixAddress(i)->srcRaw);
    if (ch != 0) {
      count++;
    }
  }
  return count;
}

uint8_t reachExpoMixCountLimit(uint8_t expo)
{
  // check mixers count limit
  if (getExpoMixCount(expo) >= (expo ? MAX_EXPOS : MAX_MIXERS)) {
    POPUP_WARNING(expo ? STR_NOFREEEXPO : STR_NOFREEMIXER);
    return true;
  }
  return false;
}

void deleteExpoMix(uint8_t expo, uint8_t idx)
{
  if (expo) {
    ExpoData *expo = expoAddress(idx);
    memmove(expo, expo+1, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(&g_model.expoData[MAX_EXPOS-1], sizeof(ExpoData));
  } else {
    MixData *mix = mixAddress(idx);
    memmove(mix, mix+1, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(&g_model.mixData[MAX_MIXERS-1], sizeof(MixData));
  }
  eeDirty(EE_MODEL);
}

int8_t s_currCh;
void insertExpoMix(uint8_t expo, uint8_t idx)
{
  if (expo) {
    ExpoData *expo = expoAddress(idx);
    memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(expo, sizeof(ExpoData));
    expo->mode = 3; // pos&neg
    expo->chn = s_currCh - 1;
    expo->weight = 100;
  } else {
    MixData *mix = mixAddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(mix, sizeof(MixData));
    mix->destCh = s_currCh-1;
    mix->srcRaw = (s_currCh > 4 ? MIXSRC_Rud - 1 + s_currCh : MIXSRC_Rud - 1 + channel_order(s_currCh));
    mix->weight = 100;
  }
  eeDirty(EE_MODEL);
}

void copyExpoMix(uint8_t expo, uint8_t idx)
{
  if (expo) {
    ExpoData *expo = expoAddress(idx);
    memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  } else {
    MixData *mix = mixAddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
  }
  eeDirty(EE_MODEL);
}

void memswap(void *a, void *b, uint8_t size)
{
  uint8_t *x = (uint8_t*)a;
  uint8_t *y = (uint8_t*)b;

  while (size--) {
    uint8_t temp = *x;
    *x++ = *y;
    *y++ = temp;
  }
}

uint8_t swapExpoMix(uint8_t expo, uint8_t &idx, uint8_t up)
{
  void *x, *y;
  uint8_t size;
  int8_t tgt_idx = (up ? idx-1 : idx+1);

  if (expo) {
    x = (ExpoData *)expoAddress(idx);

    if (tgt_idx < 0) {
      if (((ExpoData *)x)->chn == 0)
        return false;
      ((ExpoData *)x)->chn--;
      return true;
    }

    if (tgt_idx == MAX_EXPOS) {
      if (((ExpoData *)x)->chn == NUM_STICKS-1)
        return false;
      ((ExpoData *)x)->chn++;
      return true;
    }

    y = (ExpoData *)expoAddress(tgt_idx);
    if(((ExpoData *)x)->chn != ((ExpoData *)y)->chn || !EXPO_VALID((ExpoData *)y)) {
      if (up) {
        if (((ExpoData *)x)->chn>0) ((ExpoData *)x)->chn--;
        else return false;
      } else {
        if (((ExpoData *)x)->chn<NUM_STICKS-1) ((ExpoData *)x)->chn++;
        else return false;
      }
      return true;
    }

    size = sizeof(ExpoData);
  } else {
    x = (MixData *)mixAddress(idx);

    if (tgt_idx < 0) {
      if (((MixData *)x)->destCh == 0)
        return false;
      ((MixData *)x)->destCh--;
      return true;
    }

    if (tgt_idx == MAX_MIXERS) {
      if (((MixData *)x)->destCh == NUM_CHNOUT-1)
        return false;
      ((MixData *)x)->destCh++;
      return true;
    }

    y = (MixData *)mixAddress(tgt_idx);
    uint8_t destCh = ((MixData *)x)->destCh;
    if(!((MixData *)y)->srcRaw || destCh != ((MixData *)y)->destCh) {
      if (up) {
        if (destCh>0) ((MixData *)x)->destCh--;
        else return false;
      } else {
        if (destCh<NUM_CHNOUT-1) ((MixData *)x)->destCh++;
        else return false;
      }
      return true;
    }

    size = sizeof(MixData);
  }
  memswap(x, y, size);
  idx = tgt_idx;
  return true;
}

enum ExposFields {
  EXPO_FIELD_WEIGHT,
  EXPO_FIELD_EXPO,
  CASE_CURVES(EXPO_FIELD_CURVE)
  CASE_FLIGHT_MODES(EXPO_FIELD_FLIGHT_MODES)
  EXPO_FIELD_SWITCH,
  EXPO_FIELD_SIDE,
  EXPO_FIELD_MAX
};

#define CURVE_ROWS 0

void menuModelExpoOne(uint8_t event)
{
  ExpoData *ed = expoAddress(s_currIdx);
  putsMixerSource(7*FW+FW/2, 0, MIXSRC_Rud+ed->chn, 0);

  SUBMENU(STR_MENUINPUTS, EXPO_FIELD_MAX, {0, 0, CASE_CURVES(CURVE_ROWS) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/});

  SET_SCROLLBAR_X(EXPO_ONE_2ND_COLUMN+10*FW);

  int8_t sub = menuVerticalPosition;

  coord_t y = MENU_HEADER_HEIGHT + 1;

  for (uint8_t i=0; i<EXPO_FIELD_MAX+1; i++) {
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {

    case EXPO_FIELD_WEIGHT:
      lcdDrawTextLeft(y, STR_WEIGHT);
      ed->weight = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr, 0, event);
      break;

    case EXPO_FIELD_EXPO:
      lcdDrawTextLeft(y, STR_EXPO);
      if (ed->curveMode==MODE_EXPO || ed->curveParam==0) {
        ed->curveMode = MODE_EXPO;
        ed->curveParam = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->curveParam, -100, 100, attr, 0, event);
      } else {
        lcdDrawTextAtt(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_NA, attr);
      }
      break;

#if defined(CURVES)
    case EXPO_FIELD_CURVE:
      lcdDrawTextLeft(y, STR_CURVE);
      if (ed->curveMode!=MODE_EXPO || ed->curveParam==0) {
        lcdDrawCurveName(EXPO_ONE_2ND_COLUMN-3*FW, y, ed->curveParam, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, ed->curveParam, CURVE_BASE+MAX_CURVES-1);
          if (ed->curveParam) ed->curveMode = MODE_CURVE;
          if (ed->curveParam>=CURVE_BASE && event==EVT_KEY_LONG(KEY_ENTER)) {
            s_curveChan = ed->curveParam - CURVE_BASE;
            pushMenu(menuModelCurveOne);
          }
        }
      } else {
        lcdDrawTextAtt(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_NA, attr);
      }
      break;
#endif

#if defined(FLIGHT_MODES)
    case EXPO_FIELD_FLIGHT_MODES:
      ed->flightModes = editFlightModes(EXPO_ONE_2ND_COLUMN-EXPO_ONE_FM_WIDTH, y, event, ed->flightModes, attr);
      break;
#endif

    case EXPO_FIELD_SWITCH:
      ed->swtch = switchMenuItem(EXPO_ONE_2ND_COLUMN-3*FW, y, ed->swtch, attr, event);
      break;

    case EXPO_FIELD_SIDE:
      ed->mode = 4 - selectMenuItem(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_SIDE, STR_VSIDE, 4-ed->mode, 1, 3, attr, event);
      break;
    }
    y += FH;
  }

  DrawFunction(expoFn);

  int16_t x512 = calibratedStick[ed->chn];
  lcdDrawNumberNAtt(LCD_W-8, 6*FH, calcRESXto100(x512), 0);
  int16_t y512 = expoFn(x512);
  lcdDrawNumberNAtt(LCD_W-8-6*FW, 1*FH, calcRESXto100(y512), 0);

  x512 = X0+x512/(RESXu/WCHART);
  y512 = (LCD_H-1) - (uint16_t)((y512+RESX)/2) * (LCD_H-1) / RESX;

  lcdDrawSolidVerticalLine(x512, y512-3, 3*2+1);
  lcdDrawSolidHorizontalLine(x512-3, y512, 3*2+1);
}

enum MixFields {
  MIX_FIELD_SOURCE,
  MIX_FIELD_WEIGHT,
  MIX_FIELD_OFFSET,
  MIX_FIELD_TRIM,
  CASE_CURVES(MIX_FIELD_CURVE)
  CASE_FLIGHT_MODES(MIX_FIELD_FLIGHT_PHASE)
  MIX_FIELD_SWITCH,
  MIX_FIELD_WARNING,
  MIX_FIELD_MLTPX,
  MIX_FIELD_DELAY_UP,
  MIX_FIELD_DELAY_DOWN,
  MIX_FIELD_SLOW_UP,
  MIX_FIELD_SLOW_DOWN,
  MIX_FIELD_COUNT
};

void gvarWeightItem(coord_t x, coord_t y, MixData *md, uint8_t attr, uint8_t event)
{
  u_int8int16_t weight;
  MD_WEIGHT_TO_UNION(md, weight);
  weight.gvword = GVAR_MENU_ITEM(x, y, weight.gvword, GV_RANGELARGE_WEIGHT_NEG, GV_RANGELARGE_WEIGHT, attr, 0, event);
  MD_UNION_TO_WEIGHT(weight, md);
}

#define GAUGE_WIDTH  33
#define GAUGE_HEIGHT 6
void drawOffsetBar(uint8_t x, uint8_t y, MixData * md)
{
  int16_t offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
  int16_t weight = abs(GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode));
  int16_t barMin = 0;
  int16_t barMax = 0;

#if defined(OFFSET_ON_INPUT)
  //Offset on input (before weight)
  barMin = (-100 + offset) * weight / 100;
  barMax = (+100 + offset) * weight / 100;

#else
  //Offset on output (after weight)
  barMin = offset - weight;
  barMax = offset + weight;

#endif
  if (y > 15) {
    lcdDrawNumberNAtt(x-((barMin >= 0) ? 2 : 3), y-8, barMin, LEFT);
    lcdDrawNumberNAtt(x+GAUGE_WIDTH+1, y-8, barMax);
  }
  if (barMin < -101)
    barMin = -101;
  if (barMax > 101)
    barMax = 101;
  lcdDrawSolidHorizontalLineStip(x-2, y, GAUGE_WIDTH+2, DOTTED);
  lcdDrawSolidHorizontalLineStip(x-2, y+GAUGE_HEIGHT, GAUGE_WIDTH+2, DOTTED);
  lcdDrawSolidVerticalLine(x-2, y+1, GAUGE_HEIGHT-1);
  lcdDrawSolidVerticalLine(x+GAUGE_WIDTH-1, y+1, GAUGE_HEIGHT-1);
  if (barMin <= barMax) {
    int8_t right = (barMax * GAUGE_WIDTH) / 200;
    int8_t left = ((barMin * GAUGE_WIDTH) / 200)-1;
    lcdDrawFilledRect(x+GAUGE_WIDTH/2+left, y+2, right-left, GAUGE_HEIGHT-3);
  }
  lcdDrawSolidVerticalLine(x+GAUGE_WIDTH/2-1, y, GAUGE_HEIGHT+1);
  if (barMin == -101) {
    for (uint8_t i=0; i<3; ++i) {
      lcdDrawPoint(x+i, y+4-i);
      lcdDrawPoint(x+3+i, y+4-i);
    }
  }
  if (barMax == 101) {
    for (uint8_t i=0; i<3; ++i) {
      lcdDrawPoint(x+GAUGE_WIDTH-8+i, y+4-i);
      lcdDrawPoint(x+GAUGE_WIDTH-5+i, y+4-i);
    }
  }
}
#undef GAUGE_WIDTH
#undef GAUGE_HEIGHT

void menuModelMixOne(uint8_t event)
{
  TITLE(s_currCh ? STR_INSERTMIX : STR_EDITMIX);
  MixData *md2 = mixAddress(s_currIdx) ;
  putsChn(lcdLastPos+1*FW, 0, md2->destCh+1,0);

#if defined(CURVES)
  if ((menuVerticalPosition == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS) || (menuVerticalPosition == MIX_FIELD_CURVE && md2->curveMode == MODE_CURVE))
#else
  if (menuVerticalPosition == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS)
#endif
    SUBMENU_NOTITLE(MIX_FIELD_COUNT, {0, 0, 0, 0, CASE_CURVES(0) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/})
    else
      SUBMENU_NOTITLE(MIX_FIELD_COUNT, {0, 0, 0, 1, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/});

#if MENU_COLUMNS > 1
  lcdDrawSolidVerticalLine(MENU_COLUMN2_X-4, FH+1, LCD_H-FH-1);
#endif

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

  for (uint8_t k=0; k<MENU_COLUMNS*(LCD_LINES-1); k++) {

#if MENU_COLUMNS > 1
    coord_t y;
    coord_t COLUMN_X;
    if (k >= LCD_LINES-1) {
      y = 1 + (k-LCD_LINES+2)*FH;
      COLUMN_X = MENU_COLUMN2_X;
    } else {
      y = 1 + (k+1)*FH;
      COLUMN_X = 0;
    }
    int8_t i = k;
#else
    coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
    int8_t i = k + menuVerticalOffset;
#endif

    uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {

    case MIX_FIELD_SOURCE:
      lcdDrawTextColumnLeft(COLUMN_X, y, NO_INDENT(STR_SOURCE));
      putsMixerSource(COLUMN_X+MIXES_2ND_COLUMN, y, md2->srcRaw, attr);
      if (attr) CHECK_INCDEC_MODELSOURCE(event, md2->srcRaw, 1, MIXSRC_LAST);
      break;
    case MIX_FIELD_WEIGHT:
      lcdDrawTextColumnLeft(COLUMN_X, y, STR_WEIGHT);
      gvarWeightItem(COLUMN_X+MIXES_2ND_COLUMN, y, md2, attr|LEFT, event);
      break;
    case MIX_FIELD_OFFSET: {
      lcdDrawTextColumnLeft(COLUMN_X, y, NO_INDENT(STR_OFFSET));
      u_int8int16_t offset;
      MD_OFFSET_TO_UNION(md2, offset);
      offset.gvword = GVAR_MENU_ITEM(COLUMN_X+MIXES_2ND_COLUMN, y, offset.gvword, GV_RANGELARGE_OFFSET_NEG, GV_RANGELARGE_OFFSET, attr|LEFT, 0, event);
      MD_UNION_TO_OFFSET(offset, md2);
      drawOffsetBar(COLUMN_X+MIXES_2ND_COLUMN+22, y, md2);
      break;
    }
    case MIX_FIELD_TRIM: {
      uint8_t not_stick = (md2->srcRaw > NUM_STICKS);
      int8_t carryTrim = -md2->carryTrim;
      lcdDrawTextColumnLeft(COLUMN_X, y, STR_TRIM);
      lcdDrawTextAtIndex((not_stick ? COLUMN_X+MIXES_2ND_COLUMN : COLUMN_X+6*FW-3), y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, menuHorizontalPosition==0 ? attr : 0);
      if (attr && menuHorizontalPosition==0 && (not_stick || editMode>0)) md2->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
      if (!not_stick) {
        lcdDrawText(COLUMN_X+MIXES_2ND_COLUMN, y, STR_DREX);
        menu_lcd_onoff(COLUMN_X+MIXES_2ND_COLUMN+DREX_CHBOX_OFFSET, y, !md2->noExpo, menuHorizontalPosition==1 ? attr : 0);
        if (attr && menuHorizontalPosition==1 && editMode>0) md2->noExpo = !checkIncDecModel(event, !md2->noExpo, 0, 1);
      } else if (attr) {
        REPEAT_LAST_CURSOR_MOVE();
      }
      break;
    }

#if defined(CURVES)
    case MIX_FIELD_CURVE: {
      lcdDrawTextColumnLeft(COLUMN_X, y, STR_CURVE);
      int8_t curveParam = md2->curveParam;
      if (md2->curveMode == MODE_CURVE) {
        lcdDrawCurveName(COLUMN_X+MIXES_2ND_COLUMN, y, curveParam, attr);
        if (attr) {
          if (event==EVT_KEY_LONG(KEY_ENTER) && (curveParam<0 || curveParam>=CURVE_BASE)) {
            s_curveChan = (curveParam<0 ? -curveParam-1 : curveParam-CURVE_BASE);
            pushMenu(menuModelCurveOne);
          } else {
            CHECK_INCDEC_MODELVAR(event, md2->curveParam, -MAX_CURVES, CURVE_BASE+MAX_CURVES-1);
            if (md2->curveParam == 0)
              md2->curveMode = MODE_DIFFERENTIAL;
          }
        }
      } else {
        lcdDrawTextAtt(COLUMN_X+MIXES_2ND_COLUMN, y, PSTR("Diff"), menuHorizontalPosition==0 ? attr : 0);
        md2->curveParam = GVAR_MENU_ITEM(COLUMN_X+MIXES_2ND_COLUMN+5*FW, y, curveParam, -100, 100, LEFT|(menuHorizontalPosition==1 ? attr : 0), 0, editMode>0 ? event : 0);
        if (attr && editMode>0 && menuHorizontalPosition==0) {
          int8_t tmp = 0;
          CHECK_INCDEC_MODELVAR(event, tmp, -1, 1);
          if (tmp != 0) {
            md2->curveMode = MODE_CURVE;
            md2->curveParam = tmp;
          }
        }
      }
      break;
    }
#endif
#if defined(FLIGHT_MODES)
    case MIX_FIELD_FLIGHT_PHASE:
      md2->flightModes = editFlightModes(COLUMN_X+MIXES_2ND_COLUMN, y, event, md2->flightModes, attr);
      break;
#endif
    case MIX_FIELD_SWITCH:
      md2->swtch = switchMenuItem(COLUMN_X+MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
      break;
    case MIX_FIELD_WARNING:
      lcdDrawTextColumnLeft(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MIXWARNING);
      if (md2->mixWarn)
        lcdDrawNumberNAtt(COLUMN_X+MIXES_2ND_COLUMN, y, md2->mixWarn, attr|LEFT);
      else
        lcdDrawTextAtt(COLUMN_X+MIXES_2ND_COLUMN, y, STR_OFF, attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, md2->mixWarn, 3);
      break;
    case MIX_FIELD_MLTPX:
      md2->mltpx = selectMenuItem(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MULTPX, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
      break;
    case MIX_FIELD_DELAY_UP:
      md2->delayUp = editDelay(y, event, attr, STR_DELAYUP, md2->delayUp);
      break;
    case MIX_FIELD_DELAY_DOWN:
      md2->delayDown = editDelay(y, event, attr, STR_DELAYDOWN, md2->delayDown);
      break;
    case MIX_FIELD_SLOW_UP:
      md2->speedUp = editDelay(y, event, attr, STR_SLOWUP, md2->speedUp);
      break;
    case MIX_FIELD_SLOW_DOWN:
      md2->speedDown = editDelay(y, event, attr, STR_SLOWDOWN, md2->speedDown);
      break;
    }
  }
}

static uint8_t s_maxLines = 8;
static uint8_t s_copySrcIdx;
static uint8_t s_copySrcCh;

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#define EXPO_LINE_WEIGHT_POS 7*FW+1
#define EXPO_LINE_EXPO_POS   11*FW
#define EXPO_LINE_SWITCH_POS 11*FW+4
#if MAX_FLIGHT_MODES == 6
#define EXPO_LINE_SIDE_POS 15*FW
#else
#define EXPO_LINE_SIDE_POS 15*FW+2
#endif
#define EXPO_LINE_FM_POS     LCD_W-FW-MENUS_SCROLLBAR_WIDTH
#define EXPO_LINE_SELECT_POS 24
#define MIX_LINE_SRC_POS     4*FW-1
#define MIX_LINE_WEIGHT_POS  11*FW+3
#define MIX_LINE_CURVE_POS   12*FW+2
#define MIX_LINE_SWITCH_POS  16*FW
#define MIX_LINE_DELAY_POS   19*FW+7

void onExpoMixMenu(const char *result)
{
  uint8_t expo = (menuHandlers[menuLevel] == menuModelExposAll);
  uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

  if (result == STR_EDIT) {
    pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
  } else if (result == STR_INSERT_BEFORE || result == STR_INSERT_AFTER) {
    if (!reachExpoMixCountLimit(expo)) {
      s_currCh = chn;
      if (result == STR_INSERT_AFTER) {
        s_currIdx++;
        menuVerticalPosition++;
      }
      insertExpoMix(expo, s_currIdx);
      pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
    }
  } else if (result == STR_COPY || result == STR_MOVE) {
    s_copyMode = (result == STR_COPY ? COPY_MODE : MOVE_MODE);
    s_copySrcIdx = s_currIdx;
    s_copySrcCh = chn;
    s_copySrcRow = menuVerticalPosition;
  } else if (result == STR_DELETE) {
    deleteExpoMix(expo, s_currIdx);
  }
}

void displayMixInfos(coord_t y, MixData *md)
{
  if (md->curveParam) {
    if (md->curveMode == MODE_CURVE)
      lcdDrawCurveName(MIX_LINE_CURVE_POS, y, md->curveParam);
    else
      displayGVar(MIX_LINE_CURVE_POS+3*FW, y, md->curveParam, -100, 100);
  }

  if (md->swtch) {
    lcdPutsSwitches(MIX_LINE_SWITCH_POS, y, md->swtch);
  }
}

#define displayMixLine(y, md) displayMixInfos(y, md)

void displayExpoInfos(coord_t y, ExpoData *ed)
{
  if (ed->curveMode == MODE_CURVE)
    lcdDrawCurveName(EXPO_LINE_EXPO_POS-3*FW, y, ed->curveParam);
  else
    displayGVar(EXPO_LINE_EXPO_POS, y, ed->curveParam, -100, 100);

  lcdPutsSwitches(EXPO_LINE_SWITCH_POS, y, ed->swtch, 0);
}

#define displayExpoLine(y, ed) \
  displayExpoInfos(y, ed); \
  displayFlightModes(EXPO_LINE_FM_POS, y, ed->flightModes)

void menuModelExpoMix(uint8_t expo, uint8_t event)
{
  uint8_t sub = menuVerticalPosition;

  if (s_editMode > 0)
    s_editMode = 0;

  uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

  switch (event) {
  case EVT_ENTRY:
  case EVT_ENTRY_UP:
    s_copyMode = 0;
    s_copyTgtOfs = 0;
    break;
  case EVT_KEY_LONG(KEY_EXIT):
    if (s_copyMode && s_copyTgtOfs == 0) {
      deleteExpoMix(expo, s_currIdx);
      killEvents(event);
      event = 0;
    }
    // no break
  case EVT_ROTARY_LONG:
    if (s_copyMode) {
      killEvents(event);
    }
  case EVT_KEY_BREAK(KEY_EXIT):
    if (s_copyMode) {
      if (s_copyTgtOfs) {
        // cancel the current copy / move operation
        if (s_copyMode == COPY_MODE) {
          deleteExpoMix(expo, s_currIdx);
        } else {
          do {
            swapExpoMix(expo, s_currIdx, s_copyTgtOfs > 0);
            s_copyTgtOfs += (s_copyTgtOfs < 0 ? +1 : -1);
          } while (s_copyTgtOfs != 0);
          eeDirty(EE_MODEL);
        }
        menuVerticalPosition = s_copySrcRow;
        s_copyTgtOfs = 0;
      }
      s_copyMode = 0;
      event = 0;
    }
    break;
  case EVT_KEY_BREAK(KEY_ENTER):
    if (sub != 0 && (!s_currCh || (s_copyMode && !s_copyTgtOfs)) && !READ_ONLY()) {
      s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
      s_copySrcIdx = s_currIdx;
      s_copySrcCh = chn;
      s_copySrcRow = sub;
      break;
    }
    // no break

  case EVT_ROTARY_BREAK:
  case EVT_KEY_LONG(KEY_ENTER):
    killEvents(event);
    if (s_copyTgtOfs) {
      s_copyMode = 0;
      s_copyTgtOfs = 0;
    } else if (sub != 0) {
      if (READ_ONLY()) {
        if (!s_currCh) {
          pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
          return;
        }
      } else {
        if (s_copyMode) s_currCh = 0;
        if (s_currCh) {
          if (reachExpoMixCountLimit(expo)) break;
          insertExpoMix(expo, s_currIdx);
          pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
          s_copyMode = 0;
          return;
        } else {
          event = 0;
          s_copyMode = 0;
          POPUP_MENU_ADD_ITEM(STR_EDIT);
          POPUP_MENU_ADD_ITEM(STR_INSERT_BEFORE);
          POPUP_MENU_ADD_ITEM(STR_INSERT_AFTER);
          POPUP_MENU_ADD_ITEM(STR_COPY);
          POPUP_MENU_ADD_ITEM(STR_MOVE);
          POPUP_MENU_ADD_ITEM(STR_DELETE);
          popupMenuHandler = onExpoMixMenu;
        }
      }
    }
    break;
  case EVT_KEY_LONG(KEY_LEFT):
  case EVT_KEY_LONG(KEY_RIGHT):
    if (s_copyMode && !s_copyTgtOfs) {
      if (reachExpoMixCountLimit(expo)) break;
      s_currCh = chn;
      if (event == EVT_KEY_LONG(KEY_RIGHT)) {
        s_currIdx++;
        menuVerticalPosition++;
      }
      insertExpoMix(expo, s_currIdx);
      pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
      s_copyMode = 0;
      killEvents(event);
      return;
    }
    break;
  case EVT_ROTARY_LEFT:
  case EVT_ROTARY_RIGHT:
  case EVT_KEY_FIRST(KEY_MOVE_UP):
  case EVT_KEY_REPT(KEY_MOVE_UP):
  case EVT_KEY_FIRST(KEY_MOVE_DOWN):
  case EVT_KEY_REPT(KEY_MOVE_DOWN):
    if (s_copyMode) {
      uint8_t key = (event & 0x1f);
      uint8_t next_ofs = ((IS_ROTARY_UP(event) || key==KEY_MOVE_UP) ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

      if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
        // insert a mix on the same channel (just above / just below)
        if (reachExpoMixCountLimit(expo)) break;
        copyExpoMix(expo, s_currIdx);
        if (IS_ROTARY_DOWN(event) || key==KEY_MOVE_DOWN) s_currIdx++;
        else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
      } else if (next_ofs==0 && s_copyMode==COPY_MODE) {
        // delete the mix
        deleteExpoMix(expo, s_currIdx);
        if (IS_ROTARY_UP(event) || key==KEY_MOVE_UP) s_currIdx--;
      } else {
        // only swap the mix with its neighbor
        if (!swapExpoMix(expo, s_currIdx, IS_ROTARY_UP(event) || key==KEY_MOVE_UP)) break;
        eeDirty(EE_MODEL);
      }

      s_copyTgtOfs = next_ofs;
    }
    break;
  }

  lcdDrawNumberNAtt(FW*max(sizeof(TR_MENUINPUTS), sizeof(TR_MIXER))+FW+FW/2, 0, getExpoMixCount(expo));
  lcdDrawText(FW*max(sizeof(TR_MENUINPUTS), sizeof(TR_MIXER))+FW+FW/2, 0, expo ? STR_MAX(MAX_EXPOS) : STR_MAX(MAX_MIXERS));

  SIMPLE_MENU(expo ? STR_MENUINPUTS : STR_MIXER, menuTabModel, expo ? e_InputsAll : e_MixAll, s_maxLines);

  sub = menuVerticalPosition;
  s_currCh = 0;
  uint8_t cur = 1;
  uint8_t i = 0;

  for (uint8_t ch=1; ch<=(expo ? NUM_STICKS : NUM_CHNOUT); ch++) {
    void *pointer = NULL;
    MixData * &md = (MixData * &)pointer;
    ExpoData * &ed = (ExpoData * &)pointer;
    coord_t y = MENU_HEADER_HEIGHT-FH+1+(cur-menuVerticalOffset)*FH;
    if (expo ? (i<MAX_EXPOS && (ed=expoAddress(i))->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh+1 == ch)) {
      if (menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES) {
        if (expo) {
          putsMixerSource(0, y, MIXSRC_Rud+ch-1, 0);
        } else {
          putsChn(0, y, ch, 0); // show CHx
        }
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && menuVerticalOffset < cur && cur-menuVerticalOffset < 8 && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lcdDrawRect(expo ? 18 : 22, y-1, expo ? LCD_W-18 : LCD_W-22, 9, DOTTED);
            cur++;
            y+=FH;
          }
          if (s_currIdx == i) {
            sub = menuVerticalPosition = cur;
            s_currCh = ch;
          }
        } else if (sub == cur) {
          s_currIdx = i;
        }
        if (menuVerticalOffset < cur && cur-menuVerticalOffset < 8) {
          uint8_t attr = ((s_copyMode || sub != cur) ? 0 : INVERS);
          if (expo) {
            ed->weight = GVAR_MENU_ITEM(EXPO_LINE_WEIGHT_POS, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr | (isExpoActive(i) ? BOLD : 0), 0, event);
            displayExpoLine(y, ed);
            if (ed->mode!=3) {
              lcdDrawChar(EXPO_LINE_SIDE_POS, y, ed->mode == 2 ? 126 : 127);
            }
          } else {
            if (mixCnt > 0) lcdDrawTextAtIndex(FW, y, STR_VMLTPX2, md->mltpx, 0);

            putsMixerSource(MIX_LINE_SRC_POS, y, md->srcRaw, 0);

            gvarWeightItem(MIX_LINE_WEIGHT_POS, y, md, attr | (isMixActive(i) ? BOLD : 0), event);

            displayMixLine(y, md);

            char cs = ' ';
            if (md->speedDown || md->speedUp)
              cs = 'S';
            if (md->delayUp || md->delayDown)
              cs = (cs =='S' ? '*' : 'D');
            lcdDrawChar(MIX_LINE_DELAY_POS, y, cs);
          }
          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (LCD_W-EXPO_LINE_SELECT_POS) : (LCD_W-22), 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lcdDrawFilledRect(expo ? EXPO_LINE_SELECT_POS+1 : 23, y, expo ? (LCD_W-EXPO_LINE_SELECT_POS-2) : (LCD_W-24), 7);
            }
          }
        }
        cur++;
        y+=FH;
        mixCnt++;
        i++;
        if (expo) ed++;
        else md++;
      } while (expo ? (i<MAX_EXPOS && ed->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch));
      if (s_copyMode == MOVE_MODE && menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? LCD_W-EXPO_LINE_SELECT_POS : LCD_W-22, 9, DOTTED);
        cur++;
        y+=FH;
      }
    } else {
      uint8_t attr = 0;
      if (sub == cur) {
        s_currIdx = i;
        s_currCh = ch;
        if (!s_copyMode) {
          attr = INVERS;
        }
      }
      if (menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES) {
        if (expo) {
          putsMixerSource(0, y, MIXSRC_Rud+ch-1, attr);
        } else {
          putsChn(0, y, ch, attr); // show CHx
        }
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (LCD_W-EXPO_LINE_SELECT_POS) : (LCD_W-22), 9, DOTTED);
        }
      }
      cur++;
      y+=FH;
    }
  }
  s_maxLines = cur;
  if (sub >= s_maxLines-1) menuVerticalPosition = s_maxLines-1;
}

void menuModelExposAll(uint8_t event)
{
  return menuModelExpoMix(1, event);
}

void menuModelMixAll(uint8_t event)
{
  return menuModelExpoMix(0, event);
}
