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


#ifndef translations_h
#define translations_h

#if defined(TRANSLATIONS_FR)
#include "translations/fr.h"
#define LEN_SPECIAL_CHARS 3
#elif defined(TRANSLATIONS_IT)
#include "translations/it.h"
#define LEN_SPECIAL_CHARS 1
#elif defined(TRANSLATIONS_SE)
#include "translations/se.h"
#define LEN_SPECIAL_CHARS 6
#elif defined(TRANSLATIONS_FI)
#include "translations/fi.h"
#define LEN_SPECIAL_CHARS 6
#elif defined(TRANSLATIONS_DE)
#include "translations/de.h"
#define LEN_SPECIAL_CHARS 6
#elif defined(TRANSLATIONS_CZ)
#include "translations/cz.h"
#define LEN_SPECIAL_CHARS 24
#elif defined(TRANSLATIONS_SK)
#include "translations/sk.h"
#define LEN_SPECIAL_CHARS 21
#elif defined(TRANSLATIONS_ES)
#include "translations/es.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_PL)
#include "translations/pl.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_PT)
#include "translations/pt.h"
#define LEN_SPECIAL_CHARS 0
#elif defined(TRANSLATIONS_NL)
#include "translations/nl.h"
#define LEN_SPECIAL_CHARS 0
#else
#include "translations/en.h"
#define LEN_SPECIAL_CHARS 0
#endif

#define PSIZE(x) ( sizeof(x) - 1 )
#define EOFS(x)  ( OFS_##x + sizeof(TR_##x) )

// The non-0-terminated-strings

extern const pm_char STR_OPEN9X[];
#define OFS_OFFON              0
#define OFS_MMMINV             (OFS_OFFON + sizeof(TR_OFFON))
#define OFS_NCHANNELS          (OFS_MMMINV + sizeof(TR_MMMINV))
#define OFS_VBEEPMODE          (OFS_NCHANNELS + sizeof(TR_NCHANNELS))

#define OFS_VRENAVIG           (OFS_VBEEPMODE + sizeof(TR_VBEEPMODE))
#define OFS_VRENCODERS         (OFS_VRENAVIG + sizeof(TR_VRENAVIG))
#define OFS_TRNMODE            (OFS_VRENCODERS + sizeof(TR_VRENCODERS))

#define OFS_TRNCHN             (OFS_TRNMODE + sizeof(TR_TRNMODE))
#define OFS_VTRIMINC         (OFS_TRNCHN + sizeof(TR_TRNCHN))
#define OFS_RETA123          (OFS_VTRIMINC + sizeof(TR_VTRIMINC))
#if (SERIAL_PROTOCOL==MULTIMODULE)
  #define OFS_MULTIPROTOCOLS   (OFS_RETA123 + sizeof(TR_RETA123))
  #define OFS_VBLMODE           (OFS_MULTIPROTOCOLS + sizeof(TR_MULTIPROTOCOLS))
#else
  #define OFS_VBLMODE           (OFS_RETA123 + sizeof(TR_RETA123))
