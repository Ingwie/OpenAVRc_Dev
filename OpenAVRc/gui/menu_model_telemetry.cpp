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

enum menuModelTelemetryItems {
#if defined(FRSKY)
  ITEM_TELEMETRY_USR_PROTO,
#endif
  ITEM_TELEMETRY_A1_LABEL,
  ITEM_TELEMETRY_A1_RANGE,
  ITEM_TELEMETRY_A1_OFFSET,
  /*ITEM_TELEMETRY_A1_ALARM1,
  ITEM_TELEMETRY_A1_ALARM2,*/
  ITEM_TELEMETRY_A2_LABEL,
  ITEM_TELEMETRY_A2_RANGE,
  ITEM_TELEMETRY_A2_OFFSET,
  /*ITEM_TELEMETRY_A2_ALARM1,
  ITEM_TELEMETRY_A2_ALARM2,*/
  ITEM_TELEMETRY_RSSI_LABEL,
  ITEM_TELEMETRY_RSSI_ALARM1,
  /*ITEM_TELEMETRY_RSSI_ALARM2,*/
#if defined(FRSKY)
  ITEM_TELEMETRY_USR_LABEL,
  ITEM_TELEMETRY_USR_BLADES,
#endif
  ITEM_TELEMETRY_USR_VOLTAGE_SOURCE,
  ITEM_TELEMETRY_USR_CURRENT_SOURCE,
  ITEM_TELEMETRY_FAS_OFFSET,
  CASE_VARIO(ITEM_TELEMETRY_VARIO_LABEL)
#if defined(VARIO)
  ITEM_TELEMETRY_VARIO_SOURCE,
#endif
  CASE_VARIO(ITEM_TELEMETRY_VARIO_RANGE)
  ITEM_TELEMETRY_SCREEN_LABEL1,
  ITEM_TELEMETRY_SCREEN_LINE1,
  ITEM_TELEMETRY_SCREEN_LINE2,
  ITEM_TELEMETRY_SCREEN_LINE3,
  ITEM_TELEMETRY_SCREEN_LINE4,
  ITEM_TELEMETRY_SCREEN_LABEL2,
  ITEM_TELEMETRY_SCREEN_LINE5,
  ITEM_TELEMETRY_SCREEN_LINE6,
  ITEM_TELEMETRY_SCREEN_LINE7,
  ITEM_TELEMETRY_SCREEN_LINE8,
  ITEM_TELEMETRY_MAX
};

#define TELEM_COL1                   INDENT_WIDTH
#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_CZ)
#define TELEM_COL2                   (9*FW)
#else
#define TELEM_COL2                   (8*FW)
#endif
#define TELEM_BARS_COLMIN            (56-3*FW)
#define TELEM_BARS_COLMAX            (14*FW-3)
#define TELEM_SCRTYPE_COL            TELEM_COL2

#define IS_RANGE_DEFINED(k)          (g_model.telemetry.channels[k].ratio > 0)

#define CHANNEL_ROWS(x)              LABEL(CHANNEL), 1, 0
#define CHANNELS_ROWS                CHANNEL_ROWS(0), CHANNEL_ROWS(1),
#define RSSI_ROWS                    0, 1,
#define SENSORS_ROWS

#define IF_FAS_OFFSET(x)             x,

#if defined(FRSKY)
#define USRPROTO                     1,
#define USRDATA_ROWS                 LABEL(UsrData), 0, 0, 0, IF_FAS_OFFSET(0)
#else
#define USRPROTO
#define USRDATA_ROWS                 0, 0, IF_FAS_OFFSET(0)
#endif

#define SCREEN_TYPE_ROWS             0

#define VARIO_RANGE_ROWS             3

#define TELEMETRY_SCREEN_ROWS(x)     SCREEN_TYPE_ROWS, 2, 2, 2, 2
#define TELEMETRY_CURRENT_CHANNEL(k) (k >= ITEM_TELEMETRY_A2_LABEL ? TELEM_ANA_A2 : TELEM_ANA_A1)


