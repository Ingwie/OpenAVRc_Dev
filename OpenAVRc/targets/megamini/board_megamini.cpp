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


#include "../../i2c_master.h"
#include "../../OpenAVRc.h"


FORCEINLINE void boardInit()
{
  // Set up I/O port data directions and initial states (unused pin setting : input, pull-up on)
  DDRA = 0b11111111;  PORTA = 0b00000000; // LCD data
  DDRB = 0b11100111;  PORTB = 0b10111111; // 7:MegaCore Led, 6:PPM_OUT, 5:PPMSIM_OUT, 4:N/A, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
  DDRC = 0b11111100;  PORTC = 0b00000011; // 7-3:LCD, 2:BackLight, 1:ID2_SW, 0:ID1_SW
  DDRD = 0b00000000;  PORTD = 0b11111111; // 7:AilDR_SW, 6:N/A, 5:N/A, 4:PPM_IN, 3:N/A, 2:N/A, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00000010;  PORTE = 0b11111110; // 7:Rot_2A, 6:Rot_2B, 5:Rot_1B, 4:Rot_1A, 3:Rot_2_Push, 2:N/A, 1:TELEM_TX, 0:TELEM_RX
  DDRF = 0b00000000;  PORTF = 0b11111111; // 7-0:Trim switch inputs
  DDRG = 0b00000000;  PORTG = 0b11111111; // 7:N/A, 6:N/A, 5:Rot_1_Push, 4:N/A, 3:N/A, 2:TCut_SW, 1:Gear_SW, 0: RudDr_SW
  DDRH = 0b01111110;  PORTH = 0b10010000; // 7:N/A, 6:Sim_Control/RF_Power, 5:Haptic, 4:Hold_Power, 3:Speaker, 2:SPI_Xmitter_SCK, 1:SPI_Xmitter_MOSI, 0:SPI_Xmitter_MISO
  DDRJ = 0b01111010;  PORTJ = 0b11111111; // 7:N/A, 6:CS_A7105, 5:CS_NRF24L01, 4:CS_CYRF6936, 3:CS_CC2500, 2:N/A, 1:JQ6500Data , 0:JQ6500Busy
  DDRK = 0b00000000;  PORTK = 0b00000000; // Analogic input (no pull-ups)
  DDRL = 0b00000000;  PORTL = 0b11111111; // 7:TRN_SW 6:EleDR_SW, 5:ESC, 4:MENU 3:Keyb_Left, 2:Keyb_Right, 1:Keyb_Up, 0:Keyb_Down

  adcInit();

#ifndef SIMU

  /**** Set up timer/counter 0 ****/
  // TCNT0  10ms = 16MHz/1024/156(.25) periodic timer (100ms interval)
  //        cycles at 9.984ms but includes 1:4 duty cycle correction to /157 to average at 10.0ms
  TCCR2B  = (0b111 << CS20); // Norm mode, clk/1024 (differs from ATmega64 chip)
  OCR2A   = 156;
  TIMSK2 |= (1<<OCIE2A) |  (1<<TOIE2); // Enable Output-Compare and Overflow interrupts


#if defined(AUDIO)
  // TIMER4 (16bit) set to mode 9, prescaler 16MHz/64=250kHz.
  // Used for audio tone generation.
 #define AUDIO_OCRxA OCR4A // used in audio_avr.cpp
  TCCR4B = (0b10<<WGM42) | (0b011 << CS40);
  TCCR4A = (0b01<<WGM40);
#endif

  // Setup Timer 1.
  // PB5 OC1A for Protocol callback and PPMSIM.
  // PB6 OC1B for PPM.
  // PPM16 uses PB6 OC1B for first 8 channels, PB5 OC1A for last 8 channels.
  // PD4 ICP1 for capture of Trainer (Pupil) PPM.
  // Normal mode (0), OVF @ TOP (0xFFFF), F_CPU/8, Noise canceler on ICP, Falling edge trigger.
  // This timer should be left to run at all times. DO NOT DISABLE.
  TCCR1A = 0;
  TCCR1C = 0;
  TCCR1B = (2<<CS10);
  // Set up for input capture on PD4 (ICP1).
  TCCR1B &= ~(1<<ICES1); // Falling edge trigger, ICP1 should have pull-up enabled.
  // Should work better when connected to open collector stages and when left unconnected.
  TCCR1B |= (1<<ICNC1); // Input Capture Noise Canceler.
  TIMSK1 = 0;


#if defined(VOICE_JQ6500)

  #define TLM_JQ6500 3 // use uart3 on mega board (TX = PJ1)
  InitJQ6500UartTx();

#endif

  /* Rotary encoder interrupt set-up                 */
  EIMSK = 0; // disable ALL external interrupts.
  // encoder 1&2
  EICRB = (1<<ISC71) | (1<<ISC61) | (1<<ISC51) | (1<<ISC41); // 10 = interrupt on falling edge
  EIFR = (1<<INTF7) | (1<<INTF6) | (1<<INTF5) | (1<<INTF4); // clear the int. flag in case it got set when changing modes
  ENABLEROTENCAISR();
  ENABLEROTENCBISR();

#if defined(RTCLOCK) || defined(LCD_SSD1306) || defined(LCD_SH1106) || defined(EXTERNALEEPROM)
  /* Hardware I2C init */
  i2c_init();
#endif

#if defined(SPIMODULES)
// Setup (M)SPI Port.
// USART2
// PORTH0 RXD2
// PORTH1 TXD2
// PORTH2 XCK2

// Setup USART in MSPI mode.
// Initialisation of USART.
  UBRR2 = 0; // Reset is part of initialisation sequence.
  UCSR2C = 0xC0; // UMSEL21:0 = 3 DORD2=0 CPHA2=0 CPOL2=0  USART in Master SPI mode, MSB first, Clock phase=0 Polarity=0.
  UCSR2B = (1 << RXEN2) | (1 << TXEN2); // Transmit and Receive.
  UBRR2 = 1; // 4.0MHz clock ... 16MHz/(2*(UBRR+1))

  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();
  RF_CS_NRF24L01_INACTIVE();
  RF_CS_A7105_INACTIVE();

  RFModule.mode = NORMAL_MODE;

#endif // SPIMODULES

#endif // !SIMU

 WAIT_PUPIL();

 LEDOFF();
}