#endif
#define OFS_VCURVEFUNC         (OFS_VBLMODE + sizeof(TR_VBLMODE))
#define OFS_VMLTPX             (OFS_VCURVEFUNC + sizeof(TR_VCURVEFUNC))
#define OFS_VMLTPX2            (OFS_VMLTPX + sizeof(TR_VMLTPX))
#define OFS_VMIXTRIMS          (OFS_VMLTPX2 + sizeof(TR_VMLTPX2))
#define OFS_VCSWFUNC           (OFS_VMIXTRIMS + sizeof(TR_VMIXTRIMS))
#define OFS_VFSWFUNC           (OFS_VCSWFUNC + sizeof(TR_VCSWFUNC))
#define OFS_VFSWRESET          (OFS_VFSWFUNC + sizeof(TR_VFSWFUNC))
#define OFS_FUNCSOUNDS         (OFS_VFSWRESET + sizeof(TR_VFSWRESET))
#define OFS_VTELEMCHNS         (OFS_FUNCSOUNDS + sizeof(TR_FUNCSOUNDS))
#if defined(FRSKY)
#define OFS_VTELEMUNIT      (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
#define OFS_VTELPROTO          (OFS_VTELEMUNIT + sizeof(TR_VTELEMUNIT))
#define OFS_GPSFORMAT         (OFS_VTELPROTO + sizeof(TR_VTELPROTO))
#define OFS_AMPSRC            (OFS_GPSFORMAT + sizeof(TR_GPSFORMAT))
#define OFS_VARIOSRC          (OFS_AMPSRC + sizeof(TR_AMPSRC))
#define OFS_VSCREEN           (OFS_VARIOSRC + sizeof(TR_VARIOSRC))
#define OFS_VTEMPLATES        (OFS_VSCREEN + sizeof(TR_VTELEMSCREENTYPE))
#else
#define OFS_VTEMPLATES        (OFS_VTELEMCHNS + sizeof(TR_VTELEMCHNS))
#endif
#if defined(TEMPLATES)
#define OFS_VSWASHTYPE        (OFS_VTEMPLATES + sizeof(TR_VTEMPLATES))
#else
#define OFS_VSWASHTYPE        (OFS_VTEMPLATES)
#endif
#if defined(HELI)
#define OFS_VKEYS             (OFS_VSWASHTYPE + sizeof(TR_VSWASHTYPE))
#else
#define OFS_VKEYS             (OFS_VSWASHTYPE)
#endif
#define OFS_VSWITCHES           (OFS_VKEYS + sizeof(TR_VKEYS))
#define OFS_VSRCRAW             (OFS_VSWITCHES + sizeof(TR_VSWITCHES))
#define OFS_VTMRMODES           (OFS_VSRCRAW + sizeof(TR_VSRCRAW))
#define OFS_VPERSISTENT         (OFS_VTMRMODES + sizeof(TR_VTMRMODES))
#if defined(X_ANY)
#define OFS_SWITCHES_VALUES     (OFS_VPERSISTENT + sizeof(TR_VPERSISTENT))
#define OFS_XANY_EXPO           (OFS_SWITCHES_VALUES + sizeof(TR_SWITCHES_VALUES))
#else
#define OFS_SWITCHES_VALUES     (OFS_VPERSISTENT)
#define OFS_XANY_EXPO           (OFS_VPERSISTENT)
#endif

#define STR_OFFON               (STR_OPEN9X + OFS_OFFON)
#define STR_MMMINV              (STR_OPEN9X + OFS_MMMINV)
#define STR_NCHANNELS           (STR_OPEN9X + OFS_NCHANNELS)
#define STR_VBEEPMODE           (STR_OPEN9X + OFS_VBEEPMODE)
#define STR_TRNMODE             (STR_OPEN9X + OFS_TRNMODE)
#define STR_TRNCHN              (STR_OPEN9X + OFS_TRNCHN)
#define STR_UART3MODES          (STR_OPEN9X + OFS_UART3MODES)
#define STR_SWTYPES             (STR_OPEN9X + OFS_SWTYPES)
#define STR_POTTYPES            (STR_OPEN9X + OFS_POTTYPES)
#define STR_SLIDERTYPES         (STR_OPEN9X + OFS_SLIDERTYPES)
#define STR_VTRIMINC            (STR_OPEN9X + OFS_VTRIMINC)
#define STR_VDISPLAYTRIMS       (STR_OPEN9X + OFS_VDISPLAYTRIMS)
#define STR_RETA123             (STR_OPEN9X + OFS_RETA123)
#if (SERIAL_PROTOCOL==MULTIMODULE)
  #define STR_MULTIPROTOCOLS    (STR_OPEN9X + OFS_MULTIPROTOCOLS)
