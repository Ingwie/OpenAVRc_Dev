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
#include "widgets.h"

#define LBOX_CENTERX  (LCD_W/4 + 10)
#define RBOX_CENTERX  (3*LCD_W/4 - 10)
#define MODELNAME_X   (2*FW-2)
#define MODELNAME_Y   (0)
#define PHASE_X       (6*FW-1)
#define PHASE_Y       (2*FH)
#define PHASE_FLAGS   0
#define VBATT_X       (6*FW)
#define VBATT_Y       (2*FH)
#define VBATTUNIT_X   (VBATT_X-1)
#define VBATTUNIT_Y   (3*FH)
#define REBOOT_X      (20*FW-3)
#define VSWITCH_X(i)  (16 + 3*i)
#define VSWITCH_Y     (LCD_H-8)
#define BAR_HEIGHT    (BOX_WIDTH-1l)
#define TRIM_LH_X     (LCD_W*1/4+2)
#define TRIM_LV_X     3
#define TRIM_RV_X     (LCD_W-4)
#define TRIM_RH_X     (LCD_W*3/4-2)

#define TRIM_LEN 27

void drawPotsBars()
{
  // Optimization by Mike Blandford
  uint8_t x, i, len ;  // declare temporary variables
  for (x=LCD_W/2-5, i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; x+=5, i++) {
    if (IS_POT_AVAILABLE(i)) {
      len = ((calibratedStick[i]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      V_BAR(x, LCD_H-8, len);
    }
  }
}

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1)];

#if defined(REV_EVO_V1)
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], -calibStickVert);
#else
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], calibStickVert);
#endif

  calibStickVert = calibratedStick[CONVERT_MODE(2)];

#if defined(REV_EVO_V1)
  drawStick(RBOX_CENTERX, -calibratedStick[CONVERT_MODE(3)], calibStickVert);
#else
  drawStick(RBOX_CENTERX, calibratedStick[CONVERT_MODE(3)], calibStickVert);
#endif

  drawPotsBars();
}

void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<4; i++) {
    static coord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0,1,1,0};
    coord_t xm, ym;
    uint8_t stickIndex = CONVERT_MODE(i);
    xm = x[stickIndex];
    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

#if defined(REV_EVO_V1)
    if (i==1 || i==3) {
      val = -val;
    }
#endif

    int16_t dir = val;
    bool exttrim = false;
    if (val < TRIM_MIN || val > TRIM_MAX) {
      exttrim = true;
    }

    if (val < -(TRIM_LEN+1)*4) {
      val = -(TRIM_LEN+1);
    } else if (val > (TRIM_LEN+1)*4) {
      val = TRIM_LEN+1;
    } else {
      val /= 4;
    }

    if (vert[i]) {
      ym = 31;
      lcdDrawSolidVerticalLine(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      ym -= val;
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcdDrawSolidHorizontalLine(xm-1, ym,  3);
      }
    } else {
      ym = 60;
      lcdDrawSolidHorizontalLine(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      xm += val;
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcdDrawSolidVerticalLine(xm, ym-1,  3);
      }
    }
    lcd_square(xm-3, ym-3, 7, att);
  }
}


void displayTimers()
{
#if defined(TRANSLATIONS_CZ)
#define MAINTMR_LBL_COL (9*FW-FW/2-1)
#else
#define MAINTMR_LBL_COL (9*FW-FW/2+3)
#endif
  // Main timer
  if (g_model.timers[0].mode) {
    TimerState & timerState = timersStates[0];
    LcdFlags att = DBLSIZE | (timerState.val<0 ? BLINK|INVERS : 0);
    putsTimer(12*FW+2+10*FWNUM-4, FH*2, timerState.val, att, att);
    uint8_t xLabel = (timerState.val >= 0 ? MAINTMR_LBL_COL : MAINTMR_LBL_COL-7);
    lcdPutsTimerMode(xLabel, FH*3, g_model.timers[0].mode);
  }
}

