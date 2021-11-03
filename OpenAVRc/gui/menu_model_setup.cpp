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

#define MEMPROTO      shared_u8
#define PROTO_IS_SYNC (MEMPROTO == protocol)

enum menuModelSetupItems
{
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
 ITEM_MODEL_THROTTLE_TRACE,
 ITEM_MODEL_THROTTLE_SWITCH,
 ITEM_MODEL_THROTTLE_TRIM,
 ITEM_MODEL_THROTTLE_WARNING,
 ITEM_MODEL_SWITCHES_WARNING,
 ITEM_MODEL_BEEP_CENTER,
 ITEM_MODEL_PROTOCOL,
 ITEM_MODEL_PROTOCOL_PARAMS_LINE_1,
//Used with(MULTIMODULE) || (SPIMODULES)
 ITEM_MODEL_PROTOCOL_PARAMS_LINE_2,
 ITEM_MODEL_PROTOCOL_PARAMS_LINE_3,
 ITEM_MODEL_PROTOCOL_PARAMS_LINE_4,
 ITEM_MODEL_PROTOCOL_PARAMS_LINE_5,
//#endif
//Used with(SPIMODULES)
 ITEM_MODEL_PROTOCOL_PARAMS_LINE_6,
 ITEM_MODEL_PROTOCOL_PARAMS_LINE_7,
 ITEM_MODEL_PROTOCOL_PARAMS_LINE_8,
 ITEM_MODEL_SETUP_MAX

};

#define FIELD_PROTOCOL_MAX 1

#define MODEL_SETUP_2ND_COLUMN        (LCD_W-11*FW-MENUS_SCROLLBAR_WIDTH)
#define MODEL_SETUP_BIND_OFS          2*FW+1
#define MODEL_SETUP_RANGE_OFS         4*FW+3
#define MODEL_SETUP_SET_FAILSAFE_OFS  7*FW-2

