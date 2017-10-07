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


#include "simu_interface.h"
#include "../../OpenAVRc.h"

bool simu_mainloop_is_runing; //main run signal
bool simu_shutDownSimu_is_runing; //main run signal
bool simu_off; //Off signal

//EEPROM
#if defined(EXTERNALEEPROM)
unsigned char simu_eeprom[10240] = {0xFF};
#else
unsigned char simu_eeprom[4096] = {0xFF};
#endif
extern uint16_t eeprom_pointer;
extern uint8_t * eeprom_buffer_data;


//AVR REG

//PORT & PIN
REG8  simu_pina;
REG8  simu_ddra;
REG8  simu_porta;
REG8  simu_pinb;
REG8  simu_ddrb;
REG8  simu_portb;
REG8  simu_pinc;
REG8  simu_ddrc;
REG8  simu_portc;
REG8  simu_pind;
REG8  simu_ddrd;
REG8  simu_portd;
REG8  simu_pine;
REG8  simu_ddre;
REG8  simu_porte;
REG8  simu_pinf;
REG8  simu_ddrf;
REG8  simu_portf;
REG8  simu_ping;
REG8  simu_ddrg;
REG8  simu_portg;
REG8  simu_pinh;
REG8  simu_ddrh;
REG8  simu_porth;
REG8  simu_pinj;
REG8  simu_ddrj;
REG8  simu_portj;
REG8  simu_pink;
REG8  simu_ddrk;
REG8  simu_portk;
REG8  simu_pinl;
REG8  simu_ddrl;
REG8  simu_portl;

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

//EEPROM
REG8 simu_eecr;
REG8 simu_eedr;
REG16 simu_eear;

//IIC
REG8 simu_twcr;
REG8 simu_twbr;
REG8 simu_twsr;
REG8 simu_twdr;
REG8 simu_tw_rep_start;
REG8 simu_tw_start;
REG8 simu_tw_status;
REG8 simu_tw_mt_sla_ack;
REG8 simu_tw_mr_sla_ack;
REG8 simu_tw_mt_data_ack;

//MCU
REG8 simu_mcusr;

//TIMER
REG16 simu_ocr1a;
REG16 simu_ocr1b;
REG16 simu_ocr1c;
REG16 simu_tcnt1;
REG8 simu_tcnt1l;
REG8 simu_tcnt1h;
REG8 simu_tccr1a;
REG8 simu_tccr1c;
REG8 simu_tccr1b;
REG8 simu_timsk1;
REG8 simu_tifr1;
REG16 simu_icr1;
REG8 simu_icr1l;
REG8 simu_icr1h;
REG8 simu_timsk3;
REG16 simu_ocr3a;
REG8 simu_ocr3al;
REG8 simu_ocr3ah;
REG16 simu_ocr3b;
REG8 simu_ocr3bl;
REG8 simu_ocr3bh;
REG8 simu_tcnt2;
REG8 simu_ocr2a;
REG8 simu_ocr2b;
REG16 simu_icr3;
REG8 simu_icr3l;
REG8 simu_icr3h;

REG8 simu_tccr5c;
REG16 simu_tcnt5;
REG8 simu_tcnt5l;
REG8 simu_tcnt5h;
REG8 simu_tccr5b;
REG8 simu_timsk5;
REG16 simu_ocr5a;
REG8 simu_ocr5al;
REG8 simu_ocr5ah;

//UART
REG8 simu_ucsr0b;
REG8 simu_ucsr0c;
REG8 simu_ucsr0a;
REG16 simu_ubrr0;
REG8 simu_ubrrOl;
REG8 simu_ubrrOh;
REG8 simu_udr0;

//SPI
REG8 simu_spcr;
REG8 simu_spsr;
REG8 simu_spdr;

//STACK
unsigned char * simu_stackadd;
REG8 simu_bssebd;


//END SIMU REG

void eepromReadBlock (uint8_t * pointer_ram, uint16_t pointer_eeprom, uint16_t size)
{
  assert(size);
  memcpy(pointer_ram, &simu_eeprom[pointer_eeprom], size);
}

