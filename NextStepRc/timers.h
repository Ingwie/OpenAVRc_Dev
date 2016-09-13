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

#ifndef _TIMERS_H_
#define _TIMERS_H_

#define TMR_OFF      0
#define TMR_RUNNING  1
#define TMR_NEGATIVE 2
#define TMR_STOPPED  3

typedef int16_t tmrval_t;
typedef uint16_t tmrstart_t;
#define TIMER_MAX     (0xffff/2)

#define TIMER_MIN     (tmrval_t(-TIMER_MAX-1))

struct TimerState {
  uint16_t cnt;
  uint16_t sum;
  uint8_t  state;
  tmrval_t  val;
  uint8_t  val_10ms;
};

extern TimerState timersStates[TIMERS];

void timerReset(uint8_t idx);


#if defined(CPUM2560)
  void saveTimers();
  void restoreTimers();
#else
  #define saveTimers()
  #define restoreTimers()
#endif

void evalTimers(int16_t throttle, uint8_t tick10ms);

#endif
