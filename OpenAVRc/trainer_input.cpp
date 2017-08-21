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


#include "trainer_input.h"

int16_t ppmInput[NUM_TRAINER];
uint8_t ppmInputValidityTimer;

// Timer3 used for PPM_IN pulse width capture. Counter running at 16MHz / 8 = 2MHz
// equating to one count every half millisecond. (2 counts = 1ms). Control channel
// count delta values thus can range from about 1600 to 4400 counts (800us to 2200us),
// corresponding to a PPM signal in the range 0.8ms to 2.2ms (1.5ms at center).
// (The timer is free-running and is thus not reset to zero at each capture interval.)
ISR(TIMER1_CAPT_vect) // G: High frequency noise can cause stack overflow with ISR_NOBLOCK
{
  // Prevent rentrance for this IRQ only.
  TIMSK1 &= ~(1<<ICIE1); // Disable ICP Interrupt.

  uint16_t capture = ICR1;

  sei(); // enable other interrupts

  // captureTrainerPulses(capture);
  // Needs to be inlined to avoid slow function calls in ISR routines
  // inline void captureTrainerPulses(uint16_t capture)


  static uint16_t lastCapt=0;
  static uint8_t channelNumber=0;

    uint16_t val = (uint16_t)(capture - lastCapt) / 2;
    lastCapt = capture;

    // We process ppmInput right here to make servo movement as smooth as possible
    //    while under trainee control
    //
    // G: Prioritize reset pulse. (Needed when less than 16 incoming pulses)
    //
    if (val>4000 && val<19000) {
      channelNumber = 1; // triggered
    } else {
      if ((channelNumber > 0) && (channelNumber <= NUM_TRAINER)) {
        if (val>800 && val<2200) {
          ppmInputValidityTimer = PPM_IN_VALID_TIMEOUT;
          ppmInput[channelNumber++ - 1] =
            //+-500 != 512, but close enough.
            (int16_t)(val - 1500)*(g_eeGeneral.PPM_Multiplier+10)/10;
        } else {
          channelNumber = 0; // not triggered
        }
      }
    }

  cli(); // Disable other interrupts for stack pops before this function's RETI.
  TIFR1 |= (1<<ICF1); // Clear Flag, just in case ISR has taken too long or high frequency noise is on input.
  TIMSK1 |= (1<<ICIE1); // Enable ICP Interrupt.
}


#if 0
ISR(TIMER1_CAPT_vect)
{
uint16_t icr1_diff;
uint16_t icr1_current;

static uint16_t icr1_previous = 0;
static uint8_t servo_count = 0;
static uint8_t need_to_sync = 1;

icr1_current = ICR1;

//icr1_diff = icr1_current - icr1_previous;
if(icr1_current >= icr1_previous) icr1_diff = icr1_current - icr1_previous;
else icr1_diff = (0xffff - icr1_previous) + icr1_current + 1 ;

icr1_previous = icr1_current;

	if (icr1_diff > MICRO_SEC_CONVERT(2300)) // > 2.3ms pulse seen as frame sync.
	{
	need_to_sync =0;
	g_sync_count ++;
	servo_count =0;
   	}
	else if (icr1_diff < MICRO_SEC_CONVERT(700)) // < 0.7ms pulse seen as glitch.
	{
	// Do nothing with glitch.
	}
	else if (! need_to_sync) // Pulse within limits and we don't need to sync.
	{
		if (servo_count < NUM_OUT_CHANNELS)
		{
		if (icr1_diff > MICRO_SEC_CONVERT(1500 + DELTA_PPM_IN)) icr1_diff = MICRO_SEC_CONVERT(1500 + DELTA_PPM_IN);
   		else if (icr1_diff < MICRO_SEC_CONVERT(1500 - DELTA_PPM_IN)) icr1_diff = MICRO_SEC_CONVERT(1500 - DELTA_PPM_IN);

		// Subtract 1.5 ms centre offset.
		// Multiply by 2 to get max-min counter value difference to be +-1520
		// (same scaling as M-Link Packet for MPX ppm (+-550us) range !).

   		Channels[servo_count] = (icr1_diff - MICRO_SEC_CONVERT(1500)) * 2;
   		servo_count++;
		}
		else need_to_sync = 1; // More servo pulses than we can handle ... need to sync.
	}

}
#endif