uint8_t USART2_mspi_xfer(uint8_t data)
{
#if !defined(SIMU)
/* Wait for empty transmit buffer */
  while ( !( UCSR2A & (1<<UDRE2)) );
  /* Put data into buffer, sends the data */
  UDR2 = data;
  /* Wait for data to be received */
  while ( !(UCSR2A & (1<<RXC2)) );
  /* Get and return received data from buffer */
  return UDR2;
#else
TRACE("MSPI Send to RF module : ", data);
return 0;
#endif
}

void boardOff()
{
  PORTH &= ~ INP_H_Hold_Power;   // PortH-4 set to 0
}

uint8_t keyDown()
{
  return ((~PINL) & 0x3F) || ROTENC_DOWN();
}

bool switchState(EnumKeys enuk)
{
  uint8_t result = 0 ;

  if (enuk < (uint8_t)DIM(keys))
    return keys[enuk].state() ? 1 : 0;

  switch(enuk) {
  case SW_ELE:
    result = !(PINL & INP_L_ElevDR);
    break;

  case SW_AIL:
    result = !(PIND & INP_D_AileDR);
    break;

  case SW_RUD:
    result = !(PING & INP_G_RuddDR);
    break;

  //       INP_C_ID1  INP_C_ID2
  // ID0      0          1
  // ID1      1          1
  // ID2      1          0
  case SW_ID0:
    result = !(PINC & INP_C_ID1);
    break;

  case SW_ID1:
    result = (PINC & INP_C_ID1) && (PINC & INP_C_ID2);
    break;

  case SW_ID2:
    result = !(PINC & INP_C_ID2);
    break;

  case SW_GEA:
    result = !(PING & INP_G_Gear);
    break;

  case SW_THR:
    result = !(PING & INP_G_ThrCt);
    break;

  case SW_TRN:
    result = !(PINL & INP_L_Trainer);
    break;

  default:
    break;
  }

  return result;
}

