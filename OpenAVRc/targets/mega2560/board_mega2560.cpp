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
#if defined(VOICE_WTV20SD)
  DDRA = 0b11111111;  PORTA = 0b00000000; // LCD data
  DDRB = 0b01100111;  PORTB = 0b10111111; // 7:WTV20SDBusy, 6:PPM_OUT, 5:SimCTRL, 4:Unused, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
  DDRC = 0b11111100;  PORTC = 0b00000011; // 7-3:LCD, 2:BackLight, 1:ID2_SW, 0:ID1_SW
  DDRD = 0b00000000;  PORTD = 0b11111111; // 7:AilDR_SW, 6:N/A, 5:N/A, 4:N/A, 3:RENC2_B, 2:RENC2_A, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00001010;  PORTE = 0b01110100; // 7:PPM_IN, 6:N/A, 5:RENC1_B, 4:RENC1_A, 3:WTV20SDData, 2:N/A, 1:TELEM_TX, 0:TELEM_RX
  DDRF = 0b00000000;  PORTF = 0b11111111; // 7-0:Trim switch inputs
  DDRG = 0b00100000;  PORTG = 0b11011111; // 7:N/A, 6:N/A, 5:WTV20SDClock, 4:N/A, 3:N/A, 2:TCut_SW, 1:Gear_SW, 0: RudDr_SW
  DDRH = 0b00011011;  PORTH = 0b11110100; // 7:N/A, 6:RF_Activated, 5:DSC_Activated, 4:Hold_Power, 3:Speaker, 2:N/A, 1:WTV20SDReset, 0:Haptic
  DDRJ = 0b00000000;  PORTJ = 0b11111111; // 7:N/A, 6:N/A, 5:N/A, 4:N/A, 3:N/A, 2:N/A, 1:RENC2_push, 0:RENC1_push
  DDRK = 0b00000000;  PORTK = 0b00000000; // Analogic input (no pull-ups)
  DDRL = 0b00000000;  PORTL = 0b11111111; // 7:TRN_SW 6:EleDR_SW, 5:ESC, 4:MENU 3:Keyb_Left, 2:Keyb_Right, 1:Keyb_Up, 0:Keyb_Down
#endif
#if defined(VOICE_JQ6500)
  DDRA = 0b11111111;  PORTA = 0b00000000; // LCD data
  DDRB = 0b01100111;  PORTB = 0b10111111; // 7:N/A, 6:PPM_OUT, 5:SimCTRL, 4:JQ6500Busy, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
  DDRC = 0b11111100;  PORTC = 0b00000011; // 7-3:LCD, 2:BackLight, 1:ID2_SW, 0:ID1_SW
  DDRD = 0b00000000;  PORTD = 0b11111111; // 7:AilDR_SW, 6:N/A, 5:N/A, 4:N/A, 3:RENC2_B, 2:RENC2_A, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00000010;  PORTE = 0b01111100; // 7:PPM_IN, 6:N/A, 5:RENC1_B, 4:RENC1_A, 3:N/A, 2:N/A, 1:TELEM_TX, 0:TELEM_RX
  DDRF = 0b00000000;  PORTF = 0b11111111; // 7-0:Trim switch inputs
  DDRG = 0b00100000;  PORTG = 0b11111111; // 7:N/A, 6:N/A, 5:CS_CYRF6936, 4:N/A, 3:N/A, 2:TCut_SW, 1:Gear_SW, 0: RudDr_SW
  DDRH = 0b00011011;  PORTH = 0b11110110; // 7:N/A, 6:RF_Activated, 5:DSC_Activated, 4:Hold_Power, 3:Speaker, 2:N/A, 1:CS_CC2500, 0:Haptic
  DDRJ = 0b00000010;  PORTJ = 0b11111111; // 7:N/A, 6:N/A, 5:N/A, 4:N/A, 3:N/A, 2:N/A, 1:JQ6500Data (Was RENC2_push), 0:RENC1_push
  DDRK = 0b00000000;  PORTK = 0b00000000; // Analogic input (no pull-ups)
  DDRL = 0b00000000;  PORTL = 0b11111111; // 7:TRN_SW 6:EleDR_SW, 5:ESC, 4:MENU 3:Keyb_Left, 2:Keyb_Right, 1:Keyb_Up, 0:Keyb_Down
