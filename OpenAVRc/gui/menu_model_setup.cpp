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


enum menuModelSetupItems {
  ITEM_MODEL_NAME,
  ITEM_MODEL_TIMER1,
  CASE_PERSISTENT_TIMERS(ITEM_MODEL_TIMER1_PERSISTENT)
  ITEM_MODEL_TIMER1_MINUTE_BEEP,
  ITEM_MODEL_TIMER1_COUNTDOWN_BEEP,
  ITEM_MODEL_TIMER2,
  CASE_PERSISTENT_TIMERS(ITEM_MODEL_TIMER2_PERSISTENT)
  ITEM_MODEL_TIMER2_MINUTE_BEEP,
  ITEM_MODEL_TIMER2_COUNTDOWN_BEEP,
  ITEM_MODEL_EXTENDED_LIMITS,
  ITEM_MODEL_EXTENDED_TRIMS,
  ITEM_MODEL_TRIM_INC,
  ITEM_MODEL_THROTTLE_REVERSED,
  ITEM_MODEL_THROTTLE_TRACE,
  ITEM_MODEL_THROTTLE_TRIM,
  ITEM_MODEL_THROTTLE_WARNING,
  ITEM_MODEL_SWITCHES_WARNING,
  ITEM_MODEL_BEEP_CENTER,
  ITEM_MODEL_PPM1_PROTOCOL,
  ITEM_MODEL_PPM1_PARAMS,
#if defined(MULTIMODULE) || defined(SPIMODULES)
  ITEM_MODEL_EXTERNAL_MODULE_BIND,
  ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND,
  ITEM_MODEL_EXTERNAL_MODULE_LOWPOWER,
  ITEM_MODEL_EXTERNAL_MODULE_OPTION,
#endif
  ITEM_MODEL_SETUP_MAX

};
#if defined(MULTIMODULE)
  uint8_t multiBindStatus = MULTI_NORMAL_OPERATION;
  #define MODEL_SETUP_MMsetupItemsPlus 0
#else
  #define MODEL_SETUP_MMsetupItemsPlus 4
#endif

#define FIELD_PROTOCOL_MAX 1

#define MODEL_SETUP_2ND_COLUMN        (LCD_W-11*FW-MENUS_SCROLLBAR_WIDTH)
#define MODEL_SETUP_BIND_OFS          2*FW+1
#define MODEL_SETUP_RANGE_OFS         4*FW+3
#define MODEL_SETUP_SET_FAILSAFE_OFS  7*FW-2

#define CURRENT_MODULE_EDITED(k)         (EXTERNAL_MODULE)