void menuModelSetup(uint8_t event)
{
#define CURSOR_ON_CELL         (true)
#define MODEL_SETUP_MAX_LINES  (IS_PPM_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_1+2 : \
 (IS_DSM2_SERIAL_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_2+2 : \
 (IS_MULTIMODULE_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_5+2 :  \
 (IS_SPIMODULES_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_8+2 :   \
 1

 uint8_t protocol = g_model.rfProtocol;
 MEMPROTO = protocol;
 MENU_TAB({ 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 0, 1, 0, 0, 0, 0, 0, NUM_SWITCHES-1, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1, FIELD_PROTOCOL_MAX,
            2,2,0,2,0,0,0,0
          });


 MENU_CHECK(menuTabModel, e_ModelSetup, MODEL_SETUP_MAX_LINES+2);


 TITLE(STR_MENUSETUP);

 uint8_t sub = menuVerticalPosition - 1;
 int8_t editMode = s_editMode;

 for (uint8_t i=0; i<NUM_BODY_LINES; ++i)
  {
   coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
   uint8_t k = i+menuVerticalOffset;

   LcdFlags blink = ((editMode>0) ? BLINK|INVERS : INVERS);
   LcdFlags attr = (sub == k ? blink : 0);

   switch(k)
    {
    case ITEM_MODEL_NAME:
     editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_NAME, g_model.name, sizeof(g_model.name), event, attr, EE_MODEL, RANGE_NONE);
#if defined(SIMU)
     if ((attr) && (editMode>0))
      {
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
    case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP:
    {
     TimerData *timer = &g_model.timers[k>=ITEM_MODEL_TIMER2 ? 1 : 0];
     if (k==ITEM_MODEL_TIMER1_MINUTE_BEEP || k==ITEM_MODEL_TIMER2_MINUTE_BEEP)
      {
       timer->minuteBeep = onoffMenuItem(timer->minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
      }
     else if (k==ITEM_MODEL_TIMER1_COUNTDOWN_BEEP || k==ITEM_MODEL_TIMER2_COUNTDOWN_BEEP)
      {
       timer->countdownBeep = onoffMenuItem(timer->countdownBeep, MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, attr, event);
      }
     else
      {
       lcdDrawStringWithIndex(0*FW, y, STR_TIMER, k>=ITEM_MODEL_TIMER2 ? 2 : 1);
       lcdPutsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, menuHorizontalPosition==0 ? attr : 0);
       putsTimer(MODEL_SETUP_2ND_COLUMN+5*FW-2+5*FWNUM+1, y, timer->start, menuHorizontalPosition==1 ? attr : 0, menuHorizontalPosition==2 ? attr : 0);
       if (attr && (editMode>0 || p1valdiff))
        {
         div_t qr = div(timer->start, 60);
         switch (menuHorizontalPosition)
          {
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
           if ((int16_t)timer->start < 0)
            timer->start=0;
           if ((int16_t)timer->start > 5999)
            timer->start=32399; // 8:59:59
           break;
          }
        }
      }
     break;
    }

    case ITEM_MODEL_TIMER1_PERSISTENT:
    case ITEM_MODEL_TIMER2_PERSISTENT:
    {
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
     if (attr && menuHorizontalPosition>0)
      {
       s_editMode = 0;
       if (event==EVT_KEY_LONG(KEY_ENTER))
        {
         START_NO_HIGHLIGHT();
         for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++)
          {
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

    case ITEM_MODEL_THROTTLE_TRACE:
    {
     lcdDrawTextLeft(y, STR_TTRACE);
     if (attr)
      CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrTraceSrc, NUM_POTS+NUM_CHNOUT);
     setGazSource();
     putsMixerSource(MODEL_SETUP_2ND_COLUMN, y, gazSecurity.gazSource, attr);
     break;
    }

    case ITEM_MODEL_THROTTLE_SWITCH:
    {
     lcdDrawTextLeft(y, STR_TSWITCH);
     if (attr)
      CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrSwitch, NUM_SWITCHES-2);
     uint8_t idx = g_model.thrSwitch;
     if (idx)
      idx +=3;
     lcdPutsSwitches(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
     break;
    }

    case ITEM_MODEL_THROTTLE_TRIM:
     ON_OFF_MENU_ITEM(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
     break;

    case ITEM_MODEL_THROTTLE_WARNING:
     g_model.disableThrottleWarning = !onoffMenuItem(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
     break;

    case ITEM_MODEL_SWITCHES_WARNING:
    {
     lcdDrawTextLeft(y, STR_SWITCHWARNING);
     swarnstate_t states = g_model.switchWarningState;
     char c;
     if (attr)
      {
       s_editMode = 0;
       if (!READ_ONLY())
        {
         switch (event)
          {
          case EVT_ROTARY_BREAK:
          case EVT_KEY_BREAK(KEY_ENTER):
           if (menuHorizontalPosition < NUM_SWITCHES-1)
            {
             g_model.switchWarningEnable ^= (1 << menuHorizontalPosition);
             eeDirty(EE_MODEL);
            }
           break;

          case EVT_KEY_LONG(KEY_ENTER):
           if (menuHorizontalPosition == NUM_SWITCHES-1)
            {
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

     for (uint8_t i=0; i<NUM_SWITCHES-1/*not on TRN switch*/; i++)
      {
       uint8_t swactive = !(g_model.switchWarningEnable & 1 << i);
       attr = 0;

       if (IS_3POS(i))
        {
         c = '0'+(states & 0x03);
         states >>= 2;
        }
       else
        {
         if ((states & 0x01) && swactive)
          attr = INVERS;
         c = pgm_read_byte_near(STR_VSWITCHES - 2 + 9 + (3*(i+1)));
         states >>= 1;
        }
       if (line && (menuHorizontalPosition == i))
        {
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
     for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS; i++)
      {
       // TODO flash saving, \001 not needed in STR_RETA123
       coord_t x = MODEL_SETUP_2ND_COLUMN+i*FW;
       lcdDrawTextAtIndex(x, y, STR_RETA123, i, ((menuHorizontalPosition==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((BeepANACenter)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
      }
     if (attr && CURSOR_ON_CELL)
      {
       if (event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff)
        {
         if (READ_ONLY_UNLOCKED())
          {
           s_editMode = 0;
           g_model.beepANACenter ^= ((BeepANACenter)1<<menuHorizontalPosition);
           eeDirty(EE_MODEL);
          }
        }
      }
     break;

     int16_t tmp; // used for CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE

    case ITEM_MODEL_PROTOCOL:
     lcdDrawTextLeft(y, NO_INDENT(STR_PROTO));
#if defined(SPIMODULES)
     if IS_SPIMODULES_PROTOCOL(protocol)
      {
       lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN, y, STR_SPIM, menuHorizontalPosition<=0 ? attr : 0);
      }
     else
#endif
      {
       lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN, y, Protos[g_model.rfProtocol].ProtoName, menuHorizontalPosition<=0 ? attr : 0);
      }
     if (IS_PPM_PROTOCOL(protocol))
      {
       lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+7*FW, y, STR_NCHANNELS, g_model.PPMNCH, menuHorizontalPosition!=0 ? attr : 0);
      }
     else if (menuHorizontalPosition>0 && attr)
      {
       MOVE_CURSOR_FROM_HERE();
      }
     if (attr && (editMode>0 || p1valdiff))
      {
       switch (menuHorizontalPosition)
        {
        case 0:
#if defined(SPIMODULES)
         if IS_SPIMODULES_PROTOCOL(protocol)
          {
           uint8_t prototemp = LASTPROTOMENU1;
           CHECK_INCDEC_MODELVAR_ZERO(event, prototemp, LASTPROTOMENU1);
           if (prototemp < LASTPROTOMENU1)
            {
             protocol = prototemp;
            }
          }
         else
#endif
          {
           CHECK_INCDEC_MODELVAR_ZERO(event, protocol, LASTPROTOMENU1);
          }
         break;
        case 1:
         CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.PPMNCH, protocol==PROTOCOL_PPM ? 6 : 2); //limit 8 channels for PPMSim and PPM16
         g_model.PPMFRAMELENGTH = (g_model.PPMNCH-2) * 8;
         break;
        }
      }
     break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_1:
     if PROTO_IS_SYNC
     {
      if (IS_PPM_PROTOCOL(protocol))
        {
         lcdDrawTextLeft(y, STR_PPMFRAME);
         lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
         lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.PPMFRAMELENGTH*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
         lcdDrawChar(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
         lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.PPMDELAY*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
         lcdDrawCharAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.PULSEPOL ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
         if (attr && (editMode>0 || p1valdiff))
          {
           switch (menuHorizontalPosition)
            {
            case 0:
             CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE(event, g_model.PPMFRAMELENGTH, -20, protocol==PROTOCOL_PPM16 ? 0 : 35);
             break;
            case 1:
             CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE(event, g_model.PPMDELAY, -4, 10);
             break;
            case 2:
             CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.PULSEPOL, 1);
             break;
            }
          }
        }
#if (SERIAL_PROTOCOL==DSM2)
       if (IS_DSM2_SERIAL_PROTOCOL(protocol))
        {
         lcdDrawTextLeft(y, STR_TYPE);
         lcdDrawSizedTextAtt(MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfSubTypeNames+4*g_model.rfSubType, 4, menuHorizontalPosition == 0 ? attr : 0);
         if (attr && (editMode>0 || p1valdiff))
          {
           CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.rfSubType, RfOptionSettings.rfSubTypeMax);
          }
        }
#endif
#if defined(SPIMODULES)
       if IS_SPIMODULES_PROTOCOL(protocol)
        {
         lcdDrawTextLeft(y, NO_INDENT(STR_TYPE));
         lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN-5*FW, y, Protos[g_model.rfProtocol].ProtoName, menuHorizontalPosition == 0 ? attr : 0);
         if (RfOptionSettings.rfSubTypeMax)   // Check if Subtype exist
          {
           lcdDrawSizedTextAtt(MODEL_SETUP_2ND_COLUMN+4*FW, y, RfOptionSettings.rfSubTypeNames+4*g_model.rfSubType, 4, menuHorizontalPosition == 1 ? attr : 0);
          }
         if (attr  && (editMode>0))
          {

           switch (menuHorizontalPosition)
            {
            case 0:
            {
             CHECK_INCDEC_MODELVAR(event, protocol, LASTPROTOMENU1, PROTOCOL_COUNT-1);
             break;
            }
            case 1:
            {
             if (RfOptionSettings.rfSubTypeMax)
              {
               CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.rfSubType, RfOptionSettings.rfSubTypeMax); // TODO SUBTYPE
              }
             else
              break;
            }
            break;
            }
          }
        }
#endif
#if defined(MULTIMODULE)
       else if IS_MULTIMODULE_PROTOCOL(protocol)
        {
         uint8_t multi_rfProto = g_model.MULTIRFPROTOCOL;

         lcdDrawTextLeft(y, NO_INDENT(STR_TYPE));
         if(g_model.CUSTOMPROTO)
          {
           lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN-5*FW, y, STR_MULTI_CUSTOM, menuHorizontalPosition==0 ? attr : 0);
          }
         else
          {
           lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN-5*FW, y, STR_MULTIPROTOCOLS, multi_rfProto, menuHorizontalPosition==0 ? attr : 0);
          }
         const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_rfProto);
         if(g_model.CUSTOMPROTO)
          {
           lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 3 * FW, y, g_model.MULTIRFPROTOCOL, (menuHorizontalPosition == 1 ? attr : 0));
           lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfSubType, (menuHorizontalPosition == 2 ? attr : 0));
          }
         else
          {
           if (pdef->subTypeString != 0)
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+ 2 * FW, y, pdef->subTypeString, g_model.rfSubType, (menuHorizontalPosition == 1 ? attr : 0));
          }

         if (attr  && (editMode>0))
          {

           switch (menuHorizontalPosition)
            {
            case 0:
            {
             int8_t multiRfProto = g_model.CUSTOMPROTO == true ? MM_RF_PROTO_CUSTOM : g_model.MULTIRFPROTOCOL;
             CHECK_INCDEC_MODELVAR(event, multiRfProto, MM_RF_PROTO_FIRST, MM_RF_PROTO_LAST);
             if (checkIncDec_Ret)
              {
               g_model.CUSTOMPROTO = (multiRfProto == MM_RF_PROTO_CUSTOM);
               if (!g_model.CUSTOMPROTO)
                g_model.MULTIRFPROTOCOL = multiRfProto;
               g_model.rfSubType = 0;
               if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2)
                {
                 g_model.AUTOBINDMODE = 1;
                }
               else
                {
                 g_model.AUTOBINDMODE = 0;
                }
               g_model.rfOptionValue2 = 0;
              }
            }
            break;
            case 1:
            {
             if (g_model.CUSTOMPROTO)
              {
               CHECK_INCDEC_MODELVAR_ZERO(event, multi_rfProto, MULTI_RF_PROTO_LAST);
               g_model.MULTIRFPROTOCOL = multi_rfProto;
              }
             else if (pdef->maxSubtype > 0)
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
#endif
      }
     break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_2:
     if PROTO_IS_SYNC
     {
#if (SERIAL_PROTOCOL==DSM2)
      if (IS_DSM2_SERIAL_PROTOCOL(protocol))
        {
         horzpos_t l_posHorz = menuHorizontalPosition;
         lcdDrawTextLeft(y, STR_RXNUM);
         coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
         if (xOffsetBind)
          lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
         if (attr && l_posHorz==0)
          {
           if (editMode>0 || p1valdiff)
            {
             CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, MAX_MODELS);
            }
          }
         lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
         lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

         if (attr && l_posHorz>0 && s_editMode>0)
          {
           if (l_posHorz == 1)
            {
             PROTOCOL_SetBindState(1000); // 10 Sec
            }
           else if (l_posHorz == 2)
            {
             systemBolls.rangeModeIsOn = true;
            }
          }
        }
#endif
#if defined(MULTIMODULE)
       if IS_MULTIMODULE_PROTOCOL(protocol)
        {
         horzpos_t l_posHorz = menuHorizontalPosition;
         lcdDrawTextLeft(y, STR_RXNUM);
         coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
         if (xOffsetBind)
          lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
         if (attr && l_posHorz==0)
          {
           if (editMode>0 || p1valdiff)
            {
             CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, 15);
            }
          }
         lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
         lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

         if (attr && l_posHorz>0 && s_editMode>0)
          {
           if (l_posHorz == 1)
            PROTOCOL_SetBindState(500); // 5 Sec
           else if (l_posHorz == 2)
            {
             systemBolls.rangeModeIsOn = true;
            }
          }
        }
#endif
#if defined(SPIMODULES)
       if IS_SPIMODULES_PROTOCOL(protocol)
        {
         horzpos_t l_posHorz = menuHorizontalPosition;
         lcdDrawTextLeft(y, STR_RXNUM);
         coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
         if (xOffsetBind)
          lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
         if (attr && l_posHorz==0)
          {
           if (editMode>0 || p1valdiff)
            {
             CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, MAX_MODELS);
            }
          }
         lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
         lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

         if (attr && l_posHorz>0 && s_editMode>0)
          {
           if (l_posHorz == 1)
            {
             if (systemBolls.protoMode != BIND_MODE)
              {
               systemBolls.protoMode = BIND_MODE;
               startPulses(PROTOCMD_BIND);
              }
            }
           else if (l_posHorz == 2)
            {
             systemBolls.rangeModeIsOn = true;
            }
          }
         if (attr && l_posHorz == 1 && systemBolls.protoMode == BIND_MODE && s_editMode<1)
          {
           systemBolls.protoMode = NORMAL_MODE;
           startPulses(PROTOCMD_INIT);
          }
        }
#endif
      }
     break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_3:
     if PROTO_IS_SYNC
     {
#if defined(MULTIMODULE)
      if IS_MULTIMODULE_PROTOCOL(protocol)
        {
         if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2)
          ON_OFF_MENU_ITEM(g_model.AUTOBINDMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_DSM_AUTODTECT, attr, event);
         else
          ON_OFF_MENU_ITEM(g_model.AUTOBINDMODE, MODEL_SETUP_2ND_COLUMN, y, STR_AUTOBIND, attr, event);
        }
#endif
#if defined(SPIMODULES)
       if IS_SPIMODULES_PROTOCOL(protocol)
        {
         if (RfOptionSettings.rfOptionValue1Max != RfOptionSettings.rfOptionValue1Min)
          {
           lcdDrawTextLeft(y, RfOptionSettings.rfOptionValue1Name);
           lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfOptionValue1, attr);
           if (attr)
            {
             CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue1, RfOptionSettings.rfOptionValue1Min, RfOptionSettings.rfOptionValue1Max);
            }
          }
         else
          {
           lcdDrawTextAtt(0,y,STR_DUMMY,attr);
          }
        }
#endif
      }
     break;
    case  ITEM_MODEL_PROTOCOL_PARAMS_LINE_4: //Option1 for SPIMOD
     if PROTO_IS_SYNC
     {
#if defined(MULTIMODULE)
      if IS_MULTIMODULE_PROTOCOL(protocol)
        {
         ON_OFF_MENU_ITEM(g_model.LOWPOWERMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_LOWPOWER, attr, event);
        }
#endif
#if defined(SPIMODULES)
       if IS_SPIMODULES_PROTOCOL(protocol)
        {
         if (RfOptionSettings.rfOptionValue2Max != RfOptionSettings.rfOptionValue2Min)
          {
           lcdDrawTextLeft(y, RfOptionSettings.rfOptionValue2Name);
           lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfOptionValue2, attr);
           if (attr)
            {
             CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, RfOptionSettings.rfOptionValue2Min, RfOptionSettings.rfOptionValue2Max);
            }
          }
         else
          {
           lcdDrawTextAtt(0,y,STR_DUMMY,attr);
          }
        }
#endif
      }
     break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_5:
     if PROTO_IS_SYNC
     {
#if defined(MULTIMODULE)
      if IS_MULTIMODULE_PROTOCOL(protocol)
        {
         int8_t optionValue =  g_model.rfOptionValue2;
         const uint8_t multi_proto = g_model.MULTIRFPROTOCOL;
         const mm_protocol_definition* pdef = getMultiProtocolDefinition(multi_proto);
         if (pdef->optionsstr)
          {
           lcdDrawTextLeft(y, pdef->optionsstr);
           if (multi_proto == MM_RF_PROTO_FS_AFHDS2A)
            optionValue = 50 + 5 * optionValue;
           lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 2 * FW, y, optionValue, attr);
           if (attr)
            {
             if (multi_proto == MM_RF_PROTO_FS_AFHDS2A)
              {
               CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, 0, 70);
              }
             else if (multi_proto == MM_RF_PROTO_OLRS)
              {
               CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, -1, 7);
              }
             else
              {
               CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, -128, 127);
              }
            }
          }
        }
