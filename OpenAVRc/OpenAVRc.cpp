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


#include "OpenAVRc.h"
#include "timers.h"

EEGeneral  g_eeGeneral;
ModelData  g_model;

uint16_t g_tmr1Latency_max;
uint16_t g_tmr1Latency_min;
uint16_t lastMixerDuration;

uint8_t unexpectedShutdown = 0;

/* AVR: mixer duration in 1/16ms */
uint16_t maxMixerDuration;

#if defined(AUDIO)
audioQueue  audio;
#endif

uint8_t heartbeat;

uint8_t stickMode;

#if defined(OVERRIDE_CHANNEL_FUNCTION)
safetych_t safetyCh[NUM_CHNOUT];
#endif

union ReusableBuffer reusableBuffer;

const pm_uint8_t bchout_ar[] PROGMEM = {
  0x1B, 0x1E, 0x27, 0x2D, 0x36, 0x39,
  0x4B, 0x4E, 0x63, 0x6C, 0x72, 0x78,
  0x87, 0x8D, 0x93, 0x9C, 0xB1, 0xB4,
  0xC6, 0xC9, 0xD2, 0xD8, 0xE1, 0xE4
};

uint8_t channel_order(uint8_t x)
{
  return ( ((pgm_read_byte_near(bchout_ar + g_eeGeneral.templateSetup) >> (6-(x-1) * 2)) & 3 ) + 1 );
}

/*
mode1 rud ele thr ail
mode2 rud thr ele ail
mode3 ail ele thr rud
mode4 ail thr ele rud
*/
const pm_uint8_t modn12x3[] PROGMEM = {
  0, 1, 2, 3,
  0, 2, 1, 3,
  3, 1, 2, 0,
  3, 2, 1, 0
};

volatile tmr10ms_t g_tmr10ms;


void per10ms()
{
  g_tmr10ms++;

#if defined(GUI)
  if (lightOffCounter) lightOffCounter--;
  if (flashCounter) flashCounter--;
  if (noHighlightCounter) noHighlightCounter--;
#endif

  if (trimsCheckTimer) trimsCheckTimer--;
  if (ppmInputValidityTimer) ppmInputValidityTimer--;


#if defined(RTCLOCK)
  /* Update global Date/Time every 100 per10ms cycles */
  if (++g_ms100 == 100) {
    g_rtcTime++;   // inc global unix timestamp one second
    g_ms100 = 0;
  }
#endif

  readKeysAndTrims();

#if defined(ROTARY_ENCODER_NAVIGATION)
  if (IS_RE_NAVIGATION_ENABLE()) {
    static rotenc_t rePreviousValue;
    rotenc_t reNewValue = (g_rotenc[NAVIGATION_RE_IDX()] / ROTARY_ENCODER_GRANULARITY);
    int8_t scrollRE = reNewValue - rePreviousValue;
    if (scrollRE) {
      rePreviousValue = reNewValue;
      putEvent(scrollRE < 0 ? EVT_ROTARY_LEFT : EVT_ROTARY_RIGHT);
    }
    uint8_t evt = s_evt;
    if (EVT_KEY_MASK(evt) == BTN_REa + NAVIGATION_RE_IDX()) {
      if (IS_KEY_BREAK(evt)) {
        putEvent(EVT_ROTARY_BREAK);
      } else if (IS_KEY_LONG(evt)) {
        putEvent(EVT_ROTARY_LONG);
      }
    }
  }
#endif

#if defined(FRSKY) || defined(JETI)
  if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol[0]))
    telemetryInterrupt10ms();
#endif

  // These moved here from evalFlightModeMixes() to improve beep trigger reliability.
#if defined(PWM_BACKLIGHT)
  if ((g_tmr10ms&0x03) == 0x00)
    backlightFade(); // increment or decrement brightness until target brightness is reached
#endif

#if !defined(AUDIO)
  if (mixWarning & 1) if(((g_tmr10ms&0xFF)==  0)) AUDIO_MIX_WARNING(1);
  if (mixWarning & 2) if(((g_tmr10ms&0xFF)== 64) || ((g_tmr10ms&0xFF)== 72)) AUDIO_MIX_WARNING(2);
  if (mixWarning & 4) if(((g_tmr10ms&0xFF)==128) || ((g_tmr10ms&0xFF)==136) || ((g_tmr10ms&0xFF)==144)) AUDIO_MIX_WARNING(3);
#endif

#if defined(SDCARD)
  sdPoll10ms();
#endif

  heartbeat |= HEART_TIMER_10MS;

}

FlightModeData *flightModeAddress(uint8_t idx)
{
  return &g_model.flightModeData[idx];
}

ExpoData *expoAddress(uint8_t idx )
{
  return &g_model.expoData[idx];
}

MixData *mixAddress(uint8_t idx)
{
  return &g_model.mixData[idx];
}

LimitData *limitAddress(uint8_t idx)
{
  return &g_model.limitData[idx];
}


void generalDefault()
{
  memclear(&g_eeGeneral, sizeof(g_eeGeneral));
  g_eeGeneral.version  = EEPROM_VER;
  g_eeGeneral.variant = EEPROM_VARIANT;
  g_eeGeneral.contrast = 15;



  g_eeGeneral.vBatWarn = 90;

#if defined(DEFAULT_MODE)
  g_eeGeneral.stickMode = DEFAULT_MODE-1;
#endif


  g_eeGeneral.backlightMode = e_backlight_mode_all;
  g_eeGeneral.lightAutoOff = 2;
  g_eeGeneral.inactivityTimer = 10;




  g_eeGeneral.chkSum = 0xFFFF;
}

uint16_t evalChkSum()
{
  uint16_t sum = 0;
  const int16_t *calibValues = (const int16_t *) &g_eeGeneral.calib[0];
  for (int i=0; i<12; i++)
    sum += calibValues[i];
  return sum;
}


#if defined(TEMPLATES)
inline void applyDefaultTemplate()
{
  applyTemplate(TMPL_SIMPLE_4CH); // calls eeDirty internally
}
#else
void applyDefaultTemplate()
{
  eeDirty(EE_MODEL);

  for (int i=0; i<NUM_STICKS; i++) {
    MixData *mix = mixAddress(i);
    mix->destCh = i;
    mix->weight = 100;
    mix->srcRaw = MIXSRC_Rud - 1 + channel_order(i+1);
  }
}
#endif


