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


#ifndef myeeprom_h
#define myeeprom_h

#include <inttypes.h>

#define WARN_THR_BIT  0x01
#define WARN_BEP_BIT  0x80
#define WARN_SW_BIT   0x02
#define WARN_MEM_BIT  0x04
#define WARN_BVAL_BIT 0x38

#define WARN_THR     (!(g_eeGeneral.warnOpts & WARN_THR_BIT))
#define WARN_BEP     (!(g_eeGeneral.warnOpts & WARN_BEP_BIT))
#define WARN_SW      (!(g_eeGeneral.warnOpts & WARN_SW_BIT))
#define WARN_MEM     (!(g_eeGeneral.warnOpts & WARN_MEM_BIT))
#define BEEP_VAL     ( (g_eeGeneral.warnOpts & WARN_BVAL_BIT) >>3 )

#if   defined(CPUM2560)
#define EEPROM_VER             217
#define FIRST_CONV_EEPROM_VER  EEPROM_VER
#endif

#ifndef PACK
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif
#define NOBACKUP(...)                __VA_ARGS__

#define NUM_STICKS           4

#if   defined(CPUM2560)
#if defined(EXTERNALEEPROM)
#define MAX_MODELS           60
#define NUM_CHNOUT           16 // number of real output channels CH1-CH16
#define MAX_FLIGHT_MODES     6
#define MAX_MIXERS           32
#define MAX_EXPOS            16
#define NUM_LOGICAL_SWITCH   12 // number of custom switches
#define NUM_CFN              24 // number of functions assigned to switches
#define NUM_TRAINER          8
#define NUM_POTS             3
#define NUM_XPOTS            0
#define MAX_SENSORS          0
#else
#define MAX_MODELS           30
#define NUM_CHNOUT           16 // number of real output channels CH1-CH16
#define MAX_FLIGHT_MODES     6
#define MAX_MIXERS           32
#define MAX_EXPOS            16
#define NUM_LOGICAL_SWITCH   12 // number of custom switches
#define NUM_CFN              24 // number of functions assigned to switches
#define NUM_TRAINER          8
#define NUM_POTS             3
#define NUM_XPOTS            0
#define MAX_SENSORS          0
#endif
#else
#endif

#define MAX_TIMERS           2

#define NUM_CYC              3
#define NUM_CAL_PPM          4


#define LEN_MODEL_NAME       10
#define LEN_FLIGHT_MODE_NAME 6
#define MAX_CURVES           8
#define NUM_POINTS           (112-MAX_CURVES)
#define CURVDATA             int8_t

PACK(typedef struct {
  int8_t * crv;
  uint8_t points;
  bool custom;
}) CurveInfo;

extern CurveInfo curveInfo(uint8_t idx);

typedef int16_t gvar_t;

typedef uint8_t source_t;

#define LEN_GVAR_NAME 6
#define GVAR_MAX      128
#define GVAR_LIMIT    125

PACK(typedef struct {
  char    name[LEN_GVAR_NAME];
  uint8_t popup:1;
  uint8_t spare:7;
}) global_gvar_t;

#define RESERVE_RANGE_FOR_GVARS 10
// even we do not spend space in EEPROM for 10 GVARS, we reserve the space inside the range of values, like offset, weight, etc.

#define MAX_GVARS 5
#define MODEL_GVARS_DATA global_gvar_t gvars[MAX_GVARS];
#define PHASE_GVARS_DATA gvar_t gvars[MAX_GVARS]
#define GVAR_VALUE(x, p) g_model.flightModeData[p].gvars[x]

PACK(typedef struct {
  uint8_t srcChn:6; // 0-7 = ch1-8
  uint8_t mode:2;   // off,add-mode,subst-mode
  int8_t  studWeight;
}) TrainerMix;

PACK(typedef struct {
  int16_t        calib[4];
  TrainerMix     mix[4];
}) TrainerData;

PACK(typedef struct {
  int8_t    level:2;
  int8_t    value:6;
}) FrSkyRSSIAlarm;

enum MainViews {
  VIEW_OUTPUTS_VALUES,
  VIEW_OUTPUTS_BARS,
  VIEW_INPUTS,
#if defined(PCBMEGA2560) && defined(GVARS)
  VIEW_GVARS,
#endif
  VIEW_TIMER2,
  VIEW_COUNT
};

enum BeeperMode {
  e_mode_quiet = -2,
  e_mode_alarms,
  e_mode_nokeys,
  e_mode_all
};


#define swarnstate_t        uint8_t
#define swarnenable_t       uint8_t

#if   defined(PXX)
#define EXTRA_GENERAL_FIELDS uint8_t  countryCode;
#else
#define EXTRA_GENERAL_FIELDS
#endif

enum BacklightMode {
  e_backlight_mode_off  = 0,
  e_backlight_mode_keys = 1,
  e_backlight_mode_sticks = 2,
  e_backlight_mode_all = e_backlight_mode_keys+e_backlight_mode_sticks,
  e_backlight_mode_on
};

#if defined(FSPLASH)
#define SPLASH_MODE uint8_t splashMode:3
#else
#define SPLASH_MODE uint8_t splashMode:1; uint8_t splashSpare:2
#endif

PACK(typedef struct {
  int16_t mid;
  int16_t spanNeg;
  int16_t spanPos;
}) CalibData;


