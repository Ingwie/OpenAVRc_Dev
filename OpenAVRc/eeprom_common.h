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


#define WRITE_DELAY_10MS 200

#define TIME_TO_WRITE() (eepromVars.s_eeDirtyMsk && (tmr10ms_t)(getTmr10ms() - s_eeDirtyTime10ms) >= (tmr10ms_t)WRITE_DELAY_10MS)

extern tmr10ms_t s_eeDirtyTime10ms;

void eeDirty(uint8_t msk);
void eeCheck(uint8_t immediately);
void eeReadAll();
uint8_t eeModelExists(uint8_t id);
void eeLoadModel(uint8_t id);
uint8_t eeConvert();
void eeErase(uint8_t warn);
uint8_t eeFindEmptyModel(uint8_t id, uint8_t down);
void selectModel(uint8_t sub);
