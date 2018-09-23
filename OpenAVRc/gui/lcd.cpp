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
#include "../timers.h"

display_t displayBuf[DISPLAY_BUF_SIZE];

void lcdClear()
{
  memclear(displayBuf, DISPLAY_BUFER_SIZE);
}

coord_t lcdLastPos;
coord_t lcdNextPos;


void lcdDrawChar(coord_t x, coord_t y, const unsigned char c)
{
  lcdDrawCharAtt(x, y, c, 0);
}

void lcdDrawSizedTextAtt(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags flags)
{
  const coord_t orig_x = x;
  bool setx = false;
  while (len--) {
    unsigned char c;
    switch (flags & (BSS+ZCHAR)) {
    case BSS:
      c = *s;
      break;
    case ZCHAR:
      c = idx2char(*s);
      break;
    default:
      c = pgm_read_byte_near(s);
      break;
    }

    if (setx) {
      x = c;
      setx = false;
    } else if (!c) {
      break;
    } else if (c >= 0x20) {
      lcdDrawCharAtt(x, y, c, flags);
      x = lcdNextPos;
    } else if (c == 0x1F) { //X-coord prefix
      setx = true;
    } else if (c == 0x1E) { //NEWLINE
      x = orig_x;
      y += FH;
      if (y >= LCD_H) break;
    } else {
      x += (c*FW/2); // EXTENDED SPACE
    }
    s++;
  }
  lcdLastPos = x;
  lcdNextPos = x;
}

void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len)
{
  lcdDrawSizedTextAtt(x, y, s, len, 0);
}

void lcdDrawTextAtt(coord_t x, coord_t y, const pm_char * s, LcdFlags flags)
{
  lcdDrawSizedTextAtt(x, y, s, 255, flags);
}

void lcdDrawText(coord_t x, coord_t y, const pm_char * s)
{
  lcdDrawTextAtt(x, y, s, 0);
}

void lcdDrawTextLeft(coord_t y, const pm_char * s)
{
  lcdDrawText(0, y, s);
}

void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s,uint8_t idx, LcdFlags flags)
{
  uint8_t length;
  length = pgm_read_byte_near(s++);
  lcdDrawSizedTextAtt(x, y, s+length*idx, length, flags & ~(BSS|ZCHAR));
}

void lcd_outhex(uint8_t digit, coord_t x, coord_t y, uint16_t val, LcdFlags mode)
{
  x += FWNUM*4+1;
  for(uint8_t i=0; i<digit; ++i) {
    x -= FWNUM;
    char c = val & 0xf;
    c = c>9 ? c+'A'-10 : c+'0';
    lcdDrawCharAtt(x, y, c, (c>='A' ? CONDENSED : 0 ) | mode);
    val >>= 4;
  }
}

void lcd_outbin(uint8_t digit, coord_t x, coord_t y, uint16_t val, LcdFlags mode)
{
  x += FWNUM*(digit+1);
  for(uint8_t i=0; i<digit; ++i) {
    x -= FWNUM;
    uint8_t b = val & 0x01;
    lcdDrawNumberNAtt(x, y, b, mode);
    val >>= 1;
  }
}

void lcdDrawNumberNAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags flags, uint8_t len)
{
  uint8_t fw = FWNUM;
  int8_t mode = MODE(flags);
  flags &= ~LEADING0;
  bool dblsize = flags & DBLSIZE;
#define xxlsize 0
#define midsize 0
#define smlsize 0
#define tinsize 0

  bool neg = false;
  if (flags & UNSIGN) {
    flags -= UNSIGN;
  } else if (val < 0) {
    neg = true;
    val = -val;
  }

  coord_t xn = 0;
  uint8_t ln = 2;

  if (mode != MODE(LEADING0)) {
    len = 1;
    lcduint_t tmp = ((lcduint_t)val) / 10;
    while (tmp) {
      len++;
      tmp /= 10;
    }
    if (len <= mode) {
      len = mode + 1;
    }
  }

  if (dblsize) {
    fw += FWNUM;
  } else if (xxlsize) {
    fw += 4*FWNUM-1;
  } else if (midsize) {
    fw += FWNUM-3;
  } else if (tinsize) {
    fw -= 1;
  } else {
    if (flags & LEFT) {
      if (mode > 0)
        x += 2;
    }
#if defined(BOLD_SPECIFIC_FONT)
    if (flags & BOLD) fw += 1;
#endif
  }

  if (flags & LEFT) {
    x += len * fw;
    if (neg) {
      x += ((xxlsize|dblsize|midsize) ? 7 : FWNUM);
    }
  }

  lcdLastPos = x;
  x -= fw;
  if (dblsize) x++;

  for (uint8_t i=1; i<=len; i++) {
    div_t qr = div((lcduint_t)val, 10);
    char c = qr.rem + '0';
    LcdFlags f = flags;
    if (dblsize) {
      if (c=='1' && i==len && xn>x+10) {
        x+=1;
      }
      if ((lcduint_t)val >= 1000) {
        x+=FWNUM;
        f &= ~DBLSIZE;
      }
    }
    lcdDrawCharAtt(x, y, c, f);
    if (mode == i) {
      flags &= ~PREC2; // TODO not needed but removes 20bytes, could be improved for sure, check asm
      if (dblsize) {
        xn = x - 2;
        if (c>='2' && c<='3') ln++;
        uint8_t tn = (qr.quot % 10);
        if (tn==2 || tn==4) {
          if (c=='4') {
            xn++;
          } else {
            xn--;
            ln++;
          }
        }
      } else if (xxlsize) {
        x -= 17;
        lcdDrawCharAtt(x+2, y, '.', f);
      } else if (midsize) {
        x -= 3;
        xn = x;
      } else if (smlsize) {
        x -= 2;
        lcdDrawPoint(x, y+5);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
          lcdDrawSolidVerticalLine(x, y-1, 8);
        }
      } else if (tinsize) {
        x--;
        lcdDrawPoint(x-1, y+4);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
          lcdDrawSolidVerticalLine(x-1, y-1, 7);
        }
        x--;
      } else {
        x -= 2;
        lcdDrawCharAtt(x, y, '.', f);
      }
    }
    if (dblsize && (lcduint_t)val >= 1000 && (lcduint_t)val < 10000) x-=2;
    val = qr.quot;
    x -= fw;
#if defined(BOLD_SPECIFIC_FONT)
    if (i==len && (flags & BOLD)) x += 1;
#endif
  }

  if (xn) {
    if (midsize) {
      if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
        lcdDrawSolidVerticalLine(xn, y, 12);
        lcdDrawSolidVerticalLine(xn+1, y, 12);
      }
      lcdDrawSolidHorizontalLine(xn, y+9, 2);
      lcdDrawSolidHorizontalLine(xn, y+10, 2);
    } else {
      // TODO needed on CPUAVR? y &= ~0x07;
      lcdDrawFilledRect(xn, y+2*FH-3, ln, 2);
    }
  }
  if (neg) lcdDrawCharAtt(x, y, '-', flags);
}

void lcdDrawSolidHorizontalLine(coord_t x, coord_t y, coord_t w, LcdFlags att)
{
  lcdDrawSolidHorizontalLineStip(x, y, w, 0xff, att);
}


void lcdDrawSolidVerticalLine(coord_t x, scoord_t y, scoord_t h)
{
  lcdDrawSolidVerticalLineStip(x, y, h, SOLID);
}

void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  lcdDrawSolidVerticalLineStip(x, y, h, pat);
  lcdDrawSolidVerticalLineStip(x+w-1, y, h, pat);
  if (~att & ROUND) {
    x+=1;
    w-=2;
  }
  lcdDrawSolidHorizontalLineStip(x, y+h-1, w, pat);
  lcdDrawSolidHorizontalLineStip(x, y, w, pat);
}