#endif
#define STR_VBLMODE             (STR_OPEN9X + OFS_VBLMODE)
#define STR_VCURVEFUNC          (STR_OPEN9X + OFS_VCURVEFUNC)
#define STR_VSIDE               STR_VCURVEFUNC
#define LEN_VSIDE               LEN_VCURVEFUNC
#define STR_VMLTPX              (STR_OPEN9X + OFS_VMLTPX)
#define STR_VMLTPX2             (STR_OPEN9X + OFS_VMLTPX2)
#define STR_VMIXTRIMS           (STR_OPEN9X + OFS_VMIXTRIMS)
#define STR_VCSWFUNC            (STR_OPEN9X + OFS_VCSWFUNC)
#define STR_VFSWFUNC            (STR_OPEN9X + OFS_VFSWFUNC)
#define STR_VFSWRESET           (STR_OPEN9X + OFS_VFSWRESET)
#define STR_FUNCSOUNDS          (STR_OPEN9X + OFS_FUNCSOUNDS)

#define STR_VTELEMCHNS          (STR_OPEN9X + OFS_VTELEMCHNS)


#if defined(FRSKY)
#define STR_VTELEMUNIT          (STR_OPEN9X + OFS_VTELEMUNIT)
#define STR_VTELPROTO           (STR_OPEN9X + OFS_VTELPROTO)
#define STR_GPSFORMAT           (STR_OPEN9X + OFS_GPSFORMAT)
#define STR_AMPSRC              (STR_OPEN9X + OFS_AMPSRC)
#define STR_VARIOSRC            (STR_OPEN9X + OFS_VARIOSRC)
#define STR_VTELEMSCREENTYPE    (STR_OPEN9X + OFS_VSCREEN)
#define STR_TELEMCHNS           (STR_OPEN9X + OFS_TELEMCHNS)
#endif

#if defined(TEMPLATES)
#define STR_VTEMPLATES        (STR_OPEN9X + OFS_VTEMPLATES)
#endif

#if defined(HELI)
#define STR_VSWASHTYPE        (STR_OPEN9X + OFS_VSWASHTYPE)
#endif

#define STR_VKEYS               (STR_OPEN9X + OFS_VKEYS)
#define STR_VSWITCHES           (STR_OPEN9X + OFS_VSWITCHES)
#define STR_VSRCRAW             (STR_OPEN9X + OFS_VSRCRAW)
#define STR_VTMRMODES           (STR_OPEN9X + OFS_VTMRMODES)

#define STR_VRENAVIG          (STR_OPEN9X + OFS_VRENAVIG)
#define STR_VRENCODERS        (STR_OPEN9X + OFS_VRENCODERS)

//#define STR_DATETIME          (STR_OPEN9X + OFS_DATETIME)
#define STR_VPERSISTENT       (STR_OPEN9X + OFS_VPERSISTENT)
#define STR_SWITCHES_VALUES (STR_OPEN9X + OFS_SWITCHES_VALUES)
#define STR_XANY_EXPO       (STR_OPEN9X + OFS_XANY_EXPO)

// The 0-terminated-strings
#define NO_INDENT(x) (x)+LEN_INDENT

extern const pm_char STR_POPUPS[];

#if defined(OFS_EXIT)
#define STR_EXIT (STR_POPUPS + OFS_EXIT)
#else
extern const pm_char STR_EXIT[];
#endif