#if defined(SDCARD)
bool isFileAvailable(const char * filename)
{
  return f_stat(filename, 0) == FR_OK;
}
#endif

void modelDefault(uint8_t id)
{
  memset(&g_model, 0, sizeof(g_model));

  applyDefaultTemplate();

#if defined(MAVLINK)
  g_model.mavlink.rc_rssi_scale = 15;
  g_model.mavlink.pc_rssi_en = 1;
#endif
}


#if defined(AUTOSOURCE)
int8_t getMovedSource(GET_MOVED_SOURCE_PARAMS)
{
  int8_t result = 0;
  static tmr10ms_t s_move_last_time = 0;
  static int16_t sourcesStates[NUM_STICKS+NUM_POTS];

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
    if (abs(calibratedStick[i] - sourcesStates[i]) > 512) {
      result = MIXSRC_Rud+i;
      break;
    }
  }

  bool recent = ((tmr10ms_t)(get_tmr10ms() - s_move_last_time) > 10);
  if (recent) {
    result = 0;
  }

  if (result || recent) {
    memcpy(sourcesStates, calibratedStick, sizeof(sourcesStates));
  }

  s_move_last_time = get_tmr10ms();
  return result;
}
#endif

#if defined(FLIGHT_MODES)
uint8_t getFlightMode()
{
  for (uint8_t i=1; i<MAX_FLIGHT_MODES; i++) {
    FlightModeData *phase = &g_model.flightModeData[i];
    if (phase->swtch && getSwitch(phase->swtch)) {
      return i;
    }
  }
  return 0;
}
#endif

trim_t getRawTrimValue(uint8_t phase, uint8_t idx)
{
  FlightModeData *p = flightModeAddress(phase);
  return p->trim[idx];
}

int getTrimValue(uint8_t phase, uint8_t idx)
{
  return getRawTrimValue(getTrimFlightPhase(phase, idx), idx);
}

void setTrimValue(uint8_t phase, uint8_t idx, int trim)
{
  FlightModeData *p = flightModeAddress(phase);
  p->trim[idx] = trim;
  eeDirty(EE_MODEL);
}

uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx)
{
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    if (phase == 0) return 0;
    trim_t trim = getRawTrimValue(phase, idx);
    if (trim <= TRIM_EXTENDED_MAX) return phase;
    uint8_t result = trim-TRIM_EXTENDED_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

#if defined(ROTARY_ENCODERS)
uint8_t getRotaryEncoderFlightPhase(uint8_t idx)
{
  uint8_t phase = mixerCurrentFlightMode;
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    if (phase == 0) return 0;
    int16_t value = flightModeAddress(phase)->rotaryEncoders[idx];
    if (value <= ROTARY_ENCODER_MAX) return phase;
    uint8_t result = value-ROTARY_ENCODER_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

int16_t getRotaryEncoder(uint8_t idx)
{
  return flightModeAddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx];
}

void incRotaryEncoder(uint8_t idx, int8_t inc)
{
  g_rotenc[idx] += inc;
  int16_t *value = &(flightModeAddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx]);
  *value = limit((int16_t)-ROTARY_ENCODER_MAX, (int16_t)(*value + (inc * 10)), (int16_t)+ROTARY_ENCODER_MAX);
  eeDirty(EE_MODEL);
}
#endif

#if defined(GVARS)

#define SET_GVAR_VALUE(idx, phase, value) \
	GVAR_VALUE(idx, phase) = value; \
	eeDirty(EE_MODEL); \
	if (g_model.gvars[idx].popup) { \
		s_gvar_last = idx; \
		s_gvar_timer = GVAR_DISPLAY_TIME; \
	}

uint8_t s_gvar_timer = 0;
uint8_t s_gvar_last = 0;

uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx)
{
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    if (phase == 0) return 0;
    int16_t val = GVAR_VALUE(idx, phase); // TODO phase at the end everywhere to be consistent!
    if (val <= GVAR_MAX) return phase;
    uint8_t result = val-GVAR_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase)
{
  if (GV_IS_GV_VALUE(x, min, max)) {
    int8_t idx = GV_INDEX_CALCULATION(x, max);
    int8_t mul = 1;

    if (idx < 0) {
      idx = -1-idx;
      mul = -1;
    }

    x = GVAR_VALUE(idx, getGVarFlightPhase(phase, idx)) * mul;
  }
  return limit(min, x, max);
}

void setGVarValue(uint8_t idx, int16_t value, int8_t phase)
{
  value = limit((int16_t)-GVAR_LIMIT,value,(int16_t)GVAR_LIMIT); //Limit Gvar value
  phase = getGVarFlightPhase(phase, idx);
  if (GVAR_VALUE(idx, phase) != value) {
    SET_GVAR_VALUE(idx, phase, value);
  }
}

#endif

#if   defined(FRSKY)

/*
ls_telemetry_value_t minTelemValue(uint8_t channel)
{
switch (channel) {
	case TELEM_TIMER1:
	case TELEM_TIMER2:
	return -3600;
	case TELEM_ALT:
	case TELEM_MIN_ALT:
	case TELEM_MAX_ALT:
	case TELEM_GPSALT:
	return -500;
	case TELEM_T1:
	case TELEM_MAX_T1:
	case TELEM_T2:
	case TELEM_MAX_T2:
	return -30;
	case TELEM_ACCx:
	case TELEM_ACCy:
	case TELEM_ACCz:
	return -1000;
	case TELEM_VSPEED:
	return -3000;
	default:
	return 0;
}
}
*/

ls_telemetry_value_t maxTelemValue(uint8_t channel)
{
  switch (channel) {
  case TELEM_FUEL:
  case TELEM_RSSI_TX:
  case TELEM_RSSI_RX:
    return 100;
  case TELEM_HDG:
    return 180;
  default:
    return 255;
  }
}
#endif