void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  for (scoord_t i=y; i<y+h; i++) {
    if ((att&ROUND) && (i==y || i==y+h-1))
      lcdDrawSolidHorizontalLineStip(x+1, i, w-2, pat, att);
    else
      lcdDrawSolidHorizontalLineStip(x, i, w, pat, att);
    pat = (pat >> 1) + ((pat & 1) << 7);
  }
}

void lcdDrawTelemetryTopBar()
{
  putsModelName(0, 0, g_model.name, g_eeGeneral.currModel, 0);
  uint8_t att = (IS_TXBATT_WARNING() ? BLINK : 0);
  putsVBat(14*FW,0,att);
  if (g_model.timers[0].mode) {
    att = (timersStates[0].val<0 ? BLINK : 0);
    putsTimer(17*FW+5*FWNUM+1, 0, timersStates[0].val, att, att);
  }
  lcdInvertLine(0);
}


void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att, LcdFlags att2)
{
  div_t qr;

  if (!(att & LEFT)) {
    if (att & DBLSIZE)
      x -= 5*(2*FWNUM)-4;
    else
      x -= 5*FWNUM+1;
  }

  if (tme < 0) {
    lcdDrawCharAtt(x - ((att & DBLSIZE) ? FW+2 : ((att & DBLSIZE) ? FW+0 : FWNUM)), y, '-', att);
    tme = -tme;
  }

  qr = div(tme, 60);

#define separator ':'
  lcdDrawNumberNAtt(x, y, qr.quot, att|LEADING0|LEFT, 2);
  lcdDrawCharAtt(lcdLastPos, y, separator, att&att2);
  lcdDrawNumberNAtt(lcdNextPos, y, qr.rem, att2|LEADING0|LEFT, 2);
}

void lcdPutsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att)
{
  lcdDrawNumberNAtt(x, y, (int16_t)volts, (~NO_UNIT) & (att | ((att&PREC2)==PREC2 ? PREC2 : PREC1)));
  if (~att & NO_UNIT) lcdDrawCharAtt(lcdLastPos, y, 'V', att);
}

void putsVBat(coord_t x, coord_t y, LcdFlags att)
{
  lcdPutsVolts(x, y, (g_vbat10mV < 1000)? g_vbat10mV : g_vbat10mV/10, (g_vbat10mV < 1000)? att | PREC2 : att | PREC1);
}

void lcdDrawStringWithIndex(coord_t x, coord_t y, const pm_char *str, uint8_t idx, LcdFlags att)
{
  lcdDrawTextAtt(x, y, str, att & ~LEADING0);
  lcdDrawNumberNAtt(lcdNextPos, y, idx, att|LEFT, 2);
}

void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  if (idx < MIXSRC_THR)
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx, att);
  else if (idx < MIXSRC_FIRST_LOGICAL_SWITCH)
    lcdPutsSwitches(x, y, idx-MIXSRC_THR+1+3*(1), att);
  else if (idx <= MIXSRC_LAST_LOGICAL_SWITCH)
    lcdPutsSwitches(x, y, SWSRC_SW1+idx-MIXSRC_FIRST_LOGICAL_SWITCH, att);
  else if (idx < MIXSRC_CH1)
    lcdDrawStringWithIndex(x, y, STR_PPM_TRAINER, idx-MIXSRC_FIRST_TRAINER+1, att);
  else if (idx <= MIXSRC_LAST_CH) {
    lcdDrawStringWithIndex(x, y, STR_CH, idx-MIXSRC_CH1+1, att);
  }
  else if (idx <= MIXSRC_LAST_GVAR)
    lcdDrawStringWithIndex(x, y, STR_GV, idx-MIXSRC_GVAR1+1, att);
  else if (idx < MIXSRC_FIRST_TELEM) {
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1-(MIXSRC_SW1-MIXSRC_THR)-NUM_LOGICAL_SWITCH-NUM_TRAINER-NUM_CHNOUT-MAX_GVARS, att);
  } else
    lcdDrawTextAtIndex(x, y, STR_VTELEMCHNS, idx-MIXSRC_FIRST_TELEM+1, att);
}

void lcdPutsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  lcdDrawTextAtIndex(x, y, STR_RETA123, idx-1, att);
}

void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att)
{
  uint8_t len = sizeof(g_model.name);
  while (len>0 && !name[len-1]) --len;
  if (len==0) {
    lcdDrawStringWithIndex(x, y, STR_MODEL, id+1, att|LEADING0);
  } else {
    lcdDrawSizedTextAtt(x, y, name, sizeof(g_model.name), ZCHAR|att);
  }
}

void lcdPutsSwitches(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx == SWSRC_OFF)
    return lcdDrawTextAtIndex(x, y, STR_OFFON, 0, att);
  if (idx < 0) {
    lcdDrawCharAtt(x-2, y, '!', att);
    idx = -idx;
  }
  return lcdDrawTextAtIndex(x, y, STR_VSWITCHES, idx, att);
}

#if defined(FLIGHT_MODES)
void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) {
    lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, att);
    return;
  }
  if (idx < 0) {
    lcdDrawCharAtt(x-2, y, '!', att);
    idx = -idx;
  }
  if (att & CONDENSED)
    lcdDrawNumberNAtt(x+FW*1, y, idx-1, (att & ~CONDENSED), 1);
  else
    lcdDrawStringWithIndex(x, y, STR_FP, idx-1, att);
}
#endif

void lcdDrawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx < 0) {
    lcdDrawCharAtt(x-3, y, '!', att);
    idx = -idx+CURVE_BASE-1;
  }
  if (idx < CURVE_BASE)
    lcdDrawTextAtIndex(x, y, STR_VCURVEFUNC, idx, att);
  else
    lcdDrawStringWithIndex(x, y, STR_CV, idx-CURVE_BASE+1, att);
}

void lcdPutsTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att)
{
  if (mode >= 0) {
    if (mode < TMRMODE_COUNT)
      return lcdDrawTextAtIndex(x, y, STR_VTMRMODES, mode, att);
    else
      mode -= (TMRMODE_COUNT-1);
  }
  lcdPutsSwitches(x, y, mode, att);
}

void lcdPutsTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  trim_t v = getRawTrimValue(phase, idx);

  if (v > TRIM_EXTENDED_MAX) {
    uint8_t p = v - TRIM_EXTENDED_MAX - 1;
    if (p >= phase) p++;
    lcdDrawCharAtt(x, y, '0'+p, att);
  } else {
    lcdPutsChnLetter(x, y, idx+1, att);
  }
}

#if ROTARY_ENCODERS > 0
void lcdPutsRotaryEncoderMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  int16_t v = flightModeAddress(phase)->rotaryEncoders[idx];

  if (v > ROTARY_ENCODER_MAX) {
    uint8_t p = v - ROTARY_ENCODER_MAX - 1;
    if (p >= phase) p++;
    lcdDrawCharAtt(x, y, '0'+p, att);
  } else {
    lcdDrawCharAtt(x, y, 'a'+idx, att);
  }
}
#endif

