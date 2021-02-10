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


#define ADC_VREF_TYPE _BV(REFS0) // AVCC with external capacitor at AREF pin

void adcInit()
{
  ADMUX = ADC_VREF_TYPE;
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS0); // ADC enabled, pre-scaler division=32 (no interrupt, no auto-triggering)
  ADCSRB = _BV(MUX5); // use port K (AD8 - AD15)
  DIDR2 = 0xFF; // disable digital input buffer on port K (reduce impedance and noise)
}

void getADC() // 0.56 mS
{
#if defined(INV_STICK_RH) || defined(INV_STICK_LV) || defined(INV_STICK_RV) || defined(INV_STICK_LH)
uint8_t invMask = 0
#if defined(INV_STICK_RH)
+ _BV(0)
#endif
#if defined(INV_STICK_LV)
+ _BV(1)
#endif
#if defined(INV_STICK_RV)
+ _BV(2)
#endif
#if defined(INV_STICK_LH)
+ _BV(3)
#endif
;
#endif
  uint16_t temp_ana;
  for (uint8_t adc_input=0; adc_input<8; adc_input++) {
    ADMUX = adc_input|ADC_VREF_TYPE;
    ADCSRA |= _BV(ADSC); // Start the AD conversion
    while SIMU_UNLOCK_MACRO_FALSE(bit_is_set(ADCSRA,ADSC)); // Wait for the AD conversion to complete
    temp_ana = ADC;
    ADCSRA |= _BV(ADSC); // Start the second AD conversion
    while SIMU_UNLOCK_MACRO_FALSE(bit_is_set(ADCSRA,ADSC)); // Wait for the AD conversion to complete
    temp_ana += ADC;
#if defined(INV_STICK_RH) || defined(INV_STICK_LV) || defined(INV_STICK_RV) || defined(INV_STICK_LH)
    if (invMask & 0x1)
    {
      temp_ana = 0x7FE - temp_ana;
    }
    invMask >>= 1;
#endif
#if !defined(SIMU)
    s_anaFilt[adc_input] = temp_ana;
#endif
  }
}
