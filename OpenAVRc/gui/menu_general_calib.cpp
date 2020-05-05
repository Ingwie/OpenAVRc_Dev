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
#include "menu_general.h"

#define XPOT_DELTA 10
#define XPOT_DELAY 10 /* cycles */

void menuCommonCalib(uint8_t event)
{
  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) { // get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    ReBuff.calib.loVals[i] = min(vt, ReBuff.calib.loVals[i]);
    ReBuff.calib.hiVals[i] = max(vt, ReBuff.calib.hiVals[i]);
    if (i >= POT1 && i <= POT_LAST) {
      if (IS_POT_WITHOUT_DETENT(i)) {
        ReBuff.calib.midVals[i] = (ReBuff.calib.hiVals[i] + ReBuff.calib.loVals[i]) / 2;
      }
    }
  }

  calibrationState = ReBuff.calib.state; // make sure we don't scroll while calibrating

  switch (event) {
  case EVT_ENTRY:
    ReBuff.calib.state = 0;
    break;

  case EVT_KEY_BREAK(KEY_ENTER):
    ReBuff.calib.state++;
    break;
  }

  switch (ReBuff.calib.state) {
  case 0:
    // START CALIBRATION
    if (!READ_ONLY()) {
      lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUTOSTART);
    }
    break;

  case 1:
    // SET MIDPOINT
    lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+FH, STR_SETMIDPOINT, INVERS);
    lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUWHENDONE);

    for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
      ReBuff.calib.loVals[i] = 15000;
      ReBuff.calib.hiVals[i] = -15000;
      ReBuff.calib.midVals[i] = anaIn(i);
    }
    break;

  case 2:
    // MOVE STICKS/POTS
    STICK_SCROLL_DISABLE();
    lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+FH, STR_MOVESTICKSPOTS, INVERS);
    lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUWHENDONE);

    for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
      if (abs(ReBuff.calib.loVals[i]-ReBuff.calib.hiVals[i]) > 50) {
        g_eeGeneral.calib[i].mid = ReBuff.calib.midVals[i];
        int16_t v = ReBuff.calib.midVals[i] - ReBuff.calib.loVals[i];
        g_eeGeneral.calib[i].spanNeg = v - v/STICK_TOLERANCE;
        v = ReBuff.calib.hiVals[i] - ReBuff.calib.midVals[i];
        g_eeGeneral.calib[i].spanPos = v - v/STICK_TOLERANCE;
      }
    }
    break;

  case 3:
    g_eeGeneral.chkSum = evalChkSum();
    eeDirty(EE_GENERAL);
    ReBuff.calib.state = 4;
    break;

  default:
    ReBuff.calib.state = 0;
    break;
  }

  doMainScreenGraphics();
}

void menuGeneralCalib(uint8_t event)
{
  check_simple(event, e_Calib, menuTabGeneral, DIM(menuTabGeneral), 0);

  if (menuEvent) {
    calibrationState = 0;
  }

  TITLE(STR_MENUCALIBRATION);
  menuCommonCalib(READ_ONLY() ? 0 : event);
}

void menuFirstCalib(uint8_t event)
{
  if (event == EVT_KEY_BREAK(KEY_EXIT) || ReBuff.calib.state == 4) {
    calibrationState = 0;
    chainMenu(menuMainView);
  } else {
    lcdDrawTextCenter(0*FH, MENUCALIBRATION);
    lcdInvertLine(0);
    menuCommonCalib(event);
  }
}