#if   defined(FRSKY)
void lcdPutsValueWithUnit(coord_t x, coord_t y, lcdint_t val, uint8_t unit, LcdFlags att)
{
  convertUnit(val, unit);
  lcdDrawNumberNAtt(x, y, val, att & (~NO_UNIT));
  if (!(att & NO_UNIT) && unit != UNIT_RAW) {
    lcdDrawTextAtIndex(lcdLastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
  }
}

const pm_uint8_t bchunit_ar[] PROGMEM = {
  UNIT_DIST,    // Alt
  UNIT_RAW,     // Rpm
  UNIT_PERCENT, // Fuel
  UNIT_TEMPERATURE, // T1
  UNIT_TEMPERATURE, // T2
  UNIT_KTS,     // Speed
  UNIT_DIST,    // Dist
  UNIT_DIST,    // GPS Alt
};

void lcdPutsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, lcdint_t val, LcdFlags att)
{
  switch (channel) {
  case TELEM_TIMER1-1:
  case TELEM_TIMER2-1:
    att &= ~NO_UNIT;
    putsTimer(x, y, val, att, att);
    break;
#if defined(FRSKY)
  case TELEM_MIN_A1-1:
  case TELEM_MIN_A2-1:
    channel -= TELEM_MIN_A1-TELEM_A1;
  // no break
  case TELEM_A1-1:
  case TELEM_A2-1:
    channel -= TELEM_A1-1;
    // A1 and A2
    {
      lcdint_t converted_value = applyChannelRatio(channel, val);
      if (ANA_CHANNEL_UNIT(channel) >= UNIT_RAW) {
        converted_value = div10_and_round(converted_value);
      } else {
        if (abs(converted_value) < 1000) {
          att |= PREC2;
        } else {
          converted_value = div10_and_round(converted_value);
          att |= PREC1;
        }
      }
      lcdPutsValueWithUnit(x, y, converted_value, g_model.telemetry.channels[channel].type, att);
      break;
    }
#endif

  case TELEM_CELL-1:
  case TELEM_MIN_CELL-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_VOLTS, att|PREC2);
    break;

  case TELEM_TX_VOLTAGE-1:
  case TELEM_VFAS-1:
  case TELEM_CELLS_SUM-1:
  case TELEM_MIN_CELLS_SUM-1:
  case TELEM_MIN_VFAS-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_VOLTS, att|PREC1);
    break;

  case TELEM_CURRENT-1:
  case TELEM_MAX_CURRENT-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_AMPS, att|PREC1);
    break;

  case TELEM_CONSUMPTION-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_MAH, att);
    break;

  case TELEM_POWER-1:
  case TELEM_MAX_POWER-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_WATTS, att);
    break;

  case TELEM_ACCx-1:
  case TELEM_ACCy-1:
  case TELEM_ACCz-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_RAW, att|PREC2);
    break;

  case TELEM_VSPEED-1:
    lcdPutsValueWithUnit(x, y, div10_and_round(val), UNIT_RAW, att|PREC1);
    break;

  case TELEM_ASPEED-1:
  case TELEM_MAX_ASPEED-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_KTS, att|PREC1);
    break;

  case TELEM_RSSI_TX-1:
  case TELEM_RSSI_RX-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_RAW, att);
    break;

  case TELEM_HDG-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_HDG, att);
    break;

#if defined(WS_HOW_HIGH)
  case TELEM_ALT-1:
  case TELEM_MIN_ALT-1:
  case TELEM_MAX_ALT-1:
    if (IS_IMPERIAL_ENABLE() && IS_USR_PROTO_WS_HOW_HIGH()) {
      lcdPutsValueWithUnit(x, y, val, UNIT_FEET, att);
      break;
    }
#endif


  default: {
    uint8_t unit = 1;
    if (channel >= TELEM_MAX_T1-1 && channel <= TELEM_MAX_DIST-1)
      channel -= TELEM_MAX_T1 - TELEM_T1;
    if (channel <= TELEM_GPSALT-1)
      unit = channel + 1 - TELEM_ALT;
    if (channel >= TELEM_MIN_ALT-1 && channel <= TELEM_MAX_ALT-1)
      unit = 0;
    lcdPutsValueWithUnit(x, y, val, pgm_read_byte_near(bchunit_ar+unit), att);
    break;
  }
  }
}
#else // defined(FRSKY)
void lcdPutsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, lcdint_t val, uint8_t att)
{
  switch (channel) {
  case TELEM_TIMER1-1:
  case TELEM_TIMER2-1:
    att &= ~NO_UNIT;
    putsTimer(x, y, val, att, att);
    break;

  case TELEM_TX_VOLTAGE-1:
    lcdDrawNumberNAtt(x, y, val, (att|PREC1) & (~NO_UNIT));
    if (!(att & NO_UNIT))
      lcdDrawChar(lcdLastPos/*+1*/, y, 'V');
    break;
  }
}
#endif

