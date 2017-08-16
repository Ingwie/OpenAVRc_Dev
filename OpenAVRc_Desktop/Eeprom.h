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


#ifndef EEPROM_H
#define EEPROM_H

#include <inttypes.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>

#define CPUM2560
#define memclear(p, s) memset(p, 0, s)
#undef min // Avoid double declaration in wingw
#undef max
template<class t> t min(t a, t b)
{
  return a<b?a:b;
}
template<class t> t max(t a, t b)
{
  return a>b?a:b;
}
#define EE_GENERAL      0x01
#define EE_MODEL        0x02

//#define EXTERNALEEPROM

#include "myeeprom.h"

class Eeprom
{
public:
  Eeprom(uint8_t ee_version,uint8_t ee_size);
  virtual ~Eeprom();
  void init();

EEGeneral  g_eeGeneral;
ModelData  g_model;

protected:

private:


};

#endif // SPIN_H