enum Functions {
  // first the functions which need a checkbox
  FUNC_OVERRIDE_CHANNEL,
  FUNC_TRAINER,
  FUNC_INSTANT_TRIM,
  FUNC_RESET,
  FUNC_ADJUST_GVAR,
  // then the other functions
  FUNC_FIRST_WITHOUT_ENABLE,
  FUNC_PLAY_SOUND = FUNC_FIRST_WITHOUT_ENABLE,
  FUNC_PLAY_TRACK,
  FUNC_PLAY_BOTH,
  FUNC_PLAY_VALUE,
  FUNC_VARIO,
  FUNC_HAPTIC,
  FUNC_LOGS,
  FUNC_BACKLIGHT,
#if defined(DEBUG)
  FUNC_TEST, // should remain the last before MAX as not added in companion9x
#endif
  FUNC_MAX
};

#if defined(OVERRIDE_CHANNEL_FUNCTION)
#define HAS_ENABLE_PARAM(func)    ((func) < FUNC_FIRST_WITHOUT_ENABLE)
#else
#define HAS_ENABLE_PARAM(func)    ((func) < FUNC_FIRST_WITHOUT_ENABLE && (func) != FUNC_OVERRIDE_CHANNEL)
#endif

#if defined(VOICE)
#define IS_PLAY_FUNC(func)      ((func) >= FUNC_PLAY_SOUND && func <= FUNC_PLAY_VALUE)
#else
#define IS_PLAY_FUNC(func)      ((func) == FUNC_PLAY_SOUND)
#endif

#define IS_PLAY_BOTH_FUNC(func) ((func) == FUNC_PLAY_BOTH)
#define IS_VOLUME_FUNC(func)    (0)

#if defined(GVARS)
#define IS_ADJUST_GV_FUNC(func) ((func) == FUNC_ADJUST_GVAR)
#else
#define IS_ADJUST_GV_FUNC(func) (0)
#endif

#if defined(HAPTIC)
#define IS_HAPTIC_FUNC(func)    ((func) == FUNC_HAPTIC)
#else
#define IS_HAPTIC_FUNC(func)    (0)
#endif

#define HAS_REPEAT_PARAM(func)    (IS_PLAY_FUNC(func) || IS_HAPTIC_FUNC(func))

enum ResetFunctionParam {
  FUNC_RESET_TIMER1,
  FUNC_RESET_TIMER2,
  FUNC_RESET_FLIGHT,
#if defined(FRSKY)
  FUNC_RESET_TELEMETRY,
#endif
#if ROTARY_ENCODERS > 0
  FUNC_RESET_ROTENC1,
#endif
#if ROTARY_ENCODERS > 1
  FUNC_RESET_ROTENC2,
#endif
  FUNC_RESET_PARAMS_COUNT,
  FUNC_RESET_PARAM_LAST = FUNC_RESET_PARAMS_COUNT-1,
};

enum AdjustGvarFunctionParam {
  FUNC_ADJUST_GVAR_CONSTANT,
  FUNC_ADJUST_GVAR_SOURCE,
  FUNC_ADJUST_GVAR_GVAR,
  FUNC_ADJUST_GVAR_INC,
};

#if defined(CPUM2560)
PACK(typedef struct {
  int8_t  swtch;
  uint8_t func;
  uint8_t mode:2;
  uint8_t param:4;
  uint8_t active:1;
  uint8_t spare:1;
  uint8_t value;
}) CustomFunctionData;
#define CFN_SWITCH(p)       ((p)->swtch)
#define CFN_FUNC(p)         ((p)->func)
#define CFN_ACTIVE(p)       ((p)->active)
#define CFN_CH_INDEX(p)     ((p)->param)
#define CFN_TIMER_INDEX(p)  ((p)->param)
#define CFN_GVAR_INDEX(p)   ((p)->param)
#define CFN_PLAY_REPEAT(p)  ((p)->param)
#define CFN_PLAY_REPEAT_MUL 10
#define CFN_GVAR_MODE(p)    ((p)->mode)
#define CFN_PARAM(p)        ((p)->value)
#define CFN_RESET(p)        ((p)->active = 0, CFN_PARAM(p) = 0)
#define CFN_GVAR_CST_MAX    125
#endif


#define ALTERNATE_VIEW 0x10
PACK(typedef struct {
  uint8_t   version;
  uint16_t  variant;
  CalibData calib[NUM_STICKS+NUM_POTS];
  uint16_t  chkSum;
  int8_t    currModel;
  uint8_t   contrast;
  uint8_t   vBatWarn;
  int8_t    txVoltageCalibration;
  int8_t    backlightMode;
  TrainerData trainer;
  uint8_t   view;            // index of view in main screen
  int8_t    buzzerMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t   fai:1;
  int8_t    beepMode:2;      // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t   alarmsFlash:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode:2;
  int8_t    timezone:5;
  uint8_t   adjustRTC:1;
  uint8_t   inactivityTimer;
  uint8_t   mavbaud:3;
  SPLASH_MODE; /* 3bits */
  int8_t    hapticMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t blOffBright:4;
  uint8_t blOnBright:4;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;   // RETA order for receiver channels
  int8_t    PPM_Multiplier;
  int8_t    hapticLength;
  uint8_t   reNavigation;
  uint8_t   stickReverse;
  int8_t    beepLength:3;
  int8_t    hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   unexpectedShutdown:1;
  uint8_t   speakerPitch;
  int8_t    speakerVolume;
  int8_t    vBatMin;
  int8_t    vBatMax;

  EXTRA_GENERAL_FIELDS

}) EEGeneral;