getvalue_t convert8bitsTelemValue(uint8_t channel, ls_telemetry_value_t value)
{
  getvalue_t result;
  switch (channel) {
  case TELEM_TIMER1:
  case TELEM_TIMER2:
    result = value * 5;
    break;
#if defined(FRSKY)
  case TELEM_ALT:
  case TELEM_GPSALT:
  case TELEM_MAX_ALT:
  case TELEM_MIN_ALT:
    result = value * 8 - 500;
    break;
  case TELEM_RPM:
  case TELEM_MAX_RPM:
    result = value * 50;
    break;
  case TELEM_T1:
  case TELEM_T2:
  case TELEM_MAX_T1:
  case TELEM_MAX_T2:
    result = (getvalue_t)value - 30;
    break;
  case TELEM_CELL:
  case TELEM_HDG:
  case TELEM_SPEED:
  case TELEM_MAX_SPEED:
    result = value * 2;
    break;
  case TELEM_ASPEED:
  case TELEM_MAX_ASPEED:
    result = value * 20;
    break;
  case TELEM_DIST:
  case TELEM_MAX_DIST:
    result = value * 8;
    break;
  case TELEM_CURRENT:
  case TELEM_POWER:
  case TELEM_MAX_CURRENT:
  case TELEM_MAX_POWER:
    result = value * 5;
    break;
  case TELEM_CONSUMPTION:
    result = value * 100;
    break;
  case TELEM_VSPEED:
    result = ((getvalue_t)value - 125) * 10;
    break;
#endif
  default:
    result = value;
    break;
  }
  return result;
}

#if defined(FRSKY)
FORCEINLINE void convertUnit(getvalue_t & val, uint8_t & unit)
{
  if (IS_IMPERIAL_ENABLE()) {
    if (unit == UNIT_TEMPERATURE) {
      val += 18;
      val *= 115;
      val >>= 6;
    }
    if (unit == UNIT_DIST) {
      // m to ft *105/32
      val = val * 3 + (val >> 2) + (val >> 5);
    }
    if (unit == UNIT_FEET) {
      unit = UNIT_DIST;
    }
    if (unit == UNIT_KTS) {
      // kts to mph
      unit = UNIT_SPEED;
      val = (val * 23) / 20;
    }
  } else {
    if (unit == UNIT_KTS) {
      // kts to km/h
      unit = UNIT_SPEED;
      val = (val * 50) / 27;
    }
  }

  if (unit == UNIT_HDG) {
    unit = UNIT_TEMPERATURE;
  }
}
#endif

#define INAC_STICKS_SHIFT   6
#define INAC_SWITCHES_SHIFT 8
bool inputsMoved()
{
  uint8_t sum = 0;
  for (uint8_t i=0; i<NUM_STICKS; i++)
    sum += anaIn(i) >> INAC_STICKS_SHIFT;
  for (uint8_t i=0; i<NUM_SWITCHES; i++)
    sum += getValue(MIXSRC_FIRST_SWITCH+i) >> INAC_SWITCHES_SHIFT;

  if (abs((int8_t)(sum-inactivity.sum)) > 1) {
    inactivity.sum = sum;
    return true;
  } else {
    return false;
  }
}

void checkBacklight()
{
  static uint8_t tmr10ms ;


  uint8_t x = g_blinkTmr10ms;
  if (tmr10ms != x) {
    tmr10ms = x;
    if (inputsMoved()) {
      inactivity.counter = 0;
      if (g_eeGeneral.backlightMode & e_backlight_mode_sticks)
        backlightOn();
    }

    bool backlightOn = (g_eeGeneral.backlightMode == e_backlight_mode_on || lightOffCounter || isFunctionActive(FUNCTION_BACKLIGHT));
    if (flashCounter) backlightOn = !backlightOn;
    if (backlightOn)
      BACKLIGHT_ON();
    else
      BACKLIGHT_OFF();

  }
}

void backlightOn()
{
  lightOffCounter = ((uint16_t)g_eeGeneral.lightAutoOff*250) << 1;
}

#if MENUS_LOCK == 1
bool readonly = true;
bool readonlyUnlocked()
{
  if (readonly) {
    POPUP_WARNING(STR_MODS_FORBIDDEN);
    return false;
  } else {
    return true;
  }
}
#endif

#if defined(SPLASH)
void doSplash()
{
  uint8_t memtada = 0;

  if (SPLASH_NEEDED()) {
    displaySplash();

    tmr10ms_t curTime = get_tmr10ms() + 10;
    uint8_t contrast = 10;
    lcdSetRefVolt(contrast);

    getADC(); // init ADC array

    inputsMoved();

    tmr10ms_t tgtime = get_tmr10ms() + SPLASH_TIMEOUT;


    while (tgtime > get_tmr10ms()) {

      getADC();

      SIMU_SLEEP(1);

#if defined(FSPLASH)
      // Splash is forced, we can't skip it
      if (!(g_eeGeneral.splashMode & 0x04)) {
#endif

        if (((tgtime/2) == get_tmr10ms()) && (!memtada)) {
          ++memtada;    // Splash duration/2 to play TADA sound
          AUDIO_TADA();
        }

        if (keyDown() || inputsMoved()) return;

#if defined(FSPLASH)
      }
#endif

      if (pwrCheck() == e_power_off) {
        return;
      }

      if (curTime < get_tmr10ms()) {
        curTime += 10;
        if (contrast < g_eeGeneral.contrast) {
          contrast += 1;
          lcdSetRefVolt(contrast);
        }
      }

      checkBacklight();
    }
  }
}
#else
#define Splash()
#define doSplash()
#endif

#define checkFailsafe()

#if defined(GUI)
void checkAll()
{
#if defined(EEPROM_RLC)
  checkLowEEPROM();
#endif

#if defined(MODULE_ALWAYS_SEND_PULSES)
  startupWarningState = STARTUP_WARNING_THROTTLE;
#else
  checkTHR();
  checkSwitches();
  PLAY_MODEL_NAME();
  checkFailsafe();
#endif

  clearKeyEvents();

  START_SILENCE_PERIOD();
}
#endif // GUI

#if defined(MODULE_ALWAYS_SEND_PULSES)
void checkStartupWarnings()
{
  if (startupWarningState < STARTUP_WARNING_DONE) {
    if (startupWarningState == STARTUP_WARNING_THROTTLE)
      checkTHR();
    else
      checkSwitches();
  }
}
#endif

#if defined(EEPROM_RLC)
void checkLowEEPROM()
{
  if (g_eeGeneral.disableMemoryWarning) return;
  if (EeFsGetFree() < 100) {
    ALERT(STR_EEPROMWARN, STR_EEPROMLOWMEM, AU_ERROR);
  }
}
#endif