void menuModelTelemetry(uint8_t event)
{

  MENU(STR_MENUTELEMETRY, menuTabModel, e_Telemetry, ITEM_TELEMETRY_MAX+1, {USRPROTO 0, CHANNELS_ROWS RSSI_ROWS SENSORS_ROWS USRDATA_ROWS CASE_VARIO(LABEL(Vario)) CASE_VARIO(0) CASE_VARIO(VARIO_RANGE_ROWS) TELEMETRY_SCREEN_ROWS(0), TELEMETRY_SCREEN_ROWS(1), });

  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;

    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    uint8_t ch = TELEMETRY_CURRENT_CHANNEL(k);
    TelemetryChannelData & channel = g_model.telemetry.channels[ch];
    uint8_t dest = TELEM_A1-1+ch;


    switch (k) {


    case ITEM_TELEMETRY_A1_LABEL:
    case ITEM_TELEMETRY_A2_LABEL:
      lcdDrawTextLeft(y, STR_ACHANNEL);
      lcdDrawNumberNAtt(2*FW, y, ch+1, 0);
      lcdPutsTelemetryChannelValue(TELEM_COL2+6*FW, y, dest, telemetryData.analog[ch].value, LEFT);
      break;

    case ITEM_TELEMETRY_A1_RANGE:
    case ITEM_TELEMETRY_A2_RANGE:
      lcdDrawTextLeft(y, STR_RANGE);
      lcdPutsTelemetryChannelValue(TELEM_COL2, y, dest, 255-channel.offset, (menuHorizontalPosition<=0 ? attr : 0)|NO_UNIT|LEFT);
      lcdDrawTextAtIndex(lcdLastPos, y, STR_VTELEMUNIT, channel.type, menuHorizontalPosition!=0 ? attr : 0);
      if (attr && (s_editMode>0 || p1valdiff)) {
        if (menuHorizontalPosition == 0) {
          uint16_t ratio = checkIncDec(event, channel.ratio, 0, 256, EE_MODEL);
          if (checkIncDec_Ret) {
            if (ratio == 127 && channel.multiplier > 0) {
              channel.multiplier--;
              channel.ratio = 255;
            } else if (ratio == 256) {
              if (channel.multiplier < FRSKY_MULTIPLIER_MAX) {
                channel.multiplier++;
                channel.ratio = 128;
              }
            } else {
              channel.ratio = ratio;
            }
          }
        } else {
          CHECK_INCDEC_MODELVAR_ZERO(event, channel.type, UNIT_A1A2_MAX);
        }
      }
      break;

    case ITEM_TELEMETRY_A1_OFFSET:
    case ITEM_TELEMETRY_A2_OFFSET:
      lcdDrawTextLeft(y, STR_OFFSET);
      lcdPutsTelemetryChannelValue(TELEM_COL2, y, dest, 0, LEFT|attr);
      if (attr) channel.offset = checkIncDec(event, channel.offset, -256, 256, EE_MODEL);
      break;

    case ITEM_TELEMETRY_RSSI_LABEL:
      lcdDrawTextLeft(y, PSTR("RSSI"));
      break;

    case ITEM_TELEMETRY_RSSI_ALARM1:
      lcdDrawTextLeft(y, STR_ALARM);
      lcdDrawChar(TELEM_COL2-2*FW, y, '<');
      lcdDrawNumberNAtt(TELEM_COL2+2*FW, y, getRssiAlarmValue(), attr, 3);
      if (attr) {CHECK_INCDEC_MODELVAR(event, g_model.telemetry.rssiAlarm, -30, 30);}
      break;

#if defined(FRSKY)
    case ITEM_TELEMETRY_USR_LABEL:
      lcdDrawTextLeft(y, STR_USRDATA);
      break;

    case ITEM_TELEMETRY_USR_PROTO:
      lcdDrawTextLeft(y, STR_PROTO);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_VTELPROTO, g_model.telemetry.usrProto, attr);
      if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.usrProto, USR_PROTO_LAST);
          if ((checkIncDec_Ret) && (!IS_MULTIMODULE_PROTOCOL(g_model.rfProtocol))) { // No reset in Multi mode
              telemetryResetValue();
          }
      }
      break;

    case ITEM_TELEMETRY_USR_BLADES:
      lcdDrawTextLeft(y, STR_BLADES);
      lcdDrawNumberNAtt(TELEM_COL2+FWNUM, y, 2+g_model.telemetry.blades, attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.blades, MAX_BLADES);
      break;
