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


#ifndef OpenAVRc_h
#define OpenAVRc_h

#include <inttypes.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#if !defined(SIMU)
#include <avr/pgmspace.h>
#include "pgmtypes.h"
#define assert(x)
#include "targets/common_avr/board_avr.h"
#else //SIMU define
#include <stdbool.h>
#include "targets/simu/simu_interface.h"
#include "targets/mega2560/board_mega2560.h" //todo
#endif

/////////////////DEBUG FUNCTION DEFINITION///////////////////
//#define LCDDURATIONSHOW 1  //Show refresh duration

#if defined(LCDDURATIONSHOW)
#define REFRESHDURATION1                                                      \
  uint16_t t0 = getTmr16KHz();                                                \
  static uint16_t refreshDuration;                                            \
  lcdDrawNumberAttUnit(16*FW, 1, DURATION_MS_PREC2(refreshDuration), PREC2);  \

#define REFRESHDURATION2                                                      \
  t0 = getTmr16KHz() - t0;                                                    \
  if (t0 > refreshDuration) refreshDuration = t0;                             \

#else
#define REFRESHDURATION1
#define REFRESHDURATION2
#endif


#if defined(CPUM2560)
#define CASE_PERSISTENT_TIMERS(x) x,
#else
#define CASE_PERSISTENT_TIMERS(x)
#endif

#if defined(RTCLOCK)
#define CASE_RTCLOCK(x) x,
#else
#define CASE_RTCLOCK(x)
#endif

#if defined(BUZZER)
#define CASE_BUZZER(x) x,
#else
#define CASE_BUZZER(x)
#endif

#if defined(AUDIO)
#define CASE_AUDIO(x) x,
#else
#define CASE_AUDIO(x)
#endif

#if defined(VOICE)
#define CASE_VOICE(x) x,
#else
#define CASE_VOICE(x)
#endif

#if defined(PWM_BACKLIGHT)
#define CASE_PWM_BACKLIGHT(x) x,
#else
#define CASE_PWM_BACKLIGHT(x)
#endif

#if defined(FRSKY) && defined(FRSKY_HUB) && defined(GPS)
#define CASE_GPS(x) x,
#else
#define CASE_GPS(x)
#endif

#if defined(VARIO)
#define CASE_VARIO(x) x,
#else
#define CASE_VARIO(x)
#endif

#if defined(HAPTIC)
#define CASE_HAPTIC(x) x,
#else
#define CASE_HAPTIC(x)
#endif

#if defined(SPLASH)
#define CASE_SPLASH(x) x,
#else
#define CASE_SPLASH(x)
#endif

#if defined(FRSKY)
#define CASE_FRSKY(x) x,
#else
#define CASE_FRSKY(x)
#endif

#if defined(MAVLINK)
#define CASE_MAVLINK(x) x,
#else
#define CASE_MAVLINK(x)
#endif

#if defined(PXX)
#define CASE_PXX(x) x,
#else
#define CASE_PXX(x)
#endif

#if defined(SDCARD)
#define CASE_SDCARD(x) x,
#else
#define CASE_SDCARD(x)
#endif

#if defined(HELI)
#define CASE_HELI(x) x,
#else
#define CASE_HELI(x)
#endif

#if defined(TEMPLATES)
#define CASE_TEMPLATES(x) x,
#else
#define CASE_TEMPLATES(x)
#endif

#if defined(FLIGHT_MODES)
#define CASE_FLIGHT_MODES(x) x,
#else
#define CASE_FLIGHT_MODES(x)
#endif

#if defined(CURVES)
#define CASE_CURVES(x) x,
#else
#define CASE_CURVES(x)
#endif

#if defined(GVARS)
#define CASE_GVARS(x) x,
#else
#define CASE_GVARS(x)
#endif

#define CASE_CAPACITY(x)

#if ROTARY_ENCODERS > 0
#define ROTARY_ENCODER_NAVIGATION
#endif

#if defined(SIMU)
#define __DMA
#else
#define __DMA __attribute__((aligned(32)))
#endif

#if GCC_VERSION < 472
typedef int32_t int24_t;
#else
typedef __int24 int24_t;
#endif

#if defined(FAI)
#define IS_FAI_ENABLED() true
#define IF_FAI_CHOICE(x)
#elif defined(FAI_CHOICE)
#define IS_FAI_ENABLED() g_eeGeneral.fai
#define IF_FAI_CHOICE(x) x,
#else
#define IS_FAI_ENABLED() false
#define IF_FAI_CHOICE(x)
#endif

#define IS_FAI_FORBIDDEN(idx) (IS_FAI_ENABLED() && idx >= MIXSRC_FIRST_TELEM)

