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


#include "../OpenAVRc.h"

// Maybe move these definitions to the target board.h file.
// Bit Bang will work on any spare port pin.
#define PPM_PIN_HIGH()      PORTB |= (1<<OUT_B_PPM)
#define PPM_PIN_LOW()       PORTB &= ~(1<<OUT_B_PPM)
#define PPM_PIN_TOGGLE()    PORTB ^= (1<<OUT_B_PPM)

/*
 * 16 Bit Timer running @ 16MHz has a resolution of 0.5us.
 * This should give a PPM resolution of 2048.
*/
static uint16_t PPM_BB_cb()
{
  if ( *((uint16_t*)pulses2MHzRPtr) == 0) {

    if (g_model.pulsePol) PPM_PIN_LOW(); // Set idle level.
    else PPM_PIN_HIGH(); // GCC optimisation should produce a single SBI instruction.

    // Schedule next Mixer calculations.
    SCHEDULE_MIXER_END(45*8+g_model.ppmFrameLength*8);

    setupPulsesPPM(PROTO_PPM);
    heartbeat |= HEART_TIMER_PULSES;
    return SETUP_PULSES_DURATION *2;
  }

  dt = TCNT1 - OCR1A;
  /*
   * Calculate delay between OCR1A match and this point in the code before the toggle of the port pin.
   * This shows any jitter due to a delayed ISR.
   */

  PPM_PIN_TOGGLE(); // Toggle port bit.

  uint16_t temp = *((uint16_t *) pulses2MHzRPtr);
  pulses2MHzRPtr += sizeof(uint16_t); // Non PPM protocols use uint8_t pulse buffer.
  return temp; // Schedule next Timer1 interrupt vector (to this function).
}


static void PPM_BB_initialize()
{
#if defined(FRSKY) && !defined(DSM2_SERIAL)
  telemetryInit();
#endif

  pulses2MHzRPtr = pulses2MHz;
  *((uint16_t*) pulses2MHzRPtr) = 0;
  CLOCK_StartTimer(25000U *2, &PPM_BB_cb);
}


const void * PPM_BB_Cmds(enum ProtoCmds cmd)
{
    switch(cmd) {
        case PROTOCMD_INIT: PPM_BB_initialize(); return 0;
        case PROTOCMD_DEINIT:
        case PROTOCMD_RESET:
            CLOCK_StopTimer();
            return (void *) 1L;
//        case PROTOCMD_CHECK_AUTOBIND: return 0;
//        case PROTOCMD_BIND:  ppm_bb_initialize(); return 0;
//        case PROTOCMD_NUMCHAN: return (void *) 16L;
//        case PROTOCMD_DEFAULT_NUMCHAN: return (void *) 8L;
/*        case PROTOCMD_GETOPTIONS:
            if (Model.proto_opts[CENTER_PW] == 0) {
                Model.proto_opts[CENTER_PW] = 1100;
                Model.proto_opts[DELTA_PW] = 500;
                Model.proto_opts[NOTCH_PW] = 400;
                Model.proto_opts[PERIOD_PW] = 22500;
            }
            return ppm_opts;
*/
//        case PROTOCMD_TELEMETRYSTATE: return (void *)(long) PROTO_TELEM_UNSUPPORTED;
        default: break;
    }
    return 0;
}