void displayBattVoltage()
{
#if defined(BATTGRAPH)
  putsVBat(VBATT_X-8, VBATT_Y+1, 0);
#if defined(SPIMODULES)
  if (!IS_SPIMODULES_PROTOCOL(g_model.rfProtocol) || (IS_SPIMODULES_PROTOCOL(g_model.rfProtocol) && DOUBLE_BLINK_ON_PHASE))
    {
#endif
      lcdDrawFilledRect(VBATT_X-25, VBATT_Y+9, 21, 5);
      lcdDrawSolidVerticalLine(VBATT_X-4, VBATT_Y+10, 3);
      uint8_t count = GET_TXBATT_BARS();
      for (uint8_t i=0; i<count; i+=2)
        lcdDrawSolidVerticalLine(VBATT_X-24+i, VBATT_Y+10, 3);
      if (!IS_TXBATT_WARNING() || BLINK_ON_PHASE)
        lcdDrawFilledRect(VBATT_X-26, VBATT_Y, 24, 15);
#if defined(SPIMODULES)
    }
  else
    {
      lcdDrawFilledRect(VBATT_X-26, VBATT_Y, 24, 8);
      lcdDrawNumberNAtt(VBATT_X+1, VBATTUNIT_Y+1, RFPowerOut, CONDENSED|PREC2);
      lcdDrawText(VBATT_X+1,VBATTUNIT_Y+1,STR_mW);
      lcdDrawFilledRect(VBATT_X-26, VBATTUNIT_Y, 39, 9);
    }
#endif
#else
#if defined(SPIMODULES)
  if (!IS_SPIMODULES_PROTOCOL(g_model.rfProtocol) || (IS_SPIMODULES_PROTOCOL(g_model.rfProtocol) && DOUBLE_BLINK_ON_PHASE))
    {
#endif
      LcdFlags att = (IS_TXBATT_WARNING() ? BLINK|INVERS : 0) | DBLSIZE;
      putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
      lcdDrawChar(VBATT_X, VBATTUNIT_Y, 'V');
#if defined(SPIMODULES)
    }
  else
    {
      lcdDrawNumberNAtt(VBATT_X, VBATTUNIT_Y, RFPowerOut, CONDENSED|PREC2);
      lcdDrawText(VBATT_X,VBATTUNIT_Y,STR_mW);
    }
#endif
#endif
}

#define displayVoltageOrAlarm() displayBattVoltage()

#define EVT_KEY_MODEL_MENU   EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_UP)
#define EVT_KEY_CONTEXT_MENU EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_SHUTDOWN     EVT_KEY_LONG(KEY_EXIT)

#if defined(NAVIGATION_MENUS)
void onMainViewMenu(const char *result)
{
  if (result == STR_RESET_TIMER1) {
    timerReset(0);
  }
  else if (result == STR_RESET_TIMER2) {
    timerReset(1);
  }
  else if (result == STR_SAVE_TIMERS) {
    //saveTimers();
    OpenAVRcClose();
  }
#if TIMERS > 2
  else if (result == STR_RESET_TIMER3) {
    timerReset(2);
  }
#endif
#if defined(FRSKY)
  else if (result == STR_RESET_TELEMETRY) {
    telemetryResetValue();
  }
#endif
  else if (result == STR_RESET_FLIGHT) {
    flightReset();
  }
  else if (result == STR_STATISTICS) {
    chainMenu(menuStatisticsView);
  }
}
#endif

void menuMainView(uint8_t event)
{
  if (warningResult)
    {
      // Power Off
      pwrCheck = false;
    }

  STICK_SCROLL_DISABLE();

  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;

  switch(event) {

  case EVT_ENTRY:
    killEvents(KEY_EXIT);
    killEvents(KEY_UP);
    killEvents(KEY_DOWN);
    break;

  /* TODO if timer2 is OFF, it's possible to use this timer2 as in er9x...
  case EVT_KEY_BREAK(KEY_MENU):
    if (view_base == VIEW_TIMER2) {
      Timer2_running = !Timer2_running;
      AUDIO_KEYPAD_UP();
    }
  break;
  */

  case EVT_KEY_BREAK(KEY_RIGHT):
  case EVT_KEY_BREAK(KEY_LEFT):
    if (view_base < VIEW_TIMER2) {
      g_eeGeneral.view ^= ALTERNATE_VIEW;
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
    }
    break;

#if defined(NAVIGATION_MENUS)
  case EVT_KEY_CONTEXT_MENU:
    killEvents(event);


    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER1);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER2);
#if defined(FRSKY)
    POPUP_MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
    POPUP_MENU_ADD_ITEM(STR_RESET_FLIGHT);
    POPUP_MENU_ADD_ITEM(STR_STATISTICS);
    POPUP_MENU_ADD_ITEM(STR_SAVE_TIMERS);
    popupMenuHandler = onMainViewMenu;
    break;