#endif
#if defined(SPIMODULES)
       if IS_SPIMODULES_PROTOCOL(protocol)
        {
         if (RfOptionSettings.rfOptionValue3Max)
          {
           lcdDrawTextLeft(y, RfOptionSettings.rfOptionValue3Name);
           lcdDrawNumberNAtt(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfOptionValue3, attr);
           if (attr)
            {
             CHECK_INCDEC_MODELVAR_ZERO(event, g_model.rfOptionValue3, RfOptionSettings.rfOptionValue3Max);
#if defined(SIMU)
             switch (protocol)
              {
              case PROTOCOL_FRSKYD:
              case PROTOCOL_FRSKYV:
              case PROTOCOL_FRSKYX:
              case PROTOCOL_SKYARTEC:
              case PROTOCOL_CORONA:
              case PROTOCOL_HITEC:
               CC2500_ManagePower(); // Change value on gui
               break;

              case PROTOCOL_DEVO:
              case PROTOCOL_DSM:
              case PROTOCOL_J6PRO:
               CYRF_ManagePower();
               break;

              case PROTOCOL_FLYSKY:
              case PROTOCOL_AFHDS2A:
               A7105_ManagePower();
               break;

              case PROTOCOL_BAYANG:
              case PROTOCOL_CABELL:
              case PROTOCOL_STANEK:
               NRF24L01_ManagePower();
              }
#endif
            }
          }
         else
          {
           lcdDrawTextAtt(0,y,STR_DUMMY,attr);
          }
        }