extern const pm_char STR_MENUWHENDONE[];
extern const pm_char STR_FREE[];
#define LEN_FREE PSIZE(TR_FREE)
extern const pm_char STR_DELETEMODEL[];
extern const pm_char STR_COPYINGMODEL[];
extern const pm_char STR_MOVINGMODEL[];
extern const pm_char STR_LOADINGMODEL[];
extern const pm_char STR_NAME[];
extern const pm_char STR_TIMER[];
extern const pm_char STR_ELIMITS[];
extern const pm_char STR_ETRIMS[];
extern const pm_char STR_TRIMINC[];
extern const pm_char STR_TTRACE[];
extern const pm_char STR_TSWITCH[];
extern const pm_char STR_TTRIM[];
extern const pm_char STR_BEEPCTR[];
extern const pm_char STR_PROTO[];
extern const pm_char STR_PPMFRAME[];
extern const pm_char STR_MS[];
extern const pm_char STR_SWITCH[];
extern const pm_char STR_TRIMS[];
extern const pm_char STR_FADEIN[];
extern const pm_char STR_FADEOUT[];
extern const pm_char STR_DEFAULT[];
extern const pm_char STR_CHECKTRIMS[];
extern const pm_char STR_SWASHTYPE[];
extern const pm_char STR_COLLECTIVE[];
extern const pm_char STR_SWASHRING[];
extern const pm_char STR_ELEDIRECTION[];
extern const pm_char STR_AILDIRECTION[];
extern const pm_char STR_COLDIRECTION[];
extern const pm_char STR_MODE[];
#if defined(AUDIO) && defined(BUZZER)
extern const pm_char STR_SPEAKER[];
extern const pm_char STR_BUZZER[];
#else
#define STR_SPEAKER STR_MODE
#define STR_BUZZER  STR_MODE
#endif
extern const pm_char STR_NOFREEEXPO[];
extern const pm_char STR_NOFREEMIXER[];
extern const pm_char STR_INSERTMIX[];
extern const pm_char STR_EDITMIX[];
extern const pm_char STR_SOURCE[];
extern const pm_char STR_WEIGHT[];
extern const pm_char STR_EXPO[];
extern const pm_char STR_SIDE[];
extern const pm_char STR_DIFFERENTIAL[];
extern const pm_char STR_OFFSET[];
extern const pm_char STR_TRIM[];
extern const pm_char STR_DREX[];
extern const pm_char STR_CURVE[];
extern const pm_char STR_FLMODE[];
extern const pm_char STR_MIXWARNING[];
extern const pm_char STR_OFF[];
extern const pm_char STR_MULTPX[];
extern const pm_char STR_DELAYDOWN[];
extern const pm_char STR_DELAYUP[];
extern const pm_char STR_SLOWDOWN[];
extern const pm_char STR_SLOWUP[];
extern const pm_char STR_MIXER[];
extern const pm_char STR_CV[];
extern const pm_char STR_GV[];
extern const pm_char STR_ACHANNEL[];
extern const pm_char STR_RANGE[];
extern const pm_char STR_ALARM[];
extern const pm_char STR_USRDATA[];
extern const pm_char STR_BLADES[];
extern const pm_char STR_SCREEN[];
extern const pm_char STR_SOUND_LABEL[];
extern const pm_char STR_LENGTH[];
extern const pm_char STR_BEEP_LENGTH[];
#define STR_BEEP_LENGTH STR_LENGTH
extern const pm_char STR_SPKRPITCH[];
extern const pm_char STR_HAPTIC_LABEL[];
extern const pm_char STR_CONTRAST[];
extern const pm_char STR_ALARMS_LABEL[];
#if defined(BATTGRAPH)
extern const pm_char STR_BATTERY_RANGE[];
#endif
extern const pm_char STR_BATTERYWARNING[];
extern const pm_char STR_INACTIVITYALARM[];
extern const pm_char STR_MEMORYWARNING[];
extern const pm_char STR_ALARMWARNING[];
extern const pm_char STR_RENAVIG[];
extern const pm_char STR_MINUTEBEEP[];
extern const pm_char STR_BEEPCOUNTDOWN[];
extern const pm_char STR_PERSISTENT[];
extern const pm_char STR_BACKLIGHT_LABEL[];
extern const pm_char STR_BLDELAY[];
#if defined(PWM_BACKLIGHT)
extern const pm_char STR_BLONBRIGHTNESS[];
extern const pm_char STR_BLOFFBRIGHTNESS[];
#endif
extern const pm_char STR_SPLASHSCREEN[];
extern const pm_char STR_THROTTLEWARNING[];
extern const pm_char STR_SWITCHWARNING[];
extern const pm_char STR_TIMEZONE[];
extern const pm_char STR_ADJUST_RTC[];
extern const pm_char STR_GPS[];
extern const pm_char STR_GPSCOORD[];
extern const pm_char STR_VARIO[];
extern const pm_char STR_RXCHANNELORD[];
extern const pm_char STR_STICKS[];
extern const pm_char STR_POTS[];
extern const pm_char STR_SLAVE[];
extern const pm_char STR_MODESRC[];
extern const pm_char STR_MULTIPLIER[];
#define LEN_MULTIPLIER PSIZE(TR_MULTIPLIER)
extern const pm_char STR_CAL[];
extern const pm_char STR_VTRIM[];
extern const pm_char STR_MENUTOSTART[];
extern const pm_char STR_SETMIDPOINT[];
extern const pm_char STR_MOVESTICKSPOTS[];
extern const pm_char STR_RXBATT[];
extern const pm_char STR_TX[];
#define STR_RX (STR_TX+OFS_RX)
extern const pm_char STR_ACCEL[];
extern const pm_char STR_NODATA[];
extern const pm_char STR_TOTTM1TM2THRTHP[];
extern const pm_char STR_US[];
extern const pm_char STR_MENUTORESET[];
extern const pm_char STR_PPM_TRAINER[];
extern const pm_char STR_CH[];
extern const pm_char STR_MODEL[];
extern const pm_char STR_FP[];
extern const pm_char STR_EEPROMLOWMEM[];
extern const pm_char STR_ALERT[];
extern const pm_char STR_PRESSANYKEYTOSKIP[];
extern const pm_char STR_THROTTLENOTIDLE[];
extern const pm_char STR_ALARMSDISABLED[];
extern const pm_char STR_PRESSANYKEY[];
#define LEN_PRESSANYKEY PSIZE(TR_PRESSANYKEY)
extern const pm_char STR_BADEEPROMDATA[];
extern const pm_char STR_EEPROMFORMATTING[];
extern const pm_char STR_EEPROMOVERFLOW[];
extern const pm_char STR_TRIMS2OFFSETS[];
extern const pm_char STR_MENURADIOSETUP[];
extern const pm_char STR_MENUTRAINER[];
extern const pm_char STR_MENUVERSION[];
extern const pm_char STR_MENUDIAG[];
extern const pm_char STR_MENUANA[];
extern const pm_char STR_MENUCALIBRATION[];
extern const pm_char STR_MENUMODELSEL[];
extern const pm_char STR_MENUSETUP[];
extern const pm_char STR_MENUFLIGHTPHASE[];
extern const pm_char STR_MENUFLIGHTPHASES[];
extern const pm_char STR_MENUHELISETUP[];
extern const pm_char STR_MENUINPUTS[];
extern const pm_char STR_MENULIMITS[];
extern const pm_char STR_MENUCURVES[];
extern const pm_char STR_MENUCURVE[];
extern const pm_char STR_MENULOGICALSWITCHES[];
extern const pm_char STR_MENUCUSTOMFUNC[];
extern const pm_char STR_MENUCUSTOMSCRIPTS[];
extern const pm_char STR_MENUCUSTOMSCRIPT[];
extern const pm_char STR_MENUTELEMETRY[];
extern const pm_char STR_MENUTEMPLATES[];
extern const pm_char STR_MENUSTAT[];
extern const pm_char STR_MENUDEBUG[];
extern const pm_char STR_INVERT_THR[];
extern const pm_char STR_FAS_OFFSET[];