void lcdSetContrast()
{
  lcdSetRefVolt(g_eeGeneral.contrast);
}

#define LCD_BYTE_FILTER(p, keep, add) *(p) = (*(p) & (keep)) | (add)

#if defined(SDCARD)

void lcdDrawCharAtt(coord_t x, uint8_t y, const unsigned char c, LcdFlags flags)
{
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];

  uint_farptr_t q=pgm_get_far_address(zzfont_5x7);
  q += (c-0x20)*5;
  lcdNextPos = x-1;
  p--;

  bool inv = false;
  if (flags & BLINK) {
    if (BLINK_ON_PHASE) {
      if (flags & INVERS)
        inv = true;
      else {
        return;
      }
    }
  } else if (flags & INVERS) {
    inv = true;
  }

  unsigned char c_remapped = 0;

#if defined(BOLD_SPECIFIC_FONT)
  if (flags & (DBLSIZE+BOLD)) {
#else
  if (flags & DBLSIZE) {
#endif
    // To save space only some DBLSIZE and BOLD chars are available
    // c has to be remapped. All non existing chars mapped to 0 (space)

    if (c>=',' && c<=':')
      c_remapped = c - ',' + 1;
    else if (c>='A' && c<='Z')
      c_remapped = c - 'A' + 16;
    else if (c>='a' && c<='z')
      c_remapped = c - 'a' + 42;
    else if (c=='_')
      c_remapped = 4;
#if defined(BOLD_SPECIFIC_FONT)
    else if (c!=' ')
      flags &= ~BOLD;
#endif

#if defined(BOLD_SPECIFIC_FONT)
  }
  if (flags & DBLSIZE) {
#endif

    /* each letter consists of ten top bytes followed by
     * by ten bottom bytes (20 bytes per * char) */

    q = pgm_get_far_address(zzfont_10x14);
    q += (uint16_t)c_remapped*20;

    for (int8_t i=0; i<=11; i++) {
      uint8_t b1=0, b2=0;
      if (!i) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      } else if (i <= 10) {

        b1 = pgm_read_byte_far(q++); /*top byte*/
        b2 = pgm_read_byte_far(q++);

      }
      if ((b1 & b2) == 0xff) continue;
      if (inv) {
        b1 = ~b1;
        b2 = ~b2;
      }
      if(p+LCD_W < DISPLAY_END) {
        ASSERT_IN_DISPLAY(p);
        ASSERT_IN_DISPLAY(p+LCD_W);
        LCD_BYTE_FILTER(p, 0, b1);
        LCD_BYTE_FILTER(p+LCD_W, 0, b2);
        p++;
        lcdNextPos++;
      }
    }
  } else {
    const uint8_t ym8 = (y & 0x07);
#if defined(BOLD_FONT)
#if defined(BOLD_SPECIFIC_FONT)
    if (flags & BOLD) {
      q = pgm_get_far_address(zzfont_5x7_B);
      q += (c_remapped)*5;
    }
#else
    uint8_t bb = 0;
    if (inv) bb = 0xff;
#endif
#endif

    uint8_t *lineEnd = &displayBuf[ y / 8 * LCD_W + LCD_W ];

    for (int8_t i=0; i<=6; i++) {
      uint8_t b = 0;
      if (i==0) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      } else if (i <= 5) {
        b = pgm_read_byte_far(q++);
      }
      if (b == 0xff) {
        if (flags & FIXEDWIDTH)
          b = 0;
        else
          continue;
      }
      if (inv) b = ~b;
      if ((flags & CONDENSED) && i==2) {
        /*condense the letter by skipping column 3 */
        continue;
      }

#if defined(BOLD_FONT) && !defined(BOLD_SPECIFIC_FONT)
      if (flags & BOLD) {
        uint8_t a;
        if (inv)
          a = b & bb;
        else
          a = b | bb;
        bb = b;
        b = a;
      }
#endif

      if (p<DISPLAY_END && p<lineEnd) {
        ASSERT_IN_DISPLAY(p);
        uint8_t mask = ~(0xff << ym8);
        LCD_BYTE_FILTER(p, mask, b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~mask, b >> (8-ym8));
        }

        if (inv && (ym8 == 1)) *p |= 0x01;
      }
      p++;
      lcdNextPos++;
    }
  }
}

