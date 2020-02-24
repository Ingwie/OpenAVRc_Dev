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

#if TIMERS > MAX_TIMERS
#error "Timers cannot exceed " .. MAX_TIMERS
#endif

TimerState timersStates[TIMERS] = { { 0 } };

void timerReset(uint8_t idx)
{
  timersStates[idx].state = TMR_OFF; // is changed to RUNNING dep from mode
  timersStates[idx].val = g_model.timers[idx].start;
  timersStates[idx].val_10ms = 0 ;
}


void restoreTimers()
{
  for (uint8_t i=0; i<TIMERS; i++) {
    if (g_model.timers[i].persistent) {
      timersStates[i].val = g_model.timers[i].value;
    }
  }
}

void saveTimers()
{
  for (uint8_t i=0; i<TIMERS; i++) {
    if (g_model.timers[i].persistent) {
      TimerState *timerState = &timersStates[i];
      if (g_model.timers[i].value != (uint16_t)timerState->val) {
        g_model.timers[i].value = timerState->val;
        eeDirty(EE_MODEL);
      }
    }
  }

}

#define THR_TRG_TRESHOLD    13      // approximately 10% full throttle

void evalTimers(int16_t throttle, uint8_t tick10ms)
{
  if (!gazSecurity.enableGaz)
  {
    throttle = 0;
  }

  for (uint8_t i=0; i<TIMERS; i++) {
    int8_t timerMode = g_model.timers[i].mode;
    tmrstart_t timerStart = g_model.timers[i].start;
    TimerState * timerState = &timersStates[i];

    if (timerMode) {
      if ((timerState->state == TMR_OFF) && (timerMode != TMRMODE_THR_TRG)) {
        timerState->state = TMR_RUNNING;
        timerState->cnt = 0;
        timerState->sum = 0;
      }

      if (timerMode == TMRMODE_THR_REL) {
        timerState->cnt++;
        timerState->sum += throttle;
      }

      if ((timerState->val_10ms += tick10ms) >= 100) {
        if (timerState->val == TIMER_MAX) break;
        if (timerState->val == TIMER_MIN) break;

        timerState->val_10ms -= 100 ;
        tmrval_t newTimerVal = timerState->val;
        if (timerStart) newTimerVal = timerStart - newTimerVal;

        if (timerMode == TMRMODE_ABS) {
          newTimerVal++;
        } else if (timerMode == TMRMODE_THR) {
          if (throttle) newTimerVal++;
        } else if (timerMode == TMRMODE_THR_REL) {
          // @@@ open.20.fsguruh: why so complicated? we have already a s_sum field; use it for the half seconds (not showable) as well
          // check for s_cnt[i]==0 is not needed because we are shure it is at least 1
          if ((timerState->sum/timerState->cnt) >= 128) {  // throttle was normalized to 0 to 128 value (throttle/64*2 (because - range is added as well)
            newTimerVal++;  // add second used of throttle
            timerState->sum -= 128*timerState->cnt;
          }
          timerState->cnt = 0;
        } else if (timerMode == TMRMODE_THR_TRG) {
          // we can't rely on (throttle || newTimerVal > 0) as a detection if timer should be running
          // because having persistent timer brakes this rule
          if ((throttle > THR_TRG_TRESHOLD) && timerState->state == TMR_OFF) {
            timerState->state = TMR_RUNNING;  // start timer running
            timerState->cnt = 0;
            timerState->sum = 0;
            // TRACE("Timer[%d] THr triggered", i);
          }
          if (timerState->state != TMR_OFF) newTimerVal++;
        } else {
          if (timerMode > 0) timerMode -= (TMRMODE_COUNT-1);
          if (getSwitch(timerMode)) {
            newTimerVal++;
          }
        }

        switch (timerState->state) {
        case TMR_RUNNING:
          if (timerStart && newTimerVal>=(tmrval_t)timerStart) {
            AUDIO_TIMER_00(g_model.timers[i].countdownBeep);
            timerState->state = TMR_NEGATIVE;
            // TRACE("Timer[%d] negative", i);
          }
          break;
        case TMR_NEGATIVE:
          if (newTimerVal >= (tmrval_t)timerStart + MAX_ALERT_TIME) {
            timerState->state = TMR_STOPPED;
            // TRACE("Timer[%d] stopped state at %d", i, newTimerVal);
          }
          break;
        }

        if (timerStart) newTimerVal = timerStart - newTimerVal; // if counting backwards - display backwards

        if (newTimerVal != timerState->val) {
          timerState->val = newTimerVal;
          if (timerState->state == TMR_RUNNING) {
            if (g_model.timers[i].countdownBeep && g_model.timers[i].start) {
              if (newTimerVal == 30) {
                AUDIO_TIMER_30();
                // TRACE("Timer[%d] 30s announcement", i);
              }
              if (newTimerVal == 20) {
                AUDIO_TIMER_20();
                // TRACE("Timer[%d] 20s announcement", i);
              }
              if (newTimerVal <= 10) {
                AUDIO_TIMER_LT10(g_model.timers[i].countdownBeep, newTimerVal);
                // TRACE("Timer[%d] %ds announcement", i, newTimerVal);
              }
            }
            if (g_model.timers[i].minuteBeep && (newTimerVal % 60)==0) {
              AUDIO_TIMER_MINUTE(newTimerVal);
              // TRACE("Timer[%d] %d minute announcement", i, newTimerVal/60);
            }
          }
        }
      }
    }
  }
}