#if defined(SIMU)
#ifndef FORCEINLINE
#define FORCEINLINE
#endif
#if !defined(NOINLINE)
#define NOINLINE
#endif
//#define CONVERT_PTR_UINT(x) ((uint32_t)(uint64_t)(x))
//#define CONVERT_UINT_PTR(x) ((uint32_t*)(uint64_t)(x))
#define TRACE(...) simuTrace(__VA_ARGS__)
#define SIMU_PROCESSEVENTS SimuSleepMs(0)  //This function tell the simu app to process events
#define MYWDT_RESET(x) x; SimuSleepMs(1)
#define SIMU_SLEEP(x) SimuSleepMs(x)
#define SIMU_UNLOCK_MACRO(x) (false)
#define wdt_disable() simu_off = true; simu_mainloop_is_runing = true; return
char *convertSimuPath(const char *path);
extern ISR(TIMER_10MS_VECT, ISR_NOBLOCK);
extern int simumain(void);
extern  void SimuMainLoop(void);
extern  void shutDownSimu(void);

#else //NOT SIMU NOW

#define FORCEINLINE inline __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))
#define TRACE(...)
#define SIMU_SLEEP(x)
#define SIMU_PROCESSEVENTS
#define MYWDT_RESET(x) wdt_reset()
#define SIMU_UNLOCK_MACRO(x) (x)
#define CONVERT_PTR_UINT(x) ((uint32_t)(x))
#define CONVERT_UINT_PTR(x) ((uint32_t *)(x))
#define convertSimuPath(x) (x)
#endif

#if !defined(CPUM64) && !defined(ACCURAT_THROTTLE_TIMER)
//  code cost is about 16 bytes for higher throttle accuracy for timer
//  would not be noticable anyway, because all version up to this change had only 16 steps;
//  now it has already 32  steps; this define would increase to 128 steps
#if !defined(ACCURAT_THROTTLE_TIMER)
#define ACCURAT_THROTTLE_TIMER
#endif
#endif

// RESX range is used for internal calculation; The menu says -100.0 to 100.0; internally it is -1024 to 1024 to allow some optimizations
#define RESX_SHIFT 10
#define RESX       1024
#define RESXu      1024u
#define RESXul     1024ul
#define RESXl      1024l
#define F_CPU 16000000UL  // 16 MHz

#include "myeeprom.h"
#include "gui/gui.h"

#if !defined(SIMU)
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define pgm_read_adr(address_short) pgm_read_word(address_short)
#include <avr/wdt.h>
#endif

#if defined(PCBSTD) && defined(VOICE)
volatile uint8_t LcdLock;
#define LCD_LOCK() LcdLock = 1
#define LCD_UNLOCK() LcdLock = 0
#else
#define LCD_LOCK()
#define LCD_UNLOCK()
#endif

#define boardOff()  pwrOff();
#define NUM_SWITCHES     7
#define IS_3POS(sw)      ((sw) == 0)
#define IS_MOMENTARY(sw) (sw == SWSRC_TRN)
#define NUM_SW_SRCRAW    1
#define SW_DSM2_BIND     SW_TRN

#define NUM_PSWITCH        (SWSRC_LAST_SWITCH-SWSRC_FIRST_SWITCH+1)
#define NUM_POTSSW         (NUM_XPOTS*6)

#define KEY_ENTER        KEY_MENU
#define KEY_PLUS         KEY_RIGHT
#define KEY_MINUS        KEY_LEFT


#if defined(CPUM64)
void memclear(void *ptr, uint8_t size);
#else
#define memclear(p, s) memset(p, 0, s)
#endif

#define IS_POT_AVAILABLE(x)       (true)
#define IS_POT_MULTIPOS(x)        (false)
#define IS_POT_WITHOUT_DETENT(x)  (true)

#define IS_POT(x)                   ((x)>=POT1 && (x)<=POT_LAST)

#define GET_LOWRES_POT_POSITION(i)  (getValue(MIXSRC_FIRST_POT+(i)) >> 4)
#define SAVE_POT_POSITION(i)        g_model.potsWarnPosition[i] = GET_LOWRES_POT_POSITION(i)

#if ROTARY_ENCODERS > 0
#define IF_ROTARY_ENCODERS(x) x,
#else
#define IF_ROTARY_ENCODERS(x)
#endif

#define PPM_CENTER 1500

#if defined(PPM_CENTER_ADJUSTABLE)
#define PPM_CH_CENTER(ch) (PPM_CENTER+limitAddress(ch)->ppmCenter)
#else
#define PPM_CH_CENTER(ch) (PPM_CENTER)
#endif

#define tmr10ms_t uint16_t

extern volatile tmr10ms_t g_tmr10ms;

inline uint16_t get_tmr10ms()
{
  uint16_t time  ;
  cli();
  time = g_tmr10ms ;
  sei();
  return time ;
}
typedef int8_t rotenc_t;
typedef int16_t getvalue_t;
typedef uint8_t mixsrc_t;
typedef int8_t swsrc_t;


#if defined(NAVIGATION_STICKS)
extern uint8_t getSticksNavigationEvent();
extern uint8_t StickScrollAllowed;
extern uint8_t StickScrollTimer;
#define STICK_SCROLL_TIMEOUT          9
#define STICK_SCROLL_DISABLE()        StickScrollAllowed = 0
#else
#define STICK_SCROLL_DISABLE()
#endif

#include "eeprom_common.h"

