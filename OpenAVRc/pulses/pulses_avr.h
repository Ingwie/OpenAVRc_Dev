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



#ifndef pulses_avr_h
#define pulses_avr_h


static volatile uint16_t timer_counts;


extern uint8_t s_current_protocol;
uint16_t *RptrA; // For OCR1A
uint16_t *RptrB; // Zum OCR1B

extern uint8_t *pulses2MHzWPtr;

extern uint16_t nextMixerEndTime;

#define MAX_MIXER_DELTA_US   (32000) /* 32ms max as an interval between 2 mixer calculations */

#define PULSES_SETUP_TIME_US (500) // 500usec

void (*ocr1b_function_ptr)(); // Function pointer to add flexibility and simplicity to ISR.

extern uint16_t dt;


#define PULSES_WORD_SIZE  72		// 72=((2+2*6)*10)/2+2
// 72 (A 16 Channel PPM frame has 34 timing events + 1 int terminator).
#define PULSES_BYTE_SIZE  (PULSES_WORD_SIZE * 2)

#if defined(PCM_PROTOCOL)
#include "../protocol/PROTO_PCM.h"
#endif
#if defined(MULTIMODULE)
#include "../protocol/MULTI_SERIAL.h"
#endif

union p2mhz_t
{
  uint16_t pword[PULSES_WORD_SIZE];
  uint8_t  pbyte[PULSES_BYTE_SIZE]; // 144
#if defined(MULTIMODULE)
  mmSt_t   mm_st;
#endif
#if defined(PCM_PROTOCOL)
  PcmSt_t  Pcm;
#endif
} pulses2MHz;

enum ppmtype{
PPM,
PPMSIM,
PPM16LAST,
PPM16FIRST,
};

void setupPulsesPPM(enum ppmtype proto);
void DSM2_Init();
void DSM2_Done();

FORCEINLINE uint8_t pulsesStarted();
FORCEINLINE void sendStopPulses();

#endif

