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
#include "menu_general.h"


#define RADIO_SETUP_2ND_COLUMN  (LCD_W-6*FW-3-MENUS_SCROLLBAR_WIDTH)
#define RADIO_SETUP_TIME_COLUMN (FW*15+9)
#define RADIO_SETUP_DATE_COLUMN (FW*15+7)

#if !defined(CPUM64)
#define SLIDER_5POS(y, value, label, event, attr) { \
    int8_t tmp = value; \
    displaySlider(RADIO_SETUP_2ND_COLUMN, y, 2+tmp, 4, attr); \
    value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
  }
#elif defined(GRAPHICS)
#define SLIDER_5POS(y, value, label, event, attr) { \
    int8_t tmp = value; \
    display5posSlider(RADIO_SETUP_2ND_COLUMN, y, tmp, attr); \
    value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
  }
#else
#define SLIDER_5POS(y, value, label, event, attr) value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, STR_VBEEPLEN, value, -2, +2, attr, event)
#endif

#if defined(SPLASH) && !defined(FSPLASH)
#define CASE_SPLASH_PARAM(x) x,
#else
#define CASE_SPLASH_PARAM(x)
#endif

#if defined(BATTGRAPH)
#define CASE_BATTGRAPH(x) x,
#else
#define CASE_BATTGRAPH(x)
#endif

enum menuGeneralSetupItems {
  CASE_RTCLOCK(ITEM_SETUP_DATE)
  CASE_RTCLOCK(ITEM_SETUP_TIME)
  CASE_BATTGRAPH(ITEM_SETUP_BATT_RANGE)
  ITEM_SETUP_SOUND_LABEL,
  CASE_AUDIO(ITEM_SETUP_BEEP_MODE)
  CASE_BUZZER(ITEM_SETUP_BUZZER_MODE)
  //CASE_VOICE(ITEM_SETUP_SPEAKER_VOLUME)
  ITEM_SETUP_BEEP_LENGTH,
  CASE_AUDIO(ITEM_SETUP_SPEAKER_PITCH)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LABEL)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_MODE)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LENGTH)
  //CASE_HAPTIC(ITEM_SETUP_HAPTIC_STRENGTH)
  ITEM_SETUP_CONTRAST,
  ITEM_SETUP_ALARMS_LABEL,
  ITEM_SETUP_BATTERY_WARNING,
  ITEM_SETUP_INACTIVITY_ALARM,
  ITEM_SETUP_MEMORY_WARNING,
  ITEM_SETUP_ALARM_WARNING,
  IF_ROTARY_ENCODERS(ITEM_SETUP_RE_NAVIGATION)
  ITEM_SETUP_BACKLIGHT_LABEL,
  ITEM_SETUP_BACKLIGHT_MODE,
  ITEM_SETUP_BACKLIGHT_DELAY,
  CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF)
  CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON)
  ITEM_SETUP_FLASH_BEEP,
  CASE_SPLASH_PARAM(ITEM_SETUP_DISABLE_SPLASH)
  CASE_GPS(ITEM_SETUP_TIMEZONE)
  CASE_GPS(ITEM_SETUP_GPSFORMAT)
  CASE_PXX(ITEM_SETUP_COUNTRYCODE)
  IF_FAI_CHOICE(ITEM_SETUP_FAI)
  CASE_MAVLINK(ITEM_MAVLINK_BAUD)
  ITEM_SETUP_RX_CHANNEL_ORD,
  ITEM_SETUP_STICK_MODE_LABELS,
  ITEM_SETUP_STICK_MODE,
  ITEM_SETUP_MAX
};

#if defined(FRSKY_STICKS)
#define COL_TX_MODE 0
#else
#define COL_TX_MODE LABEL(TX_MODE)
#endif