#if defined(EEPROM_RLC)
#include "eeprom_rlc.h"
#endif


#include "pulses/pulses.h"

#define LOAD_MODEL_BITMAP()

#define LOAD_MODEL_CURVES()


#define IS_MODULE_PPM(idx)                (idx==TRAINER_MODULE || (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_PPM))
#define IS_MODULE_XJT(idx)                (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_XJT)
#define IS_MODULE_DSM2(idx)               (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_DSM2)
#define MAX_EXTERNAL_MODULE_CHANNELS()    ((g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_XJT) ? maxChannelsXJT[1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol] : maxChannelsModules[g_model.moduleData[EXTERNAL_MODULE].type])
#define MAX_CHANNELS(idx)                 (idx==EXTERNAL_MODULE ? MAX_EXTERNAL_MODULE_CHANNELS() : MAX_TRAINER_CHANNELS())
#define NUM_CHANNELS(idx)                 (8+g_model.moduleData[idx].channelsCount)

#if defined(CPUM64)
#define MASK_CFN_TYPE  uint16_t  // current max = 16 function switches
#define MASK_FUNC_TYPE uint8_t   // current max = 8  functions
#else
#define MASK_CFN_TYPE  uint32_t  // current max = 32 function switches
#define MASK_FUNC_TYPE uint8_t   // current max = 8 functions
#endif

typedef struct {
  MASK_FUNC_TYPE activeFunctions;
  MASK_CFN_TYPE  activeSwitches;
  tmr10ms_t lastFunctionTime[NUM_CFN];

  inline bool isFunctionActive(uint8_t func)
  {
    return activeFunctions & ((MASK_FUNC_TYPE)1 << func);
  }

  void reset()
  {
    memclear(this, sizeof(*this));
  }
} CustomFunctionsContext;


///  Dimension of Arrays
#define DIM(a) ((sizeof a) / (sizeof *a))


#include "gui/gui.h"

#if defined(TEMPLATES)
#include "templates.h"
#endif


extern const pm_uint8_t bchout_ar[];
extern const pm_uint8_t modn12x3[];

extern uint8_t stickMode;

//convert from mode 1 to mode stickMode
//NOTICE!  =>  0..3 -> 0..3
#define RUD_STICK 0
#define ELE_STICK 1
#define THR_STICK 2
#define AIL_STICK 3
#define CONVERT_MODE(x)  (((x)<=AIL_STICK) ? pgm_read_byte(modn12x3 + 4*stickMode + (x)) : (x) )

extern uint8_t channel_order(uint8_t x);

enum BaseCurves {
  CURVE_NONE,
  CURVE_X_GT0,
  CURVE_X_LT0,
  CURVE_ABS_X,
  CURVE_F_GT0,
  CURVE_F_LT0,
  CURVE_ABS_F,
  CURVE_BASE
};

#define THRCHK_DEADBAND 16

#define SPLASH_NEEDED() (!IS_DSM2_PROTOCOL(g_model.protocol) && !g_eeGeneral.splashMode)

#if defined(FSPLASH)
#define SPLASH_TIMEOUT  (g_eeGeneral.splashMode == 0 ? 60000/*infinite=10mn*/ : ((4*100) * (g_eeGeneral.splashMode & 0x03)))
#else
#define SPLASH_TIMEOUT  (4*100)  // 4 seconds
#endif

#if defined(ROTARY_ENCODERS)
#define NAVIGATION_RE_IDX()         (g_eeGeneral.reNavigation - 1)
#define IS_RE_NAVIGATION_ENABLE()   g_eeGeneral.reNavigation
#elif defined(ROTARY_ENCODER_NAVIGATION)
#define IS_RE_NAVIGATION_ENABLE()   true
#define NAVIGATION_RE_IDX()         0
#endif

#define HEART_TIMER_10MS     1
#define HEART_TIMER_PULSES   2 // when multiple modules this is the first one
#define HEART_WDT_CHECK      (HEART_TIMER_10MS + HEART_TIMER_PULSES)

extern uint8_t heartbeat;


#define MAX_ALERT_TIME   60

struct t_inactivity
{
  uint16_t counter;
  uint8_t  sum;
};

extern struct t_inactivity inactivity;

#define LEN_STD_CHARS 40

#if defined(TRANSLATIONS_CZ)
#define ZCHAR_MAX (LEN_STD_CHARS)
#else
#define ZCHAR_MAX (LEN_STD_CHARS + LEN_SPECIAL_CHARS)
#endif

char hex2zchar(uint8_t hex);
char idx2char(int8_t idx);
#if defined(SIMU)
int8_t char2idx(char c);
void str2zchar(char *dest, const char *src, int size);
int zchar2str(char *dest, const char *src, int size);
#endif

#include "keys.h"
#include "pwr.h"


bool switchState(EnumKeys enuk);
uint8_t trimDown(uint8_t idx);

uint16_t evalChkSum();