#else // NOSDCARD lcdDrawCharAtt 20% faster

void lcdDrawCharAtt(coord_t x, uint8_t y, const unsigned char c, LcdFlags flags)
{
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];

  const pm_uchar *q = &zzfont_5x7[(c-0x20)*5];

  lcdNextPos = x-1;
  p--;

  bool inv = false;
  if (flags & BLINK) {
    if (BLINK_ON_PHASE) {
      if (flags & INVERS)
        inv = true;
      else {
        return;
      }
    }
  } else if (flags & INVERS) {
    inv = true;
  }

  unsigned char c_remapped = 0;

#if defined(BOLD_SPECIFIC_FONT)
  if (flags & (DBLSIZE+BOLD)) {
#else
  if (flags & DBLSIZE) {
#endif
    // To save space only some DBLSIZE and BOLD chars are available
    // c has to be remapped. All non existing chars mapped to 0 (space)

    if (c>=',' && c<=':')
      c_remapped = c - ',' + 1;
    else if (c>='A' && c<='Z')
      c_remapped = c - 'A' + 16;
    else if (c>='a' && c<='z')
      c_remapped = c - 'a' + 42;
    else if (c=='_')
      c_remapped = 4;
#if defined(BOLD_SPECIFIC_FONT)
    else if (c!=' ')
      flags &= ~BOLD;
#endif

#if defined(BOLD_SPECIFIC_FONT)
  }
  if (flags & DBLSIZE) {
#endif

    /* each letter consists of ten top bytes followed by
     * by ten bottom bytes (20 bytes per * char) */

    q = &zzfont_10x14[((uint16_t)c_remapped)*20];

    for (int8_t i=0; i<=11; i++) {
      uint8_t b1=0, b2=0;
      if (!i) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      } else if (i <= 10) {

        b1 = *q++; /*top byte*/
        b2 = *q++;

      }
      if ((b1 & b2) == 0xff) continue;
      if (inv) {
        b1 = ~b1;
        b2 = ~b2;
      }
      if(p+LCD_W < DISPLAY_END) {
        ASSERT_IN_DISPLAY(p);
        ASSERT_IN_DISPLAY(p+LCD_W);
        LCD_BYTE_FILTER(p, 0, b1);
        LCD_BYTE_FILTER(p+LCD_W, 0, b2);
        p++;
        lcdNextPos++;
      }
    }
  } else {
    const uint8_t ym8 = (y & 0x07);
#if defined(BOLD_FONT)
#if defined(BOLD_SPECIFIC_FONT)
    if (flags & BOLD) {
      q = &zzfont_5x7_B[(c_remapped)*5];
    }
#else
    uint8_t bb = 0;
    if (inv) bb = 0xff;
#endif
#endif

    uint8_t *lineEnd = &displayBuf[ y / 8 * LCD_W + LCD_W ];

    for (int8_t i=0; i<=6; i++) {
      uint8_t b = 0;
      if (i==0) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      } else if (i <= 5) {
        b = *q++;
      }
      if (b == 0xff) {
        if (flags & FIXEDWIDTH)
          b = 0;
        else
          continue;
      }
      if (inv) b = ~b;
      if ((flags & CONDENSED) && i==2) {
        /*condense the letter by skipping column 3 */
        continue;
      }

#if defined(BOLD_FONT) && !defined(BOLD_SPECIFIC_FONT)
      if (flags & BOLD) {
        uint8_t a;
        if (inv)
          a = b & bb;
        else
          a = b | bb;
        bb = b;
        b = a;
      }
#endif

      if (p<DISPLAY_END && p<lineEnd) {
        ASSERT_IN_DISPLAY(p);
        uint8_t mask = ~(0xff << ym8);
        LCD_BYTE_FILTER(p, mask, b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~mask, b >> (8-ym8));
        }

        if (inv && (ym8 == 1)) *p |= 0x01;
      }
      p++;
      lcdNextPos++;
    }
  }
}