void menuGeneralSetup(uint8_t event)
{
#if defined(RTCLOCK)
  struct gtm t;
  gettime(&t);

  if ((menuVerticalPosition==ITEM_SETUP_DATE+1 || menuVerticalPosition==ITEM_SETUP_TIME+1) &&
      (s_editMode>0) &&
      (event==EVT_KEY_FIRST(KEY_ENTER) || event==EVT_KEY_FIRST(KEY_EXIT) || IS_ROTARY_BREAK(event) || IS_ROTARY_LONG(event))) {
    // set the date and time into RTC chip
    rtcSetTime(&t);
  }
#endif

#if defined(FAI_CHOICE)
  if (warningResult) {
    warningResult = 0;
    g_eeGeneral.fai = true;
    eeDirty(EE_GENERAL);
  }
#endif

  MENU(STR_MENURADIOSETUP, menuTabGeneral, e_Setup, ITEM_SETUP_MAX+1, {0, CASE_RTCLOCK(2) CASE_RTCLOCK(2) CASE_BATTGRAPH(1) LABEL(SOUND), CASE_AUDIO(0) CASE_BUZZER(0) /*CASE_VOICE(0)*/ 0, CASE_AUDIO(0) CASE_HAPTIC(LABEL(HAPTIC)) CASE_HAPTIC(0) CASE_HAPTIC(0) /*CASE_HAPTIC(0)*/ 0, LABEL(ALARMS), 0, 0, 0, 0, IF_ROTARY_ENCODERS(0) LABEL(BACKLIGHT), 0, 0, CASE_PWM_BACKLIGHT(0) CASE_PWM_BACKLIGHT(0) 0, CASE_SPLASH_PARAM(0) CASE_GPS(0) CASE_GPS(0) CASE_PXX(0) IF_FAI_CHOICE(0) CASE_MAVLINK(0) 0, COL_TX_MODE, 1/*to force edit mode*/});

  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
#if defined(RTCLOCK)
    case ITEM_SETUP_DATE:
      lcdDrawTextLeft(y, STR_DATE);
      lcdDrawChar(RADIO_SETUP_DATE_COLUMN, y, '-');
      lcdDrawChar(RADIO_SETUP_DATE_COLUMN+3*FW-2, y, '-');
      for (uint8_t j=0; j<3; j++) {
        uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
        switch (j) {
        case 0:
          lcdDrawNumberAttUnit(RADIO_SETUP_DATE_COLUMN, y, t.tm_year+1900, rowattr);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_year = checkIncDec(event, t.tm_year, 112, 200, 0);
          break;
        case 1:
          lcdDrawNumberNAtt(RADIO_SETUP_DATE_COLUMN+3*FW-2, y, t.tm_mon+1, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_mon = checkIncDec(event, t.tm_mon, 0, 11, 0);
          break;
        case 2: {
          int16_t year = 1900 + t.tm_year;
          int8_t dlim = (((((year%4==0) && (year%100!=0)) || (year%400==0)) && (t.tm_mon==1)) ? 1 : 0);
          static const pm_uint8_t dmon[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
          dlim += pgm_read_byte_near(&dmon[t.tm_mon]);
          lcdDrawNumberNAtt(RADIO_SETUP_DATE_COLUMN+6*FW-4, y, t.tm_mday, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_mday = checkIncDec(event, t.tm_mday, 1, dlim, 0);
          break;
        }
        }
      }
      if (attr && checkIncDec_Ret) {
        g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
      }
      break;

    case ITEM_SETUP_TIME:
      lcdDrawTextLeft(y, STR_TIME);
      lcdDrawChar(RADIO_SETUP_TIME_COLUMN+1, y, ':');
      lcdDrawChar(RADIO_SETUP_TIME_COLUMN+3*FW-2, y, ':');
      for (uint8_t j=0; j<3; j++) {
        uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
        switch (j) {
        case 0:
          lcdDrawNumberNAtt(RADIO_SETUP_TIME_COLUMN, y, t.tm_hour, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_hour = checkIncDec(event, t.tm_hour, 0, 23, 0);
          break;
        case 1:
          lcdDrawNumberNAtt(RADIO_SETUP_TIME_COLUMN+3*FWNUM, y, t.tm_min, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_min = checkIncDec(event, t.tm_min, 0, 59, 0);
          break;
        case 2:
          lcdDrawNumberNAtt(RADIO_SETUP_TIME_COLUMN+6*FWNUM, y, t.tm_sec, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_sec = checkIncDec(event, t.tm_sec, 0, 59, 0);
          break;
        }
      }
      if (attr && checkIncDec_Ret)
        g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
      break;
#endif

#if defined(BATTGRAPH)
    case ITEM_SETUP_BATT_RANGE:
      lcdDrawTextLeft(y, STR_BATTERY_RANGE);
      lcdPutsVolts(RADIO_SETUP_2ND_COLUMN, y,  90+g_eeGeneral.vBatMin, (menuHorizontalPosition==0 ? attr : 0)|LEFT|NO_UNIT);
      lcdDrawChar(lcdLastPos, y, '-');
      lcdPutsVolts(lcdLastPos+FW, y, 120+g_eeGeneral.vBatMax, (menuHorizontalPosition>0 ? attr : 0)|LEFT|NO_UNIT);
      if (attr && s_editMode>0) {
        if (menuHorizontalPosition==0)
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMin, -50, g_eeGeneral.vBatMax+29); // min=4.0V
        else
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMax, g_eeGeneral.vBatMin-29, +40); // max=16.0V
      }
      break;
#endif

    case ITEM_SETUP_SOUND_LABEL:
      lcdDrawTextLeft(y, STR_SOUND_LABEL);
      break;

#if defined(AUDIO)
    case ITEM_SETUP_BEEP_MODE:
      g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
#if defined(FRSKY)
      if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
      break;

#if defined(BUZZER)
    case ITEM_SETUP_BUZZER_MODE:
      g_eeGeneral.buzzerMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_BUZZER, STR_VBEEPMODE, g_eeGeneral.buzzerMode, -2, 1, attr, event);
#if defined(FRSKY)
      if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
      break;
#endif
#elif defined(BUZZER)
    case ITEM_SETUP_BUZZER_MODE:
      g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
#if defined(FRSKY)
      if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
      break;
#endif

#if defined(VOICE)
      /*case ITEM_SETUP_SPEAKER_VOLUME:
      {
        lcdDrawTextLeft(y, STR_SPEAKER_VOLUME);
        uint8_t b = g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF;
        displaySlider(RADIO_SETUP_2ND_COLUMN, y, b, VOLUME_LEVEL_MAX, attr);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, b, 0, VOLUME_LEVEL_MAX);
          if (checkIncDec_Ret) {
            g_eeGeneral.speakerVolume = (int8_t)b-VOLUME_LEVEL_DEF;
            setVolume(b);
          }
        }
        break;
      } Bracame not used */
#endif


    case ITEM_SETUP_BEEP_LENGTH:
      SLIDER_5POS(y, g_eeGeneral.beepLength, STR_BEEP_LENGTH, event, attr);
      break;

#if defined(AUDIO)
    case ITEM_SETUP_SPEAKER_PITCH:
      lcdDrawTextLeft( y, STR_SPKRPITCH);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.speakerPitch, attr|LEFT);
      if (attr) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.speakerPitch, 0, 20);
      }
      break;
#endif


#if defined(HAPTIC)
    case ITEM_SETUP_HAPTIC_LABEL:
      lcdDrawTextLeft(y, STR_HAPTIC_LABEL);
      break;

    case ITEM_SETUP_HAPTIC_MODE:
      g_eeGeneral.hapticMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MODE, STR_VBEEPMODE, g_eeGeneral.hapticMode, -2, 1, attr, event);
      break;

    case ITEM_SETUP_HAPTIC_LENGTH:
      SLIDER_5POS(y, g_eeGeneral.hapticLength, STR_LENGTH, event, attr);
      break;

      /*case ITEM_SETUP_HAPTIC_STRENGTH:
        SLIDER_5POS(y, g_eeGeneral.hapticStrength, STR_HAPTICSTRENGTH, event, attr);
        break; Bracame not used */