#endif

#if MENUS_LOCK != 2 /*no menus*/
  case EVT_KEY_LONG(KEY_MENU):// go to last menu
    pushMenu(lastPopMenu());
    killEvents(event);
    break;

    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_MODEL_MENU:
    pushMenu(menuModelSelect);
    killEvents(event);
    break;

    CASE_EVT_ROTARY_LONG
  case EVT_KEY_GENERAL_MENU:
    pushMenu(menuGeneralSetup);
    killEvents(event);
    break;
#endif

  case EVT_KEY_BREAK(KEY_UP):
  case EVT_KEY_BREAK(KEY_DOWN):
    g_eeGeneral.view = (event == EVT_KEY_BREAK(KEY_UP) ? (view_base == VIEW_COUNT-1 ? 0 : view_base+1) : (view_base == 0 ? VIEW_COUNT-1 : view_base-1));
    eeDirty(EE_GENERAL);
    AUDIO_KEYPAD_UP();
    break;

  case EVT_KEY_STATISTICS:
    chainMenu(menuStatisticsView);
    killEvents(event);
    break;

  case EVT_KEY_TELEMETRY:
#if defined(FRSKY)
    if (!IS_FAI_ENABLED())
      chainMenu(menuTelemetryFrsky);
#else
    chainMenu(menuStatisticsDebug);
#endif
    killEvents(event);
    break;

  case EVT_KEY_FIRST(KEY_EXIT):

    if (view == VIEW_TIMER2) {
      timerReset(1);
    }
    AUDIO_KEYPAD_UP();
    break;

  case EVT_KEY_SHUTDOWN: // Shutdown confirmation
          POPUP_CONFIRMATION(STR_POWEROFF);
          killEvents(event);
    break;

#if !defined(NAVIGATION_MENUS)
  /*case EVT_KEY_LONG(KEY_EXIT):
    flightReset();
    AUDIO_KEYPAD_UP();
    break;*/
#endif

  }

  {
    // Flight Mode Name
    uint8_t mode = mixerCurrentFlightMode;
    lcdDrawSizedTextAtt(PHASE_X, PHASE_Y, g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[mode].name), ZCHAR|PHASE_FLAGS);

    // Model Name
#if defined(DSM2_SERIAL) || defined(MULTIMODULE) || defined(SPIMODULES)
    if ((protoMode == BIND_MODE) && !BLINK_ON_PHASE)
    {
      lcdDrawTextAtt(MODELNAME_X, MODELNAME_Y, STR_BIND, BLINK|DBLSIZE);
    } else {
      putsModelName(MODELNAME_X, MODELNAME_Y, g_model.name, g_eeGeneral.currModel, DBLSIZE);
    }
#else
    putsModelName(MODELNAME_X, MODELNAME_Y, g_model.name, g_eeGeneral.currModel, DBLSIZE);
#endif

    // Main Voltage (or alarm if any)
    displayVoltageOrAlarm();

    // Timers
    displayTimers();

    // Trims sliders
    displayTrims(mode);

    DISPLAY_PROGRESS_BAR(20*FW+1);
  }

  if (view_base < VIEW_INPUTS) {
    // scroll bar
    lcdDrawSolidHorizontalLineStip(38, 34, 54, DOTTED);
    lcdDrawSolidHorizontalLine((g_eeGeneral.view & ALTERNATE_VIEW) ? 64 : 38, 34, 26, SOLID);

    for (uint8_t i=0; i<8; i++) {
      uint8_t x0,y0;
      uint8_t chan = (g_eeGeneral.view & ALTERNATE_VIEW) ? 8+i : i;

      int16_t val = channelOutputs[chan];

      switch(view_base) {
      case VIEW_OUTPUTS_VALUES:
        x0 = (i%4*9+3)*FW/2;
        y0 = i/4*FH+40;
#if defined(PPM_UNIT_US)
        lcdDrawNumberNAtt(x0+4*FW, y0, PPM_CH_CENTER(chan)+val/2, 0);
#elif defined(PPM_UNIT_PERCENT_PREC1)
        lcdDrawNumberNAtt(x0+4*FW, y0, calcRESXto1000(val), PREC1);
#else
        lcdDrawNumberNAtt(x0+4*FW, y0, calcRESXto1000(val)/10, 0);  // G: Don't like the decimal part*
#endif
        break;

      case VIEW_OUTPUTS_BARS:
#define WBAR2 (50/2)
        x0       = i<4 ? LCD_W/4+2 : LCD_W*3/4-2;
        y0       = 38+(i%4)*5;

        uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
        int8_t len = (abs(val) * WBAR2 + lim/2) / lim;

        if(len>WBAR2)  len = WBAR2;  // prevent bars from going over the end - comment for debugging
        lcdDrawSolidHorizontalLineStip(x0-WBAR2, y0, WBAR2*2+1, DOTTED);
        lcdDrawSolidVerticalLine(x0,y0-2,5);
        if (val>0)
          x0+=1;
        else
          x0-=len;
        lcdDrawSolidHorizontalLine(x0,y0+1,len);
        lcdDrawSolidHorizontalLine(x0,y0-1,len);
        break;
      }
    }
  }
