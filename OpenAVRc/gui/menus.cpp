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

MenuHandlerFunc menuHandlers[5];
uint8_t menuEvent = 0;
uint8_t menuVerticalPositions[4];
uint8_t menuLevel = 0;
uint8_t shared_u8 = 0; // Reusable shared value

void popMenu()
{
  ASSERT(menuLevel>0);
  menuLevel = menuLevel-1;
  menuEvent = EVT_ENTRY_UP;
}

void chainMenu(MenuHandlerFunc newMenu)
{
  menuHandlers[menuLevel] = newMenu;
  menuEvent = EVT_ENTRY;
}

void pushMenu(MenuHandlerFunc newMenu)
{
  killEvents(KEY_ENTER);

  if (menuLevel == 0) {
    if (newMenu == menuGeneralSetup)
      menuVerticalPositions[0] = 1;
    if (newMenu == menuModelSelect)
      menuVerticalPositions[0] = 0;
  } else {
    menuVerticalPositions[menuLevel] = menuVerticalPosition;
  }

  menuLevel++;

  ASSERT(menuLevel < DIM(menuHandlers));

  menuHandlers[menuLevel] = newMenu;
  menuEvent = EVT_ENTRY;
}