#endif
      }
     break;
#if defined(SPIMODULES)
    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_6:
     if PROTO_IS_SYNC
     {
      if IS_SPIMODULES_PROTOCOL(protocol)
        {
         if (RfOptionSettings.rfOptionBool1Used)
          {
           ON_OFF_MENU_ITEM(g_model.rfOptionBool1, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool1Name, attr, event);
          }
         else
          {
           lcdDrawTextAtt(0,y,STR_DUMMY,attr);
          }
        }
      }
     break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_7:
     if PROTO_IS_SYNC
     {
      if IS_SPIMODULES_PROTOCOL(protocol)
        {
         if (RfOptionSettings.rfOptionBool2Used)
          {
           ON_OFF_MENU_ITEM(g_model.rfOptionBool2, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool2Name, attr, event);
          }
         else
          {
           lcdDrawTextAtt(0,y,STR_DUMMY,attr);
          }
        }
      }
     break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_8:
     if PROTO_IS_SYNC
     {
      if IS_SPIMODULES_PROTOCOL(protocol)
        {
         if (RfOptionSettings.rfOptionBool3Used)
          {
           ON_OFF_MENU_ITEM(g_model.rfOptionBool3, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool3Name, attr, event);
          }
         else
          {
           lcdDrawTextAtt(0,y,STR_DUMMY,attr);
          }
        }
      }
     break;

#endif
    }
  }

 if (!PROTO_IS_SYNC)
  {
   flightReset();
   systemBolls.rangeModeIsOn = false;
   g_model.rfProtocol = protocol;
   systemBolls.protoMode = NORMAL_MODE;
   startPulses(PROTOCMD_INIT);
  }
}