#endif

    case ITEM_SETUP_CONTRAST:
      lcdDrawTextLeft(y, STR_CONTRAST);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.contrast, attr|LEFT);
      if (attr) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, CONTRAST_MIN, CONTRAST_MAX);
        lcdSetContrast();
      }
      break;

    case ITEM_SETUP_ALARMS_LABEL:
      lcdDrawTextLeft(y, STR_ALARMS_LABEL);
      break;

    case ITEM_SETUP_BATTERY_WARNING:
      lcdDrawTextLeft(y, STR_BATTERYWARNING);
      lcdPutsVolts(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.vBatWarn, attr|LEFT);
      if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatWarn, 40, 120); //4-12V
      break;

    case ITEM_SETUP_MEMORY_WARNING: {
      uint8_t b = 1-g_eeGeneral.disableMemoryWarning;
      g_eeGeneral.disableMemoryWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_MEMORYWARNING, attr, event);
      break;
    }

    case ITEM_SETUP_ALARM_WARNING: {
      uint8_t b = 1-g_eeGeneral.disableAlarmWarning;
      g_eeGeneral.disableAlarmWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_ALARMWARNING, attr, event);
      break;
    }

#if defined(TX_CAPACITY_MEASUREMENT)
    case ITEM_SETUP_CAPACITY_WARNING:
      lcdDrawTextLeft(y, STR_CAPAWARNING);
      lcdPutsValueWithUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.mAhWarn*50, UNIT_MAH, attr|LEFT) ;
      if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.mAhWarn, 0, 100);
      break;
