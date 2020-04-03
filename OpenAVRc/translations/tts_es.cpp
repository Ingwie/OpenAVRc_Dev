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

enum SpanishPrompts {
  ES_PROMPT_NUMBERS_BASE = 0, // 256
  ES_PROMPT_ZERO = ES_PROMPT_NUMBERS_BASE+0,
  ES_PROMPT_CIEN = ES_PROMPT_NUMBERS_BASE+100,
  ES_PROMPT_CIENTO = ES_PROMPT_NUMBERS_BASE+101,
  ES_PROMPT_DOSCIENTOS = ES_PROMPT_NUMBERS_BASE+102,
  ES_PROMPT_TRESCIENTOS,
  ES_PROMPT_CUATROCIENTOS,
  ES_PROMPT_QUINIENTOS,
  ES_PROMPT_SESCIENTOS,
  ES_PROMPT_SETECIENTOS,
  ES_PROMPT_OCHOCIENTOS,
  ES_PROMPT_NUEVECIENTOS,
  ES_PROMPT_MIL = ES_PROMPT_NUMBERS_BASE+110,
  ES_PROMPT_VIRGOLA = 111, // 367
  ES_PROMPT_UN,
  ES_PROMPT_UNA,
  ES_PROMPT_Y,
  ES_PROMPT_MENO,
  ES_PROMPT_HORA,
  ES_PROMPT_HORAS,
  ES_PROMPT_MINUTO,
  ES_PROMPT_MINUTOS,
  ES_PROMPT_SEGUNDO,
  ES_PROMPT_SEGUNDOS,

  ES_PROMPT_UNITS_BASE = 122, // 378
  ES_PROMPT_VOLTS = ES_PROMPT_UNITS_BASE+UNIT_VOLTS, // 379  Note (unit-1 track 378) in PLAY function
  ES_PROMPT_AMPS = ES_PROMPT_UNITS_BASE+UNIT_AMPS,
  ES_PROMPT_METERS_PER_SECOND = ES_PROMPT_UNITS_BASE+UNIT_METERS_PER_SECOND,
  ES_PROMPT_SPARE1 = ES_PROMPT_UNITS_BASE+UNIT_RAW,
  ES_PROMPT_KMH = ES_PROMPT_UNITS_BASE+UNIT_SPEED,
  ES_PROMPT_METERS = ES_PROMPT_UNITS_BASE+UNIT_DIST,
  ES_PROMPT_DEGREES = ES_PROMPT_UNITS_BASE+UNIT_TEMPERATURE,
  ES_PROMPT_PERCENT = ES_PROMPT_UNITS_BASE+UNIT_PERCENT,
  ES_PROMPT_MILLIAMPS = ES_PROMPT_UNITS_BASE+UNIT_MILLIAMPS,
  ES_PROMPT_MAH = ES_PROMPT_UNITS_BASE+UNIT_MAH,
  ES_PROMPT_WATTS = ES_PROMPT_UNITS_BASE+UNIT_WATTS,
  ES_PROMPT_FEET = ES_PROMPT_UNITS_BASE+UNIT_FEET,
  ES_PROMPT_KTS = ES_PROMPT_UNITS_BASE+UNIT_KTS,
  ES_PROMPT_HOURS = ES_PROMPT_UNITS_BASE+UNIT_HOURS,
  ES_PROMPT_MINUTES = ES_PROMPT_UNITS_BASE+UNIT_MINUTES,
  ES_PROMPT_SECONDS = ES_PROMPT_UNITS_BASE+UNIT_SECONDS,
  ES_PROMPT_RPMS = ES_PROMPT_UNITS_BASE+UNIT_RPMS,
  ES_PROMPT_G = ES_PROMPT_UNITS_BASE+UNIT_G,

};

#if defined(VOICE)

I18N_PLAY_FUNCTION(es, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{
  /*  if digit >= 1000000000:
        temp_digit, digit = divmod(digit, 1000000000)
        prompts.extend(self.getNumberPrompt(temp_digit))
        prompts.append(Prompt(GUIDE_00_BILLION, dir=2))
    if digit >= 1000000:
        temp_digit, digit = divmod(digit, 1000000)
        prompts.extend(self.getNumberPrompt(temp_digit))
        prompts.append(Prompt(GUIDE_00_MILLION, dir=2))
  */

  if (number < 0) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_MENO);
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
    if (qr.rem > 0) {
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_NUMBER_PROMPT(ES_PROMPT_VIRGOLA);
      if (mode==2 && qr.rem < 10)
        PUSH_NUMBER_PROMPT(ES_PROMPT_ZERO);
      PLAY_NUMBER(qr.rem, unit, 0);
    } else {
      PLAY_NUMBER(qr.quot, unit, 0);
    }
    return;
  }

  if (number >= 1000) {
    if (number >= 2000) {
      PLAY_NUMBER(number / 1000, 0, 0);
      PUSH_NUMBER_PROMPT(ES_PROMPT_MIL);
    } else {
      PUSH_NUMBER_PROMPT(ES_PROMPT_MIL);
    }
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number > 100) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_CIENTO + (number/100) - 1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (number == 100) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_CIEN);
    number = -1;
  }
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_ZERO+number);
  }

  if (unit) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_UNITS_BASE+unit-1);
  }
}

I18N_PLAY_FUNCTION(es, playDuration, int16_t seconds PLAY_DURATION_ATT)
{
  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(ES_PROMPT_MENO);
    seconds = -seconds;
  }

  uint8_t ore = 0;
  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0 || IS_PLAY_TIME()) {
    ore = tmp;
    if (tmp > 1) {
      PLAY_NUMBER(tmp, 0, 0);
      PUSH_NUMBER_PROMPT(ES_PROMPT_HORAS);
    } else {
      PUSH_NUMBER_PROMPT(ES_PROMPT_UNA);
      PUSH_NUMBER_PROMPT(ES_PROMPT_HORA);
    }
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0 || ore >0) {
    if (tmp != 1) {
      PLAY_NUMBER(tmp, 0, 0);
      PUSH_NUMBER_PROMPT(ES_PROMPT_MINUTOS);
    } else {
      PUSH_NUMBER_PROMPT(ES_PROMPT_UN);
      PUSH_NUMBER_PROMPT(ES_PROMPT_MINUTO);
    }
#if !defined(NOANDSECONDE)
    PUSH_NUMBER_PROMPT(ES_PROMPT_Y);
#endif
  }

  if (seconds != 1) {
    PLAY_NUMBER(seconds, 0, 0);
    PUSH_NUMBER_PROMPT(ES_PROMPT_SEGUNDOS);
  } else {
    PUSH_NUMBER_PROMPT(ES_PROMPT_UN);
    PUSH_NUMBER_PROMPT(ES_PROMPT_SEGUNDO);
  }
}

LANGUAGE_PACK_DECLARE(es, "Espanol");

#endif