#if !defined(GUI)
#define MESSAGE_SOUND_ARG
#define MESSAGE(...)
#define ALERT(...)
#elif defined(VOICE)
#define MESSAGE_SOUND_ARG , uint8_t sound
#define MESSAGE(title, msg, info, sound) message(title, msg, info, sound)
#define ALERT(title, msg, sound) alert(title, msg, sound)
#else
#define MESSAGE_SOUND_ARG
#define MESSAGE(title, msg, info, sound) message(title, msg, info)
#define ALERT(title, msg, sound) alert(title, msg)
#endif

extern void message(const pm_char *title, const pm_char *s, const char *last MESSAGE_SOUND_ARG);
extern void alert(const pm_char * t, const pm_char * s MESSAGE_SOUND_ARG);

enum PerOutMode {
  e_perout_mode_normal = 0,
  e_perout_mode_inactive_flight_mode = 1,
  e_perout_mode_notrainer = 2,
  e_perout_mode_notrims = 4,
  e_perout_mode_nosticks = 8,
  e_perout_mode_noinput = e_perout_mode_notrainer+e_perout_mode_notrims+e_perout_mode_nosticks
};


#if defined(MODULE_ALWAYS_SEND_PULSES)
extern uint8_t startupWarningState;

enum StartupWarningStates {
  STARTUP_WARNING_THROTTLE,
  STARTUP_WARNING_SWITCHES,
  STARTUP_WARNING_DONE,
};
#endif


// Fiddle to force compiler to use a pointer
#if defined(SIMU)
#define FORCE_INDIRECT(ptr)
#else
#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))
#endif

extern uint8_t mixerCurrentFlightMode;
extern uint8_t lastFlightMode;
extern uint8_t flightModeTransitionLast;

#define bitfield_channels_t uint16_t

#if defined(SIMU)
inline int availableMemory() { return 1000; }
#endif

void evalFlightModeMixes(uint8_t mode, uint8_t tick10ms);
void evalMixes(uint8_t tick10ms);
void doMixerCalculations();

void perMain();
NOINLINE void per10ms();

getvalue_t getValue(mixsrc_t i);

bool getSwitch(swsrc_t swtch);

void logicalSwitchesTimerTick();
void logicalSwitchesReset();

#define evalLogicalSwitches(xxx)
#define GETSWITCH_RECURSIVE_TYPE uint16_t
extern volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_used;
extern volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_value;
#define LS_RECURSIVE_EVALUATION_RESET() s_last_switch_used = 0

#define getSwitchesPosition(...)

extern swarnstate_t switches_states;
swsrc_t getMovedSwitch();

#define GET_MOVED_SOURCE_PARAMS
int8_t getMovedSource();
#define GET_MOVED_SOURCE(min, max) getMovedSource()

#if defined(FLIGHT_MODES)
extern uint8_t getFlightMode();
#else
#define getFlightMode() 0
#endif

uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx);

#if defined(GVARS)
extern int8_t trimGvar[NUM_STICKS];
#define TRIM_REUSED(idx) trimGvar[idx] >= 0
#else
#define TRIM_REUSED(idx) 0
#endif

trim_t getRawTrimValue(uint8_t phase, uint8_t idx);
int getTrimValue(uint8_t phase, uint8_t idx);

void setTrimValue(uint8_t phase, uint8_t idx, int trim);

#if defined(ROTARY_ENCODERS)
int16_t getRotaryEncoder(uint8_t idx);
void incRotaryEncoder(uint8_t idx, int8_t inc);
#endif

#if defined(PCBGRUVIN9X) || defined(PCBMEGA2560)
#define ROTARY_ENCODER_GRANULARITY (1)
#else
#define ROTARY_ENCODER_GRANULARITY (2)
#endif

#if defined(GVARS)
#if defined(PCBSTD)
int16_t getGVarValue(int16_t x, int16_t min, int16_t max);
void setGVarValue(uint8_t x, int8_t value);
#define GET_GVAR(x, min, max, p) getGVarValue(x, min, max)
#define SET_GVAR(idx, val, p) setGVarValue(idx, val)
#else
uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx);
int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase);
void setGVarValue(uint8_t x, int16_t value, int8_t phase);
#define GET_GVAR(x, min, max, p) getGVarValue(x, min, max, p)
#define SET_GVAR(idx, val, p) setGVarValue(idx, val, p)
#define GVAR_DISPLAY_TIME     100 /*1 second*/;
extern uint8_t s_gvar_timer;
extern uint8_t s_gvar_last;
#endif
#else
#define GET_GVAR(x, ...) (x)
#endif

#define GV_GET_GV1_VALUE(max)        ( (max<=GV_RANGESMALL) ? GV1_SMALL : GV1_LARGE )
#define GV_INDEX_CALCULATION(x,max)  ( (max<=GV1_SMALL) ? (uint8_t) x-GV1_SMALL : ((x&(GV1_LARGE*2-1))-GV1_LARGE) )
#define GV_IS_GV_VALUE(x,min,max)    ( (x>max) || (x<min) )

#define GV_INDEX_CALC_DELTA(x,delta) ((x&(delta*2-1)) - delta)

#define GV_CALC_VALUE_IDX_POS(idx,delta) (-delta+idx)
#define GV_CALC_VALUE_IDX_NEG(idx,delta) (delta+idx)