#endif


    case ITEM_SETUP_INACTIVITY_ALARM:
      lcdDrawTextLeft( y,STR_INACTIVITYALARM);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.inactivityTimer, attr|LEFT);
      lcdDrawChar(lcdLastPos, y, 'm');
      if(attr) g_eeGeneral.inactivityTimer = checkIncDec(event, g_eeGeneral.inactivityTimer, 0, 250, EE_GENERAL); //0..250minutes
      break;

#if ROTARY_ENCODERS > 0
    case ITEM_SETUP_RE_NAVIGATION:
      g_eeGeneral.reNavigation = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_RENAVIG, STR_VRENAVIG, g_eeGeneral.reNavigation, 0, NUM_ROTARY_ENCODERS, attr, event);
      if (attr && checkIncDec_Ret) {
        g_rotenc[NAVIGATION_RE_IDX()] = 0;
      }
      break;
#endif

    case ITEM_SETUP_BACKLIGHT_LABEL:
      lcdDrawTextLeft(y, STR_BACKLIGHT_LABEL);
      break;

    case ITEM_SETUP_BACKLIGHT_MODE:
      g_eeGeneral.backlightMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MODE, STR_VBLMODE, g_eeGeneral.backlightMode, e_backlight_mode_off, e_backlight_mode_on, attr, event);
      break;

    case ITEM_SETUP_FLASH_BEEP:
      g_eeGeneral.alarmsFlash = onoffMenuItem(g_eeGeneral.alarmsFlash, RADIO_SETUP_2ND_COLUMN, y, STR_ALARM, attr, event ) ;
      break;

    case ITEM_SETUP_BACKLIGHT_DELAY:
      lcdDrawTextLeft(y, STR_BLDELAY);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.lightAutoOff*5, attr|LEFT);
      lcdDrawChar(lcdLastPos, y, 's');
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.lightAutoOff, 0, 600/5);
      break;


#if defined(PWM_BACKLIGHT)
    case ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF:
      lcdDrawTextLeft(y, STR_BLOFFBRIGHTNESS);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.blOffBright, attr|LEFT);
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.blOffBright, 0, 15);
      break;

    case ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON:
      lcdDrawTextLeft(y, STR_BLONBRIGHTNESS);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, 15-g_eeGeneral.blOnBright, attr|LEFT);
      if (attr) g_eeGeneral.blOnBright = 15 - checkIncDecGen(event, 15-g_eeGeneral.blOnBright, 0, 15);
      break;
