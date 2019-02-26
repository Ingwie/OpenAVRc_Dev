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
  DDRD = 0b00001100;  PORTD = 0b11111111; // 7:AilDR_SW, 6:N/A, 5:N/A, 4:PPM_IN, 3:CS_NRF24L01, 2:CS_CYRF6936, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00000010;  PORTE = 0b11111110; // 7:Rot_2A, 6:Rot_2B, 5:Rot_1B, 4:Rot_1A, 3:Rot_2_Push, 2:N/A, 1:TELEM_TX, 0:TELEM_RX
  DDRF = 0b00000000;  PORTF = 0b11111111; // 7-0:Trim switch inputs
  DDRG = 0b00000000;  PORTG = 0b11111111; // 7:N/A, 6:N/A, 5:Rot_1_Push, 4:N/A, 3:N/A, 2:TCut_SW, 1:Gear_SW, 0: RudDr_SW
  DDRH = 0b01111011;  PORTH = 0b10010011; // 7:N/A, 6:Sim_Control/RF_Power, 5:Haptic, 4:Hold_Power, 3:Speaker, 2:N/A, 1:CS_CC2500, 0:CS_A7105
  DDRJ = 0b00000010;  PORTJ = 0b11111111; // 7:N/A, 6:N/A, 5:N/A, 4:N/A, 3:N/A, 2:N/A, 1:JQ6500Data , 0:JQ6500Busy
  DDRK = 0b00000000;  PORTK = 0b00000000; // Analogic input (no pull-ups)
  DDRL = 0b00000000;  PORTL = 0b11111111; // 7:TRN_SW 6:EleDR_SW, 5:ESC, 4:MENU 3:Keyb_Left, 2:Keyb_Right, 1:Keyb_Up, 0:Keyb_Down

  adcInit();

#ifndef SIMU

  /**** Set up timer/counter 3 ****/
  // TCNT  10ms = 16MHz/1024/156(.25) periodic timer (10ms interval)
  //        cycles at 9.984ms but includes 1:4 duty cycle correction to /157 to average at 10.0ms
  TCCR3A  = 0;
  TCCR3C  = 0;
  TCCR3B  = (0b101 << CS30); // Norm mode, clk/1024 (16 bits counter)
  OCR3A   = 156;
  TIMSK3  = (1<<OCIE3A); // Enable Output-Compare interrupts


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

#if defined(PWM_BACKLIGHT)
// Test PWM backlight on MEGACORELEDPIN.
  /*
   * Setup Timer Counter 0.
   * Fast PWM mode. Mode 3.
   */

//  TCCR0A = (1 << COM0A0) | (0b11 << WGM00); // Try this for a normal ...
  TCCR0A = (1 << COM0A1) | (0b11 << WGM00); // ... or this for an inverted PWM signal.
  OCR0A = 127;
  TIMSK0 = 0;
  TCCR0B = 0b011 << CS00; // 16MHz / 64.
#endif

#if defined(VOICE_JQ6500)

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

#if defined(RTCLOCK) || defined(LCD_SSD1306) || defined(LCD_SH1106) || defined(EXTERNALEEPROM) || defined(X_ANY)
  /* Hardware I2C init */
  i2c_init();
#endif

#if defined(X_ANY)
  Xany_init();
#endif

#if defined(SPIMODULES)
  RF_SPI_INIT();
  protoMode = NORMAL_MODE;
#endif // SPIMODULES

#endif // !SIMU

  WAIT_PUPIL();

 LEDOFF();
}

void rf_spi_init()
{
  // Reset CS pin
  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();
  RF_CS_NRF24L01_INACTIVE();
  RF_CS_A7105_INACTIVE();

  // Init SPI port with 4Mhz speed
  enable_spi_master_mode();
  //g_eeGeneral.fixed_ID.ID_32 = 0x1a2b3c4d;
}

void backlightFade(void)
{
// ToDo ... We can decrement the compare register until the target is reached.
// At the moment we just have a backlight brightness of two levels.
// OCR0A = OCR0A -1;
}

uint8_t master_rf_spi_xfer(uint8_t data)
{
#if !defined(SIMU)
  SPDR = data;
  while (!(SPSR & (1<<SPIF)));
  return SPDR;
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

uint8_t switchState(EnumKeys enuk)
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

  //keys[BTN_REa].input(~PINJ & 0x01);
  //keys[BTN_REb].input(~PINJ & 0x02);

  uint8_t tin = ~PINL;
  uint8_t in;
  in = (tin & 0x0f) << 3;
  in |= (tin & 0x30) >> 3;

  for (uint8_t i=1; i<7; i++) {
    keys[enuk].input(in & (1<<i));
    ++enuk;
  }

  // Trims
  in = ~PINF;
  for (uint8_t i=0; i<8; i++) {
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


