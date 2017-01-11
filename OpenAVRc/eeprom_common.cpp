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


#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "OpenAVRc.h"
#include "timers.h"

uint8_t   s_eeDirtyMsk;
tmr10ms_t s_eeDirtyTime10ms;

void eeDirty(uint8_t msk)
{
  s_eeDirtyMsk |= msk;
  s_eeDirtyTime10ms = get_tmr10ms() ;
}

uint8_t eeFindEmptyModel(uint8_t id, bool down)
{
  uint8_t i = id;
  for (;;) {
    i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
    if (!eeModelExists(i)) break;
    if (i == id) return 0xff; // no free space in directory left
  }
  return i;
}

void selectModel(uint8_t sub)
{
  displayPopup(STR_LOADINGMODEL);
  saveTimers();
  eeCheck(true); // force writing of current model data before this is changed
  g_eeGeneral.currModel = sub;
  eeDirty(EE_GENERAL);
  eeLoadModel(sub);
}

void eeReadAll()
{
  if (!eepromOpen() || !eeLoadGeneral()) {
    eeErase(true);
  } else {
    eeLoadModelHeaders();
  }
  stickMode = g_eeGeneral.stickMode;
  eeLoadModel(g_eeGeneral.currModel);
}