#if defined(GVARS) // Show current Gvars values
  else if (view_base == VIEW_GVARS) {
    uint8_t x0,y0;
    bool test;
#if defined(EXTERNALEEPROM)
    uint8_t offset = (g_eeGeneral.view & ALTERNATE_VIEW) ? MAX_GVARS/2 : 0;
#else
    uint8_t offset = 0;
#endif
    for (uint8_t i=0; i<6; i++) {
      x0 = (FW+FW/3)+(i%2)*2*FW*(LEN_GVAR_NAME-1)-(i%2)*FW/2;
      y0 = i/2*FH+33;
      test = false;
      for (uint8_t j=0; j<LEN_GVAR_NAME; j++) {
        if (g_model.gvars[i+offset].name[j]) {
          test = true;
          break;
        }
      }
      if (!test) lcdDrawStringWithIndex(x0+3*FW, y0, STR_GV, i+1+offset);
      else lcdDrawSizedTextAtt(x0, y0, g_model.gvars[i+offset].name, LEN_GVAR_NAME, ZCHAR|FIXEDWIDTH);
      x0 += (LEN_GVAR_NAME+3)*FW+FW/3;
      lcdDrawNumberNAtt(x0, y0, GVAR_VALUE(i+offset, getGVarFlightPhase(mixerCurrentFlightMode, i+offset)));
    }
  }
#endif
  else if (view_base == VIEW_INPUTS) {
    if (view == VIEW_INPUTS) {
      // Sticks + Pots
      doMainScreenGraphics();

      // Switches
      for (uint8_t i=SWSRC_THR; i<=SWSRC_TRN; i++) {
        int8_t sw = (i == SWSRC_TRN ? (switchState(SW_ID0) ? SWSRC_ID0 : (switchState(SW_ID1) ? SWSRC_ID1 : SWSRC_ID2)) : i);
        uint8_t x = 2*FW-2, y = i*FH+1;
        if (i>=SWSRC_AIL) {
          x = 17*FW-1;
          y -= 3*FH;
        }
        lcdPutsSwitches(x, y, sw, getSwitch(i) ? INVERS : 0);
      }
    } else {
#if defined(ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_ROTARY_ENCODERS; i++) {
        int16_t val = getRotaryEncoder(i);
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
#if ROTARY_ENCODERS > 2
#define V_BAR_W 5
        V_BAR(LCD_W/2-8+V_BAR_W*i, LCD_H-8, len);
#else
#define V_BAR_W 5
        V_BAR(LCD_W/2-3+V_BAR_W*i, LCD_H-8, len);
#endif
      }
#endif // ROTARY_ENCODERS

      // Logical Switches
      for (uint8_t i=0; i<15; i++) //Can't draw all NUM_LOGICAL_SWITCH
        lcdPutsSwitches(2*FW-3 + (i/3)*(i/3>2 ? 3*FW+2 : (3*FW-1)) + (i/3>2 ? 2*FW : 0), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
    }
  } else { // timer2
#if defined(TRANSLATIONS_CZ)
#define TMR2_LBL_COL (20-FW/2+1)
#else
#define TMR2_LBL_COL (20-FW/2+5)
#endif
    putsTimer(33+FW+2+10*FWNUM-4, FH*5, timersStates[1].val, DBLSIZE, DBLSIZE);
    lcdPutsTimerMode(timersStates[1].val >= 0 ? TMR2_LBL_COL : TMR2_LBL_COL-7, FH*6, g_model.timers[1].mode);
  }

  // And ! in case of unexpected shutdown
  if (unexpectedShutdown) {
    lcdDrawCharAtt(REBOOT_X, 0, '!', INVERS);
  }

}