#endif
#if !defined(VOICE)
  DDRA = 0b11111111;  PORTA = 0b00000000; // LCD data
  DDRB = 0b01100111;  PORTB = 0b10111111; // 7:N/A, 6:PPM_OUT, 5:SimCTRL, 4:Unused, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
  DDRC = 0b11111100;  PORTC = 0b00000011; // 7-3:LCD, 2:BackLight, 1:ID2_SW, 0:ID1_SW
  DDRD = 0b00000000;  PORTD = 0b11111111; // 7:AilDR_SW, 6:N/A, 5:N/A, 4:N/A, 3:RENC2_B, 2:RENC2_A, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00000010;  PORTE = 0b01111100; // 7:PPM_IN, 6:N/A, 5:RENC1_B, 4:RENC1_A, 3:N/A, 2:N/A, 1:TELEM_TX, 0:TELEM_RX
  DDRF = 0b00000000;  PORTF = 0b11111111; // 7-0:Trim switch inputs
  DDRG = 0b00000000;  PORTG = 0b11111111; // 7:N/A, 6:N/A, 5:N/A, 4:N/A, 3:N/A, 2:TCut_SW, 1:Gear_SW, 0: RudDr_SW
  DDRH = 0b00011001;  PORTH = 0b11110100; // 7:N/A, 6:RF_Activated, 5:DSC_Activated, 4:Hold_Power, 3:Speaker, 2:N/A, 1:N/A, 0:Haptic
  DDRJ = 0b00000000;  PORTJ = 0b11111111; // 7:N/A, 6:N/A, 5:N/A, 4:N/A, 3:N/A, 2:N/A, 1:RENC2_push, 0:RENC1_push
  DDRK = 0b00000000;  PORTK = 0b00000000; // Analogic input (no pull-ups)
  DDRL = 0b00000000;  PORTL = 0b11111111; // 7:TRN_SW 6:EleDR_SW, 5:ESC, 4:MENU 3:Keyb_Left, 2:Keyb_Right, 1:Keyb_Up, 0:Keyb_Down
#endif

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
  TCCR4B = (0b10<<WGM42) | (0b011 << CS40);
  TCCR4A = (0b01<<WGM40);
#endif

  // Setup Timer 1.
  // PB6 OC1B for Protocol callback and PPM
  // PB5 OC1A for PPMSIM
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


#if defined(VOICE_WTV20SD)
  // WTV20SD set-up, with TIMER5
  OCR5A = 0x1F4; //2ms
  TCCR5B = (1 << WGM52) | (0b011 << CS50); // CTC OCR5A
  TIMSK5 |= (1<<OCIE5A); // Start the interrupt so the unit reset can occur
#endif
#if defined(VOICE_JQ6500)
  // JQ6500 set-up, with TIMER5
  /*JQ6500_Serial_on;      // Idle state (1)
  OCR5A = 0x19; // 0x1A=104µs needed for the 9600Bps serial command
  TCCR5B = (1 << WGM52) | (0b011 << CS50); // CTC OCR5A*/

  #define TLM_JQ6500 3 // use uart3 on mega board (TX = PJ1)
  InitJQ6500UartTx();

#endif

  /* Rotary encoder interrupt set-up                 */
  EIMSK = 0; // disable ALL external interrupts.
  // encoder 1
  EICRB = (1<<ISC50) | (1<<ISC40); // 01 = interrupt on any edge
  EIFR = (3<<INTF4); // clear the int. flag in case it got set when changing modes
  // encoder 2
  EICRA = (1<<ISC30) | (1<<ISC20);
  EIFR = (3<<INTF2);
  EIMSK = (3<<INT4) | (3<<INT2); // enable the two rot. enc. ext. int. pairs.

#if defined(RTCLOCK) || defined(LCD_SSD1306) || defined(EXTERNALEEPROM)
  /* Hardware I2C init                               */
  i2c_init();
#endif

#endif // !SIMU

#if defined(SPIMODULES)
// Setup (M)SPI Port.
// USART2
// PORTH0 RXD2
// PORTH1 TXD2
// PORTH2 XCK2