#endif

void lcdMaskPoint(uint8_t *p, uint8_t mask, LcdFlags att)
{
  ASSERT_IN_DISPLAY(p);

  if (att & FORCE)
    *p |= mask;
  else if (att & ERASE)
    *p &= ~mask;
  else
    *p ^= mask;
}

void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att)
{
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];
  if (p<DISPLAY_END)
    lcdMaskPoint(p, _BV(y%8), att);
}

void lcdDrawSolidHorizontalLineStip(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att)
{
  if (y >= LCD_H) return;
  if (x+w > LCD_W) {
    w = LCD_W - x;
  }

  uint8_t *p  = &displayBuf[ y / 8 * LCD_W + x ];
  uint8_t msk = _BV(y%8);
  while (w--) {
    if(pat&1) {
      lcdMaskPoint(p, msk, att);
      pat = (pat >> 1) | 0x80;
    } else {
      pat = pat >> 1;
    }
    p++;
  }
}

// allows the att parameter...
void lcdDrawSolidVerticalLineStip(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att)
{
  if (x >= LCD_W) return;

  if (h<0) {
    y+=h;
    h=-h;
  }
  if (y<0) {
    h+=y;
    y=0;
  }
  if (y+h > LCD_H) {
    h = LCD_H - y;
  }

  if (pat==DOTTED && !(y%2))
    pat = ~pat;

  uint8_t *p  = &displayBuf[ y / 8 * LCD_W + x ];
  y = (y & 0x07);
  if (y) {
    ASSERT_IN_DISPLAY(p);
    uint8_t msk = ~(_BV(y)-1);
    h -= 8-y;
    if (h < 0)
      msk -= ~(_BV(8+h)-1);
    lcdMaskPoint(p, msk & pat, att);
    p += LCD_W;
  }
  while (h>=8) {
    ASSERT_IN_DISPLAY(p);
    lcdMaskPoint(p, pat, att);
    p += LCD_W;
    h -= 8;
  }
  if (h>0) {
    ASSERT_IN_DISPLAY(p);
    lcdMaskPoint(p, (_BV(h)-1) & pat, att);
  }
}

void lcdInvertLine(int8_t y)
{
  uint8_t *p  = &displayBuf[y * LCD_W];
  for (coord_t x=0; x<LCD_W; x++) {
    ASSERT_IN_DISPLAY(p);
    *p++ ^= 0xff;
  }
}

void lcd_imgfar(coord_t x, coord_t y,  uint_farptr_t img, uint8_t idx, LcdFlags att) // progmem "far"
{
  uint_farptr_t q = img;
  uint8_t w    = pgm_read_byte_far(q++);
  uint8_t hb   = (pgm_read_byte_far(q++)+7)/8;
  bool    inv  = (att & INVERS) ? true : (att & BLINK ? BLINK_ON_PHASE : false);
  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (coord_t i=0; i<w; i++) {
      uint8_t b = pgm_read_byte_far(q);
      q++;
      ASSERT_IN_DISPLAY(p);
      *p++ = inv ? ~b : b;
    }
  }
}

void lcd_img(coord_t x, coord_t y, const pm_uchar * img, uint8_t idx, LcdFlags att)
{
  const pm_uchar *q = img;
  uint8_t w    = pgm_read_byte_near(q++);
  uint8_t hb   = (pgm_read_byte_near(q++)+7)/8;
  bool    inv  = (att & INVERS) ? true : (att & BLINK ? BLINK_ON_PHASE : false);
  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (coord_t i=0; i<w; i++) {
      uint8_t b = pgm_read_byte_near(q);
      q++;
      ASSERT_IN_DISPLAY(p);
      *p++ = inv ? ~b : b;
    }
  }
}


