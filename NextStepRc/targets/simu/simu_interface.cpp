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

 #include "simu_interface.h"

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

//IIC
REG8 simu_twcr;
REG8 simu_twbr;
REG8 simu_twsr;
REG8 simu_twdr;
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
REG16 simu_tcnt1;
REG8 simu_tcnt1l;
REG8 simu_tcnt1h;
REG8 simu_tccr1a;
REG8 simu_tccr1c;
REG8 simu_tccr1b;
REG8 simu_timsk1;
REG8 simu_tifr1;
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

 void eepromReadBlock (uint8_t * pointer_ram, uint32_t pointer_eeprom, uint32_t size)
{
 /* assert(size);

  if (fp) {
    // TRACE("EEPROM read (pos=%d, size=%d)", pointer_eeprom, size);
    if (fseek(fp, (long)pointer_eeprom, SEEK_SET)==-1) perror("error in fseek");
    if (fread(pointer_ram, size, 1, fp) <= 0) perror("error in fread");
  }
  else {
    memcpy(pointer_ram, &eeprom[(uint64_t)pointer_eeprom], size);
  }*/
}

