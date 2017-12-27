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

CustomFunctionsContext modelFunctionsContext = { 0 };


#if defined(VOICE)
PLAY_FUNCTION(playValue, source_t idx)
{
  if (IS_FAI_FORBIDDEN(idx))
    return;

  if (idx == MIXSRC_NONE)
    return;

  getvalue_t val = getValue(idx);

  switch (idx) {
  case MIXSRC_FIRST_TELEM+TELEM_TX_VOLTAGE-1:
    PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
    break;
  case MIXSRC_FIRST_TELEM+TELEM_TIMER1-1:
  case MIXSRC_FIRST_TELEM+TELEM_TIMER2-1:
    PLAY_DURATION(val, 0);
    break;
#if defined(FRSKY)
  case MIXSRC_FIRST_TELEM+TELEM_RSSI_TX-1:
  case MIXSRC_FIRST_TELEM+TELEM_RSSI_RX-1:
    PLAY_NUMBER(val, 1+UNIT_DB, 0);
    break;
  case MIXSRC_FIRST_TELEM+TELEM_MIN_A1-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_A2-1:
    idx -= TELEM_MIN_A1-TELEM_A1;
  // no break
  case MIXSRC_FIRST_TELEM+TELEM_A1-1:
  case MIXSRC_FIRST_TELEM+TELEM_A2-1:
    if (TELEMETRY_STREAMING()) {
      idx -= (MIXSRC_FIRST_TELEM+TELEM_A1-1);
      uint8_t att = 0;
      int16_t converted_value =  div10_and_round(applyChannelRatio(idx, val));;
      if (ANA_CHANNEL_UNIT(idx) < UNIT_RAW) {
        att = PREC1;
      }
      PLAY_NUMBER(converted_value, 1+ANA_CHANNEL_UNIT(idx), att);
    }
    break;
  case MIXSRC_FIRST_TELEM+TELEM_CELL-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_CELL-1:
    PLAY_NUMBER(div10_and_round(val), 1+UNIT_VOLTS, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_VFAS-1:
  case MIXSRC_FIRST_TELEM+TELEM_CELLS_SUM-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_CELLS_SUM-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_VFAS-1:
    PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_CURRENT-1:
  case MIXSRC_FIRST_TELEM+TELEM_MAX_CURRENT-1:
    PLAY_NUMBER(val, 1+UNIT_AMPS, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_ACCx-1:
  case MIXSRC_FIRST_TELEM+TELEM_ACCy-1:
  case MIXSRC_FIRST_TELEM+TELEM_ACCz-1:
    PLAY_NUMBER(div10_and_round(val), 1+UNIT_G, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_VSPEED-1:
    PLAY_NUMBER(div10_and_round(val), 1+UNIT_METERS_PER_SECOND, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_ASPEED-1:
  case MIXSRC_FIRST_TELEM+TELEM_MAX_ASPEED-1:
    PLAY_NUMBER(val/10, 1+UNIT_KTS, 0);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_CONSUMPTION-1:
    PLAY_NUMBER(val, 1+UNIT_MAH, 0);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_POWER-1:
    PLAY_NUMBER(val, 1+UNIT_WATTS, 0);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_ALT-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_ALT-1:
  case MIXSRC_FIRST_TELEM+TELEM_MAX_ALT-1:
#if defined(WS_HOW_HIGH)
    if (IS_IMPERIAL_ENABLE() && IS_USR_PROTO_WS_HOW_HIGH())
      PLAY_NUMBER(val, 1+UNIT_FEET, 0);
    else
#endif
      PLAY_NUMBER(val, 1+UNIT_DIST, 0);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_RPM-1:
  case MIXSRC_FIRST_TELEM+TELEM_MAX_RPM-1: {
    getvalue_t rpm = val;
    if (rpm > 100)
      rpm = 10 * div10_and_round(rpm);
    if (rpm > 1000)
      rpm = 10 * div10_and_round(rpm);
    PLAY_NUMBER(rpm, 1+UNIT_RPMS, 0);
    break;
  }

  case MIXSRC_FIRST_TELEM+TELEM_HDG-1:
    PLAY_NUMBER(val, 1+UNIT_HDG, 0);
    break;

  default: {
    uint8_t unit = 1;
    if (idx < MIXSRC_GVAR1)
      val = calcRESXto100(val);
    if (idx >= MIXSRC_FIRST_TELEM+TELEM_ALT-1 && idx <= MIXSRC_FIRST_TELEM+TELEM_GPSALT-1)
      unit = idx - (MIXSRC_FIRST_TELEM+TELEM_ALT-1);
    else if (idx >= MIXSRC_FIRST_TELEM+TELEM_MAX_T1-1 && idx <= MIXSRC_FIRST_TELEM+TELEM_MAX_DIST-1)
      unit = 3 + idx - (MIXSRC_FIRST_TELEM+TELEM_MAX_T1-1);

    unit = pgm_read_byte_near(bchunit_ar+unit);
    PLAY_NUMBER(val, unit == UNIT_RAW ? 0 : unit+1, 0);
    break;
  }
#else
  default: {
    PLAY_NUMBER(val, 0, 0);
    break;
  }
#endif
  }
}
#endif



#define functions g_model.customFn
#define functionsContext modelFunctionsContext
void evalFunctions()
{
  MASK_FUNC_TYPE newActiveFunctions  = 0;
  MASK_CFN_TYPE  newActiveSwitches = 0;

#define PLAY_INDEX   (i+1)

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  static rotenc_t rePreviousValues[ROTARY_ENCODERS];
#endif

#if defined(OVERRIDE_CHANNEL_FUNCTION)
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    safetyCh[i] = OVERRIDE_CHANNEL_UNDEFINED;
  }
#endif

#if defined(GVARS)
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    trimGvar[i] = -1;
  }
#endif

  for (uint8_t i=0; i<NUM_CFN; i++) {
    const CustomFunctionData * cfn = &functions[i];
    int8_t swtch = CFN_SWITCH(cfn);
    if (swtch) {
      MASK_CFN_TYPE switch_mask = ((MASK_CFN_TYPE)1 << i);

      bool active = getSwitch(swtch);

      if (HAS_ENABLE_PARAM(CFN_FUNC(cfn))) {
        active &= (bool)CFN_ACTIVE(cfn);
      }

      if (active || IS_PLAY_BOTH_FUNC(CFN_FUNC(cfn))) {

        switch (CFN_FUNC(cfn)) {

#if defined(OVERRIDE_CHANNEL_FUNCTION)
        case FUNC_OVERRIDE_CHANNEL:
          safetyCh[CFN_CH_INDEX(cfn)] = CFN_PARAM(cfn);
          break;
#endif

        case FUNC_TRAINER: {
          uint8_t mask = 0x0f;
          if (CFN_CH_INDEX(cfn) > 0) {
            mask = (1<<(CFN_CH_INDEX(cfn)-1));
          }
          newActiveFunctions |= mask;
          break;
        }

        case FUNC_INSTANT_TRIM:
          newActiveFunctions |= (1 << FUNCTION_INSTANT_TRIM);
          if (!isFunctionActive(FUNCTION_INSTANT_TRIM)) {
#if defined(GUI)
            if (menuHandlers[0] == menuMainView
#if defined(FRSKY)
                || menuHandlers[0] == menuTelemetryFrsky
#endif
               )
#endif
            {
              instantTrim();
            }
          }
          break;

        case FUNC_RESET:
          switch (CFN_PARAM(cfn)) {
          case FUNC_RESET_TIMER1:
          case FUNC_RESET_TIMER2:
            timerReset(CFN_PARAM(cfn));
            break;
          case FUNC_RESET_FLIGHT:
            flightReset();
            break;
#if defined(FRSKY)
          case FUNC_RESET_TELEMETRY:
            telemetryResetValue();
            break;
#endif
#if ROTARY_ENCODERS > 0
          case FUNC_RESET_ROTENC1:
#if ROTARY_ENCODERS > 1
          case FUNC_RESET_ROTENC2:
#endif
            g_rotenc[CFN_PARAM(cfn)-FUNC_RESET_ROTENC1] = 0;
            break;
#endif
          }
          break;

#if defined(GVARS)
        case FUNC_ADJUST_GVAR:
          if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_CONSTANT) {
            SET_GVAR(CFN_GVAR_INDEX(cfn), CFN_PARAM(cfn), mixerCurrentFlightMode);
          } else if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_GVAR) {
            SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_PARAM(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_PARAM(cfn))), mixerCurrentFlightMode);
          } else if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_INC) {
            if (!(functionsContext.activeSwitches & switch_mask)) {
              SET_GVAR(CFN_GVAR_INDEX(cfn), limit(-GVAR_MAX, GVAR_VALUE(CFN_GVAR_INDEX(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(cfn))) + (CFN_PARAM(cfn) ? +1 : -1), GVAR_MAX), mixerCurrentFlightMode);
            }
          } else if (CFN_PARAM(cfn) >= MIXSRC_TrimRud && CFN_PARAM(cfn) <= MIXSRC_TrimAil) {
            trimGvar[CFN_PARAM(cfn)-MIXSRC_TrimRud] = CFN_GVAR_INDEX(cfn);
          }
#if defined(ROTARY_ENCODERS)
          else if (CFN_PARAM(cfn) >= MIXSRC_REa && CFN_PARAM(cfn) < MIXSRC_TrimRud) {
            int8_t scroll = rePreviousValues[CFN_PARAM(cfn)-MIXSRC_REa] - (g_rotenc[CFN_PARAM(cfn)-MIXSRC_REa] / ROTARY_ENCODER_GRANULARITY);
            if (scroll) {
              SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_GVAR_INDEX(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(cfn))) + scroll, mixerCurrentFlightMode);
            }
          }