#if defined(SPIMODULES)
extern const pm_char STR_SPIM[];
extern const pm_char STR_RFPOWER[];
extern const pm_char STR_NUMCH[];
extern const pm_char STR_UBIND[];
#endif

#if (SERIAL_PROTOCOL==MULTIMODULE)
extern const pm_char STR_MULTI_CUSTOM[];
#endif

#if (SERIAL_PROTOCOL==MULTIMODULE) || defined(SPIMODULES)
extern const pm_char STR_SUBTYPE[];
extern const pm_char STR_MULTI_VIDFREQ[];
extern const pm_char STR_RFTUNEFINE[];
extern const pm_char STR_TELEMETRY[];
extern const pm_char STR_MULTI_RFPOWER[];
extern const pm_char STR_MULTI_SERVOFREQ[];
extern const pm_char STR_MULTI_OPTION[];
extern const pm_char STR_MULTI_DSM_AUTODTECT[];
extern const pm_char STR_AUTOBIND[];
extern const pm_char STR_MULTI_LOWPOWER[];
#endif

#if defined(FRSKY)
extern const pm_char STR_LIMIT[];
extern const pm_char STR_MINRSSI[];
extern const pm_char STR_LATITUDE[];
extern const pm_char STR_LONGITUDE[];
#endif