#define SWITCHES_DELAY()            uint8_t(15+g_eeGeneral.switchesDelay)
#define SWITCHES_DELAY_NONE         (-15)
#define HAPTIC_STRENGTH()           (3+g_eeGeneral.hapticStrength)

enum CurveRefType {
  CURVE_REF_DIFF,
  CURVE_REF_EXPO,
  CURVE_REF_FUNC,
  CURVE_REF_CUSTOM
};

PACK(typedef struct {
  uint8_t type;
  int8_t  value;
}) CurveRef;

#define MODE_DIFFERENTIAL  0
#define MODE_EXPO          0
#define MODE_CURVE         1

#if defined(CPUM2560)
PACK(typedef struct {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn:2;
  uint8_t curveMode:1;
  uint8_t spare:3;
  uint8_t flightModes;
  int8_t  swtch;
  uint8_t weight;
  int8_t  curveParam;
}) ExpoData;
#define MIN_EXPO_WEIGHT         0
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))
#endif

#define limit_min_max_t     int8_t
#define LIMIT_EXT_PERCENT   125
#define LIMIT_EXT_MAX       LIMIT_EXT_PERCENT
#define PPM_CENTER_MAX      125
#define LIMIT_MAX(lim)      (lim->max+100)
#define LIMIT_MIN(lim)      (lim->min-100)
#define LIMIT_OFS(lim)      (lim->offset)
#define LIMIT_MAX_RESX(lim) calc100toRESX(LIMIT_MAX(lim))
#define LIMIT_MIN_RESX(lim) calc100toRESX(LIMIT_MIN(lim))
#define LIMIT_OFS_RESX(lim) calc1000toRESX(LIMIT_OFS(lim))

PACK(typedef struct {
  int8_t min;
  int8_t max;
  int8_t  ppmCenter;
  int16_t offset:14;
  uint16_t symetrical:1;
  uint16_t revert:1;
}) LimitData;

#define TRIM_OFF    (1)
#define TRIM_ON     (0)
#define TRIM_RUD    (-1)
#define TRIM_ELE    (-2)
#define TRIM_THR    (-3)
#define TRIM_AIL    (-4)

#define MLTPX_ADD   0
#define MLTPX_MUL   1
#define MLTPX_REP   2


// highest bit used for small values in mix 128 --> 8 bit is enough
#define GV1_SMALL  128
// highest bit used for large values in mix 256 --> 9 bits is used (8 bits + 1 extra bit from weightMode/offsetMode)
#define GV1_LARGE  256 // bracame toto remove all to switch gvar to int8

#define DELAY_STEP  2
#define SLOW_STEP   2
#define DELAY_MAX   15 /* 7.5 seconds */
#define SLOW_MAX    15 /* 7.5 seconds */

#if defined(CPUM2560)
PACK(typedef struct {
  uint8_t destCh:4;          // 0, 1..NUM_CHNOUT
  uint8_t curveMode:1;       // O=curve, 1=differential
  uint8_t noExpo:1;
  uint8_t weightMode:1;
  uint8_t offsetMode:1;
  uint8_t srcRaw;
  int8_t  weight;
  int8_t  swtch;
  uint8_t flightModes;
  uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  int8_t  carryTrim:3;
  uint8_t mixWarn:2;         // mixer warning
  uint8_t spare:1;
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;
  uint8_t speedDown:4;
  int8_t  curveParam;
  int8_t  offset;
}) MixData;
#endif

PACK(union u_gvarint_t {
  struct {
    int8_t lo;
    uint8_t hi;
  } bytes_t;

  int16_t gvword;

  u_gvarint_t(int8_t l, uint8_t h)
  {
    bytes_t.lo=l;  // hi bit is negativ sign
    bytes_t.hi=h?255:0;
  }

private:
  // prevent unwanted constructors, also saves program
  u_gvarint_t() {}
  u_gvarint_t(const u_gvarint_t&) {}
});

#define MD_WEIGHT(md) (u_gvarint_t(md->weight,md->weightMode).gvword)

PACK(union u_int8int16_t {
  struct {
    int8_t  lo;
    uint8_t hi;
  } bytes_t;
  int16_t gvword;
});

#define MD_WEIGHT_TO_UNION(md, var) var.bytes_t.lo=md->weight; var.bytes_t.hi=md->weightMode?255:0
#define MD_UNION_TO_WEIGHT(var, md) md->weight=var.bytes_t.lo; if (var.gvword<0) md->weightMode=1; else md->weightMode=0
// #define MD_SETWEIGHT(md, val) md->weight=val; if (val<0) md->weightMode=1; else md->weightMode=0

#define MD_OFFSET(md) (u_gvarint_t(md->offset,md->offsetMode).gvword)
#define MD_OFFSET_TO_UNION(md, var) var.bytes_t.lo=md->offset; var.bytes_t.hi=md->offsetMode?255:0
#define MD_UNION_TO_OFFSET(var, md) md->offset=var.bytes_t.lo; if (var.gvword<0) md->offsetMode=1; else md->offsetMode=0 /* set negative sign */
// #define MD_SETOFFSET(md, val) md->offset=val; if (val<0) md->offsetMode=1; else md->offsetMode=0


