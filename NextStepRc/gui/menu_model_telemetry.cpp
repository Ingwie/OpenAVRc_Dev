/*
 *************************************************************
 *                      NEXTSTEPRC                           *
 *                                                           *
 *             -> Build your DIY MEGA 2560 TX                *
 *                                                           *
 *      Based on code named                                  *
 *      OpenTx - https://github.com/opentx/opentx            *
 *                                                           *
 *         Only avr code here for lisibility ;-)             *
 *                                                           *
 *  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
 *                                                           *
 *************************************************************
 */

#include "../nextsteprc.h"
//#include "menu_model.h"


enum menuModelTelemetryItems {
  ITEM_TELEMETRY_A1_LABEL,
  ITEM_TELEMETRY_A1_RANGE,
  ITEM_TELEMETRY_A1_OFFSET,
  ITEM_TELEMETRY_A1_ALARM1,
  ITEM_TELEMETRY_A1_ALARM2,
  ITEM_TELEMETRY_A2_LABEL,
  ITEM_TELEMETRY_A2_RANGE,
  ITEM_TELEMETRY_A2_OFFSET,
  ITEM_TELEMETRY_A2_ALARM1,
  ITEM_TELEMETRY_A2_ALARM2,
  ITEM_TELEMETRY_RSSI_LABEL,
  ITEM_TELEMETRY_RSSI_ALARM1,
  ITEM_TELEMETRY_RSSI_ALARM2,
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  ITEM_TELEMETRY_USR_LABEL,
  ITEM_TELEMETRY_USR_PROTO,
  ITEM_TELEMETRY_USR_BLADES,
#endif
  ITEM_TELEMETRY_USR_VOLTAGE_SOURCE,
  ITEM_TELEMETRY_USR_CURRENT_SOURCE,
#if defined(FAS_OFFSET) || !defined(CPUM64)
  ITEM_TELEMETRY_FAS_OFFSET,
#endif
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

#if defined(FRSKY)
  #define TELEM_COL1                   INDENT_WIDTH
  #if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_CZ)
    #define TELEM_COL2                 (9*FW)
  #else
    #define TELEM_COL2                 (8*FW)
  #endif
  #define TELEM_BARS_COLMIN            (56-3*FW)
  #define TELEM_BARS_COLMAX            (14*FW-3)
  #define TELEM_SCRTYPE_COL            TELEM_COL2

#define IS_RANGE_DEFINED(k)            (g_model.frsky.channels[k].ratio > 0)

  #define CHANNEL_ROWS(x)              LABEL(CHANNEL), 1, 0, 2, 2
  #define CHANNELS_ROWS                CHANNEL_ROWS(0), CHANNEL_ROWS(1),
  #define SENSORS_ROWS

#if defined(FAS_OFFSET) || !defined(CPUM64)
  #define IF_FAS_OFFSET(x)             x,
#else
  #define IF_FAS_OFFSET(x)
#endif

#if   defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  #define USRDATA_ROWS                 LABEL(UsrData), 0, 0, 0, 0, IF_FAS_OFFSET(0)
#else
  #define USRDATA_ROWS                 0, 0, IF_FAS_OFFSET(0)
#endif

#define RSSI_ROWS                      LABEL(RSSI), 1, 1,

#if defined(GAUGES)
  #define SCREEN_TYPE_ROWS             0
#else
  #define SCREEN_TYPE_ROWS             LABEL(SCREEN)
#endif

#if defined(PCBSTD)
  #define VARIO_RANGE_ROWS             1
#else
  #define VARIO_RANGE_ROWS             3
#endif

  #define TELEMETRY_TYPE_ROWS

