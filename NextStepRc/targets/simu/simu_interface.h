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

#include <inttypes.h>
#include <assert.h>



typedef const unsigned char pm_uchar;
typedef const char pm_char;
typedef const uint16_t pm_uint16_t;
typedef const uint8_t pm_uint8_t;
typedef const int16_t pm_int16_t;
typedef const int8_t pm_int8_t;


#define REG8 uint8_t
#define REG16 uint16_t

//Function
extern unsigned char simu_eeprom[4096];
extern void eepromReadBlock (uint8_t * pointer_ram, uint16_t pointer_eeprom, uint16_t size);


extern bool simu_mainloop_is_runing; //main run signal
extern bool simu_shutDownSimu_is_runing; //main run signal
extern bool simu_off; //Off signal



//PORT & PIN
extern REG8  simu_pina;
extern REG8  simu_ddra;
extern REG8  simu_porta;
extern REG8  simu_pinb;
extern REG8  simu_ddrb;
extern REG8  simu_portb;
extern REG8  simu_pinc;
extern REG8  simu_ddrc;
extern REG8  simu_portc;
extern REG8  simu_pind;
extern REG8  simu_ddrd;
extern REG8  simu_portd;
extern REG8  simu_pine;
extern REG8  simu_ddre;
extern REG8  simu_porte;
extern REG8  simu_pinf;
extern REG8  simu_ddrf;
extern REG8  simu_portf;
extern REG8  simu_ping;
extern REG8  simu_ddrg;
extern REG8  simu_portg;
extern REG8  simu_pinh;
extern REG8  simu_ddrh;
extern REG8  simu_porth;
extern REG8  simu_pinj;
extern REG8  simu_ddrj;
extern REG8  simu_portj;
extern REG8  simu_pink;
extern REG8  simu_ddrk;
extern REG8  simu_portk;
extern REG8  simu_pinl;
extern REG8  simu_ddrl;
extern REG8  simu_portl;

#define PINA    simu_pina
#define PINA7   7
#define PINA6   6
#define PINA5   5
#define PINA4   4
#define PINA3   3
#define PINA2   2
#define PINA1   1
#define PINA0   0

#define DDRA    simu_ddra
#define DDA7    7
#define DDA6    6
#define DDA5    5
#define DDA4    4
#define DDA3    3
#define DDA2    2
#define DDA1    1
#define DDA0    0

#define PORTA   simu_porta
#define PA7     7
#define PA6     6
#define PA5     5
#define PA4     4
#define PA3     3
#define PA2     2
#define PA1     1
#define PA0     0

#define PINB    simu_pinb
#define PINB7   7
#define PINB6   6
#define PINB5   5
#define PINB4   4
#define PINB3   3
#define PINB2   2
#define PINB1   1
#define PINB0   0

#define DDRB    simu_ddrb
#define DDB7    7
#define DDB6    6
#define DDB5    5
#define DDB4    4
#define DDB3    3
#define DDB2    2
#define DDB1    1
#define DDB0    0

#define PORTB   simu_portb
#define PB7     7
#define PB6     6
#define PB5     5
#define PB4     4
#define PB3     3
#define PB2     2
#define PB1     1
#define PB0     0

#define PINC    simu_pinc
#define PINC7   7
#define PINC6   6
#define PINC5   5
#define PINC4   4
#define PINC3   3
#define PINC2   2
#define PINC1   1
#define PINC0   0

#define DDRC    simu_ddrc
#define DDC7    7
#define DDC6    6
#define DDC5    5
#define DDC4    4
#define DDC3    3
#define DDC2    2
#define DDC1    1
#define DDC0    0

#define PORTC   simu_portc
#define PC7     7
#define PC6     6
#define PC5     5
#define PC4     4
#define PC3     3
#define PC2     2
#define PC1     1
#define PC0     0

#define PIND    simu_pind
#define PIND7   7
#define PIND6   6
#define PIND5   5
#define PIND4   4
#define PIND3   3
#define PIND2   2
#define PIND1   1
#define PIND0   0

#define DDRD    simu_ddrd
#define DDD7    7
#define DDD6    6
#define DDD5    5
#define DDD4    4
#define DDD3    3
#define DDD2    2
#define DDD1    1
#define DDD0    0

#define PORTD   simu_portd
#define PD7     7
#define PD6     6
#define PD5     5
#define PD4     4
#define PD3     3
#define PD2     2
#define PD1     1
#define PD0     0