#endif
          else {
            SET_GVAR(CFN_GVAR_INDEX(cfn), calcRESXto100(getValue(CFN_PARAM(cfn))), mixerCurrentFlightMode);
          }
          break;
#endif


#if defined(VOICE)
        case FUNC_PLAY_SOUND:
        case FUNC_PLAY_TRACK:
        case FUNC_PLAY_BOTH:
        case FUNC_PLAY_VALUE: {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
          if (!functionsContext.lastFunctionTime[i] || (CFN_FUNC(cfn)==FUNC_PLAY_BOTH && active!=(bool)(functionsContext.activeSwitches&switch_mask)) || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
            functionsContext.lastFunctionTime[i] = tmr10ms;
            uint8_t param = CFN_PARAM(cfn);
            if (CFN_FUNC(cfn) == FUNC_PLAY_SOUND) {
              AUDIO_PLAY(AU_FRSKY_FIRST+param);
            } else if (CFN_FUNC(cfn) == FUNC_PLAY_VALUE) {
              PLAY_VALUE(param, PLAY_INDEX);
            } else {
#if defined(GVARS)
              if (CFN_FUNC(cfn) == FUNC_PLAY_TRACK && param > 250)
                param = GVAR_VALUE(param-251, getGVarFlightPhase(mixerCurrentFlightMode, param-251));
#endif
              PUSH_CUSTOM_PROMPT(active ? param : param+1, PLAY_INDEX);
            }
          }
          if (!active) {
            // PLAY_BOTH would change activeFnSwitches otherwise
            switch_mask = 0;
          }
          break;
        }