  #define TELEMETRY_SCREEN_ROWS(x)     SCREEN_TYPE_ROWS, 2, 2, 2, 2
  #define TELEMETRY_CURRENT_CHANNEL(k) (k >= ITEM_TELEMETRY_A2_LABEL ? TELEM_ANA_A2 : TELEM_ANA_A1)


void menuModelTelemetry(uint8_t event)
{

  MENU(STR_MENUTELEMETRY, menuTabModel, e_Telemetry, ITEM_TELEMETRY_MAX+1, {0, TELEMETRY_TYPE_ROWS CHANNELS_ROWS RSSI_ROWS SENSORS_ROWS USRDATA_ROWS CASE_VARIO(LABEL(Vario)) CASE_VARIO(0) CASE_VARIO(VARIO_RANGE_ROWS) TELEMETRY_SCREEN_ROWS(0), TELEMETRY_SCREEN_ROWS(1), });

  uint8_t sub = menuVerticalPosition - 1;

  switch (event) {
    case EVT_KEY_BREAK(KEY_DOWN):
    case EVT_KEY_BREAK(KEY_UP):
    case EVT_KEY_BREAK(KEY_LEFT):
    case EVT_KEY_BREAK(KEY_RIGHT):
      if (s_editMode>0 && sub<=ITEM_TELEMETRY_RSSI_ALARM2)
        frskySendAlarms(); // update FrSky module when edit mode exited
      break;
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;

    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    uint8_t ch = TELEMETRY_CURRENT_CHANNEL(k);
    FrSkyChannelData & channel = g_model.frsky.channels[ch];
    uint8_t dest = TELEM_A1-1+ch;


    switch (k) {


      case ITEM_TELEMETRY_A1_LABEL:
      case ITEM_TELEMETRY_A2_LABEL:
        lcdDrawTextLeft(y, STR_ACHANNEL);
        lcdDrawNumberAttUnit(2*FW, y, ch+1, 0);
        lcdPutsTelemetryChannelValue(TELEM_COL2+6*FW, y, dest, frskyData.analog[ch].value, LEFT);
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
                channel.multiplier--; channel.ratio = 255;
              }
              else if (ratio == 256) {
                if (channel.multiplier < FRSKY_MULTIPLIER_MAX) { channel.multiplier++; channel.ratio = 128; }
              }
              else {
                channel.ratio = ratio;
              }
            }
          }
          else {
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

      case ITEM_TELEMETRY_A1_ALARM1:
      case ITEM_TELEMETRY_A1_ALARM2:
      case ITEM_TELEMETRY_A2_ALARM1:
      case ITEM_TELEMETRY_A2_ALARM2:
      {
        uint8_t alarm = ((k==ITEM_TELEMETRY_A1_ALARM1 || k==ITEM_TELEMETRY_A2_ALARM1) ? 0 : 1);
        lcdDrawTextLeft(y, STR_ALARM);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_VALARM, ALARM_LEVEL(ch, alarm), menuHorizontalPosition<=0 ? attr : 0);
        lcdDrawTextAtIndex(TELEM_COL2+4*FW, y, STR_VALARMFN, ALARM_GREATER(ch, alarm), (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
        lcdPutsTelemetryChannelValue(TELEM_COL2+6*FW, y, dest, channel.alarms_value[alarm], ((CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0) | LEFT);

        if (attr && (s_editMode>0 || p1valdiff)) {
          uint8_t t;
          switch (menuHorizontalPosition) {
           case 0:
             t = ALARM_LEVEL(ch, alarm);
             channel.alarms_level = (channel.alarms_level & ~(3<<(2*alarm))) + (checkIncDecModel(event, t, 0, 3) << (2*alarm));
             break;
           case 1:
             t = ALARM_GREATER(ch, alarm);
             if (t != checkIncDecModel(event, t, 0, 1)) {
               channel.alarms_greater ^= (1 << alarm);
               frskySendAlarms();
             }
             break;
           case 2:
             channel.alarms_value[alarm] = checkIncDec(event, channel.alarms_value[alarm], 0, 255, EE_MODEL);
             break;
          }
        }
        break;
      }

      case ITEM_TELEMETRY_RSSI_LABEL:
        lcdDrawTextLeft(y, PSTR("RSSI"));
        break;

      case ITEM_TELEMETRY_RSSI_ALARM1:
      case ITEM_TELEMETRY_RSSI_ALARM2: {
        uint8_t alarm = k-ITEM_TELEMETRY_RSSI_ALARM1;
        lcdDrawTextLeft(y, STR_ALARM);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_VALARM, ((2+alarm+g_model.frsky.rssiAlarms[alarm].level)%4), menuHorizontalPosition<=0 ? attr : 0);
        lcdDrawChar(TELEM_COL2+4*FW, y, '<');
        lcdDrawNumberNAtt(TELEM_COL2+6*FW, y, getRssiAlarmValue(alarm), LEFT|(menuHorizontalPosition!=0 ? attr : 0), 3);

        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.rssiAlarms[alarm].level, -3, 2); // circular (saves flash)
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.rssiAlarms[alarm].value, -30, 30);
              break;
          }
        }
        break;
      }

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
      case ITEM_TELEMETRY_USR_LABEL:
        lcdDrawTextLeft(y, STR_USRDATA);
        break;

      case ITEM_TELEMETRY_USR_PROTO:
        lcdDrawTextLeft(y, STR_PROTO);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_VTELPROTO, g_model.frsky.usrProto, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.usrProto, USR_PROTO_LAST);
        break;

      case ITEM_TELEMETRY_USR_BLADES:
        lcdDrawTextLeft(y, STR_BLADES);
        lcdDrawNumberAttUnit(TELEM_COL2+FWNUM, y, 2+g_model.frsky.blades, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.blades, MAX_BLADES);
        break;
#endif

      case ITEM_TELEMETRY_USR_VOLTAGE_SOURCE:
        lcdDrawTextLeft(y, STR_VOLTAGE);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.frsky.voltsSource+1, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.voltsSource, FRSKY_VOLTS_SOURCE_LAST);
        break;

      case ITEM_TELEMETRY_USR_CURRENT_SOURCE:
        lcdDrawTextLeft(y, STR_CURRENT);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.frsky.currentSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.frsky.currentSource, FRSKY_CURRENT_SOURCE_LAST);
        break;