#define PINE    simu_pine
#define PINE7   7
#define PINE6   6
#define PINE5   5
#define PINE4   4
#define PINE3   3
#define PINE2   2
#define PINE1   1
#define PINE0   0

#define DDRE    simu_ddre
#define DDE7    7
#define DDE6    6
#define DDE5    5
#define DDE4    4
#define DDE3    3
#define DDE2    2
#define DDE1    1
#define DDE0    0

#define PORTE   simu_porte
#define PE7     7
#define PE6     6
#define PE5     5
#define PE4     4
#define PE3     3
#define PE2     2
#define PE1     1
#define PE0     0

#define PINF    simu_pinf
#define PINF7   7
#define PINF6   6
#define PINF5   5
#define PINF4   4
#define PINF3   3
#define PINF2   2
#define PINF1   1
#define PINF0   0

#define DDRF    simu_ddrf
#define DDF7    7
#define DDF6    6
#define DDF5    5
#define DDF4    4
#define DDF3    3
#define DDF2    2
#define DDF1    1
#define DDF0    0

#define PORTF   simu_portf
#define PF7     7
#define PF6     6
#define PF5     5
#define PF4     4
#define PF3     3
#define PF2     2
#define PF1     1
#define PF0     0

#define PING    simu_ping
#define PING5   5
#define PING4   4
#define PING3   3
#define PING2   2
#define PING1   1
#define PING0   0

#define DDRG    simu_ddrg
#define DDG5    5
#define DDG4    4
#define DDG3    3
#define DDG2    2
#define DDG1    1
#define DDG0    0

#define PORTG   simu_portg
#define PG5     5
#define PG4     4
#define PG3     3
#define PG2     2
#define PG1     1
#define PG0     0

# define PINH   simu_pinh
# define PINH7  7
# define PINH6  6
# define PINH5  5
# define PINH4  4
# define PINH3  3
# define PINH2  2
# define PINH1  1
# define PINH0  0

# define DDRH   simu_ddrh
# define DDH7   7
# define DDH6   6
# define DDH5   5
# define DDH4   4
# define DDH3   3
# define DDH2   2
# define DDH1   1
# define DDH0   0

# define PORTH  simu_porth
# define PH7    7
# define PH6    6
# define PH5    5
# define PH4    4
# define PH3    3
# define PH2    2
# define PH1    1
# define PH0    0

# define PINJ   simu_pinj
# define PINJ7  7
# define PINJ6  6
# define PINJ5  5
# define PINJ4  4
# define PINJ3  3
# define PINJ2  2
# define PINJ1  1
# define PINJ0  0

# define DDRJ   simu_ddrj
# define DDJ7   7
# define DDJ6   6
# define DDJ5   5
# define DDJ4   4
# define DDJ3   3
# define DDJ2   2
# define DDJ1   1
# define DDJ0   0

# define PORTJ  simu_portj
# define PJ7 7
# define PJ6 6
# define PJ5 5
# define PJ4 4
# define PJ3 3
# define PJ2 2
# define PJ1 1
# define PJ0 0

# define PINK   simu_pink
# define PINK7  7
# define PINK6  6
# define PINK5  5
# define PINK4  4
# define PINK3  3
# define PINK2  2
# define PINK1  1
# define PINK0  0

# define DDRK   simu_ddrk
# define DDK7   7
# define DDK6   6
# define DDK5   5
# define DDK4   4
# define DDK3   3
# define DDK2   2
# define DDK1   1
# define DDK0   0

# define PORTK  simu_portk
# define PK7 7
# define PK6 6
# define PK5 5
# define PK4 4
# define PK3 3
# define PK2 2
# define PK1 1
# define PK0 0

# define PINL   simu_pinl
# define PINL7  7
# define PINL6  6
# define PINL5  5
# define PINL4  4
# define PINL3  3
# define PINL2  2
# define PINL1  1
# define PINL0  0

# define DDRL   simu_ddrl
# define DDL7   7
# define DDL6   6
# define DDL5   5
# define DDL4   4
# define DDL3   3
# define DDL2   2
# define DDL1   1
# define DDL0   0

# define PORTL  simu_portl
# define PL7 7
# define PL6 6
# define PL5 5
# define PL4 4
# define PL3 3
# define PL2 2
# define PL1 1
# define PL0 0

