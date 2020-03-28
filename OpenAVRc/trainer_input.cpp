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
uint8_t puppySignalValidityTimer = 0;

/*
 * Trainer PPM input capture ISR.
 * Timer 1 is free running as it serves other purposes.
 * 1 count is 0.5us
*/
ISR(TRAINER_TC_VECT) // G: High frequency noise can cause stack overflow with ISR_NOBLOCK
{
  uint16_t capture = TRAINER_TC_REG;

  static uint16_t lastCapt = 0;
  static uint8_t channelNumber = 0;

  uint16_t val = (uint16_t) (capture - lastCapt) / 2; // Convert from timer counts to usec.
  lastCapt = capture;

  if(channelNumber && val > 800 && val < 2200 && channelNumber <= NUM_TRAINER)
  { // Accepted range is 800 to 2200 us  1500us+/-700.
    ppmInput[channelNumber++ -1] =
      (int16_t)(val - 1500)*(g_eeGeneral.PPM_Multiplier+10)/10;
  }
  else if(val > 4000 && val < 19000)
  { // Frame sync pulse >4 <19 ms.
    if (channelNumber > NUM_TRAINER) // all the frame capture OK ?
    {
      if (!puppySignalValidityTimer)
      {
        puppySignalValidityTimer = PUPPY_VALID_TIMEOUT_FIRST;
        systemBolls.puppyPpmSignalOk = 1; // Signal is from PPM (not BT)
      }
      else
      {
        puppySignalValidityTimer = PUPPY_VALID_TIMEOUT;
      }
    }
    channelNumber = 1; // Indicates start of new frame.
  }
  else channelNumber = 0; /* Glitches (<800us) or long channel pulses (2200 to 4000us) or
  pulses > 19000us reset the process */
}