void menuModelSetup(uint8_t event)
{
#define CURSOR_ON_CELL                    (true)
#define MODEL_SETUP_MAX_LINES             (IS_PPM_PROTOCOL(protocol)||IS_DSM_PROTOCOL(protocol)||IS_SPIMODULES_PROTOCOL(protocol)) ? ITEM_MODEL_SETUP_MAX+MODEL_SETUP_MMsetupItemsPlus-3 : (IS_MULTIMODULE_PROTOCOL(protocol) ? ITEM_MODEL_SETUP_MAX+MODEL_SETUP_MMsetupItemsPlus+1 : ITEM_MODEL_SETUP_MAX+MODEL_SETUP_MMsetupItemsPlus-4)
  uint8_t protocol = g_model.rfProtocol;
  uint8_t memproto = protocol;
  MENU_TAB({ 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 0, 1, 0, 0, 0, 0, 0, NUM_SWITCHES, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1, FIELD_PROTOCOL_MAX, 2,
             2,0,2,0,0,0
           });


  MENU_CHECK(menuTabModel, e_ModelSetup, MODEL_SETUP_MAX_LINES);


  TITLE(STR_MENUSETUP);

  uint8_t sub = menuVerticalPosition - 1;
  int8_t editMode = s_editMode;

  for (uint8_t i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;

    LcdFlags blink = ((editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
    case ITEM_MODEL_NAME:
      editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_MODELNAME, g_model.name, sizeof(g_model.name), event, attr);
#if defined(SIMU)
      if ((attr) && (editMode>0)) {
        s_editMode = 0;
        simu_pinl |= 0x10; // Reset Menu key pin
        simu_EditModelName(); //lcd_simu_driver.cpp
      }
#endif
      break;

    case ITEM_MODEL_TIMER1:
    case ITEM_MODEL_TIMER2:
    case ITEM_MODEL_TIMER1_MINUTE_BEEP:
    case ITEM_MODEL_TIMER2_MINUTE_BEEP:
    case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
    case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP: {
      TimerData *timer = &g_model.timers[k>=ITEM_MODEL_TIMER2 ? 1 : 0];
      if (k==ITEM_MODEL_TIMER1_MINUTE_BEEP || k==ITEM_MODEL_TIMER2_MINUTE_BEEP) {
        timer->minuteBeep = onoffMenuItem(timer->minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
      } else if (k==ITEM_MODEL_TIMER1_COUNTDOWN_BEEP || k==ITEM_MODEL_TIMER2_COUNTDOWN_BEEP) {
        timer->countdownBeep = onoffMenuItem(timer->countdownBeep, MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, attr, event);
      } else {
        lcdDrawStringWithIndex(0*FW, y, STR_TIMER, k>=ITEM_MODEL_TIMER2 ? 2 : 1);
        lcdPutsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, menuHorizontalPosition==0 ? attr : 0);
        putsTimer(MODEL_SETUP_2ND_COLUMN+5*FW-2+5*FWNUM+1, y, timer->start, menuHorizontalPosition==1 ? attr : 0, menuHorizontalPosition==2 ? attr : 0);
        if (attr && (editMode>0 || p1valdiff)) {
          div_t qr = div(timer->start, 60);
          switch (menuHorizontalPosition) {
          case 0:
            CHECK_INCDEC_MODELVAR_CHECK(event, timer->mode, SWSRC_FIRST, TMRMODE_COUNT+SWSRC_LAST-1/*SWSRC_None removed*/, isSwitchAvailableInTimers);
            break;
          case 1:
            CHECK_INCDEC_MODELVAR_ZERO(event, qr.quot, 59);
            timer->start = qr.rem + qr.quot*60;
            break;
          case 2:
            qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
            timer->start -= qr.rem ;
            if ((int16_t)timer->start < 0) timer->start=0;
            if ((int16_t)timer->start > 5999) timer->start=32399; // 8:59:59
            break;
          }
        }
      }
      break;
    }

    case ITEM_MODEL_TIMER1_PERSISTENT:
    case ITEM_MODEL_TIMER2_PERSISTENT: {
      TimerData &timer = g_model.timers[k==ITEM_MODEL_TIMER2_PERSISTENT];
      timer.persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, timer.persistent, 0, 2, attr, event);
      break;
    }

    case ITEM_MODEL_EXTENDED_LIMITS:
      ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
      break;

    case ITEM_MODEL_EXTENDED_TRIMS:
      ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
      lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_RESET_BTN, (menuHorizontalPosition>0  && !NO_HIGHLIGHT()) ? attr : 0);
      if (attr && menuHorizontalPosition>0) {
        s_editMode = 0;
        if (event==EVT_KEY_LONG(KEY_ENTER)) {
          START_NO_HIGHLIGHT();
          for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
            memclear(&g_model.flightModeData[i], TRIMS_ARRAY_SIZE);
          }
          eeDirty(EE_MODEL);
          AUDIO_WARNING1();
        }
      }
      break;


    case ITEM_MODEL_TRIM_INC:
      g_model.trimInc = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
      break;

    case ITEM_MODEL_THROTTLE_REVERSED:
      ON_OFF_MENU_ITEM(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEREVERSE, attr, event ) ;
      break;

    case ITEM_MODEL_THROTTLE_TRACE: {
      lcdDrawTextLeft(y, STR_TTRACE);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrTraceSrc, NUM_POTS+NUM_CHNOUT);
      uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
      if (idx > MIXSRC_Thr)
        idx += 1;
      if (idx >= MIXSRC_FIRST_POT+NUM_POTS)
        idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS;
      putsMixerSource(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
      break;
    }

    case ITEM_MODEL_THROTTLE_TRIM:
      ON_OFF_MENU_ITEM(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
      break;


    case ITEM_MODEL_THROTTLE_WARNING:
      g_model.disableThrottleWarning = !onoffMenuItem(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
      break;

    case ITEM_MODEL_SWITCHES_WARNING: {
      lcdDrawTextLeft(y, STR_SWITCHWARNING);
      swarnstate_t states = g_model.switchWarningState;
      char c;
      if (attr) {
        s_editMode = 0;
        if (!READ_ONLY()) {
          switch (event) {
            CASE_EVT_ROTARY_BREAK
          case EVT_KEY_BREAK(KEY_ENTER):
            if (menuHorizontalPosition < NUM_SWITCHES-1) {
              g_model.switchWarningEnable ^= (1 << menuHorizontalPosition);
              eeDirty(EE_MODEL);
            }
            break;

          case EVT_KEY_LONG(KEY_ENTER):
            if (menuHorizontalPosition == NUM_SWITCHES-1) {
              START_NO_HIGHLIGHT();
              getMovedSwitch();
              g_model.switchWarningState = switches_states;
              AUDIO_WARNING1();
              eeDirty(EE_MODEL);
            }
            killEvents(event);
            break;
          }
        }
      }

      LcdFlags line = attr;

      for (uint8_t i=0; i<NUM_SWITCHES-1/*not on TRN switch*/; i++) {
        uint8_t swactive = !(g_model.switchWarningEnable & 1 << i);
        attr = 0;

        if (IS_3POS(i)) {
          c = '0'+(states & 0x03);
          states >>= 2;
        } else {
          if ((states & 0x01) && swactive)
            attr = INVERS;
          c = pgm_read_byte_near(STR_VSWITCHES - 2 + 9 + (3*(i+1)));
          states >>= 1;
        }
        if (line && (menuHorizontalPosition == i)) {
          attr = BLINK;
          if (swactive)
            attr |= INVERS;
        }
        lcdDrawCharAtt(MODEL_SETUP_2ND_COLUMN+i*FW, y, (swactive || (attr & BLINK)) ? c : '-', attr);
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+(NUM_SWITCHES*FW), y, PSTR("<]"), (menuHorizontalPosition == NUM_SWITCHES-1 && !NO_HIGHLIGHT()) ? line : 0);
      }
      break;
    }

    case ITEM_MODEL_BEEP_CENTER:
      lcdDrawTextLeft(y, STR_BEEPCTR);
      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS; i++) {
        // TODO flash saving, \001 not needed in STR_RETA123
        coord_t x = MODEL_SETUP_2ND_COLUMN+i*FW;
        lcdDrawTextAtIndex(x, y, STR_RETA123, i, ((menuHorizontalPosition==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((BeepANACenter)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
      }
      if (attr && CURSOR_ON_CELL) {
        if (event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff) {
          if (READ_ONLY_UNLOCKED()) {
            s_editMode = 0;
            g_model.beepANACenter ^= ((BeepANACenter)1<<menuHorizontalPosition);
            eeDirty(EE_MODEL);
          }
        }
      }
      break;


    case ITEM_MODEL_PPM1_PROTOCOL:
      lcdDrawTextLeft(y, NO_INDENT(STR_PROTO));
      if IS_SPIMODULES_PROTOCOL(protocol) {
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN, y, TR_SPIM, menuHorizontalPosition<=0 ? attr : 0);
      } else {
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN, y, Protos[g_model.rfProtocol].ProtoName, menuHorizontalPosition<=0 ? attr : 0);
      }

      if (IS_PPM_PROTOCOL(protocol)) {
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+7*FW, y, STR_NCHANNELS, g_model.PPMNCH, menuHorizontalPosition!=0 ? attr : 0);
      } else if (menuHorizontalPosition>0 && attr) {
        MOVE_CURSOR_FROM_HERE();
      }
      if (attr && (editMode>0 || p1valdiff)) {
        switch (menuHorizontalPosition) {
        case 0:
          CHECK_INCDEC_MODELVAR_ZERO(event, protocol, LASTPROTOMENU1);
          break;
        case 1:
          CHECK_INCDEC_MODELVAR(event, g_model.PPMNCH, 0, 6);
          g_model.PPMFRAMELENGTH = (g_model.PPMNCH-2) * 8;
          break;
        }
      }
      break;

    case ITEM_MODEL_PPM1_PARAMS:
      if (IS_PPM_PROTOCOL(protocol)) {
        lcdDrawTextLeft(y, STR_PPMFRAME);
        lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
        lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.PPMFRAMELENGTH*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
        lcdDrawChar(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
        lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.PPMDELAY*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
        lcdDrawCharAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.PULSEPOL ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
        if (attr && (editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
          case 0:
            CHECK_INCDEC_MODELVAR(event, g_model.PPMFRAMELENGTH, -20, 35);
            break;
          case 1:
            CHECK_INCDEC_MODELVAR(event, g_model.PPMDELAY, -4, 10);
            break;
          case 2:
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.PULSEPOL, 1);
            break;
          }
        }
      }
#if defined(DSM2) || defined(PXX)
      else if (IS_DSM_PROTOCOL(protocol) || IS_PXX_PROTOCOL(protocol)) {
        if (attr && menuHorizontalPosition > 1) {
          REPEAT_LAST_CURSOR_MOVE(); // limit 3 column row to 2 colums (Rx_Num and RANGE fields)
        }
        lcdDrawTextLeft(y, STR_RXNUM);
        lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN, y, g_model.modelId, (menuHorizontalPosition<=0 ? attr : 0) | LEADING0|LEFT, 2);
        if (attr && (menuHorizontalPosition==0 && (editMode>0 || p1valdiff))) {
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, MAX_MODELS);
        }