enum LogicalSwitchesFunctions {
  LS_FUNC_NONE,
  LS_FUNC_VALMOSTEQUAL, // v~=offset
  LS_FUNC_VPOS,   // v>offset
  LS_FUNC_VNEG,   // v<offset
  LS_FUNC_APOS,   // |v|>offset
  LS_FUNC_ANEG,   // |v|<offset
  LS_FUNC_AND,
  LS_FUNC_OR,
  LS_FUNC_XOR,
  LS_FUNC_EQUAL,
  LS_FUNC_GREATER,
  LS_FUNC_LESS,
  LS_FUNC_DIFFEGREATER,
  LS_FUNC_ADIFFEGREATER,
  LS_FUNC_TIMER,
  LS_FUNC_STICKY,
  LS_FUNC_COUNT,
  LS_FUNC_MAX = LS_FUNC_COUNT-1
};

typedef uint8_t ls_telemetry_value_t;
#define MAX_LS_ANDSW    15
PACK(typedef struct { // Logical Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func:4;
  uint8_t andsw:4;
}) LogicalSwitchData;

enum TelemetryUnit {
  UNIT_VOLTS,
  UNIT_AMPS,
  UNIT_METERS_PER_SECOND,
  UNIT_RAW,
  UNIT_SPEED,
  UNIT_DIST,
  UNIT_TEMPERATURE,
  UNIT_PERCENT,
  UNIT_MILLIAMPS,
  UNIT_A1A2_MAX = UNIT_MILLIAMPS,
  UNIT_MAH,
  UNIT_WATTS,
  UNIT_DB,
  UNIT_FEET,
  UNIT_KTS,
  UNIT_HOURS,
  UNIT_MINUTES,
  UNIT_SECONDS,
  UNIT_RPMS,
  UNIT_G,
  UNIT_HDG,
};

PACK(typedef struct {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  int16_t   offset:12;
  uint16_t  type:4;             // channel unit (0=volts, ...)
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   multiplier:2;       // 0=no multiplier, 1=*2 multiplier
}) FrSkyChannelData;


enum TelemetrySource {
  TELEM_NONE,
  TELEM_TX_VOLTAGE,
  TELEM_TIMER1,
  TELEM_TIMER2,
  TELEM_TIMER_MAX=TELEM_TIMER2,
  TELEM_RSSI_TX,
  TELEM_RSSI_RX,
  TELEM_A_FIRST,
  TELEM_A1=TELEM_A_FIRST,
  TELEM_A2,
  TELEM_A_LAST=TELEM_A2,
  TELEM_ALT,
  TELEM_RPM,
  TELEM_FUEL,
  TELEM_T1,
  TELEM_T2,
  TELEM_SPEED,
  TELEM_DIST,
  TELEM_GPSALT,
  TELEM_CELL,
  TELEM_CELLS_SUM,
  TELEM_VFAS,
  TELEM_CURRENT,
  TELEM_CONSUMPTION,
  TELEM_POWER,
  TELEM_ACCx,
  TELEM_ACCy,
  TELEM_ACCz,
  TELEM_HDG,
  TELEM_VSPEED,
  TELEM_ASPEED,
  TELEM_DTE,
  TELEM_MIN_A_FIRST,
  TELEM_MIN_A1=TELEM_MIN_A_FIRST,
  TELEM_MIN_A2,
  TELEM_MIN_A_LAST=TELEM_MIN_A2,
  TELEM_MIN_ALT,
  TELEM_MAX_ALT,
  TELEM_MAX_RPM,
  TELEM_MAX_T1,
  TELEM_MAX_T2,
  TELEM_MAX_SPEED,
  TELEM_MAX_DIST,
  TELEM_MAX_ASPEED,
  TELEM_MIN_CELL,
  TELEM_MIN_CELLS_SUM,
  TELEM_MIN_VFAS,
  TELEM_MAX_CURRENT,
  TELEM_MAX_POWER,
  TELEM_ACC,
  TELEM_GPS_TIME,
  TELEM_CSW_MAX = TELEM_MAX_POWER,
  TELEM_NOUSR_MAX = TELEM_A2,
#if defined(FRSKY)
  TELEM_DISPLAY_MAX = TELEM_MAX_POWER,
#else
  TELEM_DISPLAY_MAX = TELEM_TIMER2, // because used also in PlayValue
#endif
  TELEM_STATUS_MAX = TELEM_GPS_TIME,
  TELEM_FIRST_STREAMED_VALUE = TELEM_RSSI_TX,
};

enum VarioSource {
#if !defined(FRSKY_SPORT)
  VARIO_SOURCE_ALTI,
  VARIO_SOURCE_ALTI_PLUS,
#endif
  VARIO_SOURCE_VSPEED,
  VARIO_SOURCE_A1,
  VARIO_SOURCE_A2,
#if defined(FRSKY_SPORT)
  VARIO_SOURCE_DTE,
#endif
  VARIO_SOURCE_COUNT,
  VARIO_SOURCE_LAST = VARIO_SOURCE_COUNT-1
};

#if defined(FRSKY_HUB)
#define NUM_TELEMETRY      TELEM_CSW_MAX
#elif defined(WS_HOW_HIGH)
#define NUM_TELEMETRY      TELEM_ALT
#elif defined(FRSKY)
#define NUM_TELEMETRY      TELEM_A2
#elif defined(MAVLINK)
#define NUM_TELEMETRY      4
#else
#define NUM_TELEMETRY      TELEM_TIMER2
#endif