extern const pm_char STR_SHUTDOWN[];
extern const pm_char STR_SAVEMODEL[];


extern const pm_char STR_BATT_CALIB[];

#if defined(FRSKY)
extern const pm_char STR_VOLTAGE[];
extern const pm_char STR_CURRENT[];
extern const pm_char STR_MODELISON[];
#endif

#define LEN_CALIB_FIELDS PSIZE(TR_BATT_CALIB)

extern const pm_char STR_SELECT_MODEL[];
extern const pm_char STR_CREATE_MODEL[];
extern const pm_char STR_COPY_MODEL[];
extern const pm_char STR_MOVE_MODEL[];
extern const pm_char STR_DELETE_MODEL[];
extern const pm_char STR_EDIT[];
extern const pm_char STR_INSERT_BEFORE[];
extern const pm_char STR_INSERT_AFTER[];
extern const pm_char STR_COPY[];
extern const pm_char STR_MOVE[];
extern const pm_char STR_PASTE[];
extern const pm_char STR_INSERT[];
extern const pm_char STR_DELETE[];
extern const pm_char STR_RESET_FLIGHT[];
extern const pm_char STR_RESET_TIMER1[];
extern const pm_char STR_RESET_TIMER2[];
extern const pm_char STR_RESET_TELEMETRY[];
extern const pm_char STR_STATISTICS[];
extern const pm_char STR_SAVE_TIMERS[];

extern const pm_char STR_RESET_BTN[];

#if defined(SDCARD)
extern const pm_char STR_BACKUP_MODEL[];
extern const pm_char STR_RESTORE_MODEL[];
extern const pm_char STR_SDCARD_ERROR[];
extern const pm_char STR_NO_SDCARD[];
extern const pm_char STR_INCOMPATIBLE[];
extern const pm_char STR_LOGS_PATH[];
extern const pm_char STR_LOGS_EXT[];
extern const pm_char STR_MODELS_PATH[];
extern const pm_char STR_MODELS_EXT[];
#define STR_UPDATE_LIST STR_DELAYDOWN
#endif

extern const pm_char STR_WARNING[];
extern const pm_char STR_EEPROMWARN[];
extern const pm_char STR_THROTTLEWARN[];
extern const pm_char STR_ALARMSWARN[];
extern const pm_char STR_SWITCHWARN[];