#if defined(PXX)
        if IS_PXX_PROTOCOL(protocol) {
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+4*FW, y, STR_SYNCMENU, menuHorizontalPosition!=0 ? attr : 0);
          uint8_t newFlag = 0;
          if (attr && menuHorizontalPosition>0 && editMode>0) {
            // send reset code
            newFlag = MODULE_BIND;
          }
          moduleFlag = newFlag;
        }
#endif
#if defined(DSM2)
        if (IS_DSM_PROTOCOL(protocol)) {
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+4*FW, y, STR_MODULE_RANGE, menuHorizontalPosition!=0 ? attr : 0);
          moduleFlag = (attr && menuHorizontalPosition>0 && editMode>0) ? MODULE_RANGECHECK : 0; // [MENU] key toggles range check mode
        }
#endif
      }
#endif
#if defined(SPIMODULES)
      else if IS_SPIMODULES_PROTOCOL(protocol) {

        lcdDrawTextLeft(y, NO_INDENT(STR_TYPE));
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN-5*FW, y, Protos[g_model.rfProtocol].ProtoName, menuHorizontalPosition == 0 ? attr : 0);
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+3 * FW, y, "Subtyp",  (menuHorizontalPosition == 1 ? attr : 0));
        lcd_outdez8(MODEL_SETUP_2ND_COLUMN+10 * FW, y, g_model.rfSubType);
        if (attr  && (editMode>0)) {

          switch (menuHorizontalPosition) {
          case 0: {
            CHECK_INCDEC_MODELVAR(event, protocol, LASTPROTOMENU1, PROTOCOL_COUNT-2);
            break;
          }
          case 1: {
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.rfSubType, 15); // TODO SUBTYPE
          }
          break;
          }
          break;
        }
      }