PACK(typedef struct {
  source_t source;
  ls_telemetry_value_t barMin;           // minimum for bar display
  ls_telemetry_value_t barMax;           // ditto for max display (would usually = ratio)
}) FrSkyBarData;

#define NUM_LINE_ITEMS 2

PACK(typedef struct {
  source_t sources[NUM_LINE_ITEMS];
}) FrSkyLineData;


typedef union {
  FrSkyBarData  bars[4];
  FrSkyLineData lines[4];
} FrSkyScreenData;

enum FrskyUsrProtocols {
  USR_PROTO_NONE,
  USR_PROTO_FRSKY,
  USR_PROTO_WS_HOW_HIGH,
  USR_PROTO_LAST = USR_PROTO_WS_HOW_HIGH,
};

enum FrskyCurrentSource {
  FRSKY_CURRENT_SOURCE_NONE,
  FRSKY_CURRENT_SOURCE_A1,
  FRSKY_CURRENT_SOURCE_A2,
  FRSKY_CURRENT_SOURCE_FAS,
  FRSKY_CURRENT_SOURCE_LAST=FRSKY_CURRENT_SOURCE_FAS
};

enum FrskyVoltsSource {
  FRSKY_VOLTS_SOURCE_A1,
  FRSKY_VOLTS_SOURCE_A2,
  FRSKY_VOLTS_SOURCE_FAS,
  FRSKY_VOLTS_SOURCE_CELLS,
  FRSKY_VOLTS_SOURCE_LAST=FRSKY_VOLTS_SOURCE_CELLS
};

#define MAX_FRSKY_A_CHANNELS 2
#define MAX_TELEMETRY_SCREENS 2
#define IS_BARS_SCREEN(screenIndex) (g_model.frsky.screensType & (1<<(screenIndex)))
PACK(typedef struct {
  FrSkyChannelData channels[MAX_FRSKY_A_CHANNELS];
  uint8_t usrProto:2; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh, 3=Halcyon
  uint8_t blades:2;   // How many blades for RPMs, 0=2 blades
  uint8_t screensType:2;
  uint8_t voltsSource:2;
  int8_t  varioMin:4;
  int8_t  varioMax:4;
  FrSkyRSSIAlarm rssiAlarms[2];
  FrSkyScreenData screens[MAX_TELEMETRY_SCREENS];
  uint8_t varioSource:3;
  int8_t  varioCenterMin:5;
  uint8_t currentSource:3;
  int8_t  varioCenterMax:5;
  int8_t  fasOffset;
}) FrSkyData;
#define MIN_BLADES 0 // 2 blades
#define MAX_BLADES 3 // 5 blades

#if defined(MAVLINK)
PACK(typedef struct t_MavlinkData {
  uint8_t rc_rssi_scale:4;
  uint8_t pc_rssi_en:1;
  uint8_t spare1:3;
  uint8_t spare2[3];
}) MavlinkData;
#endif

enum SwashType {
  SWASH_TYPE_NONE,
  SWASH_TYPE_120,
  SWASH_TYPE_120X,
  SWASH_TYPE_140,
  SWASH_TYPE_90,
  SWASH_TYPE_MAX = SWASH_TYPE_90
};

PACK(typedef struct {
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;
}) SwashRingData;

#define TRIM_EXTENDED_MAX 500
#define TRIM_EXTENDED_MIN (-TRIM_EXTENDED_MAX)
#define TRIM_MAX 125
#define TRIM_MIN (-TRIM_MAX)

#define ROTARY_ENCODER_MAX  1000 // =125 was 1024

#if defined(CPUM2560)
#define NUM_ROTARY_ENCODERS 2
#define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[2];
#endif
#define trim_t          int16_t
#define TRIMS_ARRAY       trim_t trim[4]
#define TRIMS_ARRAY_SIZE  8

PACK(typedef struct {
  TRIMS_ARRAY;
  int8_t swtch;       // swtch of phase[0] is not used
  char name[LEN_FLIGHT_MODE_NAME];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
  ROTARY_ENCODER_ARRAY;
  PHASE_GVARS_DATA;
}) FlightModeData;

enum SwitchSources {
  SWSRC_NONE = 0,

  SWSRC_FIRST_SWITCH,

  SWSRC_ID0 = SWSRC_FIRST_SWITCH,
  SWSRC_ID1,
  SWSRC_ID2,
  SWSRC_THR,
  SWSRC_RUD,
  SWSRC_ELE,
  SWSRC_AIL,
  SWSRC_GEA,
  SWSRC_TRN,
  SWSRC_TRAINER = SWSRC_TRN,
  SWSRC_LAST_SWITCH = SWSRC_TRN,

#if NUM_XPOTS > 0
  SWSRC_FIRST_MULTIPOS_SWITCH,
  SWSRC_LAST_MULTIPOS_SWITCH = SWSRC_FIRST_MULTIPOS_SWITCH + (NUM_XPOTS*XPOTS_MULTIPOS_COUNT) - 1,
#endif

