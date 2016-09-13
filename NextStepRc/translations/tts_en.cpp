/*
 *************************************************************
 *                      NEXTSTEPRC                           *
 *                                                           *
 *             -> Build your DIY MEGA 2560 TX                *
 *                                                           *
 *      Based on code named                                  *
 *      OpenTx - https://github.com/opentx/opentx            *
 *                                                           *
 *         Only avr code here for lisibility ;-)             *
 *                                                           *
 *  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
 *                                                           *
 *************************************************************
 */

#include "../nextsteprc.h"

enum EnglishPrompts {
  EN_PROMPT_NUMBERS_BASE = 0,
  EN_PROMPT_ZERO = EN_PROMPT_NUMBERS_BASE+0,       //02-99
  EN_PROMPT_HUNDRED = EN_PROMPT_NUMBERS_BASE+100,  //100,200 .. 900
  EN_PROMPT_THOUSAND = EN_PROMPT_NUMBERS_BASE+109, //1000
  EN_PROMPT_AND = EN_PROMPT_NUMBERS_BASE+110,
  EN_PROMPT_MINUS = EN_PROMPT_NUMBERS_BASE+111,
  EN_PROMPT_POINT = EN_PROMPT_NUMBERS_BASE+112,
  EN_PROMPT_UNITS_BASE = 113,
  EN_PROMPT_POINT_BASE = 165, //.0 - .9
};

#if defined(VOICE)

  #define EN_PUSH_UNIT_PROMPT(p, u) pushUnitPrompt((p), (u))

I18N_PLAY_FUNCTION(en, pushUnitPrompt, int16_t number, uint8_t unitprompt)
{
  if (number == 1)
    PUSH_NUMBER_PROMPT(unitprompt);
  else
    PUSH_NUMBER_PROMPT(unitprompt+1);
}

I18N_PLAY_FUNCTION(en, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{
  if (number < 0) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_MINUS);
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
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_NUMBER_PROMPT(EN_PROMPT_POINT_BASE + qr.rem);
      number = -1;
    }
    else {
      number = qr.quot;
    }
  }

  int16_t tmp = number;

  if (number >= 1000) {
    PLAY_NUMBER(number / 1000, 0, 0);
    PUSH_NUMBER_PROMPT(EN_PROMPT_THOUSAND);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_HUNDRED + (number/100)-1);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (number >= 0) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_ZERO + number);
  }
  
  if (unit) {
    EN_PUSH_UNIT_PROMPT(tmp, EN_PROMPT_UNITS_BASE + unit*2);
  }
}

I18N_PLAY_FUNCTION(en, playDuration, int seconds PLAY_DURATION_ATT)
{
  if (seconds == 0) {
    PLAY_NUMBER(seconds, 0, 0);
    return;
  }

  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(EN_PROMPT_MINUS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (tmp > 0 || IS_PLAY_TIME()) {
    PLAY_NUMBER(tmp, UNIT_HOURS, 0);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    PLAY_NUMBER(tmp, UNIT_MINUTES, 0);
    if (seconds > 0)
      PUSH_NUMBER_PROMPT(EN_PROMPT_AND);
  }

  if (seconds > 0) {
    PLAY_NUMBER(seconds, UNIT_SECONDS, 0);
  }
}

LANGUAGE_PACK_DECLARE_DEFAULT(en, "English");

#endif