void checkTHR()
{
  uint8_t thrchn = ((g_model.thrTraceSrc==0) || (g_model.thrTraceSrc>NUM_POTS)) ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1;
  // throttle channel is either the stick according stick mode (already handled in evalInputs)
  // or P1 to P3;
  // in case an output channel is choosen as throttle source (thrTraceSrc>NUM_POTS) we assume the throttle stick is the input
  // no other information available at the moment, and good enough to my option (otherwise too much exceptions...)

#if defined(MODULE_ALWAYS_SEND_PULSES)
  int16_t v = calibratedStick[thrchn];
  if (v<=THRCHK_DEADBAND-1024 || g_model.disableThrottleWarning || pwrCheck()==e_power_off || keyDown()) {
    startupWarningState = STARTUP_WARNING_THROTTLE+1;
  } else {
    calibratedStick[thrchn] = -1024;
    if (thrchn < NUM_STICKS) {
      rawAnas[thrchn] = anas[thrchn] = calibratedStick[thrchn];
    }
    MESSAGE(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_THROTTLE_ALERT);
  }
#else
  if (g_model.disableThrottleWarning) {
    return;
  }

  getADC();

  evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job

  int16_t v = calibratedStick[thrchn];
  if (v <= THRCHK_DEADBAND-1024) {
    return; // prevent warning if throttle input OK
  }

  // first - display warning; also deletes inputs if any have been before
  MESSAGE(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_THROTTLE_ALERT);


  while (1) {

    getADC();

    evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job

    v = calibratedStick[thrchn];

    if (pwrCheck() == e_power_off) {
      break;
    }

    if (keyDown() || v <= THRCHK_DEADBAND-1024) {
      break;
    }

    checkBacklight();

    MYWDT_RESET();
  }
#endif
}

void checkAlarm() // added by Gohst
{
  if (g_eeGeneral.disableAlarmWarning) {
    return;
  }

  if (IS_SOUND_OFF()) {
    ALERT(STR_ALARMSWARN, STR_ALARMSDISABLED, AU_ERROR);
  }
}

void alert(const pm_char * t, const pm_char *s MESSAGE_SOUND_ARG)
{
  MESSAGE(t, s, STR_PRESSANYKEY, sound);


  while(1) {

    if (keyDown()) return;  // wait for key release

    checkBacklight();

    MYWDT_RESET();

    if (pwrCheck() == e_power_off) {
      boardOff(); // turn power off now
    }
  }
}

#if defined(GVARS)
int8_t trimGvar[NUM_STICKS] = { -1, -1, -1, -1 };
#endif

uint8_t checkTrim(uint8_t event)
{
  int8_t k = EVT_KEY_MASK(event) - TRM_BASE;
  if (k>=0 && k<8 && !IS_KEY_BREAK(event)) {
    // LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
    uint8_t idx = CONVERT_MODE((uint8_t)k/2);
    uint8_t phase;
    int before;
    bool thro;


#if defined(GVARS)
    if (TRIM_REUSED(idx)) {
      phase = getGVarFlightPhase(mixerCurrentFlightMode, trimGvar[idx]);
      before = GVAR_VALUE(trimGvar[idx], phase);
      thro = false;
    } else {
      phase = getTrimFlightPhase(mixerCurrentFlightMode, idx);
      before = getRawTrimValue(phase, idx);
      thro = (idx==THR_STICK && g_model.thrTrim);
    }
#else
    phase = getTrimFlightPhase(mixerCurrentFlightMode, idx);
    before = getRawTrimValue(phase, idx);
    thro = (idx==THR_STICK && g_model.thrTrim);
#endif
    int8_t trimInc = g_model.trimInc + 1;
    int8_t v = (trimInc==-1) ? min(32, abs(before)/4+1) : (1 << trimInc); // TODO flash saving if (trimInc < 0)
    if (thro) v = 4; // if throttle trim and trim trottle then step=4
    int16_t after = (k&1) ? before + v : before - v;   // positive = k&1
    bool beepTrim = false;
    for (int16_t mark=TRIM_MIN; mark<=TRIM_MAX; mark+=TRIM_MAX) {
      if ((mark!=0 || !thro) && ((mark!=TRIM_MIN && after>=mark && before<mark) || (mark!=TRIM_MAX && after<=mark && before>mark))) {
        after = mark;
        beepTrim = (mark == 0 ? 1 : 2);
      }
    }

    if ((before<after && after>TRIM_MAX) || (before>after && after<TRIM_MIN)) {
      if (!g_model.extendedTrims || TRIM_REUSED(idx)) after = before;
    }

    if (after < TRIM_EXTENDED_MIN) {
      after = TRIM_EXTENDED_MIN;
    }
    if (after > TRIM_EXTENDED_MAX) {
      after = TRIM_EXTENDED_MAX;
    }

#if defined(GVARS)
    if (TRIM_REUSED(idx)) {
      SET_GVAR_VALUE(trimGvar[idx], phase, after);
    } else
#endif
    {
      setTrimValue(phase, idx, after);
    }

#if defined(AUDIO)
    // toneFreq higher/lower according to trim position
    // limit the frequency, range -125 to 125 = toneFreq: 19 to 101
    if (after > TRIM_MAX)
      after = TRIM_MAX;
    if (after < TRIM_MIN)
      after = TRIM_MIN;
    after >>= 2;
    after += 60;
#endif

    if (beepTrim) {
      if (beepTrim == 1) {
        AUDIO_TRIM_MIDDLE(after);
        pauseEvents(event);
      } else {
        AUDIO_TRIM_END(after);
        killEvents(event);
      }
    } else {
      AUDIO_TRIM(event, after);
    }
    return 0;
  }
  return event;
}

uint16_t s_anaFilt[NUMBER_ANALOG];

#if defined(CPUM2560)
// #define STARTADCONV (ADCSRA  = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADSC) | (1 << ADIE))
// G: Note that the above would have set the ADC prescaler to 128, equating to
// 125KHz sample rate. We now sample at 500KHz, with oversampling and other
// filtering options to produce 11-bit results.
uint16_t BandGap = 2040 ;
#endif

