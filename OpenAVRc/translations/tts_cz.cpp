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

enum CzechPrompts {
  CZ_PROMPT_NUMBERS_BASE = 0,
  CZ_PROMPT_NULA = CZ_PROMPT_NUMBERS_BASE+0,    //0-99
  CZ_PROMPT_STO = CZ_PROMPT_NUMBERS_BASE+100,   //100,200 .. 900
  CZ_PROMPT_TISIC = CZ_PROMPT_NUMBERS_BASE+109, //1000
  CZ_PROMPT_TISICE = CZ_PROMPT_NUMBERS_BASE+110,
  CZ_PROMPT_JEDEN = CZ_PROMPT_NUMBERS_BASE+111,
  CZ_PROMPT_JEDNO = CZ_PROMPT_NUMBERS_BASE+112,
  CZ_PROMPT_DVE = CZ_PROMPT_NUMBERS_BASE+113,
  CZ_PROMPT_CELA = CZ_PROMPT_NUMBERS_BASE+114,
  CZ_PROMPT_CELE = CZ_PROMPT_NUMBERS_BASE+115,
  CZ_PROMPT_CELYCH = CZ_PROMPT_NUMBERS_BASE+116,
  CZ_PROMPT_MINUS = CZ_PROMPT_NUMBERS_BASE+117,

  CZ_PROMPT_UNITS_BASE = 118, //(jeden)volt,(dva)volty,(pet)voltu,(desetina)voltu

};

#if defined(VOICE)

#define CZ_PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))

#define MUZSKY 0x80
#define ZENSKY 0x81
#define STREDNI 0x82

void pushUnitPrompt(int16_t number, uint8_t unitprompt)
{
  if (number == 1)
    PUSH_NUMBER_PROMPT(unitprompt);
  else if (number > 1 && number < 5)
    PUSH_NUMBER_PROMPT(unitprompt+1);
  else
    PUSH_NUMBER_PROMPT(unitprompt+2);
}

void playNumber(getvalue_t number, uint8_t unit, uint8_t att)
{

  if (number < 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_MINUS);
    number = -number;
  }

  if (unit) {
    unit--;
    convertUnit(number, unit);
    if (IS_IMPERIAL_ENABLE()) {
      if (unit == UNIT_DIST) {
        unit = UNIT_FEET;
      }
      if (unit == UNIT_SPEED) {
        unit = UNIT_KTS;
      }
    }
    unit++;
  }

  int8_t mode = MODE(att);
  if (mode > 0) {
    // we assume that we are PREC1
    div_t qr = div(number, 10);
    if (qr.rem) {
      playNumber(qr.quot, 0, ZENSKY);
      if (qr.quot == 0)
        PUSH_NUMBER_PROMPT(CZ_PROMPT_CELA);
      else
        CZ_PUSH_UNIT_PROMPT(qr.quot, CZ_PROMPT_CELA);
      playNumber(qr.rem, 0, ZENSKY);
      PUSH_NUMBER_PROMPT(CZ_PROMPT_UNITS_BASE+((unit-1)*4)+3);
      return;
    } else
      number = qr.quot;
  }

  int16_t tmp = number;
  switch(unit) {
  case 0:
    break;
  case 4:
  case 10:
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
  case 18:
    att = ZENSKY;
    break;
  case 8:
  case 19:
    att = STREDNI;
    break;
  default:
    att = MUZSKY;
    break;
  }

  if ((number == 1) && (att == MUZSKY)) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_JEDEN);
    number = -1;
  }

  if ((number == 1) && (att == STREDNI)) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_JEDNO);
    number = -1;
  }

  if ((number == 2) && ((att == ZENSKY) || (att == STREDNI))) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_DVE);
    number = -1;
  }

  if (number >= 1000) {
    if (number >= 2000)
      playNumber(number / 1000, 0, 0);
    if (number >= 2000 && number < 5000)
      PUSH_NUMBER_PROMPT(CZ_PROMPT_TISICE);
    else
      PUSH_NUMBER_PROMPT(CZ_PROMPT_TISIC);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_STO + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }

  if (number >= 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_NULA+number);
  }

  if (unit) {
    CZ_PUSH_UNIT_PROMPT(tmp, (CZ_PROMPT_UNITS_BASE+((unit-1)*4)));
  }
}

void playDuration(int16_t seconds )
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(CZ_PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0 || IS_PLAY_TIME()) {
    playNumber(tmp, UNIT_HOURS+1, ZENSKY);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    playNumber(tmp, UNIT_MINUTES+1, ZENSKY);
  }

  if (seconds > 0) {
    playNumber(seconds, UNIT_SECONDS+1, ZENSKY);
  }
}

LANGUAGE_PACK_DECLARE(cz, "Czech");

#endif