//ADC
extern REG16 simu_adc;
extern REG16 simu_adcw;
extern REG8  simu_adcl;
extern REG8  simu_adch;
extern REG8  simu_adcra;
extern REG8  simu_adcsrb;
extern REG8  simu_admux;
extern REG8  simu_didr2;
extern REG8  simu_didrO;

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

//EEPROM
extern REG8 simu_eecr;
extern REG8 simu_eedr;
extern REG16 simu_eear;
#define EEAR    simu_eear
#define EECR    simu_eecr
#define EEDR    simu_eedr
#define EEPM1   5
#define EEPM0   4
#define EERIE   3
#define EEMPE   2
#define EEPE    1
#define EERE    0

//IIC
extern REG8 simu_twcr;
extern REG8 simu_twbr;
extern REG8 simu_twsr;
extern REG8 simu_twdr;
extern REG8 simu_tw_start;
extern REG8 simu_tw_status;
extern REG8 simu_tw_mt_sla_ack;
extern REG8 simu_tw_mr_sla_ack;
extern REG8 simu_tw_mt_data_ack;

#define TW_START        simu_tw_start
#define TW_STATUS       simu_tw_status
#define TW_MT_SLA_ACK   simu_tw_mt_sla_ack
#define TW_MR_SLA_ACK   simu_tw_mr_sla_ack
#define TW_MT_DATA_ACK  simu_tw_mt_data_ack
#define TWDR    simu_twdr
#define TWBR    simu_twbr
#define TWCR    simu_twcr
#define TWINT   7
#define TWEA    6
#define TWSTA   5
#define TWSTO   4
#define TWWC    3
#define TWEN    2
#define TWIE    0
#define TWSR    simu_twsr
#define TWS7    7
#define TWS6    6
#define TWS5    5
#define TWS4    4
#define TWS3    3
#define TWPS1   1
#define TWPS0   0


//MCU
extern REG8 simu_mcusr;

#define MCUSR   simu_mcusr
#define JTRF    4
#define WDRF    3
#define BORF    2
#define EXTRF   1
#define PORF    0

//TIMER
extern REG16 simu_ocr1a;
extern REG16 simu_ocr1b;
extern REG16 simu_ocr1c;
extern REG16 simu_tcnt1;
extern REG8 simu_tcnt1l;
extern REG8 simu_tcnt1h;
extern REG8 simu_tccr1a;
extern REG8 simu_tccr1c;
extern REG8 simu_tccr1b;
extern REG8 simu_timsk1;
extern REG8 simu_tifr1;
extern REG16 simu_icr1;
extern REG8 simu_icr1l;
extern REG8 simu_icr1h;
extern REG8 simu_tcnt2;
extern REG8 simu_ocr2a;
extern REG8 simu_ocr2b;
extern REG8 simu_timsk3;
extern REG16 simu_ocr3a;
extern REG8 simu_ocr3al;
extern REG8 simu_ocr3ah;
extern REG16 simu_ocr3b;
extern REG8 simu_ocr3bl;
extern REG8 simu_ocr3bh;
extern REG16 simu_icr3;
extern REG8 simu_icr3l;
extern REG8 simu_icr3h;
extern REG8 simu_tccr5c;
extern REG16 simu_tcnt5;
extern REG8 simu_tcnt5l;
extern REG8 simu_tcnt5h;
extern REG8 simu_tccr5b;
extern REG8 simu_timsk5;
extern REG16 simu_ocr5a;
extern REG8 simu_ocr5al;
extern REG8 simu_ocr5ah;

#define OCR1A   simu_ocr1a
#define OCR1B   simu_ocr1b
#define OCR1C   simu_ocr1c

#define TCCR1A  simu_tccr1a
#define COM1A1  7
#define COM1A0  6
#define COM1B1  5
#define COM1B0  4
#define COM1C1  3
#define COM1C0  2
#define WGM11   1
#define WGM10   0

#define TCCR1B  simu_tccr1b
#define ICNC1   7
#define ICES1   6
#define WGM13   4
#define WGM12   3
#define CS12    2
#define CS11    1
#define CS10    0

#define TCCR1C  simu_tccr1c
#define FOC1A   7
#define FOC1B   6
#define FOC1C   5

#define TIMSK1  simu_timsk1
#define ICIE1   5
#define OCIE1C  3
#define OCIE1B  2
#define OCIE1A  1
#define TOIE1   0

#define TCNT1   simu_tcnt1
#define TCNT1L  simu_tcnt1l
#define TCNT1H  simu_tcnt1h