  SWSRC_FIRST_TRIM,
  SWSRC_TrimRudLeft = SWSRC_FIRST_TRIM,
  SWSRC_TrimRudRight,
  SWSRC_TrimEleDown,
  SWSRC_TrimEleUp,
  SWSRC_TrimThrDown,
  SWSRC_TrimThrUp,
  SWSRC_TrimAilLeft,
  SWSRC_TrimAilRight,
  SWSRC_LAST_TRIM = SWSRC_TrimAilRight,

#if defined(CPUM2560)
  SWSRC_REa,
  SWSRC_REb,
#endif

  SWSRC_FIRST_LOGICAL_SWITCH,
  SWSRC_SW1 = SWSRC_FIRST_LOGICAL_SWITCH,
  SWSRC_SW2,
  SWSRC_SW3,
  SWSRC_SW4,
  SWSRC_SW5,
  SWSRC_SW6,
  SWSRC_SW7,
  SWSRC_SW8,
  SWSRC_SW9,
  SWSRC_SWA,
  SWSRC_SWB,
  SWSRC_SWC,
  SWSRC_LAST_LOGICAL_SWITCH = SWSRC_FIRST_LOGICAL_SWITCH+NUM_LOGICAL_SWITCH-1,

  SWSRC_ON,
  SWSRC_ONE,


  SWSRC_COUNT,

  SWSRC_OFF = -SWSRC_ON,

  SWSRC_LAST = SWSRC_COUNT-1,
  SWSRC_FIRST = -SWSRC_LAST,

  SWSRC_LAST_IN_LOGICAL_SWITCHES = SWSRC_LAST_LOGICAL_SWITCH,
  SWSRC_LAST_IN_MIXES = SWSRC_LAST_LOGICAL_SWITCH,

  SWSRC_FIRST_IN_LOGICAL_SWITCHES = -SWSRC_LAST_IN_LOGICAL_SWITCHES,
  SWSRC_FIRST_IN_MIXES = -SWSRC_LAST_IN_MIXES,

};

enum MixSources {
  MIXSRC_NONE,
  MIXSRC_FIRST_STICK,
  MIXSRC_Rud = MIXSRC_FIRST_STICK,
  MIXSRC_Ele,
  MIXSRC_Thr,
  MIXSRC_Ail,

  MIXSRC_FIRST_POT,
  MIXSRC_P1 = MIXSRC_FIRST_POT,
  MIXSRC_P2,
  MIXSRC_P3,
  MIXSRC_LAST_POT = MIXSRC_P3,

#if   defined(CPUM2560)
  MIXSRC_REa,
  MIXSRC_REb,
#if ROTARY_ENCODERS > 2
  MIXSRC_REc,
  MIXSRC_REd,
  MIXSRC_LAST_ROTARY_ENCODER = MIXSRC_REd,
#else
  MIXSRC_LAST_ROTARY_ENCODER = MIXSRC_REb,
#endif
#endif

  MIXSRC_MAX,

  MIXSRC_FIRST_HELI,
  MIXSRC_CYC1 = MIXSRC_FIRST_HELI,
  MIXSRC_CYC2,
  MIXSRC_CYC3,

  MIXSRC_FIRST_TRIM,
  MIXSRC_TrimRud = MIXSRC_FIRST_TRIM,
  MIXSRC_TrimEle,
  MIXSRC_TrimThr,
  MIXSRC_TrimAil,
  MIXSRC_LAST_TRIM = MIXSRC_TrimAil,

  MIXSRC_FIRST_SWITCH,

  MIXSRC_3POS = MIXSRC_FIRST_SWITCH,
  MIXSRC_THR,
  MIXSRC_RUD,
  MIXSRC_ELE,
  MIXSRC_AIL,
  MIXSRC_GEA,
  MIXSRC_TRN,
  MIXSRC_LAST_SWITCH = MIXSRC_TRN,
  MIXSRC_FIRST_LOGICAL_SWITCH,
  MIXSRC_SW1 = MIXSRC_FIRST_LOGICAL_SWITCH,
  MIXSRC_SW9 = MIXSRC_SW1 + 8,
  MIXSRC_SWA,
  MIXSRC_SWB,
  MIXSRC_SWC,
  MIXSRC_LAST_LOGICAL_SWITCH = MIXSRC_FIRST_LOGICAL_SWITCH+NUM_LOGICAL_SWITCH-1,

  MIXSRC_FIRST_TRAINER,
  MIXSRC_LAST_TRAINER = MIXSRC_FIRST_TRAINER+NUM_TRAINER-1,

  MIXSRC_FIRST_CH,
  MIXSRC_CH1 = MIXSRC_FIRST_CH,
  MIXSRC_CH2,
  MIXSRC_CH3,
  MIXSRC_CH4,
  MIXSRC_CH5,
  MIXSRC_CH6,
  MIXSRC_CH7,
  MIXSRC_CH8,
  MIXSRC_CH9,
  MIXSRC_CH10,
  MIXSRC_CH11,
  MIXSRC_CH12,
  MIXSRC_CH13,
  MIXSRC_CH14,
  MIXSRC_CH15,
  MIXSRC_CH16,
  MIXSRC_LAST_CH = MIXSRC_CH1+NUM_CHNOUT-1,

  MIXSRC_FIRST_GVAR,
  MIXSRC_GVAR1 = MIXSRC_FIRST_GVAR,
  MIXSRC_LAST_GVAR = MIXSRC_FIRST_GVAR+MAX_GVARS-1,


