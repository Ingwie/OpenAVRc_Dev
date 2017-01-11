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


#include "../OpenAVRc.h"

#if defined(SPLASH)

const pm_uchar splashdata[] PROGMEM = {
  'S','P','S',0,
#include "../bitmaps/splash.lbm"
  'S','P','E',0
};

#if (!defined(PCBMEGA2560) || defined(SIMU))
const pm_uchar * const splash_lbm = splashdata+4;
#endif

void displaySplash()
{
  lcdClear();
#if (defined(PCBMEGA2560) && !defined(SIMU))
  lcd_imgfar(0, 0, (pgm_get_far_address(splashdata)+4), 0, 0); //use progmem "far" for splash working with all other options enabled
#else
  lcd_img(0, 0, splash_lbm, 0, 0);
#endif

#if MENUS_LOCK == 1
  if (readonly == false) {
    lcdDrawFilledRect((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2 - 9, 50, (sizeof(TR_UNLOCKED)-1)*FW+16, 11, SOLID, ERASE|ROUND);
    lcdDrawText((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2, 53, STR_UNLOCKED);
  }
#endif

  lcdRefresh();
}
#endif