#define GV_RANGESMALL      (GV1_SMALL - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGESMALL_NEG  (-GV1_SMALL + (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE      (GV1_LARGE - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE_NEG  (-GV1_LARGE + (RESERVE_RANGE_FOR_GVARS+1))
// for stock we just use as much as possible
#define GV_RANGELARGE_WEIGHT      GV_RANGELARGE
#define GV_RANGELARGE_WEIGHT_NEG  GV_RANGELARGE_NEG
#define GV_RANGELARGE_OFFSET      GV_RANGELARGE
#define GV_RANGELARGE_OFFSET_NEG  GV_RANGELARGE_NEG

extern uint16_t sessionTimer;
extern uint16_t s_timeCumThr;
extern uint16_t s_timeCum16ThrP;

#if defined(OVERRIDE_CHANNEL_FUNCTION)
typedef int8_t safetych_t;
#define OVERRIDE_CHANNEL_UNDEFINED -128
extern safetych_t safetyCh[NUM_CHNOUT];
#endif

extern uint8_t trimsCheckTimer;


void flightReset();

extern uint8_t unexpectedShutdown;

extern uint16_t maxMixerDuration;

extern uint8_t g_tmr1Latency_max;
extern uint8_t g_tmr1Latency_min;
extern uint16_t lastMixerDuration;

#define DURATION_MS_PREC2(x) ((x)*100)/16

#if defined(THRTRACE)
#define MAXTRACE (LCD_W - 8)
extern uint8_t  s_traceBuf[MAXTRACE];
#if LCD_W >= 255
extern int16_t  s_traceWr;
extern int16_t  s_traceCnt;
#else
extern uint8_t  s_traceWr;
extern int16_t  s_traceCnt;
#endif
extern uint8_t  s_cnt_10s;
extern uint16_t s_cnt_samples_thr_10s;
extern uint16_t s_sum_samples_thr_10s;
#define RESET_THR_TRACE() s_traceCnt = s_traceWr = s_cnt_10s = s_cnt_samples_thr_10s = s_sum_samples_thr_10s = s_timeCum16ThrP = s_timeCumThr = 0
#else
#define RESET_THR_TRACE() s_timeCum16ThrP = s_timeCumThr = 0
#endif

uint16_t getTmr16KHz();

uint16_t stackAvailable();
uint16_t freeRam();

#if defined(SPLASH)
void doSplash();
#endif

#if MENUS_LOCK == 1
extern bool readonly;
extern bool readonlyUnlocked();
#define READ_ONLY() readonly
#define READ_ONLY_UNLOCKED() readonlyUnlocked()
#else
#define READ_ONLY() false
#define READ_ONLY_UNLOCKED() true
#endif

void checkLowEEPROM();
void checkTHR();
void checkSwitches();
void checkAlarm();
void checkAll();

void getADC();

extern void backlightOn();

enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
  POT1,
  POT2,
  POT3,
  POT_LAST = POT3,
#if defined(TELEMETRY_MOD_14051) || defined(TELEMETRY_MOD_14051_SWAPPED)
  // When the mod is applied, ADC7 is connected to 14051's X pin and TX_VOLTAGE
  // is connected to 14051's X0 pin (one of the multiplexed inputs). TX_VOLTAGE
  // value is filled in by processMultiplexAna().

  // This shifts TX_VOLTAGE from 7 to 8 and makes X14051 take the 7th position
  // corresponding to ADC7.
  X14051,
#endif
  TX_VOLTAGE,
  NUMBER_ANALOG
};

void checkBacklight();

#if defined(PCBSTD) && defined(VOICE) && !defined(SIMU)
#define BACKLIGHT_ON()    (Voice.Backlight = 1)
#define BACKLIGHT_OFF()   (Voice.Backlight = 0)
#else
#define BACKLIGHT_ON()    backlightEnable()
#define BACKLIGHT_OFF()   backlightDisable()
#endif

#define BITMASK(bit) (1<<(bit))

#undef min // Avoid double declaration in wingw
#undef max
template<class t> FORCEINLINE t min(t a, t b) { return a<b?a:b; }
template<class t> FORCEINLINE t max(t a, t b) { return a>b?a:b; }
template<class t> FORCEINLINE t sgn(t a) { return a>0 ? 1 : (a < 0 ? -1 : 0); }
template<class t> FORCEINLINE t limit(t mi, t x, t ma) { return min(max(mi,x),ma); }
template<class t> void SWAP(t & a, t & b) { t tmp = b; b = a; a = tmp; }

uint16_t isqrt32(uint32_t n);

#define pauseMixerCalculations()
#define resumeMixerCalculations()

void generalDefault();
void modelDefault(uint8_t id);


extern int16_t calc100to256_16Bits(int16_t x); // @@@2 open.20.fsguruh: return x*2.56
extern int16_t calc100to256(int8_t x); // @@@2 open.20.fsguruh: return x*2.56
extern int16_t calc100toRESX_16Bits(int16_t x); // @@@ open.20.fsguruh
extern int16_t calc100toRESX(int8_t x);
extern int16_t calc1000toRESX(int16_t x);
extern int16_t calcRESXto1000(int16_t x);
extern int8_t  calcRESXto100(int16_t x);