// Setup pin states and USART in MSPI mode.
// Initialisation of USART.

  /*
  UBRR2 = 0; // Reset is part of initialisation sequence.
  UCSR2C = 0xC3; // UMSEL21:0 = 3 DORD2=0 CPHA2=1 CPOL2=1  USART in Master SPI mode, MSB first, Clock phase=1 Polarity=1.
  UCSR2B = (1 << RXEN3) | (1 << TXEN3); // Transmit and Receive.
  UBRR2 = 3; // 2.0MHz clock ... 16MHz/(2*(UBRR+1))

  DDRJ |= (1<<PORTH1) | (1<<PORTH2);
  DDRJ &= ~(1<<PORTH0);*/

  SDCARD_CS_N_INACTIVE();
  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();

  SpiRFModule.fixed_id = 0x1a2b3c4d;
  SpiRFModule.mode = NORMAL_MODE;

#endif // SPIMODULES

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
#endif
}


uint8_t pwrCheck()
{
  if ((~PINH & 0b00100000) && (~PINH & 0b01000000))   return e_power_off;
  return e_power_on;
}

void pwrOff()
{
  PORTH &= ~0x10;   // PortH-4 set to 0
}

uint8_t keyDown()
{
  return ((~PINL) & 0x3F) || ROTENC_DOWN();
}

bool switchState(EnumKeys enuk)
{
  uint8_t result = 0 ;

  if (enuk < (int)DIM(keys))
    return keys[enuk].state() ? 1 : 0;

  switch(enuk) {
  case SW_ELE:
    result = !(PINL & (1<<INP_L_ElevDR));
    break;

  case SW_AIL:
    result = !(PIND & (1<<INP_D_AileDR));
    break;

  case SW_RUD:
    result = !(PING & (1<<INP_G_RuddDR));
    break;

  //       INP_C_ID1  INP_C_ID2
  // ID0      0          1
  // ID1      1          1
  // ID2      1          0
  case SW_ID0:
    result = !(PINC & (1<<INP_C_ID1));
    break;

  case SW_ID1:
    result = (PINC & (1<<INP_C_ID1))&& (PINC & (1<<INP_C_ID2));
    break;

  case SW_ID2:
    result = !(PINC & (1<<INP_C_ID2));
    break;

  case SW_GEA:
    result = !(PING & (1<<INP_G_Gear));
    break;

  case SW_THR:
    result = !(PING & (1<<INP_G_ThrCt));
    break;

  case SW_TRN:
    result = !(PINL & (1<<INP_L_Trainer));
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
  uint8_t in = ~PINF;                //was PIND
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

  keys[BTN_REa].input(~PINJ & 0x01);
  keys[BTN_REb].input(~PINJ & 0x02);

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

// Rotary encoders increment/decrement (0 = rotary 1, 1 = rotary 2)
ISR(INT4_vect)     // Arduino2560 IO02 (portE pin4)
{
  uint8_t input = (PINE & 0x30);
#if defined (ROTENC_DIV2)// Rotenc resolution/2
  if (input == 0) incRotaryEncoder(0, -1);
#else
  if (input == 0 || input == 0x30) incRotaryEncoder(0, -1);
#endif
}

ISR(INT5_vect)     // Arduino2560 IO03 (portE pin5)
{
  uint8_t input = (PINE & 0x30);
#if defined (ROTENC_DIV2)// Rotenc resolution/2
  if (input == 0) incRotaryEncoder(0, +1);
#else
  if (input == 0 || input == 0x30) incRotaryEncoder(0, +1);
#endif
}

ISR(INT2_vect)     // Arduino2560 IO19 (portD pin2)
{
  uint8_t input = (PIND & 0x0C);
#if defined (ROTENC_DIV2)// Rotenc resolution/2
  if (input == 0) incRotaryEncoder(1, -1);
#else
  if (input == 0 || input == 0x0C) incRotaryEncoder(1, -1);
#endif
}

ISR(INT3_vect)     // Arduino2560 IO18 (portD pin3)
{
  uint8_t input = (PIND & 0x0C);
#if defined (ROTENC_DIV2)// Rotenc resolution/2
  if (input == 0) incRotaryEncoder(1, +1);
#else
  if (input == 0 || input == 0x0C) incRotaryEncoder(1, +1);
#endif
}

