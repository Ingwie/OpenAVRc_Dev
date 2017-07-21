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

#define CS_LAST_VALUE_INIT -32768


int16_t lsLastValue[NUM_LOGICAL_SWITCH];
#define LS_LAST_VALUE(fm, idx) lsLastValue[idx]

volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_used = 0;
volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_value = 0;


#define getValueForLogicalSwitch(i) getValue(i)

PACK(typedef struct {
  uint8_t state;
  uint8_t last;
}) ls_sticky_struct;

bool getLogicalSwitch(uint8_t idx)
{
  LogicalSwitchData * ls = lswAddress(idx);
  bool result;

  uint8_t s = ls->andsw;
  if (s > SWSRC_LAST_SWITCH) {
    s += SWSRC_SW1-SWSRC_LAST_SWITCH-1;
  }

  if (ls->func == LS_FUNC_NONE || (s && !getSwitch(s))) {
    if (ls->func != LS_FUNC_STICKY) {
      // AND switch must not affect STICKY and EDGE processing
      LS_LAST_VALUE(mixerCurrentFlightMode, idx) = CS_LAST_VALUE_INIT;
    }
    result = false;
  } else if ((s=lswFamily(ls->func)) == LS_FAMILY_BOOL) {
    bool res1 = getSwitch(ls->v1);
    bool res2 = getSwitch(ls->v2);
    switch (ls->func) {
    case LS_FUNC_AND:
      result = (res1 && res2);
      break;
    case LS_FUNC_OR:
      result = (res1 || res2);
      break;
    // case LS_FUNC_XOR:
    default:
      result = (res1 ^ res2);
      break;
    }
  } else if (s == LS_FAMILY_TIMER) {
    result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) <= 0);
  } else if (s == LS_FAMILY_STICKY) {
    result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) & (1<<0));
  } else {
    getvalue_t x = getValueForLogicalSwitch(ls->v1);
    getvalue_t y;
    if (s == LS_FAMILY_COMP) {
      y = getValueForLogicalSwitch(ls->v2);

      switch (ls->func) {
      case LS_FUNC_EQUAL:
        result = (x==y);
        break;
      case LS_FUNC_GREATER:
        result = (x>y);
        break;
      default:
        result = (x<y);
        break;
      }
    } else {
      mixsrc_t v1 = ls->v1;
#if defined(FRSKY)
      // Telemetry
      if (v1 >= MIXSRC_FIRST_TELEM) {
        if ((!TELEMETRY_STREAMING() && v1 >= MIXSRC_FIRST_TELEM+TELEM_FIRST_STREAMED_VALUE-1) || IS_FAI_FORBIDDEN(v1-1)) {
          result = false;
          goto DurationAndDelayProcessing;
        }

        y = convertLswTelemValue(ls);

#if defined(GAUGES)
        // Fill the telemetry bars threshold array
        if (s == LS_FAMILY_OFS) {
          uint8_t idx = v1-MIXSRC_FIRST_TELEM+1-TELEM_ALT;
          if (idx < THLD_MAX) {
            FILL_THRESHOLD(idx, ls->v2);
          }
        }
#endif

      } else if (v1 >= MIXSRC_GVAR1) {
        y = ls->v2;
      } else {
        y = calc100toRESX(ls->v2);
      }
#else
      if (v1 >= MIXSRC_FIRST_TELEM) {
        //y = (int16_t)3 * (128+ls->v2); // it's a Timer
        y = convertLswTelemValue(ls); // Add by Mentero -> under test (todo)
      } else if (v1 >= MIXSRC_GVAR1) {
        y = ls->v2; // it's a GVAR
      } else {
        y = calc100toRESX(ls->v2);
      }
#endif

      switch (ls->func) {
      case LS_FUNC_VALMOSTEQUAL:
#if defined(GVARS)
        if (v1 >= MIXSRC_GVAR1 && v1 <= MIXSRC_LAST_GVAR)
          result = (x==y);
        else
#endif
          result = (abs(x-y) < (1024 / STICK_TOLERANCE));
        break;
      case LS_FUNC_VPOS:
        result = (x>y);
        break;
      case LS_FUNC_VNEG:
        result = (x<y);
        break;
      case LS_FUNC_APOS:
        result = (abs(x)>y);
        break;
      case LS_FUNC_ANEG:
        result = (abs(x)<y);
        break;
      default: {
        if (LS_LAST_VALUE(mixerCurrentFlightMode, idx) == CS_LAST_VALUE_INIT) {
          LS_LAST_VALUE(mixerCurrentFlightMode, idx) = x;
        }
        int16_t diff = x - LS_LAST_VALUE(mixerCurrentFlightMode, idx);
        bool update = false;
        if (ls->func == LS_FUNC_DIFFEGREATER) {
          if (y >= 0) {
            result = (diff >= y);
            if (diff < 0)
              update = true;
          } else {
            result = (diff <= y);
            if (diff > 0)
              update = true;
          }
        } else {
          result = (abs(diff) >= y);
        }
        if (result || update) {
          LS_LAST_VALUE(mixerCurrentFlightMode, idx) = x;
        }
        break;
      }
      }
    }
  }

