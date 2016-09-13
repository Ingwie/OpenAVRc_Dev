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

#ifndef pulses_avr_h
#define pulses_avr_h

extern uint8_t s_current_protocol[1];
extern uint8_t s_pulses_paused;

extern uint8_t *pulses2MHzRPtr;
extern uint8_t *pulses2MHzWPtr;

extern uint16_t nextMixerEndTime;

extern uint8_t moduleFlag[NUM_MODULES];

#define MAX_MIXER_DELTA (50*16) /* 50ms max as an interval between 2 mixer calculations */

void startPulses();
inline bool pulsesStarted() { return s_current_protocol[0] != 255; }
inline void pausePulses() { s_pulses_paused = true; }
inline void resumePulses() { s_pulses_paused = false; }
void setupPulses();
void DSM2_Init();
void DSM2_Done();

#define SEND_FAILSAFE_NOW()
#define SEND_FAILSAFE_1S()

#endif
/*eof*/
