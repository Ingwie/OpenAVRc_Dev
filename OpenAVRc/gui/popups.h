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


#ifndef popup_h
#define popup_h

#include "menus.h"

const pm_char * warningText = NULL;
const pm_char * warningInfoText;
uint8_t         warningInfoLength;
uint8_t         warningType;
uint8_t         warningResult = 0;

const pm_uchar asterisk_lbm[] PROGMEM = {
#include "../bitmaps/asterisk.lbm"
};

#if defined(NAVIGATION_MENUS)
const char *popupMenuItems[POPUP_MENU_MAX_LINES];
uint8_t s_menu_item = 0;
uint16_t popupMenuNoItems = 0;
uint8_t popupMenuFlags = 0;
uint16_t popupMenuOffset = 0;
#endif // defined


#endif