#if defined(JITTER_MEASURE)
JitterMeter<uint16_t> rawJitter[NUMBER_ANALOG];
JitterMeter<uint16_t> avgJitter[NUMBER_ANALOG];
tmr10ms_t jitterResetTime = 0;
#define JITTER_MEASURE_ACTIVE()   (menuHandlers[menuLevel] == menuGeneralDiagAna)
#endif  // defined(JITTER_MEASURE)

uint16_t anaIn(uint8_t chan)
{
#if defined(TELEMETRY_MOD_14051) || defined(TELEMETRY_MOD_14051_SWAPPED)
  static const pm_char crossAna[] PROGMEM = {3,1,2,0,4,5,6,0/* shouldn't be used */,TX_VOLTAGE};
#elif defined REV_EVO_V1
  static const pm_char crossAna[] PROGMEM = {2,3,0,1,4,5}; // Hardwired Pots / connectors.
#else
  static const pm_char crossAna[] PROGMEM = {3,1,2,0,4,5,6,7};
#endif

#if defined(FRSKY_STICKS)
  volatile uint16_t temp = s_anaFilt[pgm_read_byte_near(crossAna+chan)];  // volatile saves here 40 bytes; maybe removed for newer AVR when available
  if (chan < NUM_STICKS && (g_eeGeneral.stickReverse & (1 << chan))) {
    temp = 2048 - temp;
  }
  return temp;
#else
  volatile uint16_t *p = &s_anaFilt[pgm_read_byte_near(crossAna+chan)];
  return *p;
#endif
}

uint8_t g_vbat100mV = 0;
uint16_t lightOffCounter;
uint8_t flashCounter = 0;

uint16_t sessionTimer;
uint16_t s_timeCumThr;    // THR in 1/16 sec
uint16_t s_timeCum16ThrP; // THR% in 1/16 sec

uint8_t  trimsCheckTimer = 0;


void flightReset()
{
  // we don't reset the whole audio here (the tada.wav would be cut, if a prompt is queued before FlightReset, it should be played)
  // TODO check if the vario / background music are stopped correctly if switching to a model which doesn't have these functions enabled

  if (!IS_MANUAL_RESET_TIMER(0)) {
    timerReset(0);
  }

#if TIMERS > 1
  if (!IS_MANUAL_RESET_TIMER(1)) {
    timerReset(1);
  }
#endif

#if TIMERS > 2
  if (!IS_MANUAL_RESET_TIMER(2)) {
    timerReset(2);
  }
#endif

#if defined(FRSKY)
  telemetryReset();
#endif

  s_mixer_first_run_done = false;

  START_SILENCE_PERIOD();

  RESET_THR_TRACE();
}

#if defined(THRTRACE)
uint8_t  s_traceBuf[MAXTRACE];
uint8_t  s_traceWr;
int16_t  s_traceCnt;
uint8_t  s_cnt_10s;
uint16_t s_cnt_samples_thr_10s;
uint16_t s_sum_samples_thr_10s;
#endif

FORCEINLINE void evalTrims()
{
  uint8_t phase = mixerCurrentFlightMode;
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    // do trim -> throttle trim if applicable
    int16_t trim = getTrimValue(phase, i);
    if (i==THR_STICK && g_model.thrTrim) {
      int16_t trimMin = g_model.extendedTrims ? TRIM_EXTENDED_MIN : TRIM_MIN;
      trim = (((g_model.throttleReversed)?(int32_t)(trim+trimMin):(int32_t)(trim-trimMin)) * (RESX-anas[i])) >> (RESX_SHIFT+1);
    }
    if (trimsCheckTimer > 0) {
      trim = 0;
    }

    trims[i] = trim*2;
  }
}

uint8_t s_mixer_first_run_done = false;