extern const char vers_stamp[];

extern uint8_t              g_vbat100mV;
#define GET_TXBATT_BARS() (limit<int8_t>(2, 20 * (g_vbat100mV - g_eeGeneral.vBatMin - 90) / (30 + g_eeGeneral.vBatMax - g_eeGeneral.vBatMin), 20))
#define IS_TXBATT_WARNING() (g_vbat100mV <= g_eeGeneral.vBatWarn)


#define g_blinkTmr10ms    (*(uint8_t*)&g_tmr10ms)
extern uint8_t            g_beepCnt;
extern uint8_t            g_beepVal[5];

#include "trainer_input.h"

extern int32_t            chans[NUM_CHNOUT];
extern int16_t            ex_chans[NUM_CHNOUT]; // Outputs (before LIMITS) of the last perMain
extern int16_t            channelOutputs[NUM_CHNOUT];
extern uint16_t           BandGap;

#define NUM_INPUTS      (NUM_STICKS)

int intpol(int x, uint8_t idx);
int expo(int x, int k);

#if   defined(CURVES)
int applyCurve(int x, int8_t idx);
#else
#define applyCurve(x, idx) (x)
#endif


#define applyCustomCurve(x, idx) intpol(x, idx)

#define APPLY_EXPOS_EXTRA_PARAMS_INC
#define APPLY_EXPOS_EXTRA_PARAMS


void applyExpos(int16_t *anas, uint8_t mode APPLY_EXPOS_EXTRA_PARAMS_INC);
int16_t applyLimits(uint8_t channel, int32_t value);

void evalInputs(uint8_t mode);
uint16_t anaIn(uint8_t chan);
extern int16_t calibratedStick[NUM_STICKS+NUM_POTS];

#define FLASH_DURATION 20 /*200ms*/

extern uint8_t beepAgain;
extern uint16_t lightOffCounter;
extern uint8_t flashCounter;
extern uint8_t mixWarning;

FlightModeData *flightModeAddress(uint8_t idx);
ExpoData *expoAddress(uint8_t idx);
MixData *mixAddress(uint8_t idx);
LimitData *limitAddress(uint8_t idx);
int8_t *curveAddress(uint8_t idx);
LogicalSwitchData *lswAddress(uint8_t idx);

// static variables used in evalFlightModeMixes - moved here so they don't interfere with the stack
// It's also easier to initialize them here.
extern int16_t rawAnas[NUM_INPUTS];

extern int16_t  anas[NUM_INPUTS];
extern int16_t  trims[NUM_STICKS];
extern BeepANACenter bpanaCenter;

extern uint8_t s_mixer_first_run_done;

extern int8_t s_currCh;
uint8_t getExpoMixCount(uint8_t expo);
void deleteExpoMix(uint8_t expo, uint8_t idx);
void insertExpoMix(uint8_t expo, uint8_t idx);
void applyDefaultTemplate();

void incSubtrim(uint8_t idx, int16_t inc);
void instantTrim();
FORCEINLINE void evalTrims();
void copyTrimsToOffset(uint8_t ch);
void copySticksToOffset(uint8_t ch);
void moveTrimsToOffsets();

#define ACTIVE_PHASES_TYPE uint8_t
#define delayval_t         int8_t

PACK(typedef struct {
  int16_t hold;
  uint16_t delay:10;
  bool activeMix:1;
  bool activeExpo:1;
}) SwOn;

extern SwOn     swOn  [MAX_MIXERS];
extern int24_t  act   [MAX_MIXERS];

#ifdef BOLD_FONT
inline bool isExpoActive(uint8_t expo)
{
  return swOn[expo].activeExpo;
}

inline bool isMixActive(uint8_t mix)
{
  return swOn[mix].activeMix;
}
#else
#define isExpoActive(x) false
#define isMixActive(x) false
#endif

enum CswFunctionFamilies {
  LS_FAMILY_OFS,
  LS_FAMILY_BOOL,
  LS_FAMILY_COMP,
  LS_FAMILY_DIFF,
  LS_FAMILY_TIMER,
  LS_FAMILY_STICKY,
  LS_FAMILY_RANGE,
  LS_FAMILY_EDGE
};

uint8_t lswFamily(uint8_t func);
int16_t lswTimerValue(delayval_t val);

enum FunctionsActive {
  FUNCTION_TRAINER,
  FUNCTION_INSTANT_TRIM = FUNCTION_TRAINER+4,
  FUNCTION_VARIO,
  FUNCTION_BACKLIGHT,
#if defined(SDCARD)
  FUNCTION_LOGS,
#endif
};

#define VARIO_FREQUENCY_ZERO   700/*Hz*/
#define VARIO_FREQUENCY_RANGE  1000/*Hz*/
#define VARIO_REPEAT_ZERO      500/*ms*/
#define VARIO_REPEAT_MAX       80/*ms*/