  MIXSRC_FIRST_TELEM,
  MIXSRC_LAST_TELEM = MIXSRC_FIRST_TELEM+NUM_TELEMETRY-1
};

#define MIXSRC_FIRST   (MIXSRC_NONE+1)
#define MIXSRC_LAST    MIXSRC_LAST_CH
#define INPUTSRC_FIRST MIXSRC_Rud
#define INPUTSRC_LAST  MIXSRC_LAST_TELEM

#define MIN_POINTS 3
#define MAX_POINTS 17

enum TimerModes {
  TMRMODE_NONE,
  TMRMODE_ABS,
  TMRMODE_THR,
  TMRMODE_THR_REL,
  TMRMODE_THR_TRG,
  TMRMODE_COUNT
};

#if defined(CPUM2560)
PACK(typedef struct {
  int8_t   mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t start;
  uint8_t  countdownBeep:2;
  uint8_t  minuteBeep:1;
  uint8_t  persistent:2;
  uint8_t  spare:3;
  uint16_t value;
}) TimerData;
#define IS_MANUAL_RESET_TIMER(idx) (g_model.timers[idx].persistent == 2)
#endif

enum Protocols {
  PROTO_PPM,
  PROTO_PPM16,
  PROTO_PPMSIM,
#if defined(PXX) || defined(DSM2)
  PROTO_PXX,
#endif
#if defined(DSM2)
  PROTO_DSM2_LP45,
  PROTO_DSM2_DSM2,
  PROTO_DSM2_DSMX,
#endif
  PROTO_SPIMODULE,
#if defined(MULTIMODULE)
  PROTO_MULTIMODULE,
#endif
  PROTO_MAX,
  PROTO_NONE
};

enum RFProtocols {
  RF_PROTO_OFF = -1,
  RF_PROTO_X16,
  RF_PROTO_D8,
  RF_PROTO_LR12,
  RF_PROTO_LAST = RF_PROTO_LR12
};

#define HAS_RF_PROTOCOL_FAILSAFE(rf)   ((rf) == RF_PROTO_X16)
#define HAS_RF_PROTOCOL_MODELINDEX(rf) (((rf) == RF_PROTO_X16) || ((rf) == RF_PROTO_LR12))

enum DSM2Protocols {
  DSM2_PROTO_LP45,
  DSM2_PROTO_DSM2,
  DSM2_PROTO_DSMX,
};

enum ModuleTypes {
  MODULE_TYPE_NONE = 0,
  MODULE_TYPE_PPM,
  MODULE_TYPE_XJT,
#if defined(DSM2)
  MODULE_TYPE_DSM2,
#endif
  MODULE_TYPE_SPIMODULE,
#if defined(MULTIMODULE)
  MODULE_TYPE_MULTIMODULE,
#endif
  MODULE_TYPE_COUNT
};

#define IS_PULSES_EXTERNAL_MODULE() (g_model.moduleData[EXTERNAL_MODULE].type != MODULE_TYPE_NONE)

enum FailsafeModes {
  FAILSAFE_NOT_SET,
  FAILSAFE_HOLD,
  FAILSAFE_CUSTOM,
  FAILSAFE_NOPULSES,
  FAILSAFE_RECEIVER,
  FAILSAFE_LAST = FAILSAFE_RECEIVER
};

#if defined(MAVLINK)
#define TELEMETRY_DATA MavlinkData mavlink;
#else
#define TELEMETRY_DATA FrSkyData frsky;
#endif

#if defined(CPUM2560)
#define BeepANACenter uint16_t
#endif

PACK(typedef struct {
  char      name[LEN_MODEL_NAME]; // must be first for eeLoadModelName
  uint8_t   modelId;
}) ModelHeader;

enum ThrottleSources {
  THROTTLE_SOURCE_THR,
  THROTTLE_SOURCE_FIRST_POT,
  THROTTLE_SOURCE_P1 = THROTTLE_SOURCE_FIRST_POT,
  THROTTLE_SOURCE_P2,
  THROTTLE_SOURCE_P3,
  THROTTLE_SOURCE_CH1,
};

enum TelemetryType {
  PROTOCOL_TELEMETRY_FIRST,
  PROTOCOL_FRSKY_SPORT = PROTOCOL_TELEMETRY_FIRST,
  PROTOCOL_FRSKY_D,
  PROTOCOL_FRSKY_D_SECONDARY,
  PROTOCOL_PULSES_CROSSFIRE
};

enum DisplayTrims {
  DISPLAY_TRIMS_NEVER,
  DISPLAY_TRIMS_CHANGE,
  DISPLAY_TRIMS_ALWAYS
};