void doMixerCalculations()
{
  static tmr10ms_t lastTMR = 0;

  tmr10ms_t tmr10ms = get_tmr10ms();
  uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
  // handle tick10ms overrun
  // correct overflow handling costs a lot of code; happens only each 11 min;
  // therefore forget the exact calculation and use only 1 instead; good compromise

  lastTMR = tmr10ms;

  getADC();

  getSwitchesPosition(!s_mixer_first_run_done);

  adcPrepareBandgap();

  evalMixes(tick10ms);

  // Bandgap has had plenty of time to settle...
  getADC_bandgap();

  if (tick10ms) {

#if !defined(ACCURAT_THROTTLE_TIMER)
    //  code cost is about 16 bytes for higher throttle accuracy for timer
    //  would not be noticable anyway, because all version up to this change had only 16 steps;
    //  now it has already 32  steps; this define would increase to 128 steps
#define ACCURAT_THROTTLE_TIMER
#endif

    /* Throttle trace */
    int16_t val;

    if (g_model.thrTraceSrc > NUM_POTS) {
      uint8_t ch = g_model.thrTraceSrc-NUM_POTS-1;
      val = channelOutputs[ch];

      LimitData *lim = limitAddress(ch);
      int16_t gModelMax = LIMIT_MAX_RESX(lim);
      int16_t gModelMin = LIMIT_MIN_RESX(lim);

      if (lim->revert)
        val = -val + gModelMax;
      else
        val = val - gModelMin;

#if defined(PPM_LIMITS_SYMETRICAL)
      if (lim->symetrical) {
        val -= calc1000toRESX(lim->offset);
      }
#endif

      gModelMax -= gModelMin; // we compare difference between Max and Mix for recaling needed; Max and Min are shifted to 0 by default
      // usually max is 1024 min is -1024 --> max-min = 2048 full range

#ifdef ACCURAT_THROTTLE_TIMER
      if (gModelMax!=0 && gModelMax!=2048) val = (int32_t) (val << 11) / (gModelMax); // rescaling only needed if Min, Max differs
#else
      // @@@ open.20.fsguruh  optimized calculation; now *8 /8 instead of 10 base; (*16/16 already cause a overrun; unsigned calculation also not possible, because v may be negative)
      gModelMax+=255; // force rounding up --> gModelMax is bigger --> val is smaller
      gModelMax >>= (10-2);

      if (gModelMax!=0 && gModelMax!=8) {
        val = (val << 3) / gModelMax; // rescaling only needed if Min, Max differs
      }
#endif

      if (val<0) val=0;  // prevent val be negative, which would corrupt throttle trace and timers; could occur if safetyswitch is smaller than limits
    } else {
      val = RESX + (g_model.thrTraceSrc == 0 ? rawAnas[THR_STICK] : calibratedStick[g_model.thrTraceSrc+NUM_STICKS-1]);
    }

#if defined(ACCURAT_THROTTLE_TIMER)
    val >>= (RESX_SHIFT-6); // calibrate it (resolution increased by factor 4)
#else
    val >>= (RESX_SHIFT-4); // calibrate it
#endif

    evalTimers(val, tick10ms);

    static uint8_t  s_cnt_100ms;
    static uint8_t  s_cnt_1s;
    static uint8_t  s_cnt_samples_thr_1s;
    static uint16_t s_sum_samples_thr_1s;

    s_cnt_samples_thr_1s++;
    s_sum_samples_thr_1s+=val;

    if ((s_cnt_100ms += tick10ms) >= 10) { // 0.1sec
      s_cnt_100ms -= 10;
      s_cnt_1s += 1;

      logicalSwitchesTimerTick();
      checkTrainerSignalWarning();

      if (s_cnt_1s >= 10) { // 1sec
        s_cnt_1s -= 10;
        sessionTimer += 1;

        struct t_inactivity *ptrInactivity = &inactivity;
        FORCE_INDIRECT(ptrInactivity) ;
        ptrInactivity->counter++;
        if ((((uint8_t)ptrInactivity->counter)&0x07)==0x01 && g_eeGeneral.inactivityTimer && g_vbat100mV>50 && ptrInactivity->counter > ((uint16_t)g_eeGeneral.inactivityTimer*60))
          AUDIO_INACTIVITY();

#if defined(AUDIO)
        if (mixWarning & 1) if ((sessionTimer&0x03)==0) AUDIO_MIX_WARNING(1);
        if (mixWarning & 2) if ((sessionTimer&0x03)==1) AUDIO_MIX_WARNING(2);
        if (mixWarning & 4) if ((sessionTimer&0x03)==2) AUDIO_MIX_WARNING(3);
#endif

#if defined(ACCURAT_THROTTLE_TIMER)
        val = s_sum_samples_thr_1s / s_cnt_samples_thr_1s;
        s_timeCum16ThrP += (val>>3);  // s_timeCum16ThrP would overrun if we would store throttle value with higher accuracy; therefore stay with 16 steps
        if (val) s_timeCumThr += 1;
        s_sum_samples_thr_1s>>=2;  // correct better accuracy now, because trace graph can show this information; in case thrtrace is not active, the compile should remove this
#else
        val = s_sum_samples_thr_1s / s_cnt_samples_thr_1s;
        s_timeCum16ThrP += (val>>1);
        if (val) s_timeCumThr += 1;
#endif

#if defined(THRTRACE)
        // throttle trace is done every 10 seconds; Tracebuffer is adjusted to screen size.
        // in case buffer runs out, it wraps around
        // resolution for y axis is only 32, therefore no higher value makes sense
        s_cnt_samples_thr_10s += s_cnt_samples_thr_1s;
        s_sum_samples_thr_10s += s_sum_samples_thr_1s;

        if (++s_cnt_10s >= 10) { // 10s
          s_cnt_10s -= 10;
          val = s_sum_samples_thr_10s / s_cnt_samples_thr_10s;
          s_sum_samples_thr_10s = 0;
          s_cnt_samples_thr_10s = 0;

          s_traceBuf[s_traceWr++] = val;
          if (s_traceWr >= MAXTRACE) s_traceWr = 0;
          if (s_traceCnt >= 0) s_traceCnt++;
        }
#endif

        s_cnt_samples_thr_1s = 0;
        s_sum_samples_thr_1s = 0;
      }
    }

#if defined(PXX) || defined(DSM2)
    static uint8_t countRangecheck = 0;
      if (moduleFlag != MODULE_NORMAL_MODE) {
        if (++countRangecheck >= 250) {
          countRangecheck = 0;
          AUDIO_PLAY(AU_FRSKY_CHEEP);
        }
      }
#endif

  }

  s_mixer_first_run_done = true;
}

void OpenAVRcStart()
{
  doSplash();
#if defined(SDCARD) && !defined(SIMU)
  sdMountPoll();
#endif

#if defined(DEBUG_TRACE_BUFFER)
  trace_event(trace_start, 0x12345678);
#endif

#if defined(GUI)
  checkAlarm();
  checkAll();
  if (g_eeGeneral.chkSum != evalChkSum()) {
    chainMenu(menuFirstCalib);
  }
#endif

}

#if defined(CPUM2560)
void OpenAVRcClose()
{
  AUDIO_BYE();

#if defined(FRSKY)
  // TODO needed? telemetryEnd();
#endif


#if defined(SDCARD)
  closeLogs();
#endif

#if defined(HAPTIC)
  hapticOff();
#endif

  saveTimers();

  if (s_eeDirtyMsk & EE_MODEL) {
    displayPopup(STR_SAVEMODEL);
  }

  g_eeGeneral.unexpectedShutdown = 0;

  eeDirty(EE_GENERAL);
  eeCheck(true);


#if defined(SDCARD)
  sdDone();
#endif
}
#endif