#else
        case FUNC_PLAY_SOUND: {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
          if (!functionsContext.lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
            functionsContext.lastFunctionTime[i] = tmr10ms;
            AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(cfn));
          }
          break;
        }
#endif

#if defined(FRSKY) && defined(VARIO)
        case FUNC_VARIO:
          newActiveFunctions |= (1 << FUNCTION_VARIO);
          break;
#endif

#if defined(HAPTIC)
        case FUNC_HAPTIC: {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
          if (!functionsContext.lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
            functionsContext.lastFunctionTime[i] = tmr10ms;
            haptic.event(AU_FRSKY_LAST+CFN_PARAM(cfn));
          }
          break;
        }
#endif

#if defined(SDCARD)
        case FUNC_LOGS:
          if (CFN_PARAM(cfn)) {
            newActiveFunctions |= (1 << FUNCTION_LOGS);
            logDelay = CFN_PARAM(cfn);
          }
          break;
#endif

        case FUNC_BACKLIGHT:
          newActiveFunctions |= (1 << FUNCTION_BACKLIGHT);
          break;

        }

        newActiveSwitches |= switch_mask;
      } else {
        functionsContext.lastFunctionTime[i] = 0;
      }
    }
  }

  functionsContext.activeSwitches   = newActiveSwitches;
  functionsContext.activeFunctions  = newActiveFunctions;

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  for (uint8_t i=0; i<ROTARY_ENCODERS; i++) {
    rePreviousValues[i] = (g_rotenc[i] / ROTARY_ENCODER_GRANULARITY);
  }
#endif
}

#undef functions
#undef functionsContext
