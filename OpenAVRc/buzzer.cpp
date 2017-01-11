/*
**************************************************************************
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code named                            *
*             OpenTx - https://github.com/opentx/opentx                  *
*                                                                        *
*                Only AVR code here for lisibility ;-)                   *
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


#if defined(BUZZER)

#include "OpenAVRc.h"

uint8_t g_beepCnt;
uint8_t beepAgain = 0;
uint8_t beepAgainOrig = 0;
uint8_t beepOn = false;
bool warble = false;
bool warbleC;

// The various "beep" tone lengths
static const pm_uint8_t beepTab[] PROGMEM = {
  // key, trim, warn2, warn1, error
  1,  1,  2, 10,  60, //xShort
  1,  1,  4, 20,  80, //short
  1,  1,  8, 30, 100, //normal
  2,  2, 15, 40, 120, //long
  5,  5, 30, 50, 150  //xLong
};

void beep(uint8_t val)
{
#if defined(HAPTIC) && !defined(AUDIO)
  haptic.event(val==0 ? AU_KEYPAD_UP : (val==4 ? AU_ERROR : AU_TIMER_LT10+beepAgain));
#endif

#if !defined(AUDIO)
  if (g_eeGeneral.alarmsFlash && val>1) flashCounter = FLASH_DURATION;
#endif

  if (g_eeGeneral.beepMode>0 || (g_eeGeneral.beepMode==0 && val!=0) || (g_eeGeneral.beepMode==-1 && val>=3)) {
    _beep(pgm_read_byte(beepTab+5*(2+g_eeGeneral.beepLength)+val));
  }
}

#endif // defined