void checkBattery()
{
  static uint8_t counter = 0;
#if defined(GUI)
  // TODO not the right menu I think ...
  if (menuHandlers[menuLevel] == menuGeneralDiagAna) {
    g_vbat100mV = 0;
    counter = 0;
  }
#endif
  if (counter-- == 0) {
    counter = 10;
    int32_t instant_vbat = anaIn(TX_VOLTAGE);

#if defined(CPUM2560) && defined(REV_EVO_V1)
    instant_vbat *= 4L * BandGap;
    instant_vbat /= (4095L * 100L);
    instant_vbat += 2L; // No Calibration Allowed.
    // Schottky Diode drops 0.2V before a potential divider which reduces the input to the ADC by 1/4.
#elif defined(CPUM2560)
    instant_vbat = (instant_vbat*1112 + instant_vbat*g_eeGeneral.txVoltageCalibration + (BandGap<<2)) / (BandGap<<3);
#endif

    static uint8_t  s_batCheck;
    static uint16_t s_batSum;

#if defined(VOICE)
    s_batCheck += 8;
#else
    s_batCheck += 32;
#endif

    s_batSum += instant_vbat;

    if (g_vbat100mV == 0) {
      g_vbat100mV = instant_vbat;
      s_batSum = 0;
      s_batCheck = 0;
    }
#if defined(VOICE)
    else if (!(s_batCheck & 0x3f)) {
#else
    else if (s_batCheck == 0) {
#endif
      g_vbat100mV = s_batSum / 8;
      s_batSum = 0;
#if defined(VOICE)
      if (s_batCheck != 0) {
        // no alarms
      } else
#endif
        if (IS_TXBATT_WARNING() && g_vbat100mV>50) { // No Audio Alarm if TX Battery < VCC.
          AUDIO_TX_BATTERY_LOW();
        }
    }
  }
}

volatile uint8_t g_tmr16KHz; //continuous timer 16ms (16MHz/1024/256) -- 8-bit counter overflow
ISR(TIMER_16KHZ_VECT, ISR_NOBLOCK)
{
  g_tmr16KHz++; // gruvin: Not 16KHz. Overflows occur at 61.035Hz (1/256th of 15.625KHz)
  // to give *16.384ms* intervals. Kind of matters for accuracy elsewhere. ;)
  // g_tmr16KHz is used to software-construct a 16-bit timer
  // from TIMER-0 (8-bit). See getTmr16KHz, below.
}

uint16_t getTmr16KHz()
{
#if defined(SIMU)
  uint16_t simu_tmr16 = get_tmr10ms() * 160;
  return simu_tmr16;
#else
  while(1) {
    uint8_t hb  = g_tmr16KHz;
    uint8_t lb  = COUNTER_16KHZ;
    if(hb-g_tmr16KHz==0) return (hb<<8)|lb;
  }
#endif
}



// Clocks every 10ms
ISR(TIMER_10MS_VECT, ISR_NOBLOCK)
{
  // without correction we are 0,16% too fast; that mean in one hour we are 5,76Sek too fast; we do not like that
  static uint8_t accuracyWarble; // because 16M / 1024 / 100 = 156.25. we need to correct the fault; no start value needed

#if defined(AUDIO)
  AUDIO_HEARTBEAT();
#endif

#if defined(BUZZER)
  BUZZER_HEARTBEAT();
#endif

#if defined(HAPTIC)
  HAPTIC_HEARTBEAT();
#endif

  SIMU_PROCESSEVENTS;

  per10ms();

  uint8_t bump = (!(++accuracyWarble & 0x03)) ? 157 : 156;
  TIMER_10MS_COMPVAL += bump;
}

#if defined(DSM2_SERIAL)
FORCEINLINE void DSM2_USART_vect()
{
  UDR0 = *((uint16_t*)pulses2MHzRPtr); // transmit next byte

  pulses2MHzRPtr += sizeof(uint16_t);

  if (pulses2MHzRPtr == pulses2MHzWPtr) { // if reached end of DSM2 data buffer ...
    UCSRB_N(TLM_USART) &= ~(1 << UDRIE_N(TLM_USART)); // disable UDRE interrupt
  }
}
#endif

#if !defined(SIMU)

#if defined (FRSKY)

FORCEINLINE void FRSKY_USART_vect()
{
  if (frskyTxBufferCount > 0) {
    UDR_N(TLM_USART) = frskyTxBuffer[--frskyTxBufferCount];
  } else {
    UCSRB_N(TLM_USART) &= ~(1 << UDRIE_N(TLM_USART)); // disable UDRE interrupt
  }
}

// USART0/1 Transmit Data Register Emtpy ISR
ISR(USART_UDRE_vect_N(TLM_USART))
{
#if defined(FRSKY) && defined(DSM2_SERIAL)
  if (IS_DSM2_PROTOCOL(g_model.protocol)) { // TODO not s_current_protocol?
    DSM2_USART_vect();
  } else {
    FRSKY_USART_vect();
  }
#elif defined(FRSKY)
  FRSKY_USART_vect();
#else
  DSM2_USART_vect();
#endif
}
#endif
#endif

#define INSTANT_TRIM_MARGIN 15 /* around 1.5% */

void instantTrim()
{
  evalInputs(e_perout_mode_notrainer);

  for (uint8_t stick=0; stick<NUM_STICKS; stick++) {
    if (stick!=THR_STICK) {
      // don't instant trim the throttle stick
      uint8_t trim_phase = getTrimFlightPhase(mixerCurrentFlightMode, stick);
      int16_t delta = anas[stick];
      if (abs(delta) >= INSTANT_TRIM_MARGIN) {
        int16_t trim = limit<int16_t>(TRIM_EXTENDED_MIN, (delta + trims[stick]) / 2, TRIM_EXTENDED_MAX);
        setTrimValue(trim_phase, stick, trim);
      }
    }
  }
  eeDirty(EE_MODEL);
  AUDIO_WARNING2();
}

void copySticksToOffset(uint8_t ch)
{
  int32_t zero = (int32_t)channelOutputs[ch];

  evalFlightModeMixes(e_perout_mode_nosticks+e_perout_mode_notrainer, 0);
  int32_t val = chans[ch];
  LimitData *ld = limitAddress(ch);
  limit_min_max_t lim = LIMIT_MIN(ld);
  if (val < 0) {
    val = -val;
    lim = LIMIT_MIN(ld);
  }
  zero = (zero*25600 - val*lim) / (26214-val);
  ld->offset = (ld->revert ? -zero : zero);
  eeDirty(EE_MODEL);
}

void copyTrimsToOffset(uint8_t ch)
{
  int16_t zero;

  evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  zero = applyLimits(ch, chans[ch]);

  evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

  int16_t output = applyLimits(ch, chans[ch]) - zero;
  int16_t v = g_model.limitData[ch].offset;
  if (g_model.limitData[ch].revert) output = -output;
  v += output;
  g_model.limitData[ch].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire

  eeDirty(EE_MODEL);
}

void moveTrimsToOffsets() // copy state of 3 primary to subtrim
{
  int16_t zeros[NUM_CHNOUT];

  evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    zeros[i] = applyLimits(i, chans[i]);
  }

  evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    int16_t output = applyLimits(i, chans[i]) - zeros[i];
    int16_t v = g_model.limitData[i].offset;
    if (g_model.limitData[i].revert) output = -output;
    v += output;
    g_model.limitData[i].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire
  }

  // reset all trims, except throttle (if throttle trim)
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    if (i!=THR_STICK || !g_model.thrTrim) {
      int16_t original_trim = getTrimValue(mixerCurrentFlightMode, i);
      for (uint8_t phase=0; phase<MAX_FLIGHT_MODES; phase++) {
        trim_t trim = getRawTrimValue(phase, i);
        if (trim <= TRIM_EXTENDED_MAX)
          setTrimValue(phase, i, trim - original_trim);
      }
    }
  }

  eeDirty(EE_MODEL);
  AUDIO_WARNING2();
}