enum MultiModuleRFProtocols {
  MM_RF_PROTO_CUSTOM = -1,
  MM_RF_PROTO_FIRST = MM_RF_PROTO_CUSTOM,
  MM_RF_PROTO_FLYSKY=0,
  MM_RF_PROTO_HUBSAN,
  MM_RF_PROTO_FRSKY,
  MM_RF_PROTO_HISKY,
  MM_RF_PROTO_V2X2,
  MM_RF_PROTO_DSM2,
  MM_RF_PROTO_DEVO,
  MM_RF_PROTO_YD717,
  MM_RF_PROTO_KN,
  MM_RF_PROTO_SYMAX,
  MM_RF_PROTO_SLT,
  MM_RF_PROTO_CX10,
  MM_RF_PROTO_CG023,
  MM_RF_PROTO_BAYANG,
  MM_RF_PROTO_ESky,
  MM_RF_PROTO_MT99XX,
  MM_RF_PROTO_MJXQ,
  MM_RF_PROTO_SHENQI,
  MM_RF_PROTO_FY326,
  MM_RF_PROTO_SFHSS,
  MM_RF_PROTO_J6PRO,
  MM_RF_PROTO_FQ777,
  MM_RF_PROTO_ASSAN,
  MM_RF_PROTO_HONTAI,
  MM_RF_PROTO_OLRS,
  MM_RF_PROTO_FS_AFHDS2A,
  MM_RF_PROTO_Q2X2,
  MM_RF_PROTO_WK_2X01,
  MM_RF_PROTO_Q303,
  MM_RF_PROTO_GW008,
  MM_RF_PROTO_DM002,
  MM_RF_PROTO_LAST= MM_RF_PROTO_DM002
};
enum MMDSM2Subtypes {
  MM_RF_DSM2_SUBTYPE_DSM2_22,
  MM_RF_DSM2_SUBTYPE_DSM2_11,
  MM_RF_DSM2_SUBTYPE_DSMX_22,
  MM_RF_DSM2_SUBTYPE_DSMX_11,
  MM_RF_DSM2_SUBTYPE_AUTO
};

enum MMRFrskySubtypes {
  MM_RF_FRSKY_SUBTYPE_D16,
  MM_RF_FRSKY_SUBTYPE_D8,
  MM_RF_FRSKY_SUBTYPE_D16_8CH,
  MM_RF_FRSKY_SUBTYPE_V8,
  MM_RF_FRSKY_SUBTYPE_D16_LBT,
  MM_RF_FRSKY_SUBTYPE_D16_LBT_8CH
};
 
#define MM_RF_CUSTOM_SELECTED 0xff
PACK(struct ModuleData {
  uint8_t type:4;
  int8_t  rfProtocol:6;
//  uint8_t channelsStart;
//  int8_t  channelsCount; // 0=8 channels
//  uint8_t spare:3;
  uint8_t rxnum:4; 
  uint8_t customProto:1; 
  uint8_t autoBindMode:1; 
  uint8_t lowPowerMode:1; 
  int8_t optionValue; 
//  uint8_t failsafeMode:4;  //only 3 bits used
  uint8_t subType:3; 
//  uint8_t invertedSerial:1; // telemetry serial inverted from standard
//  int16_t failsafeChannels[12];
 /* union {
    struct {
      int8_t  delay:6;
      uint8_t pulsePol:1;
      uint8_t outputType:1;    // false = open drain, true = push pull
      int8_t  frameLength;
    } ppm;
    NOBACKUP(struct {
      uint8_t rfProtocolExtra:2;
      uint8_t spare:3;
      uint8_t customProto:1;
      uint8_t autoBindMode:1;
      uint8_t lowPowerMode:1;
      int8_t optionValue;
    } multi);
    NOBACKUP(struct {
      uint8_t spare:4;
      uint8_t receiver_telem_off:1;     // false = receiver telem enabled
      uint8_t receiver_channel_9_16:1;  // false = pwm out 1-8, true 9-16
      uint8_t external_antenna:1; // false = internal antenna, true = external antenna
      uint8_t spare2:1;
      uint8_t spare3;
    } pxx);
  };

  // Helper functions to set both of the rfProto protocol at the same time
  NOBACKUP(inline uint8_t getMultiProtocol(bool returnCustom) {
    if (returnCustom && multi.customProto)
      return MM_RF_CUSTOM_SELECTED;
    return ((uint8_t) (rfProtocol & 0x0f)) + (multi.rfProtocolExtra << 4);
  })

  NOBACKUP(inline void setMultiProtocol(uint8_t proto)
  {
    rfProtocol = (uint8_t) (proto & 0x0f);
    multi.rfProtocolExtra = (proto & 0x30) >> 4;
  })*/

}); 

PACK(typedef struct {
  ModelHeader header;
  TimerData timers[MAX_TIMERS];
  uint8_t   protocol:4;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  int8_t    ppmNCH:4;
  int8_t    trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   throttleReversed:1;
  int8_t    ppmDelay;
  BeepANACenter beepANACenter;
  MixData   mixData[MAX_MIXERS];
  LimitData limitData[NUM_CHNOUT];
  ExpoData  expoData[MAX_EXPOS];

  CURVDATA  curves[MAX_CURVES];
  int8_t    points[NUM_POINTS];

  LogicalSwitchData logicalSw[NUM_LOGICAL_SWITCH];
  CustomFunctionData customFn[NUM_CFN];
  SwashRingData swashR;
  FlightModeData flightModeData[MAX_FLIGHT_MODES];

  int8_t ppmFrameLength;     // 0=22.5ms  (10ms-30ms) 0.5ms increments

  uint8_t thrTraceSrc;

  swarnstate_t  switchWarningState;
  swarnenable_t switchWarningEnable;
  
  ModuleData moduleData;

  MODEL_GVARS_DATA

  TELEMETRY_DATA

}) ModelData;


extern EEGeneral g_eeGeneral;
extern ModelData g_model;

#define TOTAL_EEPROM_USAGE (sizeof(ModelData)*MAX_MODELS + sizeof(EEGeneral))

#endif