#endif

    case ITEM_TELEMETRY_USR_VOLTAGE_SOURCE:
      lcdDrawTextLeft(y, STR_VOLTAGE);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.telemetry.voltsSource+1, attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.voltsSource, FRSKY_VOLTS_SOURCE_LAST);
      break;

    case ITEM_TELEMETRY_USR_CURRENT_SOURCE:
      lcdDrawTextLeft(y, STR_CURRENT);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.telemetry.currentSource, attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.currentSource, FRSKY_CURRENT_SOURCE_LAST);
      break;

    case ITEM_TELEMETRY_FAS_OFFSET:
      lcdDrawTextLeft(y, STR_FAS_OFFSET);
      lcdDrawNumberNAtt(TELEM_COL2, y, g_model.telemetry.fasOffset, attr|LEFT|PREC1);
      lcdDrawNumberNAtt(TELEM_COL2+6*FW, y, telemetryData.value.current, LEFT|PREC1);
      lcdDrawChar(TELEM_COL2+8*FW, y, 'A');
      if (attr) g_model.telemetry.fasOffset = checkIncDec(event, g_model.telemetry.fasOffset, -120, 120, EE_MODEL);
      break;

#if defined(VARIO)
    case ITEM_TELEMETRY_VARIO_LABEL:
      lcdDrawTextLeft(y, STR_VARIO);
      break;

    case ITEM_TELEMETRY_VARIO_SOURCE:
      lcdDrawTextLeft(y, STR_SOURCE);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_VARIOSRC, g_model.telemetry.varioSource, attr);
      if (attr) CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioSource, 0, VARIO_SOURCE_LAST);
      break;

    case ITEM_TELEMETRY_VARIO_RANGE:
      lcdDrawTextLeft(y, STR_LIMIT);
      lcdDrawNumberNAtt(TELEM_COL2, y, -10+g_model.telemetry.varioMin, (menuHorizontalPosition<=0 ? attr : 0)|LEFT);
      lcdDrawNumberNAtt(TELEM_COL2+6*FW-2, y, -5+g_model.telemetry.varioCenterMin, ((CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0)|PREC1);
      lcdDrawNumberNAtt(TELEM_COL2+9*FW, y, 5+g_model.telemetry.varioCenterMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0)|PREC1);
      lcdDrawNumberNAtt(TELEM_COL2+12*FW, y, 10+g_model.telemetry.varioMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==3) ? attr : 0));
      if (attr && (s_editMode>0 || p1valdiff)) {
        switch (menuHorizontalPosition) {
        case 0:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioMin, -7, 7);
          break;
        case 1:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioCenterMin, -16, 5+min<int8_t>(10, g_model.telemetry.varioCenterMax+5));
          break;
        case 2:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioCenterMax, -5+max<int8_t>(-10, g_model.telemetry.varioCenterMin-5), +15);
          break;
        case 3:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioMax, -7, 7);
          break;
        }
      }
      break;
