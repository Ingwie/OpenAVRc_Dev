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

extern uint8_t s_current_protocol;
uint16_t *RptrA; // For OCR1A
uint16_t *RptrB; // Zum OCR1B

extern uint8_t *pulses2MHzWPtr;

extern uint16_t nextMixerEndTime;

#define MAX_MIXER_DELTA_US (50000) /* 50ms max as an interval between 2 mixer calculations */

#define PULSES_SETUP_TIME 500 // 500usec
void (*ocr1b_function_ptr)(); // Function pointer to add flexibility and simplicity to ISR.

extern uint16_t dt;


//#ifdef SBUS_PROTOCOL
//#define PULSES_WORD_SIZE  115	// 72=((2+2*6)*10)/2+2
//#define PULSES_BYTE_SIZE  (PULSES_WORD_SIZE * 2)
//#else
#define PULSES_WORD_SIZE  72		// 72=((2+2*6)*10)/2+2
// 72 (A 16 Channel PPM frame has 34 timing events + 1 int terminator).
#define PULSES_BYTE_SIZE  (PULSES_WORD_SIZE * 2)
//#endif

union p2mhz_t
{
  uint16_t pword[PULSES_WORD_SIZE];
  uint8_t  pbyte[PULSES_BYTE_SIZE];  // 144
} pulses2MHz;

enum ppmtype{
PPM,
PPMSIM,
PPM16LAST,
PPM16FIRST,
};

void setupPulses();
void setupPulsesPPM(enum ppmtype proto);
void DSM2_Init();
void DSM2_Done();

FORCEINLINE bool pulsesStarted();
FORCEINLINE void sendStopPulses();

#endif

