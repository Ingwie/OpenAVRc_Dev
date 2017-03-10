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


#include "OpenAVRc.h"
#include "stamp-OpenAVRc.h"

#define STR2(s) #s
#define DEFNUMSTR(s)  STR2(s)

#define EEPROM_STR DEFNUMSTR(EEPROM_VER);

const pm_char vers_stamp[] PROGMEM = "FW: " "OpenAVRc-" FLAVOUR "\036VERS\037\033: " VERS_STR "\036DATE\037\033: " DATE_STR"\036TIME\037\033: " TIME_STR "\036EEPR\037\033: " EEPROM_STR;