#endif
#if defined(MULTIMODULE)
      else if IS_MULTIMODULE_PROTOCOL(protocol) {

        uint8_t multi_rfProto = g_model.MULTIRFPROTOCOL;

        lcdDrawTextLeft(y, NO_INDENT(STR_TYPE));
        if(g_model.CUSTOMPROTO) {
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN-5*FW, y, STR_MULTI_CUSTOM, menuHorizontalPosition==0 ? attr : 0);
        } else {
          lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN-5*FW, y, STR_MULTIPROTOCOLS, multi_rfProto, menuHorizontalPosition==0 ? attr : 0);
        }
        const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_rfProto);
        if(g_model.CUSTOMPROTO) {
          lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 3 * FW, y, g_model.MULTIRFPROTOCOL, (menuHorizontalPosition == 1 ? attr : 0));
          lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfSubType, (menuHorizontalPosition == 2 ? attr : 0));
        } else {
          if (pdef->subTypeString != 0)
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+ 2 * FW, y, pdef->subTypeString, g_model.rfSubType, (menuHorizontalPosition == 1 ? attr : 0));
        }

        if (attr  && (editMode>0)) {

          switch (menuHorizontalPosition) {
          case 0: {
            int8_t multiRfProto = g_model.CUSTOMPROTO == 1 ? MM_RF_PROTO_CUSTOM : g_model.MULTIRFPROTOCOL;
            CHECK_INCDEC_MODELVAR(event, multiRfProto, MM_RF_PROTO_FIRST, MM_RF_PROTO_LAST);
            if (checkIncDec_Ret) {
              g_model.CUSTOMPROTO = (multiRfProto == MM_RF_PROTO_CUSTOM);
              if (!g_model.CUSTOMPROTO)
                g_model.MULTIRFPROTOCOL = multiRfProto;
              g_model.rfSubType = 0;
              if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2) {
                g_model.AUTOBINDMODE = 1;
              } else {
                g_model.AUTOBINDMODE = 0;
              }
              g_model.rfOptionValue2 = 0;
            }
          }
          break;
          case 1: {
            if (g_model.CUSTOMPROTO) {
              CHECK_INCDEC_MODELVAR_ZERO(event, multi_rfProto, MM_RF_PROTO_LAST);
              g_model.MULTIRFPROTOCOL = multi_rfProto;
            } else if (pdef->maxSubtype > 0)
              CHECK_INCDEC_MODELVAR(event, g_model.rfSubType, 0, pdef->maxSubtype);

          }
          break;
          case 2:
            // Custom protocol, third column is subtype
            CHECK_INCDEC_MODELVAR(event, g_model.rfSubType, 0, 7);
            break;

          }
        }
      }

      break;
    case ITEM_MODEL_EXTERNAL_MODULE_BIND: {
      if IS_MULTIMODULE_PROTOCOL(protocol) {
        horzpos_t l_posHorz = menuHorizontalPosition;
        lcdDrawTextLeft(y, STR_RECEIVER_NUM);
        coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
        if (xOffsetBind) lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
        if (attr && l_posHorz==0) {
          if (editMode>0 || p1valdiff) {
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, 15);
          }
        }
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);
        uint8_t newFlag = 0;
        if (multiBindStatus == MULTI_BIND_FINISHED) {
          multiBindStatus = MULTI_NORMAL_OPERATION;
          s_editMode=0;
        }
        if (attr && l_posHorz>0 && s_editMode>0) {
          if (l_posHorz == 1)
            newFlag = MODULE_BIND;
          else if (l_posHorz == 2) {
            newFlag = MODULE_RANGECHECK;
          }
        }
        moduleFlag = newFlag;
        if (newFlag == MODULE_BIND)
          multiBindStatus = MULTI_BIND_INITIATED;
      }
    }
    break;
    case   ITEM_MODEL_EXTERNAL_MODULE_OPTION:
      if IS_MULTIMODULE_PROTOCOL(protocol) {
        int optionValue =  g_model.rfOptionValue2;

        const uint8_t multi_proto = g_model.MULTIRFPROTOCOL;
        const mm_protocol_definition* pdef = getMultiProtocolDefinition(multi_proto);
        if (pdef->optionsstr) {
          lcdDrawTextLeft(y, pdef->optionsstr);
          if (multi_proto == MM_RF_PROTO_FS_AFHDS2A) optionValue = 50 + 5 * optionValue;
          lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, optionValue, attr);
          if (attr) {
            if (multi_proto == MM_RF_PROTO_FS_AFHDS2A) {
              CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, 0, 70);
            } else if (multi_proto == MM_RF_PROTO_OLRS) {
              CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, -1, 7);
            } else {
              CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, -128, 127);
            }
          }
        }
      }
      break;
    case ITEM_MODEL_EXTERNAL_MODULE_AUTOBIND:
      if IS_MULTIMODULE_PROTOCOL(protocol) {
        if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2)
          ON_OFF_MENU_ITEM(g_model.AUTOBINDMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_DSM_AUTODTECT, attr, event);
        else
          ON_OFF_MENU_ITEM(g_model.AUTOBINDMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_AUTOBIND, attr, event);
      }
      break;
    case  ITEM_MODEL_EXTERNAL_MODULE_LOWPOWER:
      if IS_MULTIMODULE_PROTOCOL(protocol)
        ON_OFF_MENU_ITEM(g_model.LOWPOWERMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_LOWPOWER, attr, event);

#endif
      break;
    }
  }
  if (memproto != protocol) {
    g_model.rfProtocol = protocol;
    SpiRFModule.mode = NORMAL_MODE;
    startPulses(PROTOCMD_INIT);
  }
}
