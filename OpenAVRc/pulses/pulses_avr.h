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


#ifndef pulses_avr_h
#define pulses_avr_h

extern uint8_t s_current_protocol[1];
extern uint8_t s_pulses_paused;

extern uint8_t *pulses2MHzRPtr;
extern uint8_t *pulses2MHzWPtr;

extern uint16_t nextMixerEndTime;

extern uint8_t moduleFlag;

#define MAX_MIXER_DELTA (50*16) /* 50ms max as an interval between 2 mixer calculations */

void startPulses();
inline bool pulsesStarted()
{
  return s_current_protocol[0] != 255;
}
inline void pausePulses()
{
  s_pulses_paused = true;
}
inline void resumePulses()
{
  s_pulses_paused = false;
}
void setupPulses();
void DSM2_Init();
void DSM2_Done();

#define SEND_FAILSAFE_NOW()
#define SEND_FAILSAFE_1S()

#endif
/*eof*/
