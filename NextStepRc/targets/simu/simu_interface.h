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

#define reg8 uint8_t
#define reg16 uint16_t

//ADC
reg8 REFS0 = 0b01000000;
reg8 ADMUX;  //Analog Comparator Multiplexed Input
reg8 ADCSRA;
reg8 ADCSRB;
reg8 MUX5 =  0b00001000;

#endif
