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

#ifndef simpgmspace_h
#define simpgmspace_h

extern int g_snapshot_idx;

#ifndef __GNUC__
#include <windows.h>
#define sleep(x) Sleep(x)
#else
#include <unistd.h>
#define sleep(x) usleep(1000*x)
#endif

#ifdef SIMU_EXCEPTIONS
extern char * main_thread_error;
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#if defined(WIN32) || !defined(__GNUC__)
#define write_backtrace(output)
#else
#include <execinfo.h>
inline void write_backtrace(char *output)
{
  void *buf[16];
  char **s;
  int n = backtrace(buf, 16);
  s = backtrace_symbols(buf, n);
  if (s) {
    for (int i=0; i<n; ++i) {
      sprintf(output+strlen(output), "%02i: %s\n", i, s[i]);
    }
  }
}
#endif
void sig(int sgn)
{
  main_thread_error = (char *)malloc(2048);
  sprintf(main_thread_error,"Signal %d caught\n", sgn);
  write_backtrace(main_thread_error);
  throw std::exception();
}
#define assert(x) do { if (!(x)) { main_thread_error = (char *)malloc(2048); sprintf(main_thread_error, "Assert failed, %s:%d: %s\n", __FILE__, __LINE__, #x); write_backtrace(main_thread_error); throw std::exception(); } } while(0)
#else
#include <assert.h>
#endif

#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>

#undef min
#undef max

#define APM
#define __REV

typedef const unsigned char pm_uchar;
typedef const char pm_char;
typedef const uint16_t pm_uint16_t;
typedef const uint8_t pm_uint8_t;
typedef const int16_t pm_int16_t;
typedef const int8_t pm_int8_t;

extern sem_t *eeprom_write_sem;

#if !defined(EEPROM_RLC)
extern uint32_t eeprom_pointer;
extern uint8_t * eeprom_buffer_data;
extern volatile int32_t eeprom_buffer_size;
extern bool eeprom_read_operation;
extern volatile uint32_t Spi_complete;
#endif

#define PIOA 0
#define PIOB 0
#define PIOC 0

#define loop_until_bit_is_set( port, bitnum) \
  while ( 0/*! ( (port) & (1 << (bitnum)) )*/ ) ;

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

#define PORTA dummyport
#define PORTB portb
#define PORTC portc
#define PORTD dummyport
#define PORTE dummyport
#define PORTF dummyport
#define PORTG dummyport
#define PORTH porth
#define PORTL dummyport
#define DDRA  dummyport
#define DDRB  dummyport
#define DDRC  dummyport
#define DDRD  dummyport
#define DDRE  dummyport
#define DDRF  dummyport
#define DDRG  dummyport
#define PINA  ~pina
#define PINB  ~pinb
#define PINC  ~pinc
#define PIND  ~pind
#define PINE  ~pine
#define PINF  ~pinf
#define PING  ~ping
#define PINH  ~pinh
#define PINJ  ~pinj
#define PINL  ~pinl
#define EEMEM

#define UCSR0B dummyport
#define UDRIE0 dummyport
#define TXB80 dummyport
#define TXCIE0 dummyport
#define TXEN0 dummyport
#define RXEN0 dummyport
#define DDE0 dummyport
#define PORTE0 dummyport
#define RXCIE0 dummyport
#define OCR0A dummyport
#define OCR1A dummyport16
#define OCR1B dummyport16
#define OCR1C dummyport16
#define OCR2 dummyport
#define OCR3A dummyport16
#define OCR3B dummyport16
#define OCR4A dummyport
#define OCR5A dummyport
#define TCCR0A dummyport
#define TCCR1A dummyport
#define TCCR1B dummyport
#define TCCR1C dummyport
#define COM1B0 dummyport
#define COM0A0 dummyport
#define TCNT1 dummyport16
#define TCNT1L dummyport
#define TCNT5 dummyport16
#define ICR1 dummyport16
#define TIFR dummyport
#define TIFR1 dummyport
#define ETIFR dummyport