extern CustomFunctionsContext modelFunctionsContext;

#define isFunctionActive(func) modelFunctionsContext.isFunctionActive(func)
void evalFunctions();
#define customFunctionsReset() modelFunctionsContext.reset()

#if defined(ROTARY_ENCODERS)
// Global rotary encoder registers
extern volatile rotenc_t g_rotenc[ROTARY_ENCODERS];
#elif defined(ROTARY_ENCODER_NAVIGATION)
extern volatile rotenc_t g_rotenc[1];
#endif


#if defined (FRSKY)
// FrSky Telemetry
#include "telemetry/frsky.h"
#elif defined(JETI)
// Jeti-DUPLEX Telemetry
#include "telemetry/jeti.h"
#elif defined(ARDUPILOT)
// ArduPilot Telemetry
#include "telemetry/ardupilot.h"
#elif defined(NMEA)
// NMEA Telemetry
#include "telemetry/nmea.h"
#elif defined(MAVLINK)
// Mavlink Telemetry
#include "telemetry/mavlink.h"
#endif


#define PLAY_REPEAT(x)            (x)                 /* Range 0 to 15 */
#define PLAY_NOW                  0x10
#define PLAY_BACKGROUND           0x20
#define PLAY_INCREMENT(x)         ((uint8_t)(((uint8_t)x) << 6))   /* -1, 0, 1, 2 */

#define PLAY_MODEL_NAME_NUM_FILE 0xC8 //200 in buildaudiofile models name range 201 to 232

/* make sure the defines below always go in numeric order */
enum AUDIO_SOUNDS {
  AU_TADA,
#if defined(VOICE)
  AU_THROTTLE_ALERT,
  AU_SWITCH_ALERT,
  AU_BAD_EEPROM,
  AU_EEPROM_FORMATTING,
#endif
  AU_TX_BATTERY_LOW,
  AU_INACTIVITY,
  AU_ERROR,
  AU_KEYPAD_UP,
  AU_KEYPAD_DOWN,
  AU_MENUS,
  AU_TRIM_MOVE,
  AU_WARNING1,
  AU_WARNING2,
  AU_WARNING3,
  AU_TRIM_MIDDLE,
  AU_POT_MIDDLE,
  AU_MIX_WARNING_1,
  AU_MIX_WARNING_2,
  AU_MIX_WARNING_3,
  AU_TIMER_00,
  AU_TIMER_LT10,
  AU_TIMER_20,
  AU_TIMER_30,

  AU_FRSKY_FIRST,
  AU_FRSKY_BEEP1 = AU_FRSKY_FIRST,
  AU_FRSKY_BEEP2,
  AU_FRSKY_BEEP3,
  AU_FRSKY_WARN1,
  AU_FRSKY_WARN2,
  AU_FRSKY_CHEEP,
  AU_FRSKY_RATATA,
  AU_FRSKY_TICK,
  AU_FRSKY_SIREN,
  AU_FRSKY_RING,
  AU_FRSKY_SCIFI,
  AU_FRSKY_ROBOT,
  AU_FRSKY_CHIRP,
  AU_FRSKY_TADA,
  AU_FRSKY_CRICKET,
  AU_FRSKY_ALARMC,
  AU_FRSKY_LAST,

  AU_NONE=0xff
};

#if defined(AUDIO)
#include "audio_avr.h"
#endif

#include "buzzer.h"

#if defined(PCBSTD) && defined(VOICE)
#include "targets/stock/voice.h"
#endif

#if defined(PCBGRUVIN9X) && defined(VOICE)
#include "targets/gruvin9x/somo14d.h"
#endif

#if defined(PCBMEGA2560) && defined(VOICE)
#include "targets/mega2560/voice.h"
#endif

#include "translations.h"

//#define PERSONAMES //For tests
#if defined(PERSONAMES)
#include "personames.h"
#endif

#include "fonts.h"

#if defined(HAPTIC)
#include "haptic.h"
#endif

#if defined(SDCARD)
#include "sdcard.h"
#endif

#if defined(RTCLOCK)
#include "rtc.h"
#endif




extern void checkBattery();
extern void OpenAVRcClose();
extern void OpenAVRcInit(uint8_t mcusr);

// Re-useable byte array to save having multiple buffers
#define SD_SCREEN_FILE_LENGTH (32)
union ReusableBuffer
{
  // 275 bytes
  struct
  {
    char listnames[LCD_LINES-1][LEN_MODEL_NAME];
    uint16_t eepromfree;

#if defined(SDCARD)
    char menu_bss[POPUP_MENU_MAX_LINES][MENU_LINE_LENGTH];
    char mainname[45]; // because reused for SD backup / restore, max backup filename 44 chars: "/MODELS/MODEL0134353-2014-06-19-04-51-27.bin"
#else
    char mainname[LEN_MODEL_NAME];
#endif

  } modelsel;