extern const pm_char STR_SPEAKER_VOLUME[];
extern const pm_char STR_FUNC[];
extern const pm_char STR_DELAY[];
extern const pm_char STR_SD_CARD[];
extern const pm_char STR_NO_MODELS_ON_SD[];
extern const pm_char STR_PLAY_FILE[];
extern const pm_char STR_DELETE_FILE[];
extern const pm_char STR_COPY_FILE[];
extern const pm_char STR_RENAME_FILE[];
extern const pm_char STR_REMOVED[];
extern const pm_char STR_SD_INFO[];
extern const pm_char STR_NA[];
extern const pm_char STR_TIME[];
extern const pm_char STR_SD_INFO_TITLE[];
extern const pm_char STR_SD_TYPE[];
extern const pm_char STR_YEAR[];
extern const pm_char STR_SD_SIZE[];
extern const pm_char STR_TYPE[];
extern const pm_char STR_GLOBAL_VARS[];
extern const pm_char STR_GLOBAL_V[];
extern const pm_char STR_GLOBAL_VAR[];
extern const pm_char STR_OWN[];
extern const pm_char STR_ROTARY_ENCODER[];
extern const pm_char STR_DATE[];
extern const pm_char STR_DUMMY[];
extern const pm_char STR_POWEROFF[];
extern const pm_char STR_RFID[];
extern const pm_char STR_BIND[];


#if defined(VOICE)
void playNumber(getvalue_t number, uint8_t unit, uint8_t att);
void playDuration(int16_t seconds);
#define LANGUAGE_PACK_DECLARE(lng, name)
#define LANGUAGE_PACK_DECLARE_DEFAULT(lng, name)
#else
#define LANGUAGE_PACK_DECLARE(lng, name)
#define LANGUAGE_PACK_DECLARE_DEFAULT(lng, name)
#endif

#if MENUS_LOCK == 1
extern const pm_char STR_UNLOCKED[];
extern const pm_char STR_MODS_FORBIDDEN[];
#endif

#if defined(SPIMODULES)
extern const pm_char STR_mW[];
#endif

#if (SERIAL_PROTOCOL==DSM) || defined(SPIMODULES) || defined(MULTIMODULE)
extern const pm_char STR_MODULE_RANGE[];
extern const pm_char STR_MODULE_BIND[];
extern const pm_char STR_RXNUM[];
#endif

#if (SERIAL_PROTOCOL==DSM)
extern const pm_char STR_DSM_PROTOCOLS[];
#endif
#if ((SERIAL_PROTOCOL==SUMD) || (SERIAL_PROTOCOL==SBUS) || (SERIAL_PROTOCOL==CRSF))
extern const pm_char STR_PERIOD[];
#endif

#define CHR_SHORT  TR_CHR_SHORT
#define CHR_LONG   TR_CHR_LONG
#define CHR_TOGGLE TR_CHR_TOGGLE
#define CHR_HOUR   TR_CHR_HOUR
#define CHR_INPUT  TR_CHR_INPUT

#if defined(X_ANY)
extern const pm_char STR_X_ANY[];
extern const pm_char STR_ACTIVED[];
extern const pm_char STR_NUMBER[];
extern const pm_char STR_CHANNEL[];
extern const pm_char STR_NB_REPEAT[];
extern const pm_char STR_ANGLE_SENSOR[];
extern const pm_char STR_PROP[];
#endif

// New Debug Menu
extern const pm_char STR_COMPUTE[];
extern const pm_char STR_MIN[];
extern const pm_char STR_MAX[];
extern const pm_char STR_PROTOCOL[];
extern const pm_char STR_GUIBUILD[];
extern const pm_char STR_LCDDRAW[];
extern const pm_char STR_MIXERlowcase[];
extern const pm_char STR_FREESRAM[];

#if defined(RTCLOCK)
extern const pm_char STR_TXTEMP[];
#endif

#if defined(BLUETOOTH)
extern const pm_char STR_BLUETOOTH[];
extern const pm_char STR_NOBLUETOOTH[];
extern const pm_char STR_CONNECTED[];
extern const pm_char STR_BT_ROLE[];
extern const pm_char STR_BT_PIN[];
extern const pm_char STR_BT_M_S[];
extern const pm_char STR_BT_PAIR[];
extern const pm_char STR_AUTOCON[];
extern const pm_char STR_RESCANN[];
#endif

#endif