#define ICR1    simu_icr1
#define ICR1L   simu_icr1l
#define ICR1H   simu_icr1h


#define TIFR1   simu_tifr1
#define ICF1    5
#define OCF1C   3
#define OCF1B   2
#define OCF1A   1
#define TOV1    0

#define OCR3A   simu_ocr3a
#define OCR3AL  simu_ocr3al
#define OCR3AH  simu_ocr3ah

#define TCNT2   simu_tcnt2
#define OCR2A   simu_ocr2a
#define OCR2B   simu_ocr2b

#define OCR3B   simu_ocr3b
#define OCR3BL  simu_ocr3bl
#define OCR3BH  simu_ocr3bh

#define ICR3    simu_icr3
#define ICR3L   simu_icr3l
#define ICR3H   simu_icr3h

#define TIMSK3  simu_timsk3
#define ICIE3   5
#define OCIE3C  3
#define OCIE3B  2
#define OCIE3A  1
#define TOIE3   0

#define TCCR5B  simu_tccr5b
#define ICNC5   7
#define ICES5   6
#define WGM53   4
#define WGM52   3
#define CS52    2
#define CS51    1
#define CS50    0

#define TCCR5C  simu_tccr5c
#define FOC5A   7
#define FOC5B   6
#define FOC5C   5

#define TCNT5   simu_tcnt5
#define TCNT5L  simu_tcnt5l
#define TCNT5H  simu_tcnt5h
#define TIMSK5  simu_timsk5
#define ICIE5   5
#define OCIE5C  3
#define OCIE5B  2
#define OCIE5A  1
#define TOIE5   0
#define OCR5A   simu_ocr5a
#define OCR5AL  simu_ocr5al
#define OCR5AH  simu_ocr5ah

//UART
extern REG8 simu_ucsr0b;
extern REG8 simu_ucsr0c;
extern REG8 simu_ucsr0a;
extern REG16 simu_ubrr0;
extern REG8 simu_ubrrOl;
extern REG8 simu_ubrrOh;
extern REG8 simu_udr0;

#define UCSR0A  simu_ucsr0a
#define RXC0    7
#define TXC0    6
#define UDRE0   5
#define FE0     4
#define DOR0    3
#define UPE0    2
#define U2X0    1
#define MPCM0   0

#define UCSR0B  simu_ucsr0b
#define RXCIE0  7
#define TXCIE0  6
#define UDRIE0  5
#define RXEN0   4
#define TXEN0   3
#define UCSZ02  2
#define RXB80   1
#define TXB80   0

#define UCSR0C  simu_ucsr0c
#define UMSEL01 7
#define UMSEL00 6
#define UPM01   5
#define UPM00   4
#define USBS0   3
#define UCSZ01  2
#define UCSZ00  1
#define UCPOL0  0

#define UBRR0   simu_ubrr0
#define UBRR0L  simu_ubrrOl
#define UBRR0H  simu_ubrrOh
#define UDR0    simu_udr0

//SPI
extern REG8 simu_spcr;
extern REG8 simu_spsr;
extern REG8 simu_spdr;

#define SPCR    simu_spcr
#define SPIE    7
#define SPE     6
#define DORD    5
#define MSTR    4
#define CPOL    3
#define CPHA    2
#define SPR1    1
#define SPR0    0

#define SPSR    simu_spsr
#define SPIF    7
#define WCOL    6
#define SPI2X   0

#define SPDR    simu_spdr

//STACK
extern unsigned char * simu_stackadd;
extern REG8 simu_bssebd;
#define STACKPTR simu_stackadd
#define __bss_end simu_bssebd
//IRQ
#define ISR(x, ...)  void x()

//AVR
#define PROGMEM
#define pgm_read_byte(address_short) (*(uint8_t*)(address_short))
#define pgm_read_word(address_short) (*(uint16_t*)(address_short))
#define pgm_read_adr(address_short) *address_short
#define pgm_read_stringP(adr) ((adr))
#define PSTR(adr) adr
#define _delay_us(a)
#define _delay_ms(a)
#define cli()
#define sei()
#define strcpy_P strcpy
#define strcat_P strcat
#define memcpy_P memcpy
#define bit_is_clear(sfr, bit) (!(REG8(sfr) & (1 << (bit))))
#define loop_until_bit_is_set(sfr, bit) do {lcdInit();} while (bit_is_clear(sfr, bit))
#define mcusr MCUSR
//#undef assert
//#define assert(x)
//#define printf(x)

#endif