  // 103 bytes
  struct
  {
    int16_t midVals[NUM_STICKS+NUM_POTS];
    int16_t loVals[NUM_STICKS+NUM_POTS];
    int16_t hiVals[NUM_STICKS+NUM_POTS];
    uint8_t state;
  } calib;

#if defined(SDCARD)
  // 274 bytes
  struct
  {
    char lines[LCD_LINES-1][SD_SCREEN_FILE_LENGTH+1+1]; // the last char is used to store the flags (directory) of the line
    uint32_t available;
    uint16_t offset;
    uint16_t count;
    char originalName[SD_SCREEN_FILE_LENGTH+1];
  } sdmanager;
#endif
};

extern union ReusableBuffer reusableBuffer;

void checkFlashOnBeep();

#if   defined(FRSKY)
void convertUnit(getvalue_t & val, uint8_t & unit); // TODO check FORCEINLINE on stock
#else
#define convertUnit(...)
#endif


// Stick tolerance varies between transmitters, Higher is better
#if defined (PCB9XR) || defined (PCB9XR128)
#define STICK_TOLERANCE 16
#else
#define STICK_TOLERANCE 64
#endif

#if defined(FRSKY_HUB) && defined(GAUGES)
enum BarThresholdIdx {
  THLD_ALT,
  THLD_RPM,
  THLD_FUEL,
  THLD_T1,
  THLD_T2,
  THLD_SPEED,
  THLD_DIST,
  THLD_GPSALT,
  THLD_CELL,
  THLD_CELLS_SUM,
  THLD_VFAS,
  THLD_CURRENT,
  THLD_CONSUMPTION,
  THLD_MAX,
};

typedef uint8_t bar_threshold_t;
#define FILL_THRESHOLD(idx, val) barsThresholds[idx] = 128 + (val)

extern bar_threshold_t barsThresholds[THLD_MAX];
#else
#define FILL_THRESHOLD(idx, val)
#endif

#if defined(FRSKY)
ls_telemetry_value_t minTelemValue(source_t channel);
ls_telemetry_value_t maxTelemValue(source_t channel);
#else
#define minTelemValue(channel) 255
#define maxTelemValue(channel) 255
#endif


getvalue_t convert8bitsTelemValue(source_t channel, ls_telemetry_value_t value);
getvalue_t convertLswTelemValue(LogicalSwitchData * cs);

#define convertTelemValue(channel, value) convert8bitsTelemValue(channel, value)
#define convertBarTelemValue(channel, value) convert8bitsTelemValue(channel, value)
#define maxBarTelemValue(channel) maxTelemValue(channel)

#if defined(FRSKY)
lcdint_t applyChannelRatio(source_t channel, lcdint_t val);
#define ANA_CHANNEL_UNIT(channel) g_model.frsky.channels[channel].type
#endif

getvalue_t div10_and_round(getvalue_t value);
getvalue_t div100_and_round(getvalue_t value);

#if defined(FRSKY)
NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm);

extern const pm_uint8_t bchunit_ar[];

#define FRSKY_MULTIPLIER_MAX 3

enum TelemetryViews {
  TELEMETRY_CUSTOM_SCREEN_1,
  TELEMETRY_CUSTOM_SCREEN_2,
  TELEMETRY_VOLTAGES_SCREEN,
  TELEMETRY_AFTER_FLIGHT_SCREEN,
  TELEMETRY_VIEW_MAX = TELEMETRY_AFTER_FLIGHT_SCREEN
};

extern uint8_t s_frsky_view;

#endif

#define EARTH_RADIUSKM ((uint32_t)6371)
#define EARTH_RADIUS ((uint32_t)111194)

void getGpsPilotPosition();
void getGpsDistance();
void varioWakeup();

#if defined(AUDIO) && defined(BUZZER)
#define IS_SOUND_OFF() (g_eeGeneral.buzzerMode==e_mode_quiet && g_eeGeneral.beepMode==e_mode_quiet)
#else
#define IS_SOUND_OFF() (g_eeGeneral.beepMode == e_mode_quiet)
#endif

#if   defined(IMPERIAL_UNITS)
#define IS_IMPERIAL_ENABLE() (1)
#else
#define IS_IMPERIAL_ENABLE() (0)
#endif

#define IS_USR_PROTO_FRSKY_HUB()   (g_model.frsky.usrProto == USR_PROTO_FRSKY)
#define IS_USR_PROTO_WS_HOW_HIGH() (g_model.frsky.usrProto == USR_PROTO_WS_HOW_HIGH)

#if defined(FRSKY) && defined(FRSKY_HUB) && defined(GPS)
#define IS_GPS_AVAILABLE()         IS_USR_PROTO_FRSKY_HUB()
#else
#define IS_GPS_AVAILABLE()         (0)
#endif


#if defined(USB_MASS_STORAGE)
extern void usbPluggedIn();
#endif

#if defined(SDCARD)

extern uint16_t s_anaFilt[NUMBER_ANALOG];

#if defined(JITTER_MEASURE)
extern JitterMeter<uint16_t> rawJitter[NUMBER_ANALOG];
extern JitterMeter<uint16_t> avgJitter[NUMBER_ANALOG];
#endif  // defined(JITTER_MEASURE)

#endif

#endif //file
