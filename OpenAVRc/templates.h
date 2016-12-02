/*
 *************************************************************
 *                      OpenAVRc                           *
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

/*
 * ============================================================
 * Templates file
 *
 * eccpm
 * crow
 * throttle cut
 * flaperon
 * elevon
 * v-tail
 * throttle hold
 * Aileron Differential
 * Spoilers
 * Snap Roll
 * ELE->Flap
 * Flap->ELE
 *
 */

#ifndef TEMPLATES_H_
#define TEMPLATES_H_

#include <inttypes.h>


#define STK_RUD  1
#define STK_ELE  2
#define STK_THR  3
#define STK_AIL  4
#define STK_P1   5
#define STK_P2   6
#define STK_P3   7

#define CV(x) (CURVE_BASE+(x)-1)
#define CC(x) (MIXSRC_Rud - 1 + channel_order(x)) // need to invert this to work with dest

  #define mixSetCurve(mix, idx) mix->curveMode=MODE_CURVE; mix->curveParam=CURVE_BASE+idx

enum Templates {
  TMPL_CLEAR_MIXES,
  TMPL_SIMPLE_4CH,
  TMPL_STI_THR_CUT,
  TMPL_V_TAIL,
  TMPL_ELEVON_DELTA,
  TMPL_ECCPM,
  TMPL_HELI_SETUP,
  TMPL_SERVO_TEST,
  TMPL_COUNT
};

void clearMixes();
void clearCurves();
void applyTemplate(uint8_t idx);

#endif //TEMPLATES_H