#if defined(FRSKY)
DurationAndDelayProcessing:
#endif


  return result;
}

bool getSwitch(swsrc_t swtch)
{
  bool result;

  if (swtch == SWSRC_NONE)
    return true;

  uint8_t cs_idx = abs(swtch);

  if (cs_idx == SWSRC_ONE) {
    result = !s_mixer_first_run_done;
  } else if (cs_idx == SWSRC_ON) {
    result = true;
  } else if (cs_idx <= SWSRC_LAST_SWITCH) {
    result = switchState((EnumKeys)(SW_BASE+cs_idx-SWSRC_FIRST_SWITCH));

#if defined(MODULE_ALWAYS_SEND_PULSES)
    if (startupWarningState < STARTUP_WARNING_DONE) {
      // if throttle or switch warning is currently active, ignore actual stick position and use wanted values
      if (cs_idx <= 3) {
        if (!(g_model.switchWarningEnable & 1)) {     // ID1 to ID3 is just one bit in switchWarningEnable
          result = (cs_idx)==((g_model.switchWarningState & 3)+1);  // overwrite result with desired value
        }
      } else if (!(g_model.switchWarningEnable & (1<<(cs_idx-3)))) {
        // current switch should not be ignored for warning
        result = g_model.switchWarningState & (1<<(cs_idx-2)); // overwrite result with desired value
      }
    }
#endif
  } else if (cs_idx <= SWSRC_LAST_TRIM) {
    uint8_t idx = cs_idx - SWSRC_FIRST_TRIM;
    idx = (CONVERT_MODE(idx/2) << 1) + (idx & 1);
    result = trimDown(idx);
  }
#if ROTARY_ENCODERS > 0
  else if (cs_idx == SWSRC_REa) {
    result = REA_DOWN();
  }
#endif
#if ROTARY_ENCODERS > 1
  else if (cs_idx == SWSRC_REb) {
    result = REB_DOWN();
  }
#endif
  else {
    cs_idx -= SWSRC_FIRST_LOGICAL_SWITCH;
    GETSWITCH_RECURSIVE_TYPE mask = ((GETSWITCH_RECURSIVE_TYPE)1 << cs_idx);
    if (s_last_switch_used & mask) {
      result = (s_last_switch_value & mask);
    } else {
      s_last_switch_used |= mask;
      result = getLogicalSwitch(cs_idx);
      if (result) {
        s_last_switch_value |= mask;
      } else {
        s_last_switch_value &= ~mask;
      }
    }
  }

  return swtch > 0 ? result : !result;
}


swarnstate_t switches_states = 0;
swsrc_t getMovedSwitch()
{
  static tmr10ms_t s_move_last_time = 0;
  swsrc_t result = 0;

  // return delivers 1 to 3 for ID1 to ID3
  // 4..8 for all other switches if changed to true
  // -4..-8 for all other switches if changed to false
  // 9 for Trainer switch if changed to true; Change to false is ignored
  swarnstate_t mask = 0x80;
  for (uint8_t i=NUM_PSWITCH; i>1; i--) {
    bool prev;
    prev = (switches_states & mask);
    // don't use getSwitch here to always get the proper value, even getSwitch manipulates
    bool next = switchState((EnumKeys)(SW_BASE+i-1));
    if (prev != next) {
      if (((i<NUM_PSWITCH) && (i>3)) || next==true)
        result = next ? i : -i;
      if (i<=3 && result==0) result = 1;
      switches_states ^= mask;
    }
    mask >>= 1;
  }

  if ((tmr10ms_t)(get_tmr10ms() - s_move_last_time) > 10)
    result = 0;

  s_move_last_time = get_tmr10ms();
  return result;
}