#endif

#if defined(SPLASH) && !defined(FSPLASH)
    case ITEM_SETUP_DISABLE_SPLASH: {
      uint8_t b = 1-g_eeGeneral.splashMode;
      g_eeGeneral.splashMode = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_SPLASHSCREEN, attr, event);
      break;
    }
#endif

#if defined(FRSKY) && defined(FRSKY_HUB) && defined(GPS)
    case ITEM_SETUP_TIMEZONE:
      lcdDrawTextLeft(y, STR_TIMEZONE);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.timezone, attr|LEFT);
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.timezone, -12, 12);
      break;

    case ITEM_SETUP_GPSFORMAT:
      g_eeGeneral.gpsFormat = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_GPSCOORD, STR_GPSFORMAT, g_eeGeneral.gpsFormat, 0, 1, attr, event);
      break;
#endif

#if defined(PXX)
    case ITEM_SETUP_COUNTRYCODE:
      g_eeGeneral.countryCode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_COUNTRYCODE, STR_COUNTRYCODES, g_eeGeneral.countryCode, 0, 2, attr, event);
      break;
#endif


#if defined(FAI_CHOICE)
    case ITEM_SETUP_FAI:
      onoffMenuItem(g_eeGeneral.fai, RADIO_SETUP_2ND_COLUMN, y, PSTR("FAI Mode"), attr, event);
      if (attr && checkIncDec_Ret) {
        if (g_eeGeneral.fai)
          POPUP_WARNING(PSTR("FAI\001mode blocked!"));
        else
          POPUP_CONFIRMATION(PSTR("FAI mode?"));
      }
      break;
#endif

#if defined(MAVLINK)
    case ITEM_MAVLINK_BAUD:
      g_eeGeneral.mavbaud = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MAVLINK_BAUD_LABEL, STR_MAVLINK_BAUDS, g_eeGeneral.mavbaud, 0, 7, attr, event);
      break;
#endif


    case ITEM_SETUP_RX_CHANNEL_ORD:
      lcdDrawTextLeft(y, STR_RXCHANNELORD); // RAET->AETR
      for (uint8_t i=1; i<=4; i++) {
        lcdPutsChnLetter(RADIO_SETUP_2ND_COLUMN - FW + i*FW, y, channel_order(i), attr);
      }
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.templateSetup, 0, 23);
      break;

    case ITEM_SETUP_STICK_MODE_LABELS:
      lcdDrawTextLeft(y, NO_INDENT(STR_MODE));
      for (uint8_t i=0; i<4; i++) {
        lcd_img((6+4*i)*FW, y, sticks, i, 0);
#if defined(FRSKY_STICKS)
        if (g_eeGeneral.stickReverse & (1<<i)) {
          lcdDrawFilledRect((6+4*i)*FW, y, 3*FW, FH-1);
        }
#endif
      }
#if defined(FRSKY_STICKS)
      if (attr) {
        s_editMode = 0;
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.stickReverse, 0, 15);
        lcdDrawRect(6*FW-1, y-1, 15*FW+2, 9);
      }
#endif
      break;

    case ITEM_SETUP_STICK_MODE:
      lcdDrawCharAtt(2*FW, y, '1'+g_eeGeneral.stickMode, attr);
      for (uint8_t i=0; i<4; i++) {
        putsMixerSource((6+4*i)*FW, y, MIXSRC_Rud + pgm_read_byte_near(modn12x3 + 4*g_eeGeneral.stickMode + i), 0);
      }
      if (attr && s_editMode>0) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.stickMode, 0, 3);
      } else if (stickMode != g_eeGeneral.stickMode) {
        pausePulses();
        stickMode = g_eeGeneral.stickMode;
        checkTHR();
        resumePulses();
        clearKeyEvents();
      }
#if defined(ROTARY_ENCODER_NAVIGATION)
      MOVE_CURSOR_FROM_HERE();
#endif
      break;
    }
  }
}