#define SPDR dummyport
#define SPSR dummyport
#define SPIF dummyport
#define SPCR dummyport

#define TIMSK  dummyport
#define TIMSK1 dummyport
#define TIMSK3 dummyport
#define TIMSK4 dummyport
#define TIMSK5 dummyport
#define ETIMSK  dummyport
#define ETIMSK1 dummyport

#define UCSZ02 dummyport
#define UCSR0C dummyport
#define UCSZ01 dummyport
#define UCSZ00 dummyport
#define UCSR0A dummyport
#define RXC0 dummyport

#define UDR0 dummyport
#define OCIE1A dummyport
#define OCIE1B dummyport
#define OCIE1C dummyport
#define OCIE4A dummyport
#define OCIE5A dummyport

#define OUT_B_LIGHT   7
#define INP_E_ElevDR  2
#define INP_E_Trainer 5
#define INP_E_Gear    4
#define INP_C_ThrCt   6
#define INP_C_AileDR  7
#define INP_E_ID2     6

#define WGM10   0
#define WGM12   0
#define COM1B1  0
#define FOC1B   0
#define CS10    0
#define DOR0    0
#define UPE0    0
#define FE0     0

#define ISR(x, ...)  void x()
#define asm(x)


extern volatile unsigned char pina, pinb, pinc, pind, pine, pinf, ping, pinh, pinj, pinl;
extern uint8_t portb, portc, porth, dummyport;
extern uint16_t dummyport16;
extern uint8_t main_thread_running;

//#define getADC()
//#define getADC_bandgap()

#define SIMU_SLEEP(x) do { if (!main_thread_running) return; sleep(x/*ms*/); } while (0)
#define SIMU_SLEEP_NORET(x) do { sleep(x/*ms*/); } while (0)

void simuInit();

void simuSetKey(uint8_t key, bool state);
void simuSetTrim(uint8_t trim, bool state);
void simuSetSwitch(uint8_t swtch, int8_t state);

void StartMainThread(bool tests=true);
void StopMainThread();
void StartEepromThread(const char *filename="eeprom.bin");
void StopEepromThread();
  #define StartAudioThread(dummy)
  #define StopAudioThread()

extern const char * eepromFile;
void eepromReadBlock (uint8_t * pointer_ram, uint32_t address, uint32_t size);

#define wdt_enable(...) sleep(1/*ms*/)
#define wdt_reset() sleep(1/*ms*/)
#define boardInit()
#define boardOff()

#define OS_MutexID pthread_mutex_t
extern OS_MutexID audioMutex;

#define OS_FlagID uint32_t
#define OS_TID uint32_t
#define OS_TCID uint32_t
#define OS_STK uint32_t

#define E_OK   0
#define WDRF   0

#define CoInitOS(...)
#define CoStartOS(...)
#define CoCreateTask(...) (0)
#define CoCreateMutex(...) PTHREAD_MUTEX_INITIALIZER
#define CoSetFlag(...)
#define CoClearFlag(...)
#define CoSetTmrCnt(...)
#define CoEnterISR(...)
#define CoExitISR(...)
#define CoStartTmr(...)
#define CoWaitForSingleFlag(...) 0
#define CoEnterMutexSection(m) pthread_mutex_lock(&(m))
#define CoLeaveMutexSection(m) pthread_mutex_unlock(&(m))
#define CoTickDelay(...)
#define CoCreateFlag(...) 0
#define CoGetOSTime(...) 0
#define UART_Stop(...)
#define UART3_Stop(...)
#define USART_GetITStatus(...) 0

inline void delay_01us(int dummy) { }
#define configure_pins(...)

#if defined(SDCARD)
extern char simuSdDirectory[1024];
#endif

//#define sdMountPoll()
//#define sdPoll10ms()
//#define sd_card_ready()  (true)
//#define sdMounted()      (true)

#endif