// Trim switches
#if defined(TOGGLETRIM) //Toggle trim usage -> Left trim for right stick and right trim for left stick
static const pm_uchar crossTrim[] PROGMEM = {
  TRIMS_GPIO_PIN_RHL,
  TRIMS_GPIO_PIN_RHR,
  TRIMS_GPIO_PIN_RVD,
  TRIMS_GPIO_PIN_RVU,
  TRIMS_GPIO_PIN_LVD,
  TRIMS_GPIO_PIN_LVU,
  TRIMS_GPIO_PIN_LHL,
  TRIMS_GPIO_PIN_LHR
};
#else
static const pm_uchar crossTrim[] PROGMEM = {
  TRIMS_GPIO_PIN_LHL,
  TRIMS_GPIO_PIN_LHR,
  TRIMS_GPIO_PIN_LVD,
  TRIMS_GPIO_PIN_LVU,
  TRIMS_GPIO_PIN_RVD,
  TRIMS_GPIO_PIN_RVU,
  TRIMS_GPIO_PIN_RHL,
  TRIMS_GPIO_PIN_RHR
};
#endif

uint8_t trimDown(uint8_t idx)
{
  uint8_t in = ~PINF;
  return (in & pgm_read_byte_near(crossTrim+idx));
}

void readKeysAndTrims()
{

#if defined(NAVIGATION_STICKS)
  if (~PINL & (KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT)) {   //Check menu key
    StickScrollTimer = STICK_SCROLL_TIMEOUT;
    uint8_t sticks_evt = getSticksNavigationEvent();
    if (sticks_evt) {
      if (~PINL & KEYS_GPIO_PIN_MENU) {
        putEvent(EVT_KEY_LONG(sticks_evt)); // create a stick based event "long" to choose menu
      } else {
        putEvent(EVT_KEY_BREAK(sticks_evt)); // create a stick based event "first" to choose view (EXIT pressed)
        killEvents(KEY_EXIT); // Kill exit event
      }
      return;
    }
  }
#endif

  uint8_t enuk = KEY_MENU;

  keys[BTN_REa].input(REA_DOWN());
  keys[BTN_REb].input(REB_DOWN());

  uint8_t tin = ~PINL;
  uint8_t in;
  in = (tin & 0x0f) << 3;
  in |= (tin & 0x30) >> 3;

  for (int i=1; i<7; i++) {
    keys[enuk].input(in & (1<<i));
    ++enuk;
  }

  // Trims
  in = ~PINF;
  for (int i=0; i<8; i++) {
    // INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
    keys[enuk].input(in & pgm_read_byte_near(crossTrim+i));
    ++enuk;
  }
}

// Rotary encoders increment/decrement

void debounceRotEncA()
{
  DISABLEROTENCAISR();
  rotEncADebounce = ROTENCDEBOUNCEVAL;
}

void debounceRotEncB()
{
  DISABLEROTENCBISR();
  rotEncBDebounce = ROTENCDEBOUNCEVAL;
}

ISR(INT4_vect)     // Mega2560 INT4 (portE pin4)
{
  if (!(PINE & ROT_ENC_1_MASK)) {debounceRotEncA(); incRotaryEncoder(0, -1);}
}

ISR(INT5_vect)     // Mega2560 INT5 (portE pin5)
{
  if (!(PINE & ROT_ENC_1_MASK)) {debounceRotEncA(); incRotaryEncoder(0, +1);}
}

ISR(INT7_vect)     // Mega2560 INT7 (portE pin7)
{
  if (!(PINE & ROT_ENC_2_MASK)) {debounceRotEncB(); incRotaryEncoder(1, -1);}
}

ISR(INT6_vect)     // Mega2560 INT6 (portE pin6)
{
  if (!(PINE & ROT_ENC_2_MASK)) {debounceRotEncB(); incRotaryEncoder(1, +1);}
}
