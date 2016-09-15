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

#ifndef simu_interface_h
#define simu_interface_h

#define REG8 uint8_t
#define REG16 uint16_t

//ADC
REG16 simu_adc;
REG16 simu_adcw;
REG8  simu_adcl;
REG8  simu_adch;
REG8  simu_adcra;
REG8  simu_adcsrb;
REG8  simu_admux;
REG8  simu_didr2;
REG8  simu_didrO;

#define ADC     simu_adc
#define ADCW    simu_adcw
#define ADCL    simu_adcl
#define ADCH    simu_adch

#define ADCSRA  simu_adcra
#define ADEN    7
#define ADSC    6
#define ADATE   5
#define ADIF    4
#define ADIE    3
#define ADPS2   2
#define ADPS1   1
#define ADPS0   0

#define ADCSRB  simu_adcsrb
#define ACME    6
#define MUX5    3
#define ADTS2   2
#define ADTS1   1
#define ADTS0   0

#define ADMUX   simu_admux
#define REFS1   7
#define REFS0   6
#define ADLAR   5
#define MUX4    4
#define MUX3    3
#define MUX2    2
#define MUX1    1
#define MUX0    0

#define DIDR2   simu_didr2
#define ADC15D  7
#define ADC14D  6
#define ADC13D  5
#define ADC12D  4
#define ADC11D  3
#define ADC10D  2
#define ADC9D   1
#define ADC8D   0

#define DIDR0   simu_didrO
#define ADC7D   7
#define ADC6D   6
#define ADC5D   5
#define ADC4D   4
#define ADC3D   3
#define ADC2D   2
#define ADC1D   1
#define ADC0D   0

#endif