#if defined(ROTARY_ENCODERS)
volatile rotenc_t g_rotenc[ROTARY_ENCODERS] = {0};
#elif defined(ROTARY_ENCODER_NAVIGATION)
volatile rotenc_t g_rotenc[1] = {0};
#endif

#if !defined(SIMU)
#define STACKPTR     _SFR_IO16(0x3D)
extern unsigned char __bss_end ;
void stackPaint()
{
  // Init Stack while interrupts are disabled
  unsigned char *p ;
  unsigned char *q ;

  p = (unsigned char *) STACKPTR ;
  q = &__bss_end ;
  p -= 2 ;
  while ( p > q ) {
    *p-- = 0x55 ;
  }
}
#endif

uint16_t stackAvailable()
{
#if defined(SIMU)
  return 1800;
#else
  unsigned char *p ;

  p = &__bss_end + 1 ;
  while ( *p++ == 0x55 );
  return p - &__bss_end ;
#endif
}


uint16_t freeRam()
{
#if defined(SIMU)
  return 2000;
#else
  extern int __heap_start, *__brkval;
  uint16_t v;
  return (uint16_t) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
#endif
}

#if defined(CPUM2560)
#define OpenAVRc_INIT_ARGS const uint8_t mcusr
#endif

void OpenAVRcInit(OpenAVRc_INIT_ARGS)
{

  eeReadAll();



#if MENUS_LOCK == 1
  getMovedSwitch();
  if (TRIMS_PRESSED() && g_eeGeneral.switchUnlockStates==switches_states) {
    readonly = false;
  }
#endif

#if defined(VOICE)
  setVolume(g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF);
#endif

#if defined(RTCLOCK)
  rtcInit();
#endif

  if (g_eeGeneral.backlightMode != e_backlight_mode_off) backlightOn(); // on Tx start turn the light on

  if (UNEXPECTED_SHUTDOWN()) {
    // is done above on ARM
    unexpectedShutdown = 1;
  } else {
    OpenAVRcStart();
  }

#if defined(CPUM2560)
  if (!g_eeGeneral.unexpectedShutdown) {
    g_eeGeneral.unexpectedShutdown = 1;
    eeDirty(EE_GENERAL);
  }
#endif

#if defined(GUI)
  lcdSetContrast();
#endif
  backlightOn();



  doMixerCalculations();

  startPulses(PROTOCMD_INIT);

#if !defined(SIMU)
  wdt_enable(WDTO_500MS); // Enable watchdog
#endif
}

#if !defined(SIMU)
int main(void)
{
#else
int simumain(void)
{
  simu_off = false;
#endif
  // G: The WDT remains active after a WDT reset -- at maximum clock speed. So it's
  // important to disable it before commencing with system initialisation (or
  // we could put a bunch more MYWDT_RESET()s in. But I don't like that approach
  // during boot up.)
#if !defined(SIMU)
#if defined(CPUM2560)
  uint8_t mcusr = MCUSR; // save the WDT (etc) flags
  MCUSR = 0; // must be zeroed before disabling the WDT
  MCUCR = 0x80 ;   // Disable JTAG port that can interfere with POT3
  MCUCR = 0x80 ;   // Must be done twice
#endif
  wdt_disable();
#endif //SIMU
  boardInit();

#if defined(GUI)
  lcdInit();
#endif

#if !defined(SIMU)
  stackPaint();
#endif

#if defined(GUI)
  menuHandlers[0] = menuMainView;
#if MENUS_LOCK != 2 /*no menus*/
  menuHandlers[1] = menuModelSelect;
#endif
#endif

#if defined(GUI)
  lcdSetRefVolt(25);
#endif


  sei(); // interrupts needed for telemetryInit and eeReadAll.

#if defined(FRSKY) && !defined(DSM2_SERIAL)
  telemetryInit();
#endif

#if defined(DSM2_SERIAL) && !defined(FRSKY)
  DSM2_Init();
#endif

#ifdef JETI
  JETI_Init();
#endif

#ifdef ARDUPILOT
  ARDUPILOT_Init();
#endif

#ifdef NMEA
  NMEA_Init();
#endif

#ifdef MAVLINK
  MAVLINK_Init();
#endif

#ifdef MENU_ROTARY_SW
  init_rotary_sw();
#endif

  OpenAVRcInit(mcusr);


#if !defined(SIMU)
  while (1) {
#else // Simu main loop function
  SimuMainLoop();
} // Close simumain()
void SimuMainLoop(void) // Create loop function
{
  simu_mainloop_is_runing = true;
#endif //SIMU
#if defined(CPUM2560)
    uint8_t shutdown_state = 0;
    if ((shutdown_state=pwrCheck()) > e_power_trainer)
#if !defined(SIMU)
      break;
#else
      shutDownSimu();
#endif
#endif

    perMain();

    if (heartbeat == HEART_WDT_CHECK) {
      MYWDT_RESET();
      heartbeat = 0;
    }
#if defined (SIMU)
    simu_mainloop_is_runing = false;
#endif
  }

#if defined(SIMU)
  void shutDownSimu()
  {
    simu_shutDownSimu_is_runing = true;
#endif

#if defined(CPUM2560)
    // Time to switch off
    lcdClear();
    displayPopup(STR_SHUTDOWN);
    SIMU_SLEEP(200);
    OpenAVRcClose();
    SIMU_SLEEP(200);
    lcdClear() ;
    lcdRefresh() ;
    boardOff(); // Only turn power off if necessary
    wdt_disable();
#if !defined(SIMU)
    while(1); // never return from main() - there is no code to return back, if any delays occurs in physical power it does dead loop.
#endif
#endif
  }

