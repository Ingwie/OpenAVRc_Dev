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


void adcInit()
{
/* The EVO has eleven analogue inputs into the ADC (pins ADC0 - ADC10).
 * It uses the 5 Volt AVCC as the reference voltage for the ADC and the sticks
 * are supplied with the same voltage. This makes the system Ratiometric.
 * There is amplification on the stick inputs, but the sliders are fed direct into the ADC.
 * The 5V regulator has a worst case error of +/-3%, typically +/-0.4% at 25'C
 * See Atmel document AVR121 (and M2560 datasheet) about the reduction in E.N.O.B. (Effective Number of Bits)
 * when the ADC clock is greater than 200KHz.
 * We use a 250KHz ADC clock as this should provide accurate 10 bit resolution.
 */

  DIDR0 = 0xff; // Digital input disable on ADC7-0.
  DIDR2 = 0b111; // Digital input disable on ADC10-8.

  #define ADC_VREF_TYPE (1 << REFS0) // AVCC with external capacitor at AREF pin.
  ADMUX = ADC_VREF_TYPE; // Select voltage reference.
  ADCSRA = 0x86; // ADC enabled, pre-scaler division=64 (250KHz clock) (no interrupt, no auto-triggering)
}


void adcPrepareBandgap()
{
#if defined(CPUM2560) && defined(REV_EVO_V1)
  // Done in adcInit().
#else
  ADMUX = 0x1E | ADC_VREF_TYPE; // Switch MUX to internal reference.
#endif
}


void getADC()
{
  // If conversion is running, then exit - feeble re-enterant check.
  if(ADCSRA & (1 << ADSC)) return;

  uint16_t temp_ana;

  for (uint8_t adc_input=0; adc_input < (NUMBER_ANALOG); adc_input++) {
    ADMUX = ADC_VREF_TYPE | (adc_input & 0b111) ;
    if(adc_input & 0x08) ADCSRB |= (1 << MUX5);
    else ADCSRB &= ~(1 << MUX5);
    ADCSRA |= 1 << ADSC; // Start the First AD conversion
    while (ADCSRA & (1 << ADSC)); // Wait for the AD conversion to complete
    temp_ana = ADC;
    ADCSRA |= 1 << ADSC; // Start the Second AD conversion
    while (ADCSRA & (1 << ADSC)); // Wait for the AD conversion to complete
    temp_ana += ADC;
    s_anaFilt[adc_input] = temp_ana << 1; // Change to 12 Bit Value for future developments and Analogue Switches code.
  }
}


void getADC_bandgap()
{
#if defined(CPUM2560) && defined(REV_EVO_V1)
  BandGap = 5000; // 5 Volt.
#endif
}
