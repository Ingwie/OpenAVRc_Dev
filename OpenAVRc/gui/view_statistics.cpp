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


#include "menu_general.h"

void menuStatisticsView(uint8_t event)
{
  TITLE(STR_MENUSTAT);

  switch(event) {
  case EVT_KEY_FIRST(KEY_UP):
    chainMenu(menuStatisticsDebug);
    break;

  case EVT_KEY_FIRST(KEY_EXIT):
    chainMenu(menuMainView);
    break;
  }

  lcdDrawText(  1*FW, FH*0, STR_TOTTM1TM2THRTHP);
  putsTimer(    5*FW+5*FWNUM+1, FH*1, timersStates[0].val, 0, 0);
  putsTimer(   12*FW+5*FWNUM+1, FH*1, timersStates[1].val, 0, 0);

  putsTimer(    5*FW+5*FWNUM+1, FH*2, s_timeCumThr, 0, 0);
  putsTimer(   12*FW+5*FWNUM+1, FH*2, s_timeCum16ThrP/16, 0, 0);

  putsTimer(   12*FW+5*FWNUM+1, FH*0, sessionTimer, 0, 0);

#if defined(THRTRACE)
  coord_t traceRd = (s_traceCnt < 0 ? s_traceWr : 0);
  const coord_t x = 5;
  const coord_t y = 60;
  lcdDrawSolidHorizontalLine(x-3, y, MAXTRACE+3+3);
  lcdDrawSolidVerticalLine(x, y-32, 32+3);

  for (coord_t i=0; i<MAXTRACE; i+=6) {
    lcdDrawSolidVerticalLine(x+i+6, y-1, 3);
  }
  for (coord_t i=1; i<=MAXTRACE; i++) {
    lcdDrawSolidVerticalLine(x+i, y-s_traceBuf[traceRd], s_traceBuf[traceRd]);
    traceRd++;
    if (traceRd>=MAXTRACE) traceRd = 0;
    if (traceRd==s_traceWr) break;
  }
#endif
}

#define MENU_DEBUG_COL1_OFS   (14*FW)

void menuStatisticsDebug(uint8_t event)
{
  TITLE(STR_MENUDEBUG);

  switch(event) {
  case EVT_KEY_FIRST(KEY_ENTER):
    g_tmr1Latency_min = -1;
    g_tmr1Latency_max = 0;
    maxMixerDuration  = 0;
    AUDIO_KEYPAD_UP();
    break;

  case EVT_KEY_FIRST(KEY_DOWN):
    chainMenu(menuStatisticsView);
    break;
  case EVT_KEY_FIRST(KEY_EXIT):
    chainMenu(menuMainView);
    break;
  }


  lcdDrawTextLeft(1*FH, STR_TMR1LATMAXUS);
//  lcd_outdez8(MENU_DEBUG_COL1_OFS, 1*FH, g_tmr1Latency_max/2 );
  lcdDrawNumberAttUnit(MENU_DEBUG_COL1_OFS, 1*FH, (g_tmr1Latency_max/2), UNSIGN);

  lcdDrawTextLeft(2*FH, STR_TMR1LATMINUS);
//  lcd_outdez8(MENU_DEBUG_COL1_OFS, 2*FH, g_tmr1Latency_min/2 );
  lcdDrawNumberAttUnit(MENU_DEBUG_COL1_OFS, 2*FH, (g_tmr1Latency_min/2), UNSIGN);

  lcdDrawTextLeft(3*FH, STR_TMR1JITTERUS);
//  lcd_outdez8(MENU_DEBUG_COL1_OFS, 3*FH, (g_tmr1Latency_max - g_tmr1Latency_min) /2 );
  lcdDrawNumberAttUnit(MENU_DEBUG_COL1_OFS, 3*FH, ((g_tmr1Latency_max - g_tmr1Latency_min) /2), UNSIGN);

  lcdDrawTextLeft(4*FH, STR_TMIXMAXMS);
  lcdDrawNumberAttUnit(MENU_DEBUG_COL1_OFS, 4*FH, DURATION_MS_PREC2(maxMixerDuration), PREC2);
  lcdDrawTextLeft(5*FH, STR_FREESTACKMINB);
  lcdDrawNumberAttUnit(14*FW, 5*FH, stackAvailable(), UNSIGN);
  lcdDrawTextLeft(6*FH, STR_FREERAMINB);
  lcdDrawNumberAttUnit(14*FW, 6*FH, freeRam(), UNSIGN);
  lcdDrawText(4*FW, 7*FH+1, STR_MENUTORESET);
  lcd_status_line();
}