#if defined(GUI)
void checkSwitches()
{
#if defined(MODULE_ALWAYS_SEND_PULSES)
  static swarnstate_t last_bad_switches = 0xff;
#else
  swarnstate_t last_bad_switches = 0xff;
#endif
  swarnstate_t states = g_model.switchWarningState;



#if !defined(MODULE_ALWAYS_SEND_PULSES)
  while (1) {

#ifdef GETADC_COUNT
    for (int i=0; i<GETADC_COUNT; i++) {
      getADC();
    }
#undef GETADC_COUNT
#endif
#endif  // !defined(MODULE_ALWAYS_SEND_PULSES)

    getMovedSwitch();

    bool warn = false;
    for (int i=0; i<NUM_SWITCHES-1; i++) {
      if (!(g_model.switchWarningEnable & (1<<i))) {
        if (i == 0) {
          if ((states & 0x03) != (switches_states & 0x03)) {
            warn = true;
          }
        } else if ((states & (1<<(i+1))) != (switches_states & (1<<(i+1)))) {
          warn = true;
        }
      }
    }

    if (!warn) {
#if defined(MODULE_ALWAYS_SEND_PULSES)
      startupWarningState = STARTUP_WARNING_SWITCHES+1;
      last_bad_switches = 0xff;
#endif
      return;
    }

    // first - display warning
    if (last_bad_switches != switches_states) {
      MESSAGE(STR_SWITCHWARN, NULL, STR_PRESSANYKEYTOSKIP, last_bad_switches == 0xff ? AU_SWITCH_ALERT : AU_NONE);
      uint8_t x = 2;
      for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
        uint8_t attr;
        if (i == 0)
          attr = ((states & 0x03) != (switches_states & 0x03)) ? INVERS : 0;
        else
          attr = (states & (1 << (i+1))) == (switches_states & (1 << (i+1))) ? 0 : INVERS;
        if (!(g_model.switchWarningEnable & (1<<i)))
          lcdPutsSwitches(x, 5*FH, (i>0?(i+3):(states&0x3)+1), attr);
        x += 3*FW+FW/2;
      }
      lcdRefresh();
      last_bad_switches = switches_states;
    }

#if defined(MODULE_ALWAYS_SEND_PULSES)
    if (pwrCheck()==e_power_off || keyDown()) {
      startupWarningState = STARTUP_WARNING_SWITCHES+1;
      last_bad_switches = 0xff;
    }
#else

    if (keyDown()) {
      return;
    }

    if (pwrCheck() == e_power_off) {
      return;
    }

    checkBacklight();

    MYWDT_RESET();
  }
#endif
  }
#endif // GUI

  void logicalSwitchesTimerTick()
  {
    for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
      LogicalSwitchData * ls = lswAddress(i);
      if (ls->func == LS_FUNC_TIMER) {
        int16_t *lastValue = &LS_LAST_VALUE(fm, i);
        if (*lastValue == 0 || *lastValue == CS_LAST_VALUE_INIT) {
          *lastValue = -lswTimerValue(ls->v1);
        } else if (*lastValue < 0) {
          if (++(*lastValue) == 0)
            *lastValue = lswTimerValue(ls->v2);
        } else { // if (*lastValue > 0)
          *lastValue -= 1;
        }
      } else if (ls->func == LS_FUNC_STICKY) {
        ls_sticky_struct & lastValue = (ls_sticky_struct &)LS_LAST_VALUE(fm, i);
        bool before = lastValue.last & 0x01;
        if (lastValue.state) {
          bool now = getSwitch(ls->v2);
          if (now != before) {
            lastValue.last ^= 1;
            if (!before) {
              lastValue.state = 0;
            }
          }
        } else {
          bool now = getSwitch(ls->v1);
          if (before != now) {
            lastValue.last ^= 1;
            if (!before) {
              lastValue.state = 1;
            }
          }
        }
      }
    }
  }

  LogicalSwitchData * lswAddress(uint8_t idx)
  {
    return &g_model.logicalSw[idx];
  }

  uint8_t lswFamily(uint8_t func)
  {
    if (func <= LS_FUNC_ANEG)
      return LS_FAMILY_OFS;
    else if (func <= LS_FUNC_XOR)
      return LS_FAMILY_BOOL;
    else if (func <= LS_FUNC_LESS)
      return LS_FAMILY_COMP;
    else if (func <= LS_FUNC_ADIFFEGREATER)
      return LS_FAMILY_DIFF;
    else
      return LS_FAMILY_TIMER+func-LS_FUNC_TIMER;
  }

  int16_t lswTimerValue(delayval_t val)
  {
    return (val < -109 ? 129+val : (val < 7 ? (113+val)*5 : (53+val)*10));
  }

  void logicalSwitchesReset()
  {
    s_last_switch_value = 0;

    for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
      LS_LAST_VALUE(fm, i) = CS_LAST_VALUE_INIT;
    }
  }

  getvalue_t convertLswTelemValue(LogicalSwitchData * ls)
  {
    getvalue_t val;
    if (lswFamily(ls->func)==LS_FAMILY_OFS)
      val = convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128+ls->v2);
    else
      val = convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128+ls->v2) - convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128);
    return val;
  }