#if defined(FAS_OFFSET) || !defined(CPUM64)
      case ITEM_TELEMETRY_FAS_OFFSET:
        lcdDrawTextLeft(y, STR_FAS_OFFSET);
        lcdDrawNumberAttUnit(TELEM_COL2, y, g_model.frsky.fasOffset, attr|LEFT|PREC1);
        lcdDrawNumberAttUnit(TELEM_COL2+6*FW, y, frskyData.hub.current, LEFT|PREC1);
        lcdDrawChar(TELEM_COL2+8*FW, y, 'A');
        if (attr) g_model.frsky.fasOffset = checkIncDec(event, g_model.frsky.fasOffset, -120, 120, EE_MODEL);
        break;
#endif

#if defined(VARIO)
      case ITEM_TELEMETRY_VARIO_LABEL:
        lcdDrawTextLeft(y, STR_VARIO);
        break;

      case ITEM_TELEMETRY_VARIO_SOURCE:
        lcdDrawTextLeft(y, STR_SOURCE);
        lcdDrawTextAtIndex(TELEM_COL2, y, STR_VARIOSRC, g_model.frsky.varioSource, attr);
        if (attr) CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioSource, 0, VARIO_SOURCE_LAST);
        break;

      case ITEM_TELEMETRY_VARIO_RANGE:
        lcdDrawTextLeft(y, STR_LIMIT);
#if defined(PCBSTD)
        lcdDrawNumberAttUnit(TELEM_COL2, y, 5+g_model.frsky.varioCenterMax, (menuHorizontalPosition==0 ? attr : 0)|PREC1|LEFT);
        lcdDrawNumberAttUnit(TELEM_COL2+8*FW, y, 10+g_model.frsky.varioMax, (menuHorizontalPosition==1 ? attr : 0));
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioCenterMax, -15, +15);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioMax, -7, 7);
              break;
          }
        }
#else
        lcdDrawNumberAttUnit(TELEM_COL2, y, -10+g_model.frsky.varioMin, (menuHorizontalPosition<=0 ? attr : 0)|LEFT);
        lcdDrawNumberAttUnit(TELEM_COL2+7*FW-2, y, -5+g_model.frsky.varioCenterMin, ((CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0)|PREC1);
        lcdDrawNumberAttUnit(TELEM_COL2+10*FW, y, 5+g_model.frsky.varioCenterMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0)|PREC1);
        lcdDrawNumberAttUnit(TELEM_COL2+13*FW+2, y, 10+g_model.frsky.varioMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==3) ? attr : 0));
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioMin, -7, 7);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioCenterMin, -16, 5+min<int8_t>(10, g_model.frsky.varioCenterMax+5));
              break;
            case 2:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioCenterMax, -5+max<int8_t>(-10, g_model.frsky.varioCenterMin-5), +15);
              break;
            case 3:
              CHECK_INCDEC_MODELVAR(event, g_model.frsky.varioMax, -7, 7);
              break;
          }
        }
#endif
        break;
#endif

      case ITEM_TELEMETRY_SCREEN_LABEL1:
      case ITEM_TELEMETRY_SCREEN_LABEL2:
      {
        uint8_t screenIndex = (k < ITEM_TELEMETRY_SCREEN_LABEL2 ? 1 : 2);
        lcdDrawStringWithIndex(0*FW, y, STR_SCREEN, screenIndex);
#if defined(GAUGES)
        bool screenType = g_model.frsky.screensType & screenIndex;
        if (screenType != (bool)selectMenuItem(TELEM_SCRTYPE_COL, y, PSTR(""), STR_VTELEMSCREENTYPE, screenType, 0, 1, attr, event))
          g_model.frsky.screensType ^= screenIndex;
#endif
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
        }
        else {
          screenIndex = 1;
          lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE5;
        }

#if defined(GAUGES)
        if (IS_BARS_SCREEN(screenIndex)) {
          FrSkyBarData & bar = g_model.frsky.screens[screenIndex].bars[lineIndex];
          source_t barSource = bar.source;
          lcdDrawTextAtIndex(TELEM_COL1, y, STR_VTELEMCHNS, barSource, menuHorizontalPosition==0 ? attr : 0);
          if (barSource) {
            lcdPutsTelemetryChannelValue(TELEM_BARS_COLMIN, y, barSource-1, convertBarTelemValue(barSource, bar.barMin), (menuHorizontalPosition==1 ? attr : 0) | LEFT);
            lcdPutsTelemetryChannelValue(TELEM_BARS_COLMAX, y, barSource-1, convertBarTelemValue(barSource, 255-bar.barMax), (menuHorizontalPosition==2 ? attr : 0) | LEFT);
          }
          else if (attr && menuHorizontalPosition>0) {
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
        }
        else
#endif
        {
          for (uint8_t c=0; c<NUM_LINE_ITEMS; c++) {
            uint8_t cellAttr = (menuHorizontalPosition==c ? attr : 0);
            source_t & value = g_model.frsky.screens[screenIndex].lines[lineIndex].sources[c];
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
#endif