#endif

    case ITEM_TELEMETRY_SCREEN_LABEL1:
    case ITEM_TELEMETRY_SCREEN_LABEL2: {
      uint8_t screenIndex = (k < ITEM_TELEMETRY_SCREEN_LABEL2 ? 1 : 2);
      lcdDrawStringWithIndex(0*FW, y, STR_SCREEN, screenIndex);
      uint8_t screenType = g_model.telemetry.screensType & screenIndex;
      if (screenType != (bool)selectMenuItem(TELEM_SCRTYPE_COL, y, PSTR(""), STR_VTELEMSCREENTYPE, screenType, 0, 1, attr, event))
        g_model.telemetry.screensType ^= screenIndex;
      break;
    }

    case ITEM_TELEMETRY_SCREEN_LINE1:
    case ITEM_TELEMETRY_SCREEN_LINE2:
    case ITEM_TELEMETRY_SCREEN_LINE3:
    case ITEM_TELEMETRY_SCREEN_LINE4:
    case ITEM_TELEMETRY_SCREEN_LINE5:
    case ITEM_TELEMETRY_SCREEN_LINE6:
    case ITEM_TELEMETRY_SCREEN_LINE7:
    case ITEM_TELEMETRY_SCREEN_LINE8:

    {
      uint8_t screenIndex, lineIndex;
      if (k < ITEM_TELEMETRY_SCREEN_LABEL2) {
        screenIndex = 0;
        lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE1;
      } else {
        screenIndex = 1;
        lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE5;
      }

      if (IS_BARS_SCREEN(screenIndex)) {
        FrSkyBarData & bar = g_model.telemetry.screens[screenIndex].bars[lineIndex];
        source_t barSource = bar.source;
        lcdDrawTextAtIndex(TELEM_COL1, y, STR_VTELEMCHNS, barSource, menuHorizontalPosition==0 ? attr : 0);
        if (barSource) {
          lcdPutsTelemetryChannelValue(TELEM_BARS_COLMIN, y, barSource-1, convertBarTelemValue(barSource, bar.barMin), (menuHorizontalPosition==1 ? attr : 0) | LEFT);
          lcdPutsTelemetryChannelValue(TELEM_BARS_COLMAX, y, barSource-1, convertBarTelemValue(barSource, 255-bar.barMax), (menuHorizontalPosition==2 ? attr : 0) | LEFT);
        } else if (attr && menuHorizontalPosition>0) {
          menuHorizontalPosition = 0;
        }
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
          case 0:
            bar.source = CHECK_INCDEC_MODELVAR_ZERO(event, barSource, TELEM_DISPLAY_MAX);
            if (checkIncDec_Ret) {
              bar.barMin = 0;
              bar.barMax = 255 - maxBarTelemValue(bar.source);
            }
            break;
          case 1:
            bar.barMin = checkIncDec(event, bar.barMin, 0, 254-bar.barMax, EE_MODEL|NO_INCDEC_MARKS);
            break;
          case 2:
            bar.barMax = 255 - checkIncDec(event, 255-bar.barMax, bar.barMin+1, maxBarTelemValue(barSource), EE_MODEL|NO_INCDEC_MARKS);
            break;
          }
        }
      } else
      {
        for (uint8_t c=0; c<NUM_LINE_ITEMS; c++) {
          uint8_t cellAttr = (menuHorizontalPosition==c ? attr : 0);
          source_t & value = g_model.telemetry.screens[screenIndex].lines[lineIndex].sources[c];
          uint8_t pos[] = {INDENT_WIDTH, TELEM_COL2};
          lcdDrawTextAtIndex(pos[c], y, STR_VTELEMCHNS, value, cellAttr);
          if (cellAttr && (s_editMode>0 || p1valdiff)) {
            CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, value, (lineIndex==3 && c==0) ? TELEM_STATUS_MAX : TELEM_DISPLAY_MAX, isTelemetrySourceAvailable);
          }
        }
        if (attr && menuHorizontalPosition == NUM_LINE_ITEMS) {
          REPEAT_LAST_CURSOR_MOVE();
        }
      }
      break;
    }
    }
  }
}
