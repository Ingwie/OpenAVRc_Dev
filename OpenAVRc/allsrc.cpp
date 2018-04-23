# 1 "main_avr.cpp"
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


#include "OpenAVRc.h"

extern void checkBattery();
extern uint8_t checkTrim(uint8_t event);

void perMain()
{

  SIMU_PROCESSEVENTS;

  uint16_t t0 = getTmr16KHz();
  int16_t delta = (nextMixerEndTime - lastMixerDuration) - t0;
  if (delta > 0 && delta < (int16_t)US_TO_16KHZ_TICK(MAX_MIXER_DELTA_US)) {
LEDON();

#if defined(SDCARD) && !defined(SIMU)
    if (sdMounted() && isFunctionActive(FUNCTION_LOGS) && delta > (int16_t)US_TO_16KHZ_TICK(4000))
      writeLogs(); // Minimise writelogs perturbation
#endif
LEDOFF();
    return;
  }

  nextMixerEndTime = t0 + US_TO_16KHZ_TICK(MAX_MIXER_DELTA_US);
  // this is a very tricky implementation; lastMixerEndTime is just like a default value not to stop mixcalculations totally;
  // the real value for lastMixerEndTime is calculated inside protocol files

  doMixerCalculations();

  SIMU_PROCESSEVENTS;

  t0 = getTmr16KHz() - t0;
  lastMixerDuration = t0;
  if (t0 > maxMixerDuration)
    maxMixerDuration = t0;


  if (!eeprom_buffer_size) {
    if (theFile.isWriting())
      theFile.nextWriteStep();
    else if (TIME_TO_WRITE())
      eeCheck(false);
  }

#if defined(X_ANY)
  Xany_readInputsAndLoadMsg(0);
  Xany_readInputsAndLoadMsg(1);
#endif

#if defined(SDCARD) && !defined(SIMU)
  //sdMountPoll();
  //writeLogs();
#endif

  uint8_t evt = getEvent();
  evt = checkTrim(evt);

#if defined(FRSKY) || defined(MAVLINK)
  telemetryWakeup();
#endif

#if defined(NAVIGATION_STICKS)
  uint8_t sticks_evt = getSticksNavigationEvent();
  if (sticks_evt)
    evt = EVT_KEY_FIRST(sticks_evt);
#endif

  if (evt && (g_eeGeneral.backlightMode & e_backlight_mode_keys))
    backlightOn(); // on keypress turn the light on
  checkBacklight();

#if defined(GUI)
  const char *warn = warningText;
  bool popupMenuActive = (popupMenuNoItems > 0);

  lcdClear();

  if (menuEvent) {
    menuVerticalPosition = menuEvent == EVT_ENTRY_UP ? menuVerticalPositions[menuLevel] : 0;
    menuHorizontalPosition = 0;
    evt = menuEvent;
    menuEvent = 0;
    AUDIO_MENUS();
  }

  menuHandlers[menuLevel]((warn || popupMenuActive) ? 0 : evt);

  if (warn)
    DISPLAY_WARNING(evt);

#if defined(NAVIGATION_MENUS)
  if (popupMenuActive) {
    const char * result = displayPopupMenu(evt);
    if (result) {
      popupMenuHandler(result);
      putEvent(EVT_MENU_UP);
    }
  }
#endif

  drawStatusLine();
  lcdRefreshFast();

#endif // if defined(GUI)

  checkBattery();
}

# 1 "i2c_master.cpp"
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


// From https://github.com/g4lvanix/I2C-master-lib

#if !defined(SIMU)
#include <avr/io.h>
#include <util/twi.h>
#endif

#include "i2c_master.h"

// DS3231M = 400KHz
// DS3231  = 400KHz
// FM24W256 = 1MHz
// 400KHz SCL ... TWPS =0b01 in register TWSR(1:0), TWBR =3.
// 1MHz SCL ... TWPS =0b00 in register TWSR(1:0), TWBR =0.
/*
 *        FCPU
 * --------------------  = SCL frequency
 * 16 + 2(TWBR) .4^TWPS
 */

#if defined(CPUM2560)
void i2c_init()
{
  I2C_SPEED_888K();
}

void i2c_Wait()
{
  // Poll TWI Interrupt flag.
  while(! (TWCR & (1<<TWINT)) );
}

uint8_t i2c_start(uint8_t address)
{
  // transmit START condition
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  // i2c_Wait for end of transmission
  i2c_Wait();

  // check if the start condition was successfully transmitted
  uint8_t twst = TW_STATUS & 0xF8;
  if(! ((twst == TW_START) || (twst == TW_REP_START)) ) {
    return 1;
  }

  // load slave address into data register
  TWDR = address;
  // start transmission of address
  TWCR = (1<<TWINT) | (1<<TWEN);
  // i2c_Wait for end of transmission
  i2c_Wait();
  // check if the device has acknowledged the READ / WRITE mode
  twst = TW_STATUS & 0xF8;
  if ( (twst == TW_MT_SLA_ACK) || (twst == TW_MR_SLA_ACK) ) return 0;
  else return 1;
}

uint8_t i2c_write(uint8_t data)
{
  // load data into data register
  TWDR = data;
  // start transmission of data
  TWCR = (1<<TWINT) | (1<<TWEN);
  // i2c_Wait for end of transmission
  i2c_Wait();

  if( (TWSR & 0xF8) != TW_MT_DATA_ACK ) {
    return 1;
  }
  return 0;
}

uint8_t i2c_read_ack()
{
  // start TWI module and acknowledge data after reception
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
  // i2c_Wait for end of transmission
  i2c_Wait();
  // return received data from TWDR
  return TWDR;
}

uint8_t i2c_read_nack()
{
  // start receiving without acknowledging reception
  TWCR = (1<<TWINT) | (1<<TWEN);
  // i2c_Wait for end of transmission
  i2c_Wait();
  // return received data from TWDR
  return TWDR;
}

void i2c_stop()
{
  // transmit STOP condition
  TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
  while SIMU_UNLOCK_MACRO(TWCR & (1<<TWSTO));
}

inline void i2c_writeISR(uint8_t data)
{
  // load data into data register
  TWDR = data;
  // start transmission of data with isr activated
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
  // Send data and enable IRQ
}

extern uint8_t * eeprom_buffer_data;
extern volatile uint8_t eeprom_buffer_size;
ISR(TWI_vect)
{
  if (--eeprom_buffer_size) {
    i2c_writeISR(*eeprom_buffer_data);
    ++eeprom_buffer_data;
  } else {
    i2c_stop(); // This resets the TWINT Flag.
    TWCR &= ~(1<<TWIE); // Disable TWI interrupts.
  }
}
#endif


uint8_t i2c_transmit(uint8_t address, uint8_t* data, uint16_t length)
{
  if (i2c_start(address | I2C_WRITE)) return 1;

  for (uint16_t i = 0; i < length; i++) {
    if (i2c_write(data[i])) return 1;
  }

  i2c_stop();

  return 0;
}

uint8_t i2c_receive(uint8_t address, uint8_t* data, uint16_t length)
{
  if (i2c_start(address | I2C_READ)) return 1;

  for (uint16_t i = 0; i < (length-1); i++) {
    data[i] = i2c_read_ack();
  }
  data[(length-1)] = i2c_read_nack();

  i2c_stop();

  return 0;
}

uint8_t i2c_writeReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
  if (i2c_start(devaddr | I2C_WRITE)) return 1;

  i2c_write(regaddr);

  for (uint16_t i = 0; i < length; i++) {
    if (i2c_write(data[i])) return 1;
  }

  i2c_stop();

  return 0;
}

uint8_t i2c_readReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
  if (i2c_start(devaddr | I2C_WRITE)) return 1;

  i2c_write(regaddr);

  if (i2c_start(devaddr | I2C_READ)) return 1;

  for (uint16_t i = 0; i < (length-1); i++) {
    data[i] = i2c_read_ack();
  }
  data[(length-1)] = i2c_read_nack();

  i2c_stop();

  return 0;
}
# 1 "spi.c"
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


#include "OpenAVRc.h"
#include "spi.h"
//---------------------------
// AVR SPI functions
//---------------------------

void master_spi_disable()
{
  SPCR &= ~(1<<SPE);
}

//----------------------

void enable_spi_master_mode()
{
// Enable SPI as Master, MSB first.
  SPI_250K(); // Fosc/64
// Note : Make sure Slave Select pin is output or input pullup.
}

//----------------------

uint8_t master_spi_xfer(uint8_t value)
{
  // Full Duplex (4 wire) spi
  SPDR = value;
  /* Wait for transfer to complete */
#if !defined(SIMU)
  while (!(SPSR & (1<<SPIF)));
#endif
  return SPDR;
}

//----------------------
# 1 "targets/megamini/board_megamini.cpp"
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

#if defined(RTCLOCK) || defined(LCD_SSD1306) || defined(LCD_SH1106) || defined(EXTERNALEEPROM)| defined(X_ANY)
  /* Hardware I2C init */
  i2c_init();
#endif

#if defined(X_ANY)
  Xany_init();
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
# 1 "sdcard.cpp"
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


#include <stdint.h>
#include "thirdparty/FatFs/diskio.h"
#include "thirdparty/FatFs/ff.h"
#include "OpenAVRc.h"

bool listSdFiles(const char *path, const char *extension, const uint8_t maxlen, const char *selection, uint8_t flags=0)
{
  FILINFO fno;
  DIR dir;

#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

  static uint16_t lastpopupMenuOffset = 0;


  if (popupMenuOffset == 0) {
    lastpopupMenuOffset = 0;
    memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
  } else if (popupMenuOffset == popupMenuNoItems - MENU_MAX_DISPLAY_LINES) {
    lastpopupMenuOffset = 0xffff;
    memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
  } else if (popupMenuOffset == lastpopupMenuOffset) {
    // should not happen, only there because of Murphy's law
    return true;
  } else if (popupMenuOffset > lastpopupMenuOffset) {
    memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], (MENU_MAX_DISPLAY_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], 0xff, MENU_LINE_LENGTH);
  } else {
    memmove(reusableBuffer.modelsel.menu_bss[1], reusableBuffer.modelsel.menu_bss[0], (MENU_MAX_DISPLAY_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.modelsel.menu_bss[0], 0, MENU_LINE_LENGTH);
  }

  popupMenuNoItems = 0;
  POPUP_MENU_ITEMS_FROM_BSS();

  FRESULT res = f_opendir(&dir, path);        /* Open the directory */
  if (res == FR_OK) {

    if (flags) {
      ++popupMenuNoItems;
      if (selection) {
        lastpopupMenuOffset++;
      } else if (popupMenuOffset==0 || popupMenuOffset < lastpopupMenuOffset) {
        char *line = reusableBuffer.modelsel.menu_bss[0];
        memset(line, 0, MENU_LINE_LENGTH);
        strcpy(line, "---");
        popupMenuItems[0] = line;
      }
    }

    for (;;) {
      char *fn;   /* This function is assuming non-Unicode cfg. */
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */

#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif

      uint8_t len = strlen(fn);
      if (len < 5 || len > maxlen+4 || strcasecmp(fn+len-4, extension) || (fno.fattrib & AM_DIR)) continue;

      ++popupMenuNoItems;
      fn[len-4] = '\0';

      if (popupMenuOffset == 0) {
        if (selection && strncasecmp(fn, selection, maxlen) < 0) {
          lastpopupMenuOffset++;
        } else {
          for (uint8_t i=0; i<MENU_MAX_DISPLAY_LINES; i++) {
            char *line = reusableBuffer.modelsel.menu_bss[i];
            if (line[0] == '\0' || strcasecmp(fn, line) < 0) {
              if (i < MENU_MAX_DISPLAY_LINES-1) memmove(reusableBuffer.modelsel.menu_bss[i+1], line, sizeof(reusableBuffer.modelsel.menu_bss[i]) * (MENU_MAX_DISPLAY_LINES-1-i));
              memset(line, 0, MENU_LINE_LENGTH);
              strcpy(line, fn);
              break;
            }
          }
        }
        for (uint8_t i=0; i<min(popupMenuNoItems, (uint16_t)MENU_MAX_DISPLAY_LINES); i++) {
          popupMenuItems[i] = reusableBuffer.modelsel.menu_bss[i];
        }

      } else if (lastpopupMenuOffset == 0xffff) {
        for (int i=MENU_MAX_DISPLAY_LINES-1; i>=0; i--) {
          char *line = reusableBuffer.modelsel.menu_bss[i];
          if (line[0] == '\0' || strcasecmp(fn, line) > 0) {
            if (i > 0) memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], sizeof(reusableBuffer.modelsel.menu_bss[i]) * i);
            memset(line, 0, MENU_LINE_LENGTH);
            strcpy(line, fn);
            break;
          }
        }
        for (uint8_t i=0; i<min(popupMenuNoItems, (uint16_t)MENU_MAX_DISPLAY_LINES); i++) {
          popupMenuItems[i] = reusableBuffer.modelsel.menu_bss[i];
        }
      } else if (popupMenuOffset > lastpopupMenuOffset) {
        if (strcasecmp(fn, reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-2]) > 0 && strcasecmp(fn, reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1]) < 0) {
          memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], fn);
        }
      } else {
        if (strcasecmp(fn, reusableBuffer.modelsel.menu_bss[1]) < 0 && strcasecmp(fn, reusableBuffer.modelsel.menu_bss[0]) > 0) {
          memset(reusableBuffer.modelsel.menu_bss[0], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.modelsel.menu_bss[0], fn);
        }
      }
    }
  }

  if (popupMenuOffset > 0)
    lastpopupMenuOffset = popupMenuOffset;
  else
    popupMenuOffset = lastpopupMenuOffset;

  return popupMenuNoItems;
}



#if !defined(SIMU)
uint32_t sdGetNoSectors()
{
  DWORD noSectors = 0;
  if (noSectors == 0 ) {
    disk_ioctl(0, GET_SECTOR_COUNT, &noSectors);
  }
  return noSectors/1000;
}

uint32_t sdGetSize()
{
  return (sdGetNoSectors() * 512) / 1000;
}

uint32_t sdGetFreeSectors()
{
  DWORD nofree;
  FATFS * fat;
  if (f_getfree("", &nofree, &fat) != FR_OK) {
    return 0;
  }
  return nofree * fat->csize;
}

#else  // #if !defined(SIMU)
uint32_t sdGetNoSectors()
{
  return 512;
}

uint32_t sdGetSize()
{
  return 2000;
}

uint32_t sdGetFreeSectors()
{
  return 10;
}

#endif
# 1 "logs.cpp"
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


#include "OpenAVRc.h"
#include "thirdparty/FatFs/ff.h"

FIL g_oLogFile = {0};
uint8_t logDelay;

#define get2PosState(sw) (switchState(SW_ ## sw) ? -1 : 1)

#define get3PosState(sw) (switchState(SW_ ## sw ## 0) ? -1 : (switchState(SW_ ## sw ## 2) ? 1 : 0))

const pm_char *openLogs()
{
  // Determine and set log file filename
  FRESULT result;
  DIR folder;
  char filename[34]; // /LOGS/modelnamexxx-2013-01-01.log

  if (!sdMounted())
    return STR_NO_SDCARD;

  //if (sdGetFreeSectors() == 0) Todo : better place
    //return STR_SDCARD_FULL;

  // check and create folder here
  strcpy_P(filename, STR_LOGS_PATH);
  result = f_opendir(&folder, filename);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(filename);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }

  filename[sizeof(LOGS_PATH)-1] = '/';
  memcpy(&filename[sizeof(LOGS_PATH)], g_model.name, sizeof(g_model.name));
  filename[sizeof(LOGS_PATH)+sizeof(g_model.name)] = '\0';

  uint8_t i = sizeof(LOGS_PATH)+sizeof(g_model.name)-1;
  uint8_t len = 0;
  while (i>sizeof(LOGS_PATH)-1) {
    if (!len && filename[i])
      len = i+1;
    if (len) {
      if (filename[i])
        filename[i] = idx2char(filename[i]);
      else
        filename[i] = '_';
    }
    i--;
  }

  if (len == 0) {
    uint8_t num = g_eeGeneral.currModel + 1;
    strcpy_P(&filename[sizeof(LOGS_PATH)], STR_MODEL);
    filename[sizeof(LOGS_PATH) + PSIZE(TR_MODEL)] = (char)((num / 10) + '0');
    filename[sizeof(LOGS_PATH) + PSIZE(TR_MODEL) + 1] = (char)((num % 10) + '0');
    len = sizeof(LOGS_PATH) + PSIZE(TR_MODEL) + 2;
  }


#if defined(RTCLOCK)
  char * tmp = strAppendDate(&filename[len]);
#else
  char * tmp = &filename[len];
#endif

  strcpy_P(tmp, STR_LOGS_EXT);

  result = f_open(&g_oLogFile, filename, FA_OPEN_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (f_size(&g_oLogFile) == 0) {
    writeHeader();
  } else {
    result = f_lseek(&g_oLogFile, f_size(&g_oLogFile)); // append
    if (result != FR_OK) {
      return SDCARD_ERROR(result);
    }
  }

  return NULL;
}

tmr10ms_t lastLogTime = 0;

void closeLogs()
{
  if (f_close(&g_oLogFile) != FR_OK) {
    // close failed, forget file
    g_oLogFile.fs = 0;
  }
  lastLogTime = 0;
}

getvalue_t getConvertedTelemetryValue(getvalue_t val, uint8_t unit)
{
  convertUnit(val, unit);
  return val;
}

void writeHeader()
{
#if defined(RTCLOCK)
  f_puts("Date,Time,", &g_oLogFile);
#else
  f_puts("Time,", &g_oLogFile);
#endif

#if defined(FRSKY)
  f_puts("Buffer,RX,TX,A1,A2,", &g_oLogFile);
  if (IS_USR_PROTO_FRSKY_HUB()) {
    f_puts("GPS Date,GPS Time,Long,Lat,Course,GPS Speed(kts),GPS Alt,Baro Alt(", &g_oLogFile);
    f_puts(TELEMETRY_BARO_ALT_UNIT, &g_oLogFile);
    f_puts("),Vertical Speed,Air Speed(kts),Temp1,Temp2,RPM,Fuel," TELEMETRY_CELLS_LABEL "Current,Consumption,Vfas,AccelX,AccelY,AccelZ,", &g_oLogFile);
  }
#if defined(WS_HOW_HIGH)
  if (IS_USR_PROTO_WS_HOW_HIGH()) {
    f_puts("WSHH Alt,", &g_oLogFile);
  }
#endif
#endif

  f_puts("Rud,Ele,Thr,Ail,P1,P2,P3,THR,RUD,ELE,3POS,AIL,GEA,TRN\n", &g_oLogFile);
}

void writeLogs()
{
  static const pm_char * error_displayed = NULL;

  if (logDelay > 0) {
    tmr10ms_t tmr10ms = get_tmr10ms();
    if (lastLogTime == 0 || (tmr10ms_t)(tmr10ms - lastLogTime) >= (tmr10ms_t)logDelay*100) {
      lastLogTime = tmr10ms;

      if (!g_oLogFile.fs) {
        const pm_char * result = openLogs();
        if (result != NULL) {
          if (result != error_displayed) {
            error_displayed = result;
            POPUP_WARNING(result);
          }
          return;
        }
      }

#if defined(RTCLOCK)
      {
        static struct gtm utm;
        static gtime_t lastRtcTime = 0;
        if ( g_rtcTime != lastRtcTime ) {
          lastRtcTime = g_rtcTime;
          gettime(&utm);
        }
        f_printf(&g_oLogFile, "%4d-%02d-%02d,%02d:%02d:%02d.%02d0,", utm.tm_year+1900, utm.tm_mon+1, utm.tm_mday, utm.tm_hour, utm.tm_min, utm.tm_sec, g_ms100);
      }
#else
      f_printf(&g_oLogFile, "%d,", tmr10ms);
#endif

#if defined(FRSKY)
      f_printf(&g_oLogFile, "%d,%d,%d,", frskyStreaming, RAW_FRSKY_MINMAX(telemetryData.rssi[0]), RAW_FRSKY_MINMAX(telemetryData.rssi[1]));
      for (uint8_t i=0; i<MAX_FRSKY_A_CHANNELS; i++) {
        int16_t converted_value = applyChannelRatio(i, RAW_FRSKY_MINMAX(telemetryData.analog[i]));
        f_printf(&g_oLogFile, "%d.%02d,", converted_value/100, converted_value%100);
      }

      TELEMETRY_BARO_ALT_PREPARE();

      if (IS_USR_PROTO_FRSKY_HUB()) {
        f_printf(&g_oLogFile, "%4d-%02d-%02d,%02d:%02d:%02d,%03d.%04d%c,%03d.%04d%c,%03d.%02d," TELEMETRY_GPS_SPEED_FORMAT TELEMETRY_GPS_ALT_FORMAT TELEMETRY_BARO_ALT_FORMAT TELEMETRY_VSPEED_FORMAT TELEMETRY_ASPEED_FORMAT "%d,%d,%d,%d," TELEMETRY_CELLS_FORMAT TELEMETRY_CURRENT_FORMAT "%d," TELEMETRY_VFAS_FORMAT "%d,%d,%d,",
                 telemetryData.value.year+2000,
                 telemetryData.value.month,
                 telemetryData.value.day,
                 telemetryData.value.hour,
                 telemetryData.value.min,
                 telemetryData.value.sec,
                 telemetryData.value.gpsLongitude_bp,
                 telemetryData.value.gpsLongitude_ap,
                 telemetryData.value.gpsLongitudeEW ? telemetryData.value.gpsLongitudeEW : '-',
                 telemetryData.value.gpsLatitude_bp,
                 telemetryData.value.gpsLatitude_ap,
                 telemetryData.value.gpsLatitudeNS ? telemetryData.value.gpsLatitudeNS : '-',
                 telemetryData.value.gpsCourse_bp,
                 telemetryData.value.gpsCourse_ap,
                 TELEMETRY_GPS_SPEED_ARGS
                 TELEMETRY_GPS_ALT_ARGS
                 TELEMETRY_BARO_ALT_ARGS
                 TELEMETRY_VSPEED_ARGS
                 TELEMETRY_ASPEED_ARGS
                 telemetryData.value.temperature1,
                 telemetryData.value.temperature2,
                 telemetryData.value.rpm,
                 telemetryData.value.fuelLevel,
                 TELEMETRY_CELLS_ARGS
                 TELEMETRY_CURRENT_ARGS
                 telemetryData.value.currentConsumption,
                 TELEMETRY_VFAS_ARGS
                 telemetryData.value.accelX,
                 telemetryData.value.accelY,
                 telemetryData.value.accelZ);
      }

#if defined(WS_HOW_HIGH)
      if (IS_USR_PROTO_WS_HOW_HIGH()) {
        f_printf(&g_oLogFile, "%d,", TELEMETRY_RELATIVE_BARO_ALT_BP);
      }
#endif
#endif

      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
        f_printf(&g_oLogFile, "%d,", calibratedStick[i]);
      }

      int result = f_printf(&g_oLogFile, "%d,%d,%d,%d,%d,%d,%d\n",
                            get2PosState(THR),
                            get2PosState(RUD),
                            get2PosState(ELE),
                            get3PosState(ID),
                            get2PosState(AIL),
                            get2PosState(GEA),
                            get2PosState(TRN));

      if (result<0 && !error_displayed) {
        error_displayed = STR_SDCARD_ERROR;
        POPUP_WARNING(STR_SDCARD_ERROR);
        closeLogs();
      }
    }
  } else {
    error_displayed = NULL;
    if (g_oLogFile.fs) {
      closeLogs();
    }
  }
}



# 1 "audio_avr.cpp"
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


#include "OpenAVRc.h"

audioQueue::audioQueue()
{
  toneTimeLeft = 0;
  tonePause = 0;
  tone2TimeLeft = 0;

  t_queueRidx = 0;
  t_queueWidx = 0;
}


// heartbeat is responsibile for issueing the audio tones and general square waves
// it is essentially the life of the class.
// it is called every 10ms
void audioQueue::heartbeat()
{
  if (toneTimeLeft > 0) {
    if (toneFreq == 0) {  //pause only events
      speakerOff();
    } else {
      if (toneFreq) {
        AUDIO_OCRxA = (5000 / toneFreq); // sticking with old values approx 20(abs. min) to 90, 60 being the default tone(?).
        speakerOn();
        SIMUBEEP1();
      }
      toneFreq += toneFreqIncr;
    }
    toneTimeLeft--; //time gets counted down
  } else {
    if (tonePause > 0) {
      speakerOff();
      tonePause--; //time gets counted down
    } else if (t_queueRidx != t_queueWidx) {
      toneFreq = queueToneFreq[t_queueRidx];
      toneTimeLeft = queueToneLength[t_queueRidx];
      toneFreqIncr = queueToneFreqIncr[t_queueRidx];
      tonePause = queueTonePause[t_queueRidx];
      if (!queueToneRepeat[t_queueRidx]--) {
        t_queueRidx = (t_queueRidx + 1) & (AUDIO_QUEUE_LENGTH-1);
      }
    } else {
      if (tone2TimeLeft > 0) {
        if (tone2Freq) {
          AUDIO_OCRxA = (5000 / tone2Freq); // sticking with old values approx 20(abs. min) to 90, 60 being the default tone(?).
          speakerOn();
          SIMUBEEP2();
        }
        tone2TimeLeft--; //time gets counted down
      } else {
        speakerOff();
      }
    }
  }
}

inline uint8_t audioQueue::getToneLength(uint8_t tLen)
{
  uint8_t result = tLen; // default
  if (g_eeGeneral.beepLength < 0) {
    result /= (1-g_eeGeneral.beepLength);
  }
  if (g_eeGeneral.beepLength > 0) {
    result *= (1+g_eeGeneral.beepLength);
  }
  return result;
}

void audioQueue::pause(uint8_t tLen)
{
  play(0, tLen, 5); // a pause
}

void audioQueue::play(uint8_t tFreq, uint8_t tLen, uint8_t tPause, uint8_t tFlags)
{
  if (tFlags & PLAY_BACKGROUND) {
    tone2Freq = tFreq;
    tone2TimeLeft = tLen;
  } else {
    int8_t tFreqIncr = (tFlags >> 6);
    if (tFreqIncr == 3) tFreqIncr = -1;

    tFreq += g_eeGeneral.speakerPitch + BEEP_OFFSET; // add pitch compensator
    tLen = getToneLength(tLen);

    if ((tFlags & PLAY_NOW) || (!busy() && empty())) {
      toneFreq = tFreq;
      toneTimeLeft = tLen;
      tonePause = tPause;
      toneFreqIncr = tFreqIncr;
      t_queueWidx = t_queueRidx;
    } else {
      tFlags++;
    }

    tFlags &= 0x0f;
    if (tFlags) {
      uint8_t next_queueWidx = (t_queueWidx + 1) & (AUDIO_QUEUE_LENGTH-1);
      if (next_queueWidx != t_queueRidx) {
        queueToneFreq[t_queueWidx] = tFreq;
        queueToneLength[t_queueWidx] = tLen;
        queueTonePause[t_queueWidx] = tPause;
        queueToneRepeat[t_queueWidx] = tFlags - 1;
        queueToneFreqIncr[t_queueWidx] = tFreqIncr;
        t_queueWidx = next_queueWidx;
      }
    }
  }
}

void audioQueue::event(uint8_t e, uint8_t f)
{
#if defined(HAPTIC)
  haptic.event(e); //do this before audio to help sync timings
#endif

  if (e <= AU_ERROR || (e >= AU_WARNING1 && e < AU_FRSKY_FIRST)) {
    if (g_eeGeneral.alarmsFlash) {
      flashCounter = FLASH_DURATION;
    }
  }

  if (g_eeGeneral.beepMode>0 || (g_eeGeneral.beepMode==0 && e>=AU_TRIM_MOVE) || (g_eeGeneral.beepMode>=-1 && e<=AU_ERROR)) {
    if (e < AU_FRSKY_FIRST || empty()) {
      // TODO when VOICE enable some cases here are not needed!
      switch (e) {
#if !defined(VOICE)
      case AU_TX_BATTERY_LOW:
        if (empty()) {
          play(60, 20, 3, PLAY_REPEAT(2)|PLAY_INCREMENT(1));
          play(80, 20, 3, PLAY_REPEAT(2)|PLAY_INCREMENT(-1));
        }
        break;
#endif
      case AU_TRIM_MOVE:
        play(f, 6, 1, PLAY_NOW);
        break;
      case AU_TRIM_MIDDLE:
        play(f, 10, 2, PLAY_NOW);
        break;
      case AU_FRSKY_RING:
        play(BEEP_DEFAULT_FREQ+25, 5, 2, PLAY_REPEAT(10));
        play(BEEP_DEFAULT_FREQ+25, 5, 10, PLAY_REPEAT(1));
        play(BEEP_DEFAULT_FREQ+25, 5, 2, PLAY_REPEAT(10));
        break;
      case AU_FRSKY_SCIFI:
        play(80, 10, 3, PLAY_REPEAT(2)|PLAY_INCREMENT(-1));
        play(60, 10, 3, PLAY_REPEAT(2)|PLAY_INCREMENT(1));
        play(70, 10, 1);
        break;
      case AU_FRSKY_ROBOT:
        play(70, 5, 1, PLAY_REPEAT(1));
        play(50, 15, 2, PLAY_REPEAT(1));
        play(80, 15, 2, PLAY_REPEAT(1));
        break;
      case AU_FRSKY_CHIRP:
        play(BEEP_DEFAULT_FREQ+40, 5, 1, PLAY_REPEAT(2));
        play(BEEP_DEFAULT_FREQ+54, 5, 1, PLAY_REPEAT(3));
        break;
      case AU_FRSKY_TADA:
        play(50, 5, 5);
        play(90, 5, 5);
        play(110, 3, 4, PLAY_REPEAT(2));
        break;
      case AU_FRSKY_CRICKET:
        play(80, 5, 10, PLAY_REPEAT(3));
        play(80, 5, 20, PLAY_REPEAT(1));
        play(80, 5, 10, PLAY_REPEAT(3));
        break;
      case AU_FRSKY_ALARMC:
        play(50, 4, 10, PLAY_REPEAT(2));
        play(70, 8, 20, PLAY_REPEAT(1));
        play(50, 8, 10, PLAY_REPEAT(2));
        play(70, 4, 20, PLAY_REPEAT(1));
        break;
      default: {
        static const pm_uint8_t singleSounds[] PROGMEM = {
          70, 10, 2, PLAY_REPEAT(2)|PLAY_NOW,  // INACTIVITY
          BEEP_DEFAULT_FREQ, 40, 1, PLAY_NOW,  // ERROR
          BEEP_KEY_UP_FREQ, 10, 1, PLAY_NOW,   // KEYPAD_UP
          BEEP_KEY_DOWN_FREQ, 10, 1, PLAY_NOW, // KEYPAD_DOWN
          BEEP_DEFAULT_FREQ, 10, 2, PLAY_NOW,  // MENUS
          0,0,0,0, // TRIM_MOVE
          BEEP_DEFAULT_FREQ, 10, 1, PLAY_NOW,  // WARNING1
          BEEP_DEFAULT_FREQ, 20, 1, PLAY_NOW,  // WARNING2
          BEEP_DEFAULT_FREQ, 30, 1, PLAY_NOW,  // WARNING3
          0,0,0,0, // TRIM_MIDDLE
          BEEP_DEFAULT_FREQ + 50, 10, 1, PLAY_NOW,      // POT_STICK_MIDDLE
          BEEP_DEFAULT_FREQ + 50, 6, 0, 0,              // MIX_WARNING_1
          BEEP_DEFAULT_FREQ + 52, 6, 3, PLAY_REPEAT(1), // MIX_WARNING_2
          BEEP_DEFAULT_FREQ + 54, 6, 3, PLAY_REPEAT(2), // MIX_WARNING_3
          BEEP_DEFAULT_FREQ + 50, 30, 3, PLAY_NOW, // TIMER_00
          BEEP_DEFAULT_FREQ + 50, 15, 3, PLAY_NOW, // TIMER_LT10
          BEEP_DEFAULT_FREQ + 50, 15, 3, PLAY_REPEAT(1)|PLAY_NOW, // TIMER_20
          BEEP_DEFAULT_FREQ + 50, 15, 3, PLAY_REPEAT(2)|PLAY_NOW, // TIMER_30
          BEEP_DEFAULT_FREQ, 10, 1, 0, // FRSKY_BEEP1
          BEEP_DEFAULT_FREQ, 20, 1, 0, // FRSKY_BEEP2
          BEEP_DEFAULT_FREQ, 30, 1, 0, // FRSKY_BEEP3
          BEEP_DEFAULT_FREQ+20, 15, 5, PLAY_REPEAT(2), // FRSKY_WARN1
          BEEP_DEFAULT_FREQ+30, 15, 5, PLAY_REPEAT(2), // FRSKY_WARN2
          BEEP_DEFAULT_FREQ+30, 10, 2, PLAY_REPEAT(2)|PLAY_INCREMENT(2), // FRSKY_CHEEP
          BEEP_DEFAULT_FREQ+50, 5, 10, PLAY_REPEAT(10), // FRSKY_RATATA
          BEEP_DEFAULT_FREQ+50, 5, 50, PLAY_REPEAT(2), // FRSKY_TICK
          10, 20, 5, PLAY_REPEAT(2)|PLAY_INCREMENT(1), // FRSKY_SIREN
        };

        const pm_uint8_t *ptr = &singleSounds[(e-AU_INACTIVITY)<<2];
        uint8_t tFreq = pgm_read_byte_near(ptr++);
        uint8_t tLen = pgm_read_byte_near(ptr++);
        uint8_t tPause = pgm_read_byte_near(ptr++);
        uint8_t tFlags = pgm_read_byte_near(ptr);
        play(tFreq, tLen, tPause, tFlags);
        break;
      }
      }
    }
  }
}

void audioDefevent(uint8_t e)
{
  audio.event(e);
}
# 1 "rtc.cpp"
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


#include <limits.h>
#include "rtc.h"
#include "OpenAVRc.h"

extern void rtcdriver_settime(struct gtm * t);

#define LEAP_SECONDS_POSSIBLE 0

/* Shift A right by B bits portably, by dividing A by 2**B and
   truncating towards minus infinity.  A and B should be free of side
   effects, and B should be in the range 0 <= B <= INT_BITS - 2, where
   INT_BITS is the number of useful bits in an int.  GNU code can
   assume that INT_BITS is at least 32.

   ISO C99 says that A >> B is implementation-defined if A < 0.  Some
   implementations (e.g., UNICOS 9.0 on a Cray Y-MP EL) don't shift
   right in the usual way when A < 0, so SHR falls back on division if
   ordinary A >> B doesn't seem to be the usual signed shift.  */
#define SHR(a, b) (-1 >> 1 == -1 ? (a) >> (b) : (a) / (1 << (b)) - ((a) % (1 << (b)) < 0))

/* The extra casts in the following macros work around compiler bugs,
   e.g., in Cray C 5.0.3.0.  */

/* True if the arithmetic type T is an integer type.  bool counts as
   an integer.  */
#define TYPE_IS_INTEGER(t) ((t) 1.5 == 1)

/* True if negative values of the signed integer type T use two's
   complement, ones' complement, or signed magnitude representation,
   respectively.  Much GNU code assumes two's complement, but some
   people like to be portable to all possible C hosts.  */
#define TYPE_TWOS_COMPLEMENT(t) ((t) ~ (t) 0 == (t) -1)
#define TYPE_ONES_COMPLEMENT(t) ((t) ~ (t) 0 == 0)
#define TYPE_SIGNED_MAGNITUDE(t) ((t) ~ (t) 0 < (t) -1)

/* True if the arithmetic type T is signed.  */
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))

/* The maximum and minimum values for the integer type T.  These
   macros have undefined behavior if T is signed and has padding bits.
   If this is a problem for you, please let us know how to fix it for
   your host.  */
#define TYPE_MINIMUM(t) \
  ((t) (! TYPE_SIGNED (t) \
        ? (t) 0 \
        : TYPE_SIGNED_MAGNITUDE (t) \
        ? ~ (t) 0 \
        : ~ (t) 0 << (sizeof (t) * CHAR_BIT - 1)))
#define TYPE_MAXIMUM(t) \
  ((t) (! TYPE_SIGNED (t) \
        ? (t) -1 \
        : ~ (~ (t) 0 << (sizeof (t) * CHAR_BIT - 1))))

#ifndef TIME_T_MIN
# define TIME_T_MIN TYPE_MINIMUM (gtime_t)
#endif
#ifndef TIME_T_MAX
# define TIME_T_MAX TYPE_MAXIMUM (gtime_t)
#endif
#define TIME_T_MIDPOINT (SHR (TIME_T_MIN + TIME_T_MAX, 1) + 1)

/* Verify a requirement at compile-time (unlike assert, which is runtime).  */
#define verify(name, assertion) struct name { char a[(assertion) ? 1 : -1]; }

verify (gtime_t_is_integer, TYPE_IS_INTEGER (gtime_t));
verify (twos_complement_arithmetic, TYPE_TWOS_COMPLEMENT (int));
/* The code also assumes that signed integer overflow silently wraps
   around, but this assumption can't be stated without causing a
   diagnostic on some hosts.  */

#define EPOCH_YEAR 1970
#define TM_YEAR_BASE 1900
verify (base_year_is_a_multiple_of_100, TM_YEAR_BASE % 100 == 0);

/* Return 1 if YEAR + TM_YEAR_BASE is a leap year.  */
static inline int
leapyear (long int year)
{
  /* Don't add YEAR to TM_YEAR_BASE, as that might overflow.
     Also, work even if YEAR is negative.  */
  return
    ((year & 3) == 0
     && (year % 100 != 0
         || ((year / 100) & 3) == (- (TM_YEAR_BASE / 100) & 3)));
}

const unsigned short int __mon_yday[2][13] = {
  /* Normal years.  */
  { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
  /* Leap years.  */
  { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/* Compute the `struct tm' representation of *T,
   offset OFFSET seconds east of UTC,
   and store year, yday, mon, mday, wday, hour, min, sec into *TP.
   Return nonzero if successful.  */
int
__offtime (
  gtime_t *t,
  long int offset,
  struct gtm *tp)
{
  long int days, rem, y;
  const unsigned short int *ip;

  days = *t / SECS_PER_DAY;
  rem = *t % SECS_PER_DAY;
  rem += offset;
  while (rem < 0) {
    rem += SECS_PER_DAY;
    --days;
  }
  while (rem >= (long int)SECS_PER_DAY) {
    rem -= SECS_PER_DAY;
    ++days;
  }
  tp->tm_hour = rem / SECS_PER_HOUR;
  rem %= SECS_PER_HOUR;
  tp->tm_min = rem / 60;
  tp->tm_sec = rem % 60;
  /* January 1, 1970 was a Thursday.  */
  tp->tm_wday = (4 + days) % 7;
  if (tp->tm_wday < 0)
    tp->tm_wday += 7;
  y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

  while (days < 0 || days >= (leapyear (y) ? 366 : 365)) {
    /* Guess a corrected year, assuming 365 days per year.  */
    long int yg = y + days / 365 - (days % 365 < 0);

    /* Adjust DAYS and Y to match the guessed year.  */
    days -= ((yg - y) * 365
             + LEAPS_THRU_END_OF (yg - 1)
             - LEAPS_THRU_END_OF (y - 1));
    y = yg;
  }
  tp->tm_year = y - 1900;
  if (tp->tm_year != y - 1900) {
    /* The year cannot be represented due to overflow.  */
    // __set_errno (EOVERFLOW);
    return 0;
  }
  tp->tm_yday = days;
  ip = __mon_yday[leapyear(y)];
  for (y = 11; days < (long int) ip[y]; --y)
    continue;
  days -= ip[y];
  tp->tm_mon = y;
  tp->tm_mday = days + 1;
  return 1;
}

/* time_r function implementations */
// G: No time zones in our implementation so just do the converion from gtime_t to struct tm
struct gtm *
__localtime_r (gtime_t * t, struct gtm * tp)
{
  __offtime(t, 0, tp);
  return tp;
}


/* Return an integer value measuring (YEAR1-YDAY1 HOUR1:MIN1:SEC1) -
   (YEAR0-YDAY0 HOUR0:MIN0:SEC0) in seconds, assuming that the clocks
   were not adjusted between the time stamps.

   The YEAR values uses the same numbering as TP->tm_year.  Values
   need not be in the usual range.  However, YEAR1 must not be less
   than 2 * INT_MIN or greater than 2 * INT_MAX.

   The result may overflow.  It is the caller's responsibility to
   detect overflow.  */

static inline gtime_t
ydhms_diff (long int year1, long int yday1, int hour1, int min1, int sec1,
            int year0, int yday0, int hour0, int min0, int sec0)
{
  verify (C99_integer_division, -1 / 2 == 0);
  verify (long_int_year_and_yday_are_wide_enough,
          INT_MAX <= LONG_MAX / 2 || TIME_T_MAX <= UINT_MAX);

  /* Compute intervening leap days correctly even if year is negative.
     Take care to avoid integer overflow here.  */
  int a4 = SHR (year1, 2) + SHR (TM_YEAR_BASE, 2) - ! (year1 & 3);
  int b4 = SHR (year0, 2) + SHR (TM_YEAR_BASE, 2) - ! (year0 & 3);
  int a100 = a4 / 25 - (a4 % 25 < 0);
  int b100 = b4 / 25 - (b4 % 25 < 0);
  int a400 = SHR (a100, 2);
  int b400 = SHR (b100, 2);
  int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);

  /* Compute the desired time in gtime_t precision.  Overflow might
     occur here.  */
  gtime_t tyear1 = year1;
  gtime_t years = tyear1 - year0;
  gtime_t days = 365 * years + yday1 - yday0 + intervening_leap_days;
  gtime_t hours = 24 * days + hour1 - hour0;
  gtime_t minutes = 60 * hours + min1 - min0;
  gtime_t seconds = 60 * minutes + sec1 - sec0;
  return seconds;
}

/* Return a gtime_t value corresponding to (YEAR-YDAY HOUR:MIN:SEC),
   assuming that *T corresponds to *TP and that no clock adjustments
   occurred between *TP and the desired time.
   If TP is null, return a value not equal to *T; this avoids false matches.
   If overflow occurs, yield the minimal or maximal value, except do not
   yield a value equal to *T.  */
static gtime_t
guess_time_tm (long int year, long int yday, int hour, int min, int sec,
               gtime_t *t, struct gtm *tp)
{
  if (tp) {
    gtime_t d = ydhms_diff (year, yday, hour, min, sec,
                            tp->tm_year, tp->tm_yday,
                            tp->tm_hour, tp->tm_min, tp->tm_sec);
    gtime_t t1 = *t + d;
    if ((t1 < *t) == (TYPE_SIGNED (gtime_t) ? d < 0 : TIME_T_MAX / 2 < d))
      return t1;
  }

  /* Overflow occurred one way or another.  Return the nearest result
     that is actually in range, except don't report a zero difference
     if the actual difference is nonzero, as that would cause a false
     match; and don't oscillate between two values, as that would
     confuse the spring-forward gap detector.  */
  return (*t < TIME_T_MIDPOINT
          ? (*t <= TIME_T_MIN + 1 ? *t + 1 : TIME_T_MIN)
          : (TIME_T_MAX - 1 <= *t ? *t - 1 : TIME_T_MAX));
}

/* Use CONVERT to convert *T to a broken down time in *TP.
   If *T is out of range for conversion, adjust it so that
   it is the nearest in-range value and then convert that.  */
static struct gtm *
ranged_convert (struct gtm *(*convert) (gtime_t *, struct gtm *),
                gtime_t *t, struct gtm *tp)
{
  struct gtm *r = convert (t, tp);

  if (!r && *t) {
    gtime_t bad = *t;
    gtime_t ok = 0;

    /* BAD is a known unconvertible gtime_t, and OK is a known good one.
       Use binary search to narrow the range between BAD and OK until
       they differ by 1.  */
    while (bad != ok + (bad < 0 ? -1 : 1)) {
      gtime_t mid = *t = (bad < 0
                          ? bad + ((ok - bad) >> 1)
                          : ok + ((bad - ok) >> 1));
      r = convert (t, tp);
      if (r)
        ok = mid;
      else
        bad = mid;
    }

    if (!r && ok) {
      /* The last conversion attempt failed;
         revert to the most recent successful attempt.  */
      *t = ok;
      r = convert (t, tp);
    }
  }

  return r;
}

/* Convert *TP to a gtime_t value, inverting
   the monotonic and mostly-unit-linear conversion function CONVERT.
   Use *OFFSET to keep track of a guess at the offset of the result,
   compared to what the result would be for UTC without leap seconds.
   If *OFFSET's guess is correct, only one CONVERT call is needed.
   This function is external because it is used also by timegm.c.  */
gtime_t
__mktime_internal (struct gtm *tp,
                   struct gtm *(*convert) (gtime_t *, struct gtm *),
                   gtime_t *offset)
{
  gtime_t t, gt, t0, t1, t2;
  struct gtm tm;

  /* The maximum number of probes (calls to CONVERT) should be enough
     to handle any combinations of time zone rule changes, solar time,
     leap seconds, and oscillations around a spring-forward gap.
     POSIX.1 prohibits leap seconds, but some hosts have them anyway.  */
  int remaining_probes = 6;

  /* Time requested.  Copy it in case CONVERT modifies *TP; this can
     occur if TP is localtime's returned value and CONVERT is localtime.  */
  int sec = tp->tm_sec;
  int min = tp->tm_min;
  int hour = tp->tm_hour;
  int mday = tp->tm_mday;
  int mon = tp->tm_mon;
  int year_requested = tp->tm_year;

  /* Ensure that mon is in range, and set year accordingly.  */
  int mon_remainder = mon % 12;
  int negative_mon_remainder = mon_remainder < 0;
  int mon_years = mon / 12 - negative_mon_remainder;
  long int lyear_requested = year_requested;
  long int year = lyear_requested + mon_years;

  /* The other values need not be in range:
     the remaining code handles minor overflows correctly,
     assuming int and gtime_t arithmetic wraps around.
     Major overflows are caught at the end.  */

  /* Calculate day of year from year, month, and day of month.
     The result need not be in range.  */
  int mon_yday = ((__mon_yday[leapyear (year)]
                   [mon_remainder + 12 * negative_mon_remainder])
                  - 1);
  long int lmday = mday;
  long int yday = mon_yday + lmday;

  gtime_t guessed_offset = *offset;

  int sec_requested = sec;

  /*
     if (LEAP_SECONDS_POSSIBLE)
      {
        // Handle out-of-range seconds specially,
        // since ydhms_tm_diff assumes every minute has 60 seconds.
        if (sec < 0)
          sec = 0;
        if (59 < sec)
          sec = 59;
      }
  */

  /* Invert CONVERT by probing.  First assume the same offset as last
     time.  */

  t0 = ydhms_diff (year, yday, hour, min, sec,
                   EPOCH_YEAR - TM_YEAR_BASE, 0, 0, 0, - guessed_offset);

  if (TIME_T_MAX / INT_MAX / 366 / 24 / 60 / 60 < 3) {
    /* gtime_t isn't large enough to rule out overflows, so check
       for major overflows.  A gross check suffices, since if t0
       has overflowed, it is off by a multiple of TIME_T_MAX -
       TIME_T_MIN + 1.  So ignore any component of the difference
       that is bounded by a small value.  */

    /* Approximate log base 2 of the number of time units per
       biennium.  A biennium is 2 years; use this unit instead of
       years to avoid integer overflow.  For example, 2 average
       Gregorian years are 2 * 365.2425 * 24 * 60 * 60 seconds,
       which is 63113904 seconds, and rint (log2 (63113904)) is
       26.  */
    int ALOG2_SECONDS_PER_BIENNIUM = 26;
    int ALOG2_MINUTES_PER_BIENNIUM = 20;
    int ALOG2_HOURS_PER_BIENNIUM = 14;
    int ALOG2_DAYS_PER_BIENNIUM = 10;
    int LOG2_YEARS_PER_BIENNIUM = 1;

    int approx_requested_biennia =
      (SHR (year_requested, LOG2_YEARS_PER_BIENNIUM)
       - SHR (EPOCH_YEAR - TM_YEAR_BASE, LOG2_YEARS_PER_BIENNIUM)
       + SHR (mday, ALOG2_DAYS_PER_BIENNIUM)
       + SHR (hour, ALOG2_HOURS_PER_BIENNIUM)
       + SHR (min, ALOG2_MINUTES_PER_BIENNIUM)
       + (LEAP_SECONDS_POSSIBLE
          ? 0
          : SHR (sec, ALOG2_SECONDS_PER_BIENNIUM)));

    int approx_biennia = SHR (t0, ALOG2_SECONDS_PER_BIENNIUM);
    int diff = approx_biennia - approx_requested_biennia;
    int abs_diff = diff < 0 ? - diff : diff;

    /* IRIX 4.0.5 cc miscalculates TIME_T_MIN / 3: it erroneously
       gives a positive value of 715827882.  Setting a variable
       first then doing math on it seems to work.
       (ghazi@caip.rutgers.edu) */
    gtime_t time_t_max = TIME_T_MAX;
    gtime_t time_t_min = TIME_T_MIN;
    gtime_t overflow_threshold =
      (time_t_max / 3 - time_t_min / 3) >> ALOG2_SECONDS_PER_BIENNIUM;

    if (overflow_threshold < abs_diff) {
      /* Overflow occurred.  Try repairing it; this might work if
         the time zone offset is enough to undo the overflow.  */
      gtime_t repaired_t0 = -1 - t0;
      approx_biennia = SHR (repaired_t0, ALOG2_SECONDS_PER_BIENNIUM);
      diff = approx_biennia - approx_requested_biennia;
      abs_diff = diff < 0 ? - diff : diff;
      if (overflow_threshold < abs_diff)
        return -1;
      guessed_offset += repaired_t0 - t0;
      t0 = repaired_t0;
    }
  }

  /* Repeatedly use the error to improve the guess.  */

  for (t = t1 = t2 = t0;
       (gt = guess_time_tm (year, yday, hour, min, sec, &t,
                            ranged_convert (convert, &t, &tm)),
        t != gt);
       t1 = t2, t2 = t, t = gt)
    if (t == t1 && t != t2)
      goto offset_found;
    else if (--remaining_probes == 0)
      return -1;

offset_found:
  *offset = guessed_offset + t - t0;

  if (LEAP_SECONDS_POSSIBLE && sec_requested != tm.tm_sec) {
    /* Adjust time to reflect the tm_sec requested, not the normalized value.
       Also, repair any damage from a false match due to a leap second.  */
    int sec_adjustment = (sec == 0 && tm.tm_sec == 60) - sec;
    t1 = t + sec_requested;
    t2 = t1 + sec_adjustment;
    if (((t1 < t) != (sec_requested < 0))
        | ((t2 < t1) != (sec_adjustment < 0))
        | ! convert (&t2, &tm))
      return -1;
    t = t2;
  }

  *tp = tm;
  return t;
}

/* Convert *TP to a gtime_t value.  */
gtime_t
gmktime (struct gtm *tp)
{
  // no time zone stuff. Just do the math ;)
  static gtime_t localtime_offset;
  return __mktime_internal (tp, __localtime_r, &localtime_offset);
}

/* Fill a (struct tm) TP* from a given gtime_t time stamp */
gtime_t
filltm(gtime_t *t, struct gtm *tp)
{
  return __offtime(t, 0, tp);
}

gtime_t g_rtcTime;
uint8_t g_ms100 = 0; // global to allow time set function to reset to zero

void gettime(struct gtm * tm)
{
  filltm(&g_rtcTime, tm); // create a struct tm date/time structure from global unix time stamp
}


# 1 "rtc_driver.cpp"
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


/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */

#include "thirdparty/FatFs/integer.h"
#include "i2c_master.h"
#include "OpenAVRc.h"

#define RTC_ADRESS  (0x68 << 1) //0x68 for ZS042

typedef struct {
  uint32_t    year;   /* 2000..2099 */
  uint8_t    month;  /* 1..12 */
  uint8_t    mday;   /* 1.. 31 */
  uint8_t    wday;   /* 1..7 */
  uint8_t    hour;   /* 0..23 */
  uint8_t    min;    /* 0..59 */
  uint8_t    sec;    /* 0..59 */
} RTClock_t;

/*-------------------------------------------------*/
/* RTC functions                                   */

uint8_t g9x_rtcGetTime (RTClock_t *rtc)
{
  uint8_t buf[7];

  if (i2c_readReg(RTC_ADRESS, 0, buf, 7)) return 0;

  rtc->sec = (buf[0] & 0x0F) + ((buf[0] >> 4) & 7) * 10;
  rtc->min = (buf[1] & 0x0F) + (buf[1] >> 4) * 10;
  rtc->hour = (buf[2] & 0x0F) + ((buf[2] >> 4) & 3) * 10;
  rtc->wday = (buf[2] & 0x07);
  rtc->mday = (buf[4] & 0x0F) + ((buf[4] >> 4) & 3) * 10;
  rtc->month = (buf[5] & 0x0F) + ((buf[5] >> 4) & 1) * 10;
  rtc->year = 2000 + (buf[6] & 0x0F) + (buf[6] >> 4) * 10;

  return 1;
}

uint8_t g9x_rtcSetTime (const RTClock_t *rtc)
{
  uint8_t buf[7];

  buf[0] = rtc->sec / 10 * 16 + rtc->sec % 10;
  buf[1] = rtc->min / 10 * 16 + rtc->min % 10;
  buf[2] = rtc->hour / 10 * 16 + rtc->hour % 10;
  buf[3] = rtc->wday & 7;
  buf[4] = rtc->mday / 10 * 16 + rtc->mday % 10;
  buf[5] = rtc->month / 10 * 16 + rtc->month % 10;
  buf[6] = (rtc->year - 2000) / 10 * 16 + (rtc->year - 2000) % 10;
  return !i2c_writeReg(RTC_ADRESS, 0, buf, 7);
}

void rtcGetTime(struct gtm * utm)
{
  RTClock_t rtc = {0,0,0,0,0,0,0};

  g9x_rtcGetTime(&rtc);

  utm->tm_year = rtc.year - 1900;
  utm->tm_mon =  rtc.month - 1;
  utm->tm_mday = rtc.mday;
  utm->tm_hour = rtc.hour;
  utm->tm_min =  rtc.min;
  utm->tm_sec =  rtc.sec;
  utm->tm_wday = rtc.wday - 1;
}

void rtcSetTime(struct gtm * t)
{
  g_rtcTime = gmktime(t); // update local timestamp and get wday calculated
  g_ms100 = 0; // start of next second begins now

  RTClock_t rtc;
  rtc.year = t->tm_year + 1900;
  rtc.month = t->tm_mon + 1;
  rtc.mday = t->tm_mday;
  rtc.hour = t->tm_hour;
  rtc.min = t->tm_min;
  rtc.sec = t->tm_sec;
  rtc.wday = t->tm_wday + 1;
  g9x_rtcSetTime(&rtc);
}

void rtcInit ()
{
  uint8_t buf[8];	/* RTC R/W buffer */

  //i2c_writeReg(RTC_ADRESS, 0x0E, 0x00, 1); /* Init registers */
  //i2c_writeReg(RTC_ADRESS, 0x0F, 0x00, 1); /* Init registers */
  /* Read RTC registers */
  if (i2c_readReg(RTC_ADRESS, 0x00, buf, 8)) return;	/* Check IIC error */

  if (buf[7] & 0x20) {	/* When data has been volatiled, set default time */
    /* Clear nv-ram. Reg[8..63] */
    memset(buf, 0, 8);
    for (uint8_t adr = 8; adr < 64; adr += 8)
      i2c_writeReg(0x0D, adr, buf, 8);
    /* Reset time to Jan 1, '08. Reg[0..7] */
    buf[4] = 1;
    buf[5] = 1;
    buf[6] = 8;
    i2c_writeReg(0x0D, 0, buf, 8);
  }

  struct gtm utm;
  rtcGetTime(&utm);
  g_rtcTime = gmktime(&utm);
}

# 1 "targets/mega2560/voice_jq6500.cpp"
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


// JQ6500 mp3 module driver
// 6 X 10 (bits) X 104µs = 6.24 mS to send a playfile command

#include "../../OpenAVRc.h"


#define QUEUE_LENGTH (24*2)  //bytes

enum JQ6500_State {
	START = 0, //0x7E Start
	NUMBY,     //0x04 Num bytes follow
	SELEC,     //0X03 Select file
	FILEH,     //0x00 Dummy file
	FILEL,     //0x00 Dummy file
	TERMI,     //0xEF Termination
};

uint8_t JQstate = START;
uint8_t JQ6500_Data[6] = {0x7E, //Start
                          0x04, //Num bytes follow
                          0X03, //Select file
                          0x00, //Dummy MSB file
                          0x00, //Dummy LSB file
                          0xEF}; //Termination

uint8_t JQ6500_playlist[QUEUE_LENGTH] = {0};
uint8_t JQ6500_InputIndex = 0;
uint8_t JQ6500_PlayIndex = 0;

void pushPrompt(uint16_t prompt)
{
	// if mute active => no voice
	if (g_eeGeneral.beepMode == e_mode_quiet) return;
	++prompt;  // With SDformatter, first FAT address = 1 : MP3 files in a directory
	/* Load playlist and activate interrupt */
	JQ6500_playlist[JQ6500_InputIndex++] = (uint8_t)(prompt >> 8);    // MSB first
	JQ6500_playlist[JQ6500_InputIndex++] = (uint8_t)(prompt);  // LSB after
	if (JQ6500_InputIndex == QUEUE_LENGTH) JQ6500_InputIndex = 0;

}

void InitJQ6500UartTx()
{
#if !defined(SIMU)

#undef BAUD
#define BAUD 9600
#include <util/setbaud.h>

  UBRRH_N(TLM_JQ6500) = UBRRH_VALUE;
  UBRRL_N(TLM_JQ6500) = UBRRL_VALUE;
  UCSRA_N(TLM_JQ6500) &= ~(1 << U2X_N(TLM_JQ6500)); // disable double speed operation.

  UCSRB_N(TLM_JQ6500) = 0 | (0 << RXCIE_N(TLM_JQ6500)) | (0 << TXCIE_N(TLM_JQ6500))    // set 8N1
  | (0 << UDRIE_N(TLM_JQ6500)) | (0 << RXEN_N(TLM_JQ6500)) | (1 << TXEN_N(TLM_JQ6500)) // disable RX enable TX
  | (0 << UCSZ2_N(TLM_JQ6500));

  UCSRC_N(TLM_JQ6500) = 0 | (1 << UCSZ1_N(TLM_JQ6500)) | (1 << UCSZ0_N(TLM_JQ6500));

#endif
}

void JQ6500Check()
{
#if !defined(SIMU)
  if ((JQ6500_PlayIndex == JQ6500_InputIndex) || (JQstate != START) || (JQ6500_BUSY) ) return;

  JQstate = START;

  JQ6500_Data[3] = JQ6500_playlist[JQ6500_PlayIndex++];
  JQ6500_Data[4] = JQ6500_playlist[JQ6500_PlayIndex++];
  if (JQ6500_PlayIndex == QUEUE_LENGTH) JQ6500_PlayIndex = 0;

  UDR_N(TLM_JQ6500) = JQ6500_Data[JQstate]; // Send Datas
  UCSRB_N(TLM_JQ6500) |= (1 << UDRIE_N(TLM_JQ6500)); // enable UDRE(TLM_JQ6500) interrupt
#endif
}

ISR(USART_UDRE_vect_N(TLM_JQ6500))
{
#if !defined(SIMU)
  if (JQstate != TERMI) {
    UDR_N(TLM_JQ6500) = JQ6500_Data[++JQstate];
  } else {
    UCSRB_N(TLM_JQ6500) &= ~(1 << UDRIE_N(TLM_JQ6500)); // disable UDRE(TLM_JQ6500) interrupt
    JQstate = START;
  }
#endif
}

# 1 "haptic.cpp"
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


#include "OpenAVRc.h"

hapticQueue::hapticQueue()
{
  buzzTimeLeft = 0;
  buzzPause = 0;

  t_queueRidx = 0;
  t_queueWidx = 0;

  hapticTick = 0;
}

void hapticQueue::heartbeat()
{
  if (buzzTimeLeft > 0) {
    buzzTimeLeft--; // time gets counted down
    if (hapticTick-- > 0) {
      hapticOn();
    } else {
      hapticOff();
      //hapticTick = HAPTIC_STRENGTH();
    }
  } else {
    hapticOff();
    if (buzzPause > 0) {
      buzzPause--;
    } else if (t_queueRidx != t_queueWidx) {
      buzzTimeLeft = queueHapticLength[t_queueRidx];
      buzzPause = queueHapticPause[t_queueRidx];
      if (!queueHapticRepeat[t_queueRidx]--) {
        t_queueRidx = (t_queueRidx + 1) & (HAPTIC_QUEUE_LENGTH-1);
      }
    }
  }
}

void hapticQueue::play(uint8_t tLen, uint8_t tPause, uint8_t tFlags)
{
  tLen = getHapticLength(tLen);

  if ((tFlags & PLAY_NOW) || (!busy() && empty())) {
    buzzTimeLeft = tLen;
    buzzPause = tPause;
    t_queueWidx = t_queueRidx;
  } else {
    tFlags += 1;
  }

  tFlags &= 0x0f;
  if (tFlags) {
    uint8_t next_queueWidx = (t_queueWidx + 1) & (HAPTIC_QUEUE_LENGTH-1);
    if (next_queueWidx != t_queueRidx) {
      queueHapticLength[t_queueWidx] = tLen;
      queueHapticPause[t_queueWidx] = tPause;
      queueHapticRepeat[t_queueWidx] = tFlags-1;
      t_queueWidx = next_queueWidx;
    }
  }
}

void hapticQueue::event(uint8_t e)
{
  if (g_eeGeneral.hapticMode>0 || (g_eeGeneral.hapticMode==0 && e>=AU_WARNING1) || (g_eeGeneral.hapticMode>=-1 && e<=AU_ERROR)) {
    if (e <= AU_ERROR)
      play(15, 3, PLAY_NOW);
    else if (e <= AU_TRIM_MOVE)
      play(5, 0, PLAY_NOW);
    else if (e <= AU_TIMER_LT10)
      play(15, 3, PLAY_NOW);
    else if (e < AU_FRSKY_FIRST)
      play(15, 3, (e-AU_TIMER_20+1)|PLAY_NOW);
    else if (e >= AU_FRSKY_LAST && empty()) {
      play(30, 10, 0);
      play(10, 50-10*(e-AU_FRSKY_LAST), (e-AU_FRSKY_LAST));
    }
  }
}

hapticQueue haptic;
# 1 "OpenAVRc.cpp"
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


#include "OpenAVRc.h"
#include "timers.h"
#include "misclib.h"

EEGeneral  g_eeGeneral;
ModelData  g_model;

bool pwrCheck = true;
bool unexpectedShutdown = false;

/* AVR: mixer duration in 1/16ms */
uint16_t maxMixerDuration;
uint16_t lastMixerDuration;
uint16_t g_tmr1Latency_max;
uint16_t g_tmr1Latency_min;

#if defined(AUDIO)
  audioQueue  audio;
#endif

uint8_t heartbeat;
uint8_t stickMode;

#if ROTARY_ENCODERS > 0
  uint8_t rotEncADebounce;
#endif
#if ROTARY_ENCODERS > 1
  uint8_t rotEncBDebounce;
#endif


#if defined(OVERRIDE_CHANNEL_FUNCTION)
  safetych_t safetyCh[NUM_CHNOUT];
#endif

uint8_t gazSource;
bool enableGaz = false;

void setGazSource()
{
  uint8_t idx = g_model.thrTraceSrc + MIXSRC_Thr;
  if (idx > MIXSRC_Thr)
    idx += 1;
  if (idx >= MIXSRC_FIRST_POT+NUM_POTS)
    idx += MIXSRC_CH1 - MIXSRC_FIRST_POT - NUM_POTS;
  gazSource = idx;
}

bool rangeModeIsOn = false; // manage low power TX
struct Module RFModule;

uint8_t * packet = pulses2MHz.pbyte; //protocol global packet

void sendOptionsSettingsPpm()
{
  SetRfOptionSettings(pgm_get_far_address(RfOpt_PPM_Ser), // Used by PPM
                      STR_DUMMY,
                      STR_DUMMY,
                      STR_DUMMY,
                      STR_DUMMY,
                      STR_DUMMY,
                      STR_DUMMY,
                      STR_DUMMY);
  if (s_current_protocol==(PROTOCOL_PPM16-1)) g_model.PPMNCH = limit<uint8_t>(0,g_model.PPMNCH,2);
  g_model.PPMFRAMELENGTH = (g_model.PPMNCH-2) * 8;
  RFModule.mode = NORMAL_MODE;
}

struct RfOptionSettingsstruct RfOptionSettings; // used in menumodelsetup

void SetRfOptionSettings(uint_farptr_t RfOptSet,
                         const pm_char* rfSubTypeNames,
                         const pm_char* rfOptionValue1Name,
                         const pm_char* rfOptionValue2Name,
                         const pm_char* rfOptionValue3Name,
                         const pm_char* rfOptionBool1Name,
                         const pm_char* rfOptionBool2Name,
                         const pm_char* rfOptionBool3Name)
{
  RfOptionSettings.rfProtoNeed = pgm_read_byte_far(RfOptSet);
  RfOptionSettings.rfSubTypeMax = pgm_read_byte_far(++RfOptSet);
  RfOptionSettings.rfSubTypeNames = rfSubTypeNames;
  RfOptionSettings.rfOptionValue1Min = pgm_read_byte_far(++RfOptSet);
  RfOptionSettings.rfOptionValue1Max = pgm_read_byte_far(++RfOptSet);
  RfOptionSettings.rfOptionValue1Name = rfOptionValue1Name;
  RfOptionSettings.rfOptionValue2Min = pgm_read_byte_far(++RfOptSet);
  RfOptionSettings.rfOptionValue2Max = pgm_read_byte_far(++RfOptSet);
  RfOptionSettings.rfOptionValue2Name = rfOptionValue2Name;
  RfOptionSettings.rfOptionValue3Max = pgm_read_byte_far(++RfOptSet);
  RfOptionSettings.rfOptionValue3Name = rfOptionValue3Name;
  RfOptionSettings.rfOptionBool1Used = (RfOptionSettings.rfProtoNeed & BOOL1USED);
  RfOptionSettings.rfOptionBool1Name = rfOptionBool1Name;
  RfOptionSettings.rfOptionBool2Used = (RfOptionSettings.rfProtoNeed & BOOL2USED);
  RfOptionSettings.rfOptionBool2Name = rfOptionBool2Name;
  RfOptionSettings.rfOptionBool3Used = (RfOptionSettings.rfProtoNeed & BOOL3USED);
  RfOptionSettings.rfOptionBool3Name = rfOptionBool3Name;
}

void LimitRfOptionSettings()
{
  g_model.rfSubType = limit( (uint8_t)0x0, (uint8_t)g_model.rfSubType, (uint8_t)RfOptionSettings.rfSubTypeMax);
  g_model.rfOptionValue1 = limit( RfOptionSettings.rfOptionValue1Min, g_model.rfOptionValue1, RfOptionSettings.rfOptionValue1Max);
  g_model.rfOptionValue2 = limit( RfOptionSettings.rfOptionValue2Min, g_model.rfOptionValue2, RfOptionSettings.rfOptionValue2Max);
  g_model.rfOptionValue3 = limit( (int8_t) -16, (int8_t) g_model.rfOptionValue3, (int8_t) RfOptionSettings.rfOptionValue3Max);
}

union ReusableBuffer reusableBuffer;

const pm_uint8_t bchout_ar[] PROGMEM = {
  0x1B, 0x1E, 0x27, 0x2D, 0x36, 0x39,
  0x4B, 0x4E, 0x63, 0x6C, 0x72, 0x78,
  0x87, 0x8D, 0x93, 0x9C, 0xB1, 0xB4,
  0xC6, 0xC9, 0xD2, 0xD8, 0xE1, 0xE4
};

uint8_t channel_order(uint8_t x)
{
  return ( ((pgm_read_byte_near(bchout_ar + g_eeGeneral.templateSetup) >> (6-(x-1) * 2)) & 3 ) + 1 );
}

/*
mode1 rud ele thr ail
mode2 rud thr ele ail
mode3 ail ele thr rud
mode4 ail thr ele rud
*/
const pm_uint8_t modn12x3[] PROGMEM = {
  0, 1, 2, 3,
  0, 2, 1, 3,
  3, 1, 2, 0,
  3, 2, 1, 0
};

volatile tmr10ms_t g_tmr10ms;

tmr10ms_t Bind_tmr10ms = 0;

void per10ms()
{
  ++g_tmr10ms;

#if defined(GUI)
  if (lightOffCounter) --lightOffCounter;
  if (flashCounter) --flashCounter;
  if (noHighlightCounter) --noHighlightCounter;
#endif

  if (trimsCheckTimer) --trimsCheckTimer;
  if (ppmInputValidityTimer) --ppmInputValidityTimer;


#if defined(RTCLOCK)
  /* Update global Date/Time every 100 per10ms cycles */
  if (++g_ms100 == 100) {
    ++g_rtcTime;   // inc global unix timestamp one second
    g_ms100 = 0;
  }
#endif

  readKeysAndTrims();

#if defined(ROTARY_ENCODER_NAVIGATION)
  if (IS_RE_NAVIGATION_ENABLE()) {
    static rotenc_t rePreviousValue;
    rotenc_t reNewValue = (g_rotenc[NAVIGATION_RE_IDX()] / ROTARY_ENCODER_GRANULARITY);
    int8_t scrollRE = reNewValue - rePreviousValue;
    if (scrollRE) {
      rePreviousValue = reNewValue;
      putEvent(scrollRE < 0 ? EVT_ROTARY_LEFT : EVT_ROTARY_RIGHT);
    }
    uint8_t evt = s_evt;
    if (EVT_KEY_MASK(evt) == BTN_REa + NAVIGATION_RE_IDX()) {
      if (IS_KEY_BREAK(evt)) {
        putEvent(EVT_ROTARY_BREAK);
      } else if (IS_KEY_LONG(evt)) {
        putEvent(EVT_ROTARY_LONG);
      }
    }
  }
#endif

#if defined(FRSKY)
  if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol))
    telemetryInterrupt10ms();
#endif

  // These moved here from evalFlightModeMixes() to improve beep trigger reliability.
#if defined(PWM_BACKLIGHT)
  if (EVERY_PERIOD(g_tmr10ms, 4))
    backlightFade(); // increment or decrement brightness until target brightness is reached
#endif

#if defined(VOICE_JQ6500) && !defined(SIMU)
  JQ6500Check();
#endif

#if !defined(AUDIO)
  if (mixWarning & 1) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,  0)) AUDIO_MIX_WARNING(1);
  if (mixWarning & 2) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 64) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 72)) AUDIO_MIX_WARNING(2);
  if (mixWarning & 4) if(EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,128) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256,136) || EVERY_PERIOD_WITH_OFFSET(g_tmr10ms, 256, 144)) AUDIO_MIX_WARNING(3);
#endif

#if defined(SDCARD)
  sdPoll10ms();
#endif

#if ROTARY_ENCODERS > 0
  if (rotEncADebounce) {
    if (!(rotEncADebounce >>= 1)) ENABLEROTENCAISR(); // Re enable rotencA isr (deboucing)
  }
#endif
#if ROTARY_ENCODERS > 1
  if (rotEncBDebounce) {
    if (!(rotEncBDebounce >>= 1)) ENABLEROTENCBISR(); // Re enable rotencB isr (deboucing)
  }
#endif

  if (Bind_tmr10ms)
  {
    if (!--Bind_tmr10ms)
      RFModule.mode = NORMAL_MODE;
  }

  heartbeat |= HEART_TIMER_10MS;

}

FlightModeData *flightModeAddress(uint8_t idx)
{
  return &g_model.flightModeData[idx];
}

ExpoData *expoAddress(uint8_t idx )
{
  return &g_model.expoData[idx];
}

MixData *mixAddress(uint8_t idx)
{
  return &g_model.mixData[idx];
}

LimitData *limitAddress(uint8_t idx)
{
  return &g_model.limitData[idx];
}


void generalDefault()
{
  memclear(&g_eeGeneral, sizeof(g_eeGeneral));
  g_eeGeneral.version  = EEPROM_VER;
  g_eeGeneral.contrast = 15;
  g_eeGeneral.vBatMin = 50;
  g_eeGeneral.vBatMax = 90;
  g_eeGeneral.vBatWarn = 60;
#if defined(DEFAULT_MODE)
  g_eeGeneral.stickMode = DEFAULT_MODE-1;
#endif
  g_eeGeneral.backlightMode = e_backlight_mode_all;
  g_eeGeneral.lightAutoOff = 2;
  g_eeGeneral.inactivityTimer = 10;
  g_eeGeneral.chkSum = 0xFFFF;
}

uint16_t evalChkSum()
{
  uint16_t sum = 0;
  const int16_t *calibValues = (const int16_t *) &g_eeGeneral.calib[0];
  for (int i=0; i<12; i++)
    sum += calibValues[i];
  return sum;
}


#if defined(TEMPLATES)
inline void applyDefaultTemplate()
{
  applyTemplate(TMPL_SIMPLE_4CH); // calls eeDirty internally
  g_model.PPMNCH = 2; // 8Ch
}
#else
void applyDefaultTemplate()
{
  eeDirty(EE_MODEL);

  for (int i=0; i<NUM_STICKS; i++) {
    MixData *mix = mixAddress(i);
    mix->destCh = i;
    mix->weight = 100;
    mix->srcRaw = MIXSRC_Rud - 1 + channel_order(i+1);
  }
  g_model.PPMNCH = 2; // 8Ch
}
#endif


#if defined(SDCARD)
bool isFileAvailable(const char * filename)
{
  return f_stat(filename, 0) == FR_OK;
}
#endif

void modelDefault(uint8_t id)
{
  memset(&g_model, 0, sizeof(g_model));
  applyDefaultTemplate();
  g_model.modelId = id+1;
}


#if defined(AUTOSOURCE)
int8_t getMovedSource()
{
  int8_t result = 0;
  static tmr10ms_t lastMove10msTick = 0;
  static int16_t sourcesStates[NUM_STICKS+NUM_POTS];

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
    if (abs(calibratedStick[i] - sourcesStates[i]) > 512) {
      result = MIXSRC_Rud+i;
      break;
    }
  }

  bool recent = (ELAPSED_10MS_TICK_SINCE(lastMove10msTick) >= MS_TO_10MS_TICK(100));
  if (recent) {
    result = 0;
  }

  if (result || recent) {
    memcpy(sourcesStates, calibratedStick, sizeof(sourcesStates));
  }

  lastMove10msTick = get_tmr10ms();
  return result;
}
#endif

#if defined(FLIGHT_MODES)
uint8_t getFlightMode()
{
  for (uint8_t i=1; i<MAX_FLIGHT_MODES; i++) {
    FlightModeData *phase = &g_model.flightModeData[i];
    if (phase->swtch && getSwitch(phase->swtch)) {
      return i;
    }
  }
  return 0;
}
#endif

trim_t getRawTrimValue(uint8_t phase, uint8_t idx)
{
  FlightModeData *p = flightModeAddress(phase);
  return p->trim[idx];
}

int getTrimValue(uint8_t phase, uint8_t idx)
{
  return getRawTrimValue(getTrimFlightPhase(phase, idx), idx);
}

void setTrimValue(uint8_t phase, uint8_t idx, int trim)
{
  FlightModeData *p = flightModeAddress(phase);
  p->trim[idx] = trim;
  eeDirty(EE_MODEL);
}

uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx)
{
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    if (phase == 0) return 0;
    trim_t trim = getRawTrimValue(phase, idx);
    if (trim <= TRIM_EXTENDED_MAX) return phase;
    uint8_t result = trim-TRIM_EXTENDED_MAX-1;
    if (result >= phase) ++result;
    phase = result;
  }
  return 0;
}

#if defined(ROTARY_ENCODERS)
uint8_t getRotaryEncoderFlightPhase(uint8_t idx)
{
  uint8_t phase = mixerCurrentFlightMode;
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    if (phase == 0) return 0;
    int16_t value = flightModeAddress(phase)->rotaryEncoders[idx];
    if (value <= ROTARY_ENCODER_MAX) return phase;
    uint8_t result = value-ROTARY_ENCODER_MAX-1;
    if (result >= phase) ++result;
    phase = result;
  }
  return 0;
}

int16_t getRotaryEncoder(uint8_t idx)
{
  return flightModeAddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx];
}

void incRotaryEncoder(uint8_t idx, int8_t inc)
{
  g_rotenc[idx] += inc;
  int16_t *value = &(flightModeAddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx]);
  *value = limit((int16_t)-ROTARY_ENCODER_MAX, (int16_t)(*value + (inc * 8)), (int16_t)+ROTARY_ENCODER_MAX);
  eeDirty(EE_MODEL);
}
#endif

#if defined(GVARS)

#define SET_GVAR_VALUE(idx, phase, value) \
  GVAR_VALUE(idx, phase) = value;         \
  eeDirty(EE_MODEL);                      \



  uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx)
  {
    for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
      if (phase == 0) return 0;
      int16_t val = GVAR_VALUE(idx, phase); // TODO phase at the end everywhere to be consistent!
      if (val <= GVAR_MAX) return phase;
      uint8_t result = val-GVAR_MAX-1;
      if (result >= phase) ++result;
      phase = result;
    }
    return 0;
  }

  int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase)
  {
    if (GV_IS_GV_VALUE(x, min, max)) {
      int8_t idx = GV_INDEX_CALCULATION(x, max);
      int8_t mul = 1;

      if (idx < 0) {
        idx = -1-idx;
        mul = -1;
      }

      x = GVAR_VALUE(idx, getGVarFlightPhase(phase, idx)) * mul;
    }
    return limit(min, x, max);
  }

  void setGVarValue(uint8_t idx, int16_t value, int8_t phase)
  {
    value = limit((int16_t)-GVAR_LIMIT,value,(int16_t)GVAR_LIMIT); //Limit Gvar value
    phase = getGVarFlightPhase(phase, idx);
    if (GVAR_VALUE(idx, phase) != value) {
      SET_GVAR_VALUE(idx, phase, value);
    }
  }

#endif

#if   defined(FRSKY)

/*
ls_telemetry_value_t minTelemValue(uint8_t channel)
{
switch (channel) {
	case TELEM_TIMER1:
	case TELEM_TIMER2:
	return -3600;
	case TELEM_ALT:
	case TELEM_MIN_ALT:
	case TELEM_MAX_ALT:
	case TELEM_GPSALT:
	return -500;
	case TELEM_T1:
	case TELEM_MAX_T1:
	case TELEM_T2:
	case TELEM_MAX_T2:
	return -30;
	case TELEM_ACCx:
	case TELEM_ACCy:
	case TELEM_ACCz:
	return -1000;
	case TELEM_VSPEED:
	return -3000;
	default:
	return 0;
}
}
*/

ls_telemetry_value_t maxTelemValue(uint8_t channel)
{
  switch (channel) {
  case TELEM_FUEL:
  case TELEM_RSSI_TX:
  case TELEM_RSSI_RX:
    return 100;
  case TELEM_HDG:
    return 180;
  default:
    return 255;
  }
}
#endif

getvalue_t convert8bitsTelemValue(uint8_t channel, ls_telemetry_value_t value)
{
  getvalue_t result;
  switch (channel) {
  case TELEM_TIMER1:
  case TELEM_TIMER2:
    result = value * 5;
    break;
#if defined(FRSKY)
  case TELEM_ALT:
  case TELEM_GPSALT:
  case TELEM_MAX_ALT:
  case TELEM_MIN_ALT:
    result = value * 8 - 500;
    break;
  case TELEM_RPM:
  case TELEM_MAX_RPM:
    result = value * 50;
    break;
  case TELEM_T1:
  case TELEM_T2:
  case TELEM_MAX_T1:
  case TELEM_MAX_T2:
    result = (getvalue_t)value - 30;
    break;
  case TELEM_CELL:
  case TELEM_HDG:
  case TELEM_SPEED:
  case TELEM_MAX_SPEED:
    result = value * 2;
    break;
  case TELEM_ASPEED:
  case TELEM_MAX_ASPEED:
    result = value * 20;
    break;
  case TELEM_DIST:
  case TELEM_MAX_DIST:
    result = value * 8;
    break;
  case TELEM_CURRENT:
  case TELEM_POWER:
  case TELEM_MAX_CURRENT:
  case TELEM_MAX_POWER:
    result = value * 5;
    break;
  case TELEM_CONSUMPTION:
    result = value * 100;
    break;
  case TELEM_VSPEED:
    result = ((getvalue_t)value - 125) * 10;
    break;
#endif
  default:
    result = value;
    break;
  }
  return result;
}

#if defined(FRSKY)
FORCEINLINE void convertUnit(getvalue_t & val, uint8_t & unit)
{
  if (IS_IMPERIAL_ENABLE()) {
    if (unit == UNIT_TEMPERATURE) {
      val += 18;
      val *= 115;
      val >>= 6;
    }
    if (unit == UNIT_DIST) {
      // m to ft *105/32
      val = val * 3 + (val >> 2) + (val >> 5);
    }
    if (unit == UNIT_FEET) {
      unit = UNIT_DIST;
    }
    if (unit == UNIT_KTS) {
      // kts to mph
      unit = UNIT_SPEED;
      val = (val * 23) / 20;
    }
  } else {
    if (unit == UNIT_KTS) {
      // kts to km/h
      unit = UNIT_SPEED;
      val = (val * 50) / 27;
    }
  }

  if (unit == UNIT_HDG) {
    unit = UNIT_TEMPERATURE;
  }
}
#endif

#define INAC_STICKS_SHIFT   6
#define INAC_SWITCHES_SHIFT 8
bool inputsMoved()
{
  uint8_t sum = 0;
  for (uint8_t i=0; i<NUM_STICKS; i++)
    sum += anaIn(i) >> INAC_STICKS_SHIFT;
  for (uint8_t i=0; i<NUM_SWITCHES; i++)
    sum += getValue(MIXSRC_FIRST_SWITCH+i) >> INAC_SWITCHES_SHIFT;

  if (abs((int8_t)(sum-inactivity.sum)) > 1) {
    inactivity.sum = sum;
    return true;
  } else {
    return false;
  }
}

void checkBacklight()
{
  static uint8_t tmr10ms ;


  uint8_t x = g_blinkTmr10ms;
  if (tmr10ms != x) {
    tmr10ms = x;
    if (inputsMoved()) {
      inactivity.counter = 0;
      if (g_eeGeneral.backlightMode & e_backlight_mode_sticks)
        backlightOn();
    }

    bool backlightOn = (g_eeGeneral.backlightMode == e_backlight_mode_on || lightOffCounter || isFunctionActive(FUNCTION_BACKLIGHT));
    if (flashCounter) backlightOn = !backlightOn;
    if (backlightOn)
      BACKLIGHT_ON();
    else
      BACKLIGHT_OFF();

  }
}

void backlightOn()
{
  lightOffCounter = ((uint16_t)g_eeGeneral.lightAutoOff*250) << 1;
}

#if MENUS_LOCK == 1
bool readonly = true;
bool readonlyUnlocked()
{
  if (readonly) {
    POPUP_WARNING(STR_MODS_FORBIDDEN);
    return false;
  } else {
    return true;
  }
}
#endif

#if defined(SPLASH)
void doSplash()
{
  uint8_t audioTadaPerformed = 0;

  if (SPLASH_NEEDED()) {
    displaySplash();

    tmr10ms_t contrastStart10msTick = get_tmr10ms();
    tmr10ms_t splashStart10msTick   = get_tmr10ms();
    uint8_t contrast = 10;

    lcdSetRefVolt(contrast);

    getADC(); // init ADC array

    inputsMoved();

    do{

      getADC();

      SIMU_SLEEP(1);

#if defined(FSPLASH)
      // Splash is forced, we can't skip it
      if (!(g_eeGeneral.splashMode & 0x04)) {
#endif

        if (ELAPSED_10MS_TICK_SINCE(splashStart10msTick) >= MS_TO_10MS_TICK(SPLASH_TIMEOUT_MS / 2) && (!audioTadaPerformed)) {
          AUDIO_TADA();
          audioTadaPerformed = 1;    // Splash duration/2 to play TADA sound
        }

        if (keyDown() || inputsMoved()) return;

#if defined(FSPLASH)
      }
#endif

      if (!pwrCheck) {
        return;
      }

      if (ELAPSED_10MS_TICK_SINCE(contrastStart10msTick) >= MS_TO_10MS_TICK(100)) {
        contrastStart10msTick = get_tmr10ms();
        if (contrast < g_eeGeneral.contrast) {
          contrast += 1;
          lcdSetRefVolt(contrast);
        }
      }

      checkBacklight();
    }while(ELAPSED_10MS_TICK_SINCE(splashStart10msTick) < MS_TO_10MS_TICK(SPLASH_TIMEOUT_MS));
  }
}
#else
#define Splash()
#define doSplash()
#endif

#if defined(GUI)
void checkAll()
{
  checkLowEEPROM();
  checkTHR();
  checkSwitches();
  PLAY_MODEL_NAME();
  clearKeyEvents();
}
#endif // GUI


void checkLowEEPROM()
{
  if (g_eeGeneral.disableMemoryWarning) return;
  if (EeFsGetFree() < 100) {
    ALERT(STR_EEPROMWARN, STR_EEPROMLOWMEM, AU_ERROR);
  }
}

void checkTHR()
{
  uint8_t thrchn = ((g_model.thrTraceSrc==0) || (g_model.thrTraceSrc>NUM_POTS)) ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1;
  // throttle channel is either the stick according stick mode (already handled in evalInputs)
  // or P1 to P3;
  // in case an output channel is choosen as throttle source (thrTraceSrc>NUM_POTS) we assume the throttle stick is the input
  // no other information available at the moment, and good enough to my option (otherwise too much exceptions...)

  if (g_model.disableThrottleWarning) {
    return;
  }

  getADC();

  evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job

  int16_t v = calibratedStick[thrchn];
  if (v <= THRCHK_DEADBAND-1024) {
    return; // prevent warning if throttle input OK
  }

  // first - display warning; also deletes inputs if any have been before
  MESSAGE(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_THROTTLE_ALERT);


  while (1) {

    getADC();

    evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job

    v = calibratedStick[thrchn];

    if (!pwrCheck) {
      break;
    }

    if (keyDown() || v <= THRCHK_DEADBAND-1024) {
      break;
    }

    checkBacklight();

    MYWDT_RESET();
  }
}

void checkAlarm() // added by Gohst
{
  if (g_eeGeneral.disableAlarmWarning) {
    return;
  }

  if (IS_SOUND_OFF()) {
    ALERT(STR_ALARMSWARN, STR_ALARMSDISABLED, AU_ERROR);
  }
}

void alert(const pm_char * t, const pm_char *s MESSAGE_SOUND_ARG)
{
  MESSAGE(t, s, STR_PRESSANYKEY, sound);

  while(1)
  {

    if (keyDown())
      return;  // wait for key release

    checkBacklight();

    if (!pwrCheck)
    {
      boardOff(); // turn power off now
    }
    MYWDT_RESET();
  }
}

#if defined(GVARS)
int8_t trimGvar[NUM_STICKS] = { -1, -1, -1, -1 };
#endif

uint8_t checkTrim(uint8_t event)
{
  int8_t k = EVT_KEY_MASK(event) - TRM_BASE;
  if (k>=0 && k<8 && !IS_KEY_BREAK(event)) {
    // LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
    uint8_t idx = CONVERT_MODE((uint8_t)k/2);
    uint8_t phase;
    int before;
    bool thro;


#if defined(GVARS)
    if (TRIM_REUSED(idx)) {
      phase = getGVarFlightPhase(mixerCurrentFlightMode, trimGvar[idx]);
      before = GVAR_VALUE(trimGvar[idx], phase);
      thro = false;
    } else {
      phase = getTrimFlightPhase(mixerCurrentFlightMode, idx);
      before = getRawTrimValue(phase, idx);
      thro = (idx==THR_STICK && g_model.thrTrim);
    }
#else
    phase = getTrimFlightPhase(mixerCurrentFlightMode, idx);
    before = getRawTrimValue(phase, idx);
    thro = (idx==THR_STICK && g_model.thrTrim);
#endif
    int8_t trimInc = g_model.trimInc + 1;
    int8_t v = (trimInc==-1) ? min(32, abs(before)/4+1) : (1 << trimInc); // TODO flash saving if (trimInc < 0)
    if (thro) v = 4; // if throttle trim and trim trottle then step=4
    int16_t after = (k&1) ? before + v : before - v;   // positive = k&1
    int8_t TODOcheckbeeptrimalwaysat1;
    int8_t beepTrim = false;
    for (int16_t mark=TRIM_MIN; mark<=TRIM_MAX; mark+=TRIM_MAX) {
      if ((mark!=0 || !thro) && ((mark!=TRIM_MIN && after>=mark && before<mark) || (mark!=TRIM_MAX && after<=mark && before>mark))) {
        after = mark;
        beepTrim = (mark == 0 ? 1 : 2);
      }
    }

    if ((before<after && after>TRIM_MAX) || (before>after && after<TRIM_MIN)) {
      if (!g_model.extendedTrims || TRIM_REUSED(idx)) after = before;
    }

    /*if (after < TRIM_EXTENDED_MIN) {
      after = TRIM_EXTENDED_MIN;
    }
    if (after > TRIM_EXTENDED_MAX) {
      after = TRIM_EXTENDED_MAX;
    }*/
    limit<int16_t>(TRIM_EXTENDED_MIN,after,TRIM_EXTENDED_MAX);


#if defined(GVARS)
    if (TRIM_REUSED(idx)) {
      SET_GVAR_VALUE(trimGvar[idx], phase, after);
    } else
#endif
    {
      setTrimValue(phase, idx, after);
    }

#if defined(AUDIO)
    // toneFreq higher/lower according to trim position
    // limit the frequency, range -125 to 125 = toneFreq: 19 to 101

    /*if (after > TRIM_MAX)
      after = TRIM_MAX;
    if (after < TRIM_MIN)
      after = TRIM_MIN;*/
    limit<int16_t>(TRIM_MIN,after,TRIM_MAX);

    after >>= 2;
    after += 60;
#endif

    if (beepTrim) {
      if (beepTrim == 1) {
        AUDIO_TRIM_MIDDLE(after);
        pauseEvents(event);
      } else {
        AUDIO_TRIM_END(after);
        killEvents(event);
      }
    } else {
      AUDIO_TRIM(event, after);
    }
    return 0;
  }
  return event;
}

uint16_t s_anaFilt[NUMBER_ANALOG];

// #define STARTADCONV (ADCSRA  = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADSC) | (1 << ADIE))
// G: Note that the above would have set the ADC prescaler to 128, equating to
// 125KHz sample rate. We now sample at 500KHz, with oversampling and other
// filtering options to produce 11-bit results.

uint16_t anaIn(uint8_t chan)
{
#if defined REV_EVO_V1
  static const pm_char crossAna[] PROGMEM = {2,3,0,1,4,5}; // Hardwired Pots / connectors.
#else // M2560 "Standard"
  static const pm_char crossAna[] PROGMEM = {3,1,2,0,4,5,6,7};
#endif
  volatile uint16_t *p = &s_anaFilt[pgm_read_byte_near(crossAna+chan)];
  return *p;
}

uint16_t g_vbat10mV = 0;
uint16_t lightOffCounter;
uint8_t flashCounter = 0;

uint16_t sessionTimer;
uint16_t s_timeCumThr;    // THR in 1/16 sec
uint16_t s_timeCum16ThrP; // THR% in 1/16 sec

uint8_t  trimsCheckTimer = 0;


void flightReset()
{
  // we don't reset the whole audio here (the tada.wav would be cut, if a prompt is queued before FlightReset, it should be played)
  // TODO check if the vario / background music are stopped correctly if switching to a model which doesn't have these functions enabled

  if (!IS_MANUAL_RESET_TIMER(0)) {
    timerReset(0);
  }

#if TIMERS > 1
  if (!IS_MANUAL_RESET_TIMER(1)) {
    timerReset(1);
  }
#endif

#if defined(FRSKY)
  telemetryResetValue();
#endif

  s_mixer_first_run_done = false;

  RESET_THR_TRACE();
}

#if defined(THRTRACE)
  uint8_t  s_traceBuf[MAXTRACE];
  uint8_t  s_traceWr;
  int16_t  s_traceCnt;
  uint8_t  s_cnt_10s;
  uint16_t s_cnt_samples_thr_10s;
  uint16_t s_sum_samples_thr_10s;
#endif

FORCEINLINE void evalTrims()
{
  uint8_t phase = mixerCurrentFlightMode;
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    // do trim -> throttle trim if applicable
    int16_t trim = getTrimValue(phase, i);
    if (i==THR_STICK && g_model.thrTrim) {
      int16_t trimMin = g_model.extendedTrims ? TRIM_EXTENDED_MIN : TRIM_MIN;
      trim = (((g_model.throttleReversed)?(int32_t)(trim+trimMin):(int32_t)(trim-trimMin)) * (RESX-anas[i])) >> (RESX_SHIFT+1);
    }
    if (trimsCheckTimer) {
      trim = 0;
    }
    trims[i] = trim*2;
  }
}

bool s_mixer_first_run_done = false;

void doMixerCalculations()
{
  static tmr10ms_t lastTMR = 0;

  tmr10ms_t tmr10ms = get_tmr10ms();
  uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
  // handle tick10ms overrun
  // correct overflow handling costs a lot of code; happens only each 11 min;
  // therefore forget the exact calculation and use only 1 instead; good compromise

  lastTMR = tmr10ms;

  getADC();

  evalMixes(tick10ms);

  if (tick10ms) {


    /* Throttle trace */
    int16_t val;

    if (g_model.thrTraceSrc > NUM_POTS) {
      uint8_t ch = g_model.thrTraceSrc-NUM_POTS-1;
      val = channelOutputs[ch];

      LimitData *lim = limitAddress(ch);
      int16_t gModelMax = LIMIT_MAX_RESX(lim);
      int16_t gModelMin = LIMIT_MIN_RESX(lim);

      if (lim->revert)
        val = -val + gModelMax;
      else
        val = val - gModelMin;

#if defined(PPM_LIMITS_SYMETRICAL)
      if (lim->symetrical) {
        val -= calc1000toRESX(lim->offset);
      }
#endif

      gModelMax -= gModelMin; // we compare difference between Max and Mix for recaling needed; Max and Min are shifted to 0 by default
      // usually max is 1024 min is -1024 --> max-min = 2048 full range

      if (gModelMax!=0 && gModelMax!=2048) val = (int32_t) (val << 11) / (gModelMax); // rescaling only needed if Min, Max differs

      if (val<0) val=0;  // prevent val be negative, which would corrupt throttle trace and timers; could occur if safetyswitch is smaller than limits
    } else {
      val = RESX + (g_model.thrTraceSrc == 0 ? rawAnas[THR_STICK] : calibratedStick[g_model.thrTraceSrc+NUM_STICKS-1]);
    }

    val >>= (RESX_SHIFT-6); // calibrate it (resolution increased by factor 4)

    evalTimers(val, tick10ms);

    static uint8_t  s_cnt_100ms;
    static uint8_t  s_cnt_1s;
    static uint8_t  s_cnt_samples_thr_1s;
    static uint16_t s_sum_samples_thr_1s;

    ++s_cnt_samples_thr_1s;
    s_sum_samples_thr_1s+=val;

    if ((s_cnt_100ms += tick10ms) >= 10) { // 0.1sec
      s_cnt_100ms -= 10;
      s_cnt_1s += 1;

      logicalSwitchesTimerTick();
      checkTrainerSignalWarning();

      if (s_cnt_1s >= 10) { // 1sec
        s_cnt_1s -= 10;
        ++sessionTimer;

      if ((rangeModeIsOn) && !(menuHandlers[menuLevel] == menuModelSetup))
      {
         rangeModeIsOn = false; // Reset range mode if not in menuModelSetup
      }

        struct t_inactivity *ptrInactivity = &inactivity;
        FORCE_INDIRECT(ptrInactivity) ;
        ptrInactivity->counter++;
        if ((((uint8_t)ptrInactivity->counter)&0x07)==0x01 && g_eeGeneral.inactivityTimer && ptrInactivity->counter > ((uint16_t)g_eeGeneral.inactivityTimer*60))
          AUDIO_INACTIVITY();

#if defined(AUDIO)
        if (mixWarning & 1) if ((sessionTimer&0x03)==0) AUDIO_MIX_WARNING(1);
        if (mixWarning & 2) if ((sessionTimer&0x03)==1) AUDIO_MIX_WARNING(2);
        if (mixWarning & 4) if ((sessionTimer&0x03)==2) AUDIO_MIX_WARNING(3);
#endif

        val = s_sum_samples_thr_1s / s_cnt_samples_thr_1s;
        s_timeCum16ThrP += (val>>3);  // s_timeCum16ThrP would overrun if we would store throttle value with higher accuracy; therefore stay with 16 steps
        if (val) s_timeCumThr += 1;
        s_sum_samples_thr_1s>>=2;  // correct better accuracy now, because trace graph can show this information; in case thrtrace is not active, the compile should remove this

#if defined(THRTRACE)
        // throttle trace is done every 10 seconds; Tracebuffer is adjusted to screen size.
        // in case buffer runs out, it wraps around
        // resolution for y axis is only 32, therefore no higher value makes sense
        s_cnt_samples_thr_10s += s_cnt_samples_thr_1s;
        s_sum_samples_thr_10s += s_sum_samples_thr_1s;

        if (++s_cnt_10s >= 10) { // 10s
          s_cnt_10s -= 10;
          val = s_sum_samples_thr_10s / s_cnt_samples_thr_10s;
          s_sum_samples_thr_10s = 0;
          s_cnt_samples_thr_10s = 0;
          s_traceBuf[s_traceWr++] = val;
          if (s_traceWr >= MAXTRACE) s_traceWr = 0;
          if (s_traceCnt >= 0) ++s_traceCnt;
        }
#endif

        s_cnt_samples_thr_1s = 0;
        s_sum_samples_thr_1s = 0;
      }
    }
  }
}

void OpenAVRcStart()
{
#if defined(SDCARD) && !defined(SIMU)
  sdMountPoll(); // Mount SD if it is not a WDT reboot
#endif
  doSplash();
#if defined(GUI)
  checkAlarm();
  checkAll();
  if (g_eeGeneral.chkSum != evalChkSum()) {
    chainMenu(menuFirstCalib);
  }
#endif

}

void OpenAVRcClose()
{
  AUDIO_BYE();

#if defined(FRSKY)
  // TODO needed? telemetryEnd();
#endif


#if defined(SDCARD)
  closeLogs();
#endif

#if defined(HAPTIC)
  hapticOff();
#endif

  saveTimers();

  if (s_eeDirtyMsk & EE_MODEL) {
    displayPopup(STR_SAVEMODEL);
  }

  g_eeGeneral.unexpectedShutdown = 0;

  eeDirty(EE_GENERAL);
  eeCheck(true);


#if defined(SDCARD)
  SD_spi_power_off();
#endif
}

void checkBattery()
{
  uint32_t instant_vbat = anaIn(TX_VOLTAGE);

#define BANDGAP 5000 // 5 Volts : We use AVCC.
#if defined(REV_EVO_V1)
  instant_vbat *= 40L * BANDGAP;
  instant_vbat /= (4095L * 100L);
  instant_vbat += 20L; // No Calibration Allowed.
  // Schottky Diode drops 0.2V before a potential divider which reduces the input to the ADC by 1/4.
#else
  instant_vbat *= 2889L*(BANDGAP/100);
  instant_vbat /= 2047L*100L;
  instant_vbat += 38L;
  instant_vbat += g_eeGeneral.txVoltageCalibration;
  // Schottky Diode drops 0.38V before a potential divider which reduces the input to the ADC by 1/2.8889.
#endif

  if (!g_vbat10mV) {
    g_vbat10mV = instant_vbat;
  }

  g_vbat10mV = ((g_vbat10mV << 3) + instant_vbat) / 9; // Simple low pass filter

  if (IS_TXBATT_WARNING() && (g_vbat10mV > (g_eeGeneral.vBatMin*9)) && ((sessionTimer&0x1F)==0x10)) { // No Audio Alarm if TX Battery < VCCMIN X .9 & 30 Sec
    AUDIO_TX_BATTERY_LOW();
  }
}

volatile uint8_t g_tmr16KHz; //continuous timer 16ms (16MHz/1024/256) -- 8-bit counter overflow

ISR(TIMER_16KHZ_VECT, ISR_NOBLOCK)
{
  ++g_tmr16KHz; // gruvin: Not 16KHz. Overflows occur at 61.035Hz (1/256th of 15.625KHz)
  // to give *16.384ms* intervals. Kind of matters for accuracy elsewhere. ;)
  // g_tmr16KHz is used to software-construct a 16-bit timer
  // from TIMER-0 (8-bit). See getTmr16KHz, below.
}

uint16_t getTmr16KHz()
{
#if defined(SIMU)
  uint16_t simu_tmr16 = get_tmr10ms() * 160;
  return simu_tmr16;
#else
  /*uint8_t hb;
  uint8_t lb;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    hb  = g_tmr16KHz;
    lb  = COUNTER_16KHZ;
  }
  return (hb<<8)|lb;*/

  while(1){
    uint8_t hb  = g_tmr16KHz;
    uint8_t lb  = COUNTER_16KHZ;
    if(hb-g_tmr16KHz==0) return (hb<<8)|lb;
  }

#endif
}

ISR(TIMER_10MS_VECT, ISR_NOBLOCK)
{
#if defined(SIMU)
ISR10msLoop_is_runing = true;
#endif

// Clocks every 9.984ms & 10.048ms
  static uint8_t accuracyWarble = 0;

#if defined(AUDIO)
  AUDIO_HEARTBEAT();
#endif

#if defined(BUZZER)
  BUZZER_HEARTBEAT();
#endif

#if defined(HAPTIC)
  HAPTIC_HEARTBEAT();
#endif

  SIMU_PROCESSEVENTS;

  per10ms();

  TIMER_10MS_COMPVAL += (++accuracyWarble & 0x03) ? 156 : 157; // Clock correction

#if defined(SIMU)
ISR10msLoop_is_runing = false;
#endif

}

#define INSTANT_TRIM_MARGIN 15 /* around 1.5% */

void instantTrim()
{
  evalInputs(e_perout_mode_notrainer);

  for (uint8_t stick=0; stick<NUM_STICKS; stick++) {
    if (stick!=THR_STICK) {
      // don't instant trim the throttle stick
      uint8_t trim_phase = getTrimFlightPhase(mixerCurrentFlightMode, stick);
      int16_t delta = anas[stick];
      if (abs(delta) >= INSTANT_TRIM_MARGIN) {
        int16_t trim = limit<int16_t>(TRIM_EXTENDED_MIN, (delta + trims[stick]) / 2, TRIM_EXTENDED_MAX);
        setTrimValue(trim_phase, stick, trim);
      }
    }
  }
  eeDirty(EE_MODEL);
  AUDIO_WARNING2();
}

void copySticksToOffset(uint8_t ch)
{
  int32_t zero = (int32_t)channelOutputs[ch];

  evalFlightModeMixes(e_perout_mode_nosticks+e_perout_mode_notrainer, 0);
  int32_t val = chans[ch];
  LimitData *ld = limitAddress(ch);
  limit_min_max_t lim = LIMIT_MIN(ld);
  if (val < 0) {
    val = -val;
    lim = LIMIT_MIN(ld);
  }
  zero = (zero*25600 - val*lim) / (26214-val);
  ld->offset = (ld->revert ? -zero : zero);
  eeDirty(EE_MODEL);
}

void copyTrimsToOffset(uint8_t ch)
{
  int16_t zero;

  evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  zero = applyLimits(ch, chans[ch]);

  evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

  int16_t output = applyLimits(ch, chans[ch]) - zero;
  int16_t v = g_model.limitData[ch].offset;
  if (g_model.limitData[ch].revert) output = -output;
  v += output;
  g_model.limitData[ch].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire

  eeDirty(EE_MODEL);
}

void moveTrimsToOffsets() // copy state of 3 primary to subtrim
{
  int16_t zeros[NUM_CHNOUT];

  evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    zeros[i] = applyLimits(i, chans[i]);
  }

  evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    int16_t output = applyLimits(i, chans[i]) - zeros[i];
    int16_t v = g_model.limitData[i].offset;
    if (g_model.limitData[i].revert) output = -output;
    v += output;
    g_model.limitData[i].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire
  }

  // reset all trims, except throttle (if throttle trim)
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    if (i!=THR_STICK || !g_model.thrTrim) {
      int16_t original_trim = getTrimValue(mixerCurrentFlightMode, i);
      for (uint8_t phase=0; phase<MAX_FLIGHT_MODES; ++phase) {
        trim_t trim = getRawTrimValue(phase, i);
        if (trim <= TRIM_EXTENDED_MAX)
          setTrimValue(phase, i, trim - original_trim);
      }
    }
  }

  eeDirty(EE_MODEL);
  AUDIO_WARNING2();
}

#if defined(ROTARY_ENCODERS)
volatile rotenc_t g_rotenc[ROTARY_ENCODERS] = {0};
#elif defined(ROTARY_ENCODER_NAVIGATION)
volatile rotenc_t g_rotenc[1] = {0};
#endif

#if !defined(SIMU)
#define STACKPTR     _SFR_IO16(0x3D)
extern unsigned char __bss_end ;
void stackPaint()
{
  // Init Stack while interrupts are disabled
  unsigned char *p ;
  unsigned char *q ;

  p = (unsigned char *) STACKPTR ;
  q = &__bss_end ;
  p -= 2 ;
  while ( p > q ) {
    *p-- = 0x55 ;
  }
}
#endif

uint16_t stackAvailable()
{
#if defined(SIMU)
  return 1800;
#else
  unsigned char *p ;

  p = &__bss_end + 1 ;
  while ( *p++ == 0x55 );
  return p - &__bss_end ;
#endif
}


uint16_t freeRam()
{
#if defined(SIMU)
  return 2000;
#else
  extern int __heap_start, *__brkval;
  uint16_t v;
  return (uint16_t) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
#endif
}

void OpenAVRcInit(uint8_t mcusr)
{

  eeReadAll();

#if MENUS_LOCK == 1
  getMovedSwitch();
  if (TRIMS_PRESSED() && g_eeGeneral.switchUnlockStates==switches_states) {
    readonly = false;
  }
#endif

#if defined(VOICE)
//  setVolume(g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF);
#endif

#if defined(RTCLOCK)
  rtcInit();
#endif

  if (g_eeGeneral.backlightMode != e_backlight_mode_off) backlightOn(); // on Tx start turn the light on

  if (UNEXPECTED_SHUTDOWN()) {
    unexpectedShutdown = true;
  } else {
    OpenAVRcStart();
  }

  if (!g_eeGeneral.unexpectedShutdown) {
    g_eeGeneral.unexpectedShutdown = 1;
    eeDirty(EE_GENERAL);
  }

#if defined(GUI)
  lcdSetContrast();
#endif
  backlightOn();

  doMixerCalculations();
  s_mixer_first_run_done = true;

  startPulses(PROTOCMD_INIT);

#if !defined(SIMU)
  wdt_enable(WDTO_500MS); // Enable watchdog
#endif
}

#if !defined(SIMU)
int main()
{
#else
int simumain()
{
  simu_off = false;
#endif
  // G: The WDT remains active after a WDT reset -- at maximum clock speed. So it's
  // important to disable it before commencing with system initialisation (or
  // we could put a bunch more MYWDT_RESET()s in. But I don't like that approach
  // during boot up.)
#if !defined(SIMU)
  uint8_t mcusr = MCUSR; // save the WDT (etc) flags
  MCUSR = 0; // must be zeroed before disabling the WDT
  MCUCR |= (1<<JTD);    // Disable JTAG port that can interfere with POT3
  MCUCR |= (1<<JTD);   // Must be done twice within four cycles
  wdt_disable();
#endif //SIMU

  boardInit();

#if defined(GUI)
  lcdInit();
#endif

#if !defined(SIMU)
  stackPaint();
#endif

#if defined(GUI)
  menuHandlers[0] = menuMainView;
#if MENUS_LOCK != 2 /*no menus*/
  menuHandlers[1] = menuModelSelect;
#endif
#endif

#if defined(GUI)
  lcdSetRefVolt(25);
#endif

  sei(); // interrupts needed for telemetryInit and eeReadAll.

#ifdef MENU_ROTARY_SW
  init_rotary_sw();
#endif

  OpenAVRcInit(mcusr);

#if !defined(SIMU)
  while (1) {
#else // Simu main loop function
  SimuMainLoop();
  return 0;
} // Close simumain()
void SimuMainLoop() // Create loop function
{
  simu_mainloop_is_runing = true;
#endif //SIMU
    if (!pwrCheck)
#if !defined(SIMU)
      break;
#else
    {
      shutDownSimu();
      simu_mainloop_is_runing = false;
      return;
    }
#endif

    perMain();

    if (heartbeat == HEART_WDT_CHECK) {
      MYWDT_RESET();
      heartbeat = 0;
    }
#if defined (SIMU)
    simu_mainloop_is_runing = false;
#endif
  }

#if defined(SIMU)
  void shutDownSimu()
  {
    simu_shutDownSimu_is_runing = true;
#endif

    // Time to switch off
    lcdClear();
    displayPopup(STR_SHUTDOWN);
    SIMU_SLEEP(200);
    OpenAVRcClose();
    SIMU_SLEEP(200);
    lcdClear() ;
    lcdRefresh() ;
    boardOff(); // Only turn power off if necessary
    wdt_disable();
#if !defined(SIMU)
    while(1); // never return from main() - there is no code to return back, if any delays occurs in physical power it does dead loop.
#endif
  }

# 1 "functions.cpp"
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


#include "OpenAVRc.h"
#include "timers.h"

CustomFunctionsContext modelFunctionsContext = { 0 };


#if defined(VOICE)
PLAY_FUNCTION(playValue, source_t idx)
{
  if (IS_FAI_FORBIDDEN(idx))
    return;

  if (idx == MIXSRC_NONE)
    return;

  getvalue_t val = getValue(idx);

  switch (idx) {
  case MIXSRC_FIRST_TELEM+TELEM_TX_VOLTAGE-1:
    PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
    break;
  case MIXSRC_FIRST_TELEM+TELEM_TIMER1-1:
  case MIXSRC_FIRST_TELEM+TELEM_TIMER2-1:
    PLAY_DURATION(val, 0);
    break;
#if defined(FRSKY)
  case MIXSRC_FIRST_TELEM+TELEM_RSSI_TX-1:
  case MIXSRC_FIRST_TELEM+TELEM_RSSI_RX-1:
    PLAY_NUMBER(val, 1+UNIT_DB, 0);
    break;
  case MIXSRC_FIRST_TELEM+TELEM_MIN_A1-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_A2-1:
    idx -= TELEM_MIN_A1-TELEM_A1;
  // no break
  case MIXSRC_FIRST_TELEM+TELEM_A1-1:
  case MIXSRC_FIRST_TELEM+TELEM_A2-1:
    if (TELEMETRY_STREAMING()) {
      idx -= (MIXSRC_FIRST_TELEM+TELEM_A1-1);
      uint8_t att = 0;
      int16_t converted_value =  div10_and_round(applyChannelRatio(idx, val));;
      if (ANA_CHANNEL_UNIT(idx) < UNIT_RAW) {
        att = PREC1;
      }
      PLAY_NUMBER(converted_value, 1+ANA_CHANNEL_UNIT(idx), att);
    }
    break;
  case MIXSRC_FIRST_TELEM+TELEM_CELL-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_CELL-1:
    PLAY_NUMBER(div10_and_round(val), 1+UNIT_VOLTS, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_VFAS-1:
  case MIXSRC_FIRST_TELEM+TELEM_CELLS_SUM-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_CELLS_SUM-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_VFAS-1:
    PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_CURRENT-1:
  case MIXSRC_FIRST_TELEM+TELEM_MAX_CURRENT-1:
    PLAY_NUMBER(val, 1+UNIT_AMPS, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_ACCx-1:
  case MIXSRC_FIRST_TELEM+TELEM_ACCy-1:
  case MIXSRC_FIRST_TELEM+TELEM_ACCz-1:
    PLAY_NUMBER(div10_and_round(val), 1+UNIT_G, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_VSPEED-1:
    PLAY_NUMBER(div10_and_round(val), 1+UNIT_METERS_PER_SECOND, PREC1);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_ASPEED-1:
  case MIXSRC_FIRST_TELEM+TELEM_MAX_ASPEED-1:
    PLAY_NUMBER(val/10, 1+UNIT_KTS, 0);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_CONSUMPTION-1:
    PLAY_NUMBER(val, 1+UNIT_MAH, 0);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_POWER-1:
    PLAY_NUMBER(val, 1+UNIT_WATTS, 0);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_ALT-1:
  case MIXSRC_FIRST_TELEM+TELEM_MIN_ALT-1:
  case MIXSRC_FIRST_TELEM+TELEM_MAX_ALT-1:
#if defined(WS_HOW_HIGH)
    if (IS_IMPERIAL_ENABLE() && IS_USR_PROTO_WS_HOW_HIGH())
      PLAY_NUMBER(val, 1+UNIT_FEET, 0);
    else
#endif
      PLAY_NUMBER(val, 1+UNIT_DIST, 0);
    break;

  case MIXSRC_FIRST_TELEM+TELEM_RPM-1:
  case MIXSRC_FIRST_TELEM+TELEM_MAX_RPM-1: {
    getvalue_t rpm = val;
    if (rpm > 100)
      rpm = 10 * div10_and_round(rpm);
    if (rpm > 1000)
      rpm = 10 * div10_and_round(rpm);
    PLAY_NUMBER(rpm, 1+UNIT_RPMS, 0);
    break;
  }

  case MIXSRC_FIRST_TELEM+TELEM_HDG-1:
    PLAY_NUMBER(val, 1+UNIT_HDG, 0);
    break;

  default: {
    uint8_t unit = 1;
    if (idx < MIXSRC_GVAR1)
      val = calcRESXto100(val);
    if (idx >= MIXSRC_FIRST_TELEM+TELEM_ALT-1 && idx <= MIXSRC_FIRST_TELEM+TELEM_GPSALT-1)
      unit = idx - (MIXSRC_FIRST_TELEM+TELEM_ALT-1);
    else if (idx >= MIXSRC_FIRST_TELEM+TELEM_MAX_T1-1 && idx <= MIXSRC_FIRST_TELEM+TELEM_MAX_DIST-1)
      unit = 3 + idx - (MIXSRC_FIRST_TELEM+TELEM_MAX_T1-1);

    unit = pgm_read_byte_near(bchunit_ar+unit);
    PLAY_NUMBER(val, unit == UNIT_RAW ? 0 : unit+1, 0);
    break;
  }
#else
  default: {
    PLAY_NUMBER(val, 0, 0);
    break;
  }
#endif
  }
}
#endif



#define functions g_model.customFn
#define functionsContext modelFunctionsContext
void evalFunctions()
{
  MASK_FUNC_TYPE newActiveFunctions  = 0;
  MASK_CFN_TYPE  newActiveSwitches = 0;

#define PLAY_INDEX   (i+1)

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  static rotenc_t rePreviousValues[ROTARY_ENCODERS];
#endif

#if defined(OVERRIDE_CHANNEL_FUNCTION)
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    safetyCh[i] = OVERRIDE_CHANNEL_UNDEFINED;
  }
#endif

#if defined(GVARS)
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    trimGvar[i] = -1;
  }
#endif

  for (uint8_t i=0; i<NUM_CFN; i++) {
    const CustomFunctionData * cfn = &functions[i];
    int8_t swtch = CFN_SWITCH(cfn);
    if (swtch) {
      MASK_CFN_TYPE switch_mask = ((MASK_CFN_TYPE)1 << i);

      bool active = getSwitch(swtch);

      if (HAS_ENABLE_PARAM(CFN_FUNC(cfn))) {
        active &= (bool)CFN_ACTIVE(cfn);
      }

      if (active || IS_PLAY_BOTH_FUNC(CFN_FUNC(cfn))) {

        switch (CFN_FUNC(cfn)) {

#if defined(OVERRIDE_CHANNEL_FUNCTION)
        case FUNC_OVERRIDE_CHANNEL:
          safetyCh[CFN_CH_INDEX(cfn)] = CFN_PARAM(cfn);
          break;
#endif

        case FUNC_TRAINER: {
          uint8_t mask = 0x0f;
          if (CFN_CH_INDEX(cfn) > 0) {
            mask = (1<<(CFN_CH_INDEX(cfn)-1));
          }
          newActiveFunctions |= mask;
          break;
        }

        case FUNC_INSTANT_TRIM:
          newActiveFunctions |= (1 << FUNCTION_INSTANT_TRIM);
          if (!isFunctionActive(FUNCTION_INSTANT_TRIM)) {
#if defined(GUI)
            if (menuHandlers[0] == menuMainView
#if defined(FRSKY)
                || menuHandlers[0] == menuTelemetryFrsky
#endif
               )
#endif
            {
              instantTrim();
            }
          }
          break;

        case FUNC_RESET:
          switch (CFN_PARAM(cfn)) {
          case FUNC_RESET_TIMER1:
          case FUNC_RESET_TIMER2:
            timerReset(CFN_PARAM(cfn));
            break;
          case FUNC_RESET_FLIGHT:
            flightReset();
            break;
#if defined(FRSKY)
          case FUNC_RESET_TELEMETRY:
            telemetryResetValue();
            break;
#endif
#if ROTARY_ENCODERS > 0
          case FUNC_RESET_ROTENC1:
#if ROTARY_ENCODERS > 1
          case FUNC_RESET_ROTENC2:
#endif
            g_rotenc[CFN_PARAM(cfn)-FUNC_RESET_ROTENC1] = 0;
            break;
#endif
          }
          break;

#if defined(GVARS)
        case FUNC_ADJUST_GVAR:
          if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_CONSTANT) {
            SET_GVAR(CFN_GVAR_INDEX(cfn), CFN_PARAM(cfn), mixerCurrentFlightMode);
          } else if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_GVAR) {
            SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_PARAM(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_PARAM(cfn))), mixerCurrentFlightMode);
          } else if (CFN_GVAR_MODE(cfn) == FUNC_ADJUST_GVAR_INC) {
            if (!(functionsContext.activeSwitches & switch_mask)) {
              SET_GVAR(CFN_GVAR_INDEX(cfn), limit(-GVAR_MAX, GVAR_VALUE(CFN_GVAR_INDEX(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(cfn))) + (CFN_PARAM(cfn) ? +1 : -1), GVAR_MAX), mixerCurrentFlightMode);
            }
          } else if (CFN_PARAM(cfn) >= MIXSRC_TrimRud && CFN_PARAM(cfn) <= MIXSRC_TrimAil) {
            trimGvar[CFN_PARAM(cfn)-MIXSRC_TrimRud] = CFN_GVAR_INDEX(cfn);
          }
#if defined(ROTARY_ENCODERS)
          else if (CFN_PARAM(cfn) >= MIXSRC_REa && CFN_PARAM(cfn) < MIXSRC_TrimRud) {
            int8_t scroll = rePreviousValues[CFN_PARAM(cfn)-MIXSRC_REa] - (g_rotenc[CFN_PARAM(cfn)-MIXSRC_REa] / ROTARY_ENCODER_GRANULARITY);
            if (scroll) {
              SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_GVAR_INDEX(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(cfn))) + scroll, mixerCurrentFlightMode);
            }
          }
#endif
          else {
            SET_GVAR(CFN_GVAR_INDEX(cfn), calcRESXto100(getValue(CFN_PARAM(cfn))), mixerCurrentFlightMode);
          }
          break;
#endif


#if defined(VOICE)
        case FUNC_PLAY_SOUND:
        case FUNC_PLAY_TRACK:
        case FUNC_PLAY_BOTH:
        case FUNC_PLAY_VALUE: {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
          if (!functionsContext.lastFunctionTime[i] || (CFN_FUNC(cfn)==FUNC_PLAY_BOTH && active!=(bool)(functionsContext.activeSwitches&switch_mask)) || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
            functionsContext.lastFunctionTime[i] = tmr10ms;
            uint8_t param = CFN_PARAM(cfn);
            if (CFN_FUNC(cfn) == FUNC_PLAY_SOUND) {
              AUDIO_PLAY(AU_FRSKY_FIRST+param);
            } else if (CFN_FUNC(cfn) == FUNC_PLAY_VALUE) {
              PLAY_VALUE(param, PLAY_INDEX);
            } else {
#if defined(GVARS)
              if (CFN_FUNC(cfn) == FUNC_PLAY_TRACK && param > 250)
                param = GVAR_VALUE(param-251, getGVarFlightPhase(mixerCurrentFlightMode, param-251));
#endif
              PUSH_CUSTOM_PROMPT(active ? param : param+1, PLAY_INDEX);
            }
          }
          if (!active) {
            // PLAY_BOTH would change activeFnSwitches otherwise
            switch_mask = 0;
          }
          break;
        }
#else
        case FUNC_PLAY_SOUND: {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
          if (!functionsContext.lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
            functionsContext.lastFunctionTime[i] = tmr10ms;
            AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(cfn));
          }
          break;
        }
#endif

#if defined(FRSKY) && defined(VARIO)
        case FUNC_VARIO:
          newActiveFunctions |= (1 << FUNCTION_VARIO);
          break;
#endif

#if defined(HAPTIC)
        case FUNC_HAPTIC: {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
          if (!functionsContext.lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
            functionsContext.lastFunctionTime[i] = tmr10ms;
            haptic.event(AU_FRSKY_LAST+CFN_PARAM(cfn));
          }
          break;
        }
#endif

#if defined(SDCARD)
        case FUNC_LOGS:
          if (CFN_PARAM(cfn)) {
            newActiveFunctions |= (1 << FUNCTION_LOGS);
            logDelay = CFN_PARAM(cfn);
          }
          break;
#endif

        case FUNC_BACKLIGHT:
          newActiveFunctions |= (1 << FUNCTION_BACKLIGHT);
          break;

        }

        newActiveSwitches |= switch_mask;
      } else {
        functionsContext.lastFunctionTime[i] = 0;
      }
    }
  }

  functionsContext.activeSwitches   = newActiveSwitches;
  functionsContext.activeFunctions  = newActiveFunctions;

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  for (uint8_t i=0; i<ROTARY_ENCODERS; i++) {
    rePreviousValues[i] = (g_rotenc[i] / ROTARY_ENCODER_GRANULARITY);
  }
#endif
}

#undef functions
#undef functionsContext
# 1 "strhelpers.cpp"
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


#include "OpenAVRc.h"

const pm_char s_charTab[] PROGMEM = "_-.,";

char hex2zchar(uint8_t hex)
{
  return (hex >= 10 ? hex-9 : 27+hex);
}

char idx2char(int8_t idx)
{
  if (idx == 0) return ' ';
  if (idx < 0) {
    if (idx > -27) return 'a' - idx - 1;
    idx = -idx;
  }
  if (idx < 27) return 'A' + idx - 1;
  if (idx < 37) return '0' + idx - 27;
  if (idx <= 40) return pgm_read_byte_near(s_charTab+idx-37);
#if LEN_SPECIAL_CHARS > 0
  if (idx <= (LEN_STD_CHARS + LEN_SPECIAL_CHARS)) return 'z' + 5 + idx - 40;
#endif
  return ' ';
}

#if defined(SIMU)
int8_t char2idx(char c)
{
  if (c == '_') return 37;
#if LEN_SPECIAL_CHARS > 0
  if (c < 0 && c+128 <= LEN_SPECIAL_CHARS) return 41 + (c+128);
#endif
  if (c >= 'a') return 'a' - c - 1;
  if (c >= 'A') return c - 'A' + 1;
  if (c >= '0') return c - '0' + 27;
  if (c == '-') return 38;
  if (c == '.') return 39;
  if (c == ',') return 40;
  return 0;
}

void str2zchar(char *dest, const char *src, int size)
{
  memset(dest, 0, size);
  for (int c=0; c<size && src[c]; c++) {
    dest[c] = char2idx(src[c]);
  }
}

int zchar2str(char *dest, const char *src, int size)
{
  for (int c=0; c<size; c++) {
    dest[c] = idx2char(src[c]);
  }
  do {
    dest[size--] = '\0';
  } while (size >= 0 && dest[size] == ' ');
  return size+1;
}
#endif


#if defined(SDCARD)
char *strAppend(char *dest, const char *source, int len)
{
  while ((*dest++ = *source++)) {
    if (--len == 0) {
      *dest = '\0';
      return dest;
    }
  }
  return dest - 1;
}

char *strSetCursor(char *dest, int position)
{
  *dest++ = 0x1F;
  *dest++ = position;
  *dest = '\0';
  return dest;
}

char *strAppendFilename(char *dest, const char *filename, const int size)
{
  memset(dest, 0, size);
  for (int i=0; i<size; i++) {
    char c = *filename++;
    if (c == '\0' || c == '.')
      break;
    *dest++ = c;
  }
  return dest;
}

#define LEN_FILE_EXTENSION 4
char *getFileExtension(char *filename, int size)
{
  int len = min<int>(size, strlen(filename));
  for (int i=len; i>=len-LEN_FILE_EXTENSION; --i) {
    if (filename[i] == '.') {
      return &filename[i];
    }
  }
  return NULL;
}

#if defined(RTCLOCK)
#include "rtc.h"

char * strAppendDate(char * str, bool time)
{
  str[0] = '-';
  struct gtm utm;
  gettime(&utm);
  div_t qr = div(utm.tm_year+1900, 10);
  str[4] = '0' + qr.rem;
  qr = div(qr.quot, 10);
  str[3] = '0' + qr.rem;
  qr = div(qr.quot, 10);
  str[2] = '0' + qr.rem;
  str[1] = '0' + qr.quot;
  str[5] = '-';
  qr = div(utm.tm_mon+1, 10);
  str[7] = '0' + qr.rem;
  str[6] = '0' + qr.quot;
  str[8] = '-';
  qr = div(utm.tm_mday, 10);
  str[10] = '0' + qr.rem;
  str[9] = '0' + qr.quot;

  if (time) {
    str[11] = '-';
    div_t qr = div(utm.tm_hour, 10);
    str[13] = '0' + qr.rem;
    str[12] = '0' + qr.quot;
    qr = div(utm.tm_min, 10);
    str[15] = '0' + qr.rem;
    str[14] = '0' + qr.quot;
    qr = div(utm.tm_sec, 10);
    str[17] = '0' + qr.rem;
    str[16] = '0' + qr.quot;
    str[18] = '\0';
    return &str[18];
  } else {
    str[11] = '\0';
    return &str[11];
  }
}
#endif
#endif
# 1 "pulses/pulses_avr.cpp"
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



#include "../OpenAVRc.h"
#include "../protocol/misc.c"
#include "../spi.h"

uint16_t nextMixerEndTime = 0;
uint8_t s_current_protocol = 255;
uint16_t dt;
#if F_CPU > 16000000UL
static volatile uint32_t timer_counts; // Could be uint16_t for mega2560.
#else
static volatile uint16_t timer_counts; // Is uint16_t for mega2560.
#endif

FORCEINLINE bool pulsesStarted()
{
  return (s_current_protocol != 255);
}

FORCEINLINE void sendStopPulses()
{
  PROTO_Cmds(PROTOCMD_RESET);
  TRACE("  ->  RESET Proto - %s -",  Protos[s_current_protocol].ProtoName);
  SIMU_SLEEP(100);
  PROTO_Stop_Callback();
  //s_current_protocol = 255;
}

void startPulses(enum ProtoCmds Command)
{
  PROTO_Stop_Callback();
  // Reset CS pin
#if defined(SPIMODULES)
  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();
  RF_CS_NRF24L01_INACTIVE();
  RF_CS_A7105_INACTIVE();
#endif

  if (pulsesStarted()) {
    PROTO_Cmds(PROTOCMD_RESET);
    TRACE("  ->  RESET Proto - %s -",  Protos[s_current_protocol].ProtoName);
    SIMU_SLEEP(100);
  }
  if (g_model.rfProtocol > (PROTOCOL_COUNT-2)) g_model.rfProtocol = PROTOCOL_NONE+1;
  s_current_protocol = g_model.rfProtocol;
  PROTO_Cmds = *Protos[g_model.rfProtocol].Cmds;
  TRACE("  ->  INIT Proto - %s -", Protos[g_model.rfProtocol].ProtoName);
  SIMU_SLEEP(100);
  PROTO_Cmds(PROTOCMD_GETOPTIONS);
  LimitRfOptionSettings();
  PROTO_Cmds(Command);
}

// This ISR should work for xmega.
ISR(TIMER1_COMPA_vect) // ISR for Protocol Callback, PPMSIM and PPM16 (Last 8 channels).
{
#if F_CPU > 16000000UL
  if (! timer_counts)
#endif
  {
    uint16_t half_us = timer_callback(); // Function pointer e.g. skyartec_cb().

    if(! half_us) {
      PROTO_Cmds(PROTOCMD_RESET);
      return;
    }

    timer_counts = HALF_MICRO_SEC_COUNTS(half_us);
  }

#if F_CPU > 16000000UL
  if (timer_counts > 65535) {
    OCR1A += 32000;
    timer_counts -= 32000; // 16ms @ 16MHz counter clock.
  } else
#endif
  {
    OCR1A += timer_counts;
    timer_counts = 0;
  }

  if (dt > g_tmr1Latency_max) g_tmr1Latency_max = dt;
  if (dt < g_tmr1Latency_min) g_tmr1Latency_min = dt;

}



void setupPulsesPPM(enum ppmtype proto)
{
  // Total frame length is a fixed 22.5msec (more than 9 channels is non-standard and requires this to be extended.)
  // Each channel's pulse is 0.7 to 1.7ms long, with a 0.3ms stop tail, making each complete cycle 1 to 2ms.

  // The pulse ISR is 2MHz that's why everything is multiplied by 2

  int16_t PPM_range = g_model.extendedLimits ? 640*2 : 512*2;   //range of 0.7..1.7msec

  uint16_t q = (g_model.PPMDELAY*50+300)*2; // Channel sync pulse.

  int32_t rest = 22500u*2 - q;

  // PPM 16 uses a fixed frame length of 22.5msec.
  if(proto == PPM || proto == PPMSIM) rest += (int32_t(g_model.PPMFRAMELENGTH))*1000;

  // PPM and PPM16 (Channels 1-8) use first half of array. PPMSIM and PPM16 (Channels 9-16) use last half.
  uint16_t *ptr = (proto == PPM || proto == PPM16FIRST) ? &pulses2MHz.pword[0] : &pulses2MHz.pword[PULSES_WORD_SIZE/2];

  uint8_t p;
  // Fix PPM16 to 16 channels (8+8), No modification by GUI.

  if(proto == PPM || proto == PPMSIM) p = 4 + (g_model.PPMNCH * 2); // Channels *2
  else if(proto == PPM16FIRST) p = 8;
  else p = 16; // PPM16 Channels 9-16.
#if defined(X_ANY)
  Xany_scheduleTx(0);
  Xany_scheduleTx(1);
#endif
  for (uint8_t i=(proto == PPM16LAST) ? 8 : 0; i<p; i++) { // Just do channels 1-8 unless PPM16 (9-16).
    int16_t v = limit((int16_t)-PPM_range, channelOutputs[i], (int16_t)PPM_range) + 2*PPM_CH_CENTER(i);
    rest -= v;
    *ptr++ = q;
    *ptr++ = v - q; // Total pulse width includes channel sync pulse.
  }

  *ptr++ = q;
  if (rest > 65535) rest = 65535; // Prevents overflows.
  if (rest < 9000)  rest = 9000;

  *ptr++ = rest - (PULSES_SETUP_TIME_US *2);
  *ptr = 0; // End array with (uint16_t) 0;
}


ISR(TIMER1_COMPB_vect) // Timer 1 compare "B" vector. Used for PPM commutation and maybe more ...
{
  ocr1b_function_ptr();
}


# 1 "switches.cpp"
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


#include "OpenAVRc.h"

#define CS_LAST_VALUE_INIT -32768


int16_t lsLastValue[NUM_LOGICAL_SWITCH];
#define LS_LAST_VALUE(fm, idx) lsLastValue[idx]

volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_used = 0;
volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_value = 0;

PACK(typedef struct {
  uint8_t state;
  uint8_t last;
}) ls_sticky_struct;

bool getLogicalSwitch(uint8_t idx)
{
  LogicalSwitchData * ls = lswAddress(idx);
  bool result;

  uint8_t s = (ls->andsw);

  if (ls->func == LS_FUNC_NONE || (s && !getSwitch(s))) {
    if (ls->func != LS_FUNC_STICKY) {
      // AND switch must not affect STICKY and EDGE processing
      LS_LAST_VALUE(mixerCurrentFlightMode, idx) = CS_LAST_VALUE_INIT;
    }
    result = false;
  } else if ((s=lswFamily(ls->func)) == LS_FAMILY_BOOL) {
    bool res1 = getSwitch(ls->v1);
    bool res2 = getSwitch(ls->v2);
    switch (ls->func) {
    case LS_FUNC_AND:
      result = (res1 && res2);
      break;
    case LS_FUNC_OR:
      result = (res1 || res2);
      break;
    // case LS_FUNC_XOR:
    default:
      result = (res1 ^ res2);
      break;
    }
  } else if (s == LS_FAMILY_TIMER) {
    result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) <= 0);
  } else if (s == LS_FAMILY_STICKY) {
    result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) & (1<<0));
  } else {
    getvalue_t x = getValue(ls->v1);
    getvalue_t y;
    if (s == LS_FAMILY_COMP) {
      y = getValue(ls->v2);

      switch (ls->func) {
      case LS_FUNC_EQUAL:
        result = (x==y);
        break;
      case LS_FUNC_GREATER:
        result = (x>y);
        break;
      default:
        result = (x<y);
        break;
      }
    } else {
      mixsrc_t v1 = ls->v1;
#if defined(FRSKY)
      // Telemetry
      if (v1 >= MIXSRC_FIRST_TELEM) {
        if ((!TELEMETRY_STREAMING() && v1 >= MIXSRC_FIRST_TELEM+TELEM_FIRST_STREAMED_VALUE-1) || IS_FAI_FORBIDDEN(v1-1)) {
          result = false;
          goto DurationAndDelayProcessing;
        }

        y = convertLswTelemValue(ls);

        // Fill the telemetry bars threshold array
        if (s == LS_FAMILY_OFS) {
          uint8_t idx = v1-MIXSRC_FIRST_TELEM+1-TELEM_ALT;
          if (idx < THLD_MAX) {
            FILL_THRESHOLD(idx, ls->v2);
          }
        }

      } else if (v1 >= MIXSRC_GVAR1) {
        y = ls->v2;
      } else {
        y = calc100toRESX(ls->v2);
      }
#else
      if (v1 >= MIXSRC_FIRST_TELEM) {
        //y = (int16_t)3 * (128+ls->v2); // it's a Timer
        y = convertLswTelemValue(ls); // Add by Mentero -> under test (todo)
      } else if (v1 >= MIXSRC_GVAR1) {
        y = ls->v2; // it's a GVAR
      } else {
        y = calc100toRESX(ls->v2);
      }
#endif

      switch (ls->func) {

      case LS_FUNC_VEQUAL:// v==offset
        result = (x==y);
        break;

      case LS_FUNC_VALMOSTEQUAL:
#if defined(GVARS)
        if (v1 >= MIXSRC_GVAR1 && v1 <= MIXSRC_LAST_GVAR)
          result = (x==y);
        else
#endif
          result = (abs(x-y) < (1024 / STICK_TOLERANCE));
        break;
      case LS_FUNC_VPOS:
        result = (x>y);
        break;
      case LS_FUNC_VNEG:
        result = (x<y);
        break;
      case LS_FUNC_APOS:
        result = (abs(x)>y);
        break;
      case LS_FUNC_ANEG:
        result = (abs(x)<y);
        break;
      default: {
        if (LS_LAST_VALUE(mixerCurrentFlightMode, idx) == CS_LAST_VALUE_INIT) {
          LS_LAST_VALUE(mixerCurrentFlightMode, idx) = x;
        }
        int16_t diff = x - LS_LAST_VALUE(mixerCurrentFlightMode, idx);
        bool update = false;
        if (ls->func == LS_FUNC_DIFFEGREATER) {
          if (y >= 0) {
            result = (diff >= y);
            if (diff < 0)
              update = true;
          } else {
            result = (diff <= y);
            if (diff > 0)
              update = true;
          }
        } else {
          result = (abs(diff) >= y);
        }
        if (result || update) {
          LS_LAST_VALUE(mixerCurrentFlightMode, idx) = x;
        }
        break;
      }
      }
    }
  }

#if defined(FRSKY)
DurationAndDelayProcessing:
#endif


  return result;
}

bool getSwitch(swsrc_t swtch)
{
  bool result;

  if (swtch == SWSRC_NONE)
    return true;

  uint8_t cs_idx = abs(swtch);

  if (cs_idx == SWSRC_ONE) {
    result = !s_mixer_first_run_done;
  } else if (cs_idx == SWSRC_ON) {
    result = true;
  } else if (cs_idx <= SWSRC_LAST_SWITCH) {
    result = switchState((EnumKeys)(SW_BASE+cs_idx-SWSRC_FIRST_SWITCH));
  } else if (cs_idx <= SWSRC_LAST_TRIM) {
    uint8_t idx = cs_idx - SWSRC_FIRST_TRIM;
    idx = (CONVERT_MODE(idx/2) << 1) + (idx & 1);
    result = trimDown(idx);
  }
#if ROTARY_ENCODERS > 0
  else if (cs_idx == SWSRC_REA) {
    result = REA_DOWN();
  }
#endif
#if ROTARY_ENCODERS > 1
  else if (cs_idx == SWSRC_REB) {
    result = REB_DOWN();
  }
#endif
  else if (cs_idx == SWSRC_REN) {
    result = !(REA_DOWN() || REB_DOWN());
  }
  else if (cs_idx == SWSRC_XD0) {
    result = (calibratedStick[NUM_STICKS+EXTRA_3POS-1] > 0x200); // > 512
  }
  else if (cs_idx == SWSRC_XD1) {
    result = IS_IN_RANGE(calibratedStick[NUM_STICKS+EXTRA_3POS-1],-(0x200),0x200); // -512 < X < 512
  }
  else if (cs_idx == SWSRC_XD2) {
    result = (calibratedStick[NUM_STICKS+EXTRA_3POS-1] < -(0x200)); // < -512
  }
  else {
    cs_idx -= SWSRC_FIRST_LOGICAL_SWITCH;
    GETSWITCH_RECURSIVE_TYPE mask = ((GETSWITCH_RECURSIVE_TYPE)1 << cs_idx);
    if (s_last_switch_used & mask) {
      result = (s_last_switch_value & mask);
    } else {
      s_last_switch_used |= mask;
      result = getLogicalSwitch(cs_idx);
      if (result) {
        s_last_switch_value |= mask;
      } else {
        s_last_switch_value &= ~mask;
      }
    }
  }

  return swtch > 0 ? result : !result;
}

swarnstate_t switches_states = 0;

swsrc_t getMovedSwitch()
{
  static tmr10ms_t s_move_last_time = 0;
  swsrc_t result = 0;

  // return delivers 1 to 3 for ID1 to ID3
  // 4..8 for all other switches if changed to true
  // -4..-8 for all other switches if changed to false
  // 9 for Trainer switch if changed to true; Change to false is ignored
  swarnstate_t mask = 0x80;
  for (uint8_t i=NUM_PSWITCH; i>1; i--) {
    bool prev;
    prev = (switches_states & mask);
    // don't use getSwitch here to always get the proper value, even getSwitch manipulates
    bool next = switchState((EnumKeys)(SW_BASE+i-1));
    if (prev != next) {
      if (((i<NUM_PSWITCH) && (i>3)) || next==true)
        result = next ? i : -i;
      if (i<=3 && result==0) result = 1;
      switches_states ^= mask;
    }
    mask >>= 1;
  }

  if ((tmr10ms_t)(get_tmr10ms() - s_move_last_time) > 10)
    result = 0;

  s_move_last_time = get_tmr10ms();
  return result;
}

#if defined(GUI)
void checkSwitches()
{
  swarnstate_t last_bad_switches = 0xff;
  swarnstate_t states = g_model.switchWarningState;

  while (1) {

#ifdef GETADC_COUNT
    for (uint8_t i=0; i<GETADC_COUNT; i++) {
      getADC();
    }
#undef GETADC_COUNT
#endif

    getMovedSwitch();

    bool warn = false;
    for (int i=0; i<NUM_SWITCHES-1; i++) {
      if (!(g_model.switchWarningEnable & (1<<i))) {
        if (i == 0) {
          if ((states & 0x03) != (switches_states & 0x03)) {
            warn = true;
          }
        } else if ((states & (1<<(i+1))) != (switches_states & (1<<(i+1)))) {
          warn = true;
        }
      }
    }

    if (!warn) {
      return;
    }

    // first - display warning
    if (last_bad_switches != switches_states) {
      MESSAGE(STR_SWITCHWARN, NULL, STR_PRESSANYKEYTOSKIP, last_bad_switches == 0xff ? AU_SWITCH_ALERT : AU_NONE);
      uint8_t x = 2;
      for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
        uint8_t attr;
        if (i == 0)
          attr = ((states & 0x03) != (switches_states & 0x03)) ? INVERS : 0;
        else
          attr = (states & (1 << (i+1))) == (switches_states & (1 << (i+1))) ? 0 : INVERS;
        if (!(g_model.switchWarningEnable & (1<<i)))
          lcdPutsSwitches(x, 5*FH, (i>0?(i+3):(states&0x3)+1), attr);
        x += 3*FW+FW/2;
      }
      lcdRefresh();
      last_bad_switches = switches_states;
    }


    if (keyDown()) {
      return;
    }

    if (!pwrCheck) {
      return;
    }

    checkBacklight();

    MYWDT_RESET();
  }
  }
#endif // GUI

  void logicalSwitchesTimerTick()
  {
    for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
      LogicalSwitchData * ls = lswAddress(i);
      if (ls->func == LS_FUNC_TIMER) {
        int16_t *lastValue = &LS_LAST_VALUE(fm, i);
        if (*lastValue == 0 || *lastValue == CS_LAST_VALUE_INIT) {
          *lastValue = -lswTimerValue(ls->v1);
        } else if (*lastValue < 0) {
          if (++(*lastValue) == 0)
            *lastValue = lswTimerValue(ls->v2);
        } else { // if (*lastValue > 0)
          *lastValue -= 1;
        }
      } else if (ls->func == LS_FUNC_STICKY) {
        ls_sticky_struct & lastValue = (ls_sticky_struct &)LS_LAST_VALUE(fm, i);
        bool before = lastValue.last & 0x01;
        if (lastValue.state) {
          bool now = getSwitch(ls->v2);
          if (now != before) {
            lastValue.last ^= 1;
            if (!before) {
              lastValue.state = 0;
            }
          }
        } else {
          bool now = getSwitch(ls->v1);
          if (before != now) {
            lastValue.last ^= 1;
            if (!before) {
              lastValue.state = 1;
            }
          }
        }
      }
    }
  }

  LogicalSwitchData * lswAddress(uint8_t idx)
  {
    return &g_model.logicalSw[idx];
  }

  uint8_t lswFamily(uint8_t func)
  {
    if (func <= LS_FUNC_ANEG)
      return LS_FAMILY_OFS;
    else if (func <= LS_FUNC_XOR)
      return LS_FAMILY_BOOL;
    else if (func <= LS_FUNC_LESS)
      return LS_FAMILY_COMP;
    else if (func <= LS_FUNC_ADIFFEGREATER)
      return LS_FAMILY_DIFF;
    else
      return LS_FAMILY_TIMER+func-LS_FUNC_TIMER;
  }

  int16_t lswTimerValue(delayval_t val)
  {
    return (val < -109 ? 129+val : (val < 7 ? (113+val)*5 : (53+val)*10));
  }

  void logicalSwitchesReset()
  {
    s_last_switch_value = 0;

    for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
      LS_LAST_VALUE(fm, i) = CS_LAST_VALUE_INIT;
    }
  }

  getvalue_t convertLswTelemValue(LogicalSwitchData * ls)
  {
    getvalue_t val;
    if (lswFamily(ls->func)==LS_FAMILY_OFS)
      val = convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128+ls->v2);
    else
      val = convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128+ls->v2) - convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128);
    return val;
  }
# 1 "curves.cpp"
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


#include "OpenAVRc.h"

int8_t *curveAddress(uint8_t idx)
{
  return &g_model.points[idx==0 ? 0 : 5*idx+g_model.curves[idx-1]];
}

CurveInfo curveInfo(uint8_t idx)
{
  CurveInfo result;
  result.crv = curveAddress(idx);
  int8_t *next = curveAddress(idx+1);
  uint8_t size = next - result.crv;
  if ((size & 1) == 0) {
    result.points = (size / 2) + 1;
    result.custom = true;
  } else {
    result.points = size;
    result.custom = false;
  }
  return result;
}


int16_t intpol(int16_t x, uint8_t idx) // -100, -75, -50, -25, 0 ,25 ,50, 75, 100
{
  CurveInfo crv = curveInfo(idx);
  int8_t *points = crv.crv;
  uint8_t count = crv.points;
  bool custom = crv.custom;
  int16_t erg = 0;

  x += RESXu;

  if (x <= 0) {
    erg = (int16_t)points[0] * (RESX/4);
  } else if (x >= (RESX*2)) {
    erg = (int16_t)points[count-1] * (RESX/4);
  } else {
    uint16_t a=0, b=0;
    uint8_t i;
    if (custom) {
      for (i=0; i<count-1; i++) {
        a = b;
        b = (i==count-2 ? 2*RESX : RESX + calc100toRESX(points[count+i]));
        if ((uint16_t)x<=b) break;
      }
    } else {
      uint16_t d = (RESX * 2) / (count-1);
      i = (uint16_t)x / d;
      a = i * d;
      b = a + d;
    }
    erg = (int16_t)points[i]*(RESX/4) + ((int32_t)(x-a) * (points[i+1]-points[i]) * (RESX/4)) / ((b-a));
  }

  return erg / 25; // 100*D5/RESX;
}

#if   defined(CURVES)
int16_t applyCurve(int16_t x, int8_t idx)
{
  /* already tried to have only one return at the end */
  switch(idx) {
  case CURVE_NONE:
    return x;
  case CURVE_X_GT0:
    if (x < 0) x = 0; //x|x>0
    return x;
  case CURVE_X_LT0:
    if (x > 0) x = 0; //x|x<0
    return x;
  case CURVE_ABS_X: // x|abs(x)
    return abs(x);
  case CURVE_F_GT0: //f|f>0
    return x > 0 ? RESX : 0;
  case CURVE_F_LT0: //f|f<0
    return x < 0 ? -RESX : 0;
  case CURVE_ABS_F: //f|abs(f)
    return x > 0 ? RESX : -RESX;
  }
  if (idx < 0) {
    x = -x;
    idx = -idx + CURVE_BASE - 1;
  }
  return applyCustomCurve(x, idx - CURVE_BASE);
}
#endif

// #define EXTENDED_EXPO
// increases range of expo curve but costs about 82 bytes flash

// expo-funktion:
// ---------------
// kmplot
// f(x,k)=exp(ln(x)*k/10) ;P[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
// f(x,k)=x*x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=1+(x-1)*(x-1)*(x-1)*k/10 + (x-1)*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// don't know what this above should be, just confusing in my opinion,

// here is the real explanation
// actually the real formula is
/*
 f(x) = exp( ln(x) * 10^k)
 if it is 10^k or e^k or 2^k etc. just defines the max distortion of the expo curve; I think 10 is useful
 this gives values from 0 to 1 for x and output; k must be between -1 and +1
 we do not like to calculate with floating point. Therefore we rescale for x from 0 to 1024 and for k from -100 to +100
 f(x) = 1024 * ( e^( ln(x/1024) * 10^(k/100) ) )
 This would be really hard to be calculated by such a microcontroller
 Therefore Thomas Husterer compared a few usual function something like x^3, x^4*something, which look similar
 Actually the formula
 f(x) = k*x^3+x*(1-k)
 gives a similar form and should have even advantages compared to a original exp curve.
 This function again expect x from 0 to 1 and k only from 0 to 1
 Therefore rescaling is needed like before:
 f(x) = 1024* ((k/100)*(x/1024)^3 + (x/1024)*(100-k)/100)
 some mathematical tricks
 f(x) = (k*x*x*x/(1024*1024) + x*(100-k)) / 100
 for better rounding results we add the 50
 f(x) = (k*x*x*x/(1024*1024) + x*(100-k) + 50) / 100

 because we now understand the formula, we can optimize it further
 --> calc100to256(k) --> eliminates /100 by replacing with /256 which is just a simple shift right 8
 k is now between 0 and 256
 f(x) = (k*x*x*x/(1024*1024) + x*(256-k) + 128) / 256
 */

// input parameters;
//  x 0 to 1024;
//  k 0 to 100;
// output between 0 and 1024
uint16_t expou(uint16_t x, uint16_t k)
{
#if defined(EXTENDED_EXPO)
  bool extended;
  if (k>80) {
    extended=true;
  } else {
    k += (k>>2);  // use bigger values before extend, because the effect is anyway very very low
    extended=false;
  }
#endif

  k = calc100to256(k);

  uint32_t value = (uint32_t) x*x;
  value *= (uint32_t)k;
  value >>= 8;
  value *= (uint32_t)x;

#if defined(EXTENDED_EXPO)
  if (extended) {  // for higher values do more multiplications to get a stronger expo curve
    value >>= 16;
    value *= (uint32_t)x;
    value >>= 4;
    value *= (uint32_t)x;
  }
#endif

  value >>= 12;
  value += (uint32_t)(256-k)*x+128;

  return value>>8;
}

int16_t expo(int16_t x, int16_t k)
{
  if (k == 0) return x;
  int16_t y;
  bool neg = (x < 0);

  if (neg) x = -x;
  if (k<0) {
    y = RESXu-expou(RESXu-x, -k);
  } else {
    y = expou(x, k);
  }
  return neg? -y : y;
}
# 1 "mixer.cpp"
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


#include "OpenAVRc.h"
#include "timers.h"

int16_t  rawAnas[NUM_INPUTS] = {0};
int16_t  anas [NUM_INPUTS] = {0};
int16_t  trims[NUM_STICKS] = {0};
int32_t  chans[NUM_CHNOUT] = {0};
BeepANACenter bpanaCenter = 0;

int32_t act   [MAX_MIXERS] = {0};
SwOn    swOn  [MAX_MIXERS];   // TODO better name later...

uint8_t mixWarning;


int16_t calibratedStick[NUM_STICKS+NUM_POTS];
int16_t channelOutputs[NUM_CHNOUT] = {0};
int16_t ex_chans[NUM_CHNOUT] = {0}; // Outputs (before LIMITS) of the last perMain;

#if defined(HELI)
int16_t cyc_anas[3] = {0};
#endif

void applyExpos(int16_t *anas, uint8_t mode APPLY_EXPOS_EXTRA_PARAMS)
{
  int16_t anas2[NUM_INPUTS]; // values before expo, to ensure same expo base when multiple expo lines are used
  memcpy(anas2, anas, sizeof(anas2));

  int8_t cur_chn = -1;

  for (uint8_t i=0; i<MAX_EXPOS; i++) {
#if defined(BOLD_FONT)
    if (mode==e_perout_mode_normal) swOn[i].activeExpo = false;
#endif
    ExpoData * ed = expoAddress(i);
    if (!EXPO_VALID(ed)) break; // end of list
    if (ed->chn == cur_chn)
      continue;
    if (ed->flightModes & (1<<mixerCurrentFlightMode))
      continue;
    if (getSwitch(ed->swtch)) {
      int16_t v = anas2[ed->chn];
      if (EXPO_MODE_ENABLE(ed, v)) {
#if defined(BOLD_FONT)
        if (mode==e_perout_mode_normal) swOn[i].activeExpo = true;
#endif
        cur_chn = ed->chn;

        //========== CURVE=================
        int8_t curveParam = ed->curveParam;
        if (curveParam) {
          if (ed->curveMode == MODE_CURVE)
            v = applyCurve(v, curveParam);
          else
            v = expo(v, GET_GVAR(curveParam, -100, 100, mixerCurrentFlightMode));
        }

        //========== WEIGHT ===============
        int16_t weight = GET_GVAR(ed->weight, MIN_EXPO_WEIGHT, 100, mixerCurrentFlightMode);
        weight = calc100to256(weight);
        v = ((int32_t)v * weight) >> 8;

        anas[cur_chn] = v;
      }
    }
  }
}

// #define PREVENT_ARITHMETIC_OVERFLOW
// because of optimizations the reserves before overruns occurs is only the half
// this defines enables some checks the greatly improves this situation
// It should nearly prevent all overruns (is still a chance for it, but quite low)
// negative side is code cost 96 bytes flash

// we do it now half way, only in applyLimits, which costs currently 50bytes
// according opinion poll this topic is currently not very important
// the change below improves already the situation
// the check inside mixer would slow down mix a little bit and costs additionally flash
// also the check inside mixer still is not bulletproof, there may be still situations a overflow could occur
// a bulletproof implementation would take about additional 100bytes flash
// therefore with go with this compromize, interested people could activate this define

// @@@2 open.20.fsguruh ;
// channel = channelnumber -1;
// value = outputvalue with 100 mulitplied usual range -102400 to 102400; output -1024 to 1024
// changed rescaling from *100 to *256 to optimize performance
// rescaled from -262144 to 262144
int16_t applyLimits(uint8_t channel, int32_t value)
{
  LimitData * lim = limitAddress(channel);

  int16_t ofs   = LIMIT_OFS_RESX(lim);
  int16_t lim_p = LIMIT_MAX_RESX(lim);
  int16_t lim_n = LIMIT_MIN_RESX(lim);

  if (ofs > lim_p) ofs = lim_p;
  else if (ofs < lim_n) ofs = lim_n;

  // because the rescaling optimization would reduce the calculation reserve we activate this for all builds
  // it increases the calculation reserve from factor 20,25x to 32x, which it slightly better as original
  // without it we would only have 16x which is slightly worse as original, we should not do this

  // thanks to gbirkus, he motivated this change, which greatly reduces overruns
  // unfortunately the constants and 32bit compares generates about 50 bytes codes; didn't find a way to get it down.
  value = limit(int32_t(-RESXl*256), value, int32_t(RESXl*256));  // saves 2 bytes compared to other solutions up to now

#if defined(PPM_LIMITS_SYMETRICAL)
  if (value) {
    int16_t tmp;
    if (lim->symetrical)
      tmp = (value > 0) ? (lim_p) : (-lim_n);
    else
      tmp = (value > 0) ? (lim_p - ofs) : (-lim_n + ofs);
    value = (int32_t) value * tmp;   //  div by 1024*256 -> output = -1024..1024
#else
  if (value) {
    int16_t tmp = (value > 0) ? (lim_p - ofs) : (-lim_n + ofs);
    value = (int32_t) value * tmp;   //  div by 1024*256 -> output = -1024..1024
#endif

#ifdef CORRECT_NEGATIVE_SHIFTS
    int8_t sign = (value<0?1:0);
    value -= sign;
    tmp = value>>16;   // that's quite tricky: the shiftright 16 operation is assmbled just with addressmove; just forget the two least significant bytes;
    tmp >>= 2;   // now one simple shift right for two bytes does the rest
    tmp += sign;
#else
    tmp = value>>16;   // that's quite tricky: the shiftright 16 operation is assmbled just with addressmove; just forget the two least significant bytes;
    tmp >>= 2;   // now one simple shift right for two bytes does the rest
#endif

    ofs += tmp;  // ofs can to added directly because already recalculated,
  }

  if (ofs > lim_p) ofs = lim_p;
  else if (ofs < lim_n) ofs = lim_n;

  if (lim->revert) ofs = -ofs; // finally do the reverse.

#if defined(OVERRIDE_CHANNEL_FUNCTION)
  if (safetyCh[channel] != OVERRIDE_CHANNEL_UNDEFINED) {
    // safety channel available for channel check
    ofs = calc100toRESX(safetyCh[channel]);
  }
#endif

  return ofs;
}

getvalue_t getValue(mixsrc_t i)
{
  if (i==MIXSRC_NONE) return 0;
  else if IS_IN_RANGE(i, MIXSRC_FIRST_STICK, MIXSRC_LAST_POT) return calibratedStick[i-MIXSRC_Rud];

  else if (i<MIXSRC_LAST_ROTARY_ENCODER+1) return getRotaryEncoder(i-MIXSRC_REa);

  else if (i==MIXSRC_MAX) return 1024;

  else if (i<MIXSRC_CYC3+1)
#if defined(HELI)
    return cyc_anas[i-MIXSRC_CYC1];
#else
    return 0;
#endif

  else if (i<MIXSRC_TrimAil+1) return calc1000toRESX((int16_t)8 * getTrimValue(mixerCurrentFlightMode, i-MIXSRC_TrimRud));
  else if (i==MIXSRC_3POS) return (getSwitch(SW_ID0-SW_BASE+1) ? -1024 : (getSwitch(SW_ID1-SW_BASE+1) ? 0 : 1024));
  // don't use switchState directly to give getSwitch possibility to hack values if needed for switch warning
  else if (i<MIXSRC_SW1) return getSwitch(SWSRC_THR+i-MIXSRC_THR) ? 1024 : -1024;
  else if (i<MIXSRC_LAST_LOGICAL_SWITCH+1) return getSwitch(SWSRC_FIRST_LOGICAL_SWITCH+i-MIXSRC_FIRST_LOGICAL_SWITCH) ? 1024 : -1024;
  else if (i<MIXSRC_LAST_TRAINER+1) {
    int16_t x = ppmInput[i-MIXSRC_FIRST_TRAINER];
    if (i<MIXSRC_FIRST_TRAINER+NUM_CAL_PPM) {
      x-= g_eeGeneral.trainer.calib[i-MIXSRC_FIRST_TRAINER];
    }
    return x*2;
  } else if (i<MIXSRC_LAST_CH+1) return ex_chans[i-MIXSRC_CH1];

#if defined(GVARS)
  else if (i<MIXSRC_LAST_GVAR+1) return GVAR_VALUE(i-MIXSRC_GVAR1, getGVarFlightPhase(mixerCurrentFlightMode, i-MIXSRC_GVAR1));
#endif

  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_TX_VOLTAGE) return g_vbat10mV/10;
  else if (i<MIXSRC_FIRST_TELEM+TELEM_TIMER2) return timersStates[i-MIXSRC_FIRST_TELEM+1-TELEM_TIMER1].val;

#if defined(FRSKY)
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_RSSI_TX) return telemetryData.rssi[1].value;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_RSSI_RX) return telemetryData.rssi[0].value;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_A1) return telemetryData.analog[TELEM_ANA_A1].value;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_A2) return telemetryData.analog[TELEM_ANA_A2].value;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_ALT) return TELEMETRY_RELATIVE_BARO_ALT_BP;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_RPM) return telemetryData.value.rpm;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_FUEL) return telemetryData.value.fuelLevel;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_T1) return telemetryData.value.temperature1;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_T2) return telemetryData.value.temperature2;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_SPEED) return TELEMETRY_GPS_SPEED_BP;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_DIST) return telemetryData.value.gpsDistance;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_GPSALT) return TELEMETRY_RELATIVE_GPS_ALT_BP;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_CELL) return (int16_t)TELEMETRY_MIN_CELL_VOLTAGE;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_CELLS_SUM) return (int16_t)telemetryData.value.cellsSum;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_VFAS) return (int16_t)telemetryData.value.vfas;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_CURRENT) return (int16_t)telemetryData.value.current;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_CONSUMPTION) return telemetryData.value.currentConsumption;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_POWER) return telemetryData.value.power;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_ACCx) return telemetryData.value.accelX;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_ACCy) return telemetryData.value.accelY;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_ACCz) return telemetryData.value.accelZ;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_HDG) return telemetryData.value.gpsCourse_bp;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_VSPEED) return telemetryData.value.varioSpeed;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_ASPEED) return telemetryData.value.airSpeed;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_DTE) return telemetryData.value.dTE;
  else if (i<=MIXSRC_FIRST_TELEM-1+TELEM_MIN_A1) return telemetryData.analog[TELEM_ANA_A1].min;
  else if (i==MIXSRC_FIRST_TELEM-1+TELEM_MIN_A2) return telemetryData.analog[TELEM_ANA_A2].min;
  else if (i<=MIXSRC_FIRST_TELEM-1+TELEM_CSW_MAX) return *(((int16_t*)(&telemetryData.value.minAltitude))+i-(MIXSRC_FIRST_TELEM-1+TELEM_MIN_ALT));
#endif
  else return 0;
}

void evalInputs(uint8_t mode)
{
  BeepANACenter anaCenter = 0;

#if defined(HELI)
  uint16_t d = 0;
  if (g_model.swashR.value) {
    uint32_t v = (int32_t(calibratedStick[ELE_STICK])*calibratedStick[ELE_STICK] + int32_t(calibratedStick[AIL_STICK])*calibratedStick[AIL_STICK]);
    uint32_t q = calc100toRESX(g_model.swashR.value);
    q *= q;
    if (v > q) {
      d = isqrt32(v);
    }
  }
#endif

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS; i++) {

    // normalization [0..2048] -> [-1024..1024]
    uint8_t ch = (i < NUM_STICKS ? CONVERT_MODE(i) : i);

#if defined(ROTARY_ENCODERS)
    int16_t v = ((i < NUM_STICKS+NUM_POTS) ? anaIn(i) : getRotaryEncoder(i-(NUM_STICKS+NUM_POTS)));
#else
    int16_t v = anaIn(i);
#endif

    if (i < NUM_STICKS+NUM_POTS) {
      if (IS_POT_MULTIPOS(i)) {
        v -= RESX;
      } else {
        CalibData * calib = &g_eeGeneral.calib[i];
        v -= calib->mid;
        v = v * (int32_t)RESX / (max((int16_t)100, (v>0 ? calib->spanPos : calib->spanNeg)));
      }
    }

    if (v < -RESX) v = -RESX;
    else if (v >  RESX) v =  RESX;

    if (g_model.throttleReversed && ch==THR_STICK) {
      v = -v;
    }

    BeepANACenter mask = (BeepANACenter)1 << ch;

    if (i < NUM_STICKS+NUM_POTS) {

      calibratedStick[ch] = v; // for show in expo

      // filtering for center beep
      uint8_t tmp = (uint16_t)abs(v) / 16;
      if (tmp <= 1) anaCenter |= (tmp==0 ? mask : (bpanaCenter & mask));
    } else {
      // rotary encoders
      if (v == 0) anaCenter |= mask;
    }

    if (ch < NUM_STICKS) { //only do this for sticks

      if (mode <= e_perout_mode_inactive_flight_mode && isFunctionActive(FUNCTION_TRAINER+ch) && IS_TRAINER_INPUT_VALID()) {
        // trainer mode
        TrainerMix* td = &g_eeGeneral.trainer.mix[ch];
        if (td->mode) {
          uint8_t chStud = td->srcChn;
          int32_t vStud  = (ppmInput[chStud]- g_eeGeneral.trainer.calib[chStud]);
          vStud *= td->studWeight;
          vStud /= 50;
          switch (td->mode) {
          case 1:
            // add-mode
            v = limit<int16_t>(-RESX, v+vStud, RESX);
            break;
          case 2:
            // subst-mode
            v = vStud;
            break;
          }
        }
      }

#if defined(HELI)
      if (d && (ch==ELE_STICK || ch==AIL_STICK)) {
        v = (int32_t(v) * calc100toRESX(g_model.swashR.value)) / int32_t(d);
      }
#endif
      rawAnas[ch] = v;
      anas[ch] = v; // set values for mixer
    }
  }

  /* EXPOs */
  applyExpos(anas, mode);

  /* TRIMs */
  evalTrims(); // when no virtual inputs, the trims need the anas array calculated above (when throttle trim enabled)

  if (mode == e_perout_mode_normal) {
    anaCenter &= g_model.beepANACenter;
    if (((bpanaCenter ^ anaCenter) & anaCenter)) AUDIO_POT_MIDDLE();
    bpanaCenter = anaCenter;
  }
}

uint8_t mixerCurrentFlightMode;

void evalFlightModeMixes(uint8_t mode, uint8_t tick10ms)
{
  evalInputs(mode);

#if defined(HELI)
  int16_t heliEleValue = anas[ELE_STICK];
  int16_t heliAilValue = anas[AIL_STICK];
  if (g_model.swashR.value) {
    uint32_t v = ((int32_t)heliEleValue*heliEleValue + (int32_t)heliAilValue*heliAilValue);
    uint32_t q = calc100toRESX(g_model.swashR.value);
    q *= q;
    if (v>q) {
      uint16_t d = isqrt32(v);
      int16_t tmp = calc100toRESX(g_model.swashR.value);
      heliEleValue = (int32_t) heliEleValue*tmp/d;
      heliAilValue = (int32_t) heliAilValue*tmp/d;
    }
  }

#define REZ_SWASH_X(x)  ((x) - (x)/8 - (x)/128 - (x)/512)   //  1024*sin(60) ~= 886
#define REZ_SWASH_Y(x)  ((x))   //  1024 => 1024

  if (g_model.swashR.type) {
    getvalue_t vp = heliEleValue + trims[ELE_STICK];
    getvalue_t vr = heliAilValue + trims[AIL_STICK];
    getvalue_t vc = 0;
    if (g_model.swashR.collectiveSource)
      vc = getValue(g_model.swashR.collectiveSource);

    if (g_model.swashR.invertELE) vp = -vp;
    if (g_model.swashR.invertAIL) vr = -vr;
    if (g_model.swashR.invertCOL) vc = -vc;

    switch (g_model.swashR.type) {
    case SWASH_TYPE_120:
      vp = REZ_SWASH_Y(vp);
      vr = REZ_SWASH_X(vr);
      cyc_anas[0] = vc - vp;
      cyc_anas[1] = vc + vp/2 + vr;
      cyc_anas[2] = vc + vp/2 - vr;
      break;
    case SWASH_TYPE_120X:
      vp = REZ_SWASH_X(vp);
      vr = REZ_SWASH_Y(vr);
      cyc_anas[0] = vc - vr;
      cyc_anas[1] = vc + vr/2 + vp;
      cyc_anas[2] = vc + vr/2 - vp;
      break;
    case SWASH_TYPE_140:
      vp = REZ_SWASH_Y(vp);
      vr = REZ_SWASH_Y(vr);
      cyc_anas[0] = vc - vp;
      cyc_anas[1] = vc + vp + vr;
      cyc_anas[2] = vc + vp - vr;
      break;
    case SWASH_TYPE_90:
      vp = REZ_SWASH_Y(vp);
      vr = REZ_SWASH_Y(vr);
      cyc_anas[0] = vc - vp;
      cyc_anas[1] = vc + vr;
      cyc_anas[2] = vc - vr;
      break;
    default:
      break;
    }
  }
#endif

  memclear(chans, sizeof(chans));        // All outputs to 0

  //========== MIXER LOOP ===================
  uint8_t lv_mixWarning = 0;
  uint8_t pass = 0;

  bitfield_channels_t dirtyChannels = (bitfield_channels_t)-1; // all dirty when mixer starts

  do {

    bitfield_channels_t passDirtyChannels = 0;

    for (uint8_t i=0; i<MAX_MIXERS; i++) {

#if defined(BOLD_FONT)
      if (mode==e_perout_mode_normal && pass==0) swOn[i].activeMix = false;
#endif

      MixData *md = mixAddress(i);

      if (md->srcRaw == 0) break;

      mixsrc_t stickIndex = md->srcRaw - MIXSRC_Rud;

      if (!(dirtyChannels & ((bitfield_channels_t)1 << md->destCh))) continue;

      // if this is the first calculation for the destination channel, initialize it with 0 (otherwise would be random)
      if (i == 0 || md->destCh != (md-1)->destCh) {
        chans[md->destCh] = 0;
      }

      //========== PHASE && SWITCH ==========
      bool mixCondition = (md->flightModes != 0 || md->swtch);
      delayval_t mixEnabled = (!(md->flightModes & (1 << mixerCurrentFlightMode)) && getSwitch(md->swtch)) ? 1 : 0;
#define MIXER_LINE_DISABLE()  (mixCondition = true, mixEnabled = 0)
      if (mixEnabled && md->srcRaw >= MIXSRC_FIRST_TRAINER && md->srcRaw <= MIXSRC_LAST_TRAINER && !IS_TRAINER_INPUT_VALID()) {
        MIXER_LINE_DISABLE();
      }

      //========== VALUE ====================
      getvalue_t v = 0;
      if (mode > e_perout_mode_inactive_flight_mode) {
        if (!mixEnabled || stickIndex >= NUM_STICKS || (stickIndex == THR_STICK && g_model.thrTrim)) {
          continue;
        } else {
          if (!(mode & e_perout_mode_nosticks)) v = anas[stickIndex];
        }
      } else {
        if (stickIndex < NUM_STICKS) {
          v = md->noExpo ? rawAnas[stickIndex] : anas[stickIndex];
        } else {
          mixsrc_t srcRaw = MIXSRC_Rud + stickIndex;
          v = getValue(srcRaw);
          srcRaw -= MIXSRC_CH1;
          if (srcRaw<=MIXSRC_LAST_CH-MIXSRC_CH1 && md->destCh != srcRaw) {
            if (dirtyChannels & ((bitfield_channels_t)1 << srcRaw) & (passDirtyChannels|~(((bitfield_channels_t) 1 << md->destCh)-1)))
              passDirtyChannels |= (bitfield_channels_t) 1 << md->destCh;
            if (srcRaw < md->destCh || pass > 0)
              v = chans[srcRaw]>>8;
          }
        }
        if (!mixCondition) {
          mixEnabled = v>>10;
        }
      }

        //========== GAZ SECURITY ============
        if (md->srcRaw == gazSource)
        {
          if (getSwitch(g_model.thrSwitch ? g_model.thrSwitch+3 : SWSRC_NONE))
          {
            if (v <= (-RESX+10))
              enableGaz = true;
          }
          else
          {
            enableGaz = false;
          }
          if (!enableGaz)
            v = - RESX; // Gaz to min.
        }


      //========== DELAYS ===================
      if (mode <= e_perout_mode_inactive_flight_mode && (md->delayDown || md->delayUp)) { // there are delay values
        if (!s_mixer_first_run_done || !swOn[i].delay) {
          swOn[i].hold = v;     // store actual value of v as reference for next run
          swOn[i].delay = (v > swOn[i].hold ? md->delayUp : md->delayDown) * (100/DELAY_STEP); // init delay
        } else if ((swOn[i].delay > 0) && ((v > swOn[i].hold +10) || (v < swOn[i].hold -10))) { // compare v to value stored at previous run
          swOn[i].delay = max<int16_t>(0, (int16_t)swOn[i].delay - tick10ms);   // decrement delay
          v = swOn[i].hold;     // keep v to stored value until end of delay
        }
      }

      //========== SLOW DOWN ================
      // lower weight causes slower movement
      if (mode <= e_perout_mode_inactive_flight_mode && (md->speedUp || md->speedDown)) { // there are slow-down values
        int32_t tact = act[i];
        int16_t diff = v - (tact>>8);  // we recale to a mult 256 higher value for calculation
        if (diff) {
          // open.20.fsguruh: speed is defined in % movement per second; In menu we specify the full movement (-100% to 100%) = 200% in total
          // the unit of the stored value is the value from md->speedUp or md->speedDown divide SLOW_STEP seconds; e.g. value 4 means 4/SLOW_STEP = 2 seconds for CPU64
          // because we get a tick each 10msec, we need 100 ticks for one second
          // the value in md->speedXXX gives the time it should take to do a full movement from -100 to 100 therefore 200%. This equals 2048 in recalculated internal range
          if (tick10ms || !s_mixer_first_run_done) {
            // only if already time is passed add or substract a value according the speed configured
            int32_t rate = (int32_t)tick10ms<<(8+11);  // = 256*2048*tick10ms
            // rate equals a full range for one second; if less time is passed rate is accordingly smaller
            // if one second passed, rate would be 2048(full motion)*256(recalculated weight)*100(100 ticks needed for one second)
            int32_t currentValue = (int32_t)v<<8;
            if (diff > 0) {
              if (md->speedUp) {
                // if a speed upwards is defined recalculate the new value according configured speed; the higher the speed the smaller the add value is
                int32_t newValue = tact+rate/((int16_t)(100/SLOW_STEP)*md->speedUp);
                if (newValue<currentValue) currentValue = newValue; // Endposition; prevent toggling around the destination
              }
            } else { // if is <0 because ==0 is not possible
              if (md->speedDown) {
                // see explanation in speedUp
                int32_t newValue = tact-rate/((int16_t)(100/SLOW_STEP)*md->speedDown);
                if (newValue>currentValue) currentValue = newValue; // Endposition; prevent toggling around the destination
              }
            }
            act[i] = tact = currentValue;
            // open.20.fsguruh: this implementation would save about 50 bytes code
          } // endif tick10ms ; in case no time passed assign the old value, not the current value from source
          v = tact>>8;
        }
      }

      //========== Active Mix ===============
      bool apply_offset_and_curve = true;
      if (!mixEnabled) {
        if ((md->delayDown || md->delayUp) && md->mltpx!=MLTPX_REP) {
          if (mixCondition) {
            v = (md->mltpx == MLTPX_ADD ? 0 : RESX);
            apply_offset_and_curve = false;
          }
        } else if (mixCondition) {
          continue;
        }
      }
      if (mode==e_perout_mode_normal && (!mixCondition || mixEnabled || swOn[i].delay)) {
        if (md->mixWarn) lv_mixWarning |= 1 << (md->mixWarn - 1);
#if defined(BOLD_FONT)
        swOn[i].activeMix = true;
#endif
      }

      //========== OFFSET BEFORE ============
#if defined(OFFSET_ON_INPUT)
      if (apply_offset_and_curve) {
        int16_t offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
        if (offset) v += calc100toRESX_16Bits(offset);
      }
#endif

      //========== TRIMS ====================
      int16_t trim = 0;
      if (apply_offset_and_curve) {
        if (!(mode & e_perout_mode_notrims)) {
          int8_t mix_trim = md->carryTrim;
          if (mix_trim < TRIM_ON)
            mix_trim = -mix_trim - 1;
          else if (mix_trim == TRIM_ON && stickIndex < NUM_STICKS)
            mix_trim = stickIndex;
          else
            mix_trim = -1;
          if (mix_trim >= 0) {
            trim = trims[mix_trim];
            if (md->curveMode != MODE_DIFFERENTIAL) {
              if (mix_trim == THR_STICK && g_model.throttleReversed)
                v -= trim;
              else
                v += trim;
            }
          }
        }
      }

      //========== CURVES ==================
      if (apply_offset_and_curve && md->curveParam && md->curveMode == MODE_CURVE) {
        v = applyCurve(v, md->curveParam);
      }

      //========== WEIGHT ===================
      int16_t weight = GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
      weight = calc100to256_16Bits(weight);
      int32_t dv = (int32_t) v * weight;
      int32_t dtrim = (int32_t) trim * weight;

      //========== DIFFERENTIAL =============
      if (md->curveMode == MODE_DIFFERENTIAL) {
        // stick and trim are computed separatly
        // @@@2 also recalculate curveParam to a 256 basis which ease the calculation later a lot
        int16_t curveParam = calc100to256(GET_GVAR(md->curveParam, -100, 100, mixerCurrentFlightMode));
        if (curveParam > 0 && dv < 0) {
          dv = (dv * (256 - curveParam)) >> 8;
        } else if (curveParam < 0 && dv > 0) {
          dv = (dv * (256 + curveParam)) >> 8;
        }
        if (curveParam > 0 && dtrim < 0) {
          dtrim = (dtrim * (256 - curveParam)) >> 8;
        } else if (curveParam < 0 && dtrim > 0) {
          dtrim = (dtrim * (256 + curveParam)) >> 8;
        }
        dv += dtrim;
      }

      //========== OFFSET AFTER =============
#if !defined(OFFSET_ON_INPUT)
      if (apply_offset_and_curve) {
        int16_t offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
        if (offset) dv += int32_t(calc100toRESX_16Bits(offset)) << 8;
      }

#endif

      //Stick value to mixer
      //    stick => delay => slow_down => mixer source

      //Mixer output with offset before
      //    Curve(source + offset + trim) * weight
      //    Diff((source + offset) * weight) + Diff(trim * weight)

      //Mixer output with offset after
      //    Curve(source + trim) * weight + offset
      //    Diff(source * weight) + Diff(trim * weight) + offset

      int32_t *ptr = &chans[md->destCh]; // Save calculating address several times

      switch (md->mltpx) {
      case MLTPX_REP:
        *ptr = dv;
#if defined(BOLD_FONT)
        if (mode==e_perout_mode_normal) {
          for (uint8_t m=i-1; m<MAX_MIXERS && mixAddress(m)->destCh==md->destCh; m--)
            swOn[m].activeMix = false;
        }
#endif
        break;
      case MLTPX_MUL:
        // @@@2 we have to remove the weight factor of 256 in case of 100%; now we use the new base of 256
        dv >>= 8;
        dv *= *ptr;
        dv >>= RESX_SHIFT;   // same as dv /= RESXl;
        *ptr = dv;
        break;
      default: // MLTPX_ADD
        *ptr += dv; //Mixer output add up to the line (dv + (dv>0 ? 100/2 : -100/2))/(100);
        break;
      } //endswitch md->mltpx

#ifdef PREVENT_ARITHMETIC_OVERFLOW
      /*
            // a lot of assumptions must be true, for this kind of check; not really worth for only 4 bytes flash savings
            // this solution would save again 4 bytes flash
            int8_t testVar=(*ptr<<1)>>24;
            if ( (testVar!=-1) && (testVar!=0 ) ) {
              // this devices by 64 which should give a good balance between still over 100% but lower then 32x100%; should be OK
              *ptr >>= 6;  // this is quite tricky, reduces the value a lot but should be still over 100% and reduces flash need
            } */

      PACK( union u_int16int32_t {
        struct {
          int16_t lo;
          int16_t hi;
        } words_t;
        int32_t dword;
      });

      u_int16int32_t tmp;
      tmp.dword=*ptr;

      if (tmp.dword<0) {
        if ((tmp.words_t.hi&0xFF80)!=0xFF80) tmp.words_t.hi=0xFF86; // set to min nearly
      } else {
        if ((tmp.words_t.hi|0x007F)!=0x007F) tmp.words_t.hi=0x0079; // set to max nearly
      }
      *ptr = tmp.dword;
      // this implementation saves 18bytes flash

      /*      dv=*ptr>>8;
            if (dv>(32767-RESXl)) {
              *ptr=(32767-RESXl)<<8;
            } else if (dv<(-32767+RESXl)) {
              *ptr=(-32767+RESXl)<<8;
            }*/
      // *ptr=limit( int32_t(int32_t(-1)<<23), *ptr, int32_t(int32_t(1)<<23));  // limit code cost 72 bytes
      // *ptr=limit( int32_t((-32767+RESXl)<<8), *ptr, int32_t((32767-RESXl)<<8));  // limit code cost 80 bytes
#endif

    } //endfor mixers

    tick10ms = 0;
    dirtyChannels &= passDirtyChannels;

  } while (++pass < 5 && dirtyChannels);

  mixWarning = lv_mixWarning;
}

int32_t sum_chans512[NUM_CHNOUT] = {0};

#define MAX_ACT 0xffff
uint8_t lastFlightMode = 255; // TODO reinit everything here when the model changes, no???

void evalMixes(uint8_t tick10ms)
{
  static uint16_t fp_act[MAX_FLIGHT_MODES] = {0};
  static uint16_t delta = 0;
  static uint8_t flightModesFade = 0;

  LS_RECURSIVE_EVALUATION_RESET();

  uint8_t fm = getFlightMode();

  if (lastFlightMode != fm) {
    if (lastFlightMode == 255) {
      fp_act[fm] = MAX_ACT;
    } else {
      uint8_t fadeTime = max(g_model.flightModeData[lastFlightMode].fadeOut, g_model.flightModeData[fm].fadeIn);
      uint8_t transitionMask = ((uint8_t)1 << lastFlightMode) + ((uint8_t)1 << fm);
      if (fadeTime) {
        flightModesFade |= transitionMask;
        delta = (MAX_ACT / (100/SLOW_STEP)) / fadeTime;
      } else {
        flightModesFade &= ~transitionMask;
        fp_act[lastFlightMode] = 0;
        fp_act[fm] = MAX_ACT;
      }
    }
    lastFlightMode = fm;
  }

  int32_t weight = 0;
  if (flightModesFade) {
    memclear(sum_chans512, sizeof(sum_chans512));
    for (uint8_t p=0; p<MAX_FLIGHT_MODES; p++) {
      LS_RECURSIVE_EVALUATION_RESET();
      if (flightModesFade & ((uint8_t)1 << p)) {
        mixerCurrentFlightMode = p;
        evalFlightModeMixes(p==fm ? e_perout_mode_normal : e_perout_mode_inactive_flight_mode, p==fm ? tick10ms : 0);
        for (uint8_t i=0; i<NUM_CHNOUT; i++)
          sum_chans512[i] += (chans[i] >> 4) * fp_act[p];
        weight += fp_act[p];
      }
      LS_RECURSIVE_EVALUATION_RESET();
    }
    assert(weight);
    mixerCurrentFlightMode = fm;
  } else {
    mixerCurrentFlightMode = fm;
    evalFlightModeMixes(e_perout_mode_normal, tick10ms);
  }

  //========== FUNCTIONS ===============
  // must be done after mixing because some functions use the inputs/channels values
  // must be done before limits because of the applyLimit function: it checks for safety switches which would be not initialized otherwise
  if (tick10ms) {
    evalFunctions();
  }

  //========== LIMITS ===============
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    // chans[i] holds data from mixer.   chans[i] = v*weight => 1024*256
    // later we multiply by the limit (up to 100) and then we need to normalize
    // at the end chans[i] = chans[i]/256 =>  -1024..1024 (100%) -1281..1280 (125%)
    // interpolate value with min/max so we get smooth motion from center to stop
    // this limits based on v original values and min=-1024, max=1024  RESX=1024
    int32_t q = (flightModesFade ? (sum_chans512[i] / weight) << 4 : chans[i]);

    ex_chans[i] = q / 256;

    int16_t value = applyLimits(i, q);  // applyLimits will remove the 256 100% basis

    cli();
    channelOutputs[i] = value;  // copy consistent word to int-level
    sei();
    }

  if (tick10ms && flightModesFade) {
    uint16_t tick_delta = delta * tick10ms;
    for (uint8_t p=0; p<MAX_FLIGHT_MODES; p++) {
      uint8_t flightModeMask = ((uint8_t)1 << p);
      if (flightModesFade & flightModeMask) {
        if (p == fm) {
          if (MAX_ACT - fp_act[p] > tick_delta)
            fp_act[p] += tick_delta;
          else {
            fp_act[p] = MAX_ACT;
            flightModesFade -= flightModeMask;
          }
        } else {
          if (fp_act[p] > tick_delta)
            fp_act[p] -= tick_delta;
          else {
            fp_act[p] = 0;
            flightModesFade -= flightModeMask;
          }
        }
      }
    }
  }
}
# 1 "stamp.cpp"
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


#include "OpenAVRc.h"
#include "stamp-OpenAVRc.h"

#define STR2(s) #s
#define DEFNUMSTR(s)  STR2(s)

#define EEPROM_STR DEFNUMSTR(EEPROM_VER);

const pm_char vers_stamp[] PROGMEM = "FW: " "OpenAVRc-" FLAVOUR "\036VERS\037\033: " VERS_STR "\036DATE\037\033: " DATE_STR"\036TIME\037\033: " TIME_STR "\036EEPR\037\033: " EEPROM_STR;
# 1 "gui/navigation.cpp"
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


#include "../OpenAVRc.h"
#include "navigation.h"

#if defined(AUTOSWITCH)
int8_t checkIncDecMovedSwitch(int8_t val)
{
  if (s_editMode>0) {
    int8_t swtch = getMovedSwitch();
    if (swtch) {
      if (IS_MOMENTARY(val) && swtch==val)
        val = -val;
      else
        val = swtch;
    }
  }
  return val;
}
#endif

int8_t  checkIncDec_Ret;

#define DBLKEYS_PRESSED_RGT_LFT(in) ((in & (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_LEFT)) == (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_LEFT))
#define DBLKEYS_PRESSED_UP_DWN(in)  ((in & (KEYS_GPIO_PIN_UP + KEYS_GPIO_PIN_DOWN)) == (KEYS_GPIO_PIN_UP + KEYS_GPIO_PIN_DOWN))
#define DBLKEYS_PRESSED_RGT_UP(in)  ((in & (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_UP))  == (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_UP))
#define DBLKEYS_PRESSED_LFT_DWN(in) ((in & (KEYS_GPIO_PIN_LEFT + KEYS_GPIO_PIN_DOWN)) == (KEYS_GPIO_PIN_LEFT + KEYS_GPIO_PIN_DOWN))

uint8_t switchToMix(uint8_t source)
{
  if (source <= 3)
    return MIXSRC_3POS;
  else
    return MIXSRC_FIRST_SWITCH - 3 + source;
}

int16_t checkIncDec(uint8_t event, int16_t val, int16_t i_min, int16_t i_max, uint8_t i_flags)
{
  int16_t newval = val;

#if defined(DBLKEYS)
  uint8_t in = KEYS_PRESSED();
  if (!(i_flags & NO_DBLKEYS) && (EVT_KEY_MASK(event))) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in))
      newval = -val;
    else if (DBLKEYS_PRESSED_RGT_UP(in)) {
      newval = (i_max > 100 ? 100 : i_max);
    } else if (DBLKEYS_PRESSED_LFT_DWN(in)) {
      newval = (i_min < -100 ? -100 : i_min);
    } else if (DBLKEYS_PRESSED_UP_DWN(in)) {
      newval = 0;
    } else {
      dblkey = false;
    }

    if (dblkey) {
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      killEvents(KEY_RIGHT);
      killEvents(KEY_LEFT);
      event = 0;
    }
  }
#endif

  if (event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_RIGHT) || (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP)))) {
    newval++;
    AUDIO_KEYPAD_UP();
  } else if (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_REPT(KEY_LEFT) || (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN)))) {
    newval--;
    AUDIO_KEYPAD_DOWN();
  }

  if (!READ_ONLY() && i_min==0 && i_max==1 && (event==EVT_KEY_BREAK(KEY_ENTER) || IS_ROTARY_BREAK(event))) {
    s_editMode = 0;
    newval = !val;
  }

#if defined(NAVIGATION_POT1)
  // change values based on P1
  newval -= p1valdiff;
  p1valdiff = 0;
#endif

#if defined(AUTOSWITCH)
  if (i_flags & INCDEC_SWITCH) {
    newval = checkIncDecMovedSwitch(newval);
  }
#endif

#if defined(AUTOSOURCE)
  if (i_flags & INCDEC_SOURCE) {
    if (s_editMode>0) {
      int8_t source = GET_MOVED_SOURCE(i_min, i_max);
      if (source) {
        newval = source;
      }
#if defined(AUTOSWITCH)
      else {
        uint8_t swtch = abs(getMovedSwitch());
        if (swtch) {
          newval = switchToMix(swtch);
        }
      }
#endif
    }
  }
#endif

  if (newval > i_max || newval < i_min) {
    newval = (newval > i_max ? i_max : i_min);
    killEvents(event);
    AUDIO_WARNING2();
  }

  if (newval != val) {
    if (!(i_flags & NO_INCDEC_MARKS) && (newval != i_max) && (newval != i_min) && (newval==0 || newval==-100 || newval==+100) && !IS_ROTARY_EVENT(event)) {
      pauseEvents(event); // delay before auto-repeat continues
      if (newval>val) // without AUDIO it's optimized, because the 2 sounds are the same
        AUDIO_KEYPAD_UP();
      else
        AUDIO_KEYPAD_DOWN();
    }
    eeDirty(i_flags & (EE_GENERAL|EE_MODEL));
    checkIncDec_Ret = (newval > val ? 1 : -1);
  } else {
    checkIncDec_Ret = 0;
  }
  return newval;
}


#define SCROLL_TH      64
#define SCROLL_POT1_TH 32

#define CURSOR_NOT_ALLOWED_IN_ROW(row)   (MAXCOL(row) == TITLE_ROW)

#define MAXCOL(row)     (horTab ? pgm_read_byte_near(horTab+min(row, (vertpos_t)horTabMax)) : (const uint8_t)0)
#define INC(val, min, max) if (val<max) {val++;} else {val=min;}
#define DEC(val, min, max) if (val>min) {val--;} else {val=max;}


void check(check_event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, vertpos_t maxrow)
{
  vertpos_t l_posVert = menuVerticalPosition;
  horzpos_t l_posHorz = menuHorizontalPosition;

  uint8_t maxcol = MAXCOL(l_posVert);

#if defined(NAVIGATION_POT1)
  // check pot 1 - if changed -> scroll values
  static int16_t p1val;
  static int16_t p1valprev;
  p1valdiff = (p1val-calibratedStick[6]) / SCROLL_POT1_TH;
  if (p1valdiff) {
    p1valdiff = (p1valprev-calibratedStick[6]) / 2;
    p1val = calibratedStick[6];
  }
  p1valprev = calibratedStick[6];
#endif

#if defined(NAVIGATION_POT2)
  // check pot 2 - if changed -> scroll menu
  static int16_t p2valprev;
  p2valdiff = (p2valprev-calibratedStick[4]) / SCROLL_TH;
  if (p2valdiff) p2valprev = calibratedStick[4];
#endif

#if defined(NAVIGATION_POT3)
  // check pot 3 if changed -> cursor down/up
  static int16_t p3valprev;
  int8_t scrollUD = (p3valprev-calibratedStick[5]) / SCROLL_TH;
  if (scrollUD) p3valprev = calibratedStick[5];
#else
#define scrollUD 0
#endif

  if (p2valdiff || scrollUD || p1valdiff) backlightOn(); // on keypress turn the light on

  if (menuTab) {
    uint8_t attr = 0;


    if (l_posVert==0 && !calibrationState) {
      attr = INVERS;

      int8_t cc = curr;

      if (p2valdiff) {
        cc = limit((int8_t)0, (int8_t)(cc - p2valdiff), (int8_t)(menuTabSize-1));
      }

      switch(event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_BREAK:
        if (s_editMode < 0 && maxrow > 0) {
          s_editMode = 0;
          // TODO ? l_posVert = (horTab && horTab[1]==0xff) ? 2 : 1;
          l_posHorz = 0;
        } else {
          s_editMode = -1;
        }
        event = 0;
        break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_LEFT:
        if (s_editMode >= 0)
          break;
#endif
      case EVT_KEY_FIRST(KEY_LEFT):
        if (curr > 0)
          cc = curr - 1;
        else
          cc = menuTabSize-1;
        break;

#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_RIGHT:
        if (s_editMode >= 0)
          break;
#endif
      case EVT_KEY_FIRST(KEY_RIGHT):
        if (curr < (menuTabSize-1))
          cc = curr + 1;
        else
          cc = 0;
        break;
      }

      if (cc != curr) {
        chainMenu((MenuHandlerFunc)pgm_read_word_near(&menuTab[cc]));
      }

#if defined(ROTARY_ENCODER_NAVIGATION)
      if (IS_RE_NAVIGATION_ENABLE() && s_editMode < 0)
        attr = INVERS|BLINK;
#endif
    }

    calibrationState = 0;
    displayScreenIndex(curr, menuTabSize, attr);

  }

  DISPLAY_PROGRESS_BAR(menuTab ? lcdLastPos-2*FW-((curr+1)/10*FWNUM)-2 : 20*FW+1);

  if (s_editMode<=0) {
    if (scrollUD) {
      l_posVert = limit((int8_t)0, (int8_t)(l_posVert - scrollUD), (int8_t)maxrow);
      l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));
    }

    if (p2valdiff && l_posVert>0) {
      l_posHorz = limit((int8_t)0, (int8_t)((uint8_t)l_posHorz - p2valdiff), (int8_t)maxcol);
    }
  }

  switch(event) {
  case EVT_ENTRY:
    l_posVert = POS_VERT_INIT;
    l_posHorz = POS_HORZ_INIT(l_posVert);
    SET_SCROLLBAR_X(LCD_W-1);
#if defined(ROTARY_ENCODER_NAVIGATION)
    if (menuTab) {
      s_editMode = EDIT_MODE_INIT;
      break;
    }
    // no break
#else
    s_editMode = EDIT_MODE_INIT;
    break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ENTRY_UP:
    s_editMode = 0;
    SET_SCROLLBAR_X(LCD_W-1);
    break;

  case EVT_ROTARY_BREAK:
    if (s_editMode > 1) break;
#endif

  case EVT_KEY_FIRST(KEY_ENTER):
    if (!menuTab || l_posVert>0) {
      if (READ_ONLY_UNLOCKED()) {
        s_editMode = (s_editMode<=0);
      }
    }
    break;

#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_LONG:
    if (s_editMode > 1) break;
    killEvents(event);
    if (l_posVert != POS_VERT_INIT) {
      l_posVert = POS_VERT_INIT;
      s_editMode = EDIT_MODE_INIT;
      break;
    }
    // no break
#endif
  case EVT_KEY_LONG(KEY_EXIT):
    s_editMode = 0; // TODO needed? we call ENTRY_UP after which does the same
    popMenu();
    break;

  case EVT_KEY_BREAK(KEY_EXIT):
#if defined(ROTARY_ENCODER_NAVIGATION)
    if (s_editMode == 0)
      s_editMode = EDIT_MODE_INIT;
    else
#endif
      if (s_editMode>0) {
        s_editMode = 0;
        break;
      }

    if (l_posVert==0 || !menuTab) {
      popMenu();  // beeps itself
    } else {
      AUDIO_MENUS();
      l_posVert = 0;
      l_posHorz = 0;
    }
    break;

  case EVT_KEY_REPT(KEY_RIGHT):  //inc
    if (l_posHorz==maxcol) break;
  // no break

  case EVT_KEY_FIRST(KEY_RIGHT)://inc
    if (!horTab || s_editMode>0) break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_MOVE_RIGHT
    if (s_editMode != 0) break;
    if (l_posHorz < maxcol) {
      l_posHorz++;
      break;
    } else {
      l_posHorz = 0;
      if (!IS_ROTARY_MOVE_RIGHT(event))
        break;
    }
#else
    INC(l_posHorz, 0, maxcol);
    break;
#endif

  case EVT_KEY_REPT(KEY_DOWN):  //inc
    if (!IS_ROTARY_RIGHT(event) && l_posVert==maxrow) break;
  // no break

  case EVT_KEY_FIRST(KEY_DOWN): //inc
    if (s_editMode>0) break;
    do {
      INC(l_posVert, POS_VERT_INIT, maxrow);
    } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

#if defined(ROTARY_ENCODER_NAVIGATION)
    s_editMode = 0; // if we go down, we must be in this mode
#endif

    l_posHorz = min(l_posHorz, MAXCOL(l_posVert));
    break;

  case EVT_KEY_REPT(KEY_LEFT):  //dec
    if (l_posHorz==0) break;
  // no break

  case EVT_KEY_FIRST(KEY_LEFT)://dec
    if (!horTab || s_editMode>0) break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_MOVE_LEFT
    if (s_editMode != 0) break;
    if (l_posHorz > 0) {
      l_posHorz--;
      break;
    } else if (IS_ROTARY_MOVE_LEFT(event) && s_editMode == 0) {
      l_posHorz = 0xff;
    } else {
      l_posHorz = maxcol;
      break;
    }
#else
    DEC(l_posHorz, 0, maxcol);
    break;
#endif

  case EVT_KEY_REPT(KEY_UP):  //dec
    if (!IS_ROTARY_LEFT(event) && l_posVert==0) break;
  // no break
  case EVT_KEY_FIRST(KEY_UP): //dec
    if (s_editMode>0) break;

    do {
      DEC(l_posVert, POS_VERT_INIT, maxrow);
    } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

#if defined(ROTARY_ENCODER_NAVIGATION)
    s_editMode = 0; // if we go up, we must be in this mode
#endif

    l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));
    break;
  }

  uint8_t maxLines = menuTab ? LCD_LINES-1 : LCD_LINES-2;

  if (l_posVert<1) {
    menuVerticalOffset=0;
  } else {
    if (l_posVert>maxLines+menuVerticalOffset) {
      menuVerticalOffset = l_posVert-maxLines;
    } else if (l_posVert<=menuVerticalOffset) {
      menuVerticalOffset = l_posVert-1;
    }
  }

  menuVerticalPosition = l_posVert;
  menuHorizontalPosition = l_posHorz;
  // cosmetics on 9x
  if (menuVerticalOffset > 0) {
    l_posVert--;
    if (l_posVert == menuVerticalOffset && CURSOR_NOT_ALLOWED_IN_ROW(l_posVert)) {
      menuVerticalOffset = l_posVert-1;
    }
  }
}

#if defined(NAVIGATION_STICKS)
uint8_t StickScrollAllowed;
uint8_t StickScrollTimer;
static const pm_uint8_t rate[] PROGMEM = { 0, 0, 40, 15, 7, 5, 2, 1 } ;

uint8_t calcStickScroll( uint8_t index )
{
  uint8_t direction;
  int8_t value;

  if (!g_eeGeneral.stickMode & 1)  index ^= 3;

  value = calibratedStick[index] / 128;
  direction = value > 0 ? 0x80 : 0;
  if (value < 0) value = -value;  // (abs)
  if (value > 7) value = 7;
  value = pgm_read_byte_near(rate+(uint8_t)value);
  if (value) StickScrollTimer = STICK_SCROLL_TIMEOUT; // Seconds
  return value | direction;
}

uint8_t getSticksNavigationEvent()
{
  uint8_t evt = 0;
  if (StickScrollAllowed) {
    if ( StickScrollTimer ) {
      static uint8_t repeater;
      uint8_t direction;
      uint8_t value;

      if (repeater < 128) {++repeater;}
      value = calcStickScroll(CONVERT_MODE(THR_STICK));
      direction = value & 0x80;
      value &= 0x7F;

      if (value) {

        if (repeater > value) {
          repeater = 0;
          if (!evt) {
            if (direction) {
              evt = KEY_UP;
            } else {
              evt = KEY_DOWN;
            }
          }
        }
      } else {

        value = calcStickScroll(CONVERT_MODE(AIL_STICK));
        direction = value & 0x80;
        value &= 0x7F;

        if (value) {
          if (repeater > value) {
            repeater = 0;
            if (!evt) {
              if (direction) {
                evt = KEY_RIGHT;
              } else {
                evt = KEY_LEFT;
              }
            }
          }
        }
      }
    }
  } else {
    StickScrollTimer = 0;          // Seconds
  }
  StickScrollAllowed = 1 ;
  return evt;
}
#endif

void check_simple(check_event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, vertpos_t maxrow)
{
  check(event, curr, menuTab, menuTabSize, 0, 0, maxrow);
}

void check_submenu_simple(check_event_t event, uint8_t maxrow)
{
  check_simple(event, 0, 0, 0, maxrow);
}

void repeatLastCursorMove(uint8_t event)
{
  if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) {
    putEvent(event);
  } else {
    menuHorizontalPosition = 0;
  }
}
# 1 "gui/popups.cpp"
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


#include "../OpenAVRc.h"
#include "popups.h"

void displayBox()
{
  lcdDrawFilledRect(10, 16, LCD_W-20, 40, SOLID, ERASE);
  lcdDrawRect(10, 16, LCD_W-20, 40);
  lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y, warningText);
  // could be a place for a warningInfoText
}

void displayPopup(const pm_char * pstr)
{
  warningText = pstr;
  displayBox();
  warningText = NULL;
  lcdRefresh();
}

void message(const pm_char *title, const pm_char *t, const char *last MESSAGE_SOUND_ARG)
{
  lcdClear();
  lcd_img(2, 0, asterisk_lbm, 0, 0);

#define MESSAGE_LCD_OFFSET   6*FW

#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_IT) || defined(TRANSLATIONS_CZ)
  lcdDrawTextAtt(MESSAGE_LCD_OFFSET, 0, STR_WARNING, DBLSIZE);
  lcdDrawTextAtt(MESSAGE_LCD_OFFSET, 2*FH, title, DBLSIZE);
#else
  lcdDrawTextAtt(MESSAGE_LCD_OFFSET, 0, title, DBLSIZE);
  lcdDrawTextAtt(MESSAGE_LCD_OFFSET, 2*FH, STR_WARNING, DBLSIZE);
#endif

  lcdDrawFilledRect(0, 0, LCD_W, 32);
  if (t) lcdDrawTextLeft(5*FH, t);
  if (last) {
    lcdDrawTextLeft(7*FH, last);
    AUDIO_ERROR_MESSAGE(sound);
  }

#undef MESSAGE_LCD_OFFSET

  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
}

void displayWarning(uint8_t event)
{
  warningResult = false;
  displayBox();
  if (warningInfoText) {
    lcdDrawSizedTextAtt(WARNING_LINE_X, WARNING_LINE_Y+FH, warningInfoText, warningInfoLength, WARNING_INFO_FLAGS);
  }
  lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y+2*FH, warningType == WARNING_TYPE_ASTERISK ? STR_EXIT : STR_POPUPS);
  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_BREAK:
#endif
  case EVT_KEY_BREAK(KEY_ENTER):
    if (warningType == WARNING_TYPE_ASTERISK)
      break;
    warningResult = true;
    // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_LONG:
    killEvents(event);
#endif
  case EVT_KEY_BREAK(KEY_EXIT):
    warningText = NULL;
    warningType = WARNING_TYPE_ASTERISK;
    break;
  }
}


#if defined(NAVIGATION_MENUS)
void (*popupMenuHandler)(const char *result);

const char * displayPopupMenu(uint8_t event)
{
  const char * result = NULL;

  uint8_t display_count = min<uint8_t>(popupMenuNoItems, POPUP_MENU_MAX_LINES);
  uint8_t y = (display_count >= 5 ? MENU_Y - FH - 1 : MENU_Y);
  lcdDrawFilledRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2, SOLID, ERASE);
  lcdDrawRect(MENU_X, y, MENU_W, display_count * (FH+1) + 2);

  for (uint8_t i=0; i<display_count; i++) {
    lcdDrawTextAtt(MENU_X+6, i*(FH+1) + y + 2, popupMenuItems[i], popupMenuFlags);
    if (i == s_menu_item) lcdDrawFilledRect(MENU_X+1, i*(FH+1) + y + 1, MENU_W-2, 9);
  }

  if (popupMenuNoItems > display_count) {
    displayScrollbar(MENU_X+MENU_W-1, y+1, POPUP_MENU_MAX_LINES * (FH+1), popupMenuOffset, popupMenuNoItems, POPUP_MENU_MAX_LINES);
  }

  switch(event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LEFT
#endif
  case EVT_KEY_FIRST(KEY_MOVE_UP):
  case EVT_KEY_REPT(KEY_MOVE_UP):
    if (s_menu_item > 0) {
      s_menu_item--;
    }
#if defined(SDCARD)
    else if (popupMenuOffset > 0) {
      popupMenuOffset--;
      result = STR_UPDATE_LIST;
    }
#endif
    else {
      s_menu_item = display_count - 1;
#if defined(SDCARD)
      if (popupMenuNoItems > POPUP_MENU_MAX_LINES) {
        popupMenuOffset = popupMenuNoItems - display_count;
        result = STR_UPDATE_LIST;
      }
#endif
    }
    break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_RIGHT
#endif
  case EVT_KEY_FIRST(KEY_MOVE_DOWN):
  case EVT_KEY_REPT(KEY_MOVE_DOWN):
    if (s_menu_item < display_count - 1 && popupMenuOffset + s_menu_item + 1 < popupMenuNoItems) {
      s_menu_item++;
    }
#if defined(SDCARD)
    else if (popupMenuNoItems > popupMenuOffset + display_count) {
      popupMenuOffset++;
      result = STR_UPDATE_LIST;
    }
#endif
    else {
      s_menu_item = 0;
#if defined(SDCARD)
      if (popupMenuOffset) {
        popupMenuOffset = 0;
        result = STR_UPDATE_LIST;
      }
#endif
    }
    break;
    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_BREAK(KEY_ENTER):
    result = popupMenuItems[s_menu_item];
    // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LONG
    killEvents(event);
#endif
  case EVT_KEY_BREAK(KEY_EXIT):
    popupMenuNoItems = 0;
    s_menu_item = 0;
    popupMenuFlags = 0;
    popupMenuOffset = 0;
    break;
  }

  return result;
}
#endif
# 1 "gui/widgets.cpp"
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


#include "../OpenAVRc.h"

void drawStick(coord_t centrex, int16_t xval, int16_t yval)
{
#define BOX_CENTERY   (LCD_H-9-BOX_WIDTH/2)
#define MARKER_WIDTH  5
  lcd_square(centrex-BOX_WIDTH/2, BOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);
  lcdDrawSolidVerticalLine(centrex, BOX_CENTERY-1, 3);
  lcdDrawSolidHorizontalLine(centrex-1, BOX_CENTERY, 3);
  lcd_square(centrex + (xval/((2*RESX)/(BOX_WIDTH-MARKER_WIDTH))) - MARKER_WIDTH/2, BOX_CENTERY - (yval/((2*RESX)/(BOX_WIDTH-MARKER_WIDTH))) - MARKER_WIDTH/2, MARKER_WIDTH, ROUND);
#undef BOX_CENTERY
#undef MARKER_WIDTH
}

void menu_lcd_onoff(coord_t x, coord_t y, uint8_t value, LcdFlags attr)
{
#if defined(GRAPHICS)
  if (value)
    lcdDrawChar(x+1, y, '#');
  if (attr)
    lcdDrawFilledRect(x, y, 7, 7);
  else
    lcd_square(x, y, 7);
#else
  /* ON / OFF version */
  lcdDrawTextAtIndex(x, y, STR_OFFON, value, attr ? INVERS:0) ;
#endif
}

void displayScreenIndex(uint8_t index, uint8_t count, uint8_t attr)
{
  lcdDrawNumberAttUnit(LCD_W, 0, count, attr);
  coord_t x = 1+LCD_W-FW*(count>9 ? 3 : 2);
  lcdDrawCharAtt(x, 0, '/', attr);
  lcdDrawNumberAttUnit(x, 0, index+1, attr);
}

void displayScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible)
{
  lcdDrawSolidVerticalLineStip(x, y, h, DOTTED);
  coord_t yofs = (h * offset) / count;
  coord_t yhgt = (h * visible) / count;
  if (yhgt + yofs > h)
    yhgt = h - yofs;
  lcdDrawSolidVerticalLineStip(x, y + yofs, yhgt, SOLID, FORCE);
}

void title(const pm_char * s)
{
  lcdDrawTextAtt(0, 0, s, INVERS);
}

select_menu_value_t selectMenuItem(coord_t x, coord_t y, const pm_char *label, const pm_char *values, select_menu_value_t value, select_menu_value_t min, select_menu_value_t max, LcdFlags attr, uint8_t event)
{
  lcdDrawTextColumnLeft(x, y, label);
  if (values) lcdDrawTextAtIndex(x, y, values, value-min, attr);
  if (attr) value = checkIncDec(event, value, min, max, (menuVerticalPositions[0] == 0) ? EE_MODEL : EE_GENERAL);
  return value;
}

uint8_t onoffMenuItem(uint8_t value, coord_t x, coord_t y, const pm_char *label, LcdFlags attr, uint8_t event )
{
#if defined(GRAPHICS)
  menu_lcd_onoff(x, y, value, attr);
  return selectMenuItem(x, y, label, NULL, value, 0, 1, attr, event);
#else
  return selectMenuItem(x, y, label, STR_OFFON, value, 0, 1, attr, event);
#endif
}

int8_t switchMenuItem(coord_t x, coord_t y, int8_t value, LcdFlags attr, uint8_t event)
{
  lcdDrawTextColumnLeft(x, y, STR_SWITCH);
  lcdPutsSwitches(x,  y, value, attr);
  if (attr) CHECK_INCDEC_MODELSWITCH(event, value, SWSRC_FIRST_IN_MIXES, SWSRC_LAST_IN_MIXES, isSwitchAvailableInMixes);
  return value;
}

void displaySlider(coord_t x, coord_t y, uint8_t value, uint8_t max, uint8_t attr)
{
  lcdDrawChar(x+(value*4*FW)/max, y, '$');
  lcdDrawSolidHorizontalLine(x, y+3, 5*FW-1, FORCE);
  if (attr && (!(attr & BLINK) || !BLINK_ON_PHASE)) lcdDrawFilledRect(x, y, 5*FW-1, FH-1);
}

#if   defined(GVARS)
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)
{
  uint16_t delta = GV_GET_GV1_VALUE(max);
  bool invers = (attr & INVERS);

  //TRACE("gvarMenuItem(val=%d min=%d max=%d)", value, min, max);

  if (invers && event == EVT_KEY_LONG(KEY_ENTER)) {
    s_editMode = !s_editMode;
    value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode) : delta);
    eeDirty(EE_MODEL);
  }
  if (GV_IS_GV_VALUE(value, min, max)) {
    if (attr & LEFT)
      attr -= LEFT; /* because of ZCHAR */
    else
      x -= 2*FW+FWNUM;

    int8_t idx = (int16_t) GV_INDEX_CALC_DELTA(value, delta);
    if (invers) {
      idx = checkIncDec(event, idx, -MAX_GVARS, MAX_GVARS-1, EE_MODEL|NO_DBLKEYS);   // disable double keys
    }
    if (idx < 0) {
      value = (int16_t) GV_CALC_VALUE_IDX_NEG(idx, delta);
      idx = -idx;
      lcdDrawCharAtt(x-6, y, '-', attr);
    } else {
      value = (int16_t) GV_CALC_VALUE_IDX_POS(idx, delta);
      idx++;
    }
    lcdDrawStringWithIndex(x, y, STR_GV, idx, attr);
  } else {
    lcdDrawNumberAttUnit(x, y, value, attr);
    if (invers) value = checkIncDec(event, value, min, max, EE_MODEL);
  }
  return value;
}
#else
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event)
{
  lcdDrawNumberAttUnit(x, y, value, attr);
  if (attr&INVERS) value = checkIncDec(event, value, min, max, EE_MODEL);
  return value;
}
#endif

#if defined(SDCARD)
char statusLineMsg[STATUS_LINE_LENGTH];
tmr10ms_t statusLineTime = 0;
uint8_t statusLineHeight = 0;

void showStatusLine()
{
  statusLineTime = get_tmr10ms();
}

#define STATUS_LINE_DELAY (3 * 100) /* 3s */
void drawStatusLine()
{
  if (statusLineTime) {
    if ((tmr10ms_t)(get_tmr10ms() - statusLineTime) <= (tmr10ms_t)STATUS_LINE_DELAY) {
      if (statusLineHeight < FH) statusLineHeight++;
    } else if (statusLineHeight) {
      statusLineHeight--;
    } else {
      statusLineTime = 0;
    }

    lcdDrawFilledRect(0, LCD_H-statusLineHeight, LCD_W, FH, SOLID, ERASE);
    lcdDrawTextAtt(5, LCD_H+1-statusLineHeight, statusLineMsg, BSS);
    lcdDrawFilledRect(0, LCD_H-statusLineHeight, LCD_W, FH, SOLID);
  }
}
#endif
# 1 "gui/menus.cpp"
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


#include "../OpenAVRc.h"

MenuHandlerFunc menuHandlers[5];
uint8_t menuEvent = 0;
uint8_t menuVerticalPositions[4];
uint8_t menuLevel = 0;

void popMenu()
{
  assert(menuLevel>0);
  menuLevel = menuLevel-1;
  menuEvent = EVT_ENTRY_UP;
}

void chainMenu(MenuHandlerFunc newMenu)
{
  menuHandlers[menuLevel] = newMenu;
  menuEvent = EVT_ENTRY;
}

void pushMenu(MenuHandlerFunc newMenu)
{
  killEvents(KEY_ENTER);

  if (menuLevel == 0) {
    if (newMenu == menuGeneralSetup)
      menuVerticalPositions[0] = 1;
    if (newMenu == menuModelSelect)
      menuVerticalPositions[0] = 0;
  } else {
    menuVerticalPositions[menuLevel] = menuVerticalPosition;
  }

  menuLevel++;

  assert(menuLevel < DIM(menuHandlers));

  menuHandlers[menuLevel] = newMenu;
  menuEvent = EVT_ENTRY;
}

# 1 "gui/menu_model.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"


uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcdDrawTextLeft(y, str);
  lcdDrawNumberAttUnit(MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}

uint8_t s_copyMode = 0;
int8_t s_copySrcRow;
int8_t s_copyTgtOfs;
uint8_t s_currIdx;
uint8_t editNameCursorPos = 0;


void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active)
{

  uint8_t mode = 0;
  if ((active) && (s_editMode <= 0)) {
    mode = INVERS;
  }

  lcdDrawSizedTextAtt(x, y, name, size, ZCHAR | FIXEDWIDTH | mode);

  if (active) {

    uint8_t cur = editNameCursorPos;
    if (s_editMode > 0) {

      int8_t c = name[cur];
      int8_t v = c;

      if (p1valdiff || IS_ROTARY_RIGHT(event) || IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP)
          || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP)) {
        v = checkIncDec(event, abs(v), 0, ZCHAR_MAX, 0);
        if (c <= 0) v = -v;
      }

      switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_BREAK:
        if (s_editMode == EDIT_MODIFY_FIELD) {
          s_editMode = EDIT_MODIFY_STRING;
          cur = 0;
        } else if (cur<size-1)
          cur++;
        else
          s_editMode = 0;
        break;
#endif

      case EVT_KEY_BREAK(KEY_LEFT):
        if (cur>0) cur--;
        break;
      case EVT_KEY_BREAK(KEY_RIGHT):
        if (cur<size-1) cur++;
        break;

#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_LONG:
        if (v==0) {
          s_editMode = 0;
          killEvents(event);
          break;
        }
        // no break
#endif

      case EVT_KEY_LONG(KEY_LEFT):
      case EVT_KEY_LONG(KEY_RIGHT):
        if (v>=-26 && v<=26) {
          v = -v; // toggle case
          if (event==EVT_KEY_LONG(KEY_LEFT))
            killEvents(KEY_LEFT);
        }
        break;
      }

      if (c != v) {
        name[cur] = v;
        eeDirty(EE_MODEL);
      }

      lcdDrawCharAtt(x+editNameCursorPos*FW, y, idx2char(v), ERASEBG|INVERS|FIXEDWIDTH);
    } else {
      cur = 0;
    }
    editNameCursorPos = cur;
  }
}

void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active)
{
  lcdDrawTextLeft(y, label);
  editName(x, y, name, size, event, active);
}
# 1 "gui/menu_model_select.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"


#define MODELSIZE_POS_X  170
#define MODELSEL_W       LCD_W

#if defined(NAVIGATION_MENUS)
void onModelSelectMenu(const char *result)
{
  int8_t sub = menuVerticalPosition;

  if (result == STR_SELECT_MODEL || result == STR_CREATE_MODEL) {
    selectModel(sub);
    popMenu();
  } else if (result == STR_COPY_MODEL) {
    s_copyMode = COPY_MODE;
    s_copyTgtOfs = 0;
    s_copySrcRow = -1;
  } else if (result == STR_MOVE_MODEL) {
    s_copyMode = MOVE_MODE;
    s_copyTgtOfs = 0;
    s_copySrcRow = -1;
  }
#if defined(SDCARD)
  else if (result == STR_BACKUP_MODEL) {
    eeCheck(true); // force writing of current model data before this is changed
    POPUP_WARNING(eeBackupModel(sub));
  } else if (result == STR_RESTORE_MODEL || result == STR_UPDATE_LIST) {
    if (!listSdFiles(MODELS_PATH, MODELS_EXT, MENU_LINE_LENGTH-1, NULL, 0)) {
      POPUP_WARNING(STR_NO_MODELS_ON_SD);
      popupMenuFlags = 0;
    }
  }
#endif
  else if (result == STR_DELETE_MODEL) {
    POPUP_CONFIRMATION(STR_DELETEMODEL);
    char * name = reusableBuffer.modelsel.mainname;
    eeLoadModelName(sub, name);
    SET_WARNING_INFO(name, sizeof(g_model.name), ZCHAR);
  }
#if defined(SDCARD)
  else {
    // The user choosed a file on SD to restore
    POPUP_WARNING(eeRestoreModel(sub, (char *)result));
    if (!warningText && g_eeGeneral.currModel == sub) {
      eeLoadModel(sub);
    }
  }
#endif
}
#endif

void menuModelSelect(uint8_t event)
{
  if (warningResult) {
    warningResult = false;
    eeDeleteModel(menuVerticalPosition); // delete file
    s_copyMode = 0;
    event = EVT_ENTRY_UP;
  }

  uint8_t _event_ = (IS_ROTARY_BREAK(event) || IS_ROTARY_LONG(event) ? 0 : event);

  if ((s_copyMode && EVT_KEY_MASK(event) == KEY_EXIT) || event == EVT_KEY_BREAK(KEY_EXIT)) {
    _event_ -= KEY_EXIT;
  }

  int8_t oldSub = menuVerticalPosition;

  check_submenu_simple(_event_, MAX_MODELS-1);

#if defined(NAVIGATION_POT2)
  if (event==0 && p2valdiff<0) {
    event = EVT_KEY_FIRST(KEY_RIGHT);
  }
#endif

  if (s_editMode > 0) s_editMode = 0;

  if (event) {
    eeFlush(); // flush eeprom write
  }

  int8_t sub = menuVerticalPosition;

  switch (event) {
  case EVT_ENTRY:
    menuVerticalPosition = sub = g_eeGeneral.currModel;
    if (sub >= LCD_LINES-1) menuVerticalOffset = sub-LCD_LINES+2;
    s_copyMode = 0;
    s_editMode = EDIT_MODE_INIT;
    eeCheck(true);
    break;

  case EVT_KEY_LONG(KEY_EXIT):
    killEvents(event);
    if (s_copyMode && s_copyTgtOfs == 0 && g_eeGeneral.currModel != sub && eeModelExists(sub)) {
      POPUP_CONFIRMATION(STR_DELETEMODEL);
      char * name = reusableBuffer.modelsel.mainname;
      eeLoadModelName(sub, name);
      SET_WARNING_INFO(name, sizeof(g_model.name), ZCHAR);
    } else {
      s_copyMode = 0;
      menuVerticalPosition = g_eeGeneral.currModel;
    }
    break;

#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_LONG:
    killEvents(event);
    if (s_editMode < 0) {
      popMenu();
      break;
    } else if (!s_copyMode) {
      menuVerticalPosition = sub = g_eeGeneral.currModel;
      s_copyMode = 0;
      s_editMode = EDIT_MODE_INIT;
    }
    // no break
#endif

  case EVT_KEY_BREAK(KEY_EXIT):
    if (s_copyMode) {
      sub = menuVerticalPosition = (s_copyMode == MOVE_MODE || s_copySrcRow<0) ? (MAX_MODELS+sub+s_copyTgtOfs) % MAX_MODELS : s_copySrcRow;
      s_copyMode = 0;
    } else if (uint8_t(menuVerticalPosition) != g_eeGeneral.currModel) {
      menuVerticalPosition = g_eeGeneral.currModel;
    } else {
      popMenu();
    }
    break;

#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_BREAK:
    if (s_editMode == -1) {
      s_editMode = 0;
      break;
    }
    // no break;
#endif

  case EVT_KEY_LONG(KEY_ENTER):
  case EVT_KEY_BREAK(KEY_ENTER):
    s_editMode = 0;
    if (READ_ONLY()) {
      if (g_eeGeneral.currModel != sub && eeModelExists(sub)) {
      }
    } else if (s_copyMode && (s_copyTgtOfs || s_copySrcRow>=0)) {
      displayPopup(s_copyMode==COPY_MODE ? STR_COPYINGMODEL : STR_MOVINGMODEL);
      eeCheck(true); // force writing of current model data before this is changed

      uint8_t cur = (MAX_MODELS + sub + s_copyTgtOfs) % MAX_MODELS;

      if (s_copyMode == COPY_MODE) {
        if (!eeCopyModel(cur, s_copySrcRow)) {
          cur = sub;
        }
      }

      s_copySrcRow = g_eeGeneral.currModel; // to update the currModel value
      while (sub != cur) {
        uint8_t src = cur;
        cur = (s_copyTgtOfs > 0 ? cur+MAX_MODELS-1 : cur+1) % MAX_MODELS;
        eeSwapModels(src, cur);
        if (src == s_copySrcRow)
          s_copySrcRow = cur;
        else if (cur == s_copySrcRow)
          s_copySrcRow = src;
      }

      if (s_copySrcRow != g_eeGeneral.currModel) {
        g_eeGeneral.currModel = s_copySrcRow;
        eeDirty(EE_GENERAL);
      }

      s_copyMode = 0;
      event = EVT_ENTRY_UP;
    } else if (event == EVT_KEY_LONG(KEY_ENTER) || IS_ROTARY_BREAK(event)) {
      s_copyMode = 0;
      killEvents(event);
#if defined(NAVIGATION_MENUS)
      if (g_eeGeneral.currModel != sub) {
        if (eeModelExists(sub)) {
          POPUP_MENU_ADD_ITEM(STR_SELECT_MODEL);
          POPUP_MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
          POPUP_MENU_ADD_ITEM(STR_COPY_MODEL);
          POPUP_MENU_ADD_ITEM(STR_MOVE_MODEL);
          POPUP_MENU_ADD_ITEM(STR_DELETE_MODEL);
        } else {
#if defined(SDCARD)
          POPUP_MENU_ADD_ITEM(STR_CREATE_MODEL);
          POPUP_MENU_ADD_ITEM(STR_RESTORE_MODEL);
#else
          selectModel(sub);
#endif
        }
      } else {
        POPUP_MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
        POPUP_MENU_ADD_ITEM(STR_COPY_MODEL);
        POPUP_MENU_ADD_ITEM(STR_MOVE_MODEL);
      }
      popupMenuHandler = onModelSelectMenu;
#else
      if (g_eeGeneral.currModel != sub) {
        selectModel(sub);
      }
#endif
    } else if (eeModelExists(sub)) {
      s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
      s_copyTgtOfs = 0;
      s_copySrcRow = -1;
    }
    break;

#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_LEFT:
  case EVT_ROTARY_RIGHT:
#endif
  case EVT_KEY_FIRST(KEY_LEFT):
  case EVT_KEY_FIRST(KEY_RIGHT):
#if defined(ROTARY_ENCODER_NAVIGATION)
    if ((!IS_ROTARY_RIGHT(event) && !IS_ROTARY_LEFT(event)) || s_editMode < 0) {
#endif
      if (sub == g_eeGeneral.currModel) {
        chainMenu((IS_ROTARY_RIGHT(event) || event == EVT_KEY_FIRST(KEY_RIGHT)) ? menuModelSetup : menuTabModel[DIM(menuTabModel)-1]);
      } else {
        AUDIO_WARNING2();
      }
      break;
#if defined(ROTARY_ENCODER_NAVIGATION)
    }
    // no break
#endif

  case EVT_KEY_FIRST(KEY_MOVE_UP):
  case EVT_KEY_REPT(KEY_MOVE_UP):
  case EVT_KEY_FIRST(KEY_MOVE_DOWN):
  case EVT_KEY_REPT(KEY_MOVE_DOWN):
    if (s_copyMode) {
      int8_t next_ofs = s_copyTgtOfs + oldSub - menuVerticalPosition;
      if (next_ofs == MAX_MODELS || next_ofs == -MAX_MODELS)
        next_ofs = 0;

      if (s_copySrcRow < 0 && s_copyMode==COPY_MODE) {
        s_copySrcRow = oldSub;
        // find a hole (in the first empty slot above / below)
        sub = eeFindEmptyModel(s_copySrcRow, IS_ROTARY_DOWN(event) || event==EVT_KEY_FIRST(KEY_MOVE_DOWN));
        if (sub < 0) {
          // no free room for duplicating the model
          AUDIO_ERROR();
          sub = oldSub;
          s_copyMode = 0;
        }
        next_ofs = 0;
        menuVerticalPosition = sub;
      }
      s_copyTgtOfs = next_ofs;
    }
    break;
  }

  lcdDrawText(9*FW-(LEN_FREE-4)*FW, 0, STR_FREE);
  if (event) reusableBuffer.modelsel.eepromfree = EeFsGetFree();
  lcdDrawNumberAttUnit(17*FW, 0, reusableBuffer.modelsel.eepromfree, 0);

#if defined(ROTARY_ENCODER_NAVIGATION)
  displayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? ((IS_RE_NAVIGATION_ENABLE() && s_editMode < 0) ? INVERS|BLINK : INVERS) : 0);
#else
  displayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? INVERS : 0);
#endif

  TITLE(STR_MENUMODELSEL);

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;

    lcdDrawNumberNAtt(3*FW+2, y, k+1, LEADING0+((!s_copyMode && sub==k) ? INVERS : 0), 2);

    if (s_copyMode == MOVE_MODE || (s_copyMode == COPY_MODE && s_copySrcRow >= 0)) {
      if (k == sub) {
        if (s_copyMode == COPY_MODE) {
          k = s_copySrcRow;
          lcdDrawChar(MODELSEL_W-FW, y, '+');
        } else {
          k = sub + s_copyTgtOfs;
        }
      } else if (s_copyTgtOfs < 0 && ((k < sub && k >= sub+s_copyTgtOfs) || (k-MAX_MODELS < sub && k-MAX_MODELS >= sub+s_copyTgtOfs)))
        k += 1;
      else if (s_copyTgtOfs > 0 && ((k > sub && k <= sub+s_copyTgtOfs) || (k+MAX_MODELS > sub && k+MAX_MODELS <= sub+s_copyTgtOfs)))
        k += MAX_MODELS-1;
    }

    k %= MAX_MODELS;

    if (eeModelExists(k)) {
      char * name = reusableBuffer.modelsel.listnames[i];
      if (event) eeLoadModelName(k, name);
      putsModelName(4*FW, y, name, k, 0);
      lcdDrawNumberAttUnit(20*FW, y, eeModelSize(k), 0);
      if (k==g_eeGeneral.currModel && (s_copyMode!=COPY_MODE || s_copySrcRow<0 || i+menuVerticalOffset!=(vertpos_t)sub))
        lcdDrawChar(1, y, '*');
    }

    if (s_copyMode && (vertpos_t)sub==i+menuVerticalOffset) {
      lcdDrawFilledRect(9, y, MODELSEL_W-1-9, 7);
      lcdDrawRect(8, y-1, MODELSEL_W-1-7, 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
    }
  }
}
# 1 "gui/menu_model_setup.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"

#define PROTO_IS_SYNC (memproto == protocol)

enum menuModelSetupItems {
  ITEM_MODEL_NAME,
  ITEM_MODEL_TIMER1,
  CASE_PERSISTENT_TIMERS(ITEM_MODEL_TIMER1_PERSISTENT)
  ITEM_MODEL_TIMER1_MINUTE_BEEP,
  ITEM_MODEL_TIMER1_COUNTDOWN_BEEP,
  ITEM_MODEL_TIMER2,
  CASE_PERSISTENT_TIMERS(ITEM_MODEL_TIMER2_PERSISTENT)
  ITEM_MODEL_TIMER2_MINUTE_BEEP,
  ITEM_MODEL_TIMER2_COUNTDOWN_BEEP,
  ITEM_MODEL_EXTENDED_LIMITS,
  ITEM_MODEL_EXTENDED_TRIMS,
  ITEM_MODEL_TRIM_INC,
  ITEM_MODEL_THROTTLE_REVERSED,
  ITEM_MODEL_THROTTLE_TRACE,
  ITEM_MODEL_THROTTLE_SWITCH,
  ITEM_MODEL_THROTTLE_TRIM,
  ITEM_MODEL_THROTTLE_WARNING,
  ITEM_MODEL_SWITCHES_WARNING,
  ITEM_MODEL_BEEP_CENTER,
  ITEM_MODEL_PROTOCOL,
  ITEM_MODEL_PROTOCOL_PARAMS_LINE_1,
//Used with(MULTIMODULE) || (SPIMODULES)
  ITEM_MODEL_PROTOCOL_PARAMS_LINE_2,
  ITEM_MODEL_PROTOCOL_PARAMS_LINE_3,
  ITEM_MODEL_PROTOCOL_PARAMS_LINE_4,
  ITEM_MODEL_PROTOCOL_PARAMS_LINE_5,
//#endif
//Used with(SPIMODULES)
  ITEM_MODEL_PROTOCOL_PARAMS_LINE_6,
  ITEM_MODEL_PROTOCOL_PARAMS_LINE_7,
  ITEM_MODEL_SETUP_MAX

};

#define FIELD_PROTOCOL_MAX 1

#define MODEL_SETUP_2ND_COLUMN        (LCD_W-11*FW-MENUS_SCROLLBAR_WIDTH)
#define MODEL_SETUP_BIND_OFS          2*FW+1
#define MODEL_SETUP_RANGE_OFS         4*FW+3
#define MODEL_SETUP_SET_FAILSAFE_OFS  7*FW-2

void menuModelSetup(uint8_t event)
{
#define CURSOR_ON_CELL         (true)
#define MODEL_SETUP_MAX_LINES  (IS_PPM_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_1+2 : \
  (IS_DSM_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_2+2 : \
  (IS_MULTIMODULE_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_5+2 :  \
  (IS_SPIMODULES_PROTOCOL(protocol)) ? ITEM_MODEL_PROTOCOL_PARAMS_LINE_7+2 :   \
  1

  uint8_t protocol = g_model.rfProtocol;
  uint8_t memproto = protocol;
  MENU_TAB({ 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 2, CASE_PERSISTENT_TIMERS(0) 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, NUM_SWITCHES, NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS-1, FIELD_PROTOCOL_MAX,
             2,2,0,2,0,0,0
           });


  MENU_CHECK(menuTabModel, e_ModelSetup, MODEL_SETUP_MAX_LINES+2);


  TITLE(STR_MENUSETUP);

  uint8_t sub = menuVerticalPosition - 1;
  int8_t editMode = s_editMode;

  for (uint8_t i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;

    LcdFlags blink = ((editMode>0) ? BLINK|INVERS : INVERS);
    LcdFlags attr = (sub == k ? blink : 0);

    switch(k) {
    case ITEM_MODEL_NAME:
      editSingleName(MODEL_SETUP_2ND_COLUMN, y, STR_MODELNAME, g_model.name, sizeof(g_model.name), event, attr);
#if defined(SIMU)
      if ((attr) && (editMode>0)) {
        s_editMode = 0;
        simu_pinl |= 0x10; // Reset Menu key pin
        simu_EditModelName(); //lcd_simu_driver.cpp
      }
#endif
      break;

    case ITEM_MODEL_TIMER1:
    case ITEM_MODEL_TIMER2:
    case ITEM_MODEL_TIMER1_MINUTE_BEEP:
    case ITEM_MODEL_TIMER2_MINUTE_BEEP:
    case ITEM_MODEL_TIMER1_COUNTDOWN_BEEP:
    case ITEM_MODEL_TIMER2_COUNTDOWN_BEEP: {
      TimerData *timer = &g_model.timers[k>=ITEM_MODEL_TIMER2 ? 1 : 0];
      if (k==ITEM_MODEL_TIMER1_MINUTE_BEEP || k==ITEM_MODEL_TIMER2_MINUTE_BEEP) {
        timer->minuteBeep = onoffMenuItem(timer->minuteBeep, MODEL_SETUP_2ND_COLUMN, y, STR_MINUTEBEEP, attr, event);
      } else if (k==ITEM_MODEL_TIMER1_COUNTDOWN_BEEP || k==ITEM_MODEL_TIMER2_COUNTDOWN_BEEP) {
        timer->countdownBeep = onoffMenuItem(timer->countdownBeep, MODEL_SETUP_2ND_COLUMN, y, STR_BEEPCOUNTDOWN, attr, event);
      } else {
        lcdDrawStringWithIndex(0*FW, y, STR_TIMER, k>=ITEM_MODEL_TIMER2 ? 2 : 1);
        lcdPutsTimerMode(MODEL_SETUP_2ND_COLUMN, y, timer->mode, menuHorizontalPosition==0 ? attr : 0);
        putsTimer(MODEL_SETUP_2ND_COLUMN+5*FW-2+5*FWNUM+1, y, timer->start, menuHorizontalPosition==1 ? attr : 0, menuHorizontalPosition==2 ? attr : 0);
        if (attr && (editMode>0 || p1valdiff)) {
          div_t qr = div(timer->start, 60);
          switch (menuHorizontalPosition) {
          case 0:
            CHECK_INCDEC_MODELVAR_CHECK(event, timer->mode, SWSRC_FIRST, TMRMODE_COUNT+SWSRC_LAST-1/*SWSRC_None removed*/, isSwitchAvailableInTimers);
            break;
          case 1:
            CHECK_INCDEC_MODELVAR_ZERO(event, qr.quot, 59);
            timer->start = qr.rem + qr.quot*60;
            break;
          case 2:
            qr.rem -= checkIncDecModel(event, qr.rem+2, 1, 62)-2;
            timer->start -= qr.rem ;
            if ((int16_t)timer->start < 0) timer->start=0;
            if ((int16_t)timer->start > 5999) timer->start=32399; // 8:59:59
            break;
          }
        }
      }
      break;
    }

    case ITEM_MODEL_TIMER1_PERSISTENT:
    case ITEM_MODEL_TIMER2_PERSISTENT: {
      TimerData &timer = g_model.timers[k==ITEM_MODEL_TIMER2_PERSISTENT];
      timer.persistent = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_PERSISTENT, STR_VPERSISTENT, timer.persistent, 0, 2, attr, event);
      break;
    }

    case ITEM_MODEL_EXTENDED_LIMITS:
      ON_OFF_MENU_ITEM(g_model.extendedLimits, MODEL_SETUP_2ND_COLUMN, y, STR_ELIMITS, attr, event);
      break;

    case ITEM_MODEL_EXTENDED_TRIMS:
      ON_OFF_MENU_ITEM(g_model.extendedTrims, MODEL_SETUP_2ND_COLUMN, y, STR_ETRIMS, menuHorizontalPosition<=0 ? attr : 0, event==EVT_KEY_BREAK(KEY_ENTER) ? event : 0);
      lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_RESET_BTN, (menuHorizontalPosition>0  && !NO_HIGHLIGHT()) ? attr : 0);
      if (attr && menuHorizontalPosition>0) {
        s_editMode = 0;
        if (event==EVT_KEY_LONG(KEY_ENTER)) {
          START_NO_HIGHLIGHT();
          for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
            memclear(&g_model.flightModeData[i], TRIMS_ARRAY_SIZE);
          }
          eeDirty(EE_MODEL);
          AUDIO_WARNING1();
        }
      }
      break;

    case ITEM_MODEL_TRIM_INC:
      g_model.trimInc = selectMenuItem(MODEL_SETUP_2ND_COLUMN, y, STR_TRIMINC, STR_VTRIMINC, g_model.trimInc, -2, 2, attr, event);
      break;

    case ITEM_MODEL_THROTTLE_REVERSED:
      ON_OFF_MENU_ITEM(g_model.throttleReversed, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEREVERSE, attr, event ) ;
      break;

    case ITEM_MODEL_THROTTLE_TRACE: {
      lcdDrawTextLeft(y, STR_TTRACE);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrTraceSrc, NUM_POTS+NUM_CHNOUT);
      setGazSource();
      putsMixerSource(MODEL_SETUP_2ND_COLUMN, y, gazSource, attr);
      break;
    }

    case ITEM_MODEL_THROTTLE_SWITCH: {
      lcdDrawTextLeft(y, STR_TSWITCH);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.thrSwitch, NUM_SWITCHES-2);
      uint8_t idx = g_model.thrSwitch;
      if (idx) idx +=3;
      lcdPutsSwitches(MODEL_SETUP_2ND_COLUMN, y, idx, attr);
      break;
    }

    case ITEM_MODEL_THROTTLE_TRIM:
      ON_OFF_MENU_ITEM(g_model.thrTrim, MODEL_SETUP_2ND_COLUMN, y, STR_TTRIM, attr, event);
      break;

    case ITEM_MODEL_THROTTLE_WARNING:
      g_model.disableThrottleWarning = !onoffMenuItem(!g_model.disableThrottleWarning, MODEL_SETUP_2ND_COLUMN, y, STR_THROTTLEWARNING, attr, event);
      break;

    case ITEM_MODEL_SWITCHES_WARNING: {
      lcdDrawTextLeft(y, STR_SWITCHWARNING);
      swarnstate_t states = g_model.switchWarningState;
      char c;
      if (attr) {
        s_editMode = 0;
        if (!READ_ONLY()) {
          switch (event) {
            CASE_EVT_ROTARY_BREAK
          case EVT_KEY_BREAK(KEY_ENTER):
            if (menuHorizontalPosition < NUM_SWITCHES-1) {
              g_model.switchWarningEnable ^= (1 << menuHorizontalPosition);
              eeDirty(EE_MODEL);
            }
            break;

          case EVT_KEY_LONG(KEY_ENTER):
            if (menuHorizontalPosition == NUM_SWITCHES-1) {
              START_NO_HIGHLIGHT();
              getMovedSwitch();
              g_model.switchWarningState = switches_states;
              AUDIO_WARNING1();
              eeDirty(EE_MODEL);
            }
            killEvents(event);
            break;
          }
        }
      }

      LcdFlags line = attr;

      for (uint8_t i=0; i<NUM_SWITCHES-1/*not on TRN switch*/; i++) {
        uint8_t swactive = !(g_model.switchWarningEnable & 1 << i);
        attr = 0;

        if (IS_3POS(i)) {
          c = '0'+(states & 0x03);
          states >>= 2;
        } else {
          if ((states & 0x01) && swactive)
            attr = INVERS;
          c = pgm_read_byte_near(STR_VSWITCHES - 2 + 9 + (3*(i+1)));
          states >>= 1;
        }
        if (line && (menuHorizontalPosition == i)) {
          attr = BLINK;
          if (swactive)
            attr |= INVERS;
        }
        lcdDrawCharAtt(MODEL_SETUP_2ND_COLUMN+i*FW, y, (swactive || (attr & BLINK)) ? c : '-', attr);
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+(NUM_SWITCHES*FW), y, PSTR("<]"), (menuHorizontalPosition == NUM_SWITCHES-1 && !NO_HIGHLIGHT()) ? line : 0);
      }
      break;
    }

    case ITEM_MODEL_BEEP_CENTER:
      lcdDrawTextLeft(y, STR_BEEPCTR);
      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS; i++) {
        // TODO flash saving, \001 not needed in STR_RETA123
        coord_t x = MODEL_SETUP_2ND_COLUMN+i*FW;
        lcdDrawTextAtIndex(x, y, STR_RETA123, i, ((menuHorizontalPosition==i) && attr) ? BLINK|INVERS : (((g_model.beepANACenter & ((BeepANACenter)1<<i)) || (attr && CURSOR_ON_LINE())) ? INVERS : 0 ) );
      }
      if (attr && CURSOR_ON_CELL) {
        if (event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff) {
          if (READ_ONLY_UNLOCKED()) {
            s_editMode = 0;
            g_model.beepANACenter ^= ((BeepANACenter)1<<menuHorizontalPosition);
            eeDirty(EE_MODEL);
          }
        }
      }
      break;

    int16_t tmp; // used for CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE

    case ITEM_MODEL_PROTOCOL:
      lcdDrawTextLeft(y, NO_INDENT(STR_PROTO));
      if IS_SPIMODULES_PROTOCOL(protocol) {
#if defined(SPIMODULES)
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN, y, STR_SPIM, menuHorizontalPosition<=0 ? attr : 0);
#endif
      } else {
        lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN, y, Protos[g_model.rfProtocol].ProtoName, menuHorizontalPosition<=0 ? attr : 0);
      }
      if (IS_PPM_PROTOCOL(protocol)) {
        lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+7*FW, y, STR_NCHANNELS, g_model.PPMNCH, menuHorizontalPosition!=0 ? attr : 0);
      } else if (menuHorizontalPosition>0 && attr) {
        MOVE_CURSOR_FROM_HERE();
      }
      if (attr && (editMode>0 || p1valdiff)) {
        switch (menuHorizontalPosition) {
        case 0:
#if defined(SPIMODULES)
          if IS_SPIMODULES_PROTOCOL(protocol) {
            uint8_t prototemp = LASTPROTOMENU1;
            CHECK_INCDEC_MODELVAR_ZERO(event, prototemp, LASTPROTOMENU1);
            if (prototemp < LASTPROTOMENU1) {
              protocol = prototemp;
            }
          } else
#endif
          {
            CHECK_INCDEC_MODELVAR_ZERO(event, protocol, LASTPROTOMENU1);
          }
          break;
        case 1:
          CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.PPMNCH, protocol!=(PROTOCOL_PPM16-1) ? 6 : 2); //limit 8 channels for PPMSim and PPM16
          g_model.PPMFRAMELENGTH = (g_model.PPMNCH-2) * 8;
          break;
        }
      }
      break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_1:
      if PROTO_IS_SYNC {
      if (IS_PPM_PROTOCOL(protocol))
        {
          lcdDrawTextLeft(y, STR_PPMFRAME);
          lcdDrawText(MODEL_SETUP_2ND_COLUMN+3*FW, y, STR_MS);
          lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN, y, (int16_t)g_model.PPMFRAMELENGTH*5 + 225, (menuHorizontalPosition<=0 ? attr : 0) | PREC1|LEFT);
          lcdDrawChar(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, 'u');
          lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN+8*FW+2, y, (g_model.PPMDELAY*50)+300, (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
          lcdDrawCharAtt(MODEL_SETUP_2ND_COLUMN+10*FW, y, g_model.PULSEPOL ? '+' : '-', (CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0);
          if (attr && (editMode>0 || p1valdiff)) {
            switch (menuHorizontalPosition) {
            case 0:
              CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE(event, g_model.PPMFRAMELENGTH, -20, 35);
              break;
            case 1:
              CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE(event, g_model.PPMDELAY, -4, 10);
              break;
            case 2:
              CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.PULSEPOL, 1);
              break;
            }
          }
        }
#if defined(DSM2)
        if (IS_DSM_PROTOCOL(protocol))
        {
          lcdDrawTextLeft(y, STR_TYPE);
          lcdDrawSizedTextAtt(MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfSubTypeNames+4*g_model.rfSubType, 4, menuHorizontalPosition == 0 ? attr : 0);
          if (attr && (editMode>0 || p1valdiff)) {
            CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.rfSubType, RfOptionSettings.rfSubTypeMax);
          }
        }
#endif
#if defined(SPIMODULES)
        if IS_SPIMODULES_PROTOCOL(protocol)
        {
          lcdDrawTextLeft(y, NO_INDENT(STR_TYPE));
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN-5*FW, y, Protos[g_model.rfProtocol].ProtoName, menuHorizontalPosition == 0 ? attr : 0);
          if (RfOptionSettings.rfSubTypeMax) { // Check if Subtype exist
            lcdDrawSizedTextAtt(MODEL_SETUP_2ND_COLUMN+4*FW, y, RfOptionSettings.rfSubTypeNames+4*g_model.rfSubType, 4, menuHorizontalPosition == 1 ? attr : 0);
          }
          if (attr  && (editMode>0)) {

            switch (menuHorizontalPosition) {
            case 0: {
              CHECK_INCDEC_MODELVAR(event, protocol, LASTPROTOMENU1, PROTOCOL_COUNT-2);
              break;
            }
            case 1: {
              if (RfOptionSettings.rfSubTypeMax) {
                CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, g_model.rfSubType, RfOptionSettings.rfSubTypeMax); // TODO SUBTYPE
              } else break;
            }
            break;
            }
          }
        }
#endif
#if defined(MULTIMODULE)
        else if IS_MULTIMODULE_PROTOCOL(protocol)
        {
          uint8_t multi_rfProto = g_model.MULTIRFPROTOCOL;

          lcdDrawTextLeft(y, NO_INDENT(STR_TYPE));
          if(g_model.CUSTOMPROTO) {
            lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN-5*FW, y, STR_MULTI_CUSTOM, menuHorizontalPosition==0 ? attr : 0);
          } else {
            lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN-5*FW, y, STR_MULTIPROTOCOLS, multi_rfProto, menuHorizontalPosition==0 ? attr : 0);
          }
          const mm_protocol_definition *pdef = getMultiProtocolDefinition(multi_rfProto);
          if(g_model.CUSTOMPROTO) {
            lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 3 * FW, y, g_model.MULTIRFPROTOCOL, (menuHorizontalPosition == 1 ? attr : 0));
            lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfSubType, (menuHorizontalPosition == 2 ? attr : 0));
          } else {
            if (pdef->subTypeString != 0)
              lcdDrawTextAtIndex(MODEL_SETUP_2ND_COLUMN+ 2 * FW, y, pdef->subTypeString, g_model.rfSubType, (menuHorizontalPosition == 1 ? attr : 0));
          }

          if (attr  && (editMode>0)) {

            switch (menuHorizontalPosition) {
            case 0: {
              int8_t multiRfProto = g_model.CUSTOMPROTO == true ? MM_RF_PROTO_CUSTOM : g_model.MULTIRFPROTOCOL;
              CHECK_INCDEC_MODELVAR(event, multiRfProto, MM_RF_PROTO_FIRST, MM_RF_PROTO_LAST);
              if (checkIncDec_Ret) {
                g_model.CUSTOMPROTO = (multiRfProto == MM_RF_PROTO_CUSTOM);
                if (!g_model.CUSTOMPROTO)
                  g_model.MULTIRFPROTOCOL = multiRfProto;
                g_model.rfSubType = 0;
                if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2) {
                  g_model.AUTOBINDMODE = 1;
                } else {
                  g_model.AUTOBINDMODE = 0;
                }
                g_model.rfOptionValue2 = 0;
              }
            }
            break;
            case 1: {
              if (g_model.CUSTOMPROTO) {
                CHECK_INCDEC_MODELVAR_ZERO(event, multi_rfProto, MULTI_RF_PROTO_LAST);
                g_model.MULTIRFPROTOCOL = multi_rfProto;
              } else if (pdef->maxSubtype > 0)
                CHECK_INCDEC_MODELVAR(event, g_model.rfSubType, 0, pdef->maxSubtype);

            }
            break;
            case 2:
              // Custom protocol, third column is subtype
              CHECK_INCDEC_MODELVAR(event, g_model.rfSubType, 0, 7);
              break;

            }
          }
        }
#endif
      }
      break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_2:
      if PROTO_IS_SYNC {
#if defined(DSM2)
      if (IS_DSM_PROTOCOL(protocol))
        {
          horzpos_t l_posHorz = menuHorizontalPosition;
          lcdDrawTextLeft(y, STR_RXNUM);
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (xOffsetBind) lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
          if (attr && l_posHorz==0) {
            if (editMode>0 || p1valdiff) {
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, MAX_MODELS);
            }
          }
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

          if (attr && l_posHorz>0 && s_editMode>0) {
            if (l_posHorz == 1) {
              PROTOCOL_SetBindState(1000); // 10 Sec
            } else if (l_posHorz == 2) {
              rangeModeIsOn = true;
            }
          }
        }
#endif
#if defined(MULTIMODULE)
        if IS_MULTIMODULE_PROTOCOL(protocol)
        {
          horzpos_t l_posHorz = menuHorizontalPosition;
          lcdDrawTextLeft(y, STR_RXNUM);
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (xOffsetBind) lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
          if (attr && l_posHorz==0) {
            if (editMode>0 || p1valdiff) {
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, 15);
            }
          }
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

          if (attr && l_posHorz>0 && s_editMode>0) {
            if (l_posHorz == 1)
              PROTOCOL_SetBindState(500); // 5 Sec
            else if (l_posHorz == 2) {
              rangeModeIsOn = true;
            }
          }
        }
#endif
#if defined(SPIMODULES)
        if IS_SPIMODULES_PROTOCOL(protocol)
        {
          horzpos_t l_posHorz = menuHorizontalPosition;
          lcdDrawTextLeft(y, STR_RXNUM);
          coord_t xOffsetBind = MODEL_SETUP_BIND_OFS;
          if (xOffsetBind) lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 1 * FW, y, g_model.modelId, (l_posHorz==0 ? attr : 0));
          if (attr && l_posHorz==0) {
            if (editMode>0 || p1valdiff) {
              CHECK_INCDEC_MODELVAR_ZERO(event, g_model.modelId, MAX_MODELS);
            }
          }
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+xOffsetBind, y, STR_MODULE_BIND, l_posHorz==1 ? attr : 0);
          lcdDrawTextAtt(MODEL_SETUP_2ND_COLUMN+MODEL_SETUP_RANGE_OFS+xOffsetBind, y, STR_MODULE_RANGE, l_posHorz==2 ? attr : 0);

          if (attr && l_posHorz>0 && s_editMode>0) {
            if (l_posHorz == 1) {
              if (RFModule.mode != BIND_MODE) startPulses(PROTOCMD_BIND);
              PROTOCOL_SetBindState(1000); // 10 Sec
            } else if (l_posHorz == 2) {
              rangeModeIsOn = true;
            }
          }
        }
#endif
      }
      break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_3:
      if PROTO_IS_SYNC {
#if defined(MULTIMODULE)
      if IS_MULTIMODULE_PROTOCOL(protocol)
        {
          if (g_model.MULTIRFPROTOCOL == MM_RF_PROTO_DSM2)
            ON_OFF_MENU_ITEM(g_model.AUTOBINDMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_DSM_AUTODTECT, attr, event);
          else
            ON_OFF_MENU_ITEM(g_model.AUTOBINDMODE, MODEL_SETUP_2ND_COLUMN, y, STR_AUTOBIND, attr, event);
        }
#endif
#if defined(SPIMODULES)
        if IS_SPIMODULES_PROTOCOL(protocol)
        {
          if (RfOptionSettings.rfOptionValue1Max != RfOptionSettings.rfOptionValue1Min) {
            lcdDrawTextLeft(y, RfOptionSettings.rfOptionValue1Name);
            lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfOptionValue1, attr);
            if (attr) {
              CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue1, RfOptionSettings.rfOptionValue1Min, RfOptionSettings.rfOptionValue1Max);
            }
          } else {
            lcdDrawTextLeft(y,STR_DUMMY);
          }
        }
#endif
      }
      break;
    case  ITEM_MODEL_PROTOCOL_PARAMS_LINE_4: //Option1 for SPIMOD
      if PROTO_IS_SYNC {
#if defined(MULTIMODULE)
      if IS_MULTIMODULE_PROTOCOL(protocol)
        {
          ON_OFF_MENU_ITEM(g_model.LOWPOWERMODE, MODEL_SETUP_2ND_COLUMN, y, STR_MULTI_LOWPOWER, attr, event);
        }
#endif
#if defined(SPIMODULES)
        if IS_SPIMODULES_PROTOCOL(protocol)
        {
          if (RfOptionSettings.rfOptionValue2Max != RfOptionSettings.rfOptionValue2Min) {
            lcdDrawTextLeft(y, RfOptionSettings.rfOptionValue2Name);
            lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 5 * FW, y, g_model.rfOptionValue2, attr);
            if (attr) {
              CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, RfOptionSettings.rfOptionValue2Min, RfOptionSettings.rfOptionValue2Max);
            }
          } else {
            lcdDrawTextLeft(y,STR_DUMMY);
          }
        }
#endif
      }
      break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_5:
      if PROTO_IS_SYNC {
#if defined(MULTIMODULE)
      if IS_MULTIMODULE_PROTOCOL(protocol)
        {
          int8_t optionValue =  g_model.rfOptionValue2;
          const uint8_t multi_proto = g_model.MULTIRFPROTOCOL;
          const mm_protocol_definition* pdef = getMultiProtocolDefinition(multi_proto);
          if (pdef->optionsstr) {
            lcdDrawTextLeft(y, pdef->optionsstr);
            if (multi_proto == MM_RF_PROTO_FS_AFHDS2A) optionValue = 50 + 5 * optionValue;
            lcdDrawNumberAttUnit(MODEL_SETUP_2ND_COLUMN + 2 * FW, y, optionValue, attr);
            if (attr) {
              if (multi_proto == MM_RF_PROTO_FS_AFHDS2A) {
                CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, 0, 70);
              } else if (multi_proto == MM_RF_PROTO_OLRS) {
                CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, -1, 7);
              } else {
                CHECK_INCDEC_MODELVAR(event, g_model.rfOptionValue2, -128, 127);
              }
            }
          }
        }
#endif
#if defined(SPIMODULES)
        if IS_SPIMODULES_PROTOCOL(protocol)
        {
          if (RfOptionSettings.rfOptionBool1Used) {
            ON_OFF_MENU_ITEM(g_model.rfOptionBool1, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool1Name, attr, event);
          } else {
            lcdDrawTextLeft(y,STR_DUMMY);
          }
        }
#endif
      }
      break;
#if defined(SPIMODULES)
    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_6:
      if PROTO_IS_SYNC {
      if IS_SPIMODULES_PROTOCOL(protocol)
        {
          if (RfOptionSettings.rfOptionBool2Used) {
            ON_OFF_MENU_ITEM(g_model.rfOptionBool2, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool2Name, attr, event);
          } else {
            lcdDrawTextLeft(y,STR_DUMMY);
          }
        }
      }
      break;

    case ITEM_MODEL_PROTOCOL_PARAMS_LINE_7:
      if PROTO_IS_SYNC {
      if IS_SPIMODULES_PROTOCOL(protocol)
        {
          if (RfOptionSettings.rfOptionBool3Used) {
            ON_OFF_MENU_ITEM(g_model.rfOptionBool3, MODEL_SETUP_2ND_COLUMN, y, RfOptionSettings.rfOptionBool3Name, attr, event);
          } else {
            lcdDrawTextLeft(y,STR_DUMMY);
          }
        }
      }
      break;
#endif
    }
  }

  if (!PROTO_IS_SYNC) {
    flightReset();
    rangeModeIsOn = false;
    g_model.rfProtocol = protocol;
    RFModule.mode = NORMAL_MODE;
    startPulses(PROTOCMD_INIT);
  }
}
# 1 "gui/menu_model_inputs_mixes.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"
#include "widgets.h"


#define EXPO_ONE_2ND_COLUMN (7*FW+3*FW+2)
#define EXPO_ONE_FM_WIDTH   (5*FW)

#if defined(FLIGHT_MODES)
void displayFlightModes(coord_t x, coord_t y, FlightModesType value);
FlightModesType editFlightModes(coord_t x, coord_t y, uint8_t event, FlightModesType value, uint8_t attr)
{
  lcdDrawTextColumnLeft(x, y, STR_FLMODE);

  uint8_t posHorz = menuHorizontalPosition;

  for (uint8_t p=0; p<MAX_FLIGHT_MODES; p++) {
    lcdDrawCharAtt(x, y, '0'+p, ((posHorz==p) && attr) ? BLINK|INVERS : ((value & (1<<p)) ? 0 : INVERS));
    x += FW;
  }

  if (attr) {
    if (s_editMode && ((event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff))) {
      s_editMode = 0;
      value ^= (1<<posHorz);
      eeDirty(EE_MODEL);
    }
  }

  return value;
}
#else
#define displayFlightModes(...)
#endif

int16_t expoFn(int16_t x)
{
  ExpoData *ed = expoAddress(s_currIdx);
  int16_t anas[NUM_INPUTS] = {0};
  anas[ed->chn] = x;
  applyExpos(anas, e_perout_mode_inactive_flight_mode);
  return anas[ed->chn];
}

void DrawFunction(FnFuncP fn, uint8_t offset)
{
  lcdDrawSolidVerticalLineStip(X0-offset, 0, LCD_H, 0xee);
  lcdDrawSolidHorizontalLineStip(X0-WCHART-offset, Y0, WCHART*2, 0xee);

  coord_t prev_yv = (coord_t)-1;

  for (int8_t xv=-WCHART; xv<=WCHART; xv++) {
    coord_t yv = (LCD_H-1) - (((uint16_t)RESX + fn(xv * (RESX/WCHART))) / 2 * (LCD_H-1) / RESX);
    if (prev_yv != (coord_t)-1) {
      if (abs((int8_t)yv-prev_yv) <= 1) {
        lcdDrawPoint(X0+xv-offset-1, prev_yv, FORCE);
      } else {
        uint8_t tmp = (prev_yv < yv ? 0 : 1);
        lcdDrawSolidVerticalLine(X0+xv-offset-1, yv+tmp, prev_yv-yv);
      }
    }
    prev_yv = yv;
  }
}


uint8_t getExpoMixCount(uint8_t expo)
{
  uint8_t count = 0;

  for(int8_t i=(expo ? MAX_EXPOS-1 : MAX_MIXERS-1); i>=0; i--) {
    uint8_t ch = (expo ? EXPO_VALID(expoAddress(i)) : mixAddress(i)->srcRaw);
    if (ch != 0) {
      count++;
    }
  }
  return count;
}

bool reachExpoMixCountLimit(uint8_t expo)
{
  // check mixers count limit
  if (getExpoMixCount(expo) >= (expo ? MAX_EXPOS : MAX_MIXERS)) {
    POPUP_WARNING(expo ? STR_NOFREEEXPO : STR_NOFREEMIXER);
    return true;
  }
  return false;
}

void deleteExpoMix(uint8_t expo, uint8_t idx)
{
  if (expo) {
    ExpoData *expo = expoAddress(idx);
    memmove(expo, expo+1, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(&g_model.expoData[MAX_EXPOS-1], sizeof(ExpoData));
  } else {
    MixData *mix = mixAddress(idx);
    memmove(mix, mix+1, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(&g_model.mixData[MAX_MIXERS-1], sizeof(MixData));
  }
  eeDirty(EE_MODEL);
}

int8_t s_currCh;
void insertExpoMix(uint8_t expo, uint8_t idx)
{

  if (expo) {
    ExpoData *expo = expoAddress(idx);
    memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
    memclear(expo, sizeof(ExpoData));
    expo->mode = 3; // pos&neg
    expo->chn = s_currCh - 1;
    expo->weight = 100;
  } else {
    MixData *mix = mixAddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
    memclear(mix, sizeof(MixData));
    mix->destCh = s_currCh-1;
    mix->srcRaw = (s_currCh > 4 ? MIXSRC_Rud - 1 + s_currCh : MIXSRC_Rud - 1 + channel_order(s_currCh));
    mix->weight = 100;
  }
  eeDirty(EE_MODEL);
}

void copyExpoMix(uint8_t expo, uint8_t idx)
{
  if (expo) {
    ExpoData *expo = expoAddress(idx);
    memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData));
  } else {
    MixData *mix = mixAddress(idx);
    memmove(mix+1, mix, (MAX_MIXERS-(idx+1))*sizeof(MixData));
  }
  eeDirty(EE_MODEL);
}

void memswap(void *a, void *b, uint8_t size)
{
  uint8_t *x = (uint8_t*)a;
  uint8_t *y = (uint8_t*)b;

  while (size--) {
    uint8_t temp = *x;
    *x++ = *y;
    *y++ = temp;
  }
}

bool swapExpoMix(uint8_t expo, uint8_t &idx, uint8_t up)
{
  void *x, *y;
  uint8_t size;
  int8_t tgt_idx = (up ? idx-1 : idx+1);

  if (expo) {
    x = (ExpoData *)expoAddress(idx);

    if (tgt_idx < 0) {
      if (((ExpoData *)x)->chn == 0)
        return false;
      ((ExpoData *)x)->chn--;
      return true;
    }

    if (tgt_idx == MAX_EXPOS) {
      if (((ExpoData *)x)->chn == NUM_INPUTS-1)
        return false;
      ((ExpoData *)x)->chn++;
      return true;
    }

    y = (ExpoData *)expoAddress(tgt_idx);
    if(((ExpoData *)x)->chn != ((ExpoData *)y)->chn || !EXPO_VALID((ExpoData *)y)) {
      if (up) {
        if (((ExpoData *)x)->chn>0) ((ExpoData *)x)->chn--;
        else return false;
      } else {
        if (((ExpoData *)x)->chn<NUM_INPUTS-1) ((ExpoData *)x)->chn++;
        else return false;
      }
      return true;
    }

    size = sizeof(ExpoData);
  } else {
    x = (MixData *)mixAddress(idx);

    if (tgt_idx < 0) {
      if (((MixData *)x)->destCh == 0)
        return false;
      ((MixData *)x)->destCh--;
      return true;
    }

    if (tgt_idx == MAX_MIXERS) {
      if (((MixData *)x)->destCh == NUM_CHNOUT-1)
        return false;
      ((MixData *)x)->destCh++;
      return true;
    }

    y = (MixData *)mixAddress(tgt_idx);
    uint8_t destCh = ((MixData *)x)->destCh;
    if(!((MixData *)y)->srcRaw || destCh != ((MixData *)y)->destCh) {
      if (up) {
        if (destCh>0) ((MixData *)x)->destCh--;
        else return false;
      } else {
        if (destCh<NUM_CHNOUT-1) ((MixData *)x)->destCh++;
        else return false;
      }
      return true;
    }

    size = sizeof(MixData);
  }
  memswap(x, y, size);
  idx = tgt_idx;
  return true;
}

enum ExposFields {
  EXPO_FIELD_WEIGHT,
  EXPO_FIELD_EXPO,
  CASE_CURVES(EXPO_FIELD_CURVE)
  CASE_FLIGHT_MODES(EXPO_FIELD_FLIGHT_MODES)
  EXPO_FIELD_SWITCH,
  EXPO_FIELD_SIDE,
  EXPO_FIELD_MAX
};

#define CURVE_ROWS 0

void menuModelExpoOne(uint8_t event)
{
  ExpoData *ed = expoAddress(s_currIdx);
  putsMixerSource(7*FW+FW/2, 0, MIXSRC_Rud+ed->chn, 0);

  SUBMENU(STR_MENUINPUTS, EXPO_FIELD_MAX, {0, 0, CASE_CURVES(CURVE_ROWS) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0 /*, ...*/});

  SET_SCROLLBAR_X(EXPO_ONE_2ND_COLUMN+10*FW);

  int8_t sub = menuVerticalPosition;

  coord_t y = MENU_HEADER_HEIGHT + 1;

  for (uint8_t i=0; i<EXPO_FIELD_MAX+1; i++) {
    uint8_t attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {

    case EXPO_FIELD_WEIGHT:
      lcdDrawTextLeft(y, STR_WEIGHT);
      ed->weight = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr, 0, event);
      break;

    case EXPO_FIELD_EXPO:
      lcdDrawTextLeft(y, STR_EXPO);
      if (ed->curveMode==MODE_EXPO || ed->curveParam==0) {
        ed->curveMode = MODE_EXPO;
        ed->curveParam = GVAR_MENU_ITEM(EXPO_ONE_2ND_COLUMN, y, ed->curveParam, -100, 100, attr, 0, event);
      } else {
        lcdDrawTextAtt(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_NA, attr);
      }
      break;

#if defined(CURVES)
    case EXPO_FIELD_CURVE:
      lcdDrawTextLeft(y, STR_CURVE);
      if (ed->curveMode!=MODE_EXPO || ed->curveParam==0) {
        lcdDrawCurveName(EXPO_ONE_2ND_COLUMN-3*FW, y, ed->curveParam, attr);
        if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, ed->curveParam, CURVE_BASE+MAX_CURVES-1);
          if (ed->curveParam) ed->curveMode = MODE_CURVE;
          if (ed->curveParam>=CURVE_BASE && event==EVT_KEY_LONG(KEY_ENTER)) {
            s_curveChan = ed->curveParam - CURVE_BASE;
            pushMenu(menuModelCurveOne);
          }
        }
      } else {
        lcdDrawTextAtt(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_NA, attr);
      }
      break;
#endif

#if defined(FLIGHT_MODES)
    case EXPO_FIELD_FLIGHT_MODES:
      ed->flightModes = editFlightModes(EXPO_ONE_2ND_COLUMN-EXPO_ONE_FM_WIDTH, y, event, ed->flightModes, attr);
      break;
#endif

    case EXPO_FIELD_SWITCH:
      ed->swtch = switchMenuItem(EXPO_ONE_2ND_COLUMN-3*FW, y, ed->swtch, attr, event);
      break;

    case EXPO_FIELD_SIDE:
      ed->mode = 4 - selectMenuItem(EXPO_ONE_2ND_COLUMN-3*FW, y, STR_SIDE, STR_VSIDE, 4-ed->mode, 1, 3, attr, event);
      break;
    }
    y += FH;
  }

  DrawFunction(expoFn);

  int16_t x512 = calibratedStick[ed->chn];
  lcdDrawNumberAttUnit(LCD_W-8, 6*FH, calcRESXto100(x512), 0);
  int16_t y512 = expoFn(x512);
  lcdDrawNumberAttUnit(LCD_W-8-6*FW, 1*FH, calcRESXto100(y512), 0);

  x512 = X0+x512/(RESXu/WCHART);
  y512 = (LCD_H-1) - (uint16_t)((y512+RESX)/2) * (LCD_H-1) / RESX;

  lcdDrawSolidVerticalLine(x512, y512-3, 3*2+1);
  lcdDrawSolidHorizontalLine(x512-3, y512, 3*2+1);
}

enum MixFields {
  MIX_FIELD_SOURCE,
  MIX_FIELD_WEIGHT,
  MIX_FIELD_OFFSET,
  MIX_FIELD_TRIM,
  CASE_CURVES(MIX_FIELD_CURVE)
  CASE_FLIGHT_MODES(MIX_FIELD_FLIGHT_PHASE)
  MIX_FIELD_SWITCH,
  MIX_FIELD_WARNING,
  MIX_FIELD_MLTPX,
  MIX_FIELD_DELAY_UP,
  MIX_FIELD_DELAY_DOWN,
  MIX_FIELD_SLOW_UP,
  MIX_FIELD_SLOW_DOWN,
  MIX_FIELD_COUNT
};

void gvarWeightItem(coord_t x, coord_t y, MixData *md, uint8_t attr, uint8_t event)
{
  u_int8int16_t weight;
  MD_WEIGHT_TO_UNION(md, weight);
  weight.gvword = GVAR_MENU_ITEM(x, y, weight.gvword, GV_RANGELARGE_WEIGHT_NEG, GV_RANGELARGE_WEIGHT, attr, 0, event);
  MD_UNION_TO_WEIGHT(weight, md);
}

#define GAUGE_WIDTH  33
#define GAUGE_HEIGHT 6
void drawOffsetBar(uint8_t x, uint8_t y, MixData * md)
{
  int offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode);
  int weight = abs(GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, mixerCurrentFlightMode));
  int barMin = 0;
  int barMax = 0;

#if defined(OFFSET_ON_INPUT)
  //Offset on input (before weight)
  barMin = (-100 + offset) * weight / 100;
  barMax = (+100 + offset) * weight / 100;

#else
  //Offset on output (after weight)
  barMin = offset - weight;
  barMax = offset + weight;

#endif
  if (y > 15) {
    lcdDrawNumberAttUnit(x-((barMin >= 0) ? 2 : 3), y-8, barMin, LEFT);
    lcdDrawNumberAttUnit(x+GAUGE_WIDTH+1, y-8, barMax);
  }
  if (barMin < -101)
    barMin = -101;
  if (barMax > 101)
    barMax = 101;
  lcdDrawSolidHorizontalLineStip(x-2, y, GAUGE_WIDTH+2, DOTTED);
  lcdDrawSolidHorizontalLineStip(x-2, y+GAUGE_HEIGHT, GAUGE_WIDTH+2, DOTTED);
  lcdDrawSolidVerticalLine(x-2, y+1, GAUGE_HEIGHT-1);
  lcdDrawSolidVerticalLine(x+GAUGE_WIDTH-1, y+1, GAUGE_HEIGHT-1);
  if (barMin <= barMax) {
    int8_t right = (barMax * GAUGE_WIDTH) / 200;
    int8_t left = ((barMin * GAUGE_WIDTH) / 200)-1;
    lcdDrawFilledRect(x+GAUGE_WIDTH/2+left, y+2, right-left, GAUGE_HEIGHT-3);
  }
  lcdDrawSolidVerticalLine(x+GAUGE_WIDTH/2-1, y, GAUGE_HEIGHT+1);
  if (barMin == -101) {
    for (uint8_t i=0; i<3; ++i) {
      lcdDrawPoint(x+i, y+4-i);
      lcdDrawPoint(x+3+i, y+4-i);
    }
  }
  if (barMax == 101) {
    for (uint8_t i=0; i<3; ++i) {
      lcdDrawPoint(x+GAUGE_WIDTH-8+i, y+4-i);
      lcdDrawPoint(x+GAUGE_WIDTH-5+i, y+4-i);
    }
  }
}
#undef GAUGE_WIDTH
#undef GAUGE_HEIGHT

void menuModelMixOne(uint8_t event)
{
  TITLE(s_currCh ? STR_INSERTMIX : STR_EDITMIX);
  MixData *md2 = mixAddress(s_currIdx) ;
  putsChn(lcdLastPos+1*FW, 0, md2->destCh+1,0);

#if defined(ROTARY_ENCODERS)
#if defined(CURVES)
  if ((menuVerticalPosition == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS) || (menuVerticalPosition == MIX_FIELD_CURVE && md2->curveMode == MODE_CURVE))
#else
  if (menuVerticalPosition == MIX_FIELD_TRIM && md2->srcRaw > NUM_STICKS)
#endif
    SUBMENU_NOTITLE(MIX_FIELD_COUNT, {0, 0, 0, 0, CASE_CURVES(0) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/})
    else
      SUBMENU_NOTITLE(MIX_FIELD_COUNT, {0, 0, 0, 1, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/});
#else
  SUBMENU_NOTITLE(MIX_FIELD_COUNT, {0, 0, 0, 1, CASE_CURVES(1) CASE_FLIGHT_MODES((MAX_FLIGHT_MODES-1) | NAVIGATION_LINE_BY_LINE) 0, 0 /*, ...*/});
#endif

#if MENU_COLUMNS > 1
  lcdDrawSolidVerticalLine(MENU_COLUMN2_X-4, FH+1, LCD_H-FH-1);
#endif

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

  for (uint8_t k=0; k<MENU_COLUMNS*(LCD_LINES-1); k++) {

#if MENU_COLUMNS > 1
    coord_t y;
    coord_t COLUMN_X;
    if (k >= LCD_LINES-1) {
      y = 1 + (k-LCD_LINES+2)*FH;
      COLUMN_X = MENU_COLUMN2_X;
    } else {
      y = 1 + (k+1)*FH;
      COLUMN_X = 0;
    }
    int8_t i = k;
#else
    coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
    int8_t i = k + menuVerticalOffset;
#endif

    uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
    switch(i) {

    case MIX_FIELD_SOURCE:
      lcdDrawTextColumnLeft(COLUMN_X, y, NO_INDENT(STR_SOURCE));
      putsMixerSource(COLUMN_X+MIXES_2ND_COLUMN, y, md2->srcRaw, STREXPANDED|attr);
      if (attr) CHECK_INCDEC_MODELSOURCE(event, md2->srcRaw, 1, MIXSRC_LAST);
      break;
    case MIX_FIELD_WEIGHT:
      lcdDrawTextColumnLeft(COLUMN_X, y, STR_WEIGHT);
      gvarWeightItem(COLUMN_X+MIXES_2ND_COLUMN, y, md2, attr|LEFT, event);
      break;
    case MIX_FIELD_OFFSET: {
      lcdDrawTextColumnLeft(COLUMN_X, y, NO_INDENT(STR_OFFSET));
      u_int8int16_t offset;
      MD_OFFSET_TO_UNION(md2, offset);
      offset.gvword = GVAR_MENU_ITEM(COLUMN_X+MIXES_2ND_COLUMN, y, offset.gvword, GV_RANGELARGE_OFFSET_NEG, GV_RANGELARGE_OFFSET, attr|LEFT, 0, event);
      MD_UNION_TO_OFFSET(offset, md2);
      drawOffsetBar(COLUMN_X+MIXES_2ND_COLUMN+22, y, md2);
      break;
    }
    case MIX_FIELD_TRIM: {
      uint8_t not_stick = (md2->srcRaw > NUM_STICKS);
      int8_t carryTrim = -md2->carryTrim;
      lcdDrawTextColumnLeft(COLUMN_X, y, STR_TRIM);
      lcdDrawTextAtIndex((not_stick ? COLUMN_X+MIXES_2ND_COLUMN : COLUMN_X+6*FW-3), y, STR_VMIXTRIMS, (not_stick && carryTrim == 0) ? 0 : carryTrim+1, menuHorizontalPosition==0 ? attr : 0);
      if (attr && menuHorizontalPosition==0 && (not_stick || editMode>0)) md2->carryTrim = -checkIncDecModel(event, carryTrim, not_stick ? TRIM_ON : -TRIM_OFF, -TRIM_AIL);
      if (!not_stick) {
        lcdDrawText(COLUMN_X+MIXES_2ND_COLUMN, y, STR_DREX);
        menu_lcd_onoff(COLUMN_X+MIXES_2ND_COLUMN+DREX_CHBOX_OFFSET, y, !md2->noExpo, menuHorizontalPosition==1 ? attr : 0);
        if (attr && menuHorizontalPosition==1 && editMode>0) md2->noExpo = !checkIncDecModel(event, !md2->noExpo, 0, 1);
      } else if (attr) {
        REPEAT_LAST_CURSOR_MOVE();
      }
      break;
    }

#if defined(CURVES)
    case MIX_FIELD_CURVE: {
      lcdDrawTextColumnLeft(COLUMN_X, y, STR_CURVE);
      int8_t curveParam = md2->curveParam;
      if (md2->curveMode == MODE_CURVE) {
        lcdDrawCurveName(COLUMN_X+MIXES_2ND_COLUMN, y, curveParam, attr);
        if (attr) {
          if (event==EVT_KEY_LONG(KEY_ENTER) && (curveParam<0 || curveParam>=CURVE_BASE)) {
            s_curveChan = (curveParam<0 ? -curveParam-1 : curveParam-CURVE_BASE);
            pushMenu(menuModelCurveOne);
          } else {
            CHECK_INCDEC_MODELVAR(event, md2->curveParam, -MAX_CURVES, CURVE_BASE+MAX_CURVES-1);
            if (md2->curveParam == 0)
              md2->curveMode = MODE_DIFFERENTIAL;
          }
        }
      } else {
        lcdDrawTextAtt(COLUMN_X+MIXES_2ND_COLUMN, y, PSTR("Diff"), menuHorizontalPosition==0 ? attr : 0);
        md2->curveParam = GVAR_MENU_ITEM(COLUMN_X+MIXES_2ND_COLUMN+5*FW, y, curveParam, -100, 100, LEFT|(menuHorizontalPosition==1 ? attr : 0), 0, editMode>0 ? event : 0);
        if (attr && editMode>0 && menuHorizontalPosition==0) {
          int8_t tmp = 0;
          CHECK_INCDEC_MODELVAR(event, tmp, -1, 1);
          if (tmp != 0) {
            md2->curveMode = MODE_CURVE;
            md2->curveParam = tmp;
          }
        }
      }
      break;
    }
#endif
#if defined(FLIGHT_MODES)
    case MIX_FIELD_FLIGHT_PHASE:
      md2->flightModes = editFlightModes(COLUMN_X+MIXES_2ND_COLUMN, y, event, md2->flightModes, attr);
      break;
#endif
    case MIX_FIELD_SWITCH:
      md2->swtch = switchMenuItem(COLUMN_X+MIXES_2ND_COLUMN, y, md2->swtch, attr, event);
      break;
    case MIX_FIELD_WARNING:
      lcdDrawTextColumnLeft(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MIXWARNING);
      if (md2->mixWarn)
        lcdDrawNumberAttUnit(COLUMN_X+MIXES_2ND_COLUMN, y, md2->mixWarn, attr|LEFT);
      else
        lcdDrawTextAtt(COLUMN_X+MIXES_2ND_COLUMN, y, STR_OFF, attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, md2->mixWarn, 3);
      break;
    case MIX_FIELD_MLTPX:
      md2->mltpx = selectMenuItem(COLUMN_X+MIXES_2ND_COLUMN, y, STR_MULTPX, STR_VMLTPX, md2->mltpx, 0, 2, attr, event);
      break;
    case MIX_FIELD_DELAY_UP:
      md2->delayUp = EDIT_DELAY(COLUMN_X, y, event, attr, STR_DELAYUP, md2->delayUp);
      break;
    case MIX_FIELD_DELAY_DOWN:
      md2->delayDown = EDIT_DELAY(COLUMN_X, y, event, attr, STR_DELAYDOWN, md2->delayDown);
      break;
    case MIX_FIELD_SLOW_UP:
      md2->speedUp = EDIT_DELAY(COLUMN_X, y, event, attr, STR_SLOWUP, md2->speedUp);
      break;
    case MIX_FIELD_SLOW_DOWN:
      md2->speedDown = EDIT_DELAY(COLUMN_X, y, event, attr, STR_SLOWDOWN, md2->speedDown);
      break;
    }
  }
}

static uint8_t s_maxLines = 8;
static uint8_t s_copySrcIdx;
static uint8_t s_copySrcCh;

#define _STR_MAX(x) PSTR("/" #x)
#define STR_MAX(x) _STR_MAX(x)

#define EXPO_LINE_WEIGHT_POS 7*FW+1
#define EXPO_LINE_EXPO_POS   11*FW
#define EXPO_LINE_SWITCH_POS 11*FW+4
#if MAX_FLIGHT_MODES == 6
#define EXPO_LINE_SIDE_POS 15*FW
#else
#define EXPO_LINE_SIDE_POS 15*FW+2
#endif
#define EXPO_LINE_FM_POS     LCD_W-FW-MENUS_SCROLLBAR_WIDTH
#define EXPO_LINE_SELECT_POS 24
#define MIX_LINE_SRC_POS     4*FW-1
#define MIX_LINE_WEIGHT_POS  11*FW+3
#define MIX_LINE_CURVE_POS   12*FW+2
#define MIX_LINE_SWITCH_POS  16*FW
#define MIX_LINE_DELAY_POS   19*FW+7

#if defined(NAVIGATION_MENUS)
void onExpoMixMenu(const char *result)
{
  bool expo = (menuHandlers[menuLevel] == menuModelExposAll);
  uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

  if (result == STR_EDIT) {
    pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
  } else if (result == STR_INSERT_BEFORE || result == STR_INSERT_AFTER) {
    if (!reachExpoMixCountLimit(expo)) {
      s_currCh = chn;
      if (result == STR_INSERT_AFTER) {
        s_currIdx++;
        menuVerticalPosition++;
      }
      insertExpoMix(expo, s_currIdx);
      pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
    }
  } else if (result == STR_COPY || result == STR_MOVE) {
    s_copyMode = (result == STR_COPY ? COPY_MODE : MOVE_MODE);
    s_copySrcIdx = s_currIdx;
    s_copySrcCh = chn;
    s_copySrcRow = menuVerticalPosition;
  } else if (result == STR_DELETE) {
    deleteExpoMix(expo, s_currIdx);
  }
}
#endif

void displayMixInfos(coord_t y, MixData *md)
{
  if (md->curveParam) {
    if (md->curveMode == MODE_CURVE)
      lcdDrawCurveName(MIX_LINE_CURVE_POS, y, md->curveParam);
    else
      displayGVar(MIX_LINE_CURVE_POS+3*FW, y, md->curveParam, -100, 100);
  }

  if (md->swtch) {
    lcdPutsSwitches(MIX_LINE_SWITCH_POS, y, md->swtch);
  }
}

#define displayMixLine(y, md) displayMixInfos(y, md)

void displayExpoInfos(coord_t y, ExpoData *ed)
{
  if (ed->curveMode == MODE_CURVE)
    lcdDrawCurveName(EXPO_LINE_EXPO_POS-3*FW, y, ed->curveParam);
  else
    displayGVar(EXPO_LINE_EXPO_POS, y, ed->curveParam, -100, 100);

  lcdPutsSwitches(EXPO_LINE_SWITCH_POS, y, ed->swtch, 0);
}

#define displayExpoLine(y, ed) \
  displayExpoInfos(y, ed); \
  displayFlightModes(EXPO_LINE_FM_POS, y, ed->flightModes)

void menuModelExpoMix(uint8_t expo, uint8_t event)
{
  uint8_t sub = menuVerticalPosition;

  if (s_editMode > 0)
    s_editMode = 0;

  uint8_t chn = (expo ? expoAddress(s_currIdx)->chn+1 : mixAddress(s_currIdx)->destCh+1);

  switch (event) {
  case EVT_ENTRY:
  case EVT_ENTRY_UP:
    s_copyMode = 0;
    s_copyTgtOfs = 0;
    break;
  case EVT_KEY_LONG(KEY_EXIT):
    if (s_copyMode && s_copyTgtOfs == 0) {
      deleteExpoMix(expo, s_currIdx);
      killEvents(event);
      event = 0;
    }
    // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_LONG:
    if (s_copyMode) {
      killEvents(event);
    }
#endif
  case EVT_KEY_BREAK(KEY_EXIT):
    if (s_copyMode) {
      if (s_copyTgtOfs) {
        // cancel the current copy / move operation
        if (s_copyMode == COPY_MODE) {
          deleteExpoMix(expo, s_currIdx);
        } else {
          do {
            swapExpoMix(expo, s_currIdx, s_copyTgtOfs > 0);
            s_copyTgtOfs += (s_copyTgtOfs < 0 ? +1 : -1);
          } while (s_copyTgtOfs != 0);
          eeDirty(EE_MODEL);
        }
        menuVerticalPosition = s_copySrcRow;
        s_copyTgtOfs = 0;
      }
      s_copyMode = 0;
      event = 0;
    }
    break;
  case EVT_KEY_BREAK(KEY_ENTER):
    if (sub != 0 && (!s_currCh || (s_copyMode && !s_copyTgtOfs)) && !READ_ONLY()) {
      s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
      s_copySrcIdx = s_currIdx;
      s_copySrcCh = chn;
      s_copySrcRow = sub;
      break;
    }
    // no break

    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_LONG(KEY_ENTER):
    killEvents(event);
    if (s_copyTgtOfs) {
      s_copyMode = 0;
      s_copyTgtOfs = 0;
    } else if (sub != 0) {
      if (READ_ONLY()) {
        if (!s_currCh) {
          pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
          return;
        }
      } else {
        if (s_copyMode) s_currCh = 0;
#if defined(NAVIGATION_MENUS)
        if (s_currCh) {
          if (reachExpoMixCountLimit(expo)) break;
          insertExpoMix(expo, s_currIdx);
          pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
          s_copyMode = 0;
          return;
        } else {
          event = 0;
          s_copyMode = 0;
          POPUP_MENU_ADD_ITEM(STR_EDIT);
          POPUP_MENU_ADD_ITEM(STR_INSERT_BEFORE);
          POPUP_MENU_ADD_ITEM(STR_INSERT_AFTER);
          POPUP_MENU_ADD_ITEM(STR_COPY);
          POPUP_MENU_ADD_ITEM(STR_MOVE);
          POPUP_MENU_ADD_ITEM(STR_DELETE);
          popupMenuHandler = onExpoMixMenu;
        }
#else
        if (s_currCh) {
          if (reachExpoMixCountLimit(expo)) break;
          insertExpoMix(expo, s_currIdx);
        }
        pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
        s_copyMode = 0;
        return;
#endif
      }
    }
    break;
  case EVT_KEY_LONG(KEY_LEFT):
  case EVT_KEY_LONG(KEY_RIGHT):
    if (s_copyMode && !s_copyTgtOfs) {
      if (reachExpoMixCountLimit(expo)) break;
      s_currCh = chn;
      if (event == EVT_KEY_LONG(KEY_RIGHT)) {
        s_currIdx++;
        menuVerticalPosition++;
      }
      insertExpoMix(expo, s_currIdx);
      pushMenu(expo ? menuModelExpoOne : menuModelMixOne);
      s_copyMode = 0;
      killEvents(event);
      return;
    }
    break;
#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_LEFT:
  case EVT_ROTARY_RIGHT:
#endif
  case EVT_KEY_FIRST(KEY_MOVE_UP):
  case EVT_KEY_REPT(KEY_MOVE_UP):
  case EVT_KEY_FIRST(KEY_MOVE_DOWN):
  case EVT_KEY_REPT(KEY_MOVE_DOWN):
    if (s_copyMode) {
      uint8_t key = (event & 0x1f);
      uint8_t next_ofs = ((IS_ROTARY_UP(event) || key==KEY_MOVE_UP) ? s_copyTgtOfs - 1 : s_copyTgtOfs + 1);

      if (s_copyTgtOfs==0 && s_copyMode==COPY_MODE) {
        // insert a mix on the same channel (just above / just below)
        if (reachExpoMixCountLimit(expo)) break;
        copyExpoMix(expo, s_currIdx);
        if (IS_ROTARY_DOWN(event) || key==KEY_MOVE_DOWN) s_currIdx++;
        else if (sub-menuVerticalOffset >= 6) menuVerticalOffset++;
      } else if (next_ofs==0 && s_copyMode==COPY_MODE) {
        // delete the mix
        deleteExpoMix(expo, s_currIdx);
        if (IS_ROTARY_UP(event) || key==KEY_MOVE_UP) s_currIdx--;
      } else {
        // only swap the mix with its neighbor
        if (!swapExpoMix(expo, s_currIdx, IS_ROTARY_UP(event) || key==KEY_MOVE_UP)) break;
        eeDirty(EE_MODEL);
      }

      s_copyTgtOfs = next_ofs;
    }
    break;
  }

  lcdDrawNumberAttUnit(FW*max(sizeof(TR_MENUINPUTS), sizeof(TR_MIXER))+FW+FW/2, 0, getExpoMixCount(expo));
  lcdDrawText(FW*max(sizeof(TR_MENUINPUTS), sizeof(TR_MIXER))+FW+FW/2, 0, expo ? STR_MAX(MAX_EXPOS) : STR_MAX(MAX_MIXERS));

  SIMPLE_MENU(expo ? STR_MENUINPUTS : STR_MIXER, menuTabModel, expo ? e_InputsAll : e_MixAll, s_maxLines);

  sub = menuVerticalPosition;
  s_currCh = 0;
  uint8_t cur = 1;
  uint8_t i = 0;

  for (uint8_t ch=1; ch<=(expo ? NUM_INPUTS : NUM_CHNOUT); ch++) {
    void *pointer = NULL;
    MixData * &md = (MixData * &)pointer;
    ExpoData * &ed = (ExpoData * &)pointer;
    coord_t y = MENU_HEADER_HEIGHT-FH+1+(cur-menuVerticalOffset)*FH;
    if (expo ? (i<MAX_EXPOS && (ed=expoAddress(i))->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && (md=mixAddress(i))->srcRaw && md->destCh+1 == ch)) {
      if (menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES) {
        if (expo) {
          putsMixerSource(0, y, MIXSRC_Rud+ch-1, 0);
        } else {
          putsChn(0, y, ch, 0); // show CHx
        }
      }
      uint8_t mixCnt = 0;
      do {
        if (s_copyMode) {
          if (s_copyMode == MOVE_MODE && menuVerticalOffset < cur && cur-menuVerticalOffset < 8 && s_copySrcCh == ch && s_copyTgtOfs != 0 && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
            lcdDrawRect(expo ? 18 : 22, y-1, expo ? LCD_W-18 : LCD_W-22, 9, DOTTED);
            cur++;
            y+=FH;
          }
          if (s_currIdx == i) {
            sub = menuVerticalPosition = cur;
            s_currCh = ch;
          }
        } else if (sub == cur) {
          s_currIdx = i;
        }
        if (menuVerticalOffset < cur && cur-menuVerticalOffset < 8) {
          uint8_t attr = ((s_copyMode || sub != cur) ? 0 : INVERS);
          if (expo) {
            ed->weight = GVAR_MENU_ITEM(EXPO_LINE_WEIGHT_POS, y, ed->weight, MIN_EXPO_WEIGHT, 100, attr | (isExpoActive(i) ? BOLD : 0), 0, event);
            displayExpoLine(y, ed);
            if (ed->mode!=3) {
              lcdDrawChar(EXPO_LINE_SIDE_POS, y, ed->mode == 2 ? 126 : 127);
            }
          } else {
            if (mixCnt > 0) lcdDrawTextAtIndex(FW, y, STR_VMLTPX2, md->mltpx, 0);

            putsMixerSource(MIX_LINE_SRC_POS, y, md->srcRaw, 0);

            gvarWeightItem(MIX_LINE_WEIGHT_POS, y, md, attr | (isMixActive(i) ? BOLD : 0), event);

            displayMixLine(y, md);

            char cs = ' ';
            if (md->speedDown || md->speedUp)
              cs = 'S';
            if (md->delayUp || md->delayDown)
              cs = (cs =='S' ? '*' : 'D');
            lcdDrawChar(MIX_LINE_DELAY_POS, y, cs);
          }
          if (s_copyMode) {
            if ((s_copyMode==COPY_MODE || s_copyTgtOfs == 0) && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
              /* draw a border around the raw on selection mode (copy/move) */
              lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (LCD_W-EXPO_LINE_SELECT_POS) : (LCD_W-22), 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
            }
            if (cur == sub) {
              /* invert the raw when it's the current one */
              lcdDrawFilledRect(expo ? EXPO_LINE_SELECT_POS+1 : 23, y, expo ? (LCD_W-EXPO_LINE_SELECT_POS-2) : (LCD_W-24), 7);
            }
          }
        }
        cur++;
        y+=FH;
        mixCnt++;
        i++;
        if (expo) ed++;
        else md++;
      } while (expo ? (i<MAX_EXPOS && ed->chn+1 == ch && EXPO_VALID(ed)) : (i<MAX_MIXERS && md->srcRaw && md->destCh+1 == ch));
      if (s_copyMode == MOVE_MODE && menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES && s_copySrcCh == ch && i == (s_copySrcIdx + (s_copyTgtOfs<0))) {
        lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? LCD_W-EXPO_LINE_SELECT_POS : LCD_W-22, 9, DOTTED);
        cur++;
        y+=FH;
      }
    } else {
      uint8_t attr = 0;
      if (sub == cur) {
        s_currIdx = i;
        s_currCh = ch;
        if (!s_copyMode) {
          attr = INVERS;
        }
      }
      if (menuVerticalOffset < cur && cur-menuVerticalOffset < LCD_LINES) {
        if (expo) {
          putsMixerSource(0, y, MIXSRC_Rud+ch-1, attr);
        } else {
          putsChn(0, y, ch, attr); // show CHx
        }
        if (s_copyMode == MOVE_MODE && s_copySrcCh == ch) {
          lcdDrawRect(expo ? EXPO_LINE_SELECT_POS : 22, y-1, expo ? (LCD_W-EXPO_LINE_SELECT_POS) : (LCD_W-22), 9, DOTTED);
        }
      }
      cur++;
      y+=FH;
    }
  }
  s_maxLines = cur;
  if (sub >= s_maxLines-1) menuVerticalPosition = s_maxLines-1;
}

void menuModelExposAll(uint8_t event)
{
  return menuModelExpoMix(1, event);
}

void menuModelMixAll(uint8_t event)
{
  return menuModelExpoMix(0, event);
}
# 1 "gui/menu_model_limits.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"


bool isThrottleOutput(uint8_t ch)
{
  for (uint8_t i=0; i<MAX_MIXERS; i++) {
    MixData *mix = mixAddress(i);
    if (mix->destCh==ch && mix->srcRaw==MIXSRC_Thr)
      return true;
  }
  return false;
}

enum LimitsItems {
  ITEM_LIMITS_OFFSET,
  ITEM_LIMITS_MIN,
  ITEM_LIMITS_MAX,
  ITEM_LIMITS_DIRECTION,
#if defined(PPM_CENTER_ADJUSTABLE)
  ITEM_LIMITS_PPM_CENTER,
#endif
#if defined(PPM_LIMITS_SYMETRICAL)
  ITEM_LIMITS_SYMETRICAL,
#endif
  ITEM_LIMITS_COUNT,
  ITEM_LIMITS_MAXROW = ITEM_LIMITS_COUNT-1
};

#if defined(PPM_UNIT_US)
#define LIMITS_MIN_POS          12*FW+1
#else
#define LIMITS_MIN_POS          12*FW
#endif
#define LIMITS_OFFSET_POS         8*FW
#if defined(PPM_LIMITS_SYMETRICAL)
#if defined(PPM_CENTER_ADJUSTABLE)
#define LIMITS_MAX_POS        15*FW
#define LIMITS_REVERT_POS     16*FW-3
#define LIMITS_PPM_CENTER_POS 20*FW+1
#else
#define LIMITS_DIRECTION_POS  12*FW+4
#define LIMITS_MAX_POS        16*FW+4
#define LIMITS_REVERT_POS     17*FW
#endif
#else
#if defined(PPM_CENTER_ADJUSTABLE)
#define LIMITS_MAX_POS        16*FW
#define LIMITS_REVERT_POS     17*FW-2
#define LIMITS_PPM_CENTER_POS 21*FW+2
#else
#define LIMITS_MAX_POS        17*FW
#define LIMITS_REVERT_POS     18*FW
#define LIMITS_DIRECTION_POS  12*FW+5
#endif
#endif

#define LIMITS_MIN_MAX_OFFSET 100
#define CONVERT_US_MIN_MAX(x) ((int16_t(x)*128)/25)
#define MIN_MAX_ATTR          attr

#if defined(PPM_UNIT_US)
#define SET_MIN_MAX(x, val)   x = ((val)*250)/128
#define MIN_MAX_DISPLAY(x)    CONVERT_US_MIN_MAX(x)
#else
#define MIN_MAX_DISPLAY(x)    ((int8_t)(x))
#endif

void menuModelLimits(uint8_t event)
{
  uint8_t sub = menuVerticalPosition - 1;

  if (sub < NUM_CHNOUT) {
#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_UNIT_US)
    lcdDrawNumberAttUnit(13*FW, 0, PPM_CH_CENTER(sub)+channelOutputs[sub]/2, 0);
    lcdDrawText(13*FW, 0, STR_US);
#else
    lcdDrawNumberAttUnit(13*FW, 0, calcRESXto1000(channelOutputs[sub]), PREC1);
#endif
  }

  MENU(STR_MENULIMITS, menuTabModel, e_Limits, 1+NUM_CHNOUT+1, {0, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, ITEM_LIMITS_MAXROW, 0});

  if (warningResult) {
    warningResult = false;
    LimitData *ld = limitAddress(sub);
    ld->revert = !ld->revert;
    eeDirty(EE_MODEL);
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;

    if (k==NUM_CHNOUT) {
      // last line available - add the "copy trim menu" line
      uint8_t attr = (sub==NUM_CHNOUT) ? INVERS : 0;
      lcdDrawTextAtt(CENTER_OFS, y, STR_TRIMS2OFFSETS, NO_HIGHLIGHT() ? 0 : attr);
      if (attr) {
        s_editMode = 0;
        if (event==EVT_KEY_LONG(KEY_ENTER)) {
          START_NO_HIGHLIGHT();
          killEvents(event);
          moveTrimsToOffsets(); // if highlighted and menu pressed - move trims to offsets
        }
      }
      return;
    }

    LimitData *ld = limitAddress(k);

#if !defined(PPM_CENTER_ADJUSTABLE)
    int16_t v = (ld->revert) ? -LIMIT_OFS(ld) : LIMIT_OFS(ld);
    char swVal = '-';  // '-', '<', '>'
    if ((channelOutputs[k] - v) > 50) swVal = (ld->revert ? 127 : 126); // Switch to raw inputs?  - remove trim!
    if ((channelOutputs[k] - v) < -50) swVal = (ld->revert ? 126 : 127);
    putsChn(0, y, k+1, 0);
    lcdDrawChar(LIMITS_DIRECTION_POS, y, swVal);
#endif

    int8_t limit = (g_model.extendedLimits ? LIMIT_EXT_MAX : 100);

    putsChn(0, y, k+1, 0);

    for (uint8_t j=0; j<ITEM_LIMITS_COUNT; j++) {
      uint8_t attr = ((sub==k && menuHorizontalPosition==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && (s_editMode>0 || p1valdiff)) ;
      if (active) { STICK_SCROLL_DISABLE(); }
      switch(j) {
      case ITEM_LIMITS_OFFSET:
#if defined(PPM_UNIT_US)
        lcdDrawNumberAttUnit(LIMITS_OFFSET_POS, y, ((int32_t)ld->offset*128) / 25, attr|PREC1);
#else
        lcdDrawNumberAttUnit(LIMITS_OFFSET_POS, y, ld->offset, attr|PREC1);
#endif
        if (active) {
          ld->offset = checkIncDec(event, ld->offset, -1000, 1000, EE_MODEL|NO_INCDEC_MARKS);
        } else if (attr && event==EVT_KEY_LONG(KEY_MENU)) {
          copySticksToOffset(k);
          s_editMode = 0;
        }
        break;

      case ITEM_LIMITS_MIN:
        lcdDrawNumberAttUnit(LIMITS_MIN_POS, y, MIN_MAX_DISPLAY(ld->min-LIMITS_MIN_MAX_OFFSET), MIN_MAX_ATTR);
        if (active) ld->min = LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->min-LIMITS_MIN_MAX_OFFSET, -limit, 0, EE_MODEL);
        break;

      case ITEM_LIMITS_MAX:
        lcdDrawNumberAttUnit(LIMITS_MAX_POS, y, MIN_MAX_DISPLAY(ld->max+LIMITS_MIN_MAX_OFFSET), MIN_MAX_ATTR);
        if (active) ld->max = -LIMITS_MIN_MAX_OFFSET + checkIncDec(event, ld->max+LIMITS_MIN_MAX_OFFSET, 0, +limit, EE_MODEL);
        break;

      case ITEM_LIMITS_DIRECTION: {
        uint8_t revert = ld->revert;
#if defined(PPM_CENTER_ADJUSTABLE)
        lcdDrawCharAtt(LIMITS_REVERT_POS, y, revert ? 127 : 126, attr);
#else
        lcdDrawTextAtIndex(LIMITS_REVERT_POS, y, STR_MMMINV, revert, attr);
#endif
        if (active) {
          uint8_t revert_new = checkIncDecModel(event, revert, 0, 1);
          if (checkIncDec_Ret && isThrottleOutput(k)) {
            POPUP_CONFIRMATION(STR_INVERT_THR);
          } else {
            ld->revert = revert_new;
          }
        }
        break;
      }

#if defined(PPM_CENTER_ADJUSTABLE)
      case ITEM_LIMITS_PPM_CENTER:
        lcdDrawNumberAttUnit(LIMITS_PPM_CENTER_POS, y, PPM_CENTER+ld->ppmCenter, attr);
        if (active) {
          CHECK_INCDEC_MODELVAR(event, ld->ppmCenter, -PPM_CENTER_MAX, +PPM_CENTER_MAX);
        }
        break;
#endif

#if defined(PPM_LIMITS_SYMETRICAL)
      case ITEM_LIMITS_SYMETRICAL:
        lcdDrawCharAtt(LCD_W-FW-MENUS_SCROLLBAR_WIDTH, y, ld->symetrical ? '=' : '^', attr);
        if (active) {
          CHECK_INCDEC_MODELVAR_ZERO(event, ld->symetrical, 1);
        }
        break;
#endif
      }
    }
  }
}
# 1 "gui/menu_model_logical_switches.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"


enum LogicalSwitchFields {
  LS_FIELD_FUNCTION,
  LS_FIELD_V1,
  LS_FIELD_V2,
  LS_FIELD_ANDSW,
  LS_FIELD_COUNT,
  LS_FIELD_LAST = LS_FIELD_COUNT-1
};

#define CSW_1ST_COLUMN  (4*FW-3)
#define CSW_2ND_COLUMN  (8*FW-3)
#define CSW_3RD_COLUMN  (13*FW-6)
#define CSW_4TH_COLUMN  (18*FW+2)


void menuModelLogicalSwitches(uint8_t event)
{
  INCDEC_DECLARE_VARS(EE_MODEL);

  MENU(STR_MENULOGICALSWITCHES, menuTabModel, e_LogicalSwitches, NUM_LOGICAL_SWITCH+1, {0, NAVIGATION_LINE_BY_LINE|LS_FIELD_LAST/*repeated...*/});

  int8_t    sub = menuVerticalPosition - 1;
  horzpos_t horz = menuHorizontalPosition;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    uint8_t attr = (sub==k ? ((s_editMode>0) ? BLINK|INVERS : INVERS)  : 0);
    uint8_t attr1 = (horz==1 ? attr : 0);
    uint8_t attr2 = (horz==2 ? attr : 0);
    LogicalSwitchData * cs = lswAddress(k);

    // CSW name
    uint8_t sw = SWSRC_SW1+k;
    lcdPutsSwitches(0, y, sw, (getSwitch(sw) ? BOLD : 0) | ((sub==k && CURSOR_ON_LINE()) ? INVERS : 0));

    // CSW func
    lcdDrawTextAtIndex(CSW_1ST_COLUMN, y, STR_VCSWFUNC, cs->func, horz==0 ? attr : 0);

    // CSW params
    uint8_t cstate = lswFamily(cs->func);
    int8_t v1_min=0, v1_max=MIXSRC_LAST_TELEM, v2_min=0, v2_max=MIXSRC_LAST_TELEM;
#define v1_val cs->v1

    if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      lcdPutsSwitches(CSW_2ND_COLUMN, y, cs->v1, attr1);
      lcdPutsSwitches(CSW_3RD_COLUMN, y, cs->v2, attr2);
      v1_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES;
      v1_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      v2_min = SWSRC_FIRST_IN_LOGICAL_SWITCHES;
      v2_max = SWSRC_LAST_IN_LOGICAL_SWITCHES;
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SWITCH);
      INCDEC_ENABLE_CHECK(isSwitchAvailableInLogicalSwitches);
    } else if (cstate == LS_FAMILY_COMP) {
      putsMixerSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      putsMixerSource(CSW_3RD_COLUMN, y, cs->v2, attr2);
      INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
      INCDEC_ENABLE_CHECK(isSourceAvailable);
    } else if (cstate == LS_FAMILY_TIMER) {
      lcdDrawNumberAttUnit(CSW_2ND_COLUMN, y, lswTimerValue(cs->v1), LEFT|PREC1|attr1);
      lcdDrawNumberAttUnit(CSW_3RD_COLUMN, y, lswTimerValue(cs->v2), LEFT|PREC1|attr2);
      v1_min = v2_min = -128;
      v1_max = v2_max = 122;
      INCDEC_SET_FLAG(EE_MODEL);
      INCDEC_ENABLE_CHECK(NULL);
    } else {
      putsMixerSource(CSW_2ND_COLUMN, y, v1_val, attr1);
      if (horz == 1) {
        INCDEC_SET_FLAG(EE_MODEL | INCDEC_SOURCE);
        INCDEC_ENABLE_CHECK(isSourceAvailableInCustomSwitches);
      } else {
        INCDEC_SET_FLAG(EE_MODEL);
        INCDEC_ENABLE_CHECK(NULL);
      }
#if   defined(FRSKY)
      if (v1_val >= MIXSRC_FIRST_TELEM) {
        lcdPutsTelemetryChannelValue(CSW_3RD_COLUMN, y, v1_val - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT|attr2);
        v2_max = maxTelemValue(v1_val - MIXSRC_FIRST_TELEM + 1);
        if (cstate == LS_FAMILY_OFS) {
          v2_min = -128;
          v2_max -= 128;
        } else {
          v2_max = min((uint8_t)127, (uint8_t)v2_max);
          v2_min = -v2_max;
        }
        if (cs->v2 > v2_max) {
          cs->v2 = v2_max;
          eeDirty(EE_MODEL);
        }
      } else {
        lcdDrawNumberAttUnit(CSW_3RD_COLUMN, y, cs->v2, LEFT|attr2);
        {
          v2_min = -LIMIT_EXT_PERCENT;
          v2_max = +LIMIT_EXT_PERCENT;
        }
      }
#else
      if (v1_val >= MIXSRC_FIRST_TELEM) {
        lcdPutsTelemetryChannelValue(CSW_3RD_COLUMN, y, v1_val - MIXSRC_FIRST_TELEM, convertLswTelemValue(cs), LEFT|attr2);
        v2_min = -128;
        v2_max = 127;
      } else {
        lcdDrawNumberAttUnit(CSW_3RD_COLUMN, y, cs->v2, LEFT|attr2);
        v2_min = -LIMIT_EXT_PERCENT;
        v2_max = +LIMIT_EXT_PERCENT;
      }
#endif
    }

    // CSW AND switch
    lcdPutsSwitches(CSW_4TH_COLUMN, y, (cs->andsw), horz==LS_FIELD_ANDSW ? attr : 0);


    if ((s_editMode>0 || p1valdiff) && attr) {
      switch (horz) {
      case LS_FIELD_FUNCTION: {
        CHECK_INCDEC_MODELVAR_ZERO(event, cs->func, LS_FUNC_MAX);
        uint8_t new_cstate = lswFamily(cs->func);
        if (cstate != new_cstate) {
          cs->v1 = cs->v2 = (new_cstate==LS_FAMILY_TIMER ? -119/*1.0*/ : 0);
        }
        break;
      }
      case LS_FIELD_V1:
        cs->v1 = CHECK_INCDEC_PARAM(event, v1_val, v1_min, v1_max);
#if defined(SIMU) // Not needed but ...
        if (checkIncDec_Ret) TRACE("v1=%d", cs->v1);
#endif
        break;
      case LS_FIELD_V2:
        cs->v2 = CHECK_INCDEC_PARAM(event, cs->v2, v2_min, v2_max);
#if defined(SIMU)
        if (checkIncDec_Ret) TRACE("v2=%d", cs->v2);
#endif
        break;
      case LS_FIELD_ANDSW:
        CHECK_INCDEC_MODELVAR(event, cs->andsw, SWSRC_FIRST, SWSRC_LAST);
#if defined(SIMU)
        if (checkIncDec_Ret) TRACE("andsw=%d", cs->andsw);
#endif
        break;
      }
    }
  }
}
# 1 "gui/menu_model_custom_functions.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"
#include "widgets.h"

#define MODEL_CUSTOM_FUNC_1ST_COLUMN          (3)
#define MODEL_CUSTOM_FUNC_2ND_COLUMN          (5*FW-2)
#define MODEL_CUSTOM_FUNC_3RD_COLUMN          (15*FW+2)
#define MODEL_CUSTOM_FUNC_4TH_COLUMN          (20*FW)
#if defined(GRAPHICS)
#define MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF  (20*FW)
#else
#define MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF  (18*FW+2)
#endif

#if defined(SDCARD) && defined(VOICE)
#include "view_text.cpp"
#endif

void menuCustomFunctions(uint8_t event, CustomFunctionData * functions, CustomFunctionsContext * functionsContext)
{
  int8_t sub = menuVerticalPosition - 1;

  uint8_t eeFlags = EE_MODEL;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;

    CustomFunctionData *cfn = &functions[k];
    uint8_t func = CFN_FUNC(cfn);
    for (uint8_t j=0; j<5; j++) {
      uint8_t attr = ((sub==k && menuHorizontalPosition==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && (s_editMode>0 || p1valdiff));
      switch (j) {
      case 0:
        lcdPutsSwitches(MODEL_CUSTOM_FUNC_1ST_COLUMN, y, CFN_SWITCH(cfn), attr | ((functionsContext->activeSwitches & ((MASK_CFN_TYPE)1 << k)) ? BOLD : 0));
        if (active || AUTOSWITCH_ENTER_LONG()) CHECK_INCDEC_SWITCH(event, CFN_SWITCH(cfn), SWSRC_FIRST, SWSRC_LAST, eeFlags, isSwitchAvailableInCustomFunctions);
        break;

      case 1:
        if (CFN_SWITCH(cfn)) {
          lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_2ND_COLUMN, y, STR_VFSWFUNC, func, attr);
          if (active) {
            CHECK_INCDEC_MODELVAR_ZERO(event, CFN_FUNC(cfn), FUNC_MAX-1);
            if (checkIncDec_Ret) CFN_RESET(cfn);
          }
        } else {
          j = 4; // skip other fields
          if (sub==k && menuHorizontalPosition > 0) {
            REPEAT_LAST_CURSOR_MOVE();
          }
        }
        break;

      case 2: {
        int8_t maxParam = NUM_CHNOUT-1;
#if defined(OVERRIDE_CHANNEL_FUNCTION)
        if (func == FUNC_OVERRIDE_CHANNEL) {
          putsChn(lcdNextPos, y, CFN_CH_INDEX(cfn)+1, attr);
        } else
#endif
          if (func == FUNC_TRAINER) {
            maxParam = 4;
            putsMixerSource(lcdNextPos, y, MIXSRC_Rud+CFN_CH_INDEX(cfn)-1, attr);
          }
#if defined(GVARS)
          else if (func == FUNC_ADJUST_GVAR) {
            maxParam = MAX_GVARS-1;
            lcdDrawStringWithIndex(lcdNextPos, y, STR_GV, CFN_GVAR_INDEX(cfn)+1, attr);
            if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_GVAR_INDEX(cfn), maxParam);
            break;
          }
#endif
          else if (attr) {
            REPEAT_LAST_CURSOR_MOVE();
          }
        if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_CH_INDEX(cfn), maxParam);
        break;
      }

      case 3: {
        INCDEC_DECLARE_VARS(eeFlags);
        int16_t val_displayed = CFN_PARAM(cfn);
        int8_t val_min = 0;
        uint8_t val_max = 255;
        if (func == FUNC_RESET) {
          val_max = FUNC_RESET_PARAM_LAST;
          lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_VFSWRESET, CFN_PARAM(cfn), attr);
        }
#if defined(OVERRIDE_CHANNEL_FUNCTION)
        else if (func == FUNC_OVERRIDE_CHANNEL) {
          val_displayed = (int8_t)CFN_PARAM(cfn);
          val_min = -LIMIT_EXT_PERCENT;
          val_max = +LIMIT_EXT_PERCENT;
          lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
        }
#endif
#if defined(AUDIO)
        else if (func == FUNC_PLAY_SOUND) {
          val_max = AU_FRSKY_LAST-AU_FRSKY_FIRST-1;
          lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_FUNCSOUNDS, val_displayed, attr);
        }
#endif
#if defined(HAPTIC)
        else if (func == FUNC_HAPTIC) {
          val_max = 3;
          lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
        }
#endif
#if   defined(VOICE)
        else if (func == FUNC_PLAY_TRACK) {
#if defined(GVARS)
          if (attr && event==EVT_KEY_LONG(KEY_ENTER)) {
            killEvents(event);
            s_editMode = !s_editMode;
            active = true;
            val_displayed = (val_displayed > 250 ? 0 : 251);
          }
          if (val_displayed > 250) {
            lcdDrawStringWithIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_GV, val_displayed-250, attr);
          } else {
            lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+PROMPT_CUSTOM_BASE, attr|LEFT);
          }
#else
          lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+PROMPT_CUSTOM_BASE, attr|LEFT);
#endif

#if defined(SDCARD)
          if (active) {
            char Promptext[VOICE_PROMPT_TEXT_LEN] = {};
#if defined(SIMU)
            Simu_showVoiceTextLine(val_displayed,Promptext);
#else
            showVoiceTextLine(val_displayed,Promptext); // Show the prompt text file if exist
#endif
            lcdDrawTextAtt(0,0, Promptext, BSS|INVERS|BLINK);
          }
#endif

        } else if (func == FUNC_PLAY_BOTH) {
          lcdDrawCharAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN+3*FWNUM, y, '|', attr);
          lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_3RD_COLUMN+3*FWNUM, y, val_displayed+PROMPT_CUSTOM_BASE, attr);
          lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_3RD_COLUMN+2+3*FWNUM, y, (val_displayed+PROMPT_CUSTOM_BASE+1)%10, attr|LEFT);
        } else if (func == FUNC_PLAY_VALUE) {
          val_max = MIXSRC_FIRST_TELEM + TELEM_DISPLAY_MAX - 1;
          putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
          INCDEC_ENABLE_CHECK(functionsContext == &globalFunctionsContext ? isSourceAvailableInGlobalFunctions : isSourceAvailable);
        }
#endif
#if defined(SDCARD)
        else if (func == FUNC_LOGS) {
          if (val_displayed) {
            lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed*10, attr|PREC1|LEFT);
            lcdDrawChar(lcdLastPos, y, 's');
          } else {
            lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_MMMINV, 0, attr);
          }
        }
#endif
#if defined(GVARS)
        else if (func == FUNC_ADJUST_GVAR) {
          switch (CFN_GVAR_MODE(cfn)) {
          case FUNC_ADJUST_GVAR_CONSTANT:
            val_displayed = (int16_t)CFN_PARAM(cfn);
            val_min = -CFN_GVAR_CST_MAX;
            val_max = +CFN_GVAR_CST_MAX;
            lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
            break;
          case FUNC_ADJUST_GVAR_SOURCE:
            val_max = MIXSRC_LAST_CH;
            putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
            INCDEC_SET_FLAG(eeFlags | INCDEC_SOURCE);
            INCDEC_ENABLE_CHECK(isSourceAvailable);
            break;
          case FUNC_ADJUST_GVAR_GVAR:
            val_max = MAX_GVARS-1;
            lcdDrawStringWithIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_GV, val_displayed+1, attr);
            break;
          default: // FUNC_ADJUST_GVAR_INC
            val_max = 1;
            lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, PSTR("\003-=1+=1"), val_displayed, attr);
            break;
          }

          if (attr && event==EVT_KEY_LONG(KEY_ENTER)) {
            killEvents(event);
            s_editMode = !s_editMode;
            active = true;
            CFN_GVAR_MODE(cfn) += 1;
            val_displayed = 0;
          }
        }
#endif
        else if (attr) {
          REPEAT_LAST_CURSOR_MOVE();
        }

        if (active) {
          CFN_PARAM(cfn) = CHECK_INCDEC_PARAM(event, val_displayed, val_min, val_max);
        }
        break;
      }

      case 4:
        if (HAS_ENABLE_PARAM(func)) {
          menu_lcd_onoff(MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF, y, CFN_ACTIVE(cfn), attr);
          if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_ACTIVE(cfn), 1);
        } else if (HAS_REPEAT_PARAM(func)) {
          if (CFN_PLAY_REPEAT(cfn) == 0) {
            lcdDrawCharAtt(MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF+3, y, '-', attr);
          } else {
            lcdDrawNumberAttUnit(MODEL_CUSTOM_FUNC_4TH_COLUMN+2+FW, y, CFN_PLAY_REPEAT(cfn)*CFN_PLAY_REPEAT_MUL, attr);
          }
          if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_PLAY_REPEAT(cfn), 60/CFN_PLAY_REPEAT_MUL);
        } else if (attr) {
          REPEAT_LAST_CURSOR_MOVE();
        }
        break;
      }
    }
  }
}

void menuModelCustomFunctions(uint8_t event)
{
  MENU(STR_MENUCUSTOMFUNC, menuTabModel, e_CustomFunctions, NUM_CFN+1, {0, NAVIGATION_LINE_BY_LINE|4/*repeated*/});
  return menuCustomFunctions(event, g_model.customFn, &modelFunctionsContext);
}
# 1 "gui/menu_model_flightmodes.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"


void displayFlightModes(coord_t x, coord_t y, FlightModesType value)
{
  uint8_t p = MAX_FLIGHT_MODES;
  do {
    --p;
    if (!(value & (1<<p)))
      lcdDrawChar(x, y, '0'+p);
    x -= FWNUM;
  } while (p!=0);
}

enum menuModelPhaseItems {
  ITEM_MODEL_PHASE_NAME,
  ITEM_MODEL_PHASE_SWITCH,
  ITEM_MODEL_PHASE_TRIMS,
  IF_ROTARY_ENCODERS(ITEM_MODEL_PHASE_ROTARY_ENCODERS)
  ITEM_MODEL_PHASE_FADE_IN,
  ITEM_MODEL_PHASE_FADE_OUT,
#if defined(GVARS)
  ITEM_MODEL_PHASE_GVARS_LABEL,
  ITEM_MODEL_PHASE_GV1,
  ITEM_MODEL_PHASE_GV2,
  ITEM_MODEL_PHASE_GV3,
  ITEM_MODEL_PHASE_GV4,
  ITEM_MODEL_PHASE_GV5,
  ITEM_MODEL_PHASE_GV6,
#if defined(EXTERNALEEPROM)
  ITEM_MODEL_PHASE_GV7,
  ITEM_MODEL_PHASE_GV8,
  ITEM_MODEL_PHASE_GV9,
  ITEM_MODEL_PHASE_GV10,
  ITEM_MODEL_PHASE_GV11,
  ITEM_MODEL_PHASE_GV12,
#endif
#endif
  ITEM_MODEL_PHASE_MAX
};

void menuModelPhaseOne(uint8_t event)
{
  FlightModeData *fm = flightModeAddress(s_currIdx);
  putsFlightMode(13*FW, 0, s_currIdx+1, (getFlightMode()==s_currIdx ? BOLD : 0));

#if defined(GVARS)
  static const pm_uint8_t mstate_tab_fm1[] PROGMEM = {0, 0, 0, (uint8_t)-1, 1, 1, 1, 1, 1 , 1, 1, 1, 1, 1, 1, 1};
  static const pm_uint8_t mstate_tab_others[] PROGMEM = {0, 0, 3, IF_ROTARY_ENCODERS(NUM_ROTARY_ENCODERS-1) 0, 0, (uint8_t)-1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

  check(event, 0, NULL, 0, (s_currIdx == 0) ? mstate_tab_fm1 : mstate_tab_others, DIM(mstate_tab_others)-1, ITEM_MODEL_PHASE_MAX - 1 - (s_currIdx==0 ? (ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH) : 0));

  TITLE(STR_MENUFLIGHTPHASE);

#define PHASE_ONE_FIRST_LINE (1+1*FH)
#else
  SUBMENU(STR_MENUFLIGHTPHASE, 3 + (s_currIdx==0 ? 0 : 2 + (bool)NUM_ROTARY_ENCODERS), {0, 0, 3, IF_ROTARY_ENCODERS(NUM_ROTARY_ENCODERS-1) 0/*, 0*/});
#define PHASE_ONE_FIRST_LINE (1+1*FH)
#endif

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

#if defined(GVARS)
  if (s_currIdx == 0 && sub>=ITEM_MODEL_PHASE_SWITCH) sub += ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH;

  for (uint8_t k=0; k<LCD_LINES-1; k++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
    int8_t i = k + menuVerticalOffset;
    if (s_currIdx == 0 && i>=ITEM_MODEL_PHASE_SWITCH) i += ITEM_MODEL_PHASE_FADE_IN-ITEM_MODEL_PHASE_SWITCH;
    uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
#else
  for (uint8_t i=0, k=0, y=PHASE_ONE_FIRST_LINE; i<ITEM_MODEL_PHASE_MAX; i++, k++, y+=FH) {
    if (s_currIdx == 0 && i==ITEM_MODEL_PHASE_SWITCH) i = ITEM_MODEL_PHASE_FADE_IN;
    uint8_t attr = (sub==k ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
#endif
    switch(i) {
    case ITEM_MODEL_PHASE_NAME:
      editSingleName(MIXES_2ND_COLUMN, y, STR_PHASENAME, fm->name, sizeof(fm->name), event, attr);
      break;
    case ITEM_MODEL_PHASE_SWITCH:
      fm->swtch = switchMenuItem(MIXES_2ND_COLUMN, y, fm->swtch, attr, event);
      break;
    case ITEM_MODEL_PHASE_TRIMS:
      lcdDrawTextLeft(y, STR_TRIMS);
      for (uint8_t t=0; t<NUM_STICKS; t++) {
        lcdPutsTrimMode(MIXES_2ND_COLUMN+(t*FW), y, s_currIdx, t, menuHorizontalPosition==t ? attr : 0);
        if (attr && menuHorizontalPosition==t && ((editMode>0) || p1valdiff)) {
          int16_t v = getRawTrimValue(s_currIdx, t);
          if (v < TRIM_EXTENDED_MAX) v = TRIM_EXTENDED_MAX;
          v = checkIncDec(event, v, TRIM_EXTENDED_MAX, TRIM_EXTENDED_MAX+MAX_FLIGHT_MODES-1, EE_MODEL);
          if (checkIncDec_Ret) {
            if (v == TRIM_EXTENDED_MAX) v = 0;
            setTrimValue(s_currIdx, t, v);
          }
        }
      }
      break;

#if ROTARY_ENCODERS > 0
    case ITEM_MODEL_PHASE_ROTARY_ENCODERS:
      lcdDrawTextLeft(y, STR_ROTARY_ENCODER);
      for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
        lcdPutsRotaryEncoderMode(MIXES_2ND_COLUMN+(t*FW), y, s_currIdx, t, menuHorizontalPosition==t ? attr : 0);
        if (attr && menuHorizontalPosition==t && ((editMode>0) || p1valdiff)) {
          int16_t v = flightModeAddress(s_currIdx)->rotaryEncoders[t];
          if (v < ROTARY_ENCODER_MAX) v = ROTARY_ENCODER_MAX;
          v = checkIncDec(event, v, ROTARY_ENCODER_MAX, ROTARY_ENCODER_MAX+MAX_FLIGHT_MODES-1, EE_MODEL);
          if (checkIncDec_Ret) {
            if (v == ROTARY_ENCODER_MAX) v = 0;
            flightModeAddress(s_currIdx)->rotaryEncoders[t] = v;
          }
        }
      }
      break;
#endif

    case ITEM_MODEL_PHASE_FADE_IN:
      fm->fadeIn = EDIT_DELAY(0, y, event, attr, STR_FADEIN, fm->fadeIn);
      break;

    case ITEM_MODEL_PHASE_FADE_OUT:
      fm->fadeOut = EDIT_DELAY(0, y, event, attr, STR_FADEOUT, fm->fadeOut);
      break;

#if defined(GVARS)
    case ITEM_MODEL_PHASE_GVARS_LABEL:
      lcdDrawTextLeft(y, STR_GLOBAL_VARS);
      break;

    default: {
      uint8_t idx = i-ITEM_MODEL_PHASE_GV1;
      uint8_t posHorz = menuHorizontalPosition;
      if (attr && posHorz > 0 && s_currIdx==0) posHorz++;

      lcdDrawStringWithIndex(INDENT_WIDTH, y, STR_GV, idx+1);

      editName(4*FW+2, y, g_model.gvars[idx].name, LEN_GVAR_NAME, event, posHorz==0 ? attr : 0);

      int16_t v = fm->gvars[idx];
      if (v > GVAR_MAX) {
        uint8_t p = v - GVAR_MAX - 1;
        if (p >= s_currIdx) p++;
        putsFlightMode(11*FW, y, p+1, posHorz==1 ? attr : 0);
      } else {
        lcdDrawTextAtt(11*FW, y, STR_OWN, posHorz==1 ? attr : 0);
      }
      if (attr && s_currIdx>0 && posHorz==1 && (editMode>0 || p1valdiff)) {
        if (v < GVAR_MAX) v = GVAR_MAX;
        v = checkIncDec(event, v, GVAR_MAX, GVAR_MAX+MAX_FLIGHT_MODES-1, EE_MODEL);
        if (checkIncDec_Ret) {
          if (v == GVAR_MAX) v = 0;
          fm->gvars[idx] = v;
        }
      }

      uint8_t p = getGVarFlightPhase(s_currIdx, idx);
      lcdDrawNumberAttUnit(21*FW, y, GVAR_VALUE(idx, p), posHorz==2 ? attr : 0);
      if (attr && posHorz==2 && ((editMode>0) || p1valdiff)) {
        GVAR_VALUE(idx, p) = checkIncDec(event, GVAR_VALUE(idx, p), -GVAR_LIMIT, GVAR_LIMIT, EE_MODEL);
      }

      break;
    }
#endif
    }
  }
}

#if defined(ROTARY_ENCODERS)
#if ROTARY_ENCODERS > 2
#define NAME_OFS (-4-12)
#define SWITCH_OFS (-FW/2-2-13)
#define TRIMS_OFS  (-FW/2-4-15)
#define ROTARY_ENC_OFS (0)
#else
#define NAME_OFS (-4)
#define SWITCH_OFS (-FW/2-2)
#define TRIMS_OFS  (-FW/2-4)
#define ROTARY_ENC_OFS (2)
#endif
#else
#define NAME_OFS 0
#define SWITCH_OFS (FW/2)
#define TRIMS_OFS  (FW/2)
#endif

void menuModelFlightModesAll(uint8_t event)
{
  SIMPLE_MENU(STR_MENUFLIGHTPHASES, menuTabModel, e_FlightModesAll, 1+MAX_FLIGHT_MODES+1);

  int8_t sub = menuVerticalPosition - 1;

  switch (event) {
    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_FIRST(KEY_ENTER):
    if (sub == MAX_FLIGHT_MODES) {
      s_editMode = 0;
      trimsCheckTimer = 200; // 2 seconds
    }
  // no break
  case EVT_KEY_FIRST(KEY_RIGHT):
    if (sub >= 0 && sub < MAX_FLIGHT_MODES) {
      s_currIdx = sub;
      pushMenu(menuModelPhaseOne);
    }
    break;
  }

  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    uint8_t y = 1 + (i+1)*FH;
    uint8_t att = (i==sub ? INVERS : 0);
    FlightModeData *p = flightModeAddress(i);
    putsFlightMode(0, y, i+1, att|(getFlightMode()==i ? BOLD : 0));

    lcdDrawSizedTextAtt(4*FW+NAME_OFS, y, p->name, sizeof(p->name), ZCHAR);
    if (i == 0) {
      lcdDrawText((5+LEN_FLIGHT_MODE_NAME)*FW+SWITCH_OFS, y, STR_DEFAULT);
    } else {
      lcdPutsSwitches((5+LEN_FLIGHT_MODE_NAME)*FW+SWITCH_OFS, y, p->swtch, 0);
      for (uint8_t t=0; t<NUM_STICKS; t++) {
        lcdPutsTrimMode((9+LEN_FLIGHT_MODE_NAME+t)*FW+TRIMS_OFS, y, i, t, 0);
      }
      for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
        lcdPutsRotaryEncoderMode((13+LEN_FLIGHT_MODE_NAME+t)*FW+TRIMS_OFS+ROTARY_ENC_OFS, y, i, t, 0);
      }
    }

    if (p->fadeIn || p->fadeOut) {
      lcdDrawChar(LCD_W-FW-MENUS_SCROLLBAR_WIDTH, y, (p->fadeIn && p->fadeOut) ? '*' : (p->fadeIn ? 'I' : 'O'));
    }
  }


  lcdDrawTextLeft((LCD_LINES-1)*FH+1, STR_CHECKTRIMS);
  putsFlightMode(OFS_CHECKTRIMS, (LCD_LINES-1)*FH+1, mixerCurrentFlightMode+1);
  if (sub==MAX_FLIGHT_MODES && !trimsCheckTimer) {
    lcd_status_line();
  }
}
# 1 "gui/menu_model_curves.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"

uint8_t s_curveChan;

int16_t curveFn(int16_t x)
{
  return applyCustomCurve(x, s_curveChan);
}

struct point_t {
  coord_t x;
  coord_t y;
};

point_t getPoint(uint8_t i)
{
  point_t result = {0, 0};
  CurveInfo crv = curveInfo(s_curveChan);
  int8_t *points = crv.crv;
  bool custom = crv.custom;
  uint8_t count = crv.points;
  if (i < count) {
    result.x = X0-1-WCHART+i*WCHART/(count/2);
    result.y = (LCD_H-1) - (100 + points[i]) * (LCD_H-1) / 200;
    if (custom && i>0 && i<count-1)
      result.x = X0-1-WCHART + (100 + (100 + points[count+i-1]) * (2*WCHART)) / 200;
  }
  return result;
}

void DrawCurve(uint8_t offset=0)
{
  DrawFunction(curveFn, offset);

  uint8_t i = 0;
  do {
    point_t point = getPoint(i);
    i++;
    if (point.x == 0) break;
    lcdDrawFilledRect(point.x-offset, point.y-1, 3, 3, SOLID, FORCE); // do markup square
  } while(1);
}

bool moveCurve(uint8_t index, int8_t shift, int8_t custom=0)
{
  if (g_model.curves[MAX_CURVES-1] + shift > NUM_POINTS-5*MAX_CURVES) {
    AUDIO_WARNING2();
    return false;
  }

  int8_t *crv = curveAddress(index);
  if (shift < 0) {
    for (uint8_t i=0; i<custom; i++)
      crv[i] = crv[2*i];
  }

  int8_t *nextCrv = curveAddress(index+1);
  memmove(nextCrv+shift, nextCrv, 5*(MAX_CURVES-index-1)+g_model.curves[MAX_CURVES-1]-g_model.curves[index]);
  if (shift < 0) memclear(&g_model.points[NUM_POINTS-1] + shift, -shift);
  while (index<MAX_CURVES)
    g_model.curves[index++] += shift;

  for (uint8_t i=0; i<custom-2; i++)
    crv[custom+i] = -100 + ((200 * (i+1) + custom/2) / (custom-1)) ;

  eeDirty(EE_MODEL);
  return true;
}

void menuModelCurveOne(uint8_t event)
{
  TITLE(STR_MENUCURVE);
  lcdDrawNumberAttUnit(PSIZE(TR_MENUCURVE)*FW+1, 0, s_curveChan+1, INVERS|LEFT);

  CurveInfo crv = curveInfo(s_curveChan);

  switch(event) {
  case EVT_ENTRY:
    s_editMode = 1;
    break;
    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_BREAK(KEY_ENTER):
    if (s_editMode <= 0)
      menuHorizontalPosition = 0;
    if (s_editMode == 1 && crv.custom)
      s_editMode = 2;
    else
      s_editMode = 1;
    break;
  case EVT_KEY_LONG(KEY_ENTER):
    if (s_editMode <= 0) {
      if (int8_t(++menuHorizontalPosition) > 4)
        menuHorizontalPosition = -4;
      for (uint8_t i=0; i<crv.points; i++)
        crv.crv[i] = (i-(crv.points/2)) * int8_t(menuHorizontalPosition) * 50 / (crv.points-1);
      eeDirty(EE_MODEL);
      killEvents(event);
    }
    break;
  case EVT_KEY_BREAK(KEY_EXIT):
    if (s_editMode > 0) {
      if (--s_editMode == 0)
        menuHorizontalPosition = 0;
    } else {
      popMenu();
    }
    break;

  /* CASE_EVT_ROTARY_LEFT */
  case EVT_KEY_REPT(KEY_LEFT):
  case EVT_KEY_FIRST(KEY_LEFT):
    if (s_editMode==1 && menuHorizontalPosition>0) menuHorizontalPosition--;
    if (s_editMode <= 0) {
      if (crv.custom) {
        moveCurve(s_curveChan, -crv.points+2);
      } else if (crv.points > MIN_POINTS) {
        moveCurve(s_curveChan, -1, (crv.points+1)/2);
      } else {
        AUDIO_WARNING2();
      }
      return;
    }
    break;

  /* CASE_EVT_ROTARY_RIGHT */
  case EVT_KEY_REPT(KEY_RIGHT):
  case EVT_KEY_FIRST(KEY_RIGHT):
    if (s_editMode==1 && menuHorizontalPosition<(crv.points-1)) menuHorizontalPosition++;
    if (s_editMode <= 0) {
      if (!crv.custom) {
        moveCurve(s_curveChan, crv.points-2, crv.points);
      } else if (crv.points < MAX_POINTS) {
        if (moveCurve(s_curveChan, 1)) {
          for (int8_t i=crv.points+crv.points-2; i>=0; i--) {
            if (i%2)
              crv.crv[i] = (crv.crv[i/2] + crv.crv[1+i/2]) / 2;
            else
              crv.crv[i] = crv.crv[i/2];
          }
        }
      } else {
        AUDIO_WARNING2();
      }
    }
    break;
  }

  lcdDrawTextLeft(7*FH, STR_TYPE);
  uint8_t attr = (s_editMode <= 0 ? INVERS : 0);
  lcdDrawNumberAttUnit(5*FW-2, 7*FH, crv.points, LEFT|attr);
  lcdDrawTextAtt(lcdLastPos, 7*FH, crv.custom ? PSTR("pt'") : PSTR("pt"), attr);

  DrawCurve();

  if (s_editMode>0) {
    uint8_t i = menuHorizontalPosition;
    point_t point = getPoint(i);

    if (s_editMode==1 || !BLINK_ON_PHASE) {
      // do selection square
      lcdDrawFilledRect(point.x-1, point.y-2, 5, 5, SOLID, FORCE);
      lcdDrawFilledRect(point.x, point.y-1, 3, 3, SOLID);
    }

    int8_t x = -100 + 200*i/(crv.points-1);
    if (crv.custom && i>0 && i<crv.points-1) x = crv.crv[crv.points+i-1];
    lcdDrawText(7, 2*FH, PSTR("x="));
    lcdDrawNumberAttUnit(7+2*FW, 2*FH, x, LEFT);
    lcdDrawText(7, 3*FH, PSTR("y="));
    lcdDrawNumberAttUnit(7+2*FW, 3*FH, crv.crv[i], LEFT);
    lcdDrawRect(3, 1*FH+4, 7*FW-2, 3*FH-2);

    if (p1valdiff || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP))
      CHECK_INCDEC_MODELVAR(event, crv.crv[i], -100, 100);  // edit Y on up/down

    if (i>0 && i<crv.points-1 && s_editMode==2 && (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_LEFT) || event==EVT_KEY_REPT(KEY_RIGHT)))
      CHECK_INCDEC_MODELVAR(event, crv.crv[crv.points+i-1], i==1 ? -99 : crv.crv[crv.points+i-2]+1, i==crv.points-2 ? 99 : crv.crv[crv.points+i]-1);  // edit X on left/right
  }
}

#if defined(GVARS)
#define CURVE_SELECTED() (sub >= 0 && sub < MAX_CURVES)
#define GVAR_SELECTED()  (sub >= MAX_CURVES)
#else
#define CURVE_SELECTED() (sub >= 0)
#endif

void menuModelCurvesAll(uint8_t event)
{
  SIMPLE_MENU(STR_MENUCURVES, menuTabModel, e_CurvesAll, 1+MAX_CURVES);

  int8_t  sub = menuVerticalPosition - 1;

  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
  case EVT_ROTARY_BREAK:
#endif
  case EVT_KEY_FIRST(KEY_RIGHT):
  case EVT_KEY_FIRST(KEY_ENTER):
    if (CURVE_SELECTED() && !READ_ONLY()) {
      s_curveChan = sub;
      pushMenu(menuModelCurveOne);
    }
    break;
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;
    uint8_t attr = (sub == k ? INVERS : 0);
    {
      lcdDrawStringWithIndex(0, y, STR_CV, k+1, attr);
    }
  }

  if (CURVE_SELECTED()) {
    s_curveChan = sub;
    DrawCurve(23);
  }
}
# 1 "gui/menu_model_telemetry.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"

enum menuModelTelemetryItems {
#if defined(FRSKY)
  ITEM_TELEMETRY_USR_PROTO,
#endif
  ITEM_TELEMETRY_A1_LABEL,
  ITEM_TELEMETRY_A1_RANGE,
  ITEM_TELEMETRY_A1_OFFSET,
  ITEM_TELEMETRY_A1_ALARM1,
  ITEM_TELEMETRY_A1_ALARM2,
  ITEM_TELEMETRY_A2_LABEL,
  ITEM_TELEMETRY_A2_RANGE,
  ITEM_TELEMETRY_A2_OFFSET,
  ITEM_TELEMETRY_A2_ALARM1,
  ITEM_TELEMETRY_A2_ALARM2,
  ITEM_TELEMETRY_RSSI_LABEL,
  ITEM_TELEMETRY_RSSI_ALARM1,
  ITEM_TELEMETRY_RSSI_ALARM2,
#if defined(FRSKY)
  ITEM_TELEMETRY_USR_LABEL,
  ITEM_TELEMETRY_USR_BLADES,
#endif
  ITEM_TELEMETRY_USR_VOLTAGE_SOURCE,
  ITEM_TELEMETRY_USR_CURRENT_SOURCE,
  ITEM_TELEMETRY_FAS_OFFSET,
  CASE_VARIO(ITEM_TELEMETRY_VARIO_LABEL)
#if defined(VARIO)
  ITEM_TELEMETRY_VARIO_SOURCE,
#endif
  CASE_VARIO(ITEM_TELEMETRY_VARIO_RANGE)
  ITEM_TELEMETRY_SCREEN_LABEL1,
  ITEM_TELEMETRY_SCREEN_LINE1,
  ITEM_TELEMETRY_SCREEN_LINE2,
  ITEM_TELEMETRY_SCREEN_LINE3,
  ITEM_TELEMETRY_SCREEN_LINE4,
  ITEM_TELEMETRY_SCREEN_LABEL2,
  ITEM_TELEMETRY_SCREEN_LINE5,
  ITEM_TELEMETRY_SCREEN_LINE6,
  ITEM_TELEMETRY_SCREEN_LINE7,
  ITEM_TELEMETRY_SCREEN_LINE8,
  ITEM_TELEMETRY_MAX
};

#define TELEM_COL1                   INDENT_WIDTH
#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_CZ)
#define TELEM_COL2                 (9*FW)
#else
#define TELEM_COL2                 (8*FW)
#endif
#define TELEM_BARS_COLMIN            (56-3*FW)
#define TELEM_BARS_COLMAX            (14*FW-3)
#define TELEM_SCRTYPE_COL            TELEM_COL2

#define IS_RANGE_DEFINED(k)            (g_model.telemetry.channels[k].ratio > 0)

#define CHANNEL_ROWS(x)              LABEL(CHANNEL), 1, 0, 2, 2
#define CHANNELS_ROWS                CHANNEL_ROWS(0), CHANNEL_ROWS(1),
#define SENSORS_ROWS

#define IF_FAS_OFFSET(x)             x,

#if defined(FRSKY)
#define USRPROTO                     1,
#define USRDATA_ROWS                 LABEL(UsrData), 0, 0, 0, IF_FAS_OFFSET(0)
#else
#define USRPROTO
#define USRDATA_ROWS                 0, 0, IF_FAS_OFFSET(0)
#endif

#define RSSI_ROWS                      LABEL(RSSI), 1, 1,

#define SCREEN_TYPE_ROWS             0

#define VARIO_RANGE_ROWS             3

#define TELEMETRY_TYPE_ROWS

#define TELEMETRY_SCREEN_ROWS(x)     SCREEN_TYPE_ROWS, 2, 2, 2, 2
#define TELEMETRY_CURRENT_CHANNEL(k) (k >= ITEM_TELEMETRY_A2_LABEL ? TELEM_ANA_A2 : TELEM_ANA_A1)


void menuModelTelemetry(uint8_t event)
{

  MENU(STR_MENUTELEMETRY, menuTabModel, e_Telemetry, ITEM_TELEMETRY_MAX+1, {USRPROTO 0, TELEMETRY_TYPE_ROWS CHANNELS_ROWS RSSI_ROWS SENSORS_ROWS USRDATA_ROWS CASE_VARIO(LABEL(Vario)) CASE_VARIO(0) CASE_VARIO(VARIO_RANGE_ROWS) TELEMETRY_SCREEN_ROWS(0), TELEMETRY_SCREEN_ROWS(1), });

  uint8_t protocol = g_model.rfProtocol;
  uint8_t sub = menuVerticalPosition - 1;

  switch (event) {
  case EVT_KEY_BREAK(KEY_DOWN):
  case EVT_KEY_BREAK(KEY_UP):
  case EVT_KEY_BREAK(KEY_LEFT):
  case EVT_KEY_BREAK(KEY_RIGHT):
    if (s_editMode>0 && sub<=ITEM_TELEMETRY_RSSI_ALARM2)
      frskySendAlarms(); // update FrSky module when edit mode exited
    break;
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;

    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    uint8_t ch = TELEMETRY_CURRENT_CHANNEL(k);
    TelemetryChannelData & channel = g_model.telemetry.channels[ch];
    uint8_t dest = TELEM_A1-1+ch;


    switch (k) {


    case ITEM_TELEMETRY_A1_LABEL:
    case ITEM_TELEMETRY_A2_LABEL:
      lcdDrawTextLeft(y, STR_ACHANNEL);
      lcdDrawNumberAttUnit(2*FW, y, ch+1, 0);
      lcdPutsTelemetryChannelValue(TELEM_COL2+6*FW, y, dest, telemetryData.analog[ch].value, LEFT);
      break;

    case ITEM_TELEMETRY_A1_RANGE:
    case ITEM_TELEMETRY_A2_RANGE:
      lcdDrawTextLeft(y, STR_RANGE);
      lcdPutsTelemetryChannelValue(TELEM_COL2, y, dest, 255-channel.offset, (menuHorizontalPosition<=0 ? attr : 0)|NO_UNIT|LEFT);
      lcdDrawTextAtIndex(lcdLastPos, y, STR_VTELEMUNIT, channel.type, menuHorizontalPosition!=0 ? attr : 0);
      if (attr && (s_editMode>0 || p1valdiff)) {
        if (menuHorizontalPosition == 0) {
          uint16_t ratio = checkIncDec(event, channel.ratio, 0, 256, EE_MODEL);
          if (checkIncDec_Ret) {
            if (ratio == 127 && channel.multiplier > 0) {
              channel.multiplier--;
              channel.ratio = 255;
            } else if (ratio == 256) {
              if (channel.multiplier < FRSKY_MULTIPLIER_MAX) {
                channel.multiplier++;
                channel.ratio = 128;
              }
            } else {
              channel.ratio = ratio;
            }
          }
        } else {
          CHECK_INCDEC_MODELVAR_ZERO(event, channel.type, UNIT_A1A2_MAX);
        }
      }
      break;

    case ITEM_TELEMETRY_A1_OFFSET:
    case ITEM_TELEMETRY_A2_OFFSET:
      lcdDrawTextLeft(y, STR_OFFSET);
      lcdPutsTelemetryChannelValue(TELEM_COL2, y, dest, 0, LEFT|attr);
      if (attr) channel.offset = checkIncDec(event, channel.offset, -256, 256, EE_MODEL);
      break;

    case ITEM_TELEMETRY_A1_ALARM1:
    case ITEM_TELEMETRY_A1_ALARM2:
    case ITEM_TELEMETRY_A2_ALARM1:
    case ITEM_TELEMETRY_A2_ALARM2: {
      uint8_t alarm = ((k==ITEM_TELEMETRY_A1_ALARM1 || k==ITEM_TELEMETRY_A2_ALARM1) ? 0 : 1);
      lcdDrawTextLeft(y, STR_ALARM);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_VALARM, ALARM_LEVEL(ch, alarm), menuHorizontalPosition<=0 ? attr : 0);
      lcdDrawTextAtIndex(TELEM_COL2+4*FW, y, STR_VALARMFN, ALARM_GREATER(ch, alarm), (CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0);
      lcdPutsTelemetryChannelValue(TELEM_COL2+6*FW, y, dest, channel.alarms_value[alarm], ((CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0) | LEFT);

      if (attr && (s_editMode>0 || p1valdiff)) {
        uint8_t t;
        switch (menuHorizontalPosition) {
        case 0:
          t = ALARM_LEVEL(ch, alarm);
          channel.alarms_level = (channel.alarms_level & ~(3<<(2*alarm))) + (checkIncDecModel(event, t, 0, 3) << (2*alarm));
          break;
        case 1:
          t = ALARM_GREATER(ch, alarm);
          if (t != checkIncDecModel(event, t, 0, 1)) {
            channel.alarms_greater ^= (1 << alarm);
            frskySendAlarms();
          }
          break;
        case 2:
          channel.alarms_value[alarm] = checkIncDec(event, channel.alarms_value[alarm], 0, 255, EE_MODEL);
          break;
        }
      }
      break;
    }

    case ITEM_TELEMETRY_RSSI_LABEL:
      lcdDrawTextLeft(y, PSTR("RSSI"));
      break;

    case ITEM_TELEMETRY_RSSI_ALARM1:
    case ITEM_TELEMETRY_RSSI_ALARM2: {
      uint8_t alarm = k-ITEM_TELEMETRY_RSSI_ALARM1;
      lcdDrawTextLeft(y, STR_ALARM);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_VALARM, ((2+alarm+g_model.telemetry.rssiAlarms[alarm].level)%4), menuHorizontalPosition<=0 ? attr : 0);
      lcdDrawChar(TELEM_COL2+4*FW, y, '<');
      lcdDrawNumberNAtt(TELEM_COL2+6*FW, y, getRssiAlarmValue(alarm), LEFT|(menuHorizontalPosition!=0 ? attr : 0), 3);

      if (attr && (s_editMode>0 || p1valdiff)) {
        switch (menuHorizontalPosition) {
        case 0:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.rssiAlarms[alarm].level, -3, 2); // circular (saves flash)
          break;
        case 1:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.rssiAlarms[alarm].value, -30, 30);
          break;
        }
      }
      break;
    }

#if defined(FRSKY)
    case ITEM_TELEMETRY_USR_LABEL:
      lcdDrawTextLeft(y, STR_USRDATA);
      break;

    case ITEM_TELEMETRY_USR_PROTO:
      lcdDrawTextLeft(y, STR_PROTO);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_VTELPROTO, g_model.telemetry.usrProto, attr);
      if (attr) {
          CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.usrProto, USR_PROTO_LAST);
          if (!IS_MULTIMODULE_PROTOCOL(protocol)) { // No reset in Multi mode
              telemetryReset();
              telemetryInit();
          }
      }
      break;

    case ITEM_TELEMETRY_USR_BLADES:
      lcdDrawTextLeft(y, STR_BLADES);
      lcdDrawNumberAttUnit(TELEM_COL2+FWNUM, y, 2+g_model.telemetry.blades, attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.blades, MAX_BLADES);
      break;
#endif

    case ITEM_TELEMETRY_USR_VOLTAGE_SOURCE:
      lcdDrawTextLeft(y, STR_VOLTAGE);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.telemetry.voltsSource+1, attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.voltsSource, FRSKY_VOLTS_SOURCE_LAST);
      break;

    case ITEM_TELEMETRY_USR_CURRENT_SOURCE:
      lcdDrawTextLeft(y, STR_CURRENT);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_AMPSRC, g_model.telemetry.currentSource, attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.telemetry.currentSource, FRSKY_CURRENT_SOURCE_LAST);
      break;

    case ITEM_TELEMETRY_FAS_OFFSET:
      lcdDrawTextLeft(y, STR_FAS_OFFSET);
      lcdDrawNumberAttUnit(TELEM_COL2, y, g_model.telemetry.fasOffset, attr|LEFT|PREC1);
      lcdDrawNumberAttUnit(TELEM_COL2+6*FW, y, telemetryData.value.current, LEFT|PREC1);
      lcdDrawChar(TELEM_COL2+8*FW, y, 'A');
      if (attr) g_model.telemetry.fasOffset = checkIncDec(event, g_model.telemetry.fasOffset, -120, 120, EE_MODEL);
      break;

#if defined(VARIO)
    case ITEM_TELEMETRY_VARIO_LABEL:
      lcdDrawTextLeft(y, STR_VARIO);
      break;

    case ITEM_TELEMETRY_VARIO_SOURCE:
      lcdDrawTextLeft(y, STR_SOURCE);
      lcdDrawTextAtIndex(TELEM_COL2, y, STR_VARIOSRC, g_model.telemetry.varioSource, attr);
      if (attr) CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioSource, 0, VARIO_SOURCE_LAST);
      break;

    case ITEM_TELEMETRY_VARIO_RANGE:
      lcdDrawTextLeft(y, STR_LIMIT);
      lcdDrawNumberAttUnit(TELEM_COL2, y, -10+g_model.telemetry.varioMin, (menuHorizontalPosition<=0 ? attr : 0)|LEFT);
      lcdDrawNumberAttUnit(TELEM_COL2+7*FW-2, y, -5+g_model.telemetry.varioCenterMin, ((CURSOR_ON_LINE() || menuHorizontalPosition==1) ? attr : 0)|PREC1);
      lcdDrawNumberAttUnit(TELEM_COL2+10*FW, y, 5+g_model.telemetry.varioCenterMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==2) ? attr : 0)|PREC1);
      lcdDrawNumberAttUnit(TELEM_COL2+13*FW+2, y, 10+g_model.telemetry.varioMax, ((CURSOR_ON_LINE() || menuHorizontalPosition==3) ? attr : 0));
      if (attr && (s_editMode>0 || p1valdiff)) {
        switch (menuHorizontalPosition) {
        case 0:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioMin, -7, 7);
          break;
        case 1:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioCenterMin, -16, 5+min<int8_t>(10, g_model.telemetry.varioCenterMax+5));
          break;
        case 2:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioCenterMax, -5+max<int8_t>(-10, g_model.telemetry.varioCenterMin-5), +15);
          break;
        case 3:
          CHECK_INCDEC_MODELVAR(event, g_model.telemetry.varioMax, -7, 7);
          break;
        }
      }
      break;
#endif

    case ITEM_TELEMETRY_SCREEN_LABEL1:
    case ITEM_TELEMETRY_SCREEN_LABEL2: {
      uint8_t screenIndex = (k < ITEM_TELEMETRY_SCREEN_LABEL2 ? 1 : 2);
      lcdDrawStringWithIndex(0*FW, y, STR_SCREEN, screenIndex);
      bool screenType = g_model.telemetry.screensType & screenIndex;
      if (screenType != (bool)selectMenuItem(TELEM_SCRTYPE_COL, y, PSTR(""), STR_VTELEMSCREENTYPE, screenType, 0, 1, attr, event))
        g_model.telemetry.screensType ^= screenIndex;
      break;
    }

    case ITEM_TELEMETRY_SCREEN_LINE1:
    case ITEM_TELEMETRY_SCREEN_LINE2:
    case ITEM_TELEMETRY_SCREEN_LINE3:
    case ITEM_TELEMETRY_SCREEN_LINE4:
    case ITEM_TELEMETRY_SCREEN_LINE5:
    case ITEM_TELEMETRY_SCREEN_LINE6:
    case ITEM_TELEMETRY_SCREEN_LINE7:
    case ITEM_TELEMETRY_SCREEN_LINE8:

    {
      uint8_t screenIndex, lineIndex;
      if (k < ITEM_TELEMETRY_SCREEN_LABEL2) {
        screenIndex = 0;
        lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE1;
      } else {
        screenIndex = 1;
        lineIndex = k-ITEM_TELEMETRY_SCREEN_LINE5;
      }

      if (IS_BARS_SCREEN(screenIndex)) {
        FrSkyBarData & bar = g_model.telemetry.screens[screenIndex].bars[lineIndex];
        source_t barSource = bar.source;
        lcdDrawTextAtIndex(TELEM_COL1, y, STR_VTELEMCHNS, barSource, menuHorizontalPosition==0 ? attr : 0);
        if (barSource) {
          lcdPutsTelemetryChannelValue(TELEM_BARS_COLMIN, y, barSource-1, convertBarTelemValue(barSource, bar.barMin), (menuHorizontalPosition==1 ? attr : 0) | LEFT);
          lcdPutsTelemetryChannelValue(TELEM_BARS_COLMAX, y, barSource-1, convertBarTelemValue(barSource, 255-bar.barMax), (menuHorizontalPosition==2 ? attr : 0) | LEFT);
        } else if (attr && menuHorizontalPosition>0) {
          menuHorizontalPosition = 0;
        }
        if (attr && (s_editMode>0 || p1valdiff)) {
          switch (menuHorizontalPosition) {
          case 0:
            bar.source = CHECK_INCDEC_MODELVAR_ZERO(event, barSource, TELEM_DISPLAY_MAX);
            if (checkIncDec_Ret) {
              bar.barMin = 0;
              bar.barMax = 255 - maxBarTelemValue(bar.source);
            }
            break;
          case 1:
            bar.barMin = checkIncDec(event, bar.barMin, 0, 254-bar.barMax, EE_MODEL|NO_INCDEC_MARKS);
            break;
          case 2:
            bar.barMax = 255 - checkIncDec(event, 255-bar.barMax, bar.barMin+1, maxBarTelemValue(barSource), EE_MODEL|NO_INCDEC_MARKS);
            break;
          }
        }
      } else
      {
        for (uint8_t c=0; c<NUM_LINE_ITEMS; c++) {
          uint8_t cellAttr = (menuHorizontalPosition==c ? attr : 0);
          source_t & value = g_model.telemetry.screens[screenIndex].lines[lineIndex].sources[c];
          uint8_t pos[] = {INDENT_WIDTH, TELEM_COL2};
          lcdDrawTextAtIndex(pos[c], y, STR_VTELEMCHNS, value, cellAttr);
          if (cellAttr && (s_editMode>0 || p1valdiff)) {
            CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, value, (lineIndex==3 && c==0) ? TELEM_STATUS_MAX : TELEM_DISPLAY_MAX, isTelemetrySourceAvailable);
          }
        }
        if (attr && menuHorizontalPosition == NUM_LINE_ITEMS) {
          REPEAT_LAST_CURSOR_MOVE();
        }
      }
      break;
    }
    }
  }
}
# 1 "gui/menu_model_heli.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"


enum menuModelHeliItems {
  ITEM_HELI_SWASHTYPE,
  ITEM_HELI_COLLECTIVE,
  ITEM_HELI_SWASHRING,
  ITEM_HELI_ELEDIRECTION,
  ITEM_HELI_AILDIRECTION,
  ITEM_HELI_COLDIRECTION
};

#define HELI_PARAM_OFS (14*FW)

void menuModelHeli(uint8_t event)
{
  SIMPLE_MENU(STR_MENUHELISETUP, menuTabModel, e_Heli, 7);

  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<6; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t attr = (sub == i ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
    switch(i) {
    case ITEM_HELI_SWASHTYPE:
      g_model.swashR.type = selectMenuItem(HELI_PARAM_OFS, y, STR_SWASHTYPE, STR_VSWASHTYPE, g_model.swashR.type, 0, SWASH_TYPE_MAX, attr, event);
      break;

    case ITEM_HELI_COLLECTIVE:
      g_model.swashR.collectiveSource = selectMenuItem(HELI_PARAM_OFS, y, STR_COLLECTIVE, NULL, g_model.swashR.collectiveSource, 0, MIXSRC_LAST_CH, attr, event);
      putsMixerSource(HELI_PARAM_OFS, y, g_model.swashR.collectiveSource, attr);
      break;

    case ITEM_HELI_SWASHRING:
      lcdDrawTextLeft(y, STR_SWASHRING);
      lcdDrawNumberAttUnit(HELI_PARAM_OFS, y, g_model.swashR.value,  LEFT|attr);
      if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, g_model.swashR.value, 100);
      break;

    case ITEM_HELI_ELEDIRECTION:
      g_model.swashR.invertELE = selectMenuItem(HELI_PARAM_OFS, y, STR_ELEDIRECTION, STR_MMMINV, g_model.swashR.invertELE, 0, 1, attr, event);
      break;

    case ITEM_HELI_AILDIRECTION:
      g_model.swashR.invertAIL = selectMenuItem(HELI_PARAM_OFS, y, STR_AILDIRECTION, STR_MMMINV, g_model.swashR.invertAIL, 0, 1, attr, event);
      break;

    case ITEM_HELI_COLDIRECTION:
      g_model.swashR.invertCOL = selectMenuItem(HELI_PARAM_OFS, y, STR_COLDIRECTION, STR_MMMINV, g_model.swashR.invertCOL, 0, 1, attr, event);
      break;
    }
  }
}
# 1 "gui/menu_model_templates.cpp"
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


#include "../OpenAVRc.h"
#include "menu_model.h"


void menuModelTemplates(uint8_t event)
{
  SIMPLE_MENU(STR_MENUTEMPLATES, menuTabModel, e_Templates, 1+TMPL_COUNT);

  uint8_t sub = menuVerticalPosition - 1;

  if (sub < TMPL_COUNT) {
    if (warningResult) {
      warningResult = false;
      applyTemplate(sub);
      AUDIO_WARNING2();
    }
    if (event==EVT_KEY_BREAK(KEY_ENTER)) {
      POPUP_CONFIRMATION(STR_VTEMPLATES+1 + (sub * LEN2_VTEMPLATES));
      s_editMode = 0;
    }
  }

  coord_t y = MENU_HEADER_HEIGHT + 1;
  uint8_t k = 0;
  for (uint8_t i=0; i<LCD_LINES-1 && k<TMPL_COUNT; i++) {
    k = i+menuVerticalOffset;
    lcdDrawNumberNAtt(3*FW, y, k, (sub==k ? INVERS : 0)|LEADING0, 2);
    lcdDrawTextAtIndex(4*FW, y, STR_VTEMPLATES, k, (sub==k ? INVERS  : 0));
    y+=FH;
  }
}
# 1 "gui/menu_general.cpp"
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


#include "menu_general.h"


void menuGeneralSetup(uint8_t event);
void menuGeneralSdManager(uint8_t event);
void menuGeneralCustomFunctions(uint8_t event);
void menuGeneralTrainer(uint8_t event);
void menuGeneralVersion(uint8_t event);
void menuGeneralDiagKeys(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
void menuGeneralCalib(uint8_t event);




# 1 "gui/menu_general_setup.cpp"
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


#include "../OpenAVRc.h"
#include "menu_general.h"


#define RADIO_SETUP_2ND_COLUMN  (LCD_W-6*FW-3-MENUS_SCROLLBAR_WIDTH)
#define RADIO_SETUP_TIME_COLUMN (FW*15+9)
#define RADIO_SETUP_DATE_COLUMN (FW*15+7)

#define SLIDER_5POS(y, value, label, event, attr) { \
    int8_t tmp = value; \
    displaySlider(RADIO_SETUP_2ND_COLUMN, y, 2+tmp, 4, attr); \
    value = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, label, NULL, tmp, -2, +2, attr, event); \
  }

#if defined(SPLASH) && !defined(FSPLASH)
#define CASE_SPLASH_PARAM(x) x,
#else
#define CASE_SPLASH_PARAM(x)
#endif

#if defined(BATTGRAPH)
#define CASE_BATTGRAPH(x) x,
#else
#define CASE_BATTGRAPH(x)
#endif

enum menuGeneralSetupItems {
  CASE_RTCLOCK(ITEM_SETUP_DATE)
  CASE_RTCLOCK(ITEM_SETUP_TIME)
  CASE_BATTGRAPH(ITEM_SETUP_BATT_RANGE)
  ITEM_SETUP_SOUND_LABEL,
  CASE_AUDIO(ITEM_SETUP_BEEP_MODE)
  CASE_BUZZER(ITEM_SETUP_BUZZER_MODE)
  //CASE_VOICE(ITEM_SETUP_SPEAKER_VOLUME)
  ITEM_SETUP_BEEP_LENGTH,
  CASE_AUDIO(ITEM_SETUP_SPEAKER_PITCH)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LABEL)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_MODE)
  CASE_HAPTIC(ITEM_SETUP_HAPTIC_LENGTH)
  //CASE_HAPTIC(ITEM_SETUP_HAPTIC_STRENGTH)
  ITEM_SETUP_CONTRAST,
  ITEM_SETUP_ALARMS_LABEL,
  ITEM_SETUP_BATTERY_WARNING,
  ITEM_SETUP_INACTIVITY_ALARM,
  ITEM_SETUP_MEMORY_WARNING,
  ITEM_SETUP_ALARM_WARNING,
  IF_ROTARY_ENCODERS(ITEM_SETUP_RE_NAVIGATION)
  ITEM_SETUP_BACKLIGHT_LABEL,
  ITEM_SETUP_BACKLIGHT_MODE,
  ITEM_SETUP_BACKLIGHT_DELAY,
  CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF)
  CASE_PWM_BACKLIGHT(ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON)
  ITEM_SETUP_FLASH_BEEP,
  CASE_SPLASH_PARAM(ITEM_SETUP_DISABLE_SPLASH)
  CASE_GPS(ITEM_SETUP_TIMEZONE)
  CASE_GPS(ITEM_SETUP_GPSFORMAT)
  CASE_PXX(ITEM_SETUP_COUNTRYCODE)
  IF_FAI_CHOICE(ITEM_SETUP_FAI)
  CASE_MAVLINK(ITEM_MAVLINK_BAUD)
  ITEM_SETUP_RX_CHANNEL_ORD,
  ITEM_SETUP_STICK_MODE_LABELS,
  ITEM_SETUP_STICK_MODE,
  ITEM_SETUP_MAX
};

#define COL_TX_MODE LABEL(TX_MODE)

void menuGeneralSetup(uint8_t event)
{
#if defined(RTCLOCK)
  struct gtm t;
  gettime(&t);

  if ((menuVerticalPosition==ITEM_SETUP_DATE+1 || menuVerticalPosition==ITEM_SETUP_TIME+1) &&
      (s_editMode>0) &&
      (event==EVT_KEY_FIRST(KEY_ENTER) || event==EVT_KEY_FIRST(KEY_EXIT) || IS_ROTARY_BREAK(event) || IS_ROTARY_LONG(event))) {
    // set the date and time into RTC chip
    rtcSetTime(&t);
  }
#endif

#if defined(FAI_CHOICE)
  if (warningResult) {
    warningResult = false;
    g_eeGeneral.fai = true;
    eeDirty(EE_GENERAL);
  }
#endif

  MENU(STR_MENURADIOSETUP, menuTabGeneral, e_Setup, ITEM_SETUP_MAX+1, {0, CASE_RTCLOCK(2) CASE_RTCLOCK(2) CASE_BATTGRAPH(1) LABEL(SOUND), CASE_AUDIO(0) CASE_BUZZER(0) /*CASE_VOICE(0)*/ 0, CASE_AUDIO(0) CASE_HAPTIC(LABEL(HAPTIC)) CASE_HAPTIC(0) CASE_HAPTIC(0) /*CASE_HAPTIC(0)*/ 0, LABEL(ALARMS), 0, 0, 0, 0, IF_ROTARY_ENCODERS(0) LABEL(BACKLIGHT), 0, 0, CASE_PWM_BACKLIGHT(0) CASE_PWM_BACKLIGHT(0) 0, CASE_SPLASH_PARAM(0) CASE_GPS(0) CASE_GPS(0) CASE_PXX(0) IF_FAI_CHOICE(0) CASE_MAVLINK(0) 0, COL_TX_MODE, 1/*to force edit mode*/});

  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
#if defined(RTCLOCK)
    case ITEM_SETUP_DATE:
      lcdDrawTextLeft(y, STR_DATE);
      lcdDrawChar(RADIO_SETUP_DATE_COLUMN, y, '-');
      lcdDrawChar(RADIO_SETUP_DATE_COLUMN+3*FW-2, y, '-');
      for (uint8_t j=0; j<3; j++) {
        uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
        switch (j) {
        case 0:
          lcdDrawNumberAttUnit(RADIO_SETUP_DATE_COLUMN, y, t.tm_year+1900, rowattr);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_year = checkIncDec(event, t.tm_year, 112, 200, 0);
          break;
        case 1:
          lcdDrawNumberNAtt(RADIO_SETUP_DATE_COLUMN+3*FW-2, y, t.tm_mon+1, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_mon = checkIncDec(event, t.tm_mon, 0, 11, 0);
          break;
        case 2: {
          int16_t year = 1900 + t.tm_year;
          int8_t dlim = (((((year%4==0) && (year%100!=0)) || (year%400==0)) && (t.tm_mon==1)) ? 1 : 0);
          static const pm_uint8_t dmon[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
          dlim += pgm_read_byte_near(&dmon[t.tm_mon]);
          lcdDrawNumberNAtt(RADIO_SETUP_DATE_COLUMN+6*FW-4, y, t.tm_mday, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_mday = checkIncDec(event, t.tm_mday, 1, dlim, 0);
          break;
        }
        }
      }
      if (attr && checkIncDec_Ret) {
        g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
      }
      break;

    case ITEM_SETUP_TIME:
      lcdDrawTextLeft(y, STR_TIME);
      lcdDrawChar(RADIO_SETUP_TIME_COLUMN+1, y, ':');
      lcdDrawChar(RADIO_SETUP_TIME_COLUMN+3*FW-2, y, ':');
      for (uint8_t j=0; j<3; j++) {
        uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
        switch (j) {
        case 0:
          lcdDrawNumberNAtt(RADIO_SETUP_TIME_COLUMN, y, t.tm_hour, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_hour = checkIncDec(event, t.tm_hour, 0, 23, 0);
          break;
        case 1:
          lcdDrawNumberNAtt(RADIO_SETUP_TIME_COLUMN+3*FWNUM, y, t.tm_min, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_min = checkIncDec(event, t.tm_min, 0, 59, 0);
          break;
        case 2:
          lcdDrawNumberNAtt(RADIO_SETUP_TIME_COLUMN+6*FWNUM, y, t.tm_sec, rowattr|LEADING0, 2);
          if (rowattr && (s_editMode>0 || p1valdiff)) t.tm_sec = checkIncDec(event, t.tm_sec, 0, 59, 0);
          break;
        }
      }
      if (attr && checkIncDec_Ret)
        g_rtcTime = gmktime(&t); // update local timestamp and get wday calculated
      break;
#endif

#if defined(BATTGRAPH)
    case ITEM_SETUP_BATT_RANGE:
      lcdDrawTextLeft(y, STR_BATTERY_RANGE);
      lcdPutsVolts(RADIO_SETUP_2ND_COLUMN, y,  g_eeGeneral.vBatMin, (menuHorizontalPosition==0 ? attr : 0)|LEFT|NO_UNIT);
      lcdDrawChar(lcdLastPos, y, '-');
      lcdPutsVolts(lcdLastPos+FW, y, g_eeGeneral.vBatMax, (menuHorizontalPosition>0 ? attr : 0)|LEFT|NO_UNIT);
      if (attr && s_editMode>0) {
        if (menuHorizontalPosition==0)
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMin, 30, g_eeGeneral.vBatMax-10); // min=3.0V
        else
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMax, g_eeGeneral.vBatMin+10, 160); // max=16.0V
      }
      break;
#endif

    case ITEM_SETUP_SOUND_LABEL:
      lcdDrawTextLeft(y, STR_SOUND_LABEL);
      break;

#if defined(AUDIO)
    case ITEM_SETUP_BEEP_MODE:
      g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
#if defined(FRSKY)
      if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
      break;

#if defined(BUZZER)
    case ITEM_SETUP_BUZZER_MODE:
      g_eeGeneral.buzzerMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_BUZZER, STR_VBEEPMODE, g_eeGeneral.buzzerMode, -2, 1, attr, event);
#if defined(FRSKY)
      if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
      break;
#endif
#elif defined(BUZZER)
    case ITEM_SETUP_BUZZER_MODE:
      g_eeGeneral.beepMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_SPEAKER, STR_VBEEPMODE, g_eeGeneral.beepMode, -2, 1, attr, event);
#if defined(FRSKY)
      if (attr && checkIncDec_Ret) frskySendAlarms();
#endif
      break;
#endif

#if defined(VOICE)
      /*case ITEM_SETUP_SPEAKER_VOLUME:
      {
        lcdDrawTextLeft(y, STR_SPEAKER_VOLUME);
        uint8_t b = g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF;
        displaySlider(RADIO_SETUP_2ND_COLUMN, y, b, VOLUME_LEVEL_MAX, attr);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, b, 0, VOLUME_LEVEL_MAX);
          if (checkIncDec_Ret) {
            g_eeGeneral.speakerVolume = (int8_t)b-VOLUME_LEVEL_DEF;
            setVolume(b);
          }
        }
        break;
      } Bracame not used but will be used ;-)*/
#endif


    case ITEM_SETUP_BEEP_LENGTH:
      SLIDER_5POS(y, g_eeGeneral.beepLength, STR_BEEP_LENGTH, event, attr);
      break;

#if defined(AUDIO)
    case ITEM_SETUP_SPEAKER_PITCH:
      lcdDrawTextLeft( y, STR_SPKRPITCH);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.speakerPitch, attr|LEFT);
      if (attr) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.speakerPitch, 0, 20);
      }
      break;
#endif


#if defined(HAPTIC)
    case ITEM_SETUP_HAPTIC_LABEL:
      lcdDrawTextLeft(y, STR_HAPTIC_LABEL);
      break;

    case ITEM_SETUP_HAPTIC_MODE:
      g_eeGeneral.hapticMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MODE, STR_VBEEPMODE, g_eeGeneral.hapticMode, -2, 1, attr, event);
      break;

    case ITEM_SETUP_HAPTIC_LENGTH:
      SLIDER_5POS(y, g_eeGeneral.hapticLength, STR_LENGTH, event, attr);
      break;

      /*case ITEM_SETUP_HAPTIC_STRENGTH:
        SLIDER_5POS(y, g_eeGeneral.hapticStrength, STR_HAPTICSTRENGTH, event, attr);
        break; Bracame not used */
#endif

    case ITEM_SETUP_CONTRAST:
      lcdDrawTextLeft(y, STR_CONTRAST);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.contrast, attr|LEFT);
      if (attr) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, CONTRAST_MIN, CONTRAST_MAX);
        lcdSetContrast();
      }
      break;

    case ITEM_SETUP_ALARMS_LABEL:
      lcdDrawTextLeft(y, STR_ALARMS_LABEL);
      break;

    case ITEM_SETUP_BATTERY_WARNING:
      lcdDrawTextLeft(y, STR_BATTERYWARNING);
      lcdPutsVolts(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.vBatWarn, attr|LEFT);
      if(attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatWarn, g_eeGeneral.vBatMin, g_eeGeneral.vBatMax); //Min- Max
      break;

    case ITEM_SETUP_MEMORY_WARNING: {
      uint8_t b = 1-g_eeGeneral.disableMemoryWarning;
      g_eeGeneral.disableMemoryWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_MEMORYWARNING, attr, event);
      break;
    }

    case ITEM_SETUP_ALARM_WARNING: {
      uint8_t b = 1-g_eeGeneral.disableAlarmWarning;
      g_eeGeneral.disableAlarmWarning = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_ALARMWARNING, attr, event);
      break;
    }

    case ITEM_SETUP_INACTIVITY_ALARM:
      lcdDrawTextLeft( y,STR_INACTIVITYALARM);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.inactivityTimer, attr|LEFT);
      lcdDrawChar(lcdLastPos, y, 'm');
      if(attr) g_eeGeneral.inactivityTimer = checkIncDec(event, g_eeGeneral.inactivityTimer, 0, 250, EE_GENERAL); //0..250minutes
      break;

#if ROTARY_ENCODERS > 0
    case ITEM_SETUP_RE_NAVIGATION:
      g_eeGeneral.reNavigation = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_RENAVIG, STR_VRENAVIG, g_eeGeneral.reNavigation, 0, NUM_ROTARY_ENCODERS, attr, event);
      if (attr && checkIncDec_Ret) {
        g_rotenc[NAVIGATION_RE_IDX()] = 0;
      }
      break;
#endif

    case ITEM_SETUP_BACKLIGHT_LABEL:
      lcdDrawTextLeft(y, STR_BACKLIGHT_LABEL);
      break;

    case ITEM_SETUP_BACKLIGHT_MODE:
      g_eeGeneral.backlightMode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MODE, STR_VBLMODE, g_eeGeneral.backlightMode, e_backlight_mode_off, e_backlight_mode_on, attr, event);
      break;

    case ITEM_SETUP_FLASH_BEEP:
      g_eeGeneral.alarmsFlash = onoffMenuItem(g_eeGeneral.alarmsFlash, RADIO_SETUP_2ND_COLUMN, y, STR_ALARM, attr, event ) ;
      break;

    case ITEM_SETUP_BACKLIGHT_DELAY:
      lcdDrawTextLeft(y, STR_BLDELAY);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.lightAutoOff*5, attr|LEFT);
      lcdDrawChar(lcdLastPos, y, 's');
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.lightAutoOff, 0, 600/5);
      break;


#if defined(PWM_BACKLIGHT)
    case ITEM_SETUP_BACKLIGHT_BRIGHTNESS_OFF:
      lcdDrawTextLeft(y, STR_BLOFFBRIGHTNESS);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.blOffBright, attr|LEFT);
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.blOffBright, 0, 15);
      break;

    case ITEM_SETUP_BACKLIGHT_BRIGHTNESS_ON:
      lcdDrawTextLeft(y, STR_BLONBRIGHTNESS);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, 15-g_eeGeneral.blOnBright, attr|LEFT);
      if (attr) g_eeGeneral.blOnBright = 15 - checkIncDecGen(event, 15-g_eeGeneral.blOnBright, 0, 15);
      break;
#endif

#if defined(SPLASH) && !defined(FSPLASH)
    case ITEM_SETUP_DISABLE_SPLASH: {
      uint8_t b = 1-g_eeGeneral.splashMode;
      g_eeGeneral.splashMode = 1 - onoffMenuItem(b, RADIO_SETUP_2ND_COLUMN, y, STR_SPLASHSCREEN, attr, event);
      break;
    }
#endif

#if defined(FRSKY) && defined(GPS)
    case ITEM_SETUP_TIMEZONE:
      lcdDrawTextLeft(y, STR_TIMEZONE);
      lcdDrawNumberAttUnit(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.timezone, attr|LEFT);
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.timezone, -12, 12);
      break;

    case ITEM_SETUP_GPSFORMAT:
      g_eeGeneral.gpsFormat = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_GPSCOORD, STR_GPSFORMAT, g_eeGeneral.gpsFormat, 0, 1, attr, event);
      break;
#endif

#if defined(PXX)
    case ITEM_SETUP_COUNTRYCODE:
      g_eeGeneral.countryCode = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_COUNTRYCODE, STR_COUNTRYCODES, g_eeGeneral.countryCode, 0, 2, attr, event);
      break;
#endif


#if defined(FAI_CHOICE)
    case ITEM_SETUP_FAI:
      onoffMenuItem(g_eeGeneral.fai, RADIO_SETUP_2ND_COLUMN, y, PSTR("FAI Mode"), attr, event);
      if (attr && checkIncDec_Ret) {
        if (g_eeGeneral.fai)
          POPUP_WARNING(PSTR("FAI\001mode blocked!"));
        else
          POPUP_CONFIRMATION(PSTR("FAI mode?"));
      }
      break;
#endif

#if defined(MAVLINK)
    case ITEM_MAVLINK_BAUD:
      g_eeGeneral.mavbaud = selectMenuItem(RADIO_SETUP_2ND_COLUMN, y, STR_MAVLINK_BAUD_LABEL, STR_MAVLINK_BAUDS, g_eeGeneral.mavbaud, 0, 7, attr, event);
      break;
#endif


    case ITEM_SETUP_RX_CHANNEL_ORD:
      lcdDrawTextLeft(y, STR_RXCHANNELORD); // RAET->AETR
      for (uint8_t i=1; i<=4; i++) {
        lcdPutsChnLetter(RADIO_SETUP_2ND_COLUMN - FW + i*FW, y, channel_order(i), attr);
      }
      if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.templateSetup, 0, 23);
      break;

    case ITEM_SETUP_STICK_MODE_LABELS:
      lcdDrawTextLeft(y, NO_INDENT(STR_MODE));
      for (uint8_t i=0; i<4; i++) {
        lcd_img((6+4*i)*FW, y, sticks, i, 0);
      }
      break;

    case ITEM_SETUP_STICK_MODE:
      lcdDrawCharAtt(2*FW, y, '1'+g_eeGeneral.stickMode, attr);
      for (uint8_t i=0; i<4; i++) {
        putsMixerSource((6+4*i)*FW, y, MIXSRC_Rud + pgm_read_byte_near(modn12x3 + 4*g_eeGeneral.stickMode + i), 0);
      }
      if (attr && s_editMode>0) {
        CHECK_INCDEC_GENVAR(event, g_eeGeneral.stickMode, 0, 3);
      } else if (stickMode != g_eeGeneral.stickMode) {
        sendStopPulses();
        stickMode = g_eeGeneral.stickMode;
        checkTHR();
        startPulses(PROTOCMD_INIT);
        clearKeyEvents();
      }
#if defined(ROTARY_ENCODER_NAVIGATION)
      MOVE_CURSOR_FROM_HERE();
#endif
      break;
    }
  }
}
# 1 "gui/menu_general_trainer.cpp"
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


#include "../OpenAVRc.h"
#include "menu_general.h"


#define TRAINER_CALIB_POS 8

void menuGeneralTrainer(uint8_t event)
{
  bool slave = IS_WAIT_PUPIL_STATE();

  MENU(STR_MENUTRAINER, menuTabGeneral, e_Trainer, (slave ? 1 : 7), {0, 2, 2, 2, 2, 0/*, 0*/});

  if (slave) {
    lcdDrawText(7*FW, 4*FH, STR_SLAVE);
  } else {
    uint8_t attr;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);

    lcdDrawText(3*FW, MENU_HEADER_HEIGHT+1, STR_MODESRC);

    uint8_t y = MENU_HEADER_HEIGHT + 1 + FH;

    for (uint8_t i=1; i<=NUM_STICKS; i++) {
      uint8_t chan = channel_order(i);
      volatile TrainerMix *td = &g_eeGeneral.trainer.mix[chan-1];

      putsMixerSource(0, y, MIXSRC_Rud-1+chan, (menuVerticalPosition==i && CURSOR_ON_LINE()) ? INVERS : 0);

      for (uint8_t j=0; j<3; j++) {

        attr = ((menuVerticalPosition==i && menuHorizontalPosition==j) ? blink : 0);

        switch(j) {
        case 0:
          lcdDrawTextAtIndex(4*FW, y, STR_TRNMODE, td->mode, attr);
          if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->mode, 0, 2);
          break;

        case 1:
          lcdDrawNumberAttUnit(11*FW, y, td->studWeight, attr);
          if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->studWeight, -125, 125);
          break;

        case 2:
          lcdDrawTextAtIndex(12*FW, y, STR_TRNCHN, td->srcChn, attr);
          if (attr&BLINK) CHECK_INCDEC_GENVAR(event, td->srcChn, 0, 3);
          break;
        }
      }
      y += FH;
    }

    attr = (menuVerticalPosition==5) ? blink : 0;
    lcdDrawTextLeft(MENU_HEADER_HEIGHT+1+5*FH, STR_MULTIPLIER);
    lcdDrawNumberAttUnit(LEN_MULTIPLIER*FW+3*FW, MENU_HEADER_HEIGHT+1+5*FH, g_eeGeneral.PPM_Multiplier+10, attr|PREC1);
    if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.PPM_Multiplier, -10, 40);

    attr = (menuVerticalPosition==6) ? INVERS : 0;
    if (attr) s_editMode = 0;
    lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+1+6*FH, STR_CAL, attr);
    for (uint8_t i=0; i<4; i++) {
      uint8_t x = (i*TRAINER_CALIB_POS+16)*FW/2;
#if defined (PPM_UNIT_PERCENT_PREC1)
      lcdDrawNumberAttUnit(x, MENU_HEADER_HEIGHT+1+6*FH, (ppmInput[i]-g_eeGeneral.trainer.calib[i])*2, PREC1);
#else
      lcdDrawNumberAttUnit(x, MENU_HEADER_HEIGHT+1+6*FH, (ppmInput[i]-g_eeGeneral.trainer.calib[i])/5, 0);
#endif
    }

    if (attr) {
      if (event==EVT_KEY_LONG(KEY_ENTER)) {
        memcpy(g_eeGeneral.trainer.calib, ppmInput, sizeof(g_eeGeneral.trainer.calib));
        eeDirty(EE_GENERAL);
        AUDIO_WARNING1();
      }
    }
  }
}
# 1 "gui/menu_general_version.cpp"
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


#include "../OpenAVRc.h"
#include "menu_general.h"


void menuGeneralVersion(uint8_t event)
{
#define VERS_ITEMS_COUNT 2

  MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, VERS_ITEMS_COUNT, {0, 3});

  uint8_t sub = menuVerticalPosition - 1;
  uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
  uint8_t attr = (sub == menuVerticalOffset ? blink : 0);

  lcdDrawTextLeft(MENU_HEADER_HEIGHT+FH, vers_stamp);

#define ID_OFFSET 4 + 3*FW

  lcdDrawTextLeft(7*FH, STR_RFID);
  for (uint8_t j=0; j<4; j++) {
    uint8_t rowattr = (menuHorizontalPosition==j ? attr : 0);
    switch (j) {
    case 0:
      lcd_outhex(2, ID_OFFSET + 0*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[3], rowattr);
      if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[3], 0, 0xFF);
      break;
    case 1:
      lcd_outhex(2, ID_OFFSET + 2*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[2], rowattr);
      if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[2], 0, 0xFF);
      break;
    case 2:
      lcd_outhex(2, ID_OFFSET + 4*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[1], rowattr);
      if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[1], 0, 0xFF);
      break;
    case 3:
      lcd_outhex(2, ID_OFFSET + 6*FW, 7*FH, g_eeGeneral.fixed_ID.ID_8[0], rowattr);
      if (rowattr && (s_editMode>0 || p1valdiff)) CHECK_INCDEC_GENVAR(event, g_eeGeneral.fixed_ID.ID_8[0], 0, 0xFF);
      break;
    }
  }
}
# 1 "gui/menu_general_calib.cpp"
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


#include "../OpenAVRc.h"
#include "menu_general.h"

#define XPOT_DELTA 10
#define XPOT_DELAY 10 /* cycles */

void menuCommonCalib(uint8_t event)
{
  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) { // get low and high vals for sticks and trims
    int16_t vt = anaIn(i);
    reusableBuffer.calib.loVals[i] = min(vt, reusableBuffer.calib.loVals[i]);
    reusableBuffer.calib.hiVals[i] = max(vt, reusableBuffer.calib.hiVals[i]);
    if (i >= POT1 && i <= POT_LAST) {
      if (IS_POT_WITHOUT_DETENT(i)) {
        reusableBuffer.calib.midVals[i] = (reusableBuffer.calib.hiVals[i] + reusableBuffer.calib.loVals[i]) / 2;
      }
    }
  }

  calibrationState = reusableBuffer.calib.state; // make sure we don't scroll while calibrating

  switch (event) {
  case EVT_ENTRY:
    reusableBuffer.calib.state = 0;
    break;

  case EVT_KEY_BREAK(KEY_ENTER):
    reusableBuffer.calib.state++;
    break;
  }

  switch (reusableBuffer.calib.state) {
  case 0:
    // START CALIBRATION
    if (!READ_ONLY()) {
      lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUTOSTART);
    }
    break;

  case 1:
    // SET MIDPOINT
    lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+FH, STR_SETMIDPOINT, INVERS);
    lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUWHENDONE);

    for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
      reusableBuffer.calib.loVals[i] = 15000;
      reusableBuffer.calib.hiVals[i] = -15000;
      reusableBuffer.calib.midVals[i] = anaIn(i);
    }
    break;

  case 2:
    // MOVE STICKS/POTS
    STICK_SCROLL_DISABLE();
    lcdDrawTextAtt(0*FW, MENU_HEADER_HEIGHT+FH, STR_MOVESTICKSPOTS, INVERS);
    lcdDrawTextLeft(MENU_HEADER_HEIGHT+2*FH, STR_MENUWHENDONE);

    for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
      if (abs(reusableBuffer.calib.loVals[i]-reusableBuffer.calib.hiVals[i]) > 50) {
        g_eeGeneral.calib[i].mid = reusableBuffer.calib.midVals[i];
        int16_t v = reusableBuffer.calib.midVals[i] - reusableBuffer.calib.loVals[i];
        g_eeGeneral.calib[i].spanNeg = v - v/STICK_TOLERANCE;
        v = reusableBuffer.calib.hiVals[i] - reusableBuffer.calib.midVals[i];
        g_eeGeneral.calib[i].spanPos = v - v/STICK_TOLERANCE;
      }
    }
    break;

  case 3:
    g_eeGeneral.chkSum = evalChkSum();
    eeDirty(EE_GENERAL);
    reusableBuffer.calib.state = 4;
    break;

  default:
    reusableBuffer.calib.state = 0;
    break;
  }

  doMainScreenGraphics();
}

void menuGeneralCalib(uint8_t event)
{
  check_simple(event, e_Calib, menuTabGeneral, DIM(menuTabGeneral), 0);

  if (menuEvent) {
    calibrationState = 0;
  }

  TITLE(STR_MENUCALIBRATION);
  menuCommonCalib(READ_ONLY() ? 0 : event);
}

void menuFirstCalib(uint8_t event)
{
  if (event == EVT_KEY_BREAK(KEY_EXIT) || reusableBuffer.calib.state == 4) {
    calibrationState = 0;
    chainMenu(menuMainView);
  } else {
    lcdDrawTextCenter(0*FH, MENUCALIBRATION);
    lcdInvertLine(0);
    menuCommonCalib(event);
  }
}
# 1 "gui/menu_general_diagkeys.cpp"
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


#include "../OpenAVRc.h"
#include "menu_general.h"


void displayKeyState(uint8_t x, uint8_t y, EnumKeys key)
{
  uint8_t t = switchState(key);
  lcdDrawCharAtt(x, y, t+'0', t ? INVERS : 0);
}

void menuGeneralDiagKeys(uint8_t event)
{
  SIMPLE_MENU(STR_MENUDIAG, menuTabGeneral, e_Keys, 1);

  lcdDrawText(14*FW, MENU_HEADER_HEIGHT+2*FH, STR_VTRIM);

  for(uint8_t i=0; i<9; i++) {
    coord_t y;

    if (i<8) {
      y = MENU_HEADER_HEIGHT + FH*3 + FH*(i/2);
      if (i&1) lcd_img(14*FW, y, sticks, i/2, 0);
      displayKeyState(i&1? 20*FW : 18*FW, y, (EnumKeys)(TRM_BASE+i));
    }

    if (i<6) {
      y = i*FH+MENU_HEADER_HEIGHT+FH;
      lcdDrawTextAtIndex(0, y, STR_VKEYS, (5-i), 0);
      displayKeyState(5*FW+2, y, (EnumKeys)(KEY_MENU+(5-i)));
    }

    if (i != SW_ID0-SW_BASE) {
      y = MENU_HEADER_HEIGHT+i*FH-2*FH;
      lcdPutsSwitches(8*FW, y, i+1, 0); //ohne off,on
      displayKeyState(11*FW+2, y, (EnumKeys)(SW_BASE+i));
    }
  }

#if defined(ROTARY_ENCODERS) || defined(ROTARY_ENCODER_NAVIGATION)
  for(uint8_t i=0; i<DIM(g_rotenc); i++) {
    coord_t y = MENU_HEADER_HEIGHT /* ??? + 1 ??? */ + i*FH;
    lcdDrawTextAtIndex(14*FW, y, STR_VRENCODERS, i, 0);
    int16_t rex = getRotaryEncoder(i);
    rex /= 8;
    lcdDrawNumberNAtt(18*FW, y, rex, LEFT|(switchState((EnumKeys)(BTN_REa+i)) ? INVERS : 0));
  }
#endif

}
# 1 "gui/menu_general_diaganas.cpp"
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


#include "menu_general.h"

#define TR_ANAIN_POT_BAT "Pt1""Pt2""Pt3""Bat"
const pm_char STR_TR_ANAIN_POT_BAT[] PROGMEM = TR_ANAIN_POT_BAT;


void menuGeneralDiagAna(uint8_t event)
{
#define ANAS_ITEMS_COUNT 2

  SIMPLE_MENU(STR_MENUANA, menuTabGeneral, e_Ana, ANAS_ITEMS_COUNT);

  STICK_SCROLL_DISABLE();

  uint8_t numLoop = NUM_STICKS+NUM_POTS;

  for (uint8_t i=0; i<numLoop+1; i++) { // Add battery input
    coord_t y = MENU_HEADER_HEIGHT + 1 + (i/2)*FH;
    uint8_t x = i&1 ? 65 : 2;
    uint8_t attr = (anaIn(i) < 1 || anaIn(i) > 0x7FD) ? INVERS : 0;
    if (i<4) {
      putsMixerSource(x, y, MIXSRC_Rud + pgm_read_byte_near(modn12x3 + 4*g_eeGeneral.stickMode + i), attr);
    } else {
      lcdDrawSizedTextAtt(x, y, STR_TR_ANAIN_POT_BAT+3*(i-4), 3, attr);
    }
    lcdDrawChar(x+18, y, ':');
    lcd_outhex(4, x+3*FW+4, y, anaIn(i), attr);
    if (i<numLoop)
      lcd_outdez8(x+10*FW+2, y, (int16_t)calibratedStick[CONVERT_MODE(i)]*25/256);
  }
  lcdDrawTextLeft(6*FH-2, STR_BATT_CALIB);
  lcdPutsVolts(LEN_CALIB_FIELDS*FW+4*FW, 6*FH-2, g_vbat10mV, (menuVerticalPosition==1 ? INVERS : 0)|PREC2);
  if (menuVerticalPosition==1)
    CHECK_INCDEC_GENVAR(event, g_eeGeneral.txVoltageCalibration, -127, 127);

}
# 1 "gui/menu_general_sdmanager.cpp"
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


#include "../OpenAVRc.h"
#include "menu_general.h"


void menuGeneralSdManagerInfo(uint8_t event)
{
  SIMPLE_SUBMENU(STR_SD_INFO_TITLE, 1);

  lcdDrawTextLeft(2*FH, STR_SD_TYPE);
  lcdDrawText(10*FW, 2*FH, STR_SD_CARD);

  lcdDrawTextLeft(3*FH, STR_SD_SIZE);
  lcdDrawNumberAttUnit(10*FW, 3*FH, sdGetSize(), LEFT);
  lcdDrawChar(lcdLastPos, 3*FH, 'M');

  lcdDrawTextLeft(4*FH, STR_SD_SECTORS);
  lcdDrawNumberAttUnit(10*FW, 4*FH, sdGetNoSectors(), LEFT);
  lcdDrawChar(lcdLastPos, 4*FH, 'k');
}

inline bool isFilenameGreater(bool isfile, const char * fn, const char * line)
{
  return (isfile && !line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) > 0);
}

inline bool isFilenameLower(bool isfile, const char * fn, const char * line)
{
  return (!isfile && line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) < 0);
}

void onSdManagerMenu(const char *result)
{
  TCHAR lfn[_MAX_LFN+1];

  uint8_t index = menuVerticalPosition-1-menuVerticalOffset;
  if (result == STR_SD_INFO) {
    pushMenu(menuGeneralSdManagerInfo);
  } else if (result == STR_DELETE_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat_P(lfn, PSTR("/"));
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    f_unlink(lfn);
    strncpy(statusLineMsg, reusableBuffer.sdmanager.lines[index], 13);
    strcpy_P(statusLineMsg+min((uint8_t)strlen(statusLineMsg), (uint8_t)13), STR_REMOVED);
    showStatusLine();
    if ((uint16_t)menuVerticalPosition == reusableBuffer.sdmanager.count) menuVerticalPosition--;
    reusableBuffer.sdmanager.offset = menuVerticalOffset-1;
  }
}

void menuGeneralSdManager(uint8_t _event)
{
  FILINFO fno;
  DIR dir;
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);


  uint8_t event = ((READ_ONLY() && EVT_KEY_MASK(_event) == KEY_ENTER) ? 0 : _event);
  SIMPLE_MENU(STR_SD_CARD, menuTabGeneral, e_Sd, 1+reusableBuffer.sdmanager.count);

  if (s_editMode > 0)
    s_editMode = 0;

  switch(_event) {
  case EVT_ENTRY:
    f_chdir(ROOT_PATH);
    reusableBuffer.sdmanager.offset = 65535;
    break;

    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_FIRST(KEY_RIGHT):
  case EVT_KEY_FIRST(KEY_ENTER): {
    if (menuVerticalPosition > 0) {
      vertpos_t index = menuVerticalPosition-1-menuVerticalOffset;
      if (!reusableBuffer.sdmanager.lines[index][SD_SCREEN_FILE_LENGTH+1]) {
        f_chdir(reusableBuffer.sdmanager.lines[index]);
        menuVerticalOffset = 0;
        menuVerticalPosition = 1;
        reusableBuffer.sdmanager.offset = 65535;
        killEvents(_event);
        break;
      }
    }
    if (!IS_ROTARY_BREAK(_event) || menuVerticalPosition==0)
      break;
    // no break;
  }

  case EVT_KEY_LONG(KEY_ENTER):
    killEvents(_event);
    if (menuVerticalPosition == 0) {
      POPUP_MENU_ADD_ITEM(STR_SD_INFO);
    } else {
      if (!READ_ONLY()) {
        POPUP_MENU_ADD_ITEM(STR_DELETE_FILE);
      }
    }
    popupMenuHandler = onSdManagerMenu;
    break;
  }

  if (reusableBuffer.sdmanager.offset != menuVerticalOffset) {
    if (menuVerticalOffset == 0) {
      reusableBuffer.sdmanager.offset = 0;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    } else if (menuVerticalOffset == reusableBuffer.sdmanager.count-7) {
      reusableBuffer.sdmanager.offset = menuVerticalOffset;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    } else if (menuVerticalOffset > reusableBuffer.sdmanager.offset) {
      memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[6], 0xff, SD_SCREEN_FILE_LENGTH);
      reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = 1;
    } else {
      memmove(reusableBuffer.sdmanager.lines[1], reusableBuffer.sdmanager.lines[0], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
    }

    reusableBuffer.sdmanager.count = 0;
    char *fn;   /* This function is assuming non-Unicode cfg. */
    FRESULT res = f_opendir(&dir, ".");        /* Open the directory */
    if (res == FR_OK) {
      for (;;) {
        res = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fname[0] == '.' && fno.fname[1] == '\0') continue;             /* Ignore dot entry */
#if _USE_LFN
        fn = *fno.lfname ? fno.lfname : fno.fname;
#else
        fn = fno.fname;
#endif
        if (strlen(fn) > SD_SCREEN_FILE_LENGTH) continue;

        reusableBuffer.sdmanager.count++;

        bool isfile = !(fno.fattrib & AM_DIR);

        if (menuVerticalOffset == 0) {
          for (uint8_t i=0; i<LCD_LINES-1; i++) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameLower(isfile, fn, line)) {
              if (i < 6) memmove(reusableBuffer.sdmanager.lines[i+1], line, sizeof(reusableBuffer.sdmanager.lines[i]) * (6-i));
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        } else if (reusableBuffer.sdmanager.offset == menuVerticalOffset) {
          for (int8_t i=6; i>=0; i--) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameGreater(isfile, fn, line)) {
              if (i > 0) memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], sizeof(reusableBuffer.sdmanager.lines[0]) * i);
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        } else if (menuVerticalOffset > reusableBuffer.sdmanager.offset) {
          if (isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[5]) && isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[6])) {
            memset(reusableBuffer.sdmanager.lines[6], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[6], fn);
            reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        } else {
          if (isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[1]) && isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[0])) {
            memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[0], fn);
            reusableBuffer.sdmanager.lines[0][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        }
      }
    }
  }

  reusableBuffer.sdmanager.offset = menuVerticalOffset;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    lcdNextPos = 0;
    uint8_t attr = (menuVerticalPosition-1-menuVerticalOffset == i ? BSS|INVERS : BSS);
    if (reusableBuffer.sdmanager.lines[i][0]) {
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) {
        lcdDrawCharAtt(0, y, '[', attr);
      }
      lcdDrawTextAtt(lcdNextPos, y, reusableBuffer.sdmanager.lines[i], attr);
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) {
        lcdDrawCharAtt(lcdNextPos, y, ']', attr);
      }
    }
  }
}
# 1 "gui/view_main.cpp"
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


#include "../OpenAVRc.h"
#include "../timers.h"
#include "widgets.h"

#define BIGSIZE       DBLSIZE
#define LBOX_CENTERX  (LCD_W/4 + 10)
#define RBOX_CENTERX  (3*LCD_W/4 - 10)
#define MODELNAME_X   (2*FW-2)
#define MODELNAME_Y   (0)
#define PHASE_X       (6*FW-1)
#define PHASE_Y       (2*FH)
#define PHASE_FLAGS   0
#define VBATT_X       (6*FW)
#define VBATT_Y       (2*FH)
#define VBATTUNIT_X   (VBATT_X-1)
#define VBATTUNIT_Y   (3*FH)
#define REBOOT_X      (20*FW-3)
#define VSWITCH_X(i)  (16 + 3*i)
#define VSWITCH_Y     (LCD_H-8)
#define BAR_HEIGHT    (BOX_WIDTH-1l)
#define TRIM_LH_X     (LCD_W*1/4+2)
#define TRIM_LV_X     3
#define TRIM_RV_X     (LCD_W-4)
#define TRIM_RH_X     (LCD_W*3/4-2)

#define TRIM_LEN 27

void drawPotsBars()
{
  // Optimization by Mike Blandford
  uint8_t x, i, len ;  // declare temporary variables
  for (x=LCD_W/2-5, i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; x+=5, i++) {
    if (IS_POT_AVAILABLE(i)) {
      len = ((calibratedStick[i]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      V_BAR(x, LCD_H-8, len);
    }
  }
}

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1)];
  if (g_model.throttleReversed && CONVERT_MODE(1) == THR_STICK)
    calibStickVert = -calibStickVert;

#if defined(REV_EVO_V1)
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], -calibStickVert);
#else
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], calibStickVert);
#endif

  calibStickVert = calibratedStick[CONVERT_MODE(2)];
  if (g_model.throttleReversed && CONVERT_MODE(2) == THR_STICK)
    calibStickVert = -calibStickVert;

#if defined(REV_EVO_V1)
  drawStick(RBOX_CENTERX, -calibratedStick[CONVERT_MODE(3)], calibStickVert);
#else
  drawStick(RBOX_CENTERX, calibratedStick[CONVERT_MODE(3)], calibStickVert);
#endif

  drawPotsBars();
}

void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<4; i++) {
    static coord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0,1,1,0};
    coord_t xm, ym;
    uint8_t stickIndex = CONVERT_MODE(i);
    xm = x[stickIndex];
    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

#if defined(REV_EVO_V1)
    if (i==1 || i==3) {
      val = -val;
    }
#endif

    int16_t dir = val;
    bool exttrim = false;
    if (val < TRIM_MIN || val > TRIM_MAX) {
      exttrim = true;
    }

    if (val < -(TRIM_LEN+1)*4) {
      val = -(TRIM_LEN+1);
    } else if (val > (TRIM_LEN+1)*4) {
      val = TRIM_LEN+1;
    } else {
      val /= 4;
    }

    if (vert[i]) {
      ym = 31;
      lcdDrawSolidVerticalLine(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      ym -= val;
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcdDrawSolidHorizontalLine(xm-1, ym,  3);
      }
    } else {
      ym = 60;
      lcdDrawSolidHorizontalLine(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      xm += val;
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcdDrawSolidVerticalLine(xm, ym-1,  3);
      }
    }
    lcd_square(xm-3, ym-3, 7, att);
  }
}


void displayTimers()
{
#if defined(TRANSLATIONS_CZ)
#define MAINTMR_LBL_COL (9*FW-FW/2-1)
#else
#define MAINTMR_LBL_COL (9*FW-FW/2+3)
#endif
  // Main timer
  if (g_model.timers[0].mode) {
    TimerState & timerState = timersStates[0];
    LcdFlags att = DBLSIZE | (timerState.val<0 ? BLINK|INVERS : 0);
    putsTimer(12*FW+2+10*FWNUM-4, FH*2, timerState.val, att, att);
    uint8_t xLabel = (timerState.val >= 0 ? MAINTMR_LBL_COL : MAINTMR_LBL_COL-7);
    lcdPutsTimerMode(xLabel, FH*3, g_model.timers[0].mode);
  }
}

void displayBattVoltage()
{
#if defined(BATTGRAPH)
  putsVBat(VBATT_X-8, VBATT_Y+1, 0);
  lcdDrawFilledRect(VBATT_X-25, VBATT_Y+9, 21, 5);
  lcdDrawSolidVerticalLine(VBATT_X-4, VBATT_Y+10, 3);
  uint8_t count = GET_TXBATT_BARS();
  for (uint8_t i=0; i<count; i+=2)
    lcdDrawSolidVerticalLine(VBATT_X-24+i, VBATT_Y+10, 3);
  if (!IS_TXBATT_WARNING() || BLINK_ON_PHASE)
    lcdDrawFilledRect(VBATT_X-26, VBATT_Y, 24, 15);
#else
  LcdFlags att = (IS_TXBATT_WARNING() ? BLINK|INVERS : 0) | BIGSIZE;
  putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
  lcdDrawChar(VBATT_X, VBATTUNIT_Y, 'V');
#endif
}

#define displayVoltageOrAlarm() displayBattVoltage()

#define EVT_KEY_MODEL_MENU   EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_UP)
#define EVT_KEY_CONTEXT_MENU EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_SHUTDOWN     EVT_KEY_LONG(KEY_EXIT)

#if defined(NAVIGATION_MENUS)
void onMainViewMenu(const char *result)
{
  if (result == STR_RESET_TIMER1) {
    timerReset(0);
  }
  else if (result == STR_RESET_TIMER2) {
    timerReset(1);
  }
  else if (result == STR_SAVE_TIMERS) {
    //saveTimers();
    OpenAVRcClose();
  }
#if TIMERS > 2
  else if (result == STR_RESET_TIMER3) {
    timerReset(2);
  }
#endif
#if defined(FRSKY)
  else if (result == STR_RESET_TELEMETRY) {
    telemetryResetValue();
  }
#endif
  else if (result == STR_RESET_FLIGHT) {
    flightReset();
  }
  else if (result == STR_STATISTICS) {
    chainMenu(menuStatisticsView);
  }
}
#endif

void menuMainView(uint8_t event)
{
          if (warningResult) { // Power Off
            pwrCheck = false;
          }

  STICK_SCROLL_DISABLE();

  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;

  switch(event) {

  case EVT_ENTRY:
    killEvents(KEY_EXIT);
    killEvents(KEY_UP);
    killEvents(KEY_DOWN);
    break;

  /* TODO if timer2 is OFF, it's possible to use this timer2 as in er9x...
  case EVT_KEY_BREAK(KEY_MENU):
    if (view_base == VIEW_TIMER2) {
      Timer2_running = !Timer2_running;
      AUDIO_KEYPAD_UP();
    }
  break;
  */

  case EVT_KEY_BREAK(KEY_RIGHT):
  case EVT_KEY_BREAK(KEY_LEFT):
    if (view_base < VIEW_TIMER2) {
      g_eeGeneral.view ^= ALTERNATE_VIEW;
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
    }
    break;

#if defined(NAVIGATION_MENUS)
  case EVT_KEY_CONTEXT_MENU:
    killEvents(event);


    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER1);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER2);
#if defined(FRSKY)
    POPUP_MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
    POPUP_MENU_ADD_ITEM(STR_RESET_FLIGHT);
    POPUP_MENU_ADD_ITEM(STR_STATISTICS);
    POPUP_MENU_ADD_ITEM(STR_SAVE_TIMERS);
    popupMenuHandler = onMainViewMenu;
    break;
#endif

#if MENUS_LOCK != 2 /*no menus*/
  case EVT_KEY_LONG(KEY_MENU):// go to last menu
    pushMenu(lastPopMenu());
    killEvents(event);
    break;

    CASE_EVT_ROTARY_BREAK
  case EVT_KEY_MODEL_MENU:
    pushMenu(menuModelSelect);
    killEvents(event);
    break;

    CASE_EVT_ROTARY_LONG
  case EVT_KEY_GENERAL_MENU:
    pushMenu(menuGeneralSetup);
    killEvents(event);
    break;
#endif

  case EVT_KEY_BREAK(KEY_UP):
  case EVT_KEY_BREAK(KEY_DOWN):
    g_eeGeneral.view = (event == EVT_KEY_BREAK(KEY_UP) ? (view_base == VIEW_COUNT-1 ? 0 : view_base+1) : (view_base == 0 ? VIEW_COUNT-1 : view_base-1));
    eeDirty(EE_GENERAL);
    AUDIO_KEYPAD_UP();
    break;

  case EVT_KEY_STATISTICS:
    chainMenu(menuStatisticsView);
    killEvents(event);
    break;

  case EVT_KEY_TELEMETRY:
#if defined(FRSKY)
    if (!IS_FAI_ENABLED())
      chainMenu(menuTelemetryFrsky);
#elif defined(JETI)
    JETI_EnableRXD(); // enable JETI-Telemetry reception
    chainMenu(menuTelemetryJeti);
#elif defined(ARDUPILOT)
    ARDUPILOT_EnableRXD(); // enable ArduPilot-Telemetry reception
    chainMenu(menuTelemetryArduPilot);
#elif defined(NMEA)
    NMEA_EnableRXD(); // enable NMEA-Telemetry reception
    chainMenu(menuTelemetryNMEA);
#elif defined(MAVLINK)
    chainMenu(menuTelemetryMavlink);
#else
    chainMenu(menuStatisticsDebug);
#endif
    killEvents(event);
    break;

  case EVT_KEY_FIRST(KEY_EXIT):

    if (view == VIEW_TIMER2) {
      timerReset(1);
    }
    AUDIO_KEYPAD_UP();
    break;

  case EVT_KEY_SHUTDOWN: // Shutdown confirmation
          POPUP_CONFIRMATION(STR_POWEROFF);
          killEvents(event);
    break;

#if !defined(NAVIGATION_MENUS)
  /*case EVT_KEY_LONG(KEY_EXIT):
    flightReset();
    AUDIO_KEYPAD_UP();
    break;*/
#endif

  }

  {
    // Flight Mode Name
    uint8_t mode = mixerCurrentFlightMode;
    lcdDrawSizedTextAtt(PHASE_X, PHASE_Y, g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[mode].name), ZCHAR|PHASE_FLAGS);

    // Model Name
#if defined(DSM2) || defined(MULTIMODULE) || defined(SPIMODULES)
    if ((RFModule.mode == BIND_MODE) && !BLINK_ON_PHASE)
    {
      lcdDrawTextAtt(MODELNAME_X, MODELNAME_Y, STR_BIND, BLINK|BIGSIZE);
    } else {
      putsModelName(MODELNAME_X, MODELNAME_Y, g_model.name, g_eeGeneral.currModel, BIGSIZE);
    }
#else
    putsModelName(MODELNAME_X, MODELNAME_Y, g_model.name, g_eeGeneral.currModel, BIGSIZE);
#endif

    // Main Voltage (or alarm if any)
    displayVoltageOrAlarm();

    // Timers
    displayTimers();

    // Trims sliders
    displayTrims(mode);

    DISPLAY_PROGRESS_BAR(20*FW+1);
  }

  if (view_base < VIEW_INPUTS) {
    // scroll bar
    lcdDrawSolidHorizontalLineStip(38, 34, 54, DOTTED);
    lcdDrawSolidHorizontalLine((g_eeGeneral.view & ALTERNATE_VIEW) ? 64 : 38, 34, 26, SOLID);

    for (uint8_t i=0; i<8; i++) {
      uint8_t x0,y0;
      uint8_t chan = (g_eeGeneral.view & ALTERNATE_VIEW) ? 8+i : i;

      int16_t val = channelOutputs[chan];

      switch(view_base) {
      case VIEW_OUTPUTS_VALUES:
        x0 = (i%4*9+3)*FW/2;
        y0 = i/4*FH+40;
#if defined(PPM_UNIT_US)
        lcdDrawNumberAttUnit(x0+4*FW, y0, PPM_CH_CENTER(chan)+val/2, 0);
#elif defined(PPM_UNIT_PERCENT_PREC1)
        lcdDrawNumberAttUnit(x0+4*FW, y0, calcRESXto1000(val), PREC1);
#else
        lcdDrawNumberAttUnit(x0+4*FW, y0, calcRESXto1000(val)/10, 0);  // G: Don't like the decimal part*
#endif
        break;

      case VIEW_OUTPUTS_BARS:
#define WBAR2 (50/2)
        x0       = i<4 ? LCD_W/4+2 : LCD_W*3/4-2;
        y0       = 38+(i%4)*5;

        uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
        int8_t len = (abs(val) * WBAR2 + lim/2) / lim;

        if(len>WBAR2)  len = WBAR2;  // prevent bars from going over the end - comment for debugging
        lcdDrawSolidHorizontalLineStip(x0-WBAR2, y0, WBAR2*2+1, DOTTED);
        lcdDrawSolidVerticalLine(x0,y0-2,5);
        if (val>0)
          x0+=1;
        else
          x0-=len;
        lcdDrawSolidHorizontalLine(x0,y0+1,len);
        lcdDrawSolidHorizontalLine(x0,y0-1,len);
        break;
      }
    }
  }
#if defined(GVARS) // Show current Gvars values
  else if (view_base == VIEW_GVARS) {
    uint8_t x0,y0;
    bool test;
#if defined(EXTERNALEEPROM)
    uint8_t offset = (g_eeGeneral.view & ALTERNATE_VIEW) ? MAX_GVARS/2 : 0;
#else
    uint8_t offset = 0;
#endif
    for (uint8_t i=0; i<6; i++) {
      x0 = (FW+FW/3)+(i%2)*2*FW*(LEN_GVAR_NAME-1)-(i%2)*FW/2;
      y0 = i/2*FH+33;
      test = false;
      for (uint8_t j=0; j<LEN_GVAR_NAME; j++) {
        if (g_model.gvars[i+offset].name[j]) {
          test = true;
          break;
        }
      }
      if (!test) lcdDrawStringWithIndex(x0+3*FW, y0, STR_GV, i+1+offset);
      else lcdDrawSizedTextAtt(x0, y0, g_model.gvars[i+offset].name, LEN_GVAR_NAME, ZCHAR|FIXEDWIDTH);
      x0 += (LEN_GVAR_NAME+3)*FW+FW/3;
      lcd_outdez8(x0, y0, GVAR_VALUE(i+offset, getGVarFlightPhase(mixerCurrentFlightMode, i+offset)));
    }
  }
#endif
  else if (view_base == VIEW_INPUTS) {
    if (view == VIEW_INPUTS) {
      // Sticks + Pots
      doMainScreenGraphics();

      // Switches
      for (uint8_t i=SWSRC_THR; i<=SWSRC_TRN; i++) {
        int8_t sw = (i == SWSRC_TRN ? (switchState(SW_ID0) ? SWSRC_ID0 : (switchState(SW_ID1) ? SWSRC_ID1 : SWSRC_ID2)) : i);
        uint8_t x = 2*FW-2, y = i*FH+1;
        if (i>=SWSRC_AIL) {
          x = 17*FW-1;
          y -= 3*FH;
        }
        lcdPutsSwitches(x, y, sw, getSwitch(i) ? INVERS : 0);
      }
    } else {
#if defined(ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_ROTARY_ENCODERS; i++) {
        int16_t val = getRotaryEncoder(i);
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
#if ROTARY_ENCODERS > 2
#define V_BAR_W 5
        V_BAR(LCD_W/2-8+V_BAR_W*i, LCD_H-8, len);
#else
#define V_BAR_W 5
        V_BAR(LCD_W/2-3+V_BAR_W*i, LCD_H-8, len);
#endif
      }
#endif // ROTARY_ENCODERS

      // Logical Switches
      for (uint8_t i=0; i<15; i++) //Can't draw all NUM_LOGICAL_SWITCH
        lcdPutsSwitches(2*FW-3 + (i/3)*(i/3>2 ? 3*FW+2 : (3*FW-1)) + (i/3>2 ? 2*FW : 0), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
    }
  } else { // timer2
#if defined(TRANSLATIONS_CZ)
#define TMR2_LBL_COL (20-FW/2+1)
#else
#define TMR2_LBL_COL (20-FW/2+5)
#endif
    putsTimer(33+FW+2+10*FWNUM-4, FH*5, timersStates[1].val, DBLSIZE, DBLSIZE);
    lcdPutsTimerMode(timersStates[1].val >= 0 ? TMR2_LBL_COL : TMR2_LBL_COL-7, FH*6, g_model.timers[1].mode);
    // lcdDrawNumberNAtt(33+11*FW, FH*6, s_timerVal_10ms[1], LEADING0, 2); // 1/100s
  }

  // And ! in case of unexpected shutdown
  if (unexpectedShutdown) {
    lcdDrawCharAtt(REBOOT_X, 0*FH, '!', INVERS);
  }

}
# 1 "gui/view_statistics.cpp"
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


#include "menu_general.h"

void menuStatisticsView(uint8_t event)
{
  TITLE(STR_MENUSTAT);

  switch(event) {
  case EVT_KEY_FIRST(KEY_UP):
    chainMenu(menuStatisticsDebug);
    break;

  case EVT_KEY_FIRST(KEY_EXIT):
    chainMenu(menuMainView);
    break;
  }

  lcdDrawText(  1*FW, FH*0, STR_TOTTM1TM2THRTHP);
  putsTimer(    5*FW+5*FWNUM+1, FH*1, timersStates[0].val, 0, 0);
  putsTimer(   12*FW+5*FWNUM+1, FH*1, timersStates[1].val, 0, 0);

  putsTimer(    5*FW+5*FWNUM+1, FH*2, s_timeCumThr, 0, 0);
  putsTimer(   12*FW+5*FWNUM+1, FH*2, s_timeCum16ThrP/16, 0, 0);

  putsTimer(   12*FW+5*FWNUM+1, FH*0, sessionTimer, 0, 0);

#if defined(THRTRACE)
  coord_t traceRd = (s_traceCnt < 0 ? s_traceWr : 0);
  const coord_t x = 5;
  const coord_t y = 60;
  lcdDrawSolidHorizontalLine(x-3, y, MAXTRACE+3+3);
  lcdDrawSolidVerticalLine(x, y-32, 32+3);

  for (coord_t i=0; i<MAXTRACE; i+=6) {
    lcdDrawSolidVerticalLine(x+i+6, y-1, 3);
  }
  for (coord_t i=1; i<=MAXTRACE; i++) {
    lcdDrawSolidVerticalLine(x+i, y-s_traceBuf[traceRd], s_traceBuf[traceRd]);
    traceRd++;
    if (traceRd>=MAXTRACE) traceRd = 0;
    if (traceRd==s_traceWr) break;
  }
#endif
}

#define MENU_DEBUG_COL1_OFS   (14*FW+28)

void menuStatisticsDebug(uint8_t event)
{
  TITLE(STR_MENUDEBUG);

  switch(event) {
  case EVT_KEY_FIRST(KEY_ENTER):
    g_tmr1Latency_min = -1;
    g_tmr1Latency_max = 0;
    maxMixerDuration  = 0;
    AUDIO_KEYPAD_UP();
    break;

  case EVT_KEY_FIRST(KEY_DOWN):
    chainMenu(menuStatisticsView);
    break;
  case EVT_KEY_FIRST(KEY_EXIT):
    chainMenu(menuMainView);
    break;
  }


  lcdDrawTextLeft(1*FH, STR_TMR1LATMAXUS);
  lcdDrawNumberAttUnit(MENU_DEBUG_COL1_OFS, 1*FH, (g_tmr1Latency_max/2), UNSIGN);

  lcdDrawTextLeft(2*FH, STR_TMR1LATMINUS);
  lcdDrawNumberAttUnit(MENU_DEBUG_COL1_OFS, 2*FH, (g_tmr1Latency_min/2), UNSIGN);

  lcdDrawTextLeft(3*FH, STR_TMR1JITTERUS);
  lcdDrawNumberAttUnit(MENU_DEBUG_COL1_OFS, 3*FH, ((g_tmr1Latency_max - g_tmr1Latency_min) /2), UNSIGN);

  lcdDrawTextLeft(4*FH, STR_TMIXMAXMS);
  lcdDrawNumberAttUnit(MENU_DEBUG_COL1_OFS, 4*FH, DURATION_MS_PREC2(maxMixerDuration), PREC2);
  lcdDrawTextLeft(5*FH, STR_FREESTACKMINB);
  lcdDrawNumberAttUnit(14*FW, 5*FH, stackAvailable(), UNSIGN);
  lcdDrawTextLeft(6*FH, STR_FREERAMINB);
  lcdDrawNumberAttUnit(14*FW, 6*FH, freeRam(), UNSIGN);
  lcdDrawText(4*FW, 7*FH+1, STR_MENUTORESET);
  lcd_status_line();
}
# 1 "gui/lcd.cpp"
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


#include "../OpenAVRc.h"
#include "../timers.h"

display_t displayBuf[DISPLAY_BUF_SIZE];

void lcdClear()
{
  memset(displayBuf, 0, DISPLAY_BUFER_SIZE);
}

coord_t lcdLastPos;
coord_t lcdNextPos;


void lcdDrawChar(coord_t x, coord_t y, const unsigned char c)
{
  lcdDrawCharAtt(x, y, c, 0);
}

void lcdDrawSizedTextAtt(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags flags)
{
  const coord_t orig_x = x;
  bool setx = false;
  while (len--) {
    unsigned char c;
    switch (flags & (BSS+ZCHAR)) {
    case BSS:
      c = *s;
      break;
    case ZCHAR:
      c = idx2char(*s);
      break;
    default:
      c = pgm_read_byte_near(s);
      break;
    }

    if (setx) {
      x = c;
      setx = false;
    } else if (!c) {
      break;
    } else if (c >= 0x20) {
      lcdDrawCharAtt(x, y, c, flags);
      x = lcdNextPos;
    } else if (c == 0x1F) { //X-coord prefix
      setx = true;
    } else if (c == 0x1E) { //NEWLINE
      x = orig_x;
      y += FH;
      if (y >= LCD_H) break;
    } else {
      x += (c*FW/2); // EXTENDED SPACE
    }
    s++;
  }
  lcdLastPos = x;
  lcdNextPos = x;
}

void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len)
{
  lcdDrawSizedTextAtt(x, y, s, len, 0);
}

void lcdDrawTextAtt(coord_t x, coord_t y, const pm_char * s, LcdFlags flags)
{
  lcdDrawSizedTextAtt(x, y, s, 255, flags);
}

void lcdDrawText(coord_t x, coord_t y, const pm_char * s)
{
  lcdDrawTextAtt(x, y, s, 0);
}

void lcdDrawTextLeft(coord_t y, const pm_char * s)
{
  lcdDrawText(0, y, s);
}

void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s,uint8_t idx, LcdFlags flags)
{
  uint8_t length;
  length = pgm_read_byte_near(s++);
  lcdDrawSizedTextAtt(x, y, s+length*idx, length, flags & ~(BSS|ZCHAR));
}

void lcd_outhex(uint8_t digit, coord_t x, coord_t y, uint16_t val, LcdFlags mode)
{
  x += FWNUM*4+1;
  for(int i=0; i<digit; i++) {
    x -= FWNUM;
    char c = val & 0xf;
    c = c>9 ? c+'A'-10 : c+'0';
    lcdDrawCharAtt(x, y, c, (c>='A' ? CONDENSED : 0 ) | mode);
    val >>= 4;
  }
}

void lcd_outdez8(coord_t x, coord_t y, int8_t val)
{
  lcdDrawNumberAttUnit(x, y, val);
}

void lcdDrawNumberAttUnit(coord_t x, coord_t y, lcdint_t val, LcdFlags flags)
{
  lcdDrawNumberNAtt(x, y, val, flags);
}

void lcdDrawNumberNAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags flags, uint8_t len)
{
  uint8_t fw = FWNUM;
  int8_t mode = MODE(flags);
  flags &= ~LEADING0;
  bool dblsize = flags & DBLSIZE;
#define xxlsize 0
#define midsize 0
#define smlsize 0
#define tinsize 0

  bool neg = false;
  if (flags & UNSIGN) {
    flags -= UNSIGN;
  } else if (val < 0) {
    neg = true;
    val = -val;
  }

  coord_t xn = 0;
  uint8_t ln = 2;

  if (mode != MODE(LEADING0)) {
    len = 1;
    lcduint_t tmp = ((lcduint_t)val) / 10;
    while (tmp) {
      len++;
      tmp /= 10;
    }
    if (len <= mode) {
      len = mode + 1;
    }
  }

  if (dblsize) {
    fw += FWNUM;
  } else if (xxlsize) {
    fw += 4*FWNUM-1;
  } else if (midsize) {
    fw += FWNUM-3;
  } else if (tinsize) {
    fw -= 1;
  } else {
    if (flags & LEFT) {
      if (mode > 0)
        x += 2;
    }
#if defined(BOLD_SPECIFIC_FONT)
    if (flags & BOLD) fw += 1;
#endif
  }

  if (flags & LEFT) {
    x += len * fw;
    if (neg) {
      x += ((xxlsize|dblsize|midsize) ? 7 : FWNUM);
    }
  }

  lcdLastPos = x;
  x -= fw;
  if (dblsize) x++;

  for (uint8_t i=1; i<=len; i++) {
    div_t qr = div((lcduint_t)val, 10);
    char c = qr.rem + '0';
    LcdFlags f = flags;
    if (dblsize) {
      if (c=='1' && i==len && xn>x+10) {
        x+=1;
      }
      if ((lcduint_t)val >= 1000) {
        x+=FWNUM;
        f &= ~DBLSIZE;
      }
    }
    lcdDrawCharAtt(x, y, c, f);
    if (mode == i) {
      flags &= ~PREC2; // TODO not needed but removes 20bytes, could be improved for sure, check asm
      if (dblsize) {
        xn = x - 2;
        if (c>='2' && c<='3') ln++;
        uint8_t tn = (qr.quot % 10);
        if (tn==2 || tn==4) {
          if (c=='4') {
            xn++;
          } else {
            xn--;
            ln++;
          }
        }
      } else if (xxlsize) {
        x -= 17;
        lcdDrawCharAtt(x+2, y, '.', f);
      } else if (midsize) {
        x -= 3;
        xn = x;
      } else if (smlsize) {
        x -= 2;
        lcdDrawPoint(x, y+5);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
          lcdDrawSolidVerticalLine(x, y-1, 8);
        }
      } else if (tinsize) {
        x--;
        lcdDrawPoint(x-1, y+4);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
          lcdDrawSolidVerticalLine(x-1, y-1, 7);
        }
        x--;
      } else {
        x -= 2;
        lcdDrawCharAtt(x, y, '.', f);
      }
    }
    if (dblsize && (lcduint_t)val >= 1000 && (lcduint_t)val < 10000) x-=2;
    val = qr.quot;
    x -= fw;
#if defined(BOLD_SPECIFIC_FONT)
    if (i==len && (flags & BOLD)) x += 1;
#endif
  }

  if (xn) {
    if (midsize) {
      if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
        lcdDrawSolidVerticalLine(xn, y, 12);
        lcdDrawSolidVerticalLine(xn+1, y, 12);
      }
      lcdDrawSolidHorizontalLine(xn, y+9, 2);
      lcdDrawSolidHorizontalLine(xn, y+10, 2);
    } else {
      // TODO needed on CPUAVR? y &= ~0x07;
      lcdDrawFilledRect(xn, y+2*FH-3, ln, 2);
    }
  }
  if (neg) lcdDrawCharAtt(x, y, '-', flags);
}

void lcdDrawSolidHorizontalLine(coord_t x, coord_t y, coord_t w, LcdFlags att)
{
  lcdDrawSolidHorizontalLineStip(x, y, w, 0xff, att);
}


void lcdDrawSolidVerticalLine(coord_t x, scoord_t y, scoord_t h)
{
  lcdDrawSolidVerticalLineStip(x, y, h, SOLID);
}

void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  lcdDrawSolidVerticalLineStip(x, y, h, pat);
  lcdDrawSolidVerticalLineStip(x+w-1, y, h, pat);
  if (~att & ROUND) {
    x+=1;
    w-=2;
  }
  lcdDrawSolidHorizontalLineStip(x, y+h-1, w, pat);
  lcdDrawSolidHorizontalLineStip(x, y, w, pat);
}

void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  for (scoord_t i=y; i<y+h; i++) {
    if ((att&ROUND) && (i==y || i==y+h-1))
      lcdDrawSolidHorizontalLineStip(x+1, i, w-2, pat, att);
    else
      lcdDrawSolidHorizontalLineStip(x, i, w, pat, att);
    pat = (pat >> 1) + ((pat & 1) << 7);
  }
}

void lcdDrawTelemetryTopBar()
{
  putsModelName(0, 0, g_model.name, g_eeGeneral.currModel, 0);
  uint8_t att = (IS_TXBATT_WARNING() ? BLINK : 0);
  putsVBat(14*FW,0,att);
  if (g_model.timers[0].mode) {
    att = (timersStates[0].val<0 ? BLINK : 0);
    putsTimer(17*FW+5*FWNUM+1, 0, timersStates[0].val, att, att);
  }
  lcdInvertLine(0);
}


void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att, LcdFlags att2)
{
  div_t qr;

  if (!(att & LEFT)) {
    if (att & DBLSIZE)
      x -= 5*(2*FWNUM)-4;
    else
      x -= 5*FWNUM+1;
  }

  if (tme < 0) {
    lcdDrawCharAtt(x - ((att & DBLSIZE) ? FW+2 : ((att & DBLSIZE) ? FW+0 : FWNUM)), y, '-', att);
    tme = -tme;
  }

  qr = div(tme, 60);

#define separator ':'
  lcdDrawNumberNAtt(x, y, qr.quot, att|LEADING0|LEFT, 2);
  lcdDrawCharAtt(lcdLastPos, y, separator, att&att2);
  lcdDrawNumberNAtt(lcdNextPos, y, qr.rem, att2|LEADING0|LEFT, 2);
}

void lcdPutsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att)
{
  lcdDrawNumberAttUnit(x, y, (int16_t)volts, (~NO_UNIT) & (att | ((att&PREC2)==PREC2 ? PREC2 : PREC1)));
  if (~att & NO_UNIT) lcdDrawCharAtt(lcdLastPos, y, 'V', att);
}

void putsVBat(coord_t x, coord_t y, LcdFlags att)
{
#if defined(BATTGRAPH)
    lcdPutsVolts(x, y, (g_vbat10mV < 1000)? g_vbat10mV : g_vbat10mV/10, (g_vbat10mV < 1000)? att | PREC2 : att | PREC1);
#else
    lcdPutsVolts(x, y, g_vbat10mV/10, att | PREC1);
#endif
}

void lcdDrawStringWithIndex(coord_t x, coord_t y, const pm_char *str, uint8_t idx, LcdFlags att)
{
  lcdDrawTextAtt(x, y, str, att & ~LEADING0);
  lcdDrawNumberNAtt(lcdNextPos, y, idx, att|LEFT, 2);
}

void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  if (idx < MIXSRC_THR)
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx, att);
  else if (idx < MIXSRC_FIRST_LOGICAL_SWITCH)
    lcdPutsSwitches(x, y, idx-MIXSRC_THR+1+3*(1), att);
  else if (idx <= MIXSRC_LAST_LOGICAL_SWITCH)
    lcdPutsSwitches(x, y, SWSRC_SW1+idx-MIXSRC_FIRST_LOGICAL_SWITCH, att);
  else if (idx < MIXSRC_CH1)
    lcdDrawStringWithIndex(x, y, STR_PPM_TRAINER, idx-MIXSRC_FIRST_TRAINER+1, att);
  else if (idx <= MIXSRC_LAST_CH) {
    lcdDrawStringWithIndex(x, y, STR_CH, idx-MIXSRC_CH1+1, att);
  }
  else if (idx <= MIXSRC_LAST_GVAR)
    lcdDrawStringWithIndex(x, y, STR_GV, idx-MIXSRC_GVAR1+1, att);
  else if (idx < MIXSRC_FIRST_TELEM) {
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1-(MIXSRC_SW1-MIXSRC_THR)-NUM_LOGICAL_SWITCH-NUM_TRAINER-NUM_CHNOUT-MAX_GVARS, att);
  } else
    lcdDrawTextAtIndex(x, y, STR_VTELEMCHNS, idx-MIXSRC_FIRST_TELEM+1, att);
}

void lcdPutsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  lcdDrawTextAtIndex(x, y, STR_RETA123, idx-1, att);
}

void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att)
{
  uint8_t len = sizeof(g_model.name);
  while (len>0 && !name[len-1]) --len;
  if (len==0) {
    lcdDrawStringWithIndex(x, y, STR_MODEL, id+1, att|LEADING0);
  } else {
    lcdDrawSizedTextAtt(x, y, name, sizeof(g_model.name), ZCHAR|att);
  }
}

void lcdPutsSwitches(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx == SWSRC_OFF)
    return lcdDrawTextAtIndex(x, y, STR_OFFON, 0, att);
  if (idx < 0) {
    lcdDrawCharAtt(x-2, y, '!', att);
    idx = -idx;
  }
  return lcdDrawTextAtIndex(x, y, STR_VSWITCHES, idx, att);
}

#if defined(FLIGHT_MODES)
void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) {
    lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, att);
    return;
  }
  if (idx < 0) {
    lcdDrawCharAtt(x-2, y, '!', att);
    idx = -idx;
  }
  if (att & CONDENSED)
    lcdDrawNumberNAtt(x+FW*1, y, idx-1, (att & ~CONDENSED), 1);
  else
    lcdDrawStringWithIndex(x, y, STR_FP, idx-1, att);
}
#endif

void lcdDrawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx < 0) {
    lcdDrawCharAtt(x-3, y, '!', att);
    idx = -idx+CURVE_BASE-1;
  }
  if (idx < CURVE_BASE)
    lcdDrawTextAtIndex(x, y, STR_VCURVEFUNC, idx, att);
  else
    lcdDrawStringWithIndex(x, y, STR_CV, idx-CURVE_BASE+1, att);
}

void lcdPutsTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att)
{
  if (mode >= 0) {
    if (mode < TMRMODE_COUNT)
      return lcdDrawTextAtIndex(x, y, STR_VTMRMODES, mode, att);
    else
      mode -= (TMRMODE_COUNT-1);
  }
  lcdPutsSwitches(x, y, mode, att);
}

void lcdPutsTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  trim_t v = getRawTrimValue(phase, idx);

  if (v > TRIM_EXTENDED_MAX) {
    uint8_t p = v - TRIM_EXTENDED_MAX - 1;
    if (p >= phase) p++;
    lcdDrawCharAtt(x, y, '0'+p, att);
  } else {
    lcdPutsChnLetter(x, y, idx+1, att);
  }
}

#if ROTARY_ENCODERS > 0
void lcdPutsRotaryEncoderMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  int16_t v = flightModeAddress(phase)->rotaryEncoders[idx];

  if (v > ROTARY_ENCODER_MAX) {
    uint8_t p = v - ROTARY_ENCODER_MAX - 1;
    if (p >= phase) p++;
    lcdDrawCharAtt(x, y, '0'+p, att);
  } else {
    lcdDrawCharAtt(x, y, 'a'+idx, att);
  }
}
#endif

#if   defined(FRSKY)
void lcdPutsValueWithUnit(coord_t x, coord_t y, lcdint_t val, uint8_t unit, LcdFlags att)
{
  convertUnit(val, unit);
  lcdDrawNumberAttUnit(x, y, val, att & (~NO_UNIT));
  if (!(att & NO_UNIT) && unit != UNIT_RAW) {
    lcdDrawTextAtIndex(lcdLastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
  }
}

const pm_uint8_t bchunit_ar[] PROGMEM = {
  UNIT_DIST,    // Alt
  UNIT_RAW,     // Rpm
  UNIT_PERCENT, // Fuel
  UNIT_TEMPERATURE, // T1
  UNIT_TEMPERATURE, // T2
  UNIT_KTS,     // Speed
  UNIT_DIST,    // Dist
  UNIT_DIST,    // GPS Alt
};

void lcdPutsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, lcdint_t val, LcdFlags att)
{
  switch (channel) {
  case TELEM_TIMER1-1:
  case TELEM_TIMER2-1:
    att &= ~NO_UNIT;
    putsTimer(x, y, val, att, att);
    break;
#if defined(FRSKY)
  case TELEM_MIN_A1-1:
  case TELEM_MIN_A2-1:
    channel -= TELEM_MIN_A1-TELEM_A1;
  // no break
  case TELEM_A1-1:
  case TELEM_A2-1:
    channel -= TELEM_A1-1;
    // A1 and A2
    {
      lcdint_t converted_value = applyChannelRatio(channel, val);
      if (ANA_CHANNEL_UNIT(channel) >= UNIT_RAW) {
        converted_value = div10_and_round(converted_value);
      } else {
        if (abs(converted_value) < 1000) {
          att |= PREC2;
        } else {
          converted_value = div10_and_round(converted_value);
          att |= PREC1;
        }
      }
      lcdPutsValueWithUnit(x, y, converted_value, g_model.telemetry.channels[channel].type, att);
      break;
    }
#endif

  case TELEM_CELL-1:
  case TELEM_MIN_CELL-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_VOLTS, att|PREC2);
    break;

  case TELEM_TX_VOLTAGE-1:
  case TELEM_VFAS-1:
  case TELEM_CELLS_SUM-1:
  case TELEM_MIN_CELLS_SUM-1:
  case TELEM_MIN_VFAS-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_VOLTS, att|PREC1);
    break;

  case TELEM_CURRENT-1:
  case TELEM_MAX_CURRENT-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_AMPS, att|PREC1);
    break;

  case TELEM_CONSUMPTION-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_MAH, att);
    break;

  case TELEM_POWER-1:
  case TELEM_MAX_POWER-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_WATTS, att);
    break;

  case TELEM_ACCx-1:
  case TELEM_ACCy-1:
  case TELEM_ACCz-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_RAW, att|PREC2);
    break;

  case TELEM_VSPEED-1:
    lcdPutsValueWithUnit(x, y, div10_and_round(val), UNIT_RAW, att|PREC1);
    break;

  case TELEM_ASPEED-1:
  case TELEM_MAX_ASPEED-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_KTS, att|PREC1);
    break;

  case TELEM_RSSI_TX-1:
  case TELEM_RSSI_RX-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_RAW, att);
    break;

  case TELEM_HDG-1:
    lcdPutsValueWithUnit(x, y, val, UNIT_HDG, att);
    break;

  case TELEM_ALT-1:
    lcdPutsValueWithUnit(x, y, div10_and_round(val), UNIT_DIST, att|PREC1);
    break;

  default: {
    uint8_t unit = 1;
    if (channel >= TELEM_MAX_T1-1 && channel <= TELEM_MAX_DIST-1)
      channel -= TELEM_MAX_T1 - TELEM_T1;
    if (channel <= TELEM_GPSALT-1)
      unit = channel + 1 - TELEM_ALT;
    if (channel >= TELEM_MIN_ALT-1 && channel <= TELEM_MAX_ALT-1)
      unit = 0;
    lcdPutsValueWithUnit(x, y, val, pgm_read_byte_near(bchunit_ar+unit), att);
    break;
  }
  }
}
#else // defined(FRSKY)
void lcdPutsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, lcdint_t val, uint8_t att)
{
  switch (channel) {
  case TELEM_TIMER1-1:
  case TELEM_TIMER2-1:
    att &= ~NO_UNIT;
    putsTimer(x, y, val, att, att);
    break;

  case TELEM_TX_VOLTAGE-1:
    lcdDrawNumberAttUnit(x, y, val, (att|PREC1) & (~NO_UNIT));
    if (!(att & NO_UNIT))
      lcdDrawChar(lcdLastPos/*+1*/, y, 'V');
    break;
  }
}
#endif

void lcdSetContrast()
{
  lcdSetRefVolt(g_eeGeneral.contrast);
}

#define LCD_BYTE_FILTER(p, keep, add) *(p) = (*(p) & (keep)) | (add)

void lcdDrawCharAtt(coord_t x, uint8_t y, const unsigned char c, LcdFlags flags)
{
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];

  uint_farptr_t q=pgm_get_far_address(zzfont_5x7);
  q += (c-0x20)*5;
  lcdNextPos = x-1;
  p--;

  bool inv = false;
  if (flags & BLINK) {
    if (BLINK_ON_PHASE) {
      if (flags & INVERS)
        inv = true;
      else {
        return;
      }
    }
  } else if (flags & INVERS) {
    inv = true;
  }

  unsigned char c_remapped = 0;

#if defined(BOLD_SPECIFIC_FONT)
  if (flags & (DBLSIZE+BOLD)) {
#else
  if (flags & DBLSIZE) {
#endif
    // To save space only some DBLSIZE and BOLD chars are available
    // c has to be remapped. All non existing chars mapped to 0 (space)

    if (c>=',' && c<=':')
      c_remapped = c - ',' + 1;
    else if (c>='A' && c<='Z')
      c_remapped = c - 'A' + 16;
    else if (c>='a' && c<='z')
      c_remapped = c - 'a' + 42;
    else if (c=='_')
      c_remapped = 4;
#if defined(BOLD_SPECIFIC_FONT)
    else if (c!=' ')
      flags &= ~BOLD;
#endif

#if defined(BOLD_SPECIFIC_FONT)
  }
  if (flags & DBLSIZE) {
#endif

    /* each letter consists of ten top bytes followed by
     * by ten bottom bytes (20 bytes per * char) */

    q = pgm_get_far_address(zzfont_10x14);
    q += (uint16_t)c_remapped*20;

    for (int8_t i=0; i<=11; i++) {
      uint8_t b1=0, b2=0;
      if (!i) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      } else if (i <= 10) {

        b1 = pgm_read_byte_far(q++); /*top byte*/
        b2 = pgm_read_byte_far(q++);

      }
      if ((b1 & b2) == 0xff) continue;
      if (inv) {
        b1 = ~b1;
        b2 = ~b2;
      }
      if(p+LCD_W < DISPLAY_END) {
        ASSERT_IN_DISPLAY(p);
        ASSERT_IN_DISPLAY(p+LCD_W);
        LCD_BYTE_FILTER(p, 0, b1);
        LCD_BYTE_FILTER(p+LCD_W, 0, b2);
        p++;
        lcdNextPos++;
      }
    }
  } else {
    const uint8_t ym8 = (y & 0x07);
#if defined(BOLD_FONT)
#if defined(BOLD_SPECIFIC_FONT)
    if (flags & BOLD) {
      q = pgm_get_far_address(zzfont_5x7_B);
      q += (c_remapped)*5;
    }
#else
    uint8_t bb = 0;
    if (inv) bb = 0xff;
#endif
#endif

    uint8_t *lineEnd = &displayBuf[ y / 8 * LCD_W + LCD_W ];

    for (int8_t i=0; i<=6; i++) {
      uint8_t b = 0;
      if (i==0) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      } else if (i <= 5) {
        b = pgm_read_byte_far(q++);
      }
      if (b == 0xff) {
        if (flags & FIXEDWIDTH)
          b = 0;
        else
          continue;
      }
      if (inv) b = ~b;
      if ((flags & CONDENSED) && i==2) {
        /*condense the letter by skipping column 3 */
        continue;
      }

#if defined(BOLD_FONT) && !defined(BOLD_SPECIFIC_FONT)
      if (flags & BOLD) {
        uint8_t a;
        if (inv)
          a = b & bb;
        else
          a = b | bb;
        bb = b;
        b = a;
      }
#endif

      if (p<DISPLAY_END && p<lineEnd) {
        ASSERT_IN_DISPLAY(p);
        uint8_t mask = ~(0xff << ym8);
        LCD_BYTE_FILTER(p, mask, b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~mask, b >> (8-ym8));
        }

        if (inv && (ym8 == 1)) *p |= 0x01;
      }
      p++;
      lcdNextPos++;
    }
  }
}

void lcdMaskPoint(uint8_t *p, uint8_t mask, LcdFlags att)
{
  ASSERT_IN_DISPLAY(p);

  if (att & FORCE)
    *p |= mask;
  else if (att & ERASE)
    *p &= ~mask;
  else
    *p ^= mask;
}

void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att)
{
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];
  if (p<DISPLAY_END)
    lcdMaskPoint(p, _BV(y%8), att);
}

void lcdDrawSolidHorizontalLineStip(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att)
{
  if (y >= LCD_H) return;
  if (x+w > LCD_W) {
    w = LCD_W - x;
  }

  uint8_t *p  = &displayBuf[ y / 8 * LCD_W + x ];
  uint8_t msk = _BV(y%8);
  while (w--) {
    if(pat&1) {
      lcdMaskPoint(p, msk, att);
      pat = (pat >> 1) | 0x80;
    } else {
      pat = pat >> 1;
    }
    p++;
  }
}

// allows the att parameter...
void lcdDrawSolidVerticalLineStip(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att)
{
  if (x >= LCD_W) return;

  if (h<0) {
    y+=h;
    h=-h;
  }
  if (y<0) {
    h+=y;
    y=0;
  }
  if (y+h > LCD_H) {
    h = LCD_H - y;
  }

  if (pat==DOTTED && !(y%2))
    pat = ~pat;

  uint8_t *p  = &displayBuf[ y / 8 * LCD_W + x ];
  y = (y & 0x07);
  if (y) {
    ASSERT_IN_DISPLAY(p);
    uint8_t msk = ~(_BV(y)-1);
    h -= 8-y;
    if (h < 0)
      msk -= ~(_BV(8+h)-1);
    lcdMaskPoint(p, msk & pat, att);
    p += LCD_W;
  }
  while (h>=8) {
    ASSERT_IN_DISPLAY(p);
    lcdMaskPoint(p, pat, att);
    p += LCD_W;
    h -= 8;
  }
  if (h>0) {
    ASSERT_IN_DISPLAY(p);
    lcdMaskPoint(p, (_BV(h)-1) & pat, att);
  }
}

void lcdInvertLine(int8_t y)
{
  uint8_t *p  = &displayBuf[y * LCD_W];
  for (coord_t x=0; x<LCD_W; x++) {
    ASSERT_IN_DISPLAY(p);
    *p++ ^= 0xff;
  }
}

void lcd_imgfar(coord_t x, coord_t y,  uint_farptr_t img, uint8_t idx, LcdFlags att) // progmem "far"
{
  uint_farptr_t q = img;
  uint8_t w    = pgm_read_byte_far(q++);
  uint8_t hb   = (pgm_read_byte_far(q++)+7)/8;
  bool    inv  = (att & INVERS) ? true : (att & BLINK ? BLINK_ON_PHASE : false);
  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (coord_t i=0; i<w; i++) {
      uint8_t b = pgm_read_byte_far(q);
      q++;
      ASSERT_IN_DISPLAY(p);
      *p++ = inv ? ~b : b;
    }
  }
}

void lcd_img(coord_t x, coord_t y, const pm_uchar * img, uint8_t idx, LcdFlags att)
{
  const pm_uchar *q = img;
  uint8_t w    = pgm_read_byte_near(q++);
  uint8_t hb   = (pgm_read_byte_near(q++)+7)/8;
  bool    inv  = (att & INVERS) ? true : (att & BLINK ? BLINK_ON_PHASE : false);
  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (coord_t i=0; i<w; i++) {
      uint8_t b = pgm_read_byte_near(q);
      q++;
      ASSERT_IN_DISPLAY(p);
      *p++ = inv ? ~b : b;
    }
  }
}


# 1 "gui/splash.cpp"
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


#include "../OpenAVRc.h"

#if defined(SPLASH)

const pm_uchar splashdata[] PROGMEM = {
#include "../bitmaps/splash.lbm"
};

#if  defined(SIMU)
const pm_uchar * const splash_lbm = splashdata+4;
#endif

void displaySplash()
{
  lcdClear();
  lcd_imgfar(0, 0, (pgm_get_far_address(splashdata)), 0, 0); //use progmem "far" for splash working with all other options enabled

#if MENUS_LOCK == 1
  if (readonly == false) {
    lcdDrawFilledRect((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2 - 9, 50, (sizeof(TR_UNLOCKED)-1)*FW+16, 11, SOLID, ERASE|ROUND);
    lcdDrawText((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2, 53, STR_UNLOCKED);
  }
#endif

  lcdRefresh();
}
#endif
# 1 "gui/fonts.cpp"
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


#include "../OpenAVRc.h"

#if defined(FONTSTD)

const pm_uchar zzfont_5x7[] PROGMEM = {
#include "../fonts/std/font_05x07_avr.lbm"
#if defined(TRANSLATIONS_DE)
#include "../fonts/std/font_de_05x07.lbm"
#elif defined(TRANSLATIONS_CZ)
#include "../fonts/std/font_cz_05x07.lbm"
#elif defined(TRANSLATIONS_ES)
#include "../fonts/std/font_es_05x07.lbm"
#elif defined(TRANSLATIONS_FI)
#include "../fonts/std/font_fi_05x07.lbm"
#elif defined(TRANSLATIONS_FR)
#include "../fonts/std/font_fr_05x07.lbm"
#elif defined(TRANSLATIONS_IT)
#include "../fonts/std/font_it_05x07.lbm"
#elif defined(TRANSLATIONS_PL)
#include "../fonts/std/font_pl_05x07.lbm"
#elif defined(TRANSLATIONS_PT)
#include "../fonts/std/font_pt_05x07.lbm"
#elif defined(TRANSLATIONS_SE)
#include "../fonts/std/font_se_05x07.lbm"
#endif
};

#if defined(BOLD_SPECIFIC_FONT)
const pm_uchar zzfont_5x7_B[] PROGMEM = {
#include "../fonts/std/font_05x07_B_compressed.lbm"
};
#endif

const pm_uchar zzfont_10x14[] PROGMEM = {
#include "../fonts/std/font_10x14_compressed.lbm"
};

#else //SQT5

const pm_uchar zzfont_5x7[] PROGMEM = {
#include "../fonts/sqt5/font_05x07_avr.lbm"
#if defined(TRANSLATIONS_DE)
#include "../fonts/sqt5/font_de_05x07.lbm"
#elif defined(TRANSLATIONS_CZ)
#include "../fonts/sqt5/font_cz_05x07.lbm"
#elif defined(TRANSLATIONS_ES)
#include "../fonts/sqt5/font_es_05x07.lbm"
#elif defined(TRANSLATIONS_FI)
#include "../fonts/sqt5/font_fi_05x07.lbm"
#elif defined(TRANSLATIONS_FR)
#include "../fonts/sqt5/font_fr_05x07.lbm"
#elif defined(TRANSLATIONS_IT)
#include "../fonts/sqt5/font_it_05x07.lbm"
#elif defined(TRANSLATIONS_PL)
#include "../fonts/sqt5/font_pl_05x07.lbm"
#elif defined(TRANSLATIONS_PT)
#include "../fonts/sqt5/font_pt_05x07.lbm"
#elif defined(TRANSLATIONS_SE)
#include "../fonts/sqt5/font_se_05x07.lbm"
#endif
};

#if defined(BOLD_SPECIFIC_FONT)
const pm_uchar zzfont_5x7_B[] PROGMEM = {
#include "../fonts/sqt5/font_05x07_B_compressed.lbm"
};
#endif

const pm_uchar zzfont_10x14[] PROGMEM = {
#include "../fonts/sqt5/font_10x14_compressed.lbm"
};


#endif
# 1 "keys.cpp"
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


#include "OpenAVRc.h"
#include "keys.h"

uint8_t s_evt;
struct t_inactivity inactivity = {0};

uint8_t getEvent()
{
  uint8_t evt = s_evt;
  s_evt = 0;
  return evt;
}

#define KEY_LONG_DELAY 32

Key keys[NUM_KEYS];

void Key::input(bool val)
{
  uint8_t t_vals = m_vals ;
  t_vals <<= 1 ;
  if (val) t_vals |= 1; //portbit einschieben
  m_vals = t_vals ;

  m_cnt++;

  if (m_state && m_vals==0) {  //gerade eben sprung auf 0
    if (m_state != KSTATE_KILLED) {
      putEvent(EVT_KEY_BREAK(key()));
    }
    m_cnt   = 0;
    m_state = KSTATE_OFF;
  }
  switch(m_state) {
  case KSTATE_OFF:
    if (m_vals == FFVAL) { //gerade eben sprung auf ff
      m_state = KSTATE_START;
      m_cnt   = 0;
    }
    break;
  //fallthrough
  case KSTATE_START:
    putEvent(EVT_KEY_FIRST(key()));
    inactivity.counter = 0;
    m_state   = KSTATE_RPTDELAY;
    m_cnt     = 0;
    break;

  case KSTATE_RPTDELAY: // gruvin: delay state before first key repeat
    if (m_cnt == KEY_LONG_DELAY) {
      putEvent(EVT_KEY_LONG(key()));
    }
    if (m_cnt == 40) {
      m_state = 16;
      m_cnt = 0;
    }
    break;

  case 16:
  case 8:
  case 4:
  case 2:
    if (m_cnt >= 48)  { //3 6 12 24 48 pulses in every 480ms
      m_state >>= 1;
      m_cnt     = 0;
    }
  // no break
  case 1:
    if ((m_cnt & (m_state-1)) == 0) {
      putEvent(EVT_KEY_REPT(key()));
    }
    break;

  case KSTATE_PAUSE: //pause
    if (m_cnt >= 64) {
      m_state = 8;
      m_cnt   = 0;
    }
    break;

  case KSTATE_KILLED: //killed
    break;
  }
}

EnumKeys Key::key() const
{
  return static_cast<EnumKeys>(this - keys);
}

void pauseEvents(uint8_t event)
{
  event = EVT_KEY_MASK(event);
  if (event < (int)DIM(keys)) keys[event].pauseEvents();
}

void killEvents(uint8_t event)
{
#if defined(ROTARY_ENCODER_NAVIGATION)
  if (event == EVT_ROTARY_LONG) {
    killEvents(BTN_REa + NAVIGATION_RE_IDX());
  } else
#endif
  {
    event = EVT_KEY_MASK(event);
    if (event < (int)DIM(keys)) keys[event].killEvents();
  }
}

void clearKeyEvents()
{
  // loop until all keys are up
  while (keyDown()) {
    MYWDT_RESET();
  }
  memclear(keys, sizeof(keys));
  putEvent(0);
}


# 1 "maths.cpp"
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


#include "OpenAVRc.h"

// #define CORRECT_NEGATIVE_SHIFTS
// open.20.fsguruh; shift right operations do the rounding different for negative values compared to positive values
// so all negative divisions round always further down, which give absolute values bigger compared to a usual division
// this is noticable on the display, because instead of -100.0 -99.9 is shown; While in praxis I doublt somebody will notice a
// difference this is more a mental thing. Maybe people are distracted, because the easy calculations are obviously wrong
// this define would correct this, but costs 34 bytes code for stock version

// currently we set this to active always, because it might cause a fault about 1% compared positive and negative values
// is done now in makefile

int16_t calc100to256_16Bits(int16_t x) // return x*2.56
{
  // y = 2*x + x/2 +x/16-x/512-x/2048
  // 512 and 2048 are out of scope from int8 input --> forget it
#ifdef CORRECT_NEGATIVE_SHIFTS
  int16_t res=(int16_t)x<<1;
  //int8_t  sign=(uint8_t) x>>7;
  int8_t sign=(x<0?1:0);

  x-=sign;
  res+=(x>>1);
  res+=sign;
  res+=(x>>4);
  res+=sign;
  return res;
#else
  return ((int16_t)x<<1)+(x>>1)+(x>>4);
#endif
}

int16_t calc100to256(int8_t x) // return x*2.56
{
  return calc100to256_16Bits(x);
}

int16_t calc100toRESX_16Bits(int16_t x) // return x*10.24
{
#ifdef CORRECT_NEGATIVE_SHIFTS
  int16_t res= ((int16_t)x*41)>>2;
  int8_t sign=(x<0?1:0);
  //int8_t  sign=(uint8_t) x>>7;
  x-=sign;
  res-=(x>>6);
  res-=sign;
  return res;
#else
  // return (int16_t)x*10 + x/4 - x/64;
  return ((x*41)>>2) - (x>>6);
#endif
}

int16_t calc100toRESX(int8_t x) // return x*10.24
{
  return calc100toRESX_16Bits(x);
}

// return x*1.024
int16_t calc1000toRESX(int16_t x) // improve calc time by Pat MacKenzie
{
  // return x + x/32 - x/128 + x/512;
  int16_t y = x>>5;
  x+=y;
  y=y>>2;
  x-=y;
  return x+(y>>2);
}

int16_t calcRESXto1000(int16_t x)  // return x/1.024
{
// *1000/1024 = x - x/32 + x/128
  return (x - (x>>5) + (x>>7));
}

int8_t calcRESXto100(int16_t x)
{
  return (x*25) >> 8;
}

#if defined(HELI) || defined(FRSKY)
uint16_t isqrt32(uint32_t n)
{
  uint16_t c = 0x8000;
  uint16_t g = 0x8000;

  for (;;) {
    if ((uint32_t)g*g > n)
      g ^= c;
    c >>= 1;
    if(c == 0)
      return g;
    g |= c;
  }
}
#endif

/*
  Division by 10 and rounding or fixed point arithmetic values

  Examples:
    value -> result
    105 ->  11
    104 ->  10
   -205 -> -21
   -204 -> -20
*/
getvalue_t div10_and_round(getvalue_t value)
{
  if (value >= 0 ) {
    value += 5;
  } else {
    value -= 5;
  }
  return value/10;
}

getvalue_t div100_and_round(getvalue_t value)
{
  if (value >= 0 ) {
    value += 50;
  } else {
    value -= 50;
  }
  return value/100;
}


#if defined(FRSKY)
void extractLatitudeLongitude(uint32_t * latitude, uint32_t * longitude)
{
  div_t qr = div(telemetryData.value.gpsLatitude_bp, 100);
  *latitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + telemetryData.value.gpsLatitude_ap) * 5) / 3;

  qr = div(telemetryData.value.gpsLongitude_bp, 100);
  *longitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + telemetryData.value.gpsLongitude_ap) * 5) / 3;
}

void getGpsPilotPosition()
{
  extractLatitudeLongitude(&telemetryData.value.pilotLatitude, &telemetryData.value.pilotLongitude);
  uint32_t lat = telemetryData.value.pilotLatitude / 10000;
  uint32_t angle2 = (lat*lat) / 10000;
  uint32_t angle4 = angle2 * angle2;
  telemetryData.value.distFromEarthAxis = 139*(((uint32_t)10000000-((angle2*(uint32_t)123370)/81)+(angle4/25))/12500);
  //TRACE("telemetryData.value.distFromEarthAxis=%d", telemetryData.value.distFromEarthAxis);
}

void getGpsDistance()
{
  uint32_t lat, lng;

  extractLatitudeLongitude(&lat, &lng);

  // printf("lat=%d (%d), long=%d (%d)\n", lat, abs(lat - telemetryData.value.pilotLatitude), lng, abs(lng - telemetryData.value.pilotLongitude));

  uint32_t angle = (lat > telemetryData.value.pilotLatitude) ? lat - telemetryData.value.pilotLatitude : telemetryData.value.pilotLatitude - lat;
  uint32_t dist = EARTH_RADIUS * angle / 1000000;
  uint32_t result = dist*dist;

  angle = (lng > telemetryData.value.pilotLongitude) ? lng - telemetryData.value.pilotLongitude : telemetryData.value.pilotLongitude - lng;
  dist = telemetryData.value.distFromEarthAxis * angle / 1000000;
  result += dist*dist;

  dist = abs(TELEMETRY_BARO_ALT_AVAILABLE() ? TELEMETRY_RELATIVE_BARO_ALT_BP : TELEMETRY_RELATIVE_GPS_ALT_BP);
  result += dist*dist;

  telemetryData.value.gpsDistance = isqrt32(result);
  if (telemetryData.value.gpsDistance > telemetryData.value.maxGpsDistance)
    telemetryData.value.maxGpsDistance = telemetryData.value.gpsDistance;
}
#endif
# 1 "translations.cpp"
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


#include "OpenAVRc.h"

#define ISTR(x) LEN_##x TR_##x

// The non-0-terminated-strings
const pm_char STR_OPEN9X[] PROGMEM =
  ISTR(OFFON)
  ISTR(MMMINV)
  ISTR(NCHANNELS)
#if !defined(GRAPHICS)
  ISTR(VBEEPLEN)
#endif
  ISTR(VBEEPMODE)
#if defined(ROTARY_ENCODERS)
  ISTR(VRENAVIG)
#endif
#if defined(ROTARY_ENCODER_NAVIGATION)
  ISTR(VRENCODERS)
#endif
  ISTR(TRNMODE)
  ISTR(TRNCHN)
  ISTR(VTRIMINC)
  ISTR(RETA123)
#if defined(MULTIMODULE)
  ISTR(MULTIPROTOCOLS)
#endif
  ISTR(POSNEG)
  ISTR(VBLMODE)
  ISTR(VCURVEFUNC)
  ISTR(VMLTPX)
  ISTR(VMLTPX2)
  ISTR(VMIXTRIMS)
  ISTR(VCSWFUNC)
  ISTR(VFSWFUNC)
  ISTR(VFSWRESET)
  ISTR(FUNCSOUNDS)
  ISTR(VTELEMCHNS)
#if defined(FRSKY)
  ISTR(VTELEMUNIT)
  ISTR(VALARM)
  ISTR(VALARMFN)
  ISTR(VTELPROTO)
  ISTR(GPSFORMAT)
  ISTR(AMPSRC)
  ISTR(VARIOSRC)
  ISTR(VTELEMSCREENTYPE)
#endif
#if defined(TEMPLATES)
  ISTR(VTEMPLATES)
#endif
#if defined(HELI)
  ISTR(VSWASHTYPE)
#endif
  ISTR(VKEYS)
  ISTR(VSWITCHES)
  ISTR(VSRCRAW)
  ISTR(VTMRMODES)
  ISTR(DATETIME)
  ISTR(VPERSISTENT)
#if defined(PXX)
  ISTR(COUNTRYCODES)
  ISTR(VFAILSAFE)
#endif
#if defined(MAVLINK)
  ISTR(MAVLINK_BAUDS)
  ISTR(MAVLINK_AC_MODES)
  ISTR(MAVLINK_AP_MODES)
#endif
  ;

// The 0-terminated-strings
const pm_char STR_POPUPS[] PROGMEM = TR_POPUPS;
#if !defined(OFS_EXIT)
const pm_char STR_EXIT[] PROGMEM = TR_EXIT;
#endif
const pm_char STR_MENUWHENDONE[] PROGMEM = TR_MENUWHENDONE;
const pm_char STR_FREE[] PROGMEM = TR_FREE;
const pm_char STR_DELETEMODEL[] PROGMEM = TR_DELETEMODEL;
const pm_char STR_COPYINGMODEL[] PROGMEM = TR_COPYINGMODEL;
const pm_char STR_MOVINGMODEL[] PROGMEM = TR_MOVINGMODEL;
const pm_char STR_LOADINGMODEL[] PROGMEM = TR_LOADINGMODEL;
const pm_char STR_NAME[] PROGMEM = TR_NAME;
const pm_char STR_BITMAP[] PROGMEM = TR_BITMAP;
const pm_char STR_TIMER[] PROGMEM = TR_TIMER;
const pm_char STR_ELIMITS[] PROGMEM = TR_ELIMITS;
const pm_char STR_ETRIMS[] PROGMEM = TR_ETRIMS;
const pm_char STR_TRIMINC[] PROGMEM = TR_TRIMINC;
const pm_char STR_DISPLAY_TRIMS[] PROGMEM = TR_DISPLAY_TRIMS;
const pm_char STR_TTRACE[] PROGMEM = TR_TTRACE;
const pm_char STR_TSWITCH[] PROGMEM = TR_TSWITCH;
const pm_char STR_TTRIM[] PROGMEM = TR_TTRIM;
const pm_char STR_BEEPCTR[] PROGMEM = TR_BEEPCTR;
const pm_char STR_USE_GLOBAL_FUNCS[] PROGMEM = TR_USE_GLOBAL_FUNCS;
const pm_char STR_PROTO[] PROGMEM = TR_PROTO;
const pm_char STR_PPMFRAME[] PROGMEM = TR_PPMFRAME;
const pm_char STR_MS[] PROGMEM = TR_MS;
const pm_char STR_SWITCH[] PROGMEM = TR_SWITCH;
const pm_char STR_TRIMS[] PROGMEM = TR_TRIMS;
const pm_char STR_FADEIN[] PROGMEM = TR_FADEIN;
const pm_char STR_FADEOUT[] PROGMEM = TR_FADEOUT;
const pm_char STR_DEFAULT[] PROGMEM = TR_DEFAULT;
const pm_char STR_CHECKTRIMS[] PROGMEM = TR_CHECKTRIMS;
#ifdef HELI
const pm_char STR_SWASHTYPE[] PROGMEM = TR_SWASHTYPE;
const pm_char STR_COLLECTIVE[] PROGMEM = TR_COLLECTIVE;
const pm_char STR_AILERON[] PROGMEM = TR_AILERON;
const pm_char STR_ELEVATOR[] PROGMEM = TR_ELEVATOR;
const pm_char STR_SWASHRING[] PROGMEM = TR_SWASHRING;
const pm_char STR_ELEDIRECTION[] PROGMEM = TR_ELEDIRECTION;
const pm_char STR_AILDIRECTION[] PROGMEM = TR_AILDIRECTION;
const pm_char STR_COLDIRECTION[] PROGMEM = TR_COLDIRECTION;
#endif
const pm_char STR_MODE[] PROGMEM = TR_MODE;
#if defined(AUDIO) && defined(BUZZER)
const pm_char STR_SPEAKER[] PROGMEM = TR_SPEAKER;
const pm_char STR_BUZZER[] PROGMEM = TR_BUZZER;
#endif
const pm_char STR_NOFREEEXPO[] PROGMEM = TR_NOFREEEXPO;
const pm_char STR_NOFREEMIXER[] PROGMEM = TR_NOFREEMIXER;
const pm_char STR_INSERTMIX[] PROGMEM = TR_INSERTMIX;
const pm_char STR_EDITMIX[] PROGMEM = TR_EDITMIX;
const pm_char STR_SOURCE[] PROGMEM = TR_SOURCE;
const pm_char STR_WEIGHT[] PROGMEM = TR_WEIGHT;
const pm_char STR_EXPO[] PROGMEM = TR_EXPO;
const pm_char STR_SIDE[] PROGMEM = TR_SIDE;
const pm_char STR_DIFFERENTIAL[] PROGMEM = TR_DIFFERENTIAL;
const pm_char STR_OFFSET[] PROGMEM = TR_OFFSET;
const pm_char STR_TRIM[] PROGMEM = TR_TRIM;
const pm_char STR_DREX[] PROGMEM = TR_DREX;
const pm_char STR_CURVE[] PROGMEM = TR_CURVE;
const pm_char STR_FLMODE[] PROGMEM = TR_FLMODE;
const pm_char STR_MIXWARNING[] PROGMEM = TR_MIXWARNING;
const pm_char STR_OFF[] PROGMEM = TR_OFF;
const pm_char STR_MULTPX[] PROGMEM = TR_MULTPX;
const pm_char STR_DELAYDOWN[] PROGMEM = TR_DELAYDOWN;
const pm_char STR_DELAYUP[] PROGMEM = TR_DELAYUP;
const pm_char STR_SLOWDOWN[] PROGMEM = TR_SLOWDOWN;
const pm_char STR_SLOWUP[] PROGMEM = TR_SLOWUP;
const pm_char STR_MIXER[] PROGMEM = TR_MIXER;
const pm_char STR_CV[] PROGMEM = TR_CV;
const pm_char STR_GV[] PROGMEM = TR_GV;
const pm_char STR_ACHANNEL[] PROGMEM = TR_ACHANNEL;
const pm_char STR_RANGE[] PROGMEM = TR_RANGE;
const pm_char STR_CENTER[] PROGMEM = TR_CENTER;
const pm_char STR_BAR[] PROGMEM = TR_BAR;
const pm_char STR_ALARM[] PROGMEM = TR_ALARM;
const pm_char STR_USRDATA[] PROGMEM = TR_USRDATA;
const pm_char STR_BLADES[] PROGMEM = TR_BLADES;
const pm_char STR_SCREEN[] PROGMEM = TR_SCREEN;
const pm_char STR_SOUND_LABEL[] PROGMEM = TR_SOUND_LABEL;
const pm_char STR_LENGTH[] PROGMEM = TR_LENGTH;
#if defined(AUDIO)
const pm_char STR_SPKRPITCH[] PROGMEM = TR_SPKRPITCH;
#endif
#if defined(HAPTIC)
const pm_char STR_HAPTIC_LABEL[] PROGMEM = TR_HAPTIC_LABEL;
//const pm_char STR_HAPTICSTRENGTH[] PROGMEM = TR_HAPTICSTRENGTH;
#endif
const pm_char STR_CONTRAST[] PROGMEM = TR_CONTRAST;
const pm_char STR_ALARMS_LABEL[] PROGMEM = TR_ALARMS_LABEL;
#if defined(BATTGRAPH)
const pm_char STR_BATTERY_RANGE[] PROGMEM = TR_BATTERY_RANGE;
#endif
const pm_char STR_BATTERYWARNING[] PROGMEM = TR_BATTERYWARNING;
const pm_char STR_INACTIVITYALARM[] PROGMEM = TR_INACTIVITYALARM;
const pm_char STR_MEMORYWARNING[] PROGMEM = TR_MEMORYWARNING;
const pm_char STR_ALARMWARNING[] PROGMEM = TR_ALARMWARNING;
#if defined(ROTARY_ENCODERS)
const pm_char STR_RENAVIG[] PROGMEM = TR_RENAVIG;
#endif
const pm_char STR_THROTTLEREVERSE[] PROGMEM = TR_THROTTLEREVERSE;
const pm_char STR_TIMER_NAME[] PROGMEM = TR_TIMER_NAME;
const pm_char STR_MINUTEBEEP[] PROGMEM = TR_MINUTEBEEP;
const pm_char STR_BEEPCOUNTDOWN[] PROGMEM = TR_BEEPCOUNTDOWN;
const pm_char STR_PERSISTENT[] PROGMEM = TR_PERSISTENT;
const pm_char STR_BACKLIGHT_LABEL[] PROGMEM = TR_BACKLIGHT_LABEL;
const pm_char STR_BLDELAY[] PROGMEM = TR_BLDELAY;

#if defined(PWM_BACKLIGHT)
const pm_char STR_BLONBRIGHTNESS[] PROGMEM = TR_BLONBRIGHTNESS;
const pm_char STR_BLOFFBRIGHTNESS[] PROGMEM = TR_BLOFFBRIGHTNESS;
#endif

const pm_char STR_SPLASHSCREEN[] PROGMEM = TR_SPLASHSCREEN;
const pm_char STR_THROTTLEWARNING[] PROGMEM = TR_THROTTLEWARNING;
const pm_char STR_SWITCHWARNING[] PROGMEM = TR_SWITCHWARNING;
#ifdef FRSKY
const pm_char STR_TIMEZONE[] PROGMEM = TR_TIMEZONE;
const pm_char STR_ADJUST_RTC[] PROGMEM = TR_ADJUST_RTC;
const pm_char STR_GPS[] PROGMEM = TR_GPS;
const pm_char STR_GPSCOORD[] PROGMEM = TR_GPSCOORD;
const pm_char STR_VARIO[] PROGMEM = TR_VARIO;
const pm_char STR_PITCH_AT_ZERO[] PROGMEM = TR_PITCH_AT_ZERO;
const pm_char STR_PITCH_AT_MAX[] PROGMEM = TR_PITCH_AT_MAX;
const pm_char STR_REPEAT_AT_ZERO[] PROGMEM = TR_REPEAT_AT_ZERO;
#endif
const pm_char STR_RXCHANNELORD[] PROGMEM = TR_RXCHANNELORD;
const pm_char STR_STICKS[] PROGMEM = TR_STICKS;
const pm_char STR_POTS[] PROGMEM = TR_POTS;
const pm_char STR_SWITCHES[] PROGMEM = TR_SWITCHES;
const pm_char STR_SWITCHES_DELAY[] PROGMEM = TR_SWITCHES_DELAY;
const pm_char STR_SLAVE[] PROGMEM = TR_SLAVE;
const pm_char STR_MODESRC[] PROGMEM = TR_MODESRC;
const pm_char STR_MULTIPLIER[] PROGMEM = TR_MULTIPLIER;
const pm_char STR_CAL[] PROGMEM = TR_CAL;
const pm_char STR_VTRIM[] PROGMEM = TR_VTRIM;
const pm_char STR_BG[] PROGMEM = TR_BG;
const pm_char STR_MENUTOSTART[] PROGMEM = TR_MENUTOSTART;
const pm_char STR_SETMIDPOINT[] PROGMEM = TR_SETMIDPOINT;
const pm_char STR_MOVESTICKSPOTS[] PROGMEM = TR_MOVESTICKSPOTS;
const pm_char STR_RXBATT[] PROGMEM = TR_RXBATT;
const pm_char STR_TX[] PROGMEM = TR_TXnRX;
const pm_char STR_ACCEL[] PROGMEM = TR_ACCEL;
const pm_char STR_NODATA[] PROGMEM = TR_NODATA;
const pm_char STR_TOTTM1TM2THRTHP[] PROGMEM = TR_TOTTM1TM2THRTHP;
const pm_char STR_TMR1LATMAXUS[] PROGMEM = TR_TMR1LATMAXUS;
const pm_char STR_TMR1LATMINUS[] PROGMEM = TR_TMR1LATMINUS;
const pm_char STR_TMR1JITTERUS[] PROGMEM = TR_TMR1JITTERUS;
const pm_char STR_US[] PROGMEM = TR_US;
const pm_char STR_TMIXMAXMS[] PROGMEM = TR_TMIXMAXMS;
const pm_char STR_FREESTACKMINB[] PROGMEM = TR_FREESTACKMINB;
const pm_char STR_MENUTORESET[] PROGMEM = TR_MENUTORESET;
const pm_char STR_PPM_TRAINER[] PROGMEM = TR_PPM_TRAINER;
const pm_char STR_CH[] PROGMEM = TR_CH;
const pm_char STR_MODEL[] PROGMEM = TR_MODEL;
const pm_char STR_FP[] PROGMEM = TR_FP;
const pm_char STR_MIX[] PROGMEM = TR_MIX;
const pm_char STR_EEPROMLOWMEM[] PROGMEM = TR_EEPROMLOWMEM;
const pm_char STR_ALERT[] PROGMEM = TR_ALERT;
const pm_char STR_PRESSANYKEYTOSKIP[] PROGMEM = TR_PRESSANYKEYTOSKIP;
const pm_char STR_THROTTLENOTIDLE[] PROGMEM = TR_THROTTLENOTIDLE;
const pm_char STR_ALARMSDISABLED[] PROGMEM = TR_ALARMSDISABLED;
const pm_char STR_PRESSANYKEY[] PROGMEM = TR_PRESSANYKEY;
const pm_char STR_BADEEPROMDATA[] PROGMEM = TR_BADEEPROMDATA;
const pm_char STR_EEPROMFORMATTING[] PROGMEM = TR_EEPROMFORMATTING;
const pm_char STR_EEPROMOVERFLOW[] PROGMEM = TR_EEPROMOVERFLOW;
const pm_char STR_TRIMS2OFFSETS[] PROGMEM = TR_TRIMS2OFFSETS;
const pm_char STR_MENURADIOSETUP[] PROGMEM = TR_MENURADIOSETUP;

const pm_char STR_FAS_OFFSET[] PROGMEM = TR_FAS_OFFSET;

#if defined(SPIMODULES)
const pm_char STR_SPIM[] PROGMEM = TR_SPIM;
#endif
#if defined(MULTIMODULE)
const pm_char STR_MULTI_CUSTOM[] PROGMEM = TR_MULTI_CUSTOM;
#endif
#if defined(MULTIMODULE) || defined(SPIMODULES)
const pm_char STR_SUBTYPE[] PROGMEM = TR_SUBTYPE;
const pm_char STR_MULTI_VIDFREQ[] PROGMEM = TR_MULTI_VIDFREQ;
const pm_char STR_RFTUNE[] PROGMEM = TR_RFTUNE;
const pm_char STR_TELEMETRY[] PROGMEM = TR_TELEMETRY;
const pm_char STR_MULTI_RFPOWER[] PROGMEM = TR_MULTI_RFPOWER;
const pm_char STR_MULTI_SERVOFREQ[] PROGMEM = TR_MULTI_SERVOFREQ;
const pm_char STR_MULTI_OPTION[] PROGMEM = TR_MULTI_OPTION;
const pm_char STR_MULTI_DSM_AUTODTECT[] PROGMEM = TR_MULTI_DSM_AUTODTECT;
const pm_char STR_AUTOBIND[] PROGMEM = TR_AUTOBIND;
const pm_char STR_MULTI_LOWPOWER[] PROGMEM = TR_MULTI_LOWPOWER;
#endif

const pm_char STR_MENUDATEANDTIME[] PROGMEM = TR_MENUDATEANDTIME;
const pm_char STR_FREERAMINB[] PROGMEM = "Free Ram\037\124b";

const pm_char STR_MENUTRAINER[] PROGMEM = TR_MENUTRAINER;
const pm_char STR_MENUGLOBALFUNCS[] PROGMEM = TR_MENUGLOBALFUNCS;
const pm_char STR_MENUVERSION[] PROGMEM = TR_MENUVERSION;
const pm_char STR_MENUDIAG[] PROGMEM = TR_MENUDIAG;
const pm_char STR_MENUANA[] PROGMEM = TR_MENUANA;
const pm_char STR_MENUCALIBRATION[] PROGMEM = TR_MENUCALIBRATION;

const pm_char STR_MENUMODELSEL[] PROGMEM = TR_MENUMODELSEL;
const pm_char STR_MENUSETUP[] PROGMEM = TR_MENUSETUP;
const pm_char STR_MENUFLIGHTPHASE[] PROGMEM = TR_MENUFLIGHTPHASE;
const pm_char STR_MENUFLIGHTPHASES[] PROGMEM = TR_MENUFLIGHTPHASES;

#ifdef HELI
const pm_char STR_MENUHELISETUP[] PROGMEM = TR_MENUHELISETUP;
#endif

const pm_char STR_MENUINPUTS[] PROGMEM = TR_MENUINPUTS;
const pm_char STR_MENULIMITS[] PROGMEM = TR_MENULIMITS;
const pm_char STR_MENUCURVES[] PROGMEM = TR_MENUCURVES;
const pm_char STR_MENUCURVE[] PROGMEM = TR_MENUCURVE;
const pm_char STR_MENULOGICALSWITCH[] PROGMEM = TR_MENULOGICALSWITCH;
const pm_char STR_MENULOGICALSWITCHES[] PROGMEM = TR_MENULOGICALSWITCHES;
const pm_char STR_MENUCUSTOMFUNC[] PROGMEM = TR_MENUCUSTOMFUNC;


#if defined(FRSKY)
const pm_char STR_MENUTELEMETRY[] PROGMEM = TR_MENUTELEMETRY;
const pm_char STR_LIMIT[] PROGMEM = TR_LIMIT;
#endif

#if defined(TEMPLATES)
const pm_char STR_MENUTEMPLATES[] PROGMEM = TR_MENUTEMPLATES;
#endif

const pm_char STR_MENUSTAT[] PROGMEM = TR_MENUSTAT;
const pm_char STR_MENUDEBUG[] PROGMEM = TR_MENUDEBUG;
const pm_char STR_MENUGLOBALVARS[] PROGMEM = TR_MENUGLOBALVARS;


#if defined(PXX)
const pm_char STR_SYNCMENU[] PROGMEM = TR_SYNCMENU;
const pm_char STR_INTERNALRF[] PROGMEM = TR_INTERNALRF;
const pm_char STR_EXTERNALRF[] PROGMEM = TR_EXTERNALRF;
const pm_char STR_COUNTRYCODE[] PROGMEM = TR_COUNTRYCODE;
const pm_char STR_FAILSAFE[] PROGMEM = TR_FAILSAFE;
const pm_char STR_FAILSAFESET[] PROGMEM = TR_FAILSAFESET;
const pm_char STR_MENUSENSOR[] PROGMEM = TR_MENUSENSOR;
#endif

const pm_char STR_INVERT_THR[] PROGMEM = TR_INVERT_THR;
const pm_char STR_AND_SWITCH[] PROGMEM = TR_AND_SWITCH;
const pm_char STR_SF[] PROGMEM = TR_SF;
const pm_char STR_GF[] PROGMEM = TR_GF;

const pm_char STR_SHUTDOWN[] PROGMEM = TR_SHUTDOWN;
const pm_char STR_SAVEMODEL[] PROGMEM = TR_SAVEMODEL;


const pm_char STR_BATT_CALIB[] PROGMEM = TR_BATT_CALIB;

#if defined(FRSKY)
const pm_char STR_MINRSSI[] PROGMEM = TR_MINRSSI;
const pm_char STR_LATITUDE[] PROGMEM = TR_LATITUDE;
const pm_char STR_LONGITUDE[] PROGMEM = TR_LONGITUDE;
const pm_char STR_VOLTAGE[] PROGMEM = TR_VOLTAGE;
const pm_char STR_CURRENT[] PROGMEM = TR_CURRENT;
#endif


#if defined(NAVIGATION_MENUS)
const pm_char STR_SELECT_MODEL[] PROGMEM = TR_SELECT_MODEL;
const pm_char STR_CREATE_MODEL[] PROGMEM = TR_CREATE_MODEL;
const pm_char STR_COPY_MODEL[] PROGMEM = TR_COPY_MODEL;
const pm_char STR_MOVE_MODEL[] PROGMEM = TR_MOVE_MODEL;
const pm_char STR_DELETE_MODEL[] PROGMEM = TR_DELETE_MODEL;
const pm_char STR_EDIT[] PROGMEM = TR_EDIT;
const pm_char STR_INSERT_BEFORE[] PROGMEM = TR_INSERT_BEFORE;
const pm_char STR_INSERT_AFTER[] PROGMEM = TR_INSERT_AFTER;
const pm_char STR_COPY[] PROGMEM = TR_COPY;
const pm_char STR_MOVE[] PROGMEM = TR_MOVE;
const pm_char STR_PASTE[] PROGMEM = TR_PASTE;
const pm_char STR_INSERT[] PROGMEM = TR_INSERT;
const pm_char STR_DELETE[] PROGMEM = TR_DELETE;
const pm_char STR_RESET_FLIGHT[] PROGMEM = TR_RESET_FLIGHT;
const pm_char STR_RESET_TIMER1[] PROGMEM = TR_RESET_TIMER1;
const pm_char STR_RESET_TIMER2[] PROGMEM = TR_RESET_TIMER2;
const pm_char STR_RESET_TIMER3[] PROGMEM = TR_RESET_TIMER3;
const pm_char STR_RESET_TELEMETRY[] PROGMEM = TR_RESET_TELEMETRY;
const pm_char STR_STATISTICS[] PROGMEM = TR_STATISTICS;
const pm_char STR_SAVE_TIMERS[] PROGMEM = TR_SAVE_TIMERS;
#endif

const pm_char STR_RESET_BTN[] PROGMEM = TR_RESET_BTN;

#if defined(SDCARD)
const pm_char STR_BACKUP_MODEL[] PROGMEM = TR_BACKUP_MODEL;
const pm_char STR_RESTORE_MODEL[] PROGMEM = TR_RESTORE_MODEL;
const pm_char STR_SDCARD_ERROR[] PROGMEM = TR_SDCARD_ERROR;
const pm_char STR_NO_SDCARD[] PROGMEM = TR_NO_SDCARD;
const pm_char STR_SDCARD_FULL[] PROGMEM = TR_SDCARD_FULL;
const pm_char STR_INCOMPATIBLE[] PROGMEM = TR_INCOMPATIBLE;
const pm_char STR_LOGS_PATH[] PROGMEM = LOGS_PATH;
const pm_char STR_LOGS_EXT[] PROGMEM = LOGS_EXT;
const pm_char STR_MODELS_PATH[] PROGMEM = MODELS_PATH;
const pm_char STR_MODELS_EXT[] PROGMEM = MODELS_EXT;
#endif

const pm_char STR_WARNING[] PROGMEM = TR_WARNING;
const pm_char STR_EEPROMWARN[] PROGMEM = TR_EEPROMWARN;
const pm_char STR_EEPROM_CONVERTING[] PROGMEM = TR_EEPROM_CONVERTING;
const pm_char STR_THROTTLEWARN[] PROGMEM = TR_THROTTLEWARN;
const pm_char STR_ALARMSWARN[] PROGMEM = TR_ALARMSWARN;
const pm_char STR_SWITCHWARN[] PROGMEM = TR_SWITCHWARN;
const pm_char STR_FAILSAFEWARN[] PROGMEM = TR_FAILSAFEWARN;
const pm_char STR_NO_FAILSAFE[] PROGMEM = TR_NO_FAILSAFE;
const pm_char STR_KEYSTUCK[] PROGMEM = TR_KEYSTUCK;

const pm_char STR_SPEAKER_VOLUME[] PROGMEM = TR_SPEAKER_VOLUME;
const pm_char STR_LCD[] PROGMEM = TR_LCD;
const pm_char STR_FUNC[] PROGMEM = TR_FUNC;
const pm_char STR_V1[] PROGMEM = TR_V1;
const pm_char STR_V2[] PROGMEM = TR_V2;
const pm_char STR_DURATION[] PROGMEM = TR_DURATION;
const pm_char STR_DELAY[] PROGMEM = TR_DELAY;
const pm_char STR_SD_CARD[] PROGMEM = TR_SD_CARD;
const pm_char STR_SDHC_CARD[] PROGMEM = TR_SDHC_CARD;
const pm_char STR_NO_SOUNDS_ON_SD[] PROGMEM = TR_NO_SOUNDS_ON_SD;
const pm_char STR_NO_MODELS_ON_SD[] PROGMEM = TR_NO_MODELS_ON_SD;
const pm_char STR_NO_BITMAPS_ON_SD[] PROGMEM = TR_NO_BITMAPS_ON_SD;
const pm_char STR_NO_SCRIPTS_ON_SD[] PROGMEM = TR_NO_SCRIPTS_ON_SD;
const pm_char STR_SCRIPT_SYNTAX_ERROR[] PROGMEM = TR_SCRIPT_SYNTAX_ERROR;
const pm_char STR_SCRIPT_PANIC[] PROGMEM = TR_SCRIPT_PANIC;
const pm_char STR_SCRIPT_KILLED[] PROGMEM = TR_SCRIPT_KILLED;
const pm_char STR_SCRIPT_ERROR[] PROGMEM = TR_SCRIPT_ERROR;
const pm_char STR_PLAY_FILE[] PROGMEM = TR_PLAY_FILE;
const pm_char STR_ASSIGN_BITMAP[] PROGMEM = TR_ASSIGN_BITMAP;
const pm_char STR_EXECUTE_FILE[] PROGMEM = TR_EXECUTE_FILE;
const pm_char STR_DELETE_FILE[] PROGMEM = TR_DELETE_FILE;
const pm_char STR_COPY_FILE[] PROGMEM = TR_COPY_FILE;
const pm_char STR_RENAME_FILE[] PROGMEM = TR_RENAME_FILE;
const pm_char STR_SD_INFO[] PROGMEM = TR_SD_INFO;
const pm_char STR_SD_FORMAT[] PROGMEM = TR_SD_FORMAT;
const pm_char STR_REMOVED[] PROGMEM = TR_REMOVED;
const pm_char STR_NA[] PROGMEM = TR_NA;
const pm_char STR_HARDWARE[] PROGMEM = TR_HARDWARE;
const pm_char STR_FORMATTING[] PROGMEM = TR_FORMATTING;
const pm_char STR_TEMP_CALIB[] PROGMEM = TR_TEMP_CALIB;
const pm_char STR_TIME[] PROGMEM = TR_TIME;
const pm_char STR_BAUDRATE[] PROGMEM = TR_BAUDRATE;
const pm_char STR_SD_INFO_TITLE[] PROGMEM = TR_SD_INFO_TITLE;
const pm_char STR_SD_TYPE[] PROGMEM = TR_SD_TYPE;
const pm_char STR_SD_SPEED[] PROGMEM = TR_SD_SPEED;
const pm_char STR_SD_SECTORS[] PROGMEM = TR_SD_SECTORS;
const pm_char STR_SD_SIZE[] PROGMEM = TR_SD_SIZE;
const pm_char STR_TYPE[] PROGMEM = TR_TYPE;
const pm_char STR_GLOBAL_VARS[] PROGMEM = TR_GLOBAL_VARS;
const pm_char STR_GLOBAL_V[] PROGMEM = TR_GLOBAL_V;
const pm_char STR_GLOBAL_VAR[] PROGMEM = TR_GLOBAL_VAR;
const pm_char STR_OWN[] PROGMEM = TR_OWN;
const pm_char STR_ROTARY_ENCODER[] PROGMEM = TR_ROTARY_ENCODER;
const pm_char STR_DATE[] PROGMEM = TR_DATE;
const pm_char STR_CHANNELS_MONITOR[] PROGMEM = TR_CHANNELS_MONITOR;
const pm_char STR_MIXERS_MONITOR[] PROGMEM = TR_MIXERS_MONITOR;
const pm_char STR_PATH_TOO_LONG[] PROGMEM = TR_PATH_TOO_LONG;
const pm_char STR_VIEW_TEXT[] PROGMEM = TR_VIEW_TEXT;
const pm_char STR_FLASH_BOOTLOADER[] PROGMEM = TR_FLASH_BOOTLOADER;
const pm_char STR_FLASH_INTERNAL_MODULE[] PROGMEM = TR_FLASH_INTERNAL_MODULE;
const pm_char STR_FLASH_EXTERNAL_DEVICE[] PROGMEM = TR_FLASH_EXTERNAL_DEVICE;
const pm_char STR_WRITING[] PROGMEM = TR_WRITING;
const pm_char STR_CONFIRM_FORMAT[] PROGMEM = TR_CONFIRM_FORMAT;
const pm_char STR_EEBACKUP[] PROGMEM = TR_EEBACKUP;
const pm_char STR_FACTORYRESET[] PROGMEM = TR_FACTORYRESET;
const pm_char STR_BLCOLOR[]  PROGMEM = TR_BLCOLOR;
const pm_char STR_DUMMY[]  PROGMEM = "<N/A>";
const pm_char STR_POWEROFF[]  PROGMEM = TR_POWEROFF;
const pm_char STR_RFID[]  PROGMEM = "RFID  :";
const pm_char STR_BIND[]  PROGMEM = "    -BIND-     ";

#if MENUS_LOCK == 1
const pm_char STR_UNLOCKED[] PROGMEM = TR_UNLOCKED;
const pm_char STR_MODS_FORBIDDEN[] PROGMEM = TR_MODS_FORBIDDEN;
#endif

#if defined(DSM2) || defined(SPIMODULES) || defined(MULTIMODULE)
const pm_char STR_MODULE_RANGE[] PROGMEM = TR_MODULE_RANGE;
const pm_char STR_MODULE_BIND[] PROGMEM = TR_MODULE_BIND;
const pm_char STR_RXNUM[] PROGMEM = TR_RXNUM;
const pm_char STR_DSM_PROTOCOLS[] PROGMEM = TR_DSM_PROTOCOLS;
#endif

#if defined(MAVLINK)
const pm_char STR_MAVLINK_RC_RSSI_SCALE_LABEL[] PROGMEM = TR_MAVLINK_RC_RSSI_SCALE_LABEL;
const pm_char STR_MAVLINK_PC_RSSI_EN_LABEL[] PROGMEM = TR_MAVLINK_PC_RSSI_EN_LABEL;
const pm_char STR_MAVMENUSETUP_TITLE[] PROGMEM = TR_MAVMENUSETUP_TITLE;
const pm_char STR_MAVLINK_BAUD_LABEL[] PROGMEM = TR_MAVLINK_BAUD_LABEL;
const pm_char STR_MAVLINK_INFOS[] PROGMEM = TR_MAVLINK_INFOS;
const pm_char STR_MAVLINK_MODE[] PROGMEM = TR_MAVLINK_MODE;
const pm_char STR_MAVLINK_CUR_MODE[] PROGMEM = TR_MAVLINK_CUR_MODE;
const pm_char STR_MAVLINK_ARMED[] PROGMEM = TR_MAVLINK_ARMED;
const pm_char STR_MAVLINK_BAT_MENU_TITLE[] PROGMEM = TR_MAVLINK_BAT_MENU_TITLE;
const pm_char STR_MAVLINK_BATTERY_LABEL[] PROGMEM = TR_MAVLINK_BATTERY_LABEL;
const pm_char STR_MAVLINK_RC_RSSI_LABEL[] PROGMEM = TR_MAVLINK_RC_RSSI_LABEL;
const pm_char STR_MAVLINK_PC_RSSI_LABEL[] PROGMEM = TR_MAVLINK_PC_RSSI_LABEL;
const pm_char STR_MAVLINK_NAV_MENU_TITLE[] PROGMEM = TR_MAVLINK_NAV_MENU_TITLE;
const pm_char STR_MAVLINK_COURSE[] PROGMEM = TR_MAVLINK_COURSE;
const pm_char STR_MAVLINK_HEADING[] PROGMEM = TR_MAVLINK_HEADING;
const pm_char STR_MAVLINK_BEARING[] PROGMEM = TR_MAVLINK_BEARING;
const pm_char STR_MAVLINK_ALTITUDE[] PROGMEM = TR_MAVLINK_ALTITUDE;
const pm_char STR_MAVLINK_GPS[] PROGMEM = TR_MAVLINK_GPS;
const pm_char STR_MAVLINK_NO_FIX[] PROGMEM = TR_MAVLINK_NO_FIX;
const pm_char STR_MAVLINK_SAT[] PROGMEM = TR_MAVLINK_SAT;
const pm_char STR_MAVLINK_HDOP[] PROGMEM = TR_MAVLINK_HDOP;
const pm_char STR_MAVLINK_LAT[] PROGMEM = TR_MAVLINK_LAT;
const pm_char STR_MAVLINK_LON[] PROGMEM = TR_MAVLINK_LON;
#endif


# 1 "translations/tts_fr.cpp"
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


#include "../OpenAVRc.h"

enum FrenchPrompts {
  FR_PROMPT_NUMBERS_BASE = 0,
  FR_PROMPT_ZERO = FR_PROMPT_NUMBERS_BASE+0,
  FR_PROMPT_CENT = FR_PROMPT_NUMBERS_BASE+100,
  FR_PROMPT_DEUX_CENT = FR_PROMPT_CENT+1,
  FR_PROMPT_MILLE = FR_PROMPT_CENT+9,

  FR_PROMPT_UNE = 110,
  FR_PROMPT_ONZE,
  FR_PROMPT_VINGT_ET_UNE,
  FR_PROMPT_TRENTE_ET_UNE,
  FR_PROMPT_QUARANTE_ET_UNE,
  FR_PROMPT_CINQUANTE_ET_UNE,
  FR_PROMPT_SOIXANTE_ET_UNE,
  FR_PROMPT_SOIXANTE_ET_ONZE,
  FR_PROMPT_QUATRE_VINGT_UNE,

  FR_PROMPT_VIRGULE = 119,
  FR_PROMPT_ET = 120,
  FR_PROMPT_MOINS = 121,
  FR_PROMPT_MINUIT = 122,
  FR_PROMPT_MIDI = 123,

  FR_PROMPT_UNITS_BASE = 124,

  FR_PROMPT_HEURE = FR_PROMPT_UNITS_BASE+UNIT_HOURS+1, //I don't know why different ...
  FR_PROMPT_MINUTE = FR_PROMPT_UNITS_BASE+UNIT_MINUTES+1,
  FR_PROMPT_SECONDE = FR_PROMPT_UNITS_BASE+UNIT_SECONDS+1,
  FR_PROMPT_VIRGULE_BASE = 180, //,0 - ,9
};

#if defined(VOICE)

#define FEMININ 0x80

I18N_PLAY_FUNCTION(fr, playNumber, getvalue_t number, uint8_t unit, uint8_t att)
{
  /*  if digit >= 1000000000:
        temp_digit, digit = divmod(digit, 1000000000)
        prompts.extend(self.getNumberPrompt(temp_digit))
        prompts.append(Prompt(GUIDE_00_BILLION, dir=2))
    if digit >= 1000000:
        temp_digit, digit = divmod(digit, 1000000)
        prompts.extend(self.getNumberPrompt(temp_digit))
        prompts.append(Prompt(GUIDE_00_MILLION, dir=2))
  */

  if (number < 0) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_MOINS);
    number = -number;
  }

  int8_t mode = MODE(att);
  if (mode > 0) {
    // we assume that we are PREC1
    div_t qr = div(number, 10);
    if (qr.rem) {
      PLAY_NUMBER(qr.quot, 0, 0);
      PUSH_NUMBER_PROMPT(FR_PROMPT_VIRGULE_BASE + qr.rem);
      number = -1;
    } else {
      number = qr.quot;
    }
  }

  if (number >= 1000) {
    if (number >= 2000)
      PLAY_NUMBER(number / 1000, 0, 0);
    PUSH_NUMBER_PROMPT(FR_PROMPT_MILLE);
    number %= 1000;
    if (number == 0)
      number = -1;
  }
  if (number >= 100) {
    if (number >= 200)
      PUSH_NUMBER_PROMPT(FR_PROMPT_ZERO + number/100);
    PUSH_NUMBER_PROMPT(FR_PROMPT_CENT);
    number %= 100;
    if (number == 0)
      number = -1;
  }
  if (((number % 10) == 1) && number < 90 && (att & FEMININ)) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_UNE+(number/10));
  } else if (number >= 0) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_ZERO+number);
  }

  if (unit) {
    unit--;
    convertUnit(number, unit);
    if (IS_IMPERIAL_ENABLE()) {
      if (unit == UNIT_DIST) {
        unit = UNIT_FEET;
      }
      if (unit == UNIT_SPEED) {
        unit = UNIT_KTS;
      }
    }
    unit++;
    PUSH_NUMBER_PROMPT(FR_PROMPT_UNITS_BASE+unit);
  }
}

I18N_PLAY_FUNCTION(fr, playDuration, int seconds PLAY_DURATION_ATT)
{
  if (seconds == 0) {
    PLAY_NUMBER(0, 0, FEMININ);
    return;
  }

  if (seconds < 0) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_MOINS);
    seconds = -seconds;
  }

  uint8_t tmp = seconds / 3600;
  seconds %= 3600;
  if (IS_PLAY_TIME() && tmp==0) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_MINUIT);
  } else if (IS_PLAY_TIME() && tmp==12) {
    PUSH_NUMBER_PROMPT(FR_PROMPT_MIDI);
  } else if (tmp > 0) {
    PLAY_NUMBER(tmp, 0, FEMININ);
    PUSH_NUMBER_PROMPT(FR_PROMPT_HEURE);
  }

  tmp = seconds / 60;
  seconds %= 60;
  if (tmp > 0) {
    if (IS_PLAY_TIME()) {
      PLAY_NUMBER(tmp, 0, tmp==1 ? FEMININ : 0);
    } else {
      PLAY_NUMBER(tmp, 0, FEMININ);
      PUSH_NUMBER_PROMPT(FR_PROMPT_MINUTE);
#if !defined(NOANDSECONDE)
      if (seconds > 0) PUSH_NUMBER_PROMPT(FR_PROMPT_ET);
#endif
    }
  }

  if (!IS_PLAY_TIME() && seconds > 0) {
    PLAY_NUMBER(seconds, 0, FEMININ);
    PUSH_NUMBER_PROMPT(FR_PROMPT_SECONDE);
  }
}

LANGUAGE_PACK_DECLARE(fr, "Francais");

#endif
# 1 "eeprom_common.cpp"
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


#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "OpenAVRc.h"
#include "timers.h"

uint8_t   s_eeDirtyMsk;
tmr10ms_t s_eeDirtyTime10ms;

void eeDirty(uint8_t msk)
{
  s_eeDirtyMsk |= msk;
  s_eeDirtyTime10ms = get_tmr10ms() ;
}

uint8_t eeFindEmptyModel(uint8_t id, bool down)
{
  uint8_t i = id;
  for (;;) {
    i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
    if (!eeModelExists(i)) break;
    if (i == id) return 0xff; // no free space in directory left
  }
  return i;
}

void selectModel(uint8_t sub)
{
  displayPopup(STR_LOADINGMODEL);
  saveTimers();
  eeCheck(true); // force writing of current model data before this is changed
  g_eeGeneral.currModel = sub;
  eeDirty(EE_GENERAL);
  eeLoadModel(sub);
}

void eeReadAll()
{
  if (!eepromOpen() || !eeLoadGeneral()) {
    eeErase(true);
  } else {
    eeLoadModelHeaders();
  }
  stickMode = g_eeGeneral.stickMode;
  eeLoadModel(g_eeGeneral.currModel);
}
# 1 "eeprom_rlc.cpp"
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

#include "OpenAVRc.h"

uint8_t   s_write_err = 0;    // error reasons
RlcFile   theFile;  //used for any file operation
EeFs      eeFs;

#if defined(EXTERNALEEPROM)
blkid_t   freeBlocks = 0;
#endif

uint8_t  s_sync_write = false;

uint16_t eeprom_pointer;
uint8_t * eeprom_buffer_data;
volatile uint8_t eeprom_buffer_size = 0;

#if !defined(EXTERNALEEPROM) || defined(SIMU)
inline void eeprom_write_byte()
{
  EEAR = eeprom_pointer;
  EEDR = *eeprom_buffer_data;
// TODO add some define here
  EECR |= 1<<EEMPE;
  EECR |= 1<<EEPE;
#if defined(SIMU) //Use virtual register as possible ;-)
  simu_eeprom[EEAR] = EEDR;
#endif
  eeprom_pointer++;
  eeprom_buffer_data++;
}

ISR(EE_READY_vect)
{
  if (--eeprom_buffer_size) {
    eeprom_write_byte();
  } else {
    TRACE("ISR 'EE_READY_vect' Cleared : Eeprom is saved");
    EECR &= ~(1<<EERIE);
  }
}

void eepromWriteBlock(uint8_t * i_pointer_ram, uint16_t i_pointer_eeprom, size_t size)
{
  assert(!eeprom_buffer_size);

  eeprom_pointer = i_pointer_eeprom;
  eeprom_buffer_data = i_pointer_ram;
  eeprom_buffer_size = size+1;
  EECR |= (1<<EERIE);

  if (s_sync_write) {
    while (eeprom_buffer_size) MYWDT_RESET(EE_READY_vect()); //Simulate ISR in Simu mode, else reset watchdog
  }
}

#else // Use extern FRAM EEPROM IIC Mode

void Ext_eeprom_read_block(uint8_t * pointer_ram, uint16_t pointer_eeprom, size_t size)
{
  i2c_start(ADDRESS_EXTERN_EEPROM+I2C_WRITE);     // set device address and write mode
  i2c_write((uint8_t)(pointer_eeprom >> 8));      //MSB write address
  i2c_write((uint8_t)pointer_eeprom);             //LSB write address
  i2c_start(ADDRESS_EXTERN_EEPROM+I2C_READ);      // set device address and write mode
  --size;
  if (size) do {                                  // more than one value to read
    *pointer_ram++ = i2c_read_ack();              // read value from EEPROM
    --size;
  } while (size);
  *pointer_ram = i2c_read_nack();                 // read last value
  i2c_stop();                                     // set stop conditon = release bus
}

void eepromWriteBlock(uint8_t * i_pointer_ram, uint16_t i_pointer_eeprom, size_t size) // Write I2C eeprom
{
  assert(!eeprom_buffer_size);

  eeprom_pointer = i_pointer_eeprom;
  eeprom_buffer_data = i_pointer_ram;
  eeprom_buffer_size = size+1;

  i2c_start(ADDRESS_EXTERN_EEPROM+I2C_WRITE);     // set device address and write mode
  i2c_write((uint8_t)(i_pointer_eeprom >> 8)); //MSB write address
  i2c_write((uint8_t)i_pointer_eeprom); //LSB write address
  i2c_writeISR(*eeprom_buffer_data);    // write value to EEPROM
  ++eeprom_buffer_data; // increase data adress
  --eeprom_buffer_size; // one byte less to write
  if (s_sync_write) {
    while (eeprom_buffer_size) MYWDT_RESET(); // Wait completion and reset watchdog
  }
}

#endif

static uint8_t EeFsRead(blkid_t blk, uint8_t ofs)
{
  uint8_t ret;
  eepromReadBlock(&ret, (uint16_t)(blk*BS+ofs+BLOCKS_OFFSET), 1);
  return ret;
}

static blkid_t EeFsGetLink(blkid_t blk)
{
#if defined(EXTERNALEEPROM)
  blkid_t ret;
  eepromReadBlock((uint8_t *)&ret, blk*BS+BLOCKS_OFFSET, sizeof(blkid_t));
  return ret;
#else
  return EeFsRead(blk, 0);
#endif
}

static void EeFsSetLink(blkid_t blk, blkid_t val)
{
  static blkid_t s_link; // we write asynchronously, then nothing on the stack!
  s_link = val;
  eepromWriteBlock((uint8_t *)&s_link, (blk*BS)+BLOCKS_OFFSET, sizeof(blkid_t));
}

static uint8_t EeFsGetDat(blkid_t blk, uint8_t ofs)
{
  return EeFsRead(blk, ofs+sizeof(blkid_t));
}

static void EeFsSetDat(blkid_t blk, uint8_t ofs, uint8_t *buf, uint8_t len)
{
  eepromWriteBlock(buf, (blk*BS)+ofs+sizeof(blkid_t)+BLOCKS_OFFSET, len);
}

static void EeFsFlushFreelist()
{
  eepromWriteBlock((uint8_t *)&eeFs.freeList, offsetof(EeFs, freeList), sizeof(eeFs.freeList));
}

static void EeFsFlushDirEnt(uint8_t i_fileId)
{
  eepromWriteBlock((uint8_t *)&eeFs.files[i_fileId], offsetof(EeFs, files) + sizeof(DirEnt)*i_fileId, sizeof(DirEnt));
}

static void EeFsFlush()
{
  eepromWriteBlock((uint8_t *)&eeFs, 0, sizeof(eeFs));
}

uint16_t EeFsGetFree()
{
#if defined(EXTERNALEEPROM)
  int32_t ret = freeBlocks * (BS-sizeof(blkid_t));
#else
  int16_t ret = 0;
  blkid_t i = eeFs.freeList;
  while (i) {
    ret += BS-sizeof(blkid_t);
    i = EeFsGetLink(i);
  }
#endif
  ret += eeFs.files[FILE_TMP].size;
  ret -= eeFs.files[FILE_MODEL(g_eeGeneral.currModel)].size;
  return (ret > 0 ? ret : 0);
}

/// free one or more blocks
static void EeFsFree(blkid_t blk)
{
  blkid_t i = blk;
  blkid_t tmp;

#if defined(EXTERNALEEPROM)
  freeBlocks++;
#endif

  while ((tmp=EeFsGetLink(i))) {
    i = tmp;
#if defined(EXTERNALEEPROM)
    freeBlocks++;
#endif
  }

  EeFsSetLink(i, eeFs.freeList);
  eeFs.freeList = blk; //chain in front
  EeFsFlushFreelist();
}

void eepromCheck()
{
  ENABLE_SYNC_WRITE(true);

  uint8_t *bufp = (uint8_t *)&g_model;
  memclear(bufp, BLOCKS);
  blkid_t blk ;

#if defined(EXTERNALEEPROM)
  blkid_t blocksCount;
#endif
  for (uint8_t i=0; i<=MAXFILES; i++) {
#if defined(EXTERNALEEPROM)
    blocksCount = 0;
#endif
    blkid_t *startP = (i==MAXFILES ? &eeFs.freeList : &eeFs.files[i].startBlk);
    blkid_t lastBlk = 0;
    blk = *startP;
    while (blk) {
      if (blk < FIRSTBLK || // bad blk index
          blk >= BLOCKS  || // bad blk indexchan
          bufp[blk]) {      // blk double usage
        if (lastBlk) {
          EeFsSetLink(lastBlk, 0);
        } else {
          *startP = 0; // interrupt chain at startpos
          EeFsFlush();
        }
        blk = 0; // abort
      } else {
#if defined(EXTERNALEEPROM)
        blocksCount++;
#endif
        bufp[blk] = i+1;
        lastBlk   = blk;
        blk       = EeFsGetLink(blk);
      }
    }
  }

#if defined(EXTERNALEEPROM)
  freeBlocks = blocksCount;
#endif
  for (blk=FIRSTBLK; blk<BLOCKS; blk++) {
    if (!bufp[blk]) { // unused block
#if defined(EXTERNALEEPROM)
      freeBlocks++;
#endif
      EeFsSetLink(blk, eeFs.freeList);
      eeFs.freeList = blk; // chain in front
      EeFsFlushFreelist();
    }
  }

  ENABLE_SYNC_WRITE(false);
}

void eepromFormat()
{
  ENABLE_SYNC_WRITE(true);

  uint8_t fil[4] = {0xFF,0xFF,0xFF,0xFF};

  for (uint16_t i=0; i<(EESIZE/DIM(fil)); i+=DIM(fil)) // erase eeprom
  {
    eepromWriteBlock((uint8_t*)&fil, i, DIM(fil));
  }

#if defined(SIMU)
  // write zero to the end of the new EEPROM file to set it's proper size
  uint8_t dummy = 0;
  eepromWriteBlock(&dummy, EESIZE-1, 1);
#endif

  memclear(&eeFs, sizeof(eeFs));
  eeFs.version  = EEFS_VERS;
  eeFs.mySize   = sizeof(eeFs);
  eeFs.freeList = 0;
  eeFs.bs       = BS;
  for (blkid_t i=FIRSTBLK; i<BLOCKS-1; i++) {
    EeFsSetLink(i, i+1);
  }
  EeFsSetLink(BLOCKS-1, 0);
  eeFs.freeList = FIRSTBLK;
#if defined(EXTERNALEEPROM)
  freeBlocks = BLOCKS;
#endif
  EeFsFlush();

  ENABLE_SYNC_WRITE(false);
}

bool eepromOpen()
{
  eepromReadBlock((uint8_t *)&eeFs, 0, sizeof(eeFs));

#if defined(SIMU)
  if (eeFs.version != EEFS_VERS) {
    TRACE("bad eeFs.version (%d instead of %d)", eeFs.version, EEFS_VERS);
  }
  if (eeFs.mySize != sizeof(eeFs)) {
    TRACE("bad eeFs.mySize (%d instead of %d)", (int)eeFs.mySize, (int)sizeof(eeFs));
  }
#endif

  if (eeFs.version != EEFS_VERS || eeFs.mySize != sizeof(eeFs)) {
    return false;
  }

  eepromCheck();
  return true;
}

bool EFile::exists(uint8_t i_fileId)
{
  return eeFs.files[i_fileId].startBlk;
}

/*
 * Swap two files in eeprom
 */
void EFile::swap(uint8_t i_fileId1, uint8_t i_fileId2)
{
  DirEnt            tmp = eeFs.files[i_fileId1];
  eeFs.files[i_fileId1] = eeFs.files[i_fileId2];
  eeFs.files[i_fileId2] = tmp;

  ENABLE_SYNC_WRITE(true);
  EeFsFlushDirEnt(i_fileId1);
  EeFsFlushDirEnt(i_fileId2);
  ENABLE_SYNC_WRITE(false);
}

void EFile::rm(uint8_t i_fileId)
{
  blkid_t i = eeFs.files[i_fileId].startBlk;
  memclear(&eeFs.files[i_fileId], sizeof(eeFs.files[i_fileId]));
  ENABLE_SYNC_WRITE(true);
  EeFsFlushDirEnt(i_fileId);
  if (i) EeFsFree(i); //chain in
  ENABLE_SYNC_WRITE(false);
}

/*
 * Open file i_fileId for reading.
 * Return the file's type
 */
void EFile::openRd(uint8_t i_fileId)
{
  m_fileId = i_fileId;
  m_pos      = 0;
  m_currBlk  = eeFs.files[m_fileId].startBlk;
  m_ofs      = 0;
  s_write_err = ERR_NONE;       // error reasons */
}

void RlcFile::openRlc(uint8_t i_fileId)
{
  EFile::openRd(i_fileId);
  m_zeroes   = 0;
  m_bRlc     = 0;
}

uint8_t EFile::read(uint8_t *buf, uint8_t i_len)
{
  uint16_t len = eeFs.files[m_fileId].size - m_pos;
  if (i_len > len) i_len = len;

  uint8_t remaining = i_len;
  while (remaining) {
    if (!m_currBlk) break;

    *buf++ = EeFsGetDat(m_currBlk, m_ofs++);
    if (m_ofs >= BS-sizeof(blkid_t)) {
      m_ofs = 0;
      m_currBlk = EeFsGetLink(m_currBlk);
    }
    remaining--;
  }

  i_len -= remaining;
  m_pos += i_len;
  return i_len;
}

/*
 * Read runlength (RLE) compressed bytes into buf.
 */
uint16_t RlcFile::readRlc(uint8_t *buf, uint16_t i_len)
{
  uint16_t i = 0;
  for( ; 1; ) {
    uint8_t ln = min<uint16_t>(m_zeroes, i_len-i);
    memclear(&buf[i], ln);
    i        += ln;
    m_zeroes -= ln;
    if (m_zeroes) break;

    ln = min<uint16_t>(m_bRlc, i_len-i);
    uint8_t lr = read(&buf[i], ln);
    i        += lr ;
    m_bRlc   -= lr;
    if(m_bRlc) break;

    if (read(&m_bRlc, 1) !=1) break; // read how many bytes to read

    assert(m_bRlc & 0x7f);

    if (m_bRlc&0x80) { // if contains high byte
      m_zeroes  =(m_bRlc>>4) & 0x7;
      m_bRlc    = m_bRlc & 0x0f;
    } else if(m_bRlc&0x40) {
      m_zeroes  = m_bRlc & 0x3f;
      m_bRlc    = 0;
    }
  }
  return i;
}

void RlcFile::write1(uint8_t b)
{
  m_write1_byte = b;
  write(&m_write1_byte, 1);
}

void RlcFile::write(uint8_t *buf, uint8_t i_len)
{
  m_write_len = i_len;
  m_write_buf = buf;

  do {
    nextWriteStep();
  } while (IS_SYNC_WRITE_ENABLE() && m_write_len && !s_write_err);
}

void RlcFile::nextWriteStep()
{
  if (!m_currBlk && m_pos==0) {
    eeFs.files[FILE_TMP].startBlk = m_currBlk = eeFs.freeList;
    if (m_currBlk) {
#if defined(EXTERNALEEPROM)
      freeBlocks--;
#endif
      eeFs.freeList = EeFsGetLink(m_currBlk);
      m_write_step |= WRITE_FIRST_LINK;
      EeFsFlushFreelist();
      return;
    }
  }

  if ((m_write_step & 0x0f) == WRITE_FIRST_LINK) {
    m_write_step -= WRITE_FIRST_LINK;
    EeFsSetLink(m_currBlk, 0);
    return;
  }

  while (m_write_len) {
    if (!m_currBlk) {
      s_write_err = ERR_FULL;
      break;
    }
    if (m_ofs >= (BS-sizeof(blkid_t))) {
      m_ofs = 0;
      blkid_t nextBlk = EeFsGetLink(m_currBlk);
      if (!nextBlk) {
        if (!eeFs.freeList) {
          s_write_err = ERR_FULL;
          break;
        }
        m_write_step += WRITE_NEXT_LINK_1;
        EeFsSetLink(m_currBlk, eeFs.freeList);
        // TODO not good
        return;
      }
      m_currBlk = nextBlk;
    }
    switch (m_write_step & 0x0f) {
    case WRITE_NEXT_LINK_1:
      m_currBlk = eeFs.freeList;
#if defined(EXTERNALEEPROM)
      freeBlocks--;
#endif
      eeFs.freeList = EeFsGetLink(eeFs.freeList);
      m_write_step += 1;
      EeFsFlushFreelist();
      return;
    case WRITE_NEXT_LINK_2:
      m_write_step -= WRITE_NEXT_LINK_2;
      EeFsSetLink(m_currBlk, 0);
      return;
    }
    uint8_t tmp = BS-sizeof(blkid_t)-m_ofs;
    if(tmp>m_write_len) tmp = m_write_len;
    m_write_buf += tmp;
    m_write_len -= tmp;
    m_ofs += tmp;
    m_pos += tmp;
    EeFsSetDat(m_currBlk, m_ofs-tmp, m_write_buf-tmp, tmp);
    return;
  }

  if (s_write_err == ERR_FULL) {
    POPUP_WARNING(STR_EEPROMOVERFLOW);
    m_write_step = 0;
    m_write_len = 0;
    m_cur_rlc_len = 0;
  } else if (!IS_SYNC_WRITE_ENABLE()) {
    nextRlcWriteStep();
  }
}

void RlcFile::create(uint8_t i_fileId, uint8_t typ, uint8_t sync_write)
{
  // all write operations will be executed on FILE_TMP
  openRlc(FILE_TMP); // internal use
  eeFs.files[FILE_TMP].typ      = typ;
  eeFs.files[FILE_TMP].size     = 0;
  m_fileId = i_fileId;
  ENABLE_SYNC_WRITE(sync_write);
}

/*
 * Copy file src to dst
 */
bool RlcFile::copy(uint8_t i_fileDst, uint8_t i_fileSrc)
{
  EFile theFile2;
  theFile2.openRd(i_fileSrc);

  create(i_fileDst, FILE_TYP_MODEL/*optimization, only model files are copied. should be eeFs.files[i_fileSrc].typ*/, true);

  uint8_t buf[BS-sizeof(blkid_t)];
  uint8_t len;
  while ((len=theFile2.read(buf, sizeof(buf)))) {
    write(buf, len);
    if (write_errno() != 0) {
      ENABLE_SYNC_WRITE(false);
      return false;
    }
  }

  blkid_t fri=0;
  if (m_currBlk && (fri=EeFsGetLink(m_currBlk)))
    EeFsSetLink(m_currBlk, 0);

  if (fri) EeFsFree(fri);  //chain in

  eeFs.files[FILE_TMP].size = m_pos;
  EFile::swap(m_fileId, FILE_TMP);

  assert(!m_write_step);

  // s_sync_write is set to false in swap();
  return true;
}

#if defined(SDCARD)
const pm_char * eeBackupModel(uint8_t i_fileSrc)
{
  char *buf = reusableBuffer.modelsel.mainname;
  DIR archiveFolder;
  UINT written;

  // we must close the logs as we reuse the same FIL structure
  closeLogs();

  // check and create folder here
  strcpy_P(buf, STR_MODELS_PATH);
  FRESULT result = f_opendir(&archiveFolder, buf);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(buf);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }

  buf[sizeof(MODELS_PATH)-1] = '/';
  eeLoadModelName(i_fileSrc, &buf[sizeof(MODELS_PATH)]);
  buf[sizeof(MODELS_PATH)+sizeof(g_model.name)] = '\0';

  uint8_t i = sizeof(MODELS_PATH)+sizeof(g_model.name)-1;
  uint8_t len = 0;
  while (i>sizeof(MODELS_PATH)-1) {
    if (!len && buf[i])
      len = i+1;
    if (len) {
      if (buf[i])
        buf[i] = idx2char(buf[i]);
      else
        buf[i] = '_';
    }
    i--;
  }

  if (len == 0) {
    uint8_t num = i_fileSrc + 1;
    strcpy_P(&buf[sizeof(MODELS_PATH)], STR_MODEL);
    buf[sizeof(MODELS_PATH) + PSIZE(TR_MODEL)] = (char)((num / 10) + '0');
    buf[sizeof(MODELS_PATH) + PSIZE(TR_MODEL) + 1] = (char)((num % 10) + '0');
    len = sizeof(MODELS_PATH) + PSIZE(TR_MODEL) + 2;
  }


  strcpy_P(&buf[len], STR_MODELS_EXT);

  TRACE("SD-card backup filename=%s", buf);

  result = f_open(&g_oLogFile, buf, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  EFile theFile2;
  theFile2.openRd(FILE_MODEL(i_fileSrc));

  *(uint32_t*)&buf[0] = O9X_FOURCC;
  buf[4] = g_eeGeneral.version;
  buf[5] = 'M';
  *(uint16_t*)&buf[6] = eeModelSize(i_fileSrc);

  result = f_write(&g_oLogFile, buf, 8, &written);
  if (result != FR_OK || written != 8) {
    f_close(&g_oLogFile);
    return SDCARD_ERROR(result);
  }

  while ((len=theFile2.read((uint8_t *)buf, 15))) {
    result = f_write(&g_oLogFile, (uint8_t *)buf, len, &written);
    if (result != FR_OK || written != len) {
      f_close(&g_oLogFile);
      return SDCARD_ERROR(result);
    }
  }

  f_close(&g_oLogFile);
  return NULL;
}

const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name)
{
  char *buf = reusableBuffer.modelsel.mainname;
  UINT read;

  // we must close the logs as we reuse the same FIL structure
  closeLogs();

  strcpy_P(buf, STR_MODELS_PATH);
  buf[sizeof(MODELS_PATH)-1] = '/';
  strcpy(&buf[sizeof(MODELS_PATH)], model_name);
  strcpy_P(&buf[strlen(buf)], STR_MODELS_EXT);

  FRESULT result = f_open(&g_oLogFile, buf, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (f_size(&g_oLogFile) < 8) {
    f_close(&g_oLogFile);
    return STR_INCOMPATIBLE;
  }

  result = f_read(&g_oLogFile, (uint8_t *)buf, 8, &read);
  if (result != FR_OK || read != 8) {
    f_close(&g_oLogFile);
    return SDCARD_ERROR(result);
  }

  uint8_t version = (uint8_t)buf[4];
  if (*(uint32_t*)&buf[0] != O9X_FOURCC || version != EEPROM_VER || buf[5] != 'M') {
    f_close(&g_oLogFile);
    return STR_INCOMPATIBLE;
  }

  if (eeModelExists(i_fileDst)) {
    eeDeleteModel(i_fileDst);
  }

  theFile.create(FILE_MODEL(i_fileDst), FILE_TYP_MODEL, true);

  do {
    result = f_read(&g_oLogFile, (uint8_t *)buf, 15, &read);
    if (result != FR_OK) {
      ENABLE_SYNC_WRITE(false);
      f_close(&g_oLogFile);
      return SDCARD_ERROR(result);
    }
    if (read > 0) {
      theFile.write((uint8_t *)buf, read);
      if (write_errno() != 0) {
        ENABLE_SYNC_WRITE(false);
        f_close(&g_oLogFile);
        return STR_EEPROMOVERFLOW;
      }
    }
  } while (read == 15);

  blkid_t fri=0;
  if (theFile.m_currBlk && (fri=EeFsGetLink(theFile.m_currBlk)))
    EeFsSetLink(theFile.m_currBlk, 0);

  if (fri) EeFsFree(fri);  //chain in

  eeFs.files[FILE_TMP].size = theFile.m_pos;
  EFile::swap(theFile.m_fileId, FILE_TMP); // s_sync_write is set to false in swap();

  f_close(&g_oLogFile);

  return NULL;
}
#endif

void RlcFile::writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t *buf, uint16_t i_len, uint8_t sync_write)
{
  create(i_fileId, typ, sync_write);

  m_write_step = WRITE_START_STEP;
  m_rlc_buf = buf;
  m_rlc_len = i_len;
  m_cur_rlc_len = 0;
#if defined (EEPROM_PROGRESS_BAR)
  m_ratio = (typ == FILE_TYP_MODEL ? 100 : 10);
#endif

  do {
    nextRlcWriteStep();
  } while (IS_SYNC_WRITE_ENABLE() && m_write_step && !s_write_err);
}

void RlcFile::nextRlcWriteStep()
{
  uint8_t cnt    = 1;
  uint8_t cnt0   = 0;
  uint16_t i = 0;

  if (m_cur_rlc_len) {
    uint8_t tmp1 = m_cur_rlc_len;
    uint8_t *tmp2 = m_rlc_buf;
    m_rlc_buf += m_cur_rlc_len;
    m_cur_rlc_len = 0;
    write(tmp2, tmp1);
    return;
  }

  bool run0 = (m_rlc_buf[0] == 0);

  if (m_rlc_len==0) goto close;

  for (i=1; 1; i++) { // !! laeuft ein byte zu weit !!
    bool cur0 = m_rlc_buf[i] == 0;
    if (cur0 != run0 || cnt==0x3f || (cnt0 && cnt==0x0f) || i==m_rlc_len) {
      if (run0) {
        assert(cnt0==0);
        if (cnt<8 && i!=m_rlc_len)
          cnt0 = cnt; //aufbew fuer spaeter
        else {
          m_rlc_buf+=cnt;
          m_rlc_len-=cnt;
          write1(cnt|0x40);
          return;
        }
      } else {
        m_rlc_buf+=cnt0;
        m_rlc_len-=cnt0+cnt;
        m_cur_rlc_len=cnt;
        if(cnt0) {
          write1(0x80 | (cnt0<<4) | cnt);
        } else {
          write1(cnt);
        }
        return;
      }
      cnt=0;
      if (i==m_rlc_len) break;
      run0 = cur0;
    }
    cnt++;
  }

close:

  switch(m_write_step) {
  case WRITE_START_STEP: {
    blkid_t fri = 0;

    if (m_currBlk && (fri = EeFsGetLink(m_currBlk))) {
      // TODO reuse EeFsFree!!!
      blkid_t prev_freeList = eeFs.freeList;
      eeFs.freeList = fri;
#if defined(EXTERNALEEPROM)
      freeBlocks++;
#endif
      while (EeFsGetLink(fri)) {
        fri = EeFsGetLink(fri);
#if defined(EXTERNALEEPROM)
        freeBlocks++;
#endif
      }
      m_write_step = WRITE_FREE_UNUSED_BLOCKS_STEP1;
      EeFsSetLink(fri, prev_freeList);
      return;
    }
  }

  case WRITE_FINAL_DIRENT_STEP: {
    m_currBlk = eeFs.files[FILE_TMP].startBlk;
    DirEnt & f = eeFs.files[m_fileId];
    eeFs.files[FILE_TMP].startBlk = f.startBlk;
    eeFs.files[FILE_TMP].size = f.size;
    f.startBlk = m_currBlk;
    f.size = m_pos;
    f.typ = eeFs.files[FILE_TMP].typ;
    m_write_step = WRITE_TMP_DIRENT_STEP;
    EeFsFlushDirEnt(m_fileId);
    return;
  }

  case WRITE_TMP_DIRENT_STEP:
    m_write_step = 0;
    EeFsFlushDirEnt(FILE_TMP);
    return;

  case WRITE_FREE_UNUSED_BLOCKS_STEP1:
    m_write_step = WRITE_FREE_UNUSED_BLOCKS_STEP2;
    EeFsSetLink(m_currBlk, 0);
    return;

  case WRITE_FREE_UNUSED_BLOCKS_STEP2:
    m_write_step = WRITE_FINAL_DIRENT_STEP;
    EeFsFlushFreelist();
    return;
  }
}

void RlcFile::flush()
{
  while (eeprom_buffer_size) MYWDT_RESET(EE_READY_vect()); //Simulate ISR in Simu mode

  ENABLE_SYNC_WRITE(true);

  while (m_write_len && !s_write_err)
    nextWriteStep();

  while (isWriting() && !s_write_err)
    nextRlcWriteStep();

  ENABLE_SYNC_WRITE(false);
}

#if defined (EEPROM_PROGRESS_BAR)
void RlcFile::DisplayProgressBar(uint8_t x)
{
  if (s_eeDirtyMsk || isWriting() || eeprom_buffer_size) {
    uint8_t len = s_eeDirtyMsk ? 1 : limit<uint8_t>(1, (7 - (m_rlc_len/m_ratio)), 7);
    lcdDrawFilledRect(x+1, 0, 5, FH, SOLID, ERASE);
    lcdDrawFilledRect(x+2, 7-len, 3, len);
  }
}
#endif

// For conversions ...

bool eeLoadGeneral()
{
  theFile.openRlc(FILE_GENERAL);
  if (theFile.readRlc((uint8_t*)&g_eeGeneral, 1) == 1 && g_eeGeneral.version == EEPROM_VER) {
    theFile.openRlc(FILE_GENERAL);
    if (theFile.readRlc((uint8_t*)&g_eeGeneral, sizeof(g_eeGeneral)) <= sizeof(EEGeneral)) {
      return true;
    }
  }

  TRACE("EEPROM version %d instead of %d", g_eeGeneral.version, EEPROM_VER);
  return false;
}

void eeLoadModelName(uint8_t id, char *name)
{
  memclear(name, sizeof(g_model.name));
  if (id < MAX_MODELS) {
    theFile.openRlc(FILE_MODEL(id));
    theFile.readRlc((uint8_t*)name, sizeof(g_model.name));
  }
}

bool eeModelExists(uint8_t id)
{
  return EFile::exists(FILE_MODEL(id));
}

void eeLoadModel(uint8_t id)
{
  if (id<MAX_MODELS) {


#if defined(SDCARD)
    closeLogs();
#endif

    if (pulsesStarted()) {
      sendStopPulses();
    }

    theFile.openRlc(FILE_MODEL(id));
    uint16_t sz = theFile.readRlc((uint8_t*)&g_model, sizeof(g_model));

    bool newModel = false;

    if (sz < 256) {
      modelDefault(id);
      eeCheck(true);
      newModel = true;
    }

    AUDIO_FLUSH();
    flightReset();
    logicalSwitchesReset();
    setGazSource();

    if (pulsesStarted()) {
#if defined(GUI)
      if (!newModel) {
        checkAll();
      }
#endif
    startPulses(PROTOCMD_INIT);
    }

    customFunctionsReset();

    restoreTimers();

    // TODO pulses should be started after mixer calculations ...

#if defined(FRSKY)
    frskySendAlarms();
#endif

  }
}

void eeErase(bool warn)
{
  generalDefault();

  if (warn) {
    ALERT(STR_EEPROMWARN, STR_BADEEPROMDATA, AU_BAD_EEPROM);
  }

  MESSAGE(STR_EEPROMWARN, STR_EEPROMFORMATTING, NULL, AU_EEPROM_FORMATTING);
  eepromFormat();
  theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), true);
  modelDefault(0);
  theFile.writeRlc(FILE_MODEL(0), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), true);
}

void eeCheck(bool immediately)
{
  if (immediately) {
    eeFlush();
  }

  if (s_eeDirtyMsk & EE_GENERAL) {
    TRACE("eeprom write general");
    s_eeDirtyMsk -= EE_GENERAL;
    theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), immediately);
    if (!immediately) return;
  }

  if (s_eeDirtyMsk & EE_MODEL) {
    TRACE("eeprom write model");
    s_eeDirtyMsk = 0;
    theFile.writeRlc(FILE_MODEL(g_eeGeneral.currModel), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), immediately);
  }
}

# 1 "timers.cpp"
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


#include "OpenAVRc.h"
#include "timers.h"

#if TIMERS > MAX_TIMERS
#error "Timers cannot exceed " .. MAX_TIMERS
#endif

TimerState timersStates[TIMERS] = { { 0 } };

void timerReset(uint8_t idx)
{
  timersStates[idx].state = TMR_OFF; // is changed to RUNNING dep from mode
  timersStates[idx].val = g_model.timers[idx].start;
  timersStates[idx].val_10ms = 0 ;
}


void restoreTimers()
{
  for (uint8_t i=0; i<TIMERS; i++) {
    if (g_model.timers[i].persistent) {
      timersStates[i].val = g_model.timers[i].value;
    }
  }
}

void saveTimers()
{
  for (uint8_t i=0; i<TIMERS; i++) {
    if (g_model.timers[i].persistent) {
      TimerState *timerState = &timersStates[i];
      if (g_model.timers[i].value != (uint16_t)timerState->val) {
        g_model.timers[i].value = timerState->val;
        eeDirty(EE_MODEL);
      }
    }
  }

}

#define THR_TRG_TRESHOLD    13      // approximately 10% full throttle

void evalTimers(int16_t throttle, uint8_t tick10ms)
{
  for (uint8_t i=0; i<TIMERS; i++) {
    int8_t timerMode = g_model.timers[i].mode;
    tmrstart_t timerStart = g_model.timers[i].start;
    TimerState * timerState = &timersStates[i];

    if (timerMode) {
      if ((timerState->state == TMR_OFF) && (timerMode != TMRMODE_THR_TRG)) {
        timerState->state = TMR_RUNNING;
        timerState->cnt = 0;
        timerState->sum = 0;
      }

      if (timerMode == TMRMODE_THR_REL) {
        timerState->cnt++;
        timerState->sum += throttle;
      }

      if ((timerState->val_10ms += tick10ms) >= 100) {
        if (timerState->val == TIMER_MAX) break;
        if (timerState->val == TIMER_MIN) break;

        timerState->val_10ms -= 100 ;
        tmrval_t newTimerVal = timerState->val;
        if (timerStart) newTimerVal = timerStart - newTimerVal;

        if (timerMode == TMRMODE_ABS) {
          newTimerVal++;
        } else if (timerMode == TMRMODE_THR) {
          if (throttle) newTimerVal++;
        } else if (timerMode == TMRMODE_THR_REL) {
          // @@@ open.20.fsguruh: why so complicated? we have already a s_sum field; use it for the half seconds (not showable) as well
          // check for s_cnt[i]==0 is not needed because we are shure it is at least 1
          if ((timerState->sum/timerState->cnt) >= 128) {  // throttle was normalized to 0 to 128 value (throttle/64*2 (because - range is added as well)
            newTimerVal++;  // add second used of throttle
            timerState->sum -= 128*timerState->cnt;
          }
          timerState->cnt = 0;
        } else if (timerMode == TMRMODE_THR_TRG) {
          // we can't rely on (throttle || newTimerVal > 0) as a detection if timer should be running
          // because having persistent timer brakes this rule
          if ((throttle > THR_TRG_TRESHOLD) && timerState->state == TMR_OFF) {
            timerState->state = TMR_RUNNING;  // start timer running
            timerState->cnt = 0;
            timerState->sum = 0;
            // TRACE("Timer[%d] THr triggered", i);
          }
          if (timerState->state != TMR_OFF) newTimerVal++;
        } else {
          if (timerMode > 0) timerMode -= (TMRMODE_COUNT-1);
          if (getSwitch(timerMode)) {
            newTimerVal++;
          }
        }

        switch (timerState->state) {
        case TMR_RUNNING:
          if (timerStart && newTimerVal>=(tmrval_t)timerStart) {
            AUDIO_TIMER_00(g_model.timers[i].countdownBeep);
            timerState->state = TMR_NEGATIVE;
            // TRACE("Timer[%d] negative", i);
          }
          break;
        case TMR_NEGATIVE:
          if (newTimerVal >= (tmrval_t)timerStart + MAX_ALERT_TIME) {
            timerState->state = TMR_STOPPED;
            // TRACE("Timer[%d] stopped state at %d", i, newTimerVal);
          }
          break;
        }

        if (timerStart) newTimerVal = timerStart - newTimerVal; // if counting backwards - display backwards

        if (newTimerVal != timerState->val) {
          timerState->val = newTimerVal;
          if (timerState->state == TMR_RUNNING) {
            if (g_model.timers[i].countdownBeep && g_model.timers[i].start) {
              if (newTimerVal == 30) {
                AUDIO_TIMER_30();
                // TRACE("Timer[%d] 30s announcement", i);
              }
              if (newTimerVal == 20) {
                AUDIO_TIMER_20();
                // TRACE("Timer[%d] 20s announcement", i);
              }
              if (newTimerVal <= 10) {
                AUDIO_TIMER_LT10(g_model.timers[i].countdownBeep, newTimerVal);
                // TRACE("Timer[%d] %ds announcement", i, newTimerVal);
              }
            }
            if (g_model.timers[i].minuteBeep && (newTimerVal % 60)==0) {
              AUDIO_TIMER_MINUTE(newTimerVal);
              // TRACE("Timer[%d] %d minute announcement", i, newTimerVal/60);
            }
          }
        }
      }
    }
  }
}
# 1 "trainer_input.cpp"
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


#include "trainer_input.h"

int16_t ppmInput[NUM_TRAINER];
uint8_t ppmInputValidityTimer;
/*
 * Trainer PPM input capture ISR.
 * Timer 1 is free running as it serves other purposes.
 * 1 count is 0.5us
*/
ISR(TIMER1_CAPT_vect) // G: High frequency noise can cause stack overflow with ISR_NOBLOCK
{
  uint16_t capture = ICR1;

  static uint16_t lastCapt = 0;
  static uint8_t channelNumber = 0;

  uint16_t val = (uint16_t) (capture - lastCapt) / 2; // Convert from timer counts to usec.
  lastCapt = capture;

  if(channelNumber && val > 800 && val < 2200 && channelNumber <= NUM_TRAINER)
  { // Accepted range is 800 to 2200 us  1500us+/-700.
    ppmInputValidityTimer = PPM_IN_VALID_TIMEOUT;
    ppmInput[channelNumber++ -1] =
      (int16_t)(val - 1500)*(g_eeGeneral.PPM_Multiplier+10)/10;
  }
  else if(val > 4000 && val < 19000)
  { // Frame sync pulse >4 <19 ms.
    channelNumber = 1; // Indicates start of new frame.
  }
  else channelNumber = 0; /* Glitches (<800us) or long channel pulses (2200 to 4000us) or
  pulses > 19000us reset the process */
}


#if 0
ISR(TIMER1_CAPT_vect)
{
uint16_t icr1_diff;
uint16_t icr1_current;

static uint16_t icr1_previous = 0;
static uint8_t servo_count = 0;
static uint8_t need_to_sync = 1;

icr1_current = ICR1;

//icr1_diff = icr1_current - icr1_previous;
if(icr1_current >= icr1_previous) icr1_diff = icr1_current - icr1_previous;
else icr1_diff = (0xffff - icr1_previous) + icr1_current + 1 ;

icr1_previous = icr1_current;

	if (icr1_diff > MICRO_SEC_CONVERT(2300)) // > 2.3ms pulse seen as frame sync.
	{
	need_to_sync =0;
	g_sync_count ++;
	servo_count =0;
   	}
	else if (icr1_diff < MICRO_SEC_CONVERT(700)) // < 0.7ms pulse seen as glitch.
	{
	// Do nothing with glitch.
	}
	else if (! need_to_sync) // Pulse within limits and we don't need to sync.
	{
		if (servo_count < NUM_OUT_CHANNELS)
		{
		if (icr1_diff > MICRO_SEC_CONVERT(1500 + DELTA_PPM_IN)) icr1_diff = MICRO_SEC_CONVERT(1500 + DELTA_PPM_IN);
   		else if (icr1_diff < MICRO_SEC_CONVERT(1500 - DELTA_PPM_IN)) icr1_diff = MICRO_SEC_CONVERT(1500 - DELTA_PPM_IN);

		// Subtract 1.5 ms centre offset.
		// Multiply by 2 to get max-min counter value difference to be +-1520
		// (same scaling as M-Link Packet for MPX ppm (+-550us) range !).

   		Channels[servo_count] = (icr1_diff - MICRO_SEC_CONVERT(1500)) * 2;
   		servo_count++;
		}
		else need_to_sync = 1; // More servo pulses than we can handle ... need to sync.
	}

}
#endif

# 1 "telemetry/frsky.cpp"
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


#include "../OpenAVRc.h"

uint8_t frskyStreaming = 0;

TelemetryData telemetryData;

void telemetryInit()
{
//9600 8N1 - "D" 57600 8N1 -> "S.port"
  if IS_USR_PROTO_SMART_PORT() {
  Usart0Set57600BAUDS();
  } else {
  Usart0Set9600BAUDS();
  }
  Usart0Set8N1();
  Usart0EnableTx(); // enable FrSky-Telemetry emission
  Usart0EnableRx(); // enable FrSky-Telemetry reception}

  Usart0TxBufferCount = 0; // TODO not driver code
}

void telemetryReset()
{
  Usart0DisableTx();
  Usart0DisableRx();
  telemetryResetValue();
}

void telemetryResetValue()
{
  memclear(&telemetryData, sizeof(telemetryData));

  frskyStreaming = 0; // reset counter only if valid frsky packets are being detected

  telemetryData.value.gpsLatitude_bp = 2;
  telemetryData.value.gpsLongitude_bp = 2;
  telemetryData.value.gpsFix = -1;

  }

NOINLINE void processSerialData(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

  switch (dataState) {
  case STATE_DATA_START:
    if (data == START_STOP) {
      if (IS_USR_PROTO_SMART_PORT()) {
        dataState = STATE_DATA_IN_FRAME ;
        Usart0RxBufferCount = 0;
      }
    } else {
      if (Usart0RxBufferCount < USART0_RX_PACKET_SIZE) {
        Usart0RxBuffer[Usart0RxBufferCount++] = data;
      }
      dataState = STATE_DATA_IN_FRAME;
    }
    break;

  case STATE_DATA_IN_FRAME:
    if (data == BYTESTUFF) {
      dataState = STATE_DATA_XOR; // XOR next byte
    } else if (data == START_STOP) {
      if (IS_USR_PROTO_SMART_PORT()) {
        dataState = STATE_DATA_IN_FRAME ;
        Usart0RxBufferCount = 0;
      } else {
        // end of frame detected
        frskyDProcessPacket(Usart0RxBuffer);
        dataState = STATE_DATA_IDLE;
      }
      break;
    } else if (Usart0RxBufferCount < USART0_RX_PACKET_SIZE) {
      Usart0RxBuffer[Usart0RxBufferCount++] = data;
    }
    break;

  case STATE_DATA_XOR:
    if (Usart0RxBufferCount < USART0_RX_PACKET_SIZE) {
      Usart0RxBuffer[Usart0RxBufferCount++] = data ^ STUFF_MASK;
    }
    dataState = STATE_DATA_IN_FRAME;
    break;

  case STATE_DATA_IDLE:
    if (data == START_STOP) {
      Usart0RxBufferCount = 0;
      dataState = STATE_DATA_START;
    }
    break;

  } // switch

  if (IS_USR_PROTO_SMART_PORT() && Usart0RxBufferCount >= FRSKY_SPORT_PACKET_SIZE) {
    processSportPacket(Usart0RxBuffer);
    dataState = STATE_DATA_IDLE;
  }
}

bool checkSportPacket(uint8_t *packet)
{
  uint16_t crc = 0;
  for (uint8_t i=1; i<USART0_RX_PACKET_SIZE; i++) {
    crc += packet[i]; //0-1FF
    crc += crc >> 8; //0-100
    crc &= 0x00ff;
    crc += crc >> 8; //0-0FF
    crc &= 0x00ff;
  }
  return (crc == 0x00ff);
}

void setBaroAltitude(int32_t baroAltitude) //S.port function
{
  // First received barometer altitude => Altitude offset
  if (!telemetryData.value.baroAltitudeOffset)
    telemetryData.value.baroAltitudeOffset = -baroAltitude;

  baroAltitude += telemetryData.value.baroAltitudeOffset;
  telemetryData.value.baroAltitude_bp = baroAltitude;

  baroAltitude /= 100;
  if (baroAltitude > telemetryData.value.maxAltitude)
    telemetryData.value.maxAltitude = baroAltitude;
  if (baroAltitude < telemetryData.value.minAltitude)
    telemetryData.value.minAltitude = baroAltitude;
}

void processSportPacket(uint8_t *packet)
{
  /* uint8_t  dataId = packet[0]; */
  uint8_t  prim   = packet[1];
  uint16_t appId  = *((uint16_t *)(packet+2));

  if (!checkSportPacket(packet))
    return;

  frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected

  switch (prim) {
  case DATA_FRAME:

    if (appId == RSSI_ID) {
      telemetryData.rssi[0].set(SPORT_DATA_U8(packet));
    }
    if (appId == SWR_ID) {
      telemetryData.rssi[1].set(SPORT_DATA_U8(packet));
    } else if (appId == ADC1_ID || appId == ADC2_ID) {
      // A1/A2 of DxR receivers
      telemetryData.analog[appId-ADC1_ID].set(SPORT_DATA_U8(packet),g_model.telemetry.channels[appId-ADC1_ID].type);
#if defined(VARIO)
      uint8_t varioSource = g_model.telemetry.varioSource - VARIO_SOURCE_A1;
      if (varioSource == appId-ADC1_ID) {
        telemetryData.value.varioSpeed = applyChannelRatio(varioSource, telemetryData.analog[varioSource].value);
      }
#endif
    } else if (appId == BATT_ID) {
      telemetryData.analog[0].set(SPORT_DATA_U8(packet),UNIT_VOLTS);
    } else if ((appId >> 8) == 0) {
      // The old FrSky IDs
      uint16_t value = HUB_DATA_U16(packet);
      processHubPacket((uint8_t)appId, value);
    } else if (appId == BETA_BARO_ALT_ID) {
      int32_t baroAltitude = SPORT_DATA_S32(packet);
      setBaroAltitude(10 * (baroAltitude >> 8));
    } else if (appId == BETA_VARIO_ID) {
      int32_t varioSpeed = SPORT_DATA_S32(packet);
      telemetryData.value.varioSpeed = 10 * (varioSpeed >> 8);
    } else if IS_IN_RANGE(appId, T1_FIRST_ID, T1_LAST_ID) {
      telemetryData.value.temperature1 = SPORT_DATA_S32(packet);
      if (telemetryData.value.temperature1 > telemetryData.value.maxTemperature1)
        telemetryData.value.maxTemperature1 = telemetryData.value.temperature1;
    } else if IS_IN_RANGE(appId, T2_FIRST_ID, T2_LAST_ID) {
      telemetryData.value.temperature2 = SPORT_DATA_S32(packet);
      if (telemetryData.value.temperature2 > telemetryData.value.maxTemperature2)
        telemetryData.value.maxTemperature2 = telemetryData.value.temperature2;
    } else if IS_IN_RANGE(appId, RPM_FIRST_ID, RPM_LAST_ID) {
      telemetryData.value.rpm = SPORT_DATA_U32(packet) / (g_model.telemetry.blades+2);
      if (telemetryData.value.rpm > telemetryData.value.maxRpm)
        telemetryData.value.maxRpm = telemetryData.value.rpm;
    } else if IS_IN_RANGE(appId, FUEL_FIRST_ID, FUEL_LAST_ID) {
      telemetryData.value.fuelLevel = SPORT_DATA_U32(packet);
    } else if IS_IN_RANGE(appId, ALT_FIRST_ID, ALT_LAST_ID) {
      setBaroAltitude(SPORT_DATA_S32(packet));
    } else if IS_IN_RANGE(appId, VARIO_FIRST_ID, VARIO_LAST_ID) {
      telemetryData.value.varioSpeed = SPORT_DATA_S32(packet);
    } else if IS_IN_RANGE(appId, ACCX_FIRST_ID, ACCX_LAST_ID) {
      telemetryData.value.accelX = SPORT_DATA_S32(packet);
    } else if IS_IN_RANGE(appId, ACCY_FIRST_ID, ACCY_LAST_ID) {
      telemetryData.value.accelY = SPORT_DATA_S32(packet);
    } else if IS_IN_RANGE(appId, ACCZ_FIRST_ID, ACCZ_LAST_ID) {
      telemetryData.value.accelZ = SPORT_DATA_S32(packet);
    } else if IS_IN_RANGE(appId, CURR_FIRST_ID, CURR_LAST_ID) {
      telemetryData.value.current = SPORT_DATA_U32(packet);
      if (telemetryData.value.current > telemetryData.value.maxCurrent)
        telemetryData.value.maxCurrent = telemetryData.value.current;
    } else if IS_IN_RANGE(appId, VFAS_FIRST_ID, VFAS_LAST_ID) {
      telemetryData.value.vfas = SPORT_DATA_U32(packet)/10;   //TODO: remove /10 and display with PREC2 when using SPORT
    } else if (appId >= GPS_SPEED_FIRST_ID && appId <= GPS_SPEED_LAST_ID) {
      telemetryData.value.gpsSpeed_bp = SPORT_DATA_U32(packet);
      telemetryData.value.gpsSpeed_bp = (telemetryData.value.gpsSpeed_bp * 46) / 25 / 1000;
      if (telemetryData.value.gpsSpeed_bp > telemetryData.value.maxGpsSpeed)
        telemetryData.value.maxGpsSpeed = telemetryData.value.gpsSpeed_bp;
    } else if IS_IN_RANGE(appId, GPS_TIME_DATE_FIRST_ID, GPS_TIME_DATE_LAST_ID) {
      uint32_t gps_time_date = SPORT_DATA_U32(packet);
      if (gps_time_date & 0x000000ff) {
        telemetryData.value.year = (uint16_t) ((gps_time_date & 0xff000000) >> 24);
        telemetryData.value.month = (uint8_t) ((gps_time_date & 0x00ff0000) >> 16);
        telemetryData.value.day = (uint8_t) ((gps_time_date & 0x0000ff00) >> 8);
      } else {
        telemetryData.value.hour = (uint8_t) ((gps_time_date & 0xff000000) >> 24);
        telemetryData.value.min = (uint8_t) ((gps_time_date & 0x00ff0000) >> 16);
        telemetryData.value.sec = (uint16_t) ((gps_time_date & 0x0000ff00) >> 8);
        telemetryData.value.hour = ((uint8_t) (telemetryData.value.hour + g_eeGeneral.timezone + 24)) % 24;
      }
    } else if IS_IN_RANGE(appId, GPS_COURS_FIRST_ID, GPS_COURS_LAST_ID) {
      uint32_t course = SPORT_DATA_U32(packet);
      telemetryData.value.gpsCourse_bp = course / 100;
      telemetryData.value.gpsCourse_ap = course % 100;
    } else if IS_IN_RANGE(appId, GPS_ALT_FIRST_ID, GPS_ALT_LAST_ID) {
      uint32_t gpsAlt = SPORT_DATA_S32(packet);
      telemetryData.value.gpsAltitude_bp = gpsAlt / 100;
      telemetryData.value.gpsAltitude_ap = gpsAlt % 100;


      if (!telemetryData.value.gpsAltitudeOffset)
          telemetryData.value.gpsAltitudeOffset = -telemetryData.value.gpsAltitude_bp;

        if (!telemetryData.value.baroAltitudeOffset) {
          int16_t altitude = TELEMETRY_RELATIVE_GPS_ALT_BP;
          if (altitude > telemetryData.value.maxAltitude)
            telemetryData.value.maxAltitude = altitude;
          if (altitude < telemetryData.value.minAltitude)
            telemetryData.value.minAltitude = altitude;
        }

      if (telemetryData.value.gpsFix > 0) {
        if (!telemetryData.value.pilotLatitude && !telemetryData.value.pilotLongitude) {
          // First received GPS position => Pilot GPS position
          getGpsPilotPosition();
        }
          else if (telemetryData.value.gpsDistNeeded || menuHandlers[menuLevel] == menuTelemetryFrsky) {
            getGpsDistance();
          }
      }
    } else if IS_IN_RANGE(appId, GPS_LONG_LATI_FIRST_ID, GPS_LONG_LATI_LAST_ID) {
      uint32_t gps_long_lati_data = SPORT_DATA_U32(packet);
      uint32_t gps_long_lati_b1w, gps_long_lati_a1w;
      gps_long_lati_b1w = (gps_long_lati_data & 0x3fffffff) / 10000;
      gps_long_lati_a1w = (gps_long_lati_data & 0x3fffffff) % 10000;
      switch ((gps_long_lati_data & 0xc0000000) >> 30) {
      case 0:
        telemetryData.value.gpsLatitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        telemetryData.value.gpsLatitude_ap = gps_long_lati_a1w;
        telemetryData.value.gpsLatitudeNS = 'N';
        break;
      case 1:
        telemetryData.value.gpsLatitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        telemetryData.value.gpsLatitude_ap = gps_long_lati_a1w;
        telemetryData.value.gpsLatitudeNS = 'S';
        break;
      case 2:
        telemetryData.value.gpsLongitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        telemetryData.value.gpsLongitude_ap = gps_long_lati_a1w;
        telemetryData.value.gpsLongitudeEW = 'E';
        break;
      case 3:
        telemetryData.value.gpsLongitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
        telemetryData.value.gpsLongitude_ap = gps_long_lati_a1w;
        telemetryData.value.gpsLongitudeEW = 'W';
        break;
      }
      if (telemetryData.value.gpsLongitudeEW && telemetryData.value.gpsLatitudeNS) {
        telemetryData.value.gpsFix = 1;
      } else if (telemetryData.value.gpsFix > 0) {
        telemetryData.value.gpsFix = 0;
      }
    }

    else if IS_IN_RANGE(appId, CELLS_FIRST_ID, CELLS_LAST_ID) {
      uint32_t cells = SPORT_DATA_U32(packet);
      uint8_t battnumber = cells & 0xF;
      uint32_t minCell, minCellNum;

      //TODO: Use reported total voltages (bits 4-7)?
      telemetryData.value.cellVolts[battnumber] = ((cells & 0x000FFF00) >> 8) / 10;
      telemetryData.value.cellVolts[battnumber+1] = ((cells & 0xFFF00000) >> 20) / 10;

      if (telemetryData.value.cellsCount < battnumber+2)
        telemetryData.value.cellsCount = battnumber+2;
      if (telemetryData.value.cellVolts[battnumber+1] == 0)
        --telemetryData.value.cellsCount;

      if ((telemetryData.value.cellVolts[battnumber] < telemetryData.value.cellVolts[battnumber+1]) || (telemetryData.value.cellVolts[battnumber+1] == 0)) {
        minCell = telemetryData.value.cellVolts[battnumber];
        minCellNum = battnumber;
      } else {
        minCell = telemetryData.value.cellVolts[battnumber+1];
        minCellNum = battnumber+1;
      }

      if (!telemetryData.value.minCellVolts || minCell < telemetryData.value.minCellVolts || minCellNum==telemetryData.value.minCellIdx) {
        telemetryData.value.minCellIdx = minCellNum;
        telemetryData.value.minCellVolts = minCell;
      }
    }
    break;

  }
}

void frskyDProcessPacket(uint8_t *packet)
{
  // What type of packet?
  switch (packet[0]) {
  case LINKPKT: { // A1/A2/RSSI values
    telemetryData.analog[TELEM_ANA_A1].set(packet[1], g_model.telemetry.channels[TELEM_ANA_A1].type);
    telemetryData.analog[TELEM_ANA_A2].set(packet[2], g_model.telemetry.channels[TELEM_ANA_A2].type);
    telemetryData.rssi[0].set(packet[3]);
    telemetryData.rssi[1].set(packet[4] / 2);
    frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected

#if defined(VARIO)
    uint8_t varioSource = g_model.telemetry.varioSource - VARIO_SOURCE_A1;
    if (varioSource < 2) {
      telemetryData.value.varioSpeed = applyChannelRatio(varioSource, telemetryData.analog[varioSource].value);
    }
#endif
    break;
  }
    /*case BFSPPKT:
    case RXSPPKT: {
      uint16_t MMSmartPort_id; // = (packet[3] << 8) | packet[2];
      uint32_t MMSmartPort_data;
      MMSmartPort_id = packet[3];
      MMSmartPort_id <<=8;
      MMSmartPort_id |=packet[2];
      MMSmartPort_data = packet[7];
      MMSmartPort_data <<=8;
      MMSmartPort_data |= packet[6];
      MMSmartPort_data <<=8;
      MMSmartPort_data |= packet[5];
      MMSmartPort_data <<=8;
      MMSmartPort_data |= packet[4];
      parseTelemMMsmartData(MMSmartPort_id, MMSmartPort_data, packet[4]);

      frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
      link_counter += 256 / FRSKY_D_AVERAGING;

      break;
    }*/

  case USRPKT: // User Data packet
    uint8_t numBytes = 3 + (packet[1] & 0x07); // sanitize in case of data corruption leading to buffer overflow
    for (uint8_t i=3; i<numBytes; i++) {
      if (IS_USR_PROTO_FRSKY_HUB()) {
        parseTelemHubByte(packet[i]);
      }
#if defined(WS_HOW_HIGH)
      if (IS_USR_PROTO_WS_HOW_HIGH()) {
        parseTelemWSHowHighByte(packet[i]);
      }
#endif
    }
    break;
  }
}

void parseTelemHubByte(uint8_t byte)
{
  static uint8_t structPos;
  static uint8_t lowByte;
  static TS_STATE state = TS_IDLE;

  if (byte == 0x5e) {
    state = TS_DATA_ID;
    return;
  }
  if (state == TS_IDLE) {
    return;
  }
  if (state & TS_XOR) {
    byte = byte ^ 0x60;
    state = (TS_STATE)(state - TS_XOR);
  } else if (byte == 0x5d) {
    state = (TS_STATE)(state | TS_XOR);
    return;
  }
  if (state == TS_DATA_ID) {
    if (byte > 0x3f) {
      state = TS_IDLE;
    } else {
      structPos = byte * 2;
      state = TS_DATA_LOW;
    }
    return;
  }
  if (state == TS_DATA_LOW) {
    lowByte = byte;
    state = TS_DATA_HIGH;
    return;
  }

  state = TS_IDLE;

  processHubPacket(structPos,byte<<8 | lowByte);
}

void processHubPacket(uint8_t id, uint16_t value)
{
#if defined(GPS)
  if (id == offsetof(TelemetrySerialData, gpsLatitude_bp)) {
    if (value)
      telemetryData.value.gpsFix = 1;
    else if (telemetryData.value.gpsFix > 0 && telemetryData.value.gpsLatitude_bp > 1)
      telemetryData.value.gpsFix = 0;
  } else if (id == offsetof(TelemetrySerialData, gpsLongitude_bp)) {
    if (value)
      telemetryData.value.gpsFix = 1;
    else if (telemetryData.value.gpsFix > 0 && telemetryData.value.gpsLongitude_bp > 1)
      telemetryData.value.gpsFix = 0;
  }

  if  (id == offsetof(TelemetrySerialData, gpsAltitude_bp) ||
       (IS_IN_RANGE(id, offsetof(TelemetrySerialData, gpsAltitude_ap), offsetof(TelemetrySerialData, gpsLatitudeNS)) &&
        (id != offsetof(TelemetrySerialData, baroAltitude_bp)) && (id != offsetof(TelemetrySerialData, baroAltitude_ap))))
  {
    // If we don't have a fix, we may discard the value
    if (telemetryData.value.gpsFix <= 0)
      return;
  }
#endif  // #if defined(GPS)

  ((uint8_t*)&telemetryData.value)[id] = (uint8_t)value;
  ((uint8_t*)&telemetryData.value)[id+1] = value>>8;

  switch (id) {

  case offsetof(TelemetrySerialData, rpm):
    telemetryData.value.rpm *= (uint8_t)60/(g_model.telemetry.blades+2);
    if (telemetryData.value.rpm > telemetryData.value.maxRpm)
      telemetryData.value.maxRpm = telemetryData.value.rpm;
    break;

  case offsetof(TelemetrySerialData, temperature1):
    if (telemetryData.value.temperature1 > telemetryData.value.maxTemperature1)
      telemetryData.value.maxTemperature1 = telemetryData.value.temperature1;
    break;

  case offsetof(TelemetrySerialData, temperature2):
    if (telemetryData.value.temperature2 > telemetryData.value.maxTemperature2)
      telemetryData.value.maxTemperature2 = telemetryData.value.temperature2;
    break;

  case offsetof(TelemetrySerialData, current):
    if ((int16_t)telemetryData.value.current > 0 && ((int16_t)telemetryData.value.current + g_model.telemetry.fasOffset) > 0)
      telemetryData.value.current += g_model.telemetry.fasOffset;
    else
      telemetryData.value.current = 0;
    if (telemetryData.value.current > telemetryData.value.maxCurrent)
      telemetryData.value.maxCurrent = telemetryData.value.current;
    break;

  case offsetof(TelemetrySerialData, currentConsumption):
    // we receive data from openXsensor. stops the calculation of consumption and power
    telemetryData.value.openXsensor = 1;
    break;

  case offsetof(TelemetrySerialData, volts_ap):
#if defined(FAS_BSS)
    telemetryData.value.vfas = (telemetryData.value.volts_bp * 10 + telemetryData.value.volts_ap);
#else
    telemetryData.value.vfas = ((telemetryData.value.volts_bp * 100 + telemetryData.value.volts_ap * 10) * 21) / 110;
#endif
    if (!telemetryData.value.minVfas || telemetryData.value.minVfas > telemetryData.value.vfas)
      telemetryData.value.minVfas = telemetryData.value.vfas;
    break;

  case offsetof(TelemetrySerialData, baroAltitude_bp):
    // First received barometer altitude => Altitude offset
    if (!telemetryData.value.baroAltitudeOffset)
      telemetryData.value.baroAltitudeOffset = -telemetryData.value.baroAltitude_bp;
    telemetryData.value.baroAltitude_bp += telemetryData.value.baroAltitudeOffset;
    checkMinMaxAltitude();
    break;

#if defined(GPS)
  case offsetof(TelemetrySerialData, gpsAltitude_ap):
    if (!telemetryData.value.gpsAltitudeOffset) {
      telemetryData.value.gpsAltitudeOffset = -telemetryData.value.gpsAltitude_bp;
    }
    telemetryData.value.gpsAltitude_bp += telemetryData.value.gpsAltitudeOffset;
    if (!telemetryData.value.baroAltitudeOffset) {
      if (telemetryData.value.gpsAltitude_bp > telemetryData.value.maxAltitude)
        telemetryData.value.maxAltitude = telemetryData.value.gpsAltitude_bp;
      if (telemetryData.value.gpsAltitude_bp < telemetryData.value.minAltitude)
        telemetryData.value.minAltitude = telemetryData.value.gpsAltitude_bp;
    }
        if (!telemetryData.value.pilotLatitude && !telemetryData.value.pilotLongitude) {
          // First received GPS position => Pilot GPS position
          getGpsPilotPosition();
        }
          else if (telemetryData.value.gpsDistNeeded || menuHandlers[menuLevel] == menuTelemetryFrsky) {
            getGpsDistance();
          }
              break;

  case offsetof(TelemetrySerialData, gpsSpeed_bp):
    // Speed => Max speed
    if (telemetryData.value.gpsSpeed_bp > telemetryData.value.maxGpsSpeed)
      telemetryData.value.maxGpsSpeed = telemetryData.value.gpsSpeed_bp;
    break;
#endif

  case offsetof(TelemetrySerialData, volts):
    frskyUpdateCells();
    break;

#if defined(GPS)
  case offsetof(TelemetrySerialData, hour):
    telemetryData.value.hour = ((uint8_t)(telemetryData.value.hour + g_eeGeneral.timezone + 24)) % 24;
    break;
#endif

  case offsetof(TelemetrySerialData, accelX):
  case offsetof(TelemetrySerialData, accelY):
  case offsetof(TelemetrySerialData, accelZ):
    *(int16_t*)(&((uint8_t*)&telemetryData.value)[id]) /= 10;
    break;
  }
}

#if defined(WS_HOW_HIGH)
  uint8_t frskyUsrStreaming = 0;

void parseTelemWSHowHighByte(uint8_t byte)
{
  if (frskyUsrStreaming < (WSHH_TIMEOUT10ms - 10)) {
    ((uint8_t*)&telemetryData.value)[offsetof(TelemetrySerialData, baroAltitude_bp)] = byte;
    checkMinMaxAltitude();
  } else {
    // At least 100mS passed since last data received
    ((uint8_t*)&telemetryData.value)[offsetof(TelemetrySerialData, baroAltitude_bp)+1] = byte;
  }
  // baroAltitude_bp unit here is feet!
  frskyUsrStreaming = WSHH_TIMEOUT10ms; // reset counter
}
#endif

void telemetryInterrupt10ms()
{
#if defined(SPIMODULES)
  //frskyRFProcessPacket(Usart0RxBuffer);
#endif

  uint16_t voltage = 0; /* unit: 1/10 volts */
  for (uint8_t i=0; i<telemetryData.value.cellsCount; i++)
    voltage += telemetryData.value.cellVolts[i];
  voltage /= (10 / TELEMETRY_CELL_VOLTAGE_MUTLIPLIER);
  telemetryData.value.cellsSum = voltage;
  if (telemetryData.value.cellsSum < telemetryData.value.minCells) {
    telemetryData.value.minCells = telemetryData.value.cellsSum;
  }

  if (TELEMETRY_STREAMING()) {
    if (!TELEMETRY_OPENXSENSOR()) {
      // power calculation
      uint8_t channel = g_model.telemetry.voltsSource;
      if (channel <= FRSKY_VOLTS_SOURCE_A2) {
        voltage = applyChannelRatio(channel, telemetryData.analog[channel].value) / 10;
      }

      else if (channel == FRSKY_VOLTS_SOURCE_FAS) {
        voltage = telemetryData.value.vfas;
      }

      uint16_t current = telemetryData.value.current; /* unit: 1/10 amps */

      channel = g_model.telemetry.currentSource - FRSKY_CURRENT_SOURCE_A1;
      if (channel < MAX_FRSKY_A_CHANNELS) {
        current = applyChannelRatio(channel, telemetryData.analog[channel].value) / 10;
      }

      telemetryData.value.power = ((current>>1) * (voltage>>1)) / 25;

      telemetryData.value.currentPrescale += current;
      if (telemetryData.value.currentPrescale >= 3600) {
        telemetryData.value.currentConsumption += 1;
        telemetryData.value.currentPrescale -= 3600;
      }
    }

    if (telemetryData.value.power > telemetryData.value.maxPower) {
      telemetryData.value.maxPower = telemetryData.value.power;
    }
  }

#if defined(WS_HOW_HIGH)
  if (frskyUsrStreaming) {
    --frskyUsrStreaming;
  }
#endif

  if (frskyStreaming) {
    --frskyStreaming;
  } else {
    telemetryData.rssi[0].set(0);
    telemetryData.rssi[1].set(0);
  }
}

void frskyUpdateCells()
{
  // Voltage => Cell number + Cell voltage
  uint8_t battnumber = ((telemetryData.value.volts & 0x00F0) >> 4);
  if (battnumber < 12) {
    if (telemetryData.value.cellsCount < battnumber+1) {
      telemetryData.value.cellsCount = battnumber+1;
    }
    uint8_t cellVolts = (uint8_t)(((((telemetryData.value.volts & 0xFF00) >> 8) + ((telemetryData.value.volts & 0x000F) << 8))) / 10);
    telemetryData.value.cellVolts[battnumber] = cellVolts;
    if (!telemetryData.value.minCellVolts || cellVolts<telemetryData.value.minCellVolts || battnumber==telemetryData.value.minCellIdx) {
      telemetryData.value.minCellIdx = battnumber;
      telemetryData.value.minCellVolts = cellVolts;
      if (!telemetryData.value.minCell || telemetryData.value.minCellVolts<telemetryData.value.minCell)
        telemetryData.value.minCell = telemetryData.value.minCellVolts;
    }
  }
}

void checkMinMaxAltitude()
{
  if (TELEMETRY_RELATIVE_BARO_ALT_BP > telemetryData.value.maxAltitude)
    telemetryData.value.maxAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
  if (TELEMETRY_RELATIVE_BARO_ALT_BP < telemetryData.value.minAltitude)
    telemetryData.value.minAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
}


// Alarms level sent to the FrSky module (TODO Remove code ?)

uint8_t frskyAlarmsSendState = 0 ;

void telemetryWakeup()
{
  if (!IS_USR_PROTO_SMART_PORT()) {
    // Attempt to transmit any waiting Fr-Sky alarm set packets every 50ms (subject to packet buffer availability)
    static uint8_t frskyTxDelay = 5;
    if (frskyAlarmsSendState && (--frskyTxDelay == 0)) {
      frskyTxDelay = 5; // 50ms
#if !defined(SIMU)
      frskyDSendNextAlarm();
#endif
    }
  }


#if defined(VARIO)
  if (TELEMETRY_STREAMING() && !IS_FAI_ENABLED()) {
    varioWakeup();
  }
#endif

#define FRSKY_BAD_ANTENNA() (telemetryData.swr.value > 0x33)

}

void frskyPushValue(uint8_t *&ptr, uint8_t value)
{
  // byte stuff the only byte than might need it
  bool bytestuff = false;

  if (value == START_STOP) {
    bytestuff = true;
    value = 0x5e;
  } else if (value == BYTESTUFF) {
    bytestuff = true;
    value = 0x5d;
  }

  *ptr++ = value;
  if (bytestuff)
    *ptr = BYTESTUFF;
}

void frskySendPacket(uint8_t type, uint8_t value, uint8_t p1, uint8_t p2)
{
  uint8_t *ptr = &Usart0TxBuffer[0];

  *ptr++ = START_STOP;        // End of packet
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = (IS_SOUND_OFF() ? alarm_off : p2);
  *ptr++ = p1;
  frskyPushValue(ptr, value);
  *ptr++ = type;
  *ptr++ = START_STOP; // Start of packet

  Usart0TxBufferCount = ptr - &Usart0TxBuffer[0];
#if !defined(SIMU)
  Usart0TransmitBuffer();
#endif
}

inline void frskyDSendNextAlarm()
{
  if (Usart0TxBufferCount)
    return; // we only have one buffer. If it's in use, then we can't send yet.

  // Now send a packet
  frskyAlarmsSendState -= 1;
  uint8_t alarm = 1 - (frskyAlarmsSendState % 2);
  if (frskyAlarmsSendState < SEND_MODEL_ALARMS) {
    uint8_t channel = 1 - (frskyAlarmsSendState / 2);
    frskySendPacket(A22PKT + frskyAlarmsSendState, g_model.telemetry.channels[channel].alarms_value[alarm], ALARM_GREATER(channel, alarm), ALARM_LEVEL(channel, alarm));
  } else {
    frskySendPacket(RSSI1PKT-alarm, getRssiAlarmValue(alarm), 0, (2+alarm+g_model.telemetry.rssiAlarms[alarm].level) % 4);
  }
}

#if (0)
void frskyRFProcessPacket(uint8_t *packet)
{
  // 20 bytes
  /*
  *  pkt 0 = length not counting appended status bytes
  *  pkt 1,2 = fixed_id
  *  pkt 3 = A1 : 52mV per count; 4.5V = 0x56
  *  pkt 4 = A2 : 13.4mV per count; 3.0V = 0xE3 on D6FR
  *  pkt 5 = RSSI
  *  pkt 6 = number of stream bytes
  *  pkt 7 = sequence number increments mod 32 when packet containing stream data acknowledged
  *  pkt 8-(8+(pkt[6]-1)) = stream data
  *  pkt len-2 = Downlink RSSI
  *  pkt len-1 = crc status (bit7 set indicates good), link quality indicator (bits6-0)
  */

  // A1, A2, RSSI values.

  telemetryData.analog[TELEM_ANA_A1].set(packet[3], g_model.telemetry.channels[TELEM_ANA_A1].type);
  telemetryData.analog[TELEM_ANA_A2].set(packet[4], g_model.telemetry.channels[TELEM_ANA_A2].type);
  telemetryData.rssi[0].set(packet[5]); // RSSI Tx -> Rx.

  telemetryData.rssi[1].set(packet[ packet[0]+1 ]); // RSSI Rx -> Tx.
}


void parseTelemMMsmartData(uint16_t SP_id, uint32_t SP_data, uint8_t SP_data8)
{
  switch (SP_id) {
  case CURR_FIRST_ID:
    telemetryData.value.current = SP_data;
    if (telemetryData.value.current > telemetryData.value.maxCurrent)
      telemetryData.value.maxCurrent = telemetryData.value.current;
    break;
  case VFAS_FIRST_ID:
    telemetryData.value.vfas = SP_data / 10;
    if (telemetryData.value.vfas < telemetryData.value.minVfas || telemetryData.value.minVfas == 0)
      telemetryData.value.minVfas = telemetryData.value.vfas;
    break;
  case FUEL_FIRST_ID:
    telemetryData.value.currentConsumption = SP_data;
    telemetryData.value.fuelLevel = SP_data8;
    break;
  case RSSI_ID:
    telemetryData.rssi[0].value = SP_data8;
    if (telemetryData.rssi[0].value < telemetryData.rssi[0].min || telemetryData.rssi[0].min == 0)
      telemetryData.rssi[0].min = telemetryData.rssi[0].value;
    telemetryData.rssi[1].value = SP_data8;
    if (telemetryData.rssi[1].value < telemetryData.rssi[1].min || telemetryData.rssi[1].min == 0)
      telemetryData.rssi[1].min = telemetryData.rssi[1].value;
    break;
  case ADC2_ID:

    break;
  case BATT_ID:

    break;
  case A4_FIRST_ID:
    telemetryData.value.minCellVolts = SP_data / 10;
    break;

  }
}

#endif
# 1 "gui/view_telemetry.cpp"
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


#include "../OpenAVRc.h"

#define STATUS_BAR_Y     (7*FH+1)
#define TELEM_2ND_COLUMN (10*FW)

bar_threshold_t barsThresholds[THLD_MAX];

uint8_t s_frsky_view = 0;

#define BAR_LEFT    25
#define BAR_WIDTH   100

void displayRssiLine()
{
  if (TELEMETRY_STREAMING()) {
    lcdDrawSolidHorizontalLine(0, 55, 128, 0); // separator
    uint8_t rssi;
    rssi = min((uint8_t)99, telemetryData.rssi[1].value);
    lcdDrawTextLeft(STATUS_BAR_Y, STR_TX);
    lcdDrawNumberNAtt(4*FW+1, STATUS_BAR_Y, rssi, LEADING0, 2);
    lcdDrawRect(BAR_LEFT+1, 57, 38, 7);
    lcdDrawFilledRect(BAR_LEFT+1, 58, 4*rssi/11, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
    rssi = min((uint8_t)99, TELEMETRY_RSSI());
    lcdDrawText(104, STATUS_BAR_Y, STR_RX);
    lcdDrawNumberNAtt(105+4*FW, STATUS_BAR_Y, rssi, LEADING0, 2);
    lcdDrawRect(65, 57, 38, 7);
    uint8_t v = 4*rssi/11;
    lcdDrawFilledRect(66+36-v, 58, v, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  } else {
    lcdDrawTextAtt(7*FW, STATUS_BAR_Y, STR_NODATA, BLINK);
    lcd_status_line();
  }
}

#if defined(GPS)
void displayGpsTime()
{
  uint8_t att = (TELEMETRY_STREAMING() ? LEFT|LEADING0 : LEFT|LEADING0|BLINK);
  lcdDrawNumberNAtt(CENTER_OFS+6*FW+7, STATUS_BAR_Y, telemetryData.value.hour, att, 2);
  lcdDrawCharAtt(CENTER_OFS+8*FW+4, STATUS_BAR_Y, ':', att);
  lcdDrawNumberNAtt(CENTER_OFS+9*FW+2, STATUS_BAR_Y, telemetryData.value.min, att, 2);
  lcdDrawCharAtt(CENTER_OFS+11*FW-1, STATUS_BAR_Y, ':', att);
  lcdDrawNumberNAtt(CENTER_OFS+12*FW-3, STATUS_BAR_Y, telemetryData.value.sec, att, 2);
  lcd_status_line();
}

void displayGpsCoord(uint8_t y, char direction, int16_t bp, int16_t ap)
{
  if (telemetryData.value.gpsFix >= 0) {
    if (!direction) direction = '-';
    lcdDrawNumberAttUnit(TELEM_2ND_COLUMN, y, bp / 100, LEFT); // ddd before '.'
    lcdDrawChar(lcdLastPos, y, '@');
    uint8_t mn = bp % 100; // TODO div_t
    if (g_eeGeneral.gpsFormat == 0) {
      lcdDrawChar(lcdLastPos+FWNUM, y, direction);
      lcdDrawNumberNAtt(lcdLastPos+FW+FW+1, y, mn, LEFT|LEADING0, 2); // mm before '.'
      lcdDrawSolidVerticalLine(lcdLastPos, y, 2);
      uint16_t ss = ap * 6;
      lcdDrawNumberNAtt(lcdLastPos+3, y, ss / 1000, LEFT|LEADING0, 2); // ''
      lcdDrawPoint(lcdLastPos, y+FH-2, 0); // small decimal point
      lcdDrawNumberNAtt(lcdLastPos+2, y, ss % 1000, LEFT|LEADING0, 3); // ''
      lcdDrawSolidVerticalLine(lcdLastPos, y, 2);
      lcdDrawSolidVerticalLine(lcdLastPos+2, y, 2);
    } else {
      lcdDrawNumberNAtt(lcdLastPos+FW, y, mn, LEFT|LEADING0, 2); // mm before '.'
      lcdDrawPoint(lcdLastPos, y+FH-2, 0); // small decimal point
      lcdDrawNumberNAtt(lcdLastPos+2, y, ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
      lcdDrawChar(lcdLastPos+1, y, direction);
    }
  } else {
    // no fix
    lcdDrawText(TELEM_2ND_COLUMN, y, STR_VCSWFUNC+1/*----*/);
  }
}
#else
#define displayGpsTime()
#define displayGpsCoord(...)
#endif

NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm)
{
  return (45 - 3*alarm + g_model.telemetry.rssiAlarms[alarm].value);
}

void displayVoltageScreenLine(uint8_t y, uint8_t index)
{
  lcdDrawStringWithIndex(0, y, STR_A, index+1, 0);
  if (TELEMETRY_STREAMING()) {
    lcdPutsTelemetryChannelValue(3*FW+6*FW+4, y-FH, index+TELEM_A1-1, telemetryData.analog[index].value, DBLSIZE);
    lcdDrawChar(12*FW-1, y-FH, '<');
    lcdPutsTelemetryChannelValue(17*FW, y-FH, index+TELEM_A1-1, telemetryData.analog[index].min, NO_UNIT);
    lcdDrawChar(12*FW, y, '>');
    lcdPutsTelemetryChannelValue(17*FW, y, index+TELEM_A1-1, telemetryData.analog[index].max, NO_UNIT);
  }
}

uint8_t barCoord(int16_t value, int16_t min, int16_t max)
{
  return limit<uint8_t>(0, (((int32_t)(BAR_WIDTH-1) * (value - min)) / (max - min)), BAR_WIDTH);
}

void displayVoltagesScreen()
{
  // Volts / Amps / Watts / mAh
  uint8_t analog = 0;
  lcdDrawTextAtIndex(0, 2*FH, STR_AMPSRC, g_model.telemetry.voltsSource+1, 0);
  switch (g_model.telemetry.voltsSource) {
  case FRSKY_VOLTS_SOURCE_A1:
  case FRSKY_VOLTS_SOURCE_A2:
    displayVoltageScreenLine(2*FH, g_model.telemetry.voltsSource);
    analog = 1+g_model.telemetry.voltsSource;
    break;
  case FRSKY_VOLTS_SOURCE_FAS:
    lcdPutsTelemetryChannelValue(3*FW+6*FW+4, FH, TELEM_VFAS-1, telemetryData.value.vfas, DBLSIZE);
    break;
  case FRSKY_VOLTS_SOURCE_CELLS:
    lcdPutsTelemetryChannelValue(3*FW+6*FW+4, FH, TELEM_CELLS_SUM-1, telemetryData.value.cellsSum, DBLSIZE);
    break;
  }

  if (g_model.telemetry.currentSource) {
    lcdDrawTextAtIndex(0, 4*FH, STR_AMPSRC, g_model.telemetry.currentSource, 0);
    switch(g_model.telemetry.currentSource) {
    case FRSKY_CURRENT_SOURCE_A1:
    case FRSKY_CURRENT_SOURCE_A2:
      displayVoltageScreenLine(4*FH, g_model.telemetry.currentSource-1);
      break;
    case FRSKY_CURRENT_SOURCE_FAS:
      lcdPutsTelemetryChannelValue(3*FW+6*FW+4, 3*FH, TELEM_CURRENT-1, telemetryData.value.current, DBLSIZE);
      break;
    }

    lcdPutsTelemetryChannelValue(4, 5*FH, TELEM_POWER-1, telemetryData.value.power, LEFT|DBLSIZE);
    lcdPutsTelemetryChannelValue(3*FW+4+4*FW+6*FW+FW, 5*FH, TELEM_CONSUMPTION-1, telemetryData.value.currentConsumption, DBLSIZE);
  } else {
    displayVoltageScreenLine(analog > 0 ? 5*FH : 4*FH, analog ? 2-analog : 0);
    if (analog == 0) displayVoltageScreenLine(6*FH, 1);
  }

  // Cells voltage
  if (telemetryData.value.cellsCount > 0) {
    uint8_t y = 1*FH;
    for (uint8_t k=0; k<telemetryData.value.cellsCount && k<6; k++) {
      uint8_t attr = (barsThresholds[THLD_CELL] && telemetryData.value.cellVolts[k] < barsThresholds[THLD_CELL]) ? BLINK|PREC2 : PREC2;
      lcdDrawNumberNAtt(LCD_W, y, TELEMETRY_CELL_VOLTAGE(k), attr, 4);
      y += 1*FH;
    }
    lcdDrawSolidVerticalLine(LCD_W-3*FW-2, 8, 47);
  }

  displayRssiLine();
}

void displayAfterFlightScreen()
{
  uint8_t line=1*FH+1;
  if (IS_GPS_AVAILABLE()) {
    // Latitude
    lcdDrawTextLeft(line, STR_LATITUDE);
    displayGpsCoord(line, telemetryData.value.gpsLatitudeNS, telemetryData.value.gpsLatitude_bp, telemetryData.value.gpsLatitude_ap);
    // Longitude
    line+=1*FH+1;
    lcdDrawTextLeft(line, STR_LONGITUDE);
    displayGpsCoord(line, telemetryData.value.gpsLongitudeEW, telemetryData.value.gpsLongitude_bp, telemetryData.value.gpsLongitude_ap);
    displayGpsTime();
    line+=1*FH+1;
  }
  // Rssi
  lcdDrawTextLeft(line, STR_MINRSSI);
  lcdDrawText(TELEM_2ND_COLUMN, line, STR_TX);
  lcdDrawNumberNAtt(TELEM_2ND_COLUMN+3*FW, line, telemetryData.rssi[1].min, LEFT|LEADING0, 2);
  lcdDrawText(TELEM_2ND_COLUMN+6*FW, line, STR_RX);
  lcdDrawNumberNAtt(TELEM_2ND_COLUMN+9*FW, line, telemetryData.rssi[0].min, LEFT|LEADING0, 2);
}

bool displayGaugesTelemetryScreen(telemetryScreenData & screen)
{
  // Custom Screen with gauges
  uint8_t barHeight = 5;
  for (int8_t i=3; i>=0; i--) {
    FrSkyBarData & bar = screen.bars[i];
    source_t source = bar.source;
    getvalue_t barMin = convertBarTelemValue(source, bar.barMin);
    getvalue_t barMax = convertBarTelemValue(source, 255-bar.barMax);
    if (source && barMax > barMin) {
      uint8_t y = barHeight+6+i*(barHeight+6);
      lcdDrawTextAtIndex(0, y+barHeight-5, STR_VTELEMCHNS, source, 0);
      lcdDrawRect(BAR_LEFT, y, BAR_WIDTH+1, barHeight+2);
      getvalue_t value = getValue(MIXSRC_FIRST_TELEM+source-1);

      uint8_t thresholdX = 0;

      getvalue_t threshold = 0;
      if (source <= TELEM_TIMER_MAX)
        threshold = 0;
      else if (source <= TELEM_RSSI_RX)
        threshold = getRssiAlarmValue(source-TELEM_RSSI_TX);
      else if (source <= TELEM_A2)
        threshold = g_model.telemetry.channels[source-TELEM_A1].alarms_value[0];
      else {
        threshold = convertBarTelemValue(source, barsThresholds[source-TELEM_ALT]);
      }

      if (threshold) {
        thresholdX = barCoord(threshold, barMin, barMax);
        if (thresholdX == 100)
          thresholdX = 0;
      }

      uint8_t width = barCoord(value, barMin, barMax);

      // reversed barshade for T1/T2
      uint8_t barShade = ((threshold > value) ? DOTTED : SOLID);
      if (source == TELEM_T1 || source == TELEM_T2) {
        barShade = -barShade;
      }

      lcdDrawFilledRect(BAR_LEFT+1, y+1, width, barHeight, barShade);

      for (uint8_t j=24; j<99; j+=25) {
        if (j>thresholdX || j>width) {
          lcdDrawSolidVerticalLine(j*BAR_WIDTH/100+BAR_LEFT+1, y+1, barHeight);
        }
      }

      if (thresholdX) {
        lcdDrawSolidVerticalLineStip(BAR_LEFT+1+thresholdX, y-2, barHeight+3, DOTTED);
        lcdDrawSolidHorizontalLine(BAR_LEFT+thresholdX, y-2, 3);
      }
    } else {
      barHeight += 2;
    }
  }
  displayRssiLine();
  return barHeight < 13;
}

bool displayNumbersTelemetryScreen(telemetryScreenData & screen)
{
  // Custom Screen with numbers
  uint8_t fields_count = 0;
  for (uint8_t i=0; i<4; i++) {
    for (uint8_t j=0; j<NUM_LINE_ITEMS; j++) {
      uint8_t field = screen.lines[i].sources[j];
      if (field > 0) {
        fields_count++;
      }
      if (i==3) {
        lcdDrawSolidVerticalLine(63, 8, 48);
        if (TELEMETRY_STREAMING()) {
          if (field == TELEM_ACC) {
            lcdDrawTextLeft(STATUS_BAR_Y, STR_ACCEL);
            lcdDrawNumberNAtt(4*FW, STATUS_BAR_Y, telemetryData.value.accelX, LEFT|PREC2);
            lcdDrawNumberNAtt(10*FW, STATUS_BAR_Y, telemetryData.value.accelY, LEFT|PREC2);
            lcdDrawNumberNAtt(16*FW, STATUS_BAR_Y, telemetryData.value.accelZ, LEFT|PREC2);
            break;
          }
#if defined(GPS)
          else if (field == TELEM_GPS_TIME) {
            displayGpsTime();
            break;
          }
#endif
        } else {
          displayRssiLine();
          return fields_count;
        }
      }
      if (field) {
        getvalue_t value = getValue(MIXSRC_FIRST_TELEM+field-1);
        uint8_t att = (i==3 ? NO_UNIT : DBLSIZE|NO_UNIT);
        coord_t pos[] = {0, 65, 130};
        lcdPutsTelemetryChannelValue(pos[j+1]-2, FH+2*FH*i, field-1, value, att);

        if (field >= TELEM_TIMER1 && field <= TELEM_TIMER_MAX && i!=3) {
          // there is not enough space on LCD for displaying "Tmr1" or "Tmr2" and still see the - sign, we write "T1" or "T2" instead
          field = field-TELEM_TIMER1+TELEM_T1;
        }

        lcdDrawTextAtIndex(pos[j], 1+FH+2*FH*i, STR_VTELEMCHNS, field, 0);
      }
    }
  }
  lcd_status_line();
  return fields_count;
}

bool displayCustomTelemetryScreen(uint8_t index)
{
  telemetryScreenData & screen = g_model.telemetry.screens[index];

  if (IS_BARS_SCREEN(s_frsky_view)) {
    return displayGaugesTelemetryScreen(screen);
  }

  return displayNumbersTelemetryScreen(screen);
}

bool displayTelemetryScreen()
{

  lcdDrawTelemetryTopBar();

  if (s_frsky_view < MAX_TELEMETRY_SCREENS) {
    return displayCustomTelemetryScreen(s_frsky_view);
  }

  if (s_frsky_view == TELEMETRY_VOLTAGES_SCREEN) {
    displayVoltagesScreen();
  }

  else {
    displayAfterFlightScreen();
  }

  return true;
}

void decrTelemetryScreen()
{
  if (s_frsky_view-- == 0)
    s_frsky_view = TELEMETRY_VIEW_MAX;
}
void incrTelemetryScreen()
{
  if (s_frsky_view++ == TELEMETRY_VIEW_MAX)
    s_frsky_view = 0;
}

void menuTelemetryFrsky(uint8_t event)
{

  switch (event) {
  case EVT_KEY_FIRST(KEY_EXIT):
    killEvents(event);
    chainMenu(menuMainView);
    break;

  case EVT_KEY_FIRST(KEY_UP):
    decrTelemetryScreen();
    break;

  case EVT_KEY_FIRST(KEY_DOWN):
    incrTelemetryScreen();
    break;

  case EVT_KEY_FIRST(KEY_ENTER):
    telemetryResetValue();
    break;
  }

  if (!displayTelemetryScreen()) {
    putEvent(event == EVT_KEY_FIRST(KEY_UP) ? event : EVT_KEY_FIRST(KEY_DOWN));
  }
}

# 1 "vario.cpp"
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


#include "OpenAVRc.h"


#if   defined(FRSKY)

void varioWakeup()
{
  static tmr10ms_t s_varioTmr;
  tmr10ms_t tmr10ms = get_tmr10ms();

  if (isFunctionActive(FUNCTION_VARIO)) {
#if defined(AUDIO)
    cli();
    int16_t verticalSpeed = telemetryData.value.varioSpeed;
    sei();
    int varioCenterMin = (int)g_model.telemetry.varioCenterMin * 10 - 50;
    int varioCenterMax = (int)g_model.telemetry.varioCenterMax * 10 + 50;
    int varioMax = (10+(int)g_model.telemetry.varioMax) * 100;
    int varioMin = (-10+(int)g_model.telemetry.varioMin) * 100;

    if (verticalSpeed < varioCenterMin || (verticalSpeed > varioCenterMax && (int16_t)(s_varioTmr - tmr10ms) < 0)) {
      if (verticalSpeed > varioMax)
        verticalSpeed = varioMax;
      else if (verticalSpeed < varioMin)
        verticalSpeed = varioMin;

      uint8_t varioFreq, varioDuration;
      if (verticalSpeed > 0) {
        varioFreq = (verticalSpeed * 4 + 8000) >> 7;
        varioDuration = (8000 - verticalSpeed * 5) / 100;
      } else {
        varioFreq = (verticalSpeed * 3 + 8000) >> 7;
        varioDuration = 20;
      }
      s_varioTmr = tmr10ms + (varioDuration/2);
      AUDIO_VARIO(varioFreq, varioDuration);
    }

#elif defined(BUZZER) // && !defined(AUDIO)

    int8_t verticalSpeed = limit((int16_t)-100, (int16_t)(telemetryData.value.varioSpeed/10), (int16_t)+100);

    uint16_t interval;
    if (verticalSpeed == 0) {
      interval = 300;
    } else {
      if (verticalSpeed < 0) {
        verticalSpeed = -verticalSpeed;
        warble = 1;
      }
      interval = (uint8_t)200 / verticalSpeed;
    }
    if (g_tmr10ms - s_varioTmr > interval) {
      s_varioTmr = g_tmr10ms;
      if (warble)
        AUDIO_VARIO_DOWN();
      else
        AUDIO_VARIO_UP();
    }
#endif
  } else {
    s_varioTmr = tmr10ms;
  }
}

#endif
# 1 "templates.cpp"
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


/*
 *
 * ============================================================
 * Templates file
 *
 * eccpm
 * crow
 * throttle cut
 * flaperon
 * elevon
 * v-tail
 * throttle hold
 * Aileron Differential
 * Spoilers
 * Snap Roll
 * ELE->Flap
 * Flap->ELE
 *
 */

#include "OpenAVRc.h"


MixData* setDest(uint8_t dch, uint8_t src, bool clear=false)
{
  uint8_t i = 0;
  MixData * mix;

  while (1) {
    mix = mixAddress(i);
    if (mix->srcRaw && mix->destCh <= dch) {
      if (clear && mix->destCh == dch) {
        deleteExpoMix(0, i);
      } else {
        if (++i==MAX_MIXERS) {
          // TODO should return null pointer but needs to be tested then
          mix = mixAddress(0);
          break;
        }
      }
    } else {
      break;
    }
  }

  memmove(mix+1, mix, (MAX_MIXERS-(i+1))*sizeof(MixData) );
  memclear(mix, sizeof(MixData));
  mix->destCh = dch;
  mix->srcRaw = src;
  mix->weight = 100;
  return mix;
}

void mixSetWeight(MixData* md, int8_t weight)
{
  u_int8int16_t tmp;
  tmp.gvword=weight;
  MD_UNION_TO_WEIGHT(tmp,md);
  // MD_SETWEIGHT(md,weight);  doesn't matter here in code cost compiler optimizes this anyway
}

#define clearInputs()
#define defaultInputs()
#define TMPL_INPUT(x) (MIXSRC_Rud+x-1)

void clearMixes()
{
  memset(g_model.mixData, 0, sizeof(g_model.mixData)); // clear all mixes
}

void clearCurves()
{
  memclear(g_model.curves, sizeof(g_model.curves) + sizeof(g_model.points)); // clear all curves
}

void setCurve(uint8_t c, const pm_int8_t ar[])
{
  int8_t * cv = curveAddress(c);
  for (uint8_t i=0; i<5; i++) {
    cv[i] = pgm_read_byte_near(&ar[i]);
  }
}

void setLogicalSwitch(uint8_t idx, uint8_t func, int8_t v1, int8_t v2)
{
  LogicalSwitchData *cs = lswAddress(idx-1);
  cs->func = func;
  cs->v1   = v1;
  cs->v2   = v2;
}

const pm_int8_t heli_ar1[] PROGMEM = {-100, 20, 30, 70, 90};
const pm_int8_t heli_ar2[] PROGMEM = {80, 70, 60, 70, 100};
const pm_int8_t heli_ar3[] PROGMEM = {100, 90, 80, 90, 100};
const pm_int8_t heli_ar4[] PROGMEM = {-30, -15, 0, 50, 100};
const pm_int8_t heli_ar5[] PROGMEM = {-100, -50, 0, 50, 100};

void applyTemplate(uint8_t idx)
{
  MixData *md;

  //CC(STK)   -> vSTK
  //ICC(vSTK) -> STK
#define ICC(x) icc[(x)-1]
  uint8_t icc[4] = {0};
  for (uint8_t i=0; i<4; i++) { //generate inverse array
    for(uint8_t j=0; j<4; j++)
      if(CC(i+1)==j+MIXSRC_Rud) icc[j]=i;
  }

  switch (idx) {
  case TMPL_CLEAR_MIXES:
  case TMPL_SIMPLE_4CH:
  case TMPL_HELI_SETUP:
    clearMixes();
    break;
  }

  switch (idx) {
  // Simple 4-Ch
  case TMPL_SIMPLE_4CH:
    defaultInputs();
    setDest(ICC(STK_RUD), TMPL_INPUT(STK_RUD));
    setDest(ICC(STK_ELE), TMPL_INPUT(STK_ELE));
    setDest(ICC(STK_THR), TMPL_INPUT(STK_THR));
    setDest(ICC(STK_AIL), TMPL_INPUT(STK_AIL));
    break;

  // Sticky-T-Cut
  case TMPL_STI_THR_CUT:
    md=setDest(ICC(STK_THR), MIXSRC_MAX);
    mixSetWeight(md, -100);
    md->swtch=SWSRC_SWC;
    md->mltpx=MLTPX_REP;
    md=setDest(13, MIXSRC_CH14); // md->weight= 100; done by setDest anyway
    md=setDest(13, MIXSRC_MAX);
    mixSetWeight(md, -100);
    md->swtch=SWSRC_SWB;
    md->mltpx=MLTPX_REP;
    md=setDest(13, MIXSRC_MAX); /* md->weight= 100;*/  md->swtch=SWSRC_THR;
    md->mltpx=MLTPX_REP;
    setLogicalSwitch(11, LS_FUNC_VNEG, STK_THR, -99);
    setLogicalSwitch(12, LS_FUNC_VPOS, MIXSRC_CH14, 0);
    break;

  // V-Tail
  case TMPL_V_TAIL:
    defaultInputs();
    setDest(ICC(STK_RUD), TMPL_INPUT(STK_RUD), true);
    md=setDest(ICC(STK_RUD), TMPL_INPUT(STK_ELE));
    mixSetWeight(md, -100);
    setDest(ICC(STK_ELE), TMPL_INPUT(STK_RUD), true);
    setDest(ICC(STK_ELE), TMPL_INPUT(STK_ELE));
    break;

  // Elevon\\Delta
  case TMPL_ELEVON_DELTA:
    defaultInputs();
    setDest(ICC(STK_ELE), MIXSRC_Ele, true);
    setDest(ICC(STK_ELE), MIXSRC_Ail);
    setDest(ICC(STK_AIL), MIXSRC_Ele, true);
    md=setDest(ICC(STK_AIL), MIXSRC_Ail);
    mixSetWeight(md, -100);
    break;

  // eCCPM
  case TMPL_ECCPM:
    md=setDest(ICC(STK_ELE), MIXSRC_Ele, true);
    md->weight= 72;
    md=setDest(ICC(STK_ELE), MIXSRC_Thr);
    md->weight= 55;
    md=setDest(ICC(STK_AIL), MIXSRC_Ele, true);
    mixSetWeight(md, -36);
    md=setDest(ICC(STK_AIL), MIXSRC_Ail);
    md->weight= 62;
    md=setDest(ICC(STK_AIL), MIXSRC_Thr);
    md->weight= 55;
    md=setDest(5, MIXSRC_Ele, true);
    mixSetWeight(md, -36);
    md=setDest(5, MIXSRC_Ail);
    mixSetWeight(md, -62);
    md=setDest(5, MIXSRC_Thr);
    md->weight= 55;
    break;

  // Heli Setup
  case TMPL_HELI_SETUP:
    clearCurves();

    //Set up Mixes
    // 3 cyclic channels
    md=setDest(0, MIXSRC_CYC1); // md->weight=100;
    md=setDest(1, MIXSRC_CYC2); // md->weight=100;
    md=setDest(2, MIXSRC_CYC3); // md->weight=100;

    // rudder
    md=setDest(3, MIXSRC_Rud); // md->weight=100;

    // throttle
    md=setDest(4, MIXSRC_Thr);
    md->swtch=SWSRC_ID0;
    mixSetCurve(md, 0);
    md->carryTrim=TRIM_OFF;
    md=setDest(4, MIXSRC_Thr);
    md->swtch=SWSRC_ID1;
    mixSetCurve(md, 1);
    md->carryTrim=TRIM_OFF;
    md=setDest(4, MIXSRC_Thr);
    md->swtch=SWSRC_ID2;
    mixSetCurve(md, 2);
    md->carryTrim=TRIM_OFF;
    md=setDest(4, MIXSRC_MAX);
    mixSetWeight(md, -100);
    md->swtch=SWSRC_THR;
    md->mltpx=MLTPX_REP;

    // gyro gain
    md=setDest(5, MIXSRC_MAX);
    md->weight= 30;
    md->swtch=-SWSRC_GEA;
    md=setDest(5, MIXSRC_MAX);
    mixSetWeight(md, -30);
    md->swtch= SWSRC_GEA;

    // collective
    md=setDest(10, MIXSRC_Thr); /*md->weight= 100;*/ md->swtch=SWSRC_ID0;
    mixSetCurve(md, 3);
    md->carryTrim=TRIM_OFF;
    md=setDest(10, MIXSRC_Thr); /*md->weight= 100;*/ md->swtch=SWSRC_ID1;
    mixSetCurve(md, 4);
    md->carryTrim=TRIM_OFF;
    md=setDest(10, MIXSRC_Thr); /*md->weight= 100;*/ md->swtch=SWSRC_ID2;
    mixSetCurve(md, 5);
    md->carryTrim=TRIM_OFF;

    g_model.swashR.collectiveSource = MIXSRC_CH11;
    g_model.swashR.type = SWASH_TYPE_120;

    // curves
    setCurve(0, heli_ar1);
    setCurve(1, heli_ar2);
    setCurve(2, heli_ar3);
    setCurve(3, heli_ar4);
    setCurve(4, heli_ar5);
    setCurve(5, heli_ar5);
    break;

  // Servo Test
  case TMPL_SERVO_TEST:
    md=setDest(NUM_CHNOUT-1, MIXSRC_SW1, true);
    md->weight=110;
    md->mltpx=MLTPX_ADD;
    md->delayUp = 6;
    md->delayDown = 6;
    md->speedUp = 8;
    md->speedDown = 8;
    setLogicalSwitch(1, LS_FUNC_VNEG, MIXSRC_LAST_CH, 0);
    break;

  default:
    break;

  }

  eeDirty(EE_MODEL);
}
# 1 "adc_driver.cpp"
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


#include "OpenAVRc.h"


#define ADC_VREF_TYPE (1 << REFS0) // AVCC with external capacitor at AREF pin

void adcInit()
{
  ADMUX = ADC_VREF_TYPE;
  ADCSRA = 0x85; // ADC enabled, pre-scaler division=32 (no interrupt, no auto-triggering)
  ADCSRB = (1 << MUX5);
}


void getADC() // 0.56 mS
{
  for (uint8_t adc_input=0; adc_input<8; adc_input++) {
    uint16_t temp_ana;
    ADMUX = adc_input|ADC_VREF_TYPE;
    ADCSRA |= 1 << ADSC; // Start the AD conversion
    while SIMU_UNLOCK_MACRO(bit_is_set(ADCSRA,ADSC)); // Wait for the AD conversion to complete
    temp_ana = ADC;
    ADCSRA |= 1 << ADSC; // Start the second AD conversion
    while SIMU_UNLOCK_MACRO(bit_is_set(ADCSRA,ADSC)); // Wait for the AD conversion to complete
    temp_ana += ADC;
#if !defined(SIMU)
    s_anaFilt[adc_input] = temp_ana;
#endif
  }
}
# 1 "targets/lcd/lcd_driver.cpp"
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


#include "../../OpenAVRc.h"


#if defined(SIMU)
#include "lcd_simu_driver.cpp"
#else
#if defined(LCD_KS108)
#include "targets/lcd/lcd_ks108_driver.cpp"
#elif defined(LCD_ST7920)
#include "targets/lcd/lcd_st7920_driver.cpp"
#elif defined(LCD_SSD1306)
#include "targets/lcd/lcd_ssd1306_driver.cpp"
#elif defined(LCD_SH1106)
#include "targets/lcd/lcd_sh1106_driver.cpp"
#else
#include "targets/lcd/lcd_default_driver.cpp"
#endif
#endif // defined
# 1 "telemetry_driver.cpp"
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


#include "OpenAVRc.h"

uint8_t Usart0RxBuffer[USART0_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)

uint8_t Usart0TxBuffer[USART0_TX_PACKET_SIZE];

uint8_t Usart0TxBufferCount = 0;

uint8_t Usart0RxBufferCount = 0;

void Usart0EnableTx()
{
  UCSRB_N(TLM_USART0) |= (1 << TXEN_N(TLM_USART0)); // enable TX
}

void Usart0EnableRx()
{
  UCSRB_N(TLM_USART0) |= (1 << RXEN_N(TLM_USART0));  // enable RX
  UCSRB_N(TLM_USART0) |= (1 << RXCIE_N(TLM_USART0)); // enable Interrupt
  while (UCSRA_N(TLM_USART0) & (1 << RXC_N(TLM_USART0))) UDR_N(TLM_USART0); // Flush RX buffer.
}

void Usart0DisableTx()
{
  UCSRB_N(TLM_USART0) &= ~(1 << UDRIE_N(TLM_USART0));// disable Interrupt
  UCSRB_N(TLM_USART0) &= ~(1 << TXEN_N(TLM_USART0)); // disable TX
}

void Usart0DisableRx()
{
  UCSRB_N(TLM_USART0) &= ~(1 << RXCIE_N(TLM_USART0)); // disable Interrupt
  UCSRB_N(TLM_USART0) &= ~(1 << RXEN_N(TLM_USART0));  // disable RX
}

void Usart0Set8N1()
{
  UCSRB_N(TLM_USART0) = (0 << RXCIE_N(TLM_USART0)) | (0 << TXCIE_N(TLM_USART0)) | (0 << UDRIE_N(TLM_USART0)) | (0 << RXEN_N(TLM_USART0)) | (0 << TXEN_N(TLM_USART0)) | (0 << UCSZ2_N(TLM_USART0));
  UCSRC_N(TLM_USART0) = (1 << UCSZ1_N(TLM_USART0)) | (1 << UCSZ0_N(TLM_USART0)); // Set 1 stop bit, No parity bit.
}

void Usart0Set8E2()
{
  UCSRB_N(TLM_USART0) = (0 << RXCIE_N(TLM_USART0)) | (0 << TXCIE_N(TLM_USART0)) | (0 << UDRIE_N(TLM_USART0)) | (0 << RXEN_N(TLM_USART0)) | (0 << TXEN_N(TLM_USART0)) | (0 << UCSZ2_N(TLM_USART0));
  UCSRC_N(TLM_USART0) = (1 << UPM01) | (1 << USBS0)| (1 << UCSZ1_N(TLM_USART0)) | (1 << UCSZ0_N(TLM_USART0)); // set 2 stop bits, even parity BIT
}

void Usart0Set9600BAUDS() //Frsky "D" telemetry
{
#undef BAUD
#define BAUD 9600
#if !defined(SIMU)
#include <util/setbaud.h>
  UBRRH_N(TLM_USART0) = UBRRH_VALUE;
  UBRRL_N(TLM_USART0) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART0) |= (1 << U2X_N(TLM_USART0));
#else
   UCSRA_N(TLM_USART0) &= ~(1 << U2X_N(TLM_USART0));
#endif
#endif
}

void Usart0Set57600BAUDS() //Frsky S.port telemetry
{
#undef BAUD
#define BAUD 57600
#if !defined(SIMU)
#include <util/setbaud.h>
  UBRRH_N(TLM_USART0) = UBRRH_VALUE;
  UBRRL_N(TLM_USART0) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART0) |= (1 << U2X_N(TLM_USART0));
#else
   UCSRA_N(TLM_USART0) &= ~(1 << U2X_N(TLM_USART0));
#endif
#endif
}

void Usart0Set125000BAUDS() //DSM Serial protocol
{
#undef BAUD
#define BAUD 125000
#if !defined(SIMU)
#include <util/setbaud.h>
  UBRRH_N(TLM_USART0) = UBRRH_VALUE;
  UBRRL_N(TLM_USART0) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART0) |= (1 << U2X_N(TLM_USART0));
#else
   UCSRA_N(TLM_USART0) &= ~(1 << U2X_N(TLM_USART0));
#endif
#endif
}

void Usart0Set100000BAUDS() //Multiprotocole Serial
{
#undef BAUD
#define BAUD 100000
#if !defined(SIMU)
#include <util/setbaud.h>
  UBRRH_N(TLM_USART0) = UBRRH_VALUE;
  UBRRL_N(TLM_USART0) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART0) |= (1 << U2X_N(TLM_USART0));
#else
   UCSRA_N(TLM_USART0) &= ~(1 << U2X_N(TLM_USART0));
#endif
#endif
}

void Usart0TransmitBuffer()
{
  UCSRB_N(TLM_USART0) |= (1 << UDRIE_N(TLM_USART0)); // enable Data Register Empty Interrupt
}

#if defined(FRSKY) || defined(MULTI)

ISR(USART_RX_vect_N(TLM_USART0))
{
  uint8_t stat;
  uint8_t data;

  UCSRB_N(TLM_USART0) &= ~(1 << RXCIE_N(TLM_USART0)); // disable Interrupt
  sei();

  stat = UCSRA_N(TLM_USART0); // USART control and Status Register 0/1 A

  /*
              bit      7      6      5      4      3      2      1      0
                      RxC0  TxC0  UDRE0    FE0   DOR0   UPE0   U2X0  MPCM0

              RxC0:   Receive complete
              TXC0:   Transmit Complete
              UDRE0:  USART Data Register Empty
              FE0:    Frame Error
              DOR0:   Data OverRun
              UPE0:   USART Parity Error
              U2X0:   Double Tx Speed
              PCM0:   MultiProcessor Comms Mode
   */
  // rh = UCSRB_N(TLM_USART0); //USART control and Status Register 0/1 B

  /*
            bit      7      6      5      4      3      2      1      0
                 RXCIE0 TxCIE0 UDRIE0  RXEN0  TXEN0 UCSZ02  RXB80  TXB80

            RxCIE0:   Receive Complete int enable
            TXCIE0:   Transmit Complete int enable
            UDRIE0:   USART Data Register Empty int enable
            RXEN0:    Rx Enable
            TXEN0:    Tx Enable
            UCSZ02:   Character Size bit 2
            RXB80:    Rx data bit 8
            TXB80:    Tx data bit 8
  */

  data = UDR_N(TLM_USART0); // USART data register 0

  if (stat & ((1 << FE_N(TLM_USART0)) | (1 << DOR_N(TLM_USART0)) | (1 << UPE_N(TLM_USART0)))) {
    // discard buffer and start fresh on any comms error
    Usart0RxBufferCount = 0;
  } else {
    processSerialData(data);
  }

  cli() ;
  UCSRB_N(TLM_USART0) |= (1 << RXCIE_N(TLM_USART0)); // enable Interrupt
}
#endif

// USART0 Transmit Data Register Emtpy ISR (UDR was loaded in Shift Register)
ISR(USART_UDRE_vect_N(TLM_USART0))
{
  if (Usart0TxBufferCount > 0) {
    UDR_N(TLM_USART0) = Usart0TxBuffer[--Usart0TxBufferCount];
  } else {
    UCSRB_N(TLM_USART0) &= ~(1 << UDRIE_N(TLM_USART0)); // Disable UDRE interrupt.
  }
}

#if defined(FRSKY)
void TelemetryValueWithMin::set(uint8_t value)
{
  if (!this->value) {
    this->value = value;
  } else {
        this->value = (((this->value<<1) + value)/3);
        if (this->value<value) { ++this->value; }
  }
  if (!min || value < min) {
    min = value;
  }
}

void TelemetryValueWithMinMax::set(uint8_t value, uint8_t unit)
{
  TelemetryValueWithMin::set(value);
  if (unit != UNIT_VOLTS) {
    this->value = value;
  }
  if (!max || value > max) {
    max = value;
  }
}
#endif

uint16_t getChannelRatio(source_t channel)
{
  return (uint16_t)g_model.telemetry.channels[channel].ratio << g_model.telemetry.channels[channel].multiplier;
}

lcdint_t applyChannelRatio(source_t channel, lcdint_t val)
{
  return ((int32_t)val+g_model.telemetry.channels[channel].offset) * getChannelRatio(channel) * 2 / 51;
}

# 1 "thirdparty/FatFs/ff.c"
/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.10c                (C)ChaN, 2014
/-----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following terms.
/
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-----------------------------------------------------------------------------/
/ Feb 26,'06 R0.00  Prototype.
/
/ Apr 29,'06 R0.01  First stable version.
/
/ Jun 01,'06 R0.02  Added FAT12 support.
/                   Removed unbuffered mode.
/                   Fixed a problem on small (<32M) partition.
/ Jun 10,'06 R0.02a Added a configuration option (_FS_MINIMUM).
/
/ Sep 22,'06 R0.03  Added f_rename().
/                   Changed option _FS_MINIMUM to _FS_MINIMIZE.
/ Dec 11,'06 R0.03a Improved cluster scan algorithm to write files fast.
/                   Fixed f_mkdir() creates incorrect directory on FAT32.
/
/ Feb 04,'07 R0.04  Supported multiple drive system.
/                   Changed some interfaces for multiple drive system.
/                   Changed f_mountdrv() to f_mount().
/                   Added f_mkfs().
/ Apr 01,'07 R0.04a Supported multiple partitions on a physical drive.
/                   Added a capability of extending file size to f_lseek().
/                   Added minimization level 3.
/                   Fixed an endian sensitive code in f_mkfs().
/ May 05,'07 R0.04b Added a configuration option _USE_NTFLAG.
/                   Added FSINFO support.
/                   Fixed DBCS name can result FR_INVALID_NAME.
/                   Fixed short seek (<= csize) collapses the file object.
/
/ Aug 25,'07 R0.05  Changed arguments of f_read(), f_write() and f_mkfs().
/                   Fixed f_mkfs() on FAT32 creates incorrect FSINFO.
/                   Fixed f_mkdir() on FAT32 creates incorrect directory.
/ Feb 03,'08 R0.05a Added f_truncate() and f_utime().
/                   Fixed off by one error at FAT sub-type determination.
/                   Fixed btr in f_read() can be mistruncated.
/                   Fixed cached sector is not flushed when create and close without write.
/
/ Apr 01,'08 R0.06  Added fputc(), fputs(), fprintf() and fgets().
/                   Improved performance of f_lseek() on moving to the same or following cluster.
/
/ Apr 01,'09 R0.07  Merged Tiny-FatFs as a configuration option. (_FS_TINY)
/                   Added long file name feature.
/                   Added multiple code page feature.
/                   Added re-entrancy for multitask operation.
/                   Added auto cluster size selection to f_mkfs().
/                   Added rewind option to f_readdir().
/                   Changed result code of critical errors.
/                   Renamed string functions to avoid name collision.
/ Apr 14,'09 R0.07a Separated out OS dependent code on reentrant cfg.
/                   Added multiple sector size feature.
/ Jun 21,'09 R0.07c Fixed f_unlink() can return FR_OK on error.
/                   Fixed wrong cache control in f_lseek().
/                   Added relative path feature.
/                   Added f_chdir() and f_chdrive().
/                   Added proper case conversion to extended character.
/ Nov 03,'09 R0.07e Separated out configuration options from ff.h to ffconf.h.
/                   Fixed f_unlink() fails to remove a sub-directory on _FS_RPATH.
/                   Fixed name matching error on the 13 character boundary.
/                   Added a configuration option, _LFN_UNICODE.
/                   Changed f_readdir() to return the SFN with always upper case on non-LFN cfg.
/
/ May 15,'10 R0.08  Added a memory configuration option. (_USE_LFN = 3)
/                   Added file lock feature. (_FS_SHARE)
/                   Added fast seek feature. (_USE_FASTSEEK)
/                   Changed some types on the API, XCHAR->TCHAR.
/                   Changed .fname in the FILINFO structure on Unicode cfg.
/                   String functions support UTF-8 encoding files on Unicode cfg.
/ Aug 16,'10 R0.08a Added f_getcwd().
/                   Added sector erase feature. (_USE_ERASE)
/                   Moved file lock semaphore table from fs object to the bss.
/                   Fixed a wrong directory entry is created on non-LFN cfg when the given name contains ';'.
/                   Fixed f_mkfs() creates wrong FAT32 volume.
/ Jan 15,'11 R0.08b Fast seek feature is also applied to f_read() and f_write().
/                   f_lseek() reports required table size on creating CLMP.
/                   Extended format syntax of f_printf().
/                   Ignores duplicated directory separators in given path name.
/
/ Sep 06,'11 R0.09  f_mkfs() supports multiple partition to complete the multiple partition feature.
/                   Added f_fdisk().
/ Aug 27,'12 R0.09a Changed f_open() and f_opendir() reject null object pointer to avoid crash.
/                   Changed option name _FS_SHARE to _FS_LOCK.
/                   Fixed assertion failure due to OS/2 EA on FAT12/16 volume.
/ Jan 24,'13 R0.09b Added f_setlabel() and f_getlabel().
/
/ Oct 02,'13 R0.10  Added selection of character encoding on the file. (_STRF_ENCODE)
/                   Added f_closedir().
/                   Added forced full FAT scan for f_getfree(). (_FS_NOFSINFO)
/                   Added forced mount feature with changes of f_mount().
/                   Improved behavior of volume auto detection.
/                   Improved write throughput of f_puts() and f_printf().
/                   Changed argument of f_chdrive(), f_mkfs(), disk_read() and disk_write().
/                   Fixed f_write() can be truncated when the file size is close to 4GB.
/                   Fixed f_open(), f_mkdir() and f_setlabel() can return incorrect error code.
/ Jan 15,'14 R0.10a Added arbitrary strings as drive number in the path name. (_STR_VOLUME_ID)
/                   Added a configuration option of minimum sector size. (_MIN_SS)
/                   2nd argument of f_rename() can have a drive number and it will be ignored.
/                   Fixed f_mount() with forced mount fails when drive number is >= 1.
/                   Fixed f_close() invalidates the file object without volume lock.
/                   Fixed f_closedir() returns but the volume lock is left acquired.
/                   Fixed creation of an entry with LFN fails on too many SFN collisions.
/ May 19,'14 R0.10b Fixed a hard error in the disk I/O layer can collapse the directory entry.
/                   Fixed LFN entry is not deleted on delete/rename an object with lossy converted SFN.
/ Nov 09,'14 R0.10c Added a configuration option for the platforms without RTC. (_FS_NORTC)
/                   Fixed volume label created by Mac OS X cannot be retrieved with f_getlabel().
/                   Fixed a potential problem of FAT access that can appear on disk error.
/                   Fixed null pointer dereference on attempting to delete the root direcotry.
/---------------------------------------------------------------------------*/

#include "ff.h"			/* Declarations of FatFs API */
#include "diskio.h"		/* Declarations of disk I/O functions */




/*--------------------------------------------------------------------------

   Module Private Definitions

---------------------------------------------------------------------------*/

#if _FATFS != 80376	/* Revision ID */
#error Wrong include file (ff.h).
#endif

#include "../../OpenAVRc.h"

/* Reentrancy related */
#if _FS_REENTRANT
#if _USE_LFN == 1
#error Static LFN work area cannot be used at thread-safe configuration
#endif
#define	ENTER_FF(fs)		{ if (!lock_fs(fs)) return FR_TIMEOUT; }
#define	LEAVE_FF(fs, res)	{ unlock_fs(fs, res); return res; }
#else
#define	ENTER_FF(fs)
#define LEAVE_FF(fs, res)	return res
#endif

#define	ABORT(fs, res)		{ fp->err = (BYTE)(res); LEAVE_FF(fs, res); }


/* Definitions of sector size */
#if (_MAX_SS < _MIN_SS) || (_MAX_SS != 512 && _MAX_SS != 1024 && _MAX_SS != 2048 && _MAX_SS != 4096) || (_MIN_SS != 512 && _MIN_SS != 1024 && _MIN_SS != 2048 && _MIN_SS != 4096)
#error Wrong sector size configuration
#endif
#if _MAX_SS == _MIN_SS
#define	SS(fs)	((UINT)_MAX_SS)	/* Fixed sector size */
#else
#define	SS(fs)	((fs)->ssize)	/* Variable sector size */
#endif


/* Timestamp feature */
#if _FS_NORTC == 1
#if _NORTC_YEAR < 1980 || _NORTC_YEAR > 2107 || _NORTC_MON < 1 || _NORTC_MON > 12 || _NORTC_MDAY < 1 || _NORTC_MDAY > 31
#error Invalid _FS_NORTC settings
#endif
#define GET_FATTIME()	((DWORD)(_NORTC_YEAR - 1980) << 25 | (DWORD)_NORTC_MON << 21 | (DWORD)_NORTC_MDAY << 16)
#else
#define GET_FATTIME()	get_fattime()
#endif


/* File access control feature */
#if _FS_LOCK
#if _FS_READONLY
#error _FS_LOCK must be 0 at read-only configuration
#endif
typedef struct {
	FATFS *fs;		/* Object ID 1, volume (NULL:blank entry) */
	DWORD clu;		/* Object ID 2, directory (0:root) */
	WORD idx;		/* Object ID 3, directory index */
	WORD ctr;		/* Object open counter, 0:none, 0x01..0xFF:read mode open count, 0x100:write mode */
} FILESEM;
#endif



/* DBCS code ranges and SBCS extend character conversion table */

#if _CODE_PAGE == 932	/* Japanese Shift-JIS */
#define _DF1S	0x81	/* DBC 1st byte range 1 start */
#define _DF1E	0x9F	/* DBC 1st byte range 1 end */
#define _DF2S	0xE0	/* DBC 1st byte range 2 start */
#define _DF2E	0xFC	/* DBC 1st byte range 2 end */
#define _DS1S	0x40	/* DBC 2nd byte range 1 start */
#define _DS1E	0x7E	/* DBC 2nd byte range 1 end */
#define _DS2S	0x80	/* DBC 2nd byte range 2 start */
#define _DS2E	0xFC	/* DBC 2nd byte range 2 end */

#elif _CODE_PAGE == 936	/* Simplified Chinese GBK */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0x80
#define _DS2E	0xFE

#elif _CODE_PAGE == 949	/* Korean */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x41
#define _DS1E	0x5A
#define _DS2S	0x61
#define _DS2E	0x7A
#define _DS3S	0x81
#define _DS3E	0xFE

#elif _CODE_PAGE == 950	/* Traditional Chinese Big5 */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0xA1
#define _DS2E	0xFE

#elif _CODE_PAGE == 437	/* U.S. (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F,0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 720	/* Arabic (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x45,0x41,0x84,0x41,0x86,0x43,0x45,0x45,0x45,0x49,0x49,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x49,0x49,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 737	/* Greek (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x96,0x97,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, \
				0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0xAA,0x92,0x93,0x94,0x95,0x96,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x97,0xEA,0xEB,0xEC,0xE4,0xED,0xEE,0xE7,0xE8,0xF1,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 775	/* Baltic (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x91,0xA0,0x8E,0x95,0x8F,0x80,0xAD,0xED,0x8A,0x8A,0xA1,0x8D,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0x95,0x96,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xE0,0xA3,0xA3,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xA5,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE3,0xE8,0xE8,0xEA,0xEA,0xEE,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 850	/* Multilingual Latin 1 (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 852	/* Latin 2 (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xDE,0x8F,0x80,0x9D,0xD3,0x8A,0x8A,0xD7,0x8D,0x8E,0x8F,0x90,0x91,0x91,0xE2,0x99,0x95,0x95,0x97,0x97,0x99,0x9A,0x9B,0x9B,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA4,0xA4,0xA6,0xA6,0xA8,0xA8,0xAA,0x8D,0xAC,0xB8,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC6,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD2,0xD5,0xD6,0xD7,0xB7,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE3,0xD5,0xE6,0xE6,0xE8,0xE9,0xE8,0xEB,0xED,0xED,0xDD,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xEB,0xFC,0xFC,0xFE,0xFF}

#elif _CODE_PAGE == 855	/* Cyrillic (OEM) */
#define _DF1S	0
#define _EXCVT {0x81,0x81,0x83,0x83,0x85,0x85,0x87,0x87,0x89,0x89,0x8B,0x8B,0x8D,0x8D,0x8F,0x8F,0x91,0x91,0x93,0x93,0x95,0x95,0x97,0x97,0x99,0x99,0x9B,0x9B,0x9D,0x9D,0x9F,0x9F, \
				0xA1,0xA1,0xA3,0xA3,0xA5,0xA5,0xA7,0xA7,0xA9,0xA9,0xAB,0xAB,0xAD,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB6,0xB6,0xB8,0xB8,0xB9,0xBA,0xBB,0xBC,0xBE,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD3,0xD3,0xD5,0xD5,0xD7,0xD7,0xDD,0xD9,0xDA,0xDB,0xDC,0xDD,0xE0,0xDF, \
				0xE0,0xE2,0xE2,0xE4,0xE4,0xE6,0xE6,0xE8,0xE8,0xEA,0xEA,0xEC,0xEC,0xEE,0xEE,0xEF,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 857	/* Turkish (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0x98,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x98,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9E, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA6,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xDE,0x59,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 858	/* Multilingual Latin 1 + Euro (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 862	/* Hebrew (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 866	/* Russian (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x90,0x91,0x92,0x93,0x9d,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 874	/* Thai (OEM, Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1250 /* Central Europe (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xA3,0xB4,0xB5,0xB6,0xB7,0xB8,0xA5,0xAA,0xBB,0xBC,0xBD,0xBC,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _CODE_PAGE == 1251 /* Cyrillic (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x82,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x80,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
				0xA0,0xA2,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB2,0xA5,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xA3,0xBD,0xBD,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF}

#elif _CODE_PAGE == 1252 /* Latin 1 (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0xAd,0x9B,0x8C,0x9D,0xAE,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _CODE_PAGE == 1253 /* Greek (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xA2,0xB8,0xB9,0xBA, \
				0xE0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xFB,0xBC,0xFD,0xBF,0xFF}

#elif _CODE_PAGE == 1254 /* Turkish (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _CODE_PAGE == 1255 /* Hebrew (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1256 /* Arabic (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x8C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x41,0xE1,0x41,0xE3,0xE4,0xE5,0xE6,0x43,0x45,0x45,0x45,0x45,0xEC,0xED,0x49,0x49,0xF0,0xF1,0xF2,0xF3,0x4F,0xF5,0xF6,0xF7,0xF8,0x55,0xFA,0x55,0x55,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1257 /* Baltic (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xBC,0xBD,0xBE,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _CODE_PAGE == 1258 /* Vietnam (OEM, Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0xAC,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xEC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xFE,0x9F}

#elif _CODE_PAGE == 1	/* ASCII (for only non-LFN cfg) */
#if _USE_LFN
#error Cannot use LFN feature without valid code page.
#endif
#define _DF1S	0

#else
#error Unknown code page

#endif


/* Character code support macros */
#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))
#define IsDigit(c)	(((c)>='0')&&((c)<='9'))

#if _DF1S		/* Code page is DBCS */

#ifdef _DF2S	/* Two 1st byte areas */
#define IsDBCS1(c)	(((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E) || ((BYTE)(c) >= _DF2S && (BYTE)(c) <= _DF2E))
#else			/* One 1st byte area */
#define IsDBCS1(c)	((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E)
#endif

#ifdef _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E) || ((BYTE)(c) >= _DS3S && (BYTE)(c) <= _DS3E))
#else			/* Two 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E))
#endif

#else			/* Code page is SBCS */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* _DF1S */


/* Name status flags */
#define NSFLAG		11		/* Index of name status byte in fn[] */
#define NS_LOSS		0x01	/* Out of 8.3 format */
#define NS_LFN		0x02	/* Force to create LFN entry */
#define NS_LAST		0x04	/* Last segment */
#define NS_BODY		0x08	/* Lower case flag (body) */
#define NS_EXT		0x10	/* Lower case flag (ext) */
#define NS_DOT		0x20	/* Dot entry */


/* FAT sub-type boundaries (Differ from specs but correct for real DOS/Windows) */
#define MIN_FAT16	4086U	/* Minimum number of clusters as FAT16 */
#define	MIN_FAT32	65526U	/* Minimum number of clusters as FAT32 */


/* FatFs refers the members in the FAT structures as byte array instead of
/ structure member because the structure is not binary compatible between
/ different platforms */

#define BS_jmpBoot			0		/* x86 jump instruction (3) */
#define BS_OEMName			3		/* OEM name (8) */
#define BPB_BytsPerSec		11		/* Sector size [byte] (2) */
#define BPB_SecPerClus		13		/* Cluster size [sector] (1) */
#define BPB_RsvdSecCnt		14		/* Size of reserved area [sector] (2) */
#define BPB_NumFATs			16		/* Number of FAT copies (1) */
#define BPB_RootEntCnt		17		/* Number of root directory entries for FAT12/16 (2) */
#define BPB_TotSec16		19		/* Volume size [sector] (2) */
#define BPB_Media			21		/* Media descriptor (1) */
#define BPB_FATSz16			22		/* FAT size [sector] (2) */
#define BPB_SecPerTrk		24		/* Track size [sector] (2) */
#define BPB_NumHeads		26		/* Number of heads (2) */
#define BPB_HiddSec			28		/* Number of special hidden sectors (4) */
#define BPB_TotSec32		32		/* Volume size [sector] (4) */
#define BS_DrvNum			36		/* Physical drive number (2) */
#define BS_BootSig			38		/* Extended boot signature (1) */
#define BS_VolID			39		/* Volume serial number (4) */
#define BS_VolLab			43		/* Volume label (8) */
#define BS_FilSysType		54		/* File system type (1) */
#define BPB_FATSz32			36		/* FAT size [sector] (4) */
#define BPB_ExtFlags		40		/* Extended flags (2) */
#define BPB_FSVer			42		/* File system version (2) */
#define BPB_RootClus		44		/* Root directory first cluster (4) */
#define BPB_FSInfo			48		/* Offset of FSINFO sector (2) */
#define BPB_BkBootSec		50		/* Offset of backup boot sector (2) */
#define BS_DrvNum32			64		/* Physical drive number (2) */
#define BS_BootSig32		66		/* Extended boot signature (1) */
#define BS_VolID32			67		/* Volume serial number (4) */
#define BS_VolLab32			71		/* Volume label (8) */
#define BS_FilSysType32		82		/* File system type (1) */
#define	FSI_LeadSig			0		/* FSI: Leading signature (4) */
#define	FSI_StrucSig		484		/* FSI: Structure signature (4) */
#define	FSI_Free_Count		488		/* FSI: Number of free clusters (4) */
#define	FSI_Nxt_Free		492		/* FSI: Last allocated cluster (4) */
#define MBR_Table			446		/* MBR: Partition table offset (2) */
#define	SZ_PTE				16		/* MBR: Size of a partition table entry */
#define BS_55AA				510		/* Signature word (2) */

#define	DIR_Name			0		/* Short file name (11) */
#define	DIR_Attr			11		/* Attribute (1) */
#define	DIR_NTres			12		/* NT flag (1) */
#define DIR_CrtTimeTenth	13		/* Created time sub-second (1) */
#define	DIR_CrtTime			14		/* Created time (2) */
#define	DIR_CrtDate			16		/* Created date (2) */
#define DIR_LstAccDate		18		/* Last accessed date (2) */
#define	DIR_FstClusHI		20		/* Higher 16-bit of first cluster (2) */
#define	DIR_WrtTime			22		/* Modified time (2) */
#define	DIR_WrtDate			24		/* Modified date (2) */
#define	DIR_FstClusLO		26		/* Lower 16-bit of first cluster (2) */
#define	DIR_FileSize		28		/* File size (4) */
#define	LDIR_Ord			0		/* LFN entry order and LLE flag (1) */
#define	LDIR_Attr			11		/* LFN attribute (1) */
#define	LDIR_Type			12		/* LFN type (1) */
#define	LDIR_Chksum			13		/* Sum of corresponding SFN entry */
#define	LDIR_FstClusLO		26		/* Filled by zero (0) */
#define	SZ_DIR				32		/* Size of a directory entry */
#define	LLE					0x40	/* Last long entry flag in LDIR_Ord */
#define	DDE					0xE5	/* Deleted directory entry mark in DIR_Name[0] */
#define	NDDE				0x05	/* Replacement of the character collides with DDE */




/*------------------------------------------------------------*/
/* Module private work area                                   */
/*------------------------------------------------------------*/
/* Remark: Uninitialized variables with static duration are
/  guaranteed zero/null at start-up. If not, either the linker
/  or start-up routine being used is out of ANSI-C standard.
*/

#if _VOLUMES < 1 || _VOLUMES > 9
#error Wrong _VOLUMES setting
#endif
static FATFS *FatFs[_VOLUMES];	/* Pointer to the file system objects (logical drives) */
static WORD Fsid;				/* File system mount ID */

#if _FS_RPATH && _VOLUMES >= 2
static BYTE CurrVol;			/* Current drive */
#endif

#if _FS_LOCK
static FILESEM Files[_FS_LOCK];	/* Open object lock semaphores */
#endif

#if _USE_LFN == 0			/* Non LFN feature */
#define	DEF_NAMEBUF			BYTE sfn[12]
#define INIT_BUF(dobj)		(dobj).fn = sfn
#define	FREE_BUF()
#else
#if _MAX_LFN < 12 || _MAX_LFN > 255
#error Wrong _MAX_LFN setting
#endif
#if _USE_LFN == 1			/* LFN feature with static working buffer */
static WCHAR LfnBuf[_MAX_LFN+1];
#define	DEF_NAMEBUF			BYTE sfn[12]
#define INIT_BUF(dobj)		{ (dobj).fn = sfn; (dobj).lfn = LfnBuf; }
#define	FREE_BUF()
#elif _USE_LFN == 2 		/* LFN feature with dynamic working buffer on the stack */
#define	DEF_NAMEBUF			BYTE sfn[12]; WCHAR lbuf[_MAX_LFN+1]
#define INIT_BUF(dobj)		{ (dobj).fn = sfn; (dobj).lfn = lbuf; }
#define	FREE_BUF()
#elif _USE_LFN == 3 		/* LFN feature with dynamic working buffer on the heap */
#define	DEF_NAMEBUF			BYTE sfn[12]; WCHAR *lfn
#define INIT_BUF(dobj)		{ lfn = ff_memalloc((_MAX_LFN + 1) * 2); if (!lfn) LEAVE_FF((dobj).fs, FR_NOT_ENOUGH_CORE); (dobj).lfn = lfn; (dobj).fn = sfn; }
#define	FREE_BUF()			ff_memfree(lfn)
#else
#error Wrong _USE_LFN setting
#endif
#endif

#ifdef _EXCVT
static const BYTE ExCvt[] PROGMEM = _EXCVT;	/* Upper conversion table for extended characters */
#endif






/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* String functions                                                      */
/*-----------------------------------------------------------------------*/

DWORD get_fattime()
{
  struct gtm t;

  gettime(&t);

  /* Pack date and time into a DWORD variable */
  return ((DWORD)(t.tm_year - 80) << 25)
    | ((uint32_t)(t.tm_mon+1) << 21)
    | ((uint32_t)t.tm_mday << 16)
    | ((uint32_t)t.tm_hour << 11)
    | ((uint32_t)t.tm_min << 5)
    | ((uint32_t)t.tm_sec >> 1);
}

/* Copy memory to memory */
static
void mem_cpy (void* dst, const void* src, UINT cnt) {
	BYTE *d = (BYTE*)dst;
	const BYTE *s = (const BYTE*)src;

#if _WORD_ACCESS == 1
	while (cnt >= sizeof (int)) {
		*(int*)d = *(int*)s;
		d += sizeof (int); s += sizeof (int);
		cnt -= sizeof (int);
	}
#endif
	while (cnt--)
		*d++ = *s++;
}

/* Fill memory */
static
void mem_set (void* dst, int val, UINT cnt) {
	BYTE *d = (BYTE*)dst;

	while (cnt--)
		*d++ = (BYTE)val;
}

/* Compare memory to memory */
static
int mem_cmp (const void* dst, const void* src, UINT cnt) {
	const BYTE *d = (const BYTE *)dst, *s = (const BYTE *)src;
	int r = 0;

	while (cnt-- && (r = *d++ - *s++) == 0) ;
	return r;
}

/* Check if chr is contained in the string */
static
int chk_chr (const char* str, int chr) {
	while (*str && *str != chr) str++;
	return *str;
}




/*-----------------------------------------------------------------------*/
/* Request/Release grant to access the volume                            */
/*-----------------------------------------------------------------------*/
#if _FS_REENTRANT
static
int lock_fs (
	FATFS* fs		/* File system object */
)
{
	return ff_req_grant(fs->sobj);
}


static
void unlock_fs (
	FATFS* fs,		/* File system object */
	FRESULT res		/* Result code to be returned */
)
{
	if (fs &&
		res != FR_NOT_ENABLED &&
		res != FR_INVALID_DRIVE &&
		res != FR_INVALID_OBJECT &&
		res != FR_TIMEOUT) {
		ff_rel_grant(fs->sobj);
	}
}
#endif




/*-----------------------------------------------------------------------*/
/* File lock control functions                                           */
/*-----------------------------------------------------------------------*/
#if _FS_LOCK

static
FRESULT chk_lock (	/* Check if the file can be accessed */
	DIR* dp,		/* Directory object pointing the file to be checked */
	int acc			/* Desired access type (0:Read, 1:Write, 2:Delete/Rename) */
)
{
	UINT i, be;

	/* Search file semaphore table */
	for (i = be = 0; i < _FS_LOCK; i++) {
		if (Files[i].fs) {	/* Existing entry */
			if (Files[i].fs == dp->fs &&	 	/* Check if the object matched with an open object */
				Files[i].clu == dp->sclust &&
				Files[i].idx == dp->index) break;
		} else {			/* Blank entry */
			be = 1;
		}
	}
	if (i == _FS_LOCK)	/* The object is not opened */
		return (be || acc == 2) ? FR_OK : FR_TOO_MANY_OPEN_FILES;	/* Is there a blank entry for new object? */

	/* The object has been opened. Reject any open against writing file and all write mode open */
	return (acc || Files[i].ctr == 0x100) ? FR_LOCKED : FR_OK;
}


static
int enq_lock ()	/* Check if an entry is available for a new object */
{
	UINT i;

	for (i = 0; i < _FS_LOCK && Files[i].fs; i++) ;
	return (i == _FS_LOCK) ? 0 : 1;
}


static
UINT inc_lock (	/* Increment object open counter and returns its index (0:Internal error) */
	DIR* dp,	/* Directory object pointing the file to register or increment */
	int acc		/* Desired access (0:Read, 1:Write, 2:Delete/Rename) */
)
{
	UINT i;


	for (i = 0; i < _FS_LOCK; i++) {	/* Find the object */
		if (Files[i].fs == dp->fs &&
			Files[i].clu == dp->sclust &&
			Files[i].idx == dp->index) break;
	}

	if (i == _FS_LOCK) {				/* Not opened. Register it as new. */
		for (i = 0; i < _FS_LOCK && Files[i].fs; i++) ;
		if (i == _FS_LOCK) return 0;	/* No free entry to register (int err) */
		Files[i].fs = dp->fs;
		Files[i].clu = dp->sclust;
		Files[i].idx = dp->index;
		Files[i].ctr = 0;
	}

	if (acc && Files[i].ctr) return 0;	/* Access violation (int err) */

	Files[i].ctr = acc ? 0x100 : Files[i].ctr + 1;	/* Set semaphore value */

	return i + 1;
}


static
FRESULT dec_lock (	/* Decrement object open counter */
	UINT i			/* Semaphore index (1..) */
)
{
	WORD n;
	FRESULT res;


	if (--i < _FS_LOCK) {	/* Shift index number origin from 0 */
		n = Files[i].ctr;
		if (n == 0x100) n = 0;		/* If write mode open, delete the entry */
		if (n) n--;					/* Decrement read mode open count */
		Files[i].ctr = n;
		if (!n) Files[i].fs = 0;	/* Delete the entry if open count gets zero */
		res = FR_OK;
	} else {
		res = FR_INT_ERR;			/* Invalid index nunber */
	}
	return res;
}


static
void clear_lock (	/* Clear lock entries of the volume */
	FATFS *fs
)
{
	UINT i;

	for (i = 0; i < _FS_LOCK; i++) {
		if (Files[i].fs == fs) Files[i].fs = 0;
	}
}
#endif




/*-----------------------------------------------------------------------*/
/* Move/Flush disk access window in the file system object               */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT sync_window (
	FATFS* fs		/* File system object */
)
{
	DWORD wsect;
	UINT nf;
	FRESULT res = FR_OK;


	if (fs->wflag) {	/* Write back the sector if it is dirty */
		wsect = fs->winsect;	/* Current sector number */
		if (disk_write(fs->drv, fs->win, wsect, 1) != RES_OK) {
			res = FR_DISK_ERR;
		} else {
			fs->wflag = 0;
			if (wsect - fs->fatbase < fs->fsize) {		/* Is it in the FAT area? */
				for (nf = fs->n_fats; nf >= 2; nf--) {	/* Reflect the change to all FAT copies */
					wsect += fs->fsize;
					disk_write(fs->drv, fs->win, wsect, 1);
				}
			}
		}
	}
	return res;
}
#endif


static
FRESULT move_window (
	FATFS* fs,		/* File system object */
	DWORD sector	/* Sector number to make appearance in the fs->win[] */
)
{
	FRESULT res = FR_OK;


	if (sector != fs->winsect) {	/* Window offset changed? */
#if !_FS_READONLY
		res = sync_window(fs);		/* Write-back changes */
#endif
		if (res == FR_OK) {			/* Fill sector window with new data */
			if (disk_read(fs->drv, fs->win, sector, 1) != RES_OK) {
				sector = 0xFFFFFFFF;	/* Invalidate window if data is not reliable */
				res = FR_DISK_ERR;
			}
			fs->winsect = sector;
		}
	}
	return res;
}




/*-----------------------------------------------------------------------*/
/* Synchronize file system and strage device                             */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT sync_fs (	/* FR_OK: successful, FR_DISK_ERR: failed */
	FATFS* fs		/* File system object */
)
{
	FRESULT res;


	res = sync_window(fs);
	if (res == FR_OK) {
		/* Update FSINFO sector if needed */
		if (fs->fs_type == FS_FAT32 && fs->fsi_flag == 1) {
			/* Create FSINFO structure */
			mem_set(fs->win, 0, SS(fs));
			ST_WORD(fs->win+BS_55AA, 0xAA55);
			ST_DWORD(fs->win+FSI_LeadSig, 0x41615252);
			ST_DWORD(fs->win+FSI_StrucSig, 0x61417272);
			ST_DWORD(fs->win+FSI_Free_Count, fs->free_clust);
			ST_DWORD(fs->win+FSI_Nxt_Free, fs->last_clust);
			/* Write it into the FSINFO sector */
			fs->winsect = fs->volbase + 1;
			disk_write(fs->drv, fs->win, fs->winsect, 1);
			fs->fsi_flag = 0;
		}
		/* Make sure that no pending write process in the physical drive */
		if (disk_ioctl(fs->drv, CTRL_SYNC, 0) != RES_OK)
			res = FR_DISK_ERR;
	}

	return res;
}
#endif




/*-----------------------------------------------------------------------*/
/* Get sector# from cluster#                                             */
/*-----------------------------------------------------------------------*/
/* Hidden API for hacks and disk tools */

DWORD clust2sect (	/* !=0: Sector number, 0: Failed - invalid cluster# */
	FATFS* fs,		/* File system object */
	DWORD clst		/* Cluster# to be converted */
)
{
	clst -= 2;
	if (clst >= fs->n_fatent - 2) return 0;		/* Invalid cluster# */
	return clst * fs->csize + fs->database;
}




/*-----------------------------------------------------------------------*/
/* FAT access - Read value of a FAT entry                                */
/*-----------------------------------------------------------------------*/
/* Hidden API for hacks and disk tools */

DWORD get_fat (	/* 0xFFFFFFFF:Disk error, 1:Internal error, 2..0x0FFFFFFF:Cluster status */
	FATFS* fs,	/* File system object */
	DWORD clst	/* FAT item index (cluster#) to get the value */
)
{
	UINT wc, bc;
	BYTE *p;
	DWORD val;


	if (clst < 2 || clst >= fs->n_fatent) {	/* Check range */
		val = 1;	/* Internal error */

	} else {
		val = 0xFFFFFFFF;	/* Default value falls on disk error */

		switch (fs->fs_type) {
		case FS_FAT12 :
			bc = (UINT)clst; bc += bc / 2;
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK) break;
			wc = fs->win[bc++ % SS(fs)];
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK) break;
			wc |= fs->win[bc % SS(fs)] << 8;
			val = clst & 1 ? wc >> 4 : (wc & 0xFFF);
			break;

		case FS_FAT16 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))) != FR_OK) break;
			p = &fs->win[clst * 2 % SS(fs)];
			val = LD_WORD(p);
			break;

		case FS_FAT32 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) != FR_OK) break;
			p = &fs->win[clst * 4 % SS(fs)];
			val = LD_DWORD(p) & 0x0FFFFFFF;
			break;

		default:
			val = 1;	/* Internal error */
		}
	}

	return val;
}




/*-----------------------------------------------------------------------*/
/* FAT access - Change value of a FAT entry                              */
/*-----------------------------------------------------------------------*/
/* Hidden API for hacks and disk tools */

#if !_FS_READONLY
FRESULT put_fat (
	FATFS* fs,	/* File system object */
	DWORD clst,	/* FAT item index (cluster#) to be set */
	DWORD val	/* New value to mark the cluster */
)
{
	UINT bc;
	BYTE *p;
	FRESULT res;


	if (clst < 2 || clst >= fs->n_fatent) {	/* Check range */
		res = FR_INT_ERR;

	} else {
		switch (fs->fs_type) {
		case FS_FAT12 :
			bc = (UINT)clst; bc += bc / 2;
			res = move_window(fs, fs->fatbase + (bc / SS(fs)));
			if (res != FR_OK) break;
			p = &fs->win[bc++ % SS(fs)];
			*p = (clst & 1) ? ((*p & 0x0F) | ((BYTE)val << 4)) : (BYTE)val;
			fs->wflag = 1;
			res = move_window(fs, fs->fatbase + (bc / SS(fs)));
			if (res != FR_OK) break;
			p = &fs->win[bc % SS(fs)];
			*p = (clst & 1) ? (BYTE)(val >> 4) : ((*p & 0xF0) | ((BYTE)(val >> 8) & 0x0F));
			fs->wflag = 1;
			break;

		case FS_FAT16 :
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)));
			if (res != FR_OK) break;
			p = &fs->win[clst * 2 % SS(fs)];
			ST_WORD(p, (WORD)val);
			fs->wflag = 1;
			break;

		case FS_FAT32 :
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)));
			if (res != FR_OK) break;
			p = &fs->win[clst * 4 % SS(fs)];
			val |= LD_DWORD(p) & 0xF0000000;
			ST_DWORD(p, val);
			fs->wflag = 1;
			break;

		default :
			res = FR_INT_ERR;
		}
	}

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* FAT handling - Remove a cluster chain                                 */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT remove_chain (
	FATFS* fs,			/* File system object */
	DWORD clst			/* Cluster# to remove a chain from */
)
{
	FRESULT res;
	DWORD nxt;
#if _USE_TRIM
	DWORD scl = clst, ecl = clst, rt[2];
#endif

	if (clst < 2 || clst >= fs->n_fatent) {	/* Check range */
		res = FR_INT_ERR;

	} else {
		res = FR_OK;
		while (clst < fs->n_fatent) {			/* Not a last link? */
			nxt = get_fat(fs, clst);			/* Get cluster status */
			if (nxt == 0) break;				/* Empty cluster? */
			if (nxt == 1) { res = FR_INT_ERR; break; }	/* Internal error? */
			if (nxt == 0xFFFFFFFF) { res = FR_DISK_ERR; break; }	/* Disk error? */
			res = put_fat(fs, clst, 0);			/* Mark the cluster "empty" */
			if (res != FR_OK) break;
			if (fs->free_clust != 0xFFFFFFFF) {	/* Update FSINFO */
				fs->free_clust++;
				fs->fsi_flag |= 1;
			}
#if _USE_TRIM
			if (ecl + 1 == nxt) {	/* Is next cluster contiguous? */
				ecl = nxt;
			} else {				/* End of contiguous clusters */
				rt[0] = clust2sect(fs, scl);					/* Start sector */
				rt[1] = clust2sect(fs, ecl) + fs->csize - 1;	/* End sector */
				disk_ioctl(fs->drv, CTRL_TRIM, rt);				/* Erase the block */
				scl = ecl = nxt;
			}
#endif
			clst = nxt;	/* Next cluster */
		}
	}

	return res;
}
#endif




/*-----------------------------------------------------------------------*/
/* FAT handling - Stretch or Create a cluster chain                      */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
DWORD create_chain (	/* 0:No free cluster, 1:Internal error, 0xFFFFFFFF:Disk error, >=2:New cluster# */
	FATFS* fs,			/* File system object */
	DWORD clst			/* Cluster# to stretch. 0 means create a new chain. */
)
{
	DWORD cs, ncl, scl;
	FRESULT res;


	if (clst == 0) {		/* Create a new chain */
		scl = fs->last_clust;			/* Get suggested start point */
		if (!scl || scl >= fs->n_fatent) scl = 1;
	}
	else {					/* Stretch the current chain */
		cs = get_fat(fs, clst);			/* Check the cluster status */
		if (cs < 2) return 1;			/* Invalid value */
		if (cs == 0xFFFFFFFF) return cs;	/* A disk error occurred */
		if (cs < fs->n_fatent) return cs;	/* It is already followed by next cluster */
		scl = clst;
	}

	ncl = scl;				/* Start cluster */
	for (;;) {
		ncl++;							/* Next cluster */
		if (ncl >= fs->n_fatent) {		/* Check wrap around */
			ncl = 2;
			if (ncl > scl) return 0;	/* No free cluster */
		}
		cs = get_fat(fs, ncl);			/* Get the cluster status */
		if (cs == 0) break;				/* Found a free cluster */
		if (cs == 0xFFFFFFFF || cs == 1)/* An error occurred */
			return cs;
		if (ncl == scl) return 0;		/* No free cluster */
	}

	res = put_fat(fs, ncl, 0x0FFFFFFF);	/* Mark the new cluster "last link" */
	if (res == FR_OK && clst != 0) {
		res = put_fat(fs, clst, ncl);	/* Link it to the previous one if needed */
	}
	if (res == FR_OK) {
		fs->last_clust = ncl;			/* Update FSINFO */
		if (fs->free_clust != 0xFFFFFFFF) {
			fs->free_clust--;
			fs->fsi_flag |= 1;
		}
	} else {
		ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1;
	}

	return ncl;		/* Return new cluster number or error code */
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* FAT handling - Convert offset into cluster with link map table        */
/*-----------------------------------------------------------------------*/

#if _USE_FASTSEEK
static
DWORD clmt_clust (	/* <2:Error, >=2:Cluster number */
	FIL* fp,		/* Pointer to the file object */
	DWORD ofs		/* File offset to be converted to cluster# */
)
{
	DWORD cl, ncl, *tbl;


	tbl = fp->cltbl + 1;	/* Top of CLMT */
	cl = ofs / SS(fp->fs) / fp->fs->csize;	/* Cluster order from top of the file */
	for (;;) {
		ncl = *tbl++;			/* Number of cluters in the fragment */
		if (!ncl) return 0;		/* End of table? (error) */
		if (cl < ncl) break;	/* In this fragment? */
		cl -= ncl; tbl++;		/* Next fragment */
	}
	return cl + *tbl;	/* Return the cluster number */
}
#endif	/* _USE_FASTSEEK */




/*-----------------------------------------------------------------------*/
/* Directory handling - Set directory index                              */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_sdi (
	DIR* dp,		/* Pointer to directory object */
	UINT idx		/* Index of directory table */
)
{
	DWORD clst, sect;
	UINT ic;


	dp->index = (WORD)idx;	/* Current index */
	clst = dp->sclust;		/* Table start cluster (0:root) */
	if (clst == 1 || clst >= dp->fs->n_fatent)	/* Check start cluster range */
		return FR_INT_ERR;
	if (!clst && dp->fs->fs_type == FS_FAT32)	/* Replace cluster# 0 with root cluster# if in FAT32 */
		clst = dp->fs->dirbase;

	if (clst == 0) {	/* Static table (root-directory in FAT12/16) */
		if (idx >= dp->fs->n_rootdir)	/* Is index out of range? */
			return FR_INT_ERR;
		sect = dp->fs->dirbase;
	}
	else {				/* Dynamic table (root-directory in FAT32 or sub-directory) */
		ic = SS(dp->fs) / SZ_DIR * dp->fs->csize;	/* Entries per cluster */
		while (idx >= ic) {	/* Follow cluster chain */
			clst = get_fat(dp->fs, clst);				/* Get next cluster */
			if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
			if (clst < 2 || clst >= dp->fs->n_fatent)	/* Reached to end of table or internal error */
				return FR_INT_ERR;
			idx -= ic;
		}
		sect = clust2sect(dp->fs, clst);
	}
	dp->clust = clst;	/* Current cluster# */
	if (!sect) return FR_INT_ERR;
	dp->sect = sect + idx / (SS(dp->fs) / SZ_DIR);					/* Sector# of the directory entry */
	dp->dir = dp->fs->win + (idx % (SS(dp->fs) / SZ_DIR)) * SZ_DIR;	/* Ptr to the entry in the sector */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Directory handling - Move directory table index next                  */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_next (	/* FR_OK:Succeeded, FR_NO_FILE:End of table, FR_DENIED:Could not stretch */
	DIR* dp,		/* Pointer to the directory object */
	int stretch		/* 0: Do not stretch table, 1: Stretch table if needed */
)
{
	DWORD clst;
	UINT i;


	i = dp->index + 1;
	if (!(i & 0xFFFF) || !dp->sect)	/* Report EOT when index has reached 65535 */
		return FR_NO_FILE;

	if (!(i % (SS(dp->fs) / SZ_DIR))) {	/* Sector changed? */
		dp->sect++;					/* Next sector */

		if (!dp->clust) {		/* Static table */
			if (i >= dp->fs->n_rootdir)	/* Report EOT if it reached end of static table */
				return FR_NO_FILE;
		}
		else {					/* Dynamic table */
			if (((i / (SS(dp->fs) / SZ_DIR)) & (dp->fs->csize - 1)) == 0) {	/* Cluster changed? */
				clst = get_fat(dp->fs, dp->clust);				/* Get next cluster */
				if (clst <= 1) return FR_INT_ERR;
				if (clst == 0xFFFFFFFF) return FR_DISK_ERR;
				if (clst >= dp->fs->n_fatent) {					/* If it reached end of dynamic table, */
#if !_FS_READONLY
					UINT c;
					if (!stretch) return FR_NO_FILE;			/* If do not stretch, report EOT */
					clst = create_chain(dp->fs, dp->clust);		/* Stretch cluster chain */
					if (clst == 0) return FR_DENIED;			/* No free cluster */
					if (clst == 1) return FR_INT_ERR;
					if (clst == 0xFFFFFFFF) return FR_DISK_ERR;
					/* Clean-up stretched table */
					if (sync_window(dp->fs)) return FR_DISK_ERR;/* Flush disk access window */
					mem_set(dp->fs->win, 0, SS(dp->fs));		/* Clear window buffer */
					dp->fs->winsect = clust2sect(dp->fs, clst);	/* Cluster start sector */
					for (c = 0; c < dp->fs->csize; c++) {		/* Fill the new cluster with 0 */
						dp->fs->wflag = 1;
						if (sync_window(dp->fs)) return FR_DISK_ERR;
						dp->fs->winsect++;
					}
					dp->fs->winsect -= c;						/* Rewind window offset */
#else
					if (!stretch) return FR_NO_FILE;			/* If do not stretch, report EOT (this is to suppress warning) */
					return FR_NO_FILE;							/* Report EOT */
#endif
				}
				dp->clust = clst;				/* Initialize data for new cluster */
				dp->sect = clust2sect(dp->fs, clst);
			}
		}
	}

	dp->index = (WORD)i;	/* Current index */
	dp->dir = dp->fs->win + (i % (SS(dp->fs) / SZ_DIR)) * SZ_DIR;	/* Current entry in the window */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Directory handling - Reserve directory entry                          */
/*-----------------------------------------------------------------------*/

#if !_FS_READONLY
static
FRESULT dir_alloc (
	DIR* dp,	/* Pointer to the directory object */
	UINT nent	/* Number of contiguous entries to allocate (1-21) */
)
{
	FRESULT res;
	UINT n;


	res = dir_sdi(dp, 0);
	if (res == FR_OK) {
		n = 0;
		do {
			res = move_window(dp->fs, dp->sect);
			if (res != FR_OK) break;
			if (dp->dir[0] == DDE || dp->dir[0] == 0) {	/* Is it a free entry? */
				if (++n == nent) break;	/* A block of contiguous free entries is found */
			} else {
				n = 0;					/* Not a blank entry. Restart to search */
			}
			res = dir_next(dp, 1);		/* Next entry with table stretch enabled */
		} while (res == FR_OK);
	}
	if (res == FR_NO_FILE) res = FR_DENIED;	/* No directory entry to allocate */
	return res;
}
#endif




/*-----------------------------------------------------------------------*/
/* Directory handling - Load/Store start cluster number                  */
/*-----------------------------------------------------------------------*/

static
DWORD ld_clust (
	FATFS* fs,	/* Pointer to the fs object */
	BYTE* dir	/* Pointer to the directory entry */
)
{
	DWORD cl;

	cl = LD_WORD(dir+DIR_FstClusLO);
	if (fs->fs_type == FS_FAT32)
		cl |= (DWORD)LD_WORD(dir+DIR_FstClusHI) << 16;

	return cl;
}


#if !_FS_READONLY
static
void st_clust (
	BYTE* dir,	/* Pointer to the directory entry */
	DWORD cl	/* Value to be set */
)
{
	ST_WORD(dir+DIR_FstClusLO, cl);
	ST_WORD(dir+DIR_FstClusHI, cl >> 16);
}
#endif




/*-----------------------------------------------------------------------*/
/* LFN handling - Test/Pick/Fit an LFN segment from/to directory entry   */
/*-----------------------------------------------------------------------*/
#if _USE_LFN
static
const BYTE LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};	/* Offset of LFN characters in the directory entry */


static
int cmp_lfn (			/* 1:Matched, 0:Not matched */
	WCHAR* lfnbuf,		/* Pointer to the LFN to be compared */
	BYTE* dir			/* Pointer to the directory entry containing a part of LFN */
)
{
	UINT i, s;
	WCHAR wc, uc;


	i = ((dir[LDIR_Ord] & ~LLE) - 1) * 13;	/* Get offset in the LFN buffer */
	s = 0; wc = 1;
	do {
		uc = LD_WORD(dir+LfnOfs[s]);	/* Pick an LFN character from the entry */
		if (wc) {	/* Last character has not been processed */
			wc = ff_wtoupper(uc);		/* Convert it to upper case */
			if (i >= _MAX_LFN || wc != ff_wtoupper(lfnbuf[i++]))	/* Compare it */
				return 0;				/* Not matched */
		} else {
			if (uc != 0xFFFF) return 0;	/* Check filler */
		}
	} while (++s < 13);				/* Repeat until all characters in the entry are checked */

	if ((dir[LDIR_Ord] & LLE) && wc && lfnbuf[i])	/* Last segment matched but different length */
		return 0;

	return 1;						/* The part of LFN matched */
}



static
int pick_lfn (			/* 1:Succeeded, 0:Buffer overflow */
	WCHAR* lfnbuf,		/* Pointer to the Unicode-LFN buffer */
	BYTE* dir			/* Pointer to the directory entry */
)
{
	UINT i, s;
	WCHAR wc, uc;


	i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13;	/* Offset in the LFN buffer */

	s = 0; wc = 1;
	do {
		uc = LD_WORD(dir+LfnOfs[s]);		/* Pick an LFN character from the entry */
		if (wc) {	/* Last character has not been processed */
			if (i >= _MAX_LFN) return 0;	/* Buffer overflow? */
			lfnbuf[i++] = wc = uc;			/* Store it */
		} else {
			if (uc != 0xFFFF) return 0;		/* Check filler */
		}
	} while (++s < 13);						/* Read all character in the entry */

	if (dir[LDIR_Ord] & LLE) {				/* Put terminator if it is the last LFN part */
		if (i >= _MAX_LFN) return 0;		/* Buffer overflow? */
		lfnbuf[i] = 0;
	}

	return 1;
}


#if !_FS_READONLY
static
void fit_lfn (
	const WCHAR* lfnbuf,	/* Pointer to the LFN buffer */
	BYTE* dir,				/* Pointer to the directory entry */
	BYTE ord,				/* LFN order (1-20) */
	BYTE sum				/* SFN sum */
)
{
	UINT i, s;
	WCHAR wc;


	dir[LDIR_Chksum] = sum;			/* Set check sum */
	dir[LDIR_Attr] = AM_LFN;		/* Set attribute. LFN entry */
	dir[LDIR_Type] = 0;
	ST_WORD(dir+LDIR_FstClusLO, 0);

	i = (ord - 1) * 13;				/* Get offset in the LFN buffer */
	s = wc = 0;
	do {
		if (wc != 0xFFFF) wc = lfnbuf[i++];	/* Get an effective character */
		ST_WORD(dir+LfnOfs[s], wc);	/* Put it */
		if (!wc) wc = 0xFFFF;		/* Padding characters following last character */
	} while (++s < 13);
	if (wc == 0xFFFF || !lfnbuf[i]) ord |= LLE;	/* Bottom LFN part is the start of LFN sequence */
	dir[LDIR_Ord] = ord;			/* Set the LFN order */
}

#endif
#endif




/*-----------------------------------------------------------------------*/
/* Create numbered name                                                  */
/*-----------------------------------------------------------------------*/
#if _USE_LFN && !_FS_READONLY
static
void gen_numname (
	BYTE* dst,			/* Pointer to the buffer to store numbered SFN */
	const BYTE* src,	/* Pointer to SFN */
	const WCHAR* lfn,	/* Pointer to LFN */
	UINT seq			/* Sequence number */
)
{
	BYTE ns[8], c;
	UINT i, j;


	mem_cpy(dst, src, 11);

	if (seq > 5) {	/* On many collisions, generate a hash number instead of sequential number */
		WCHAR wc;
		DWORD sr = seq;

		while (*lfn) {	/* Create a CRC */
			wc = *lfn++;
			for (i = 0; i < 16; i++) {
				sr = (sr << 1) + (wc & 1);
				wc >>= 1;
				if (sr & 0x10000) sr ^= 0x11021;
			}
		}
		seq = (UINT)sr;
	}

	/* itoa (hexdecimal) */
	i = 7;
	do {
		c = (seq % 16) + '0';
		if (c > '9') c += 7;
		ns[i--] = c;
		seq /= 16;
	} while (seq);
	ns[i] = '~';

	/* Append the number */
	for (j = 0; j < i && dst[j] != ' '; j++) {
		if (IsDBCS1(dst[j])) {
			if (j == i - 1) break;
			j++;
		}
	}
	do {
		dst[j++] = (i < 8) ? ns[i++] : ' ';
	} while (j < 8);
}
#endif




/*-----------------------------------------------------------------------*/
/* Calculate sum of an SFN                                               */
/*-----------------------------------------------------------------------*/
#if _USE_LFN
static
BYTE sum_sfn (
	const BYTE* dir		/* Pointer to the SFN entry */
)
{
	BYTE sum = 0;
	UINT n = 11;

	do sum = (sum >> 1) + (sum << 7) + *dir++; while (--n);
	return sum;
}
#endif




/*-----------------------------------------------------------------------*/
/* Directory handling - Find an object in the directory                  */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_find (
	DIR* dp			/* Pointer to the directory object linked to the file name */
)
{
	FRESULT res;
	BYTE c, *dir;
#if _USE_LFN
	BYTE a, ord, sum;
#endif

	res = dir_sdi(dp, 0);			/* Rewind directory object */
	if (res != FR_OK) return res;

#if _USE_LFN
	ord = sum = 0xFF; dp->lfn_idx = 0xFFFF;	/* Reset LFN sequence */
#endif
	do {
		res = move_window(dp->fs, dp->sect);
		if (res != FR_OK) break;
		dir = dp->dir;					/* Ptr to the directory entry of current index */
		c = dir[DIR_Name];
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of table */
#if _USE_LFN	/* LFN configuration */
		a = dir[DIR_Attr] & AM_MASK;
		if (c == DDE || ((a & AM_VOL) && a != AM_LFN)) {	/* An entry without valid data */
			ord = 0xFF; dp->lfn_idx = 0xFFFF;	/* Reset LFN sequence */
		} else {
			if (a == AM_LFN) {			/* An LFN entry is found */
				if (dp->lfn) {
					if (c & LLE) {		/* Is it start of LFN sequence? */
						sum = dir[LDIR_Chksum];
						c &= ~LLE; ord = c;	/* LFN start order */
						dp->lfn_idx = dp->index;	/* Start index of LFN */
					}
					/* Check validity of the LFN entry and compare it with given name */
					ord = (c == ord && sum == dir[LDIR_Chksum] && cmp_lfn(dp->lfn, dir)) ? ord - 1 : 0xFF;
				}
			} else {					/* An SFN entry is found */
				if (!ord && sum == sum_sfn(dir)) break;	/* LFN matched? */
				if (!(dp->fn[NSFLAG] & NS_LOSS) && !mem_cmp(dir, dp->fn, 11)) break;	/* SFN matched? */
				ord = 0xFF; dp->lfn_idx = 0xFFFF;	/* Reset LFN sequence */
			}
		}
#else		/* Non LFN configuration */
		if (!(dir[DIR_Attr] & AM_VOL) && !mem_cmp(dir, dp->fn, 11)) /* Is it a valid entry? */
			break;
#endif
		res = dir_next(dp, 0);		/* Next entry */
	} while (res == FR_OK);

	return res;
}




/*-----------------------------------------------------------------------*/
/* Read an object from the directory                                     */
/*-----------------------------------------------------------------------*/
#if _FS_MINIMIZE <= 1 || _USE_LABEL || _FS_RPATH >= 2
static
FRESULT dir_read (
	DIR* dp,		/* Pointer to the directory object */
	int vol			/* Filtered by 0:file/directory or 1:volume label */
)
{
	FRESULT res;
	BYTE a, c, *dir;
#if _USE_LFN
	BYTE ord = 0xFF, sum = 0xFF;
#endif

	res = FR_NO_FILE;
	while (dp->sect) {
		res = move_window(dp->fs, dp->sect);
		if (res != FR_OK) break;
		dir = dp->dir;					/* Ptr to the directory entry of current index */
		c = dir[DIR_Name];
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of table */
		a = dir[DIR_Attr] & AM_MASK;
#if _USE_LFN	/* LFN configuration */
		if (c == DDE || (!_FS_RPATH && c == '.') || (int)((a & ~AM_ARC) == AM_VOL) != vol) {	/* An entry without valid data */
			ord = 0xFF;
		} else {
			if (a == AM_LFN) {			/* An LFN entry is found */
				if (c & LLE) {			/* Is it start of LFN sequence? */
					sum = dir[LDIR_Chksum];
					c &= ~LLE; ord = c;
					dp->lfn_idx = dp->index;
				}
				/* Check LFN validity and capture it */
				ord = (c == ord && sum == dir[LDIR_Chksum] && pick_lfn(dp->lfn, dir)) ? ord - 1 : 0xFF;
			} else {					/* An SFN entry is found */
				if (ord || sum != sum_sfn(dir))	/* Is there a valid LFN? */
					dp->lfn_idx = 0xFFFF;		/* It has no LFN. */
				break;
			}
		}
#else		/* Non LFN configuration */
		if (c != DDE && (_FS_RPATH || c != '.') && a != AM_LFN && (int)((a & ~AM_ARC) == AM_VOL) == vol)	/* Is it a valid entry? */
			break;
#endif
		res = dir_next(dp, 0);				/* Next entry */
		if (res != FR_OK) break;
	}

	if (res != FR_OK) dp->sect = 0;

	return res;
}
#endif	/* _FS_MINIMIZE <= 1 || _USE_LABEL || _FS_RPATH >= 2 */




/*-----------------------------------------------------------------------*/
/* Register an object to the directory                                   */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT dir_register (	/* FR_OK:Successful, FR_DENIED:No free entry or too many SFN collision, FR_DISK_ERR:Disk error */
	DIR* dp				/* Target directory with object name to be created */
)
{
	FRESULT res;
#if _USE_LFN	/* LFN configuration */
	UINT n, nent;
	BYTE sn[12], *fn, sum;
	WCHAR *lfn;


	fn = dp->fn; lfn = dp->lfn;
	mem_cpy(sn, fn, 12);

	if (_FS_RPATH && (sn[NSFLAG] & NS_DOT))		/* Cannot create dot entry */
		return FR_INVALID_NAME;

	if (sn[NSFLAG] & NS_LOSS) {			/* When LFN is out of 8.3 format, generate a numbered name */
		fn[NSFLAG] = 0; dp->lfn = 0;			/* Find only SFN */
		for (n = 1; n < 100; n++) {
			gen_numname(fn, sn, lfn, n);	/* Generate a numbered name */
			res = dir_find(dp);				/* Check if the name collides with existing SFN */
			if (res != FR_OK) break;
		}
		if (n == 100) return FR_DENIED;		/* Abort if too many collisions */
		if (res != FR_NO_FILE) return res;	/* Abort if the result is other than 'not collided' */
		fn[NSFLAG] = sn[NSFLAG]; dp->lfn = lfn;
	}

	if (sn[NSFLAG] & NS_LFN) {			/* When LFN is to be created, allocate entries for an SFN + LFNs. */
		for (n = 0; lfn[n]; n++) ;
		nent = (n + 25) / 13;
	} else {						/* Otherwise allocate an entry for an SFN  */
		nent = 1;
	}
	res = dir_alloc(dp, nent);		/* Allocate entries */

	if (res == FR_OK && --nent) {	/* Set LFN entry if needed */
		res = dir_sdi(dp, dp->index - nent);
		if (res == FR_OK) {
			sum = sum_sfn(dp->fn);	/* Sum value of the SFN tied to the LFN */
			do {					/* Store LFN entries in bottom first */
				res = move_window(dp->fs, dp->sect);
				if (res != FR_OK) break;
				fit_lfn(dp->lfn, dp->dir, (BYTE)nent, sum);
				dp->fs->wflag = 1;
				res = dir_next(dp, 0);	/* Next entry */
			} while (res == FR_OK && --nent);
		}
	}
#else	/* Non LFN configuration */
	res = dir_alloc(dp, 1);		/* Allocate an entry for SFN */
#endif

	if (res == FR_OK) {				/* Set SFN entry */
		res = move_window(dp->fs, dp->sect);
		if (res == FR_OK) {
			mem_set(dp->dir, 0, SZ_DIR);	/* Clean the entry */
			mem_cpy(dp->dir, dp->fn, 11);	/* Put SFN */
#if _USE_LFN
			dp->dir[DIR_NTres] = dp->fn[NSFLAG] & (NS_BODY | NS_EXT);	/* Put NT flag */
#endif
			dp->fs->wflag = 1;
		}
	}

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Remove an object from the directory                                   */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY && !_FS_MINIMIZE
static
FRESULT dir_remove (	/* FR_OK: Successful, FR_DISK_ERR: A disk error */
	DIR* dp				/* Directory object pointing the entry to be removed */
)
{
	FRESULT res;
#if _USE_LFN	/* LFN configuration */
	UINT i;

	i = dp->index;	/* SFN index */
	res = dir_sdi(dp, (dp->lfn_idx == 0xFFFF) ? i : dp->lfn_idx);	/* Goto the SFN or top of the LFN entries */
	if (res == FR_OK) {
		do {
			res = move_window(dp->fs, dp->sect);
			if (res != FR_OK) break;
			mem_set(dp->dir, 0, SZ_DIR);	/* Clear and mark the entry "deleted" */
			*dp->dir = DDE;
			dp->fs->wflag = 1;
			if (dp->index >= i) break;	/* When reached SFN, all entries of the object has been deleted. */
			res = dir_next(dp, 0);		/* Next entry */
		} while (res == FR_OK);
		if (res == FR_NO_FILE) res = FR_INT_ERR;
	}

#else			/* Non LFN configuration */
	res = dir_sdi(dp, dp->index);
	if (res == FR_OK) {
		res = move_window(dp->fs, dp->sect);
		if (res == FR_OK) {
			mem_set(dp->dir, 0, SZ_DIR);	/* Clear and mark the entry "deleted" */
			*dp->dir = DDE;
			dp->fs->wflag = 1;
		}
	}
#endif

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Get file information from directory entry                             */
/*-----------------------------------------------------------------------*/
#if _FS_MINIMIZE <= 1 || _FS_RPATH >= 2
static
void get_fileinfo (		/* No return code */
	DIR* dp,			/* Pointer to the directory object */
	FILINFO* fno	 	/* Pointer to the file information to be filled */
)
{
	UINT i;
	TCHAR *p, c;


	p = fno->fname;
	if (dp->sect) {		/* Get SFN */
		BYTE *dir = dp->dir;

		i = 0;
		while (i < 11) {		/* Copy name body and extension */
			c = (TCHAR)dir[i++];
			if (c == ' ') continue;			/* Skip padding spaces */
			if (c == NDDE) c = (TCHAR)DDE;	/* Restore replaced DDE character */
			if (i == 9) *p++ = '.';			/* Insert a . if extension is exist */
#if _USE_LFN
			if (IsUpper(c) && (dir[DIR_NTres] & (i >= 9 ? NS_EXT : NS_BODY)))
				c += 0x20;			/* To lower */
#if _LFN_UNICODE
			if (IsDBCS1(c) && i != 8 && i != 11 && IsDBCS2(dir[i]))
				c = c << 8 | dir[i++];
			c = ff_convert(c, 1);	/* OEM -> Unicode */
			if (!c) c = '?';
#endif
#endif
			*p++ = c;
		}
		fno->fattrib = dir[DIR_Attr];				/* Attribute */
		fno->fsize = LD_DWORD(dir+DIR_FileSize);	/* Size */
		fno->fdate = LD_WORD(dir+DIR_WrtDate);		/* Date */
		fno->ftime = LD_WORD(dir+DIR_WrtTime);		/* Time */
	}
	*p = 0;		/* Terminate SFN string by a \0 */

#if _USE_LFN
	if (fno->lfname) {
		WCHAR w, *lfn;

		i = 0; p = fno->lfname;
		if (dp->sect && fno->lfsize && dp->lfn_idx != 0xFFFF) {	/* Get LFN if available */
			lfn = dp->lfn;
			while ((w = *lfn++) != 0) {		/* Get an LFN character */
#if !_LFN_UNICODE
				w = ff_convert(w, 0);		/* Unicode -> OEM */
				if (!w) { i = 0; break; }	/* No LFN if it could not be converted */
				if (_DF1S && w >= 0x100)	/* Put 1st byte if it is a DBC (always false on SBCS cfg) */
					p[i++] = (TCHAR)(w >> 8);
#endif
				if (i >= fno->lfsize - 1) { i = 0; break; }	/* No LFN if buffer overflow */
				p[i++] = (TCHAR)w;
			}
		}
		p[i] = 0;	/* Terminate LFN string by a \0 */
	}
#endif
}
#endif /* _FS_MINIMIZE <= 1 || _FS_RPATH >= 2*/




/*-----------------------------------------------------------------------*/
/* Pick a segment and create the object name in directory form           */
/*-----------------------------------------------------------------------*/

static
FRESULT create_name (
	DIR* dp,			/* Pointer to the directory object */
	const TCHAR** path	/* Pointer to pointer to the segment in the path string */
)
{
#if _USE_LFN	/* LFN configuration */
	BYTE b, cf;
	WCHAR w, *lfn;
	UINT i, ni, si, di;
	const TCHAR *p;

	/* Create LFN in Unicode */
	for (p = *path; *p == '/' || *p == '\\'; p++) ;	/* Strip duplicated separator */
	lfn = dp->lfn;
	si = di = 0;
	for (;;) {
		w = p[si++];					/* Get a character */
		if (w < ' ' || w == '/' || w == '\\') break;	/* Break on end of segment */
		if (di >= _MAX_LFN)				/* Reject too long name */
			return FR_INVALID_NAME;
#if !_LFN_UNICODE
		w &= 0xFF;
		if (IsDBCS1(w)) {				/* Check if it is a DBC 1st byte (always false on SBCS cfg) */
			b = (BYTE)p[si++];			/* Get 2nd byte */
			if (!IsDBCS2(b))
				return FR_INVALID_NAME;	/* Reject invalid sequence */
			w = (w << 8) + b;			/* Create a DBC */
		}
		w = ff_convert(w, 1);			/* Convert ANSI/OEM to Unicode */
		if (!w) return FR_INVALID_NAME;	/* Reject invalid code */
#endif
		if (w < 0x80 && chk_chr("\"*:<>\?|\x7F", w)) /* Reject illegal characters for LFN */
			return FR_INVALID_NAME;
		lfn[di++] = w;					/* Store the Unicode character */
	}
	*path = &p[si];						/* Return pointer to the next segment */
	cf = (w < ' ') ? NS_LAST : 0;		/* Set last segment flag if end of path */
#if _FS_RPATH
	if ((di == 1 && lfn[di-1] == '.') || /* Is this a dot entry? */
		(di == 2 && lfn[di-1] == '.' && lfn[di-2] == '.')) {
		lfn[di] = 0;
		for (i = 0; i < 11; i++)
			dp->fn[i] = (i < di) ? '.' : ' ';
		dp->fn[i] = cf | NS_DOT;		/* This is a dot entry */
		return FR_OK;
	}
#endif
	while (di) {						/* Strip trailing spaces and dots */
		w = lfn[di-1];
		if (w != ' ' && w != '.') break;
		di--;
	}
	if (!di) return FR_INVALID_NAME;	/* Reject nul string */

	lfn[di] = 0;						/* LFN is created */

	/* Create SFN in directory form */
	mem_set(dp->fn, ' ', 11);
	for (si = 0; lfn[si] == ' ' || lfn[si] == '.'; si++) ;	/* Strip leading spaces and dots */
	if (si) cf |= NS_LOSS | NS_LFN;
	while (di && lfn[di - 1] != '.') di--;	/* Find extension (di<=si: no extension) */

	b = i = 0; ni = 8;
	for (;;) {
		w = lfn[si++];					/* Get an LFN character */
		if (!w) break;					/* Break on end of the LFN */
		if (w == ' ' || (w == '.' && si != di)) {	/* Remove spaces and dots */
			cf |= NS_LOSS | NS_LFN; continue;
		}

		if (i >= ni || si == di) {		/* Extension or end of SFN */
			if (ni == 11) {				/* Long extension */
				cf |= NS_LOSS | NS_LFN; break;
			}
			if (si != di) cf |= NS_LOSS | NS_LFN;	/* Out of 8.3 format */
			if (si > di) break;			/* No extension */
			si = di; i = 8; ni = 11;	/* Enter extension section */
			b <<= 2; continue;
		}

		if (w >= 0x80) {				/* Non ASCII character */
#ifdef _EXCVT
			w = ff_convert(w, 0);		/* Unicode -> OEM code */
			if (w) w = (pgm_read_byte_near(&ExCvt[w - 0x80]));	/* Convert extended character to upper (SBCS) */
#else
			w = ff_convert(ff_wtoupper(w), 0);	/* Upper converted Unicode -> OEM code */
#endif
			cf |= NS_LFN;				/* Force create LFN entry */
		}

		if (_DF1S && w >= 0x100) {		/* Double byte character (always false on SBCS cfg) */
			if (i >= ni - 1) {
				cf |= NS_LOSS | NS_LFN; i = ni; continue;
			}
			dp->fn[i++] = (BYTE)(w >> 8);
		} else {						/* Single byte character */
			if (!w || chk_chr("+,;=[]", w)) {	/* Replace illegal characters for SFN */
				w = '_'; cf |= NS_LOSS | NS_LFN;/* Lossy conversion */
			} else {
				if (IsUpper(w)) {		/* ASCII large capital */
					b |= 2;
				} else {
					if (IsLower(w)) {	/* ASCII small capital */
						b |= 1; w -= 0x20;
					}
				}
			}
		}
		dp->fn[i++] = (BYTE)w;
	}

	if (dp->fn[0] == DDE) dp->fn[0] = NDDE;	/* If the first character collides with deleted mark, replace it with 0x05 */

	if (ni == 8) b <<= 2;
	if ((b & 0x0C) == 0x0C || (b & 0x03) == 0x03)	/* Create LFN entry when there are composite capitals */
		cf |= NS_LFN;
	if (!(cf & NS_LFN)) {						/* When LFN is in 8.3 format without extended character, NT flags are created */
		if ((b & 0x03) == 0x01) cf |= NS_EXT;	/* NT flag (Extension has only small capital) */
		if ((b & 0x0C) == 0x04) cf |= NS_BODY;	/* NT flag (Filename has only small capital) */
	}

	dp->fn[NSFLAG] = cf;	/* SFN is created */

	return FR_OK;


#else	/* Non-LFN configuration */
	BYTE b, c, d, *sfn;
	UINT ni, si, i;
	const char *p;

	/* Create file name in directory form */
	for (p = *path; *p == '/' || *p == '\\'; p++) ;	/* Strip duplicated separator */
	sfn = dp->fn;
	mem_set(sfn, ' ', 11);
	si = i = b = 0; ni = 8;
#if _FS_RPATH
	if (p[si] == '.') { /* Is this a dot entry? */
		for (;;) {
			c = (BYTE)p[si++];
			if (c != '.' || si >= 3) break;
			sfn[i++] = c;
		}
		if (c != '/' && c != '\\' && c > ' ') return FR_INVALID_NAME;
		*path = &p[si];									/* Return pointer to the next segment */
		sfn[NSFLAG] = (c <= ' ') ? NS_LAST | NS_DOT : NS_DOT;	/* Set last segment flag if end of path */
		return FR_OK;
	}
#endif
	for (;;) {
		c = (BYTE)p[si++];
		if (c <= ' ' || c == '/' || c == '\\') break;	/* Break on end of segment */
		if (c == '.' || i >= ni) {
			if (ni != 8 || c != '.') return FR_INVALID_NAME;
			i = 8; ni = 11;
			b <<= 2; continue;
		}
		if (c >= 0x80) {				/* Extended character? */
			b |= 3;						/* Eliminate NT flag */
#ifdef _EXCVT
			c = (pgm_read_byte_near(&ExCvt[c - 0x80]));		/* To upper extended characters (SBCS cfg) */
#else
#if !_DF1S
			return FR_INVALID_NAME;		/* Reject extended characters (ASCII cfg) */
#endif
#endif
		}
		if (IsDBCS1(c)) {				/* Check if it is a DBC 1st byte (always false on SBCS cfg) */
			d = (BYTE)p[si++];			/* Get 2nd byte */
			if (!IsDBCS2(d) || i >= ni - 1)	/* Reject invalid DBC */
				return FR_INVALID_NAME;
			sfn[i++] = c;
			sfn[i++] = d;
		} else {						/* Single byte code */
			if (chk_chr("\"*+,:;<=>\?[]|\x7F", c))	/* Reject illegal chrs for SFN */
				return FR_INVALID_NAME;
			if (IsUpper(c)) {			/* ASCII large capital? */
				b |= 2;
			} else {
				if (IsLower(c)) {		/* ASCII small capital? */
					b |= 1; c -= 0x20;
				}
			}
			sfn[i++] = c;
		}
	}
	*path = &p[si];						/* Return pointer to the next segment */
	c = (c <= ' ') ? NS_LAST : 0;		/* Set last segment flag if end of path */

	if (!i) return FR_INVALID_NAME;		/* Reject nul string */
	if (sfn[0] == DDE) sfn[0] = NDDE;	/* When first character collides with DDE, replace it with 0x05 */

	if (ni == 8) b <<= 2;
	if ((b & 0x03) == 0x01) c |= NS_EXT;	/* NT flag (Name extension has only small capital) */
	if ((b & 0x0C) == 0x04) c |= NS_BODY;	/* NT flag (Name body has only small capital) */

	sfn[NSFLAG] = c;		/* Store NT flag, File name is created */

	return FR_OK;
#endif
}




/*-----------------------------------------------------------------------*/
/* Follow a file path                                                    */
/*-----------------------------------------------------------------------*/

static
FRESULT follow_path (	/* FR_OK(0): successful, !=0: error code */
	DIR* dp,			/* Directory object to return last directory and found object */
	const TCHAR* path	/* Full-path string to find a file or directory */
)
{
	FRESULT res;
	BYTE *dir, ns;


#if _FS_RPATH
	if (*path == '/' || *path == '\\') {	/* There is a heading separator */
		path++;	dp->sclust = 0;				/* Strip it and start from the root directory */
	} else {								/* No heading separator */
		dp->sclust = dp->fs->cdir;			/* Start from the current directory */
	}
#else
	if (*path == '/' || *path == '\\')		/* Strip heading separator if exist */
		path++;
	dp->sclust = 0;							/* Always start from the root directory */
#endif

	if ((UINT)*path < ' ') {				/* Null path name is the origin directory itself */
		res = dir_sdi(dp, 0);
		dp->dir = 0;
	} else {								/* Follow path */
		for (;;) {
			res = create_name(dp, &path);	/* Get a segment name of the path */
			if (res != FR_OK) break;
			res = dir_find(dp);				/* Find an object with the sagment name */
			ns = dp->fn[NSFLAG];
			if (res != FR_OK) {				/* Failed to find the object */
				if (res == FR_NO_FILE) {	/* Object is not found */
					if (_FS_RPATH && (ns & NS_DOT)) {	/* If dot entry is not exist, */
						dp->sclust = 0; dp->dir = 0;	/* it is the root directory and stay there */
						if (!(ns & NS_LAST)) continue;	/* Continue to follow if not last segment */
						res = FR_OK;					/* Ended at the root directroy. Function completed. */
					} else {							/* Could not find the object */
						if (!(ns & NS_LAST)) res = FR_NO_PATH;	/* Adjust error code if not last segment */
					}
				}
				break;
			}
			if (ns & NS_LAST) break;			/* Last segment matched. Function completed. */
			dir = dp->dir;						/* Follow the sub-directory */
			if (!(dir[DIR_Attr] & AM_DIR)) {	/* It is not a sub-directory and cannot follow */
				res = FR_NO_PATH; break;
			}
			dp->sclust = ld_clust(dp->fs, dir);
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Get logical drive number from path name                               */
/*-----------------------------------------------------------------------*/

static
int get_ldnumber (		/* Returns logical drive number (-1:invalid drive) */
	const TCHAR** path	/* Pointer to pointer to the path name */
)
{
	const TCHAR *tp, *tt;
	UINT i;
	int vol = -1;


	if (*path) {	/* If the pointer is not a null */
		for (tt = *path; (UINT)*tt >= (_USE_LFN ? ' ' : '!') && *tt != ':'; tt++) ;	/* Find ':' in the path */
		if (*tt == ':') {	/* If a ':' is exist in the path name */
			tp = *path;
			i = *tp++ - '0';
			if (i < 10 && tp == tt) {	/* Is there a numeric drive id? */
				if (i < _VOLUMES) {	/* If a drive id is found, get the value and strip it */
					vol = (int)i;
					*path = ++tt;
				}
			} else {	/* No numeric drive number */
#if _STR_VOLUME_ID		/* Find string drive id */
				static const char* const str[] = {_VOLUME_STRS};
				const char *sp;
				char c;
				TCHAR tc;

				i = 0; tt++;
				do {
					sp = str[i]; tp = *path;
					do {	/* Compare a string drive id with path name */
						c = *sp++; tc = *tp++;
						if (IsLower(tc)) tc -= 0x20;
					} while (c && (TCHAR)c == tc);
				} while ((c || tp != tt) && ++i < _VOLUMES);	/* Repeat for each id until pattern match */
				if (i < _VOLUMES) {	/* If a drive id is found, get the value and strip it */
					vol = (int)i;
					*path = tt;
				}
#endif
			}
			return vol;
		}
#if _FS_RPATH && _VOLUMES >= 2
		vol = CurrVol;	/* Current drive */
#else
		vol = 0;		/* Drive 0 */
#endif
	}
	return vol;
}




/*-----------------------------------------------------------------------*/
/* Load a sector and check if it is an FAT boot sector                   */
/*-----------------------------------------------------------------------*/

static
BYTE check_fs (	/* 0:FAT boor sector, 1:Valid boor sector but not FAT, 2:Not a boot sector, 3:Disk error */
	FATFS* fs,	/* File system object */
	DWORD sect	/* Sector# (lba) to check if it is an FAT boot record or not */
)
{
	fs->wflag = 0; fs->winsect = 0xFFFFFFFF;	/* Invaidate window */
	if (move_window(fs, sect) != FR_OK)			/* Load boot record */
		return 3;

	if (LD_WORD(&fs->win[BS_55AA]) != 0xAA55)	/* Check boot record signature (always placed at offset 510 even if the sector size is >512) */
		return 2;

	if ((LD_DWORD(&fs->win[BS_FilSysType]) & 0xFFFFFF) == 0x544146)		/* Check "FAT" string */
		return 0;
	if ((LD_DWORD(&fs->win[BS_FilSysType32]) & 0xFFFFFF) == 0x544146)	/* Check "FAT" string */
		return 0;

	return 1;
}




/*-----------------------------------------------------------------------*/
/* Find logical drive and check if the volume is mounted                 */
/*-----------------------------------------------------------------------*/

static
FRESULT find_volume (	/* FR_OK(0): successful, !=0: any error occurred */
	FATFS** rfs,		/* Pointer to pointer to the found file system object */
	const TCHAR** path,	/* Pointer to pointer to the path name (drive number) */
	BYTE wmode			/* !=0: Check write protection for write access */
)
{
	BYTE fmt;
	int vol;
	DSTATUS stat;
	DWORD bsect, fasize, tsect, sysect, nclst, szbfat;
	WORD nrsv;
	FATFS *fs;


	/* Get logical drive number from the path name */
	*rfs = 0;
	vol = get_ldnumber(path);
	if (vol < 0) return FR_INVALID_DRIVE;

	/* Check if the file system object is valid or not */
	fs = FatFs[vol];					/* Get pointer to the file system object */
	if (!fs) return FR_NOT_ENABLED;		/* Is the file system object available? */

	ENTER_FF(fs);						/* Lock the volume */
	*rfs = fs;							/* Return pointer to the file system object */

	if (fs->fs_type) {					/* If the volume has been mounted */
		stat = disk_status(fs->drv);
		if (!(stat & STA_NOINIT)) {		/* and the physical drive is kept initialized */
			if (!_FS_READONLY && wmode && (stat & STA_PROTECT))	/* Check write protection if needed */
				return FR_WRITE_PROTECTED;
			return FR_OK;				/* The file system object is valid */
		}
	}

	/* The file system object is not valid. */
	/* Following code attempts to mount the volume. (analyze BPB and initialize the fs object) */

	fs->fs_type = 0;					/* Clear the file system object */
	fs->drv = LD2PD(vol);				/* Bind the logical drive and a physical drive */
	stat = disk_initialize(fs->drv);	/* Initialize the physical drive */
	if (stat & STA_NOINIT)				/* Check if the initialization succeeded */
		return FR_NOT_READY;			/* Failed to initialize due to no medium or hard error */
	if (!_FS_READONLY && wmode && (stat & STA_PROTECT))	/* Check disk write protection if needed */
		return FR_WRITE_PROTECTED;
#if _MAX_SS != _MIN_SS						/* Get sector size (multiple sector size cfg only) */
	if (disk_ioctl(fs->drv, GET_SECTOR_SIZE, &SS(fs)) != RES_OK
		|| SS(fs) < _MIN_SS || SS(fs) > _MAX_SS) return FR_DISK_ERR;
#endif
	/* Find an FAT partition on the drive. Supports only generic partitioning, FDISK and SFD. */
	bsect = 0;
	fmt = check_fs(fs, bsect);					/* Load sector 0 and check if it is an FAT boot sector as SFD */
	if (fmt == 1 || (!fmt && (LD2PT(vol)))) {	/* Not an FAT boot sector or forced partition number */
		UINT i;
		DWORD br[4];

		for (i = 0; i < 4; i++) {			/* Get partition offset */
			BYTE *pt = fs->win+MBR_Table + i * SZ_PTE;
			br[i] = pt[4] ? LD_DWORD(&pt[8]) : 0;
		}
		i = LD2PT(vol);						/* Partition number: 0:auto, 1-4:forced */
		if (i) i--;
		do {								/* Find an FAT volume */
			bsect = br[i];
			fmt = bsect ? check_fs(fs, bsect) : 2;	/* Check the partition */
		} while (!LD2PT(vol) && fmt && ++i < 4);
	}
	if (fmt == 3) return FR_DISK_ERR;		/* An error occured in the disk I/O layer */
	if (fmt) return FR_NO_FILESYSTEM;		/* No FAT volume is found */

	/* An FAT volume is found. Following code initializes the file system object */

	if (LD_WORD(fs->win+BPB_BytsPerSec) != SS(fs))		/* (BPB_BytsPerSec must be equal to the physical sector size) */
		return FR_NO_FILESYSTEM;

	fasize = LD_WORD(fs->win+BPB_FATSz16);				/* Number of sectors per FAT */
	if (!fasize) fasize = LD_DWORD(fs->win+BPB_FATSz32);
	fs->fsize = fasize;

	fs->n_fats = fs->win[BPB_NumFATs];					/* Number of FAT copies */
	if (fs->n_fats != 1 && fs->n_fats != 2)				/* (Must be 1 or 2) */
		return FR_NO_FILESYSTEM;
	fasize *= fs->n_fats;								/* Number of sectors for FAT area */

	fs->csize = fs->win[BPB_SecPerClus];				/* Number of sectors per cluster */
	if (!fs->csize || (fs->csize & (fs->csize - 1)))	/* (Must be power of 2) */
		return FR_NO_FILESYSTEM;

	fs->n_rootdir = LD_WORD(fs->win+BPB_RootEntCnt);	/* Number of root directory entries */
	if (fs->n_rootdir % (SS(fs) / SZ_DIR))				/* (Must be sector aligned) */
		return FR_NO_FILESYSTEM;

	tsect = LD_WORD(fs->win+BPB_TotSec16);				/* Number of sectors on the volume */
	if (!tsect) tsect = LD_DWORD(fs->win+BPB_TotSec32);

	nrsv = LD_WORD(fs->win+BPB_RsvdSecCnt);				/* Number of reserved sectors */
	if (!nrsv) return FR_NO_FILESYSTEM;					/* (Must not be 0) */

	/* Determine the FAT sub type */
	sysect = nrsv + fasize + fs->n_rootdir / (SS(fs) / SZ_DIR);	/* RSV+FAT+DIR */
	if (tsect < sysect) return FR_NO_FILESYSTEM;		/* (Invalid volume size) */
	nclst = (tsect - sysect) / fs->csize;				/* Number of clusters */
	if (!nclst) return FR_NO_FILESYSTEM;				/* (Invalid volume size) */
	fmt = FS_FAT12;
	if (nclst >= MIN_FAT16) fmt = FS_FAT16;
	if (nclst >= MIN_FAT32) fmt = FS_FAT32;

	/* Boundaries and Limits */
	fs->n_fatent = nclst + 2;							/* Number of FAT entries */
	fs->volbase = bsect;								/* Volume start sector */
	fs->fatbase = bsect + nrsv; 						/* FAT start sector */
	fs->database = bsect + sysect;						/* Data start sector */
	if (fmt == FS_FAT32) {
		if (fs->n_rootdir) return FR_NO_FILESYSTEM;		/* (BPB_RootEntCnt must be 0) */
		fs->dirbase = LD_DWORD(fs->win+BPB_RootClus);	/* Root directory start cluster */
		szbfat = fs->n_fatent * 4;						/* (Needed FAT size) */
	} else {
		if (!fs->n_rootdir)	return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must not be 0) */
		fs->dirbase = fs->fatbase + fasize;				/* Root directory start sector */
		szbfat = (fmt == FS_FAT16) ?					/* (Needed FAT size) */
			fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
	}
	if (fs->fsize < (szbfat + (SS(fs) - 1)) / SS(fs))	/* (BPB_FATSz must not be less than the size needed) */
		return FR_NO_FILESYSTEM;

#if !_FS_READONLY
	/* Initialize cluster allocation information */
	fs->last_clust = fs->free_clust = 0xFFFFFFFF;

	/* Get fsinfo if available */
	fs->fsi_flag = 0x80;
#if (_FS_NOFSINFO & 3) != 3
	if (fmt == FS_FAT32				/* Enable FSINFO only if FAT32 and BPB_FSInfo is 1 */
		&& LD_WORD(fs->win+BPB_FSInfo) == 1
		&& move_window(fs, bsect + 1) == FR_OK)
	{
		fs->fsi_flag = 0;
		if (LD_WORD(fs->win+BS_55AA) == 0xAA55	/* Load FSINFO data if available */
			&& LD_DWORD(fs->win+FSI_LeadSig) == 0x41615252
			&& LD_DWORD(fs->win+FSI_StrucSig) == 0x61417272)
		{
#if (_FS_NOFSINFO & 1) == 0
			fs->free_clust = LD_DWORD(fs->win+FSI_Free_Count);
#endif
#if (_FS_NOFSINFO & 2) == 0
			fs->last_clust = LD_DWORD(fs->win+FSI_Nxt_Free);
#endif
		}
	}
#endif
#endif
	fs->fs_type = fmt;	/* FAT sub-type */
	fs->id = ++Fsid;	/* File system mount ID */
#if _FS_RPATH
	fs->cdir = 0;		/* Set current directory to root */
#endif
#if _FS_LOCK			/* Clear file lock semaphores */
	clear_lock(fs);
#endif

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Check if the file/directory object is valid or not                    */
/*-----------------------------------------------------------------------*/

static
FRESULT validate (	/* FR_OK(0): The object is valid, !=0: Invalid */
	void* obj		/* Pointer to the object FIL/DIR to check validity */
)
{
	FIL *fil = (FIL*)obj;	/* Assuming offset of .fs and .id in the FIL/DIR structure is identical */


	if (!fil || !fil->fs || !fil->fs->fs_type || fil->fs->id != fil->id)
		return FR_INVALID_OBJECT;

	ENTER_FF(fil->fs);		/* Lock file system */

	if (disk_status(fil->fs->drv) & STA_NOINIT)
		return FR_NOT_READY;

	return FR_OK;
}




/*--------------------------------------------------------------------------

   Public Functions

--------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Mount/Unmount a Logical Drive                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_mount (
	FATFS* fs,			/* Pointer to the file system object (NULL:unmount)*/
	const TCHAR* path,	/* Logical drive number to be mounted/unmounted */
	BYTE opt			/* 0:Do not mount (delayed mount), 1:Mount immediately */
)
{
	FATFS *cfs;
	int vol;
	FRESULT res;
	const TCHAR *rp = path;


	vol = get_ldnumber(&rp);
	if (vol < 0) return FR_INVALID_DRIVE;
	cfs = FatFs[vol];					/* Pointer to fs object */

	if (cfs) {
#if _FS_LOCK
		clear_lock(cfs);
#endif
#if _FS_REENTRANT						/* Discard sync object of the current volume */
		if (!ff_del_syncobj(cfs->sobj)) return FR_INT_ERR;
#endif
		cfs->fs_type = 0;				/* Clear old fs object */
	}

	if (fs) {
		fs->fs_type = 0;				/* Clear new fs object */
#if _FS_REENTRANT						/* Create sync object for the new volume */
		if (!ff_cre_syncobj((BYTE)vol, &fs->sobj)) return FR_INT_ERR;
#endif
	}
	FatFs[vol] = fs;					/* Register new fs object */

	if (!fs || opt != 1) return FR_OK;	/* Do not mount now, it will be mounted later */

	res = find_volume(&fs, &path, 0);	/* Force mounted the volume */
	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open (
	FIL* fp,			/* Pointer to the blank file object */
	const TCHAR* path,	/* Pointer to the file name */
	BYTE mode			/* Access mode and file open mode flags */
)
{
	FRESULT res;
	DIR dj;
	BYTE *dir;
	DEF_NAMEBUF;


	if (!fp) return FR_INVALID_OBJECT;
	fp->fs = 0;			/* Clear file object */

	/* Get logical drive number */
#if !_FS_READONLY
	mode &= FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW;
	res = find_volume(&dj.fs, &path, (BYTE)(mode & ~FA_READ));
#else
	mode &= FA_READ;
	res = find_volume(&dj.fs, &path, 0);
#endif
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);	/* Follow the file path */
		dir = dj.dir;
#if !_FS_READONLY	/* R/W configuration */
		if (res == FR_OK) {
			if (!dir)	/* Default directory itself */
				res = FR_INVALID_NAME;
#if _FS_LOCK
			else
				res = chk_lock(&dj, (mode & ~FA_READ) ? 1 : 0);
#endif
		}
		/* Create or Open a file */
		if (mode & (FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW)) {
			DWORD dw, cl;

			if (res != FR_OK) {					/* No file, create new */
				if (res == FR_NO_FILE)			/* There is no file to open, create a new entry */
#if _FS_LOCK
					res = enq_lock() ? dir_register(&dj) : FR_TOO_MANY_OPEN_FILES;
#else
					res = dir_register(&dj);
#endif
				mode |= FA_CREATE_ALWAYS;		/* File is created */
				dir = dj.dir;					/* New entry */
			}
			else {								/* Any object is already existing */
				if (dir[DIR_Attr] & (AM_RDO | AM_DIR)) {	/* Cannot overwrite it (R/O or DIR) */
					res = FR_DENIED;
				} else {
					if (mode & FA_CREATE_NEW)	/* Cannot create as new file */
						res = FR_EXIST;
				}
			}
			if (res == FR_OK && (mode & FA_CREATE_ALWAYS)) {	/* Truncate it if overwrite mode */
				dw = GET_FATTIME();				/* Created time */
				ST_DWORD(dir+DIR_CrtTime, dw);
				dir[DIR_Attr] = 0;				/* Reset attribute */
				ST_DWORD(dir+DIR_FileSize, 0);	/* size = 0 */
				cl = ld_clust(dj.fs, dir);		/* Get start cluster */
				st_clust(dir, 0);				/* cluster = 0 */
				dj.fs->wflag = 1;
				if (cl) {						/* Remove the cluster chain if exist */
					dw = dj.fs->winsect;
					res = remove_chain(dj.fs, cl);
					if (res == FR_OK) {
						dj.fs->last_clust = cl - 1;	/* Reuse the cluster hole */
						res = move_window(dj.fs, dw);
					}
				}
			}
		}
		else {	/* Open an existing file */
			if (res == FR_OK) {					/* Follow succeeded */
				if (dir[DIR_Attr] & AM_DIR) {	/* It is a directory */
					res = FR_NO_FILE;
				} else {
					if ((mode & FA_WRITE) && (dir[DIR_Attr] & AM_RDO)) /* R/O violation */
						res = FR_DENIED;
				}
			}
		}
		if (res == FR_OK) {
			if (mode & FA_CREATE_ALWAYS)		/* Set file change flag if created or overwritten */
				mode |= FA__WRITTEN;
			fp->dir_sect = dj.fs->winsect;		/* Pointer to the directory entry */
			fp->dir_ptr = dir;
#if _FS_LOCK
			fp->lockid = inc_lock(&dj, (mode & ~FA_READ) ? 1 : 0);
			if (!fp->lockid) res = FR_INT_ERR;
#endif
		}

#else				/* R/O configuration */
		if (res == FR_OK) {					/* Follow succeeded */
			dir = dj.dir;
			if (!dir) {						/* Current directory itself */
				res = FR_INVALID_NAME;
			} else {
				if (dir[DIR_Attr] & AM_DIR)	/* It is a directory */
					res = FR_NO_FILE;
			}
		}
#endif
		FREE_BUF();

		if (res == FR_OK) {
			fp->flag = mode;					/* File access mode */
			fp->err = 0;						/* Clear error flag */
			fp->sclust = ld_clust(dj.fs, dir);	/* File start cluster */
			fp->fsize = LD_DWORD(dir+DIR_FileSize);	/* File size */
			fp->fptr = 0;						/* File pointer */
			fp->dsect = 0;
#if _USE_FASTSEEK
			fp->cltbl = 0;						/* Normal seek mode */
#endif
			fp->fs = dj.fs;	 					/* Validate file object */
			fp->id = fp->fs->id;
		}
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read (
	FIL* fp, 		/* Pointer to the file object */
	void* buff,		/* Pointer to data buffer */
	UINT btr,		/* Number of bytes to read */
	UINT* br		/* Pointer to number of bytes read */
)
{
	FRESULT res;
	DWORD clst, sect, remain;
	UINT rcnt, cc;
	BYTE csect, *rbuff = (BYTE*)buff;


	*br = 0;	/* Clear read byte counter */

	res = validate(fp);							/* Check validity */
	if (res != FR_OK) LEAVE_FF(fp->fs, res);
	if (fp->err)								/* Check error */
		LEAVE_FF(fp->fs, (FRESULT)fp->err);
	if (!(fp->flag & FA_READ)) 					/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);
	remain = fp->fsize - fp->fptr;
	if (btr > remain) btr = (UINT)remain;		/* Truncate btr by remaining bytes */

	for ( ;  btr;								/* Repeat until all data read */
		rbuff += rcnt, fp->fptr += rcnt, *br += rcnt, btr -= rcnt) {
		if ((fp->fptr % SS(fp->fs)) == 0) {		/* On the sector boundary? */
			csect = (BYTE)(fp->fptr / SS(fp->fs) & (fp->fs->csize - 1));	/* Sector offset in the cluster */
			if (!csect) {						/* On the cluster boundary? */
				if (fp->fptr == 0) {			/* On the top of the file? */
					clst = fp->sclust;			/* Follow from the origin */
				} else {						/* Middle or end of the file */
#if _USE_FASTSEEK
					if (fp->cltbl)
						clst = clmt_clust(fp, fp->fptr);	/* Get cluster# from the CLMT */
					else
#endif
						clst = get_fat(fp->fs, fp->clust);	/* Follow cluster chain on the FAT */
				}
				if (clst < 2) ABORT(fp->fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF) ABORT(fp->fs, FR_DISK_ERR);
				fp->clust = clst;				/* Update current cluster */
			}
			sect = clust2sect(fp->fs, fp->clust);	/* Get current sector */
			if (!sect) ABORT(fp->fs, FR_INT_ERR);
			sect += csect;
			cc = btr / SS(fp->fs);				/* When remaining bytes >= sector size, */
			if (cc) {							/* Read maximum contiguous sectors directly */
				if (csect + cc > fp->fs->csize)	/* Clip at cluster boundary */
					cc = fp->fs->csize - csect;
				if (disk_read(fp->fs->drv, rbuff, sect, cc) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
#if !_FS_READONLY && _FS_MINIMIZE <= 2			/* Replace one of the read sectors with cached data if it contains a dirty sector */
#if _FS_TINY
				if (fp->fs->wflag && fp->fs->winsect - sect < cc)
					mem_cpy(rbuff + ((fp->fs->winsect - sect) * SS(fp->fs)), fp->fs->win, SS(fp->fs));
#else
				if ((fp->flag & FA__DIRTY) && fp->dsect - sect < cc)
					mem_cpy(rbuff + ((fp->dsect - sect) * SS(fp->fs)), fp->buf, SS(fp->fs));
#endif
#endif
				rcnt = SS(fp->fs) * cc;			/* Number of bytes transferred */
				continue;
			}
#if !_FS_TINY
			if (fp->dsect != sect) {			/* Load data sector if not in cache */
#if !_FS_READONLY
				if (fp->flag & FA__DIRTY) {		/* Write-back dirty sector cache */
					if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
						ABORT(fp->fs, FR_DISK_ERR);
					fp->flag &= ~FA__DIRTY;
				}
#endif
				if (disk_read(fp->fs->drv, fp->buf, sect, 1) != RES_OK)	/* Fill sector cache */
					ABORT(fp->fs, FR_DISK_ERR);
			}
#endif
			fp->dsect = sect;
		}
		rcnt = SS(fp->fs) - ((UINT)fp->fptr % SS(fp->fs));	/* Get partial sector data from sector buffer */
		if (rcnt > btr) rcnt = btr;
#if _FS_TINY
		if (move_window(fp->fs, fp->dsect) != FR_OK)		/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
		mem_cpy(rbuff, &fp->fs->win[fp->fptr % SS(fp->fs)], rcnt);	/* Pick partial sector */
#else
		mem_cpy(rbuff, &fp->buf[fp->fptr % SS(fp->fs)], rcnt);	/* Pick partial sector */
#endif
	}

	LEAVE_FF(fp->fs, FR_OK);
}




#if !_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Write File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_write (
	FIL* fp,			/* Pointer to the file object */
	const void *buff,	/* Pointer to the data to be written */
	UINT btw,			/* Number of bytes to write */
	UINT* bw			/* Pointer to number of bytes written */
)
{
	FRESULT res;
	DWORD clst, sect;
	UINT wcnt, cc;
	const BYTE *wbuff = (const BYTE*)buff;
	BYTE csect;


	*bw = 0;	/* Clear write byte counter */

	res = validate(fp);						/* Check validity */
	if (res != FR_OK) LEAVE_FF(fp->fs, res);
	if (fp->err)							/* Check error */
		LEAVE_FF(fp->fs, (FRESULT)fp->err);
	if (!(fp->flag & FA_WRITE))				/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);
	if (fp->fptr + btw < fp->fptr) btw = 0;	/* File size cannot reach 4GB */

	for ( ;  btw;							/* Repeat until all data written */
		wbuff += wcnt, fp->fptr += wcnt, *bw += wcnt, btw -= wcnt) {
		if ((fp->fptr % SS(fp->fs)) == 0) {	/* On the sector boundary? */
			csect = (BYTE)(fp->fptr / SS(fp->fs) & (fp->fs->csize - 1));	/* Sector offset in the cluster */
			if (!csect) {					/* On the cluster boundary? */
				if (fp->fptr == 0) {		/* On the top of the file? */
					clst = fp->sclust;		/* Follow from the origin */
					if (clst == 0)			/* When no cluster is allocated, */
						clst = create_chain(fp->fs, 0);	/* Create a new cluster chain */
				} else {					/* Middle or end of the file */
#if _USE_FASTSEEK
					if (fp->cltbl)
						clst = clmt_clust(fp, fp->fptr);	/* Get cluster# from the CLMT */
					else
#endif
						clst = create_chain(fp->fs, fp->clust);	/* Follow or stretch cluster chain on the FAT */
				}
				if (clst == 0) break;		/* Could not allocate a new cluster (disk full) */
				if (clst == 1) ABORT(fp->fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF) ABORT(fp->fs, FR_DISK_ERR);
				fp->clust = clst;			/* Update current cluster */
				if (fp->sclust == 0) fp->sclust = clst;	/* Set start cluster if the first write */
			}
#if _FS_TINY
			if (fp->fs->winsect == fp->dsect && sync_window(fp->fs))	/* Write-back sector cache */
				ABORT(fp->fs, FR_DISK_ERR);
#else
			if (fp->flag & FA__DIRTY) {		/* Write-back sector cache */
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
				fp->flag &= ~FA__DIRTY;
			}
#endif
			sect = clust2sect(fp->fs, fp->clust);	/* Get current sector */
			if (!sect) ABORT(fp->fs, FR_INT_ERR);
			sect += csect;
			cc = btw / SS(fp->fs);			/* When remaining bytes >= sector size, */
			if (cc) {						/* Write maximum contiguous sectors directly */
				if (csect + cc > fp->fs->csize)	/* Clip at cluster boundary */
					cc = fp->fs->csize - csect;
				if (disk_write(fp->fs->drv, wbuff, sect, cc) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
#if _FS_MINIMIZE <= 2
#if _FS_TINY
				if (fp->fs->winsect - sect < cc) {	/* Refill sector cache if it gets invalidated by the direct write */
					mem_cpy(fp->fs->win, wbuff + ((fp->fs->winsect - sect) * SS(fp->fs)), SS(fp->fs));
					fp->fs->wflag = 0;
				}
#else
				if (fp->dsect - sect < cc) { /* Refill sector cache if it gets invalidated by the direct write */
					mem_cpy(fp->buf, wbuff + ((fp->dsect - sect) * SS(fp->fs)), SS(fp->fs));
					fp->flag &= ~FA__DIRTY;
				}
#endif
#endif
				wcnt = SS(fp->fs) * cc;		/* Number of bytes transferred */
				continue;
			}
#if _FS_TINY
			if (fp->fptr >= fp->fsize) {	/* Avoid silly cache filling at growing edge */
				if (sync_window(fp->fs)) ABORT(fp->fs, FR_DISK_ERR);
				fp->fs->winsect = sect;
			}
#else
			if (fp->dsect != sect) {		/* Fill sector cache with file data */
				if (fp->fptr < fp->fsize &&
					disk_read(fp->fs->drv, fp->buf, sect, 1) != RES_OK)
						ABORT(fp->fs, FR_DISK_ERR);
			}
#endif
			fp->dsect = sect;
		}
		wcnt = SS(fp->fs) - ((UINT)fp->fptr % SS(fp->fs));/* Put partial sector into file I/O buffer */
		if (wcnt > btw) wcnt = btw;
#if _FS_TINY
		if (move_window(fp->fs, fp->dsect) != FR_OK)	/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
		mem_cpy(&fp->fs->win[fp->fptr % SS(fp->fs)], wbuff, wcnt);	/* Fit partial sector */
		fp->fs->wflag = 1;
#else
		mem_cpy(&fp->buf[fp->fptr % SS(fp->fs)], wbuff, wcnt);	/* Fit partial sector */
		fp->flag |= FA__DIRTY;
#endif
	}

	if (fp->fptr > fp->fsize) fp->fsize = fp->fptr;	/* Update file size if needed */
	fp->flag |= FA__WRITTEN;						/* Set file change flag */

	LEAVE_FF(fp->fs, FR_OK);
}




/*-----------------------------------------------------------------------*/
/* Synchronize the File                                                  */
/*-----------------------------------------------------------------------*/

FRESULT f_sync (
	FIL* fp		/* Pointer to the file object */
)
{
	FRESULT res;
	DWORD tm;
	BYTE *dir;


	res = validate(fp);					/* Check validity of the object */
	if (res == FR_OK) {
		if (fp->flag & FA__WRITTEN) {	/* Has the file been written? */
			/* Write-back dirty buffer */
#if !_FS_TINY
			if (fp->flag & FA__DIRTY) {
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
					LEAVE_FF(fp->fs, FR_DISK_ERR);
				fp->flag &= ~FA__DIRTY;
			}
#endif
			/* Update the directory entry */
			res = move_window(fp->fs, fp->dir_sect);
			if (res == FR_OK) {
				dir = fp->dir_ptr;
				dir[DIR_Attr] |= AM_ARC;					/* Set archive bit */
				ST_DWORD(dir+DIR_FileSize, fp->fsize);		/* Update file size */
				st_clust(dir, fp->sclust);					/* Update start cluster */
				tm = GET_FATTIME();							/* Update updated time */
				ST_DWORD(dir+DIR_WrtTime, tm);
				ST_WORD(dir+DIR_LstAccDate, 0);
				fp->flag &= ~FA__WRITTEN;
				fp->fs->wflag = 1;
				res = sync_fs(fp->fs);
			}
		}
	}

	LEAVE_FF(fp->fs, res);
}

#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close (
	FIL *fp		/* Pointer to the file object to be closed */
)
{
	FRESULT res;


#if !_FS_READONLY
	res = f_sync(fp);					/* Flush cached data */
	if (res == FR_OK)
#endif
	{
		res = validate(fp);				/* Lock volume */
		if (res == FR_OK) {
#if _FS_REENTRANT
			FATFS *fs = fp->fs;
#endif
#if _FS_LOCK
			res = dec_lock(fp->lockid);	/* Decrement file open counter */
			if (res == FR_OK)
#endif
				fp->fs = 0;				/* Invalidate file object */
#if _FS_REENTRANT
			unlock_fs(fs, FR_OK);		/* Unlock volume */
#endif
		}
#if _FS_REENTRANT
		else if (res == FR_NOT_READY) {
			unlock_fs(fp->fs, res);
		}
#endif
	}
	return res;
}




/*-----------------------------------------------------------------------*/
/* Change Current Directory or Current Drive, Get Current Directory      */
/*-----------------------------------------------------------------------*/

#if _FS_RPATH >= 1
#if _VOLUMES >= 2
FRESULT f_chdrive (
	const TCHAR* path		/* Drive number */
)
{
	int vol;


	vol = get_ldnumber(&path);
	if (vol < 0) return FR_INVALID_DRIVE;

	CurrVol = (BYTE)vol;

	return FR_OK;
}
#endif


FRESULT f_chdir (
	const TCHAR* path	/* Pointer to the directory path */
)
{
	FRESULT res;
	DIR dj;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 0);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);		/* Follow the path */
		FREE_BUF();
		if (res == FR_OK) {					/* Follow completed */
			if (!dj.dir) {
				dj.fs->cdir = dj.sclust;	/* Start directory itself */
			} else {
				if (dj.dir[DIR_Attr] & AM_DIR)	/* Reached to the directory */
					dj.fs->cdir = ld_clust(dj.fs, dj.dir);
				else
					res = FR_NO_PATH;		/* Reached but a file */
			}
		}
		if (res == FR_NO_FILE) res = FR_NO_PATH;
	}

	LEAVE_FF(dj.fs, res);
}


#if _FS_RPATH >= 2
FRESULT f_getcwd (
	TCHAR* buff,	/* Pointer to the directory path */
	UINT len		/* Size of path */
)
{
	FRESULT res;
	DIR dj;
	UINT i, n;
	DWORD ccl;
	TCHAR *tp;
	FILINFO fno;
	DEF_NAMEBUF;


	*buff = 0;
	/* Get logical drive number */
	res = find_volume(&dj.fs, (const TCHAR**)&buff, 0);	/* Get current volume */
	if (res == FR_OK) {
		INIT_BUF(dj);
		i = len;			/* Bottom of buffer (directory stack base) */
		dj.sclust = dj.fs->cdir;			/* Start to follow upper directory from current directory */
		while ((ccl = dj.sclust) != 0) {	/* Repeat while current directory is a sub-directory */
			res = dir_sdi(&dj, 1);			/* Get parent directory */
			if (res != FR_OK) break;
			res = dir_read(&dj, 0);
			if (res != FR_OK) break;
			dj.sclust = ld_clust(dj.fs, dj.dir);	/* Goto parent directory */
			res = dir_sdi(&dj, 0);
			if (res != FR_OK) break;
			do {							/* Find the entry links to the child directory */
				res = dir_read(&dj, 0);
				if (res != FR_OK) break;
				if (ccl == ld_clust(dj.fs, dj.dir)) break;	/* Found the entry */
				res = dir_next(&dj, 0);
			} while (res == FR_OK);
			if (res == FR_NO_FILE) res = FR_INT_ERR;/* It cannot be 'not found'. */
			if (res != FR_OK) break;
#if _USE_LFN
			fno.lfname = buff;
			fno.lfsize = i;
#endif
			get_fileinfo(&dj, &fno);		/* Get the directory name and push it to the buffer */
			tp = fno.fname;
#if _USE_LFN
			if (*buff) tp = buff;
#endif
			for (n = 0; tp[n]; n++) ;
			if (i < n + 3) {
				res = FR_NOT_ENOUGH_CORE; break;
			}
			while (n) buff[--i] = tp[--n];
			buff[--i] = '/';
		}
		tp = buff;
		if (res == FR_OK) {
#if _VOLUMES >= 2
			*tp++ = '0' + CurrVol;			/* Put drive number */
			*tp++ = ':';
#endif
			if (i == len) {					/* Root-directory */
				*tp++ = '/';
			} else {						/* Sub-directroy */
				do		/* Add stacked path str */
					*tp++ = buff[i++];
				while (i < len);
			}
		}
		*tp = 0;
		FREE_BUF();
	}

	LEAVE_FF(dj.fs, res);
}
#endif /* _FS_RPATH >= 2 */
#endif /* _FS_RPATH >= 1 */



#if _FS_MINIMIZE <= 2
/*-----------------------------------------------------------------------*/
/* Seek File R/W Pointer                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_lseek (
	FIL* fp,		/* Pointer to the file object */
	DWORD ofs		/* File pointer from top of file */
)
{
	FRESULT res;


	res = validate(fp);					/* Check validity of the object */
	if (res != FR_OK) LEAVE_FF(fp->fs, res);
	if (fp->err)						/* Check error */
		LEAVE_FF(fp->fs, (FRESULT)fp->err);

#if _USE_FASTSEEK
	if (fp->cltbl) {	/* Fast seek */
		DWORD cl, pcl, ncl, tcl, dsc, tlen, ulen, *tbl;

		if (ofs == CREATE_LINKMAP) {	/* Create CLMT */
			tbl = fp->cltbl;
			tlen = *tbl++; ulen = 2;	/* Given table size and required table size */
			cl = fp->sclust;			/* Top of the chain */
			if (cl) {
				do {
					/* Get a fragment */
					tcl = cl; ncl = 0; ulen += 2;	/* Top, length and used items */
					do {
						pcl = cl; ncl++;
						cl = get_fat(fp->fs, cl);
						if (cl <= 1) ABORT(fp->fs, FR_INT_ERR);
						if (cl == 0xFFFFFFFF) ABORT(fp->fs, FR_DISK_ERR);
					} while (cl == pcl + 1);
					if (ulen <= tlen) {		/* Store the length and top of the fragment */
						*tbl++ = ncl; *tbl++ = tcl;
					}
				} while (cl < fp->fs->n_fatent);	/* Repeat until end of chain */
			}
			*fp->cltbl = ulen;	/* Number of items used */
			if (ulen <= tlen)
				*tbl = 0;		/* Terminate table */
			else
				res = FR_NOT_ENOUGH_CORE;	/* Given table size is smaller than required */

		} else {						/* Fast seek */
			if (ofs > fp->fsize)		/* Clip offset at the file size */
				ofs = fp->fsize;
			fp->fptr = ofs;				/* Set file pointer */
			if (ofs) {
				fp->clust = clmt_clust(fp, ofs - 1);
				dsc = clust2sect(fp->fs, fp->clust);
				if (!dsc) ABORT(fp->fs, FR_INT_ERR);
				dsc += (ofs - 1) / SS(fp->fs) & (fp->fs->csize - 1);
				if (fp->fptr % SS(fp->fs) && dsc != fp->dsect) {	/* Refill sector cache if needed */
#if !_FS_TINY
#if !_FS_READONLY
					if (fp->flag & FA__DIRTY) {		/* Write-back dirty sector cache */
						if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
							ABORT(fp->fs, FR_DISK_ERR);
						fp->flag &= ~FA__DIRTY;
					}
#endif
					if (disk_read(fp->fs->drv, fp->buf, dsc, 1) != RES_OK)	/* Load current sector */
						ABORT(fp->fs, FR_DISK_ERR);
#endif
					fp->dsect = dsc;
				}
			}
		}
	} else
#endif

	/* Normal Seek */
	{
		DWORD clst, bcs, nsect, ifptr;

		if (ofs > fp->fsize					/* In read-only mode, clip offset with the file size */
#if !_FS_READONLY
			 && !(fp->flag & FA_WRITE)
#endif
			) ofs = fp->fsize;

		ifptr = fp->fptr;
		fp->fptr = nsect = 0;
		if (ofs) {
			bcs = (DWORD)fp->fs->csize * SS(fp->fs);	/* Cluster size (byte) */
			if (ifptr > 0 &&
				(ofs - 1) / bcs >= (ifptr - 1) / bcs) {	/* When seek to same or following cluster, */
				fp->fptr = (ifptr - 1) & ~(bcs - 1);	/* start from the current cluster */
				ofs -= fp->fptr;
				clst = fp->clust;
			} else {									/* When seek to back cluster, */
				clst = fp->sclust;						/* start from the first cluster */
#if !_FS_READONLY
				if (clst == 0) {						/* If no cluster chain, create a new chain */
					clst = create_chain(fp->fs, 0);
					if (clst == 1) ABORT(fp->fs, FR_INT_ERR);
					if (clst == 0xFFFFFFFF) ABORT(fp->fs, FR_DISK_ERR);
					fp->sclust = clst;
				}
#endif
				fp->clust = clst;
			}
			if (clst != 0) {
				while (ofs > bcs) {						/* Cluster following loop */
#if !_FS_READONLY
					if (fp->flag & FA_WRITE) {			/* Check if in write mode or not */
						clst = create_chain(fp->fs, clst);	/* Force stretch if in write mode */
						if (clst == 0) {				/* When disk gets full, clip file size */
							ofs = bcs; break;
						}
					} else
#endif
						clst = get_fat(fp->fs, clst);	/* Follow cluster chain if not in write mode */
					if (clst == 0xFFFFFFFF) ABORT(fp->fs, FR_DISK_ERR);
					if (clst <= 1 || clst >= fp->fs->n_fatent) ABORT(fp->fs, FR_INT_ERR);
					fp->clust = clst;
					fp->fptr += bcs;
					ofs -= bcs;
				}
				fp->fptr += ofs;
				if (ofs % SS(fp->fs)) {
					nsect = clust2sect(fp->fs, clst);	/* Current sector */
					if (!nsect) ABORT(fp->fs, FR_INT_ERR);
					nsect += ofs / SS(fp->fs);
				}
			}
		}
		if (fp->fptr % SS(fp->fs) && nsect != fp->dsect) {	/* Fill sector cache if needed */
#if !_FS_TINY
#if !_FS_READONLY
			if (fp->flag & FA__DIRTY) {			/* Write-back dirty sector cache */
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
				fp->flag &= ~FA__DIRTY;
			}
#endif
			if (disk_read(fp->fs->drv, fp->buf, nsect, 1) != RES_OK)	/* Fill sector cache */
				ABORT(fp->fs, FR_DISK_ERR);
#endif
			fp->dsect = nsect;
		}
#if !_FS_READONLY
		if (fp->fptr > fp->fsize) {			/* Set file change flag if the file size is extended */
			fp->fsize = fp->fptr;
			fp->flag |= FA__WRITTEN;
		}
#endif
	}

	LEAVE_FF(fp->fs, res);
}



#if _FS_MINIMIZE <= 1
/*-----------------------------------------------------------------------*/
/* Create a Directory Object                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_opendir (
	DIR* dp,			/* Pointer to directory object to create */
	const TCHAR* path	/* Pointer to the directory path */
)
{
	FRESULT res;
	FATFS* fs;
	DEF_NAMEBUF;


	if (!dp) return FR_INVALID_OBJECT;

	/* Get logical drive number */
	res = find_volume(&fs, &path, 0);
	if (res == FR_OK) {
		dp->fs = fs;
		INIT_BUF(*dp);
		res = follow_path(dp, path);			/* Follow the path to the directory */
		FREE_BUF();
		if (res == FR_OK) {						/* Follow completed */
			if (dp->dir) {						/* It is not the origin directory itself */
				if (dp->dir[DIR_Attr] & AM_DIR)	/* The object is a sub directory */
					dp->sclust = ld_clust(fs, dp->dir);
				else							/* The object is a file */
					res = FR_NO_PATH;
			}
			if (res == FR_OK) {
				dp->id = fs->id;
				res = dir_sdi(dp, 0);			/* Rewind directory */
#if _FS_LOCK
				if (res == FR_OK) {
					if (dp->sclust) {
						dp->lockid = inc_lock(dp, 0);	/* Lock the sub directory */
						if (!dp->lockid)
							res = FR_TOO_MANY_OPEN_FILES;
					} else {
						dp->lockid = 0;	/* Root directory need not to be locked */
					}
				}
#endif
			}
		}
		if (res == FR_NO_FILE) res = FR_NO_PATH;
	}
	if (res != FR_OK) dp->fs = 0;		/* Invalidate the directory object if function faild */

	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Close Directory                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_closedir (
	DIR *dp		/* Pointer to the directory object to be closed */
)
{
	FRESULT res;


	res = validate(dp);
	if (res == FR_OK) {
#if _FS_REENTRANT
		FATFS *fs = dp->fs;
#endif
#if _FS_LOCK
		if (dp->lockid)				/* Decrement sub-directory open counter */
			res = dec_lock(dp->lockid);
		if (res == FR_OK)
#endif
			dp->fs = 0;				/* Invalidate directory object */
#if _FS_REENTRANT
		unlock_fs(fs, FR_OK);		/* Unlock volume */
#endif
	}
#if _FS_REENTRANT
	else if (res == FR_NOT_READY) {
		unlock_fs(dp->fs, res);
	}
#endif
	return res;
}




/*-----------------------------------------------------------------------*/
/* Read Directory Entries in Sequence                                    */
/*-----------------------------------------------------------------------*/

FRESULT f_readdir (
	DIR* dp,			/* Pointer to the open directory object */
	FILINFO* fno		/* Pointer to file information to return */
)
{
	FRESULT res;
	DEF_NAMEBUF;


	res = validate(dp);						/* Check validity of the object */
	if (res == FR_OK) {
		if (!fno) {
			res = dir_sdi(dp, 0);			/* Rewind the directory object */
		} else {
			INIT_BUF(*dp);
			res = dir_read(dp, 0);			/* Read an item */
			if (res == FR_NO_FILE) {		/* Reached end of directory */
				dp->sect = 0;
				res = FR_OK;
			}
			if (res == FR_OK) {				/* A valid entry is found */
				get_fileinfo(dp, fno);		/* Get the object information */
				res = dir_next(dp, 0);		/* Increment index for next */
				if (res == FR_NO_FILE) {
					dp->sect = 0;
					res = FR_OK;
				}
			}
			FREE_BUF();
		}
	}

	LEAVE_FF(dp->fs, res);
}



#if _FS_MINIMIZE == 0
/*-----------------------------------------------------------------------*/
/* Get File Status                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_stat (
	const TCHAR* path,	/* Pointer to the file path */
	FILINFO* fno		/* Pointer to file information to return */
)
{
	FRESULT res;
	DIR dj;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 0);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);	/* Follow the file path */
		if (res == FR_OK) {				/* Follow completed */
			if (dj.dir) {		/* Found an object */
				if (fno) get_fileinfo(&dj, fno);
			} else {			/* It is root directory */
				res = FR_INVALID_NAME;
			}
		}
		FREE_BUF();
	}

	LEAVE_FF(dj.fs, res);
}



#if !_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Get Number of Free Clusters                                           */
/*-----------------------------------------------------------------------*/

FRESULT f_getfree (
	const TCHAR* path,	/* Path name of the logical drive number */
	DWORD* nclst,		/* Pointer to a variable to return number of free clusters */
	FATFS** fatfs		/* Pointer to return pointer to corresponding file system object */
)
{
	FRESULT res;
	FATFS *fs;
	DWORD n, clst, sect, stat;
	UINT i;
	BYTE fat, *p;


	/* Get logical drive number */
	res = find_volume(fatfs, &path, 0);
	fs = *fatfs;
	if (res == FR_OK) {
		/* If free_clust is valid, return it without full cluster scan */
		if (fs->free_clust <= fs->n_fatent - 2) {
			*nclst = fs->free_clust;
		} else {
			/* Get number of free clusters */
			fat = fs->fs_type;
			n = 0;
			if (fat == FS_FAT12) {
				clst = 2;
				do {
					stat = get_fat(fs, clst);
					if (stat == 0xFFFFFFFF) { res = FR_DISK_ERR; break; }
					if (stat == 1) { res = FR_INT_ERR; break; }
					if (stat == 0) n++;
				} while (++clst < fs->n_fatent);
			} else {
				clst = fs->n_fatent;
				sect = fs->fatbase;
				i = 0; p = 0;
				do {
					if (!i) {
						res = move_window(fs, sect++);
						if (res != FR_OK) break;
						p = fs->win;
						i = SS(fs);
					}
					if (fat == FS_FAT16) {
						if (LD_WORD(p) == 0) n++;
						p += 2; i -= 2;
					} else {
						if ((LD_DWORD(p) & 0x0FFFFFFF) == 0) n++;
						p += 4; i -= 4;
					}
				} while (--clst);
			}
			fs->free_clust = n;
			fs->fsi_flag |= 1;
			*nclst = n;
		}
	}
	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Truncate File                                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_truncate (
	FIL* fp		/* Pointer to the file object */
)
{
	FRESULT res;
	DWORD ncl;


	res = validate(fp);						/* Check validity of the object */
	if (res == FR_OK) {
		if (fp->err) {						/* Check error */
			res = (FRESULT)fp->err;
		} else {
			if (!(fp->flag & FA_WRITE))		/* Check access mode */
				res = FR_DENIED;
		}
	}
	if (res == FR_OK) {
		if (fp->fsize > fp->fptr) {
			fp->fsize = fp->fptr;	/* Set file size to current R/W point */
			fp->flag |= FA__WRITTEN;
			if (fp->fptr == 0) {	/* When set file size to zero, remove entire cluster chain */
				res = remove_chain(fp->fs, fp->sclust);
				fp->sclust = 0;
			} else {				/* When truncate a part of the file, remove remaining clusters */
				ncl = get_fat(fp->fs, fp->clust);
				res = FR_OK;
				if (ncl == 0xFFFFFFFF) res = FR_DISK_ERR;
				if (ncl == 1) res = FR_INT_ERR;
				if (res == FR_OK && ncl < fp->fs->n_fatent) {
					res = put_fat(fp->fs, fp->clust, 0x0FFFFFFF);
					if (res == FR_OK) res = remove_chain(fp->fs, ncl);
				}
			}
#if !_FS_TINY
			if (res == FR_OK && (fp->flag & FA__DIRTY)) {
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK)
					res = FR_DISK_ERR;
				else
					fp->flag &= ~FA__DIRTY;
			}
#endif
		}
		if (res != FR_OK) fp->err = (FRESULT)res;
	}

	LEAVE_FF(fp->fs, res);
}




/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_unlink (
	const TCHAR* path		/* Pointer to the file or directory path */
)
{
	FRESULT res;
	DIR dj, sdj;
	BYTE *dir;
	DWORD dclst = 0;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 1);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);		/* Follow the file path */
		if (_FS_RPATH && res == FR_OK && (dj.fn[NSFLAG] & NS_DOT))
			res = FR_INVALID_NAME;			/* Cannot remove dot entry */
#if _FS_LOCK
		if (res == FR_OK) res = chk_lock(&dj, 2);	/* Cannot remove open object */
#endif
		if (res == FR_OK) {					/* The object is accessible */
			dir = dj.dir;
			if (!dir) {
				res = FR_INVALID_NAME;		/* Cannot remove the origin directory */
			} else {
				if (dir[DIR_Attr] & AM_RDO)
					res = FR_DENIED;		/* Cannot remove R/O object */
			}
			if (res == FR_OK) {
				dclst = ld_clust(dj.fs, dir);
				if (dir[DIR_Attr] & AM_DIR) {   /* Is it a sub-dir? */
					if (!dclst) {
						res = FR_INT_ERR;
					} else {					/* Make sure the sub-directory is empty */
						mem_cpy(&sdj, &dj, sizeof (DIR));
						sdj.sclust = dclst;
						res = dir_sdi(&sdj, 2);		/* Exclude dot entries */
						if (res == FR_OK) {
							res = dir_read(&sdj, 0);	/* Read an item */
							if (res == FR_OK		/* Not empty directory */
#if _FS_RPATH
							|| dclst == dj.fs->cdir	/* or current directory */
#endif
							) res = FR_DENIED;
							if (res == FR_NO_FILE) res = FR_OK;	/* It is empty */
						}
					}
				}
			}
			if (res == FR_OK) {
				res = dir_remove(&dj);		/* Remove the directory entry */
				if (res == FR_OK && dclst)	/* Remove the cluster chain if exist */
					res = remove_chain(dj.fs, dclst);
				if (res == FR_OK) res = sync_fs(dj.fs);
			}
		}
		FREE_BUF();
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Create a Directory                                                    */
/*-----------------------------------------------------------------------*/

FRESULT f_mkdir (
	const TCHAR* path		/* Pointer to the directory path */
)
{
	FRESULT res;
	DIR dj;
	BYTE *dir, n;
	DWORD dsc, dcl, pcl, tm = GET_FATTIME();
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 1);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);			/* Follow the file path */
		if (res == FR_OK) res = FR_EXIST;		/* Any object with same name is already existing */
		if (_FS_RPATH && res == FR_NO_FILE && (dj.fn[NSFLAG] & NS_DOT))
			res = FR_INVALID_NAME;
		if (res == FR_NO_FILE) {				/* Can create a new directory */
			dcl = create_chain(dj.fs, 0);		/* Allocate a cluster for the new directory table */
			res = FR_OK;
			if (dcl == 0) res = FR_DENIED;		/* No space to allocate a new cluster */
			if (dcl == 1) res = FR_INT_ERR;
			if (dcl == 0xFFFFFFFF) res = FR_DISK_ERR;
			if (res == FR_OK)					/* Flush FAT */
				res = sync_window(dj.fs);
			if (res == FR_OK) {					/* Initialize the new directory table */
				dsc = clust2sect(dj.fs, dcl);
				dir = dj.fs->win;
				mem_set(dir, 0, SS(dj.fs));
				mem_set(dir+DIR_Name, ' ', 11);	/* Create "." entry */
				dir[DIR_Name] = '.';
				dir[DIR_Attr] = AM_DIR;
				ST_DWORD(dir+DIR_WrtTime, tm);
				st_clust(dir, dcl);
				mem_cpy(dir+SZ_DIR, dir, SZ_DIR); 	/* Create ".." entry */
				dir[SZ_DIR+1] = '.'; pcl = dj.sclust;
				if (dj.fs->fs_type == FS_FAT32 && pcl == dj.fs->dirbase)
					pcl = 0;
				st_clust(dir+SZ_DIR, pcl);
				for (n = dj.fs->csize; n; n--) {	/* Write dot entries and clear following sectors */
					dj.fs->winsect = dsc++;
					dj.fs->wflag = 1;
					res = sync_window(dj.fs);
					if (res != FR_OK) break;
					mem_set(dir, 0, SS(dj.fs));
				}
			}
			if (res == FR_OK) res = dir_register(&dj);	/* Register the object to the directoy */
			if (res != FR_OK) {
				remove_chain(dj.fs, dcl);			/* Could not register, remove cluster chain */
			} else {
				dir = dj.dir;
				dir[DIR_Attr] = AM_DIR;				/* Attribute */
				ST_DWORD(dir+DIR_WrtTime, tm);		/* Created time */
				st_clust(dir, dcl);					/* Table start cluster */
				dj.fs->wflag = 1;
				res = sync_fs(dj.fs);
			}
		}
		FREE_BUF();
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Change Attribute                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_chmod (
	const TCHAR* path,	/* Pointer to the file path */
	BYTE value,			/* Attribute bits */
	BYTE mask			/* Attribute mask to change */
)
{
	FRESULT res;
	DIR dj;
	BYTE *dir;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 1);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);		/* Follow the file path */
		FREE_BUF();
		if (_FS_RPATH && res == FR_OK && (dj.fn[NSFLAG] & NS_DOT))
			res = FR_INVALID_NAME;
		if (res == FR_OK) {
			dir = dj.dir;
			if (!dir) {						/* Is it a root directory? */
				res = FR_INVALID_NAME;
			} else {						/* File or sub directory */
				mask &= AM_RDO|AM_HID|AM_SYS|AM_ARC;	/* Valid attribute mask */
				dir[DIR_Attr] = (value & mask) | (dir[DIR_Attr] & (BYTE)~mask);	/* Apply attribute change */
				dj.fs->wflag = 1;
				res = sync_fs(dj.fs);
			}
		}
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Rename File/Directory                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_rename (
	const TCHAR* path_old,	/* Pointer to the object to be renamed */
	const TCHAR* path_new	/* Pointer to the new name */
)
{
	FRESULT res;
	DIR djo, djn;
	BYTE buf[21], *dir;
	DWORD dw;
	DEF_NAMEBUF;


	/* Get logical drive number of the source object */
	res = find_volume(&djo.fs, &path_old, 1);
	if (res == FR_OK) {
		djn.fs = djo.fs;
		INIT_BUF(djo);
		res = follow_path(&djo, path_old);		/* Check old object */
		if (_FS_RPATH && res == FR_OK && (djo.fn[NSFLAG] & NS_DOT))
			res = FR_INVALID_NAME;
#if _FS_LOCK
		if (res == FR_OK) res = chk_lock(&djo, 2);
#endif
		if (res == FR_OK) {						/* Old object is found */
			if (!djo.dir) {						/* Is root dir? */
				res = FR_NO_FILE;
			} else {
				mem_cpy(buf, djo.dir+DIR_Attr, 21);		/* Save the object information except name */
				mem_cpy(&djn, &djo, sizeof (DIR));		/* Duplicate the directory object */
				if (get_ldnumber(&path_new) >= 0)		/* Snip drive number off and ignore it */
					res = follow_path(&djn, path_new);	/* and make sure if new object name is not conflicting */
				else
					res = FR_INVALID_DRIVE;
				if (res == FR_OK) res = FR_EXIST;		/* The new object name is already existing */
				if (res == FR_NO_FILE) { 				/* It is a valid path and no name collision */
/* Start of critical section that any interruption can cause a cross-link */
					res = dir_register(&djn);			/* Register the new entry */
					if (res == FR_OK) {
						dir = djn.dir;					/* Copy object information except name */
						mem_cpy(dir+13, buf+2, 19);
						dir[DIR_Attr] = buf[0] | AM_ARC;
						djo.fs->wflag = 1;
						if ((dir[DIR_Attr] & AM_DIR) && djo.sclust != djn.sclust) {	/* Update .. entry in the directory if needed */
							dw = clust2sect(djo.fs, ld_clust(djo.fs, dir));
							if (!dw) {
								res = FR_INT_ERR;
							} else {
								res = move_window(djo.fs, dw);
								dir = djo.fs->win+SZ_DIR;	/* .. entry */
								if (res == FR_OK && dir[1] == '.') {
									st_clust(dir, djn.sclust);
									djo.fs->wflag = 1;
								}
							}
						}
						if (res == FR_OK) {
							res = dir_remove(&djo);		/* Remove old entry */
							if (res == FR_OK)
								res = sync_fs(djo.fs);
						}
					}
/* End of critical section */
				}
			}
		}
		FREE_BUF();
	}

	LEAVE_FF(djo.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Change Timestamp                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_utime (
	const TCHAR* path,	/* Pointer to the file/directory name */
	const FILINFO* fno	/* Pointer to the time stamp to be set */
)
{
	FRESULT res;
	DIR dj;
	BYTE *dir;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 1);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);	/* Follow the file path */
		FREE_BUF();
		if (_FS_RPATH && res == FR_OK && (dj.fn[NSFLAG] & NS_DOT))
			res = FR_INVALID_NAME;
		if (res == FR_OK) {
			dir = dj.dir;
			if (!dir) {					/* Root directory */
				res = FR_INVALID_NAME;
			} else {					/* File or sub-directory */
				ST_WORD(dir+DIR_WrtTime, fno->ftime);
				ST_WORD(dir+DIR_WrtDate, fno->fdate);
				dj.fs->wflag = 1;
				res = sync_fs(dj.fs);
			}
		}
	}

	LEAVE_FF(dj.fs, res);
}

#endif /* !_FS_READONLY */
#endif /* _FS_MINIMIZE == 0 */
#endif /* _FS_MINIMIZE <= 1 */
#endif /* _FS_MINIMIZE <= 2 */



#if _USE_LABEL
/*-----------------------------------------------------------------------*/
/* Get volume label                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_getlabel (
	const TCHAR* path,	/* Path name of the logical drive number */
	TCHAR* label,		/* Pointer to a buffer to return the volume label */
	DWORD* vsn			/* Pointer to a variable to return the volume serial number */
)
{
	FRESULT res;
	DIR dj;
	UINT i, j;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 0);

	/* Get volume label */
	if (res == FR_OK && label) {
		dj.sclust = 0;					/* Open root directory */
		res = dir_sdi(&dj, 0);
		if (res == FR_OK) {
			res = dir_read(&dj, 1);		/* Get an entry with AM_VOL */
			if (res == FR_OK) {			/* A volume label is exist */
#if _USE_LFN && _LFN_UNICODE
				WCHAR w;
				i = j = 0;
				do {
					w = (i < 11) ? dj.dir[i++] : ' ';
					if (IsDBCS1(w) && i < 11 && IsDBCS2(dj.dir[i]))
						w = w << 8 | dj.dir[i++];
					label[j++] = ff_convert(w, 1);	/* OEM -> Unicode */
				} while (j < 11);
#else
				mem_cpy(label, dj.dir, 11);
#endif
				j = 11;
				do {
					label[j] = 0;
					if (!j) break;
				} while (label[--j] == ' ');
			}
			if (res == FR_NO_FILE) {	/* No label, return nul string */
				label[0] = 0;
				res = FR_OK;
			}
		}
	}

	/* Get volume serial number */
	if (res == FR_OK && vsn) {
		res = move_window(dj.fs, dj.fs->volbase);
		if (res == FR_OK) {
			i = dj.fs->fs_type == FS_FAT32 ? BS_VolID32 : BS_VolID;
			*vsn = LD_DWORD(&dj.fs->win[i]);
		}
	}

	LEAVE_FF(dj.fs, res);
}



#if !_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Set volume label                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_setlabel (
	const TCHAR* label	/* Pointer to the volume label to set */
)
{
	FRESULT res;
	DIR dj;
	BYTE vn[11];
	UINT i, j, sl;
	WCHAR w;
	DWORD tm;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &label, 1);
	if (res) LEAVE_FF(dj.fs, res);

	/* Create a volume label in directory form */
	vn[0] = 0;
	for (sl = 0; label[sl]; sl++) ;				/* Get name length */
	for ( ; sl && label[sl-1] == ' '; sl--) ;	/* Remove trailing spaces */
	if (sl) {	/* Create volume label in directory form */
		i = j = 0;
		do {
#if _USE_LFN && _LFN_UNICODE
			w = ff_convert(ff_wtoupper(label[i++]), 0);
#else
			w = (BYTE)label[i++];
			if (IsDBCS1(w))
				w = (j < 10 && i < sl && IsDBCS2(label[i])) ? w << 8 | (BYTE)label[i++] : 0;
#if _USE_LFN
			w = ff_convert(ff_wtoupper(ff_convert(w, 1)), 0);
#else
			if (IsLower(w)) w -= 0x20;			/* To upper ASCII characters */
#ifdef _EXCVT
			if (w >= 0x80) w = (pgm_read_byte_near(&ExCvt[w - 0x80]));	/* To upper extended characters (SBCS cfg) */
#else
			if (!_DF1S && w >= 0x80) w = 0;		/* Reject extended characters (ASCII cfg) */
#endif
#endif
#endif
			if (!w || chk_chr("\"*+,.:;<=>\?[]|\x7F", w) || j >= (UINT)((w >= 0x100) ? 10 : 11)) /* Reject invalid characters for volume label */
				LEAVE_FF(dj.fs, FR_INVALID_NAME);
			if (w >= 0x100) vn[j++] = (BYTE)(w >> 8);
			vn[j++] = (BYTE)w;
		} while (i < sl);
		while (j < 11) vn[j++] = ' ';
	}

	/* Set volume label */
	dj.sclust = 0;					/* Open root directory */
	res = dir_sdi(&dj, 0);
	if (res == FR_OK) {
		res = dir_read(&dj, 1);		/* Get an entry with AM_VOL */
		if (res == FR_OK) {			/* A volume label is found */
			if (vn[0]) {
				mem_cpy(dj.dir, vn, 11);	/* Change the volume label name */
				tm = GET_FATTIME();
				ST_DWORD(dj.dir+DIR_WrtTime, tm);
			} else {
				dj.dir[0] = DDE;			/* Remove the volume label */
			}
			dj.fs->wflag = 1;
			res = sync_fs(dj.fs);
		} else {					/* No volume label is found or error */
			if (res == FR_NO_FILE) {
				res = FR_OK;
				if (vn[0]) {				/* Create volume label as new */
					res = dir_alloc(&dj, 1);	/* Allocate an entry for volume label */
					if (res == FR_OK) {
						mem_set(dj.dir, 0, SZ_DIR);	/* Set volume label */
						mem_cpy(dj.dir, vn, 11);
						dj.dir[DIR_Attr] = AM_VOL;
						tm = GET_FATTIME();
						ST_DWORD(dj.dir+DIR_WrtTime, tm);
						dj.fs->wflag = 1;
						res = sync_fs(dj.fs);
					}
				}
			}
		}
	}

	LEAVE_FF(dj.fs, res);
}

#endif /* !_FS_READONLY */
#endif /* _USE_LABEL */



/*-----------------------------------------------------------------------*/
/* Forward data to the stream directly (available on only tiny cfg)      */
/*-----------------------------------------------------------------------*/
#if _USE_FORWARD && _FS_TINY

FRESULT f_forward (
	FIL* fp, 						/* Pointer to the file object */
	UINT (*func)(const BYTE*,UINT),	/* Pointer to the streaming function */
	UINT btf,						/* Number of bytes to forward */
	UINT* bf						/* Pointer to number of bytes forwarded */
)
{
	FRESULT res;
	DWORD remain, clst, sect;
	UINT rcnt;
	BYTE csect;


	*bf = 0;	/* Clear transfer byte counter */

	res = validate(fp);								/* Check validity of the object */
	if (res != FR_OK) LEAVE_FF(fp->fs, res);
	if (fp->err)									/* Check error */
		LEAVE_FF(fp->fs, (FRESULT)fp->err);
	if (!(fp->flag & FA_READ))						/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);

	remain = fp->fsize - fp->fptr;
	if (btf > remain) btf = (UINT)remain;			/* Truncate btf by remaining bytes */

	for ( ;  btf && (*func)(0, 0);					/* Repeat until all data transferred or stream becomes busy */
		fp->fptr += rcnt, *bf += rcnt, btf -= rcnt) {
		csect = (BYTE)(fp->fptr / SS(fp->fs) & (fp->fs->csize - 1));	/* Sector offset in the cluster */
		if ((fp->fptr % SS(fp->fs)) == 0) {			/* On the sector boundary? */
			if (!csect) {							/* On the cluster boundary? */
				clst = (fp->fptr == 0) ?			/* On the top of the file? */
					fp->sclust : get_fat(fp->fs, fp->clust);
				if (clst <= 1) ABORT(fp->fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF) ABORT(fp->fs, FR_DISK_ERR);
				fp->clust = clst;					/* Update current cluster */
			}
		}
		sect = clust2sect(fp->fs, fp->clust);		/* Get current data sector */
		if (!sect) ABORT(fp->fs, FR_INT_ERR);
		sect += csect;
		if (move_window(fp->fs, sect) != FR_OK)		/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
		fp->dsect = sect;
		rcnt = SS(fp->fs) - (WORD)(fp->fptr % SS(fp->fs));	/* Forward data from sector window */
		if (rcnt > btf) rcnt = btf;
		rcnt = (*func)(&fp->fs->win[(WORD)fp->fptr % SS(fp->fs)], rcnt);
		if (!rcnt) ABORT(fp->fs, FR_INT_ERR);
	}

	LEAVE_FF(fp->fs, FR_OK);
}
#endif /* _USE_FORWARD */



#if _USE_MKFS && !_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Create File System on the Drive                                       */
/*-----------------------------------------------------------------------*/
#define N_ROOTDIR	512		/* Number of root directory entries for FAT12/16 */
#define N_FATS		1		/* Number of FATs (1 or 2) */


FRESULT f_mkfs (
	const TCHAR* path,	/* Logical drive number */
	BYTE sfd,			/* Partitioning rule 0:FDISK, 1:SFD */
	UINT au				/* Size of allocation unit in unit of byte or sector */
)
{
	static const WORD vst[] = { 1024,   512,  256,  128,   64,    32,   16,    8,    4,    2,   0};
	static const WORD cst[] = {32768, 16384, 8192, 4096, 2048, 16384, 8192, 4096, 2048, 1024, 512};
	int vol;
	BYTE fmt, md, sys, *tbl, pdrv, part;
	DWORD n_clst, vs, n, wsect;
	UINT i;
	DWORD b_vol, b_fat, b_dir, b_data;	/* LBA */
	DWORD n_vol, n_rsv, n_fat, n_dir;	/* Size */
	FATFS *fs;
	DSTATUS stat;


	/* Check mounted drive and clear work area */
	if (sfd > 1) return FR_INVALID_PARAMETER;
	vol = get_ldnumber(&path);
	if (vol < 0) return FR_INVALID_DRIVE;
	fs = FatFs[vol];
	if (!fs) return FR_NOT_ENABLED;
	fs->fs_type = 0;
	pdrv = LD2PD(vol);	/* Physical drive */
	part = LD2PT(vol);	/* Partition (0:auto detect, 1-4:get from partition table)*/

	/* Get disk statics */
	stat = disk_initialize(pdrv);
	if (stat & STA_NOINIT) return FR_NOT_READY;
	if (stat & STA_PROTECT) return FR_WRITE_PROTECTED;
#if _MAX_SS != _MIN_SS		/* Get disk sector size */
	if (disk_ioctl(pdrv, GET_SECTOR_SIZE, &SS(fs)) != RES_OK || SS(fs) > _MAX_SS || SS(fs) < _MIN_SS)
		return FR_DISK_ERR;
#endif
	if (_MULTI_PARTITION && part) {
		/* Get partition information from partition table in the MBR */
		if (disk_read(pdrv, fs->win, 0, 1) != RES_OK) return FR_DISK_ERR;
		if (LD_WORD(fs->win+BS_55AA) != 0xAA55) return FR_MKFS_ABORTED;
		tbl = &fs->win[MBR_Table + (part - 1) * SZ_PTE];
		if (!tbl[4]) return FR_MKFS_ABORTED;	/* No partition? */
		b_vol = LD_DWORD(tbl+8);	/* Volume start sector */
		n_vol = LD_DWORD(tbl+12);	/* Volume size */
	} else {
		/* Create a partition in this function */
		if (disk_ioctl(pdrv, GET_SECTOR_COUNT, &n_vol) != RES_OK || n_vol < 128)
			return FR_DISK_ERR;
		b_vol = (sfd) ? 0 : 63;		/* Volume start sector */
		n_vol -= b_vol;				/* Volume size */
	}

	if (au & (au - 1)) au = 0;
	if (!au) {						/* AU auto selection */
		vs = n_vol / (2000 / (SS(fs) / 512));
		for (i = 0; vs < vst[i]; i++) ;
		au = cst[i];
	}
	if (au >= _MIN_SS) au /= SS(fs);	/* Number of sectors per cluster */
	if (!au) au = 1;
	if (au > 128) au = 128;

	/* Pre-compute number of clusters and FAT sub-type */
	n_clst = n_vol / au;
	fmt = FS_FAT12;
	if (n_clst >= MIN_FAT16) fmt = FS_FAT16;
	if (n_clst >= MIN_FAT32) fmt = FS_FAT32;

	/* Determine offset and size of FAT structure */
	if (fmt == FS_FAT32) {
		n_fat = ((n_clst * 4) + 8 + SS(fs) - 1) / SS(fs);
		n_rsv = 32;
		n_dir = 0;
	} else {
		n_fat = (fmt == FS_FAT12) ? (n_clst * 3 + 1) / 2 + 3 : (n_clst * 2) + 4;
		n_fat = (n_fat + SS(fs) - 1) / SS(fs);
		n_rsv = 1;
		n_dir = (DWORD)N_ROOTDIR * SZ_DIR / SS(fs);
	}
	b_fat = b_vol + n_rsv;				/* FAT area start sector */
	b_dir = b_fat + n_fat * N_FATS;		/* Directory area start sector */
	b_data = b_dir + n_dir;				/* Data area start sector */
	if (n_vol < b_data + au - b_vol) return FR_MKFS_ABORTED;	/* Too small volume */

	/* Align data start sector to erase block boundary (for flash memory media) */
	if (disk_ioctl(pdrv, GET_BLOCK_SIZE, &n) != RES_OK || !n || n > 32768) n = 1;
	n = (b_data + n - 1) & ~(n - 1);	/* Next nearest erase block from current data start */
	n = (n - b_data) / N_FATS;
	if (fmt == FS_FAT32) {		/* FAT32: Move FAT offset */
		n_rsv += n;
		b_fat += n;
	} else {					/* FAT12/16: Expand FAT size */
		n_fat += n;
	}

	/* Determine number of clusters and final check of validity of the FAT sub-type */
	n_clst = (n_vol - n_rsv - n_fat * N_FATS - n_dir) / au;
	if (   (fmt == FS_FAT16 && n_clst < MIN_FAT16)
		|| (fmt == FS_FAT32 && n_clst < MIN_FAT32))
		return FR_MKFS_ABORTED;

	/* Determine system ID in the partition table */
	if (fmt == FS_FAT32) {
		sys = 0x0C;		/* FAT32X */
	} else {
		if (fmt == FS_FAT12 && n_vol < 0x10000) {
			sys = 0x01;	/* FAT12(<65536) */
		} else {
			sys = (n_vol < 0x10000) ? 0x04 : 0x06;	/* FAT16(<65536) : FAT12/16(>=65536) */
		}
	}

	if (_MULTI_PARTITION && part) {
		/* Update system ID in the partition table */
		tbl = &fs->win[MBR_Table + (part - 1) * SZ_PTE];
		tbl[4] = sys;
		if (disk_write(pdrv, fs->win, 0, 1) != RES_OK)	/* Write it to teh MBR */
			return FR_DISK_ERR;
		md = 0xF8;
	} else {
		if (sfd) {	/* No partition table (SFD) */
			md = 0xF0;
		} else {	/* Create partition table (FDISK) */
			mem_set(fs->win, 0, SS(fs));
			tbl = fs->win+MBR_Table;	/* Create partition table for single partition in the drive */
			tbl[1] = 1;						/* Partition start head */
			tbl[2] = 1;						/* Partition start sector */
			tbl[3] = 0;						/* Partition start cylinder */
			tbl[4] = sys;					/* System type */
			tbl[5] = 254;					/* Partition end head */
			n = (b_vol + n_vol) / 63 / 255;
			tbl[6] = (BYTE)(n >> 2 | 63);	/* Partition end sector */
			tbl[7] = (BYTE)n;				/* End cylinder */
			ST_DWORD(tbl+8, 63);			/* Partition start in LBA */
			ST_DWORD(tbl+12, n_vol);		/* Partition size in LBA */
			ST_WORD(fs->win+BS_55AA, 0xAA55);	/* MBR signature */
			if (disk_write(pdrv, fs->win, 0, 1) != RES_OK)	/* Write it to the MBR */
				return FR_DISK_ERR;
			md = 0xF8;
		}
	}

	/* Create BPB in the VBR */
	tbl = fs->win;							/* Clear sector */
	mem_set(tbl, 0, SS(fs));
	mem_cpy(tbl, "\xEB\xFE\x90" "MSDOS5.0", 11);/* Boot jump code, OEM name */
	i = SS(fs);								/* Sector size */
	ST_WORD(tbl+BPB_BytsPerSec, i);
	tbl[BPB_SecPerClus] = (BYTE)au;			/* Sectors per cluster */
	ST_WORD(tbl+BPB_RsvdSecCnt, n_rsv);		/* Reserved sectors */
	tbl[BPB_NumFATs] = N_FATS;				/* Number of FATs */
	i = (fmt == FS_FAT32) ? 0 : N_ROOTDIR;	/* Number of root directory entries */
	ST_WORD(tbl+BPB_RootEntCnt, i);
	if (n_vol < 0x10000) {					/* Number of total sectors */
		ST_WORD(tbl+BPB_TotSec16, n_vol);
	} else {
		ST_DWORD(tbl+BPB_TotSec32, n_vol);
	}
	tbl[BPB_Media] = md;					/* Media descriptor */
	ST_WORD(tbl+BPB_SecPerTrk, 63);			/* Number of sectors per track */
	ST_WORD(tbl+BPB_NumHeads, 255);			/* Number of heads */
	ST_DWORD(tbl+BPB_HiddSec, b_vol);		/* Hidden sectors */
	n = GET_FATTIME();						/* Use current time as VSN */
	if (fmt == FS_FAT32) {
		ST_DWORD(tbl+BS_VolID32, n);		/* VSN */
		ST_DWORD(tbl+BPB_FATSz32, n_fat);	/* Number of sectors per FAT */
		ST_DWORD(tbl+BPB_RootClus, 2);		/* Root directory start cluster (2) */
		ST_WORD(tbl+BPB_FSInfo, 1);			/* FSINFO record offset (VBR+1) */
		ST_WORD(tbl+BPB_BkBootSec, 6);		/* Backup boot record offset (VBR+6) */
		tbl[BS_DrvNum32] = 0x80;			/* Drive number */
		tbl[BS_BootSig32] = 0x29;			/* Extended boot signature */
		mem_cpy(tbl+BS_VolLab32, "NO NAME    " "FAT32   ", 19);	/* Volume label, FAT signature */
	} else {
		ST_DWORD(tbl+BS_VolID, n);			/* VSN */
		ST_WORD(tbl+BPB_FATSz16, n_fat);	/* Number of sectors per FAT */
		tbl[BS_DrvNum] = 0x80;				/* Drive number */
		tbl[BS_BootSig] = 0x29;				/* Extended boot signature */
		mem_cpy(tbl+BS_VolLab, "NO NAME    " "FAT     ", 19);	/* Volume label, FAT signature */
	}
	ST_WORD(tbl+BS_55AA, 0xAA55);			/* Signature (Offset is fixed here regardless of sector size) */
	if (disk_write(pdrv, tbl, b_vol, 1) != RES_OK)	/* Write it to the VBR sector */
		return FR_DISK_ERR;
	if (fmt == FS_FAT32)					/* Write backup VBR if needed (VBR+6) */
		disk_write(pdrv, tbl, b_vol + 6, 1);

	/* Initialize FAT area */
	wsect = b_fat;
	for (i = 0; i < N_FATS; i++) {		/* Initialize each FAT copy */
		mem_set(tbl, 0, SS(fs));			/* 1st sector of the FAT  */
		n = md;								/* Media descriptor byte */
		if (fmt != FS_FAT32) {
			n |= (fmt == FS_FAT12) ? 0x00FFFF00 : 0xFFFFFF00;
			ST_DWORD(tbl+0, n);				/* Reserve cluster #0-1 (FAT12/16) */
		} else {
			n |= 0xFFFFFF00;
			ST_DWORD(tbl+0, n);				/* Reserve cluster #0-1 (FAT32) */
			ST_DWORD(tbl+4, 0xFFFFFFFF);
			ST_DWORD(tbl+8, 0x0FFFFFFF);	/* Reserve cluster #2 for root directory */
		}
		if (disk_write(pdrv, tbl, wsect++, 1) != RES_OK)
			return FR_DISK_ERR;
		mem_set(tbl, 0, SS(fs));			/* Fill following FAT entries with zero */
		for (n = 1; n < n_fat; n++) {		/* This loop may take a time on FAT32 volume due to many single sector writes */
			if (disk_write(pdrv, tbl, wsect++, 1) != RES_OK)
				return FR_DISK_ERR;
		}
	}

	/* Initialize root directory */
	i = (fmt == FS_FAT32) ? au : (UINT)n_dir;
	do {
		if (disk_write(pdrv, tbl, wsect++, 1) != RES_OK)
			return FR_DISK_ERR;
	} while (--i);

#if _USE_TRIM	/* Erase data area if needed */
	{
		DWORD eb[2];

		eb[0] = wsect; eb[1] = wsect + (n_clst - ((fmt == FS_FAT32) ? 1 : 0)) * au - 1;
		disk_ioctl(pdrv, CTRL_TRIM, eb);
	}
#endif

	/* Create FSINFO if needed */
	if (fmt == FS_FAT32) {
		ST_DWORD(tbl+FSI_LeadSig, 0x41615252);
		ST_DWORD(tbl+FSI_StrucSig, 0x61417272);
		ST_DWORD(tbl+FSI_Free_Count, n_clst - 1);	/* Number of free clusters */
		ST_DWORD(tbl+FSI_Nxt_Free, 2);				/* Last allocated cluster# */
		ST_WORD(tbl+BS_55AA, 0xAA55);
		disk_write(pdrv, tbl, b_vol + 1, 1);	/* Write original (VBR+1) */
		disk_write(pdrv, tbl, b_vol + 7, 1);	/* Write backup (VBR+7) */
	}

	return (disk_ioctl(pdrv, CTRL_SYNC, 0) == RES_OK) ? FR_OK : FR_DISK_ERR;
}



#if _MULTI_PARTITION
/*-----------------------------------------------------------------------*/
/* Divide Physical Drive                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_fdisk (
	BYTE pdrv,			/* Physical drive number */
	const DWORD szt[],	/* Pointer to the size table for each partitions */
	void* work			/* Pointer to the working buffer */
)
{
	UINT i, n, sz_cyl, tot_cyl, b_cyl, e_cyl, p_cyl;
	BYTE s_hd, e_hd, *p, *buf = (BYTE*)work;
	DSTATUS stat;
	DWORD sz_disk, sz_part, s_part;


	stat = disk_initialize(pdrv);
	if (stat & STA_NOINIT) return FR_NOT_READY;
	if (stat & STA_PROTECT) return FR_WRITE_PROTECTED;
	if (disk_ioctl(pdrv, GET_SECTOR_COUNT, &sz_disk)) return FR_DISK_ERR;

	/* Determine CHS in the table regardless of the drive geometry */
	for (n = 16; n < 256 && sz_disk / n / 63 > 1024; n *= 2) ;
	if (n == 256) n--;
	e_hd = n - 1;
	sz_cyl = 63 * n;
	tot_cyl = sz_disk / sz_cyl;

	/* Create partition table */
	mem_set(buf, 0, _MAX_SS);
	p = buf + MBR_Table; b_cyl = 0;
	for (i = 0; i < 4; i++, p += SZ_PTE) {
		p_cyl = (szt[i] <= 100U) ? (DWORD)tot_cyl * szt[i] / 100 : szt[i] / sz_cyl;
		if (!p_cyl) continue;
		s_part = (DWORD)sz_cyl * b_cyl;
		sz_part = (DWORD)sz_cyl * p_cyl;
		if (i == 0) {	/* Exclude first track of cylinder 0 */
			s_hd = 1;
			s_part += 63; sz_part -= 63;
		} else {
			s_hd = 0;
		}
		e_cyl = b_cyl + p_cyl - 1;
		if (e_cyl >= tot_cyl) return FR_INVALID_PARAMETER;

		/* Set partition table */
		p[1] = s_hd;						/* Start head */
		p[2] = (BYTE)((b_cyl >> 2) + 1);	/* Start sector */
		p[3] = (BYTE)b_cyl;					/* Start cylinder */
		p[4] = 0x06;						/* System type (temporary setting) */
		p[5] = e_hd;						/* End head */
		p[6] = (BYTE)((e_cyl >> 2) + 63);	/* End sector */
		p[7] = (BYTE)e_cyl;					/* End cylinder */
		ST_DWORD(p + 8, s_part);			/* Start sector in LBA */
		ST_DWORD(p + 12, sz_part);			/* Partition size */

		/* Next partition */
		b_cyl += p_cyl;
	}
	ST_WORD(p, 0xAA55);

	/* Write it to the MBR */
	return (disk_write(pdrv, buf, 0, 1) != RES_OK || disk_ioctl(pdrv, CTRL_SYNC, 0) != RES_OK) ? FR_DISK_ERR : FR_OK;
}


#endif /* _MULTI_PARTITION */
#endif /* _USE_MKFS && !_FS_READONLY */




#if _USE_STRFUNC
/*-----------------------------------------------------------------------*/
/* Get a string from the file                                            */
/*-----------------------------------------------------------------------*/

TCHAR* f_gets (
	TCHAR* buff,	/* Pointer to the string buffer to read */
	int len,		/* Size of string buffer (characters) */
	FIL* fp			/* Pointer to the file object */
)
{
	int n = 0;
	TCHAR c, *p = buff;
	BYTE s[2];
	UINT rc;


	while (n < len - 1) {	/* Read characters until buffer gets filled */
#if _USE_LFN && _LFN_UNICODE
#if _STRF_ENCODE == 3		/* Read a character in UTF-8 */
		f_read(fp, s, 1, &rc);
		if (rc != 1) break;
		c = s[0];
		if (c >= 0x80) {
			if (c < 0xC0) continue;	/* Skip stray trailer */
			if (c < 0xE0) {			/* Two-byte sequence */
				f_read(fp, s, 1, &rc);
				if (rc != 1) break;
				c = (c & 0x1F) << 6 | (s[0] & 0x3F);
				if (c < 0x80) c = '?';
			} else {
				if (c < 0xF0) {		/* Three-byte sequence */
					f_read(fp, s, 2, &rc);
					if (rc != 2) break;
					c = c << 12 | (s[0] & 0x3F) << 6 | (s[1] & 0x3F);
					if (c < 0x800) c = '?';
				} else {			/* Reject four-byte sequence */
					c = '?';
				}
			}
		}
#elif _STRF_ENCODE == 2		/* Read a character in UTF-16BE */
		f_read(fp, s, 2, &rc);
		if (rc != 2) break;
		c = s[1] + (s[0] << 8);
#elif _STRF_ENCODE == 1		/* Read a character in UTF-16LE */
		f_read(fp, s, 2, &rc);
		if (rc != 2) break;
		c = s[0] + (s[1] << 8);
#else						/* Read a character in ANSI/OEM */
		f_read(fp, s, 1, &rc);
		if (rc != 1) break;
		c = s[0];
		if (IsDBCS1(c)) {
			f_read(fp, s, 1, &rc);
			if (rc != 1) break;
			c = (c << 8) + s[0];
		}
		c = ff_convert(c, 1);	/* OEM -> Unicode */
		if (!c) c = '?';
#endif
#else						/* Read a character without conversion */
		f_read(fp, s, 1, &rc);
		if (rc != 1) break;
		c = s[0];
#endif
		if (_USE_STRFUNC == 2 && c == '\r') continue;	/* Strip '\r' */
		*p++ = c;
		n++;
		if (c == '\n') break;		/* Break on EOL */
	}
	*p = 0;
	return n ? buff : 0;			/* When no data read (eof or error), return with error. */
}



#if !_FS_READONLY
#include <stdarg.h>
/*-----------------------------------------------------------------------*/
/* Put a character to the file                                           */
/*-----------------------------------------------------------------------*/

typedef struct {
	FIL* fp;
	int idx, nchr;
	BYTE buf[64];
} putbuff;


static
void putc_bfd (
	putbuff* pb,
	TCHAR c
)
{
	UINT bw;
	int i;


	if (_USE_STRFUNC == 2 && c == '\n')	 /* LF -> CRLF conversion */
		putc_bfd(pb, '\r');

	i = pb->idx;	/* Buffer write index (-1:error) */
	if (i < 0) return;

#if _USE_LFN && _LFN_UNICODE
#if _STRF_ENCODE == 3			/* Write a character in UTF-8 */
	if (c < 0x80) {				/* 7-bit */
		pb->buf[i++] = (BYTE)c;
	} else {
		if (c < 0x800) {		/* 11-bit */
			pb->buf[i++] = (BYTE)(0xC0 | c >> 6);
		} else {				/* 16-bit */
			pb->buf[i++] = (BYTE)(0xE0 | c >> 12);
			pb->buf[i++] = (BYTE)(0x80 | (c >> 6 & 0x3F));
		}
		pb->buf[i++] = (BYTE)(0x80 | (c & 0x3F));
	}
#elif _STRF_ENCODE == 2			/* Write a character in UTF-16BE */
	pb->buf[i++] = (BYTE)(c >> 8);
	pb->buf[i++] = (BYTE)c;
#elif _STRF_ENCODE == 1			/* Write a character in UTF-16LE */
	pb->buf[i++] = (BYTE)c;
	pb->buf[i++] = (BYTE)(c >> 8);
#else							/* Write a character in ANSI/OEM */
	c = ff_convert(c, 0);	/* Unicode -> OEM */
	if (!c) c = '?';
	if (c >= 0x100)
		pb->buf[i++] = (BYTE)(c >> 8);
	pb->buf[i++] = (BYTE)c;
#endif
#else							/* Write a character without conversion */
	pb->buf[i++] = (BYTE)c;
#endif

	if (i >= (int)(sizeof pb->buf) - 3) {	/* Write buffered characters to the file */
		f_write(pb->fp, pb->buf, (UINT)i, &bw);
		i = (bw == (UINT)i) ? 0 : -1;
	}
	pb->idx = i;
	pb->nchr++;
}



int f_putc (
	TCHAR c,	/* A character to be output */
	FIL* fp		/* Pointer to the file object */
)
{
	putbuff pb;
	UINT nw;


	pb.fp = fp;			/* Initialize output buffer */
	pb.nchr = pb.idx = 0;

	putc_bfd(&pb, c);	/* Put a character */

	if (   pb.idx >= 0	/* Flush buffered characters to the file */
		&& f_write(pb.fp, pb.buf, (UINT)pb.idx, &nw) == FR_OK
		&& (UINT)pb.idx == nw) return pb.nchr;
	return EOF;
}




/*-----------------------------------------------------------------------*/
/* Put a string to the file                                              */
/*-----------------------------------------------------------------------*/

int f_puts (
	const TCHAR* str,	/* Pointer to the string to be output */
	FIL* fp				/* Pointer to the file object */
)
{
	putbuff pb;
	UINT nw;


	pb.fp = fp;				/* Initialize output buffer */
	pb.nchr = pb.idx = 0;

	while (*str)			/* Put the string */
		putc_bfd(&pb, *str++);

	if (   pb.idx >= 0		/* Flush buffered characters to the file */
		&& f_write(pb.fp, pb.buf, (UINT)pb.idx, &nw) == FR_OK
		&& (UINT)pb.idx == nw) return pb.nchr;
	return EOF;
}




/*-----------------------------------------------------------------------*/
/* Put a formatted string to the file                                    */
/*-----------------------------------------------------------------------*/

int f_printf (
	FIL* fp,			/* Pointer to the file object */
	const TCHAR* fmt,	/* Pointer to the format string */
	...					/* Optional arguments... */
)
{
	va_list arp;
	BYTE f, r;
	UINT nw, i, j, w;
	DWORD v;
	TCHAR c, d, s[16], *p;
	putbuff pb;


	pb.fp = fp;				/* Initialize output buffer */
	pb.nchr = pb.idx = 0;

	va_start(arp, fmt);

	for (;;) {
		c = *fmt++;
		if (c == 0) break;			/* End of string */
		if (c != '%') {				/* Non escape character */
			putc_bfd(&pb, c);
			continue;
		}
		w = f = 0;
		c = *fmt++;
		if (c == '0') {				/* Flag: '0' padding */
			f = 1; c = *fmt++;
		} else {
			if (c == '-') {			/* Flag: left justified */
				f = 2; c = *fmt++;
			}
		}
		while (IsDigit(c)) {		/* Precision */
			w = w * 10 + c - '0';
			c = *fmt++;
		}
		if (c == 'l' || c == 'L') {	/* Prefix: Size is long int */
			f |= 4; c = *fmt++;
		}
		if (!c) break;
		d = c;
		if (IsLower(d)) d -= 0x20;
		switch (d) {				/* Type is... */
		case 'S' :					/* String */
			p = va_arg(arp, TCHAR*);
			for (j = 0; p[j]; j++) ;
			if (!(f & 2)) {
				while (j++ < w) putc_bfd(&pb, ' ');
			}
			while (*p) putc_bfd(&pb, *p++);
			while (j++ < w) putc_bfd(&pb, ' ');
			continue;
		case 'C' :					/* Character */
			putc_bfd(&pb, (TCHAR)va_arg(arp, int)); continue;
		case 'B' :					/* Binary */
			r = 2; break;
		case 'O' :					/* Octal */
			r = 8; break;
		case 'D' :					/* Signed decimal */
		case 'U' :					/* Unsigned decimal */
			r = 10; break;
		case 'X' :					/* Hexdecimal */
			r = 16; break;
		default:					/* Unknown type (pass-through) */
			putc_bfd(&pb, c); continue;
		}

		/* Get an argument and put it in numeral */
		v = (f & 4) ? (DWORD)va_arg(arp, long) : ((d == 'D') ? (DWORD)(long)va_arg(arp, int) : (DWORD)va_arg(arp, unsigned int));
		if (d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= 8;
		}
		i = 0;
		do {
			d = (TCHAR)(v % r); v /= r;
			if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
			s[i++] = d + '0';
		} while (v && i < sizeof s / sizeof s[0]);
		if (f & 8) s[i++] = '-';
		j = i; d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w) putc_bfd(&pb, d);
		do putc_bfd(&pb, s[--i]); while (i);
		while (j++ < w) putc_bfd(&pb, d);
	}

	va_end(arp);

	if (   pb.idx >= 0		/* Flush buffered characters to the file */
		&& f_write(pb.fp, pb.buf, (UINT)pb.idx, &nw) == FR_OK
		&& (UINT)pb.idx == nw) return pb.nchr;
	return EOF;
}

#endif /* !_FS_READONLY */
#endif /* _USE_STRFUNC */
# 1 "thirdparty/FatFs/option/ccsbcs.c"
/*------------------------------------------------------------------------*/
/* Unicode - Local code bidirectional converter  (C)ChaN, 2012            */
/* (SBCS code pages)                                                      */
/*------------------------------------------------------------------------*/
/*  437   U.S. (OEM)
/   720   Arabic (OEM)
/   1256  Arabic (Windows)
/   737   Greek (OEM)
/   1253  Greek (Windows)
/   1250  Central Europe (Windows)
/   775   Baltic (OEM)
/   1257  Baltic (Windows)
/   850   Multilingual Latin 1 (OEM)
/   852   Latin 2 (OEM)
/   1252  Latin 1 (Windows)
/   855   Cyrillic (OEM)
/   1251  Cyrillic (Windows)
/   866   Russian (OEM)
/   857   Turkish (OEM)
/   1254  Turkish (Windows)
/   858   Multilingual Latin 1 + Euro (OEM)
/   862   Hebrew (OEM)
/   1255  Hebrew (Windows)
/   874   Thai (OEM, Windows)
/   1258  Vietnam (OEM, Windows)
*/

#include "../ff.h"
#if !defined(SIMU)
#include <avr/pgmspace.h>
#endif

#if _CODE_PAGE == 437
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP437(0x80-0xFF) to Unicode conversion table */
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
	0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
	0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
	0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
	0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
	0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
	0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
	0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 720
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP720(0x80-0xFF) to Unicode conversion table */
	0x0000, 0x0000, 0x00E9, 0x00E2, 0x0000, 0x00E0, 0x0000, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0651, 0x0652, 0x00F4, 0x00A4, 0x0640, 0x00FB, 0x00F9,
	0x0621, 0x0622, 0x0623, 0x0624, 0x00A3, 0x0625, 0x0626, 0x0627,
	0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
	0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
	0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
	0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x0636, 0x0637, 0x0638, 0x0639, 0x063A, 0x0641, 0x00B5, 0x0642,
	0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649, 0x064A,
	0x2261, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F, 0x0650, 0x2248,
	0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 737
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP737(0x80-0xFF) to Unicode conversion table */
	0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398,
	0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F, 0x03A0,
	0x03A1, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7, 0x03A8, 0x03A9,
	0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7, 0x03B8,
	0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF, 0x03C0,
	0x03C1, 0x03C3, 0x03C2, 0x03C4, 0x03C5, 0x03C6, 0x03C7, 0x03C8,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
	0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
	0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x03C9, 0x03AC, 0x03AD, 0x03AE, 0x03CA, 0x03AF, 0x03CC, 0x03CD,
	0x03CB, 0x03CE, 0x0386, 0x0388, 0x0389, 0x038A, 0x038C, 0x038E,
	0x038F, 0x00B1, 0x2265, 0x2264, 0x03AA, 0x03AB, 0x00F7, 0x2248,
	0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 775
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP775(0x80-0xFF) to Unicode conversion table */
	0x0106, 0x00FC, 0x00E9, 0x0101, 0x00E4, 0x0123, 0x00E5, 0x0107,
	0x0142, 0x0113, 0x0156, 0x0157, 0x012B, 0x0179, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x014D, 0x00F6, 0x0122, 0x00A2, 0x015A,
	0x015B, 0x00D6, 0x00DC, 0x00F8, 0x00A3, 0x00D8, 0x00D7, 0x00A4,
	0x0100, 0x012A, 0x00F3, 0x017B, 0x017C, 0x017A, 0x201D, 0x00A6,
	0x00A9, 0x00AE, 0x00AC, 0x00BD, 0x00BC, 0x0141, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x0104, 0x010C, 0x0118,
	0x0116, 0x2563, 0x2551, 0x2557, 0x255D, 0x012E, 0x0160, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x0172, 0x016A,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x017D,
	0x0105, 0x010D, 0x0119, 0x0117, 0x012F, 0x0161, 0x0173, 0x016B,
	0x017E, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x00D3, 0x00DF, 0x014C, 0x0143, 0x00F5, 0x00D5, 0x00B5, 0x0144,
	0x0136, 0x0137, 0x013B, 0x013C, 0x0146, 0x0112, 0x0145, 0x2019,
	0x00AD, 0x00B1, 0x201C, 0x00BE, 0x00B6, 0x00A7, 0x00F7, 0x201E,
	0x00B0, 0x2219, 0x00B7, 0x00B9, 0x00B3, 0x00B2, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 850
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP850(0x80-0xFF) to Unicode conversion table */
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
	0x00FF, 0x00D6, 0x00DC, 0x00F8, 0x00A3, 0x00D8, 0x00D7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
	0x00BF, 0x00AE, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00C1, 0x00C2, 0x00C0,
	0x00A9, 0x2563, 0x2551, 0x2557, 0x255D, 0x00A2, 0x00A5, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x00E3, 0x00C3,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x00A4,
	0x00F0, 0x00D0, 0x00CA, 0x00CB, 0x00C8, 0x0131, 0x00CD, 0x00CE,
	0x00CF, 0x2518, 0x250C, 0x2588, 0x2584, 0x00A6, 0x00CC, 0x2580,
	0x00D3, 0x00DF, 0x00D4, 0x00D2, 0x00F5, 0x00D5, 0x00B5, 0x00FE,
	0x00DE, 0x00DA, 0x00DB, 0x00D9, 0x00FD, 0x00DD, 0x00AF, 0x00B4,
	0x00AD, 0x00B1, 0x2017, 0x00BE, 0x00B6, 0x00A7, 0x00F7, 0x00B8,
	0x00B0, 0x00A8, 0x00B7, 0x00B9, 0x00B3, 0x00B2, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 852
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP852(0x80-0xFF) to Unicode conversion table */
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x016F, 0x0107, 0x00E7,
	0x0142, 0x00EB, 0x0150, 0x0151, 0x00EE, 0x0179, 0x00C4, 0x0106,
	0x00C9, 0x0139, 0x013A, 0x00F4, 0x00F6, 0x013D, 0x013E, 0x015A,
	0x015B, 0x00D6, 0x00DC, 0x0164, 0x0165, 0x0141, 0x00D7, 0x010D,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x0104, 0x0105, 0x017D, 0x017E,
	0x0118, 0x0119, 0x00AC, 0x017A, 0x010C, 0x015F, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00C1, 0x00C2, 0x011A,
	0x015E, 0x2563, 0x2551, 0x2557, 0x255D, 0x017B, 0x017C, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x0102, 0x0103,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x00A4,
	0x0111, 0x0110, 0x010E, 0x00CB, 0x010F, 0x0147, 0x00CD, 0x00CE,
	0x011B, 0x2518, 0x250C, 0x2588, 0x2584, 0x0162, 0x016E, 0x2580,
	0x00D3, 0x00DF, 0x00D4, 0x0143, 0x0144, 0x0148, 0x0160, 0x0161,
	0x0154, 0x00DA, 0x0155, 0x0170, 0x00FD, 0x00DD, 0x0163, 0x00B4,
	0x00AD, 0x02DD, 0x02DB, 0x02C7, 0x02D8, 0x00A7, 0x00F7, 0x00B8,
	0x00B0, 0x00A8, 0x02D9, 0x0171, 0x0158, 0x0159, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 855
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP855(0x80-0xFF) to Unicode conversion table */
	0x0452, 0x0402, 0x0453, 0x0403, 0x0451, 0x0401, 0x0454, 0x0404,
	0x0455, 0x0405, 0x0456, 0x0406, 0x0457, 0x0407, 0x0458, 0x0408,
	0x0459, 0x0409, 0x045A, 0x040A, 0x045B, 0x040B, 0x045C, 0x040C,
	0x045E, 0x040E, 0x045F, 0x040F, 0x044E, 0x042E, 0x044A, 0x042A,
	0x0430, 0x0410, 0x0431, 0x0411, 0x0446, 0x0426, 0x0434, 0x0414,
	0x0435, 0x0415, 0x0444, 0x0424, 0x0433, 0x0413, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x0445, 0x0425, 0x0438,
	0x0418, 0x2563, 0x2551, 0x2557, 0x255D, 0x0439, 0x0419, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x043A, 0x041A,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x00A4,
	0x043B, 0x041B, 0x043C, 0x041C, 0x043D, 0x041D, 0x043E, 0x041E,
	0x043F, 0x2518, 0x250C, 0x2588, 0x2584, 0x041F, 0x044F, 0x2580,
	0x042F, 0x0440, 0x0420, 0x0441, 0x0421, 0x0442, 0x0422, 0x0443,
	0x0423, 0x0436, 0x0416, 0x0432, 0x0412, 0x044C, 0x042C, 0x2116,
	0x00AD, 0x044B, 0x042B, 0x0437, 0x0417, 0x0448, 0x0428, 0x044D,
	0x042D, 0x0449, 0x0429, 0x0447, 0x0427, 0x00A7, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 857
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP857(0x80-0xFF) to Unicode conversion table */
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x0131, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
	0x0130, 0x00D6, 0x00DC, 0x00F8, 0x00A3, 0x00D8, 0x015E, 0x015F,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x011E, 0x011F,
	0x00BF, 0x00AE, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00C1, 0x00C2, 0x00C0,
	0x00A9, 0x2563, 0x2551, 0x2557, 0x255D, 0x00A2, 0x00A5, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x00E3, 0x00C3,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x00A4,
	0x00BA, 0x00AA, 0x00CA, 0x00CB, 0x00C8, 0x0000, 0x00CD, 0x00CE,
	0x00CF, 0x2518, 0x250C, 0x2588, 0x2584, 0x00A6, 0x00CC, 0x2580,
	0x00D3, 0x00DF, 0x00D4, 0x00D2, 0x00F5, 0x00D5, 0x00B5, 0x0000,
	0x00D7, 0x00DA, 0x00DB, 0x00D9, 0x00EC, 0x00FF, 0x00AF, 0x00B4,
	0x00AD, 0x00B1, 0x0000, 0x00BE, 0x00B6, 0x00A7, 0x00F7, 0x00B8,
	0x00B0, 0x00A8, 0x00B7, 0x00B9, 0x00B3, 0x00B2, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 858
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP858(0x80-0xFF) to Unicode conversion table */
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
	0x00FF, 0x00D6, 0x00DC, 0x00F8, 0x00A3, 0x00D8, 0x00D7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
	0x00BF, 0x00AE, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00C1, 0x00C2, 0x00C0,
	0x00A9, 0x2563, 0x2551, 0x2557, 0x2550, 0x00A2, 0x00A5, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x00E3, 0x00C3,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x00A4,
	0x00F0, 0x00D0, 0x00CA, 0x00CB, 0x00C8, 0x20AC, 0x00CD, 0x00CE,
	0x00CF, 0x2518, 0x250C, 0x2588, 0x2584, 0x00C6, 0x00CC, 0x2580,
	0x00D3, 0x00DF, 0x00D4, 0x00D2, 0x00F5, 0x00D5, 0x00B5, 0x00FE,
	0x00DE, 0x00DA, 0x00DB, 0x00D9, 0x00FD, 0x00DD, 0x00AF, 0x00B4,
	0x00AD, 0x00B1, 0x2017, 0x00BE, 0x00B6, 0x00A7, 0x00F7, 0x00B8,
	0x00B0, 0x00A8, 0x00B7, 0x00B9, 0x00B3, 0x00B2, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 862
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP862(0x80-0xFF) to Unicode conversion table */
	0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
	0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
	0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
	0x05E8, 0x05E9, 0x05EA, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
	0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
	0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
	0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
	0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
	0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
	0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 866
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP866(0x80-0xFF) to Unicode conversion table */
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
	0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
	0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
	0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
	0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
	0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
	0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
	0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
	0x0401, 0x0451, 0x0404, 0x0454, 0x0407, 0x0457, 0x040E, 0x045E,
	0x00B0, 0x2219, 0x00B7, 0x221A, 0x2116, 0x00A4, 0x25A0, 0x00A0
};

#elif _CODE_PAGE == 874
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP874(0x80-0xFF) to Unicode conversion table */
	0x20AC, 0x0000, 0x0000, 0x0000, 0x0000, 0x2026, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x00A0, 0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07,
	0x0E08, 0x0E09, 0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F,
	0x0E10, 0x0E11, 0x0E12, 0x0E13, 0x0E14, 0x0E15, 0x0E16, 0x0E17,
	0x0E18, 0x0E19, 0x0E1A, 0x0E1B, 0x0E1C, 0x0E1D, 0x0E1E, 0x0E1F,
	0x0E20, 0x0E21, 0x0E22, 0x0E23, 0x0E24, 0x0E25, 0x0E26, 0x0E27,
	0x0E28, 0x0E29, 0x0E2A, 0x0E2B, 0x0E2C, 0x0E2D, 0x0E2E, 0x0E2F,
	0x0E30, 0x0E31, 0x0E32, 0x0E33, 0x0E34, 0x0E35, 0x0E36, 0x0E37,
	0x0E38, 0x0E39, 0x0E3A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0E3F,
	0x0E40, 0x0E41, 0x0E42, 0x0E43, 0x0E44, 0x0E45, 0x0E46, 0x0E47,
	0x0E48, 0x0E49, 0x0E4A, 0x0E4B, 0x0E4C, 0x0E4D, 0x0E4E, 0x0E4F,
	0x0E50, 0x0E51, 0x0E52, 0x0E53, 0x0E54, 0x0E55, 0x0E56, 0x0E57,
	0x0E58, 0x0E59, 0x0E5A, 0x0E5B, 0x0000, 0x0000, 0x0000, 0x0000
};

#elif _CODE_PAGE == 1250
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP1250(0x80-0xFF) to Unicode conversion table */
	0x20AC, 0x0000, 0x201A, 0x0000, 0x201E, 0x2026, 0x2020, 0x2021,
	0x0000, 0x2030, 0x0160, 0x2039, 0x015A, 0x0164, 0x017D, 0x0179,
	0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x0000, 0x2122, 0x0161, 0x203A, 0x015B, 0x0165, 0x017E, 0x017A,
	0x00A0, 0x02C7, 0x02D8, 0x0141, 0x00A4, 0x0104, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x015E, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x017B,
	0x00B0, 0x00B1, 0x02DB, 0x0142, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x0105, 0x015F, 0x00BB, 0x013D, 0x02DD, 0x013E, 0x017C,
	0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
	0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
	0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
	0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
	0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
	0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
	0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
	0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
};

#elif _CODE_PAGE == 1251
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP1251(0x80-0xFF) to Unicode conversion table */
	0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
	0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
	0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x0000, 0x2111, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
	0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
	0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
	0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
	0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
	0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
	0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
	0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
	0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};

#elif _CODE_PAGE == 1252
#define _TBLDEF 1

#if !defined(SIMU)
static
const WCHAR Tbl[] PROGMEM = {	/*  CP1252(0x80-0xFF) to Unicode conversion table */
#else
static
const WCHAR Tbl[] = {	/*  CP1252(0x80-0xFF) to Unicode conversion table */
#endif
	0x20AC, 0x0000, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x0000, 0x017D, 0x0000,
	0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x0000, 0x017E, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
};

#elif _CODE_PAGE == 1253
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP1253(0x80-0xFF) to Unicode conversion table */
	0x20AC, 0x0000, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x0000, 0x2030, 0x0000, 0x2039, 0x000C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x0000, 0x2122, 0x0000, 0x203A, 0x0000, 0x0000, 0x0000, 0x0000,
	0x00A0, 0x0385, 0x0386, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x0000, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x2015,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x0384, 0x00B5, 0x00B6, 0x00B7,
	0x0388, 0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD, 0x038E, 0x038F,
	0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
	0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
	0x03A0, 0x03A1, 0x0000, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
	0x03A8, 0x03A9, 0x03AA, 0x03AD, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
	0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
	0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
	0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
	0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE, 0x0000
};

#elif _CODE_PAGE == 1254
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP1254(0x80-0xFF) to Unicode conversion table */
	0x20AC, 0x0000, 0x210A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x0000, 0x0000, 0x0000,
	0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x0000, 0x0000, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x011E, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00BD, 0x00DC, 0x0130, 0x015E, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x011F, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0131, 0x015F, 0x00FF
};

#elif _CODE_PAGE == 1255
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP1255(0x80-0xFF) to Unicode conversion table */
	0x20AC, 0x0000, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0000, 0x2039, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0x0000, 0x203A, 0x0000, 0x0000, 0x0000, 0x0000,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00D7, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00F7, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x05B0, 0x05B1, 0x05B2, 0x05B3, 0x05B4, 0x05B5, 0x05B6, 0x05B7,
	0x05B8, 0x05B9, 0x0000, 0x05BB, 0x05BC, 0x05BD, 0x05BE, 0x05BF,
	0x05C0, 0x05C1, 0x05C2, 0x05C3, 0x05F0, 0x05F1, 0x05F2, 0x05F3,
	0x05F4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x05D0, 0x05D1, 0x05D2, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7,
	0x05D8, 0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF,
	0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7,
	0x05E8, 0x05E9, 0x05EA, 0x0000, 0x0000, 0x200E, 0x200F, 0x0000
};

#elif _CODE_PAGE == 1256
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP1256(0x80-0xFF) to Unicode conversion table */
	0x20AC, 0x067E, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0679, 0x2039, 0x0152, 0x0686, 0x0698, 0x0688,
	0x06AF, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x06A9, 0x2122, 0x0691, 0x203A, 0x0153, 0x200C, 0x200D, 0x06BA,
	0x00A0, 0x060C, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x06BE, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x061B, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x061F,
	0x06C1, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
	0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 0x062D, 0x062E, 0x062F,
	0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x00D7,
	0x0637, 0x0638, 0x0639, 0x063A, 0x0640, 0x0640, 0x0642, 0x0643,
	0x00E0, 0x0644, 0x00E2, 0x0645, 0x0646, 0x0647, 0x0648, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x0649, 0x064A, 0x00EE, 0x00EF,
	0x064B, 0x064C, 0x064D, 0x064E, 0x00F4, 0x064F, 0x0650, 0x00F7,
	0x0651, 0x00F9, 0x0652, 0x00FB, 0x00FC, 0x200E, 0x200F, 0x06D2
};

#elif _CODE_PAGE == 1257
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP1257(0x80-0xFF) to Unicode conversion table */
	0x20AC, 0x0000, 0x201A, 0x0000, 0x201E, 0x2026, 0x2020, 0x2021,
	0x0000, 0x2030, 0x0000, 0x2039, 0x0000, 0x00A8, 0x02C7, 0x00B8,
	0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x0000, 0x2122, 0x0000, 0x203A, 0x0000, 0x00AF, 0x02DB, 0x0000,
	0x00A0, 0x0000, 0x00A2, 0x00A3, 0x00A4, 0x0000, 0x00A6, 0x00A7,
	0x00D8, 0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6,
	0x0104, 0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112,
	0x010C, 0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B,
	0x0160, 0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7,
	0x0172, 0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF,
	0x0105, 0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113,
	0x010D, 0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C,
	0x0161, 0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7,
	0x0173, 0x014E, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x02D9
};

#elif _CODE_PAGE == 1258
#define _TBLDEF 1
static
const WCHAR Tbl[] = {	/*  CP1258(0x80-0xFF) to Unicode conversion table */
	0x20AC, 0x0000, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0000, 0x2039, 0x0152, 0x0000, 0x0000, 0x0000,
	0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0x0000, 0x203A, 0x0153, 0x0000, 0x0000, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x0300, 0x00CD, 0x00CE, 0x00CF,
	0x0110, 0x00D1, 0x0309, 0x00D3, 0x00D4, 0x01A0, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x01AF, 0x0303, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x0301, 0x00ED, 0x00EE, 0x00EF,
	0x0111, 0x00F1, 0x0323, 0x00F3, 0x00F4, 0x01A1, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x01B0, 0x20AB, 0x00FF
};

#endif


#if !_TBLDEF || !_USE_LFN
#error This file is not needed in current configuration. Remove from the project.
#endif


#if !defined(SIMU)
WCHAR ff_convert (	/* Converted character, Returns zero on error */
	WCHAR	chr,	/* Character code to be converted */
	UINT	dir		/* 0: Unicode to OEMCP, 1: OEMCP to Unicode */
)
{
	WCHAR c;


	if (chr < 0x80) {	/* ASCII */
		c = chr;

	} else {
		if (dir) {		/* OEMCP to Unicode */
			c = (chr >= 0x100) ? 0 : pgm_read_word_near(&Tbl[chr - 0x80]);

		} else {		/* Unicode to OEMCP */
			for (c = 0; c < 0x80; c++) {
				if (chr == pgm_read_word_near(&Tbl[c])) break;
			}
			c = (c + 0x80) & 0xFF;
		}
	}

	return c;
}


WCHAR ff_wtoupper (	/* Upper converted character */
	WCHAR chr		/* Input character */
)
{
	static const WCHAR tbl_lower[] PROGMEM = { 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0xA1, 0x00A2, 0x00A3, 0x00A5, 0x00AC, 0x00AF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x0FF, 0x101, 0x103, 0x105, 0x107, 0x109, 0x10B, 0x10D, 0x10F, 0x111, 0x113, 0x115, 0x117, 0x119, 0x11B, 0x11D, 0x11F, 0x121, 0x123, 0x125, 0x127, 0x129, 0x12B, 0x12D, 0x12F, 0x131, 0x133, 0x135, 0x137, 0x13A, 0x13C, 0x13E, 0x140, 0x142, 0x144, 0x146, 0x148, 0x14B, 0x14D, 0x14F, 0x151, 0x153, 0x155, 0x157, 0x159, 0x15B, 0x15D, 0x15F, 0x161, 0x163, 0x165, 0x167, 0x169, 0x16B, 0x16D, 0x16F, 0x171, 0x173, 0x175, 0x177, 0x17A, 0x17C, 0x17E, 0x192, 0x3B1, 0x3B2, 0x3B3, 0x3B4, 0x3B5, 0x3B6, 0x3B7, 0x3B8, 0x3B9, 0x3BA, 0x3BB, 0x3BC, 0x3BD, 0x3BE, 0x3BF, 0x3C0, 0x3C1, 0x3C3, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 0x3C8, 0x3C9, 0x3CA, 0x430, 0x431, 0x432, 0x433, 0x434, 0x435, 0x436, 0x437, 0x438, 0x439, 0x43A, 0x43B, 0x43C, 0x43D, 0x43E, 0x43F, 0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44A, 0x44B, 0x44C, 0x44D, 0x44E, 0x44F, 0x451, 0x452, 0x453, 0x454, 0x455, 0x456, 0x457, 0x458, 0x459, 0x45A, 0x45B, 0x45C, 0x45E, 0x45F, 0x2170, 0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177, 0x2178, 0x2179, 0x217A, 0x217B, 0x217C, 0x217D, 0x217E, 0x217F, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A, 0 };
	static const WCHAR tbl_upper[] PROGMEM = { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x21, 0xFFE0, 0xFFE1, 0xFFE5, 0xFFE2, 0xFFE3, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0x178, 0x100, 0x102, 0x104, 0x106, 0x108, 0x10A, 0x10C, 0x10E, 0x110, 0x112, 0x114, 0x116, 0x118, 0x11A, 0x11C, 0x11E, 0x120, 0x122, 0x124, 0x126, 0x128, 0x12A, 0x12C, 0x12E, 0x130, 0x132, 0x134, 0x136, 0x139, 0x13B, 0x13D, 0x13F, 0x141, 0x143, 0x145, 0x147, 0x14A, 0x14C, 0x14E, 0x150, 0x152, 0x154, 0x156, 0x158, 0x15A, 0x15C, 0x15E, 0x160, 0x162, 0x164, 0x166, 0x168, 0x16A, 0x16C, 0x16E, 0x170, 0x172, 0x174, 0x176, 0x179, 0x17B, 0x17D, 0x191, 0x391, 0x392, 0x393, 0x394, 0x395, 0x396, 0x397, 0x398, 0x399, 0x39A, 0x39B, 0x39C, 0x39D, 0x39E, 0x39F, 0x3A0, 0x3A1, 0x3A3, 0x3A4, 0x3A5, 0x3A6, 0x3A7, 0x3A8, 0x3A9, 0x3AA, 0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416, 0x417, 0x418, 0x419, 0x41A, 0x41B, 0x41C, 0x41D, 0x41E, 0x41F, 0x420, 0x421, 0x422, 0x423, 0x424, 0x425, 0x426, 0x427, 0x428, 0x429, 0x42A, 0x42B, 0x42C, 0x42D, 0x42E, 0x42F, 0x401, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40A, 0x40B, 0x40C, 0x40E, 0x40F, 0x2160, 0x2161, 0x2162, 0x2163, 0x2164, 0x2165, 0x2166, 0x2167, 0x2168, 0x2169, 0x216A, 0x216B, 0x216C, 0x216D, 0x216E, 0x216F, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F, 0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A, 0 };
	int i;


	for (i = 0; pgm_read_byte_near(&tbl_lower[i]) && chr != pgm_read_byte_near(&tbl_lower[i]); i++) ;

	return pgm_read_byte_near(&tbl_lower[i]) ? pgm_read_byte_near(&tbl_upper[i]) : chr;
}
#else //not SIMU
WCHAR ff_convert (	/* Converted character, Returns zero on error */
	WCHAR	chr,	/* Character code to be converted */
	UINT	dir		/* 0: Unicode to OEMCP, 1: OEMCP to Unicode */
)
{
	WCHAR c;


	if (chr < 0x80) {	/* ASCII */
		c = chr;

	} else {
		if (dir) {		/* OEMCP to Unicode */
			c = (chr >= 0x100) ? 0 : Tbl[chr - 0x80];

		} else {		/* Unicode to OEMCP */
			for (c = 0; c < 0x80; c++) {
				if (chr == Tbl[c]) break;
			}
			c = (c + 0x80) & 0xFF;
		}
	}

	return c;
}


WCHAR ff_wtoupper (	/* Upper converted character */
	WCHAR chr		/* Input character */
)
{
	static const WCHAR tbl_lower[] = { 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0xA1, 0x00A2, 0x00A3, 0x00A5, 0x00AC, 0x00AF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x0FF, 0x101, 0x103, 0x105, 0x107, 0x109, 0x10B, 0x10D, 0x10F, 0x111, 0x113, 0x115, 0x117, 0x119, 0x11B, 0x11D, 0x11F, 0x121, 0x123, 0x125, 0x127, 0x129, 0x12B, 0x12D, 0x12F, 0x131, 0x133, 0x135, 0x137, 0x13A, 0x13C, 0x13E, 0x140, 0x142, 0x144, 0x146, 0x148, 0x14B, 0x14D, 0x14F, 0x151, 0x153, 0x155, 0x157, 0x159, 0x15B, 0x15D, 0x15F, 0x161, 0x163, 0x165, 0x167, 0x169, 0x16B, 0x16D, 0x16F, 0x171, 0x173, 0x175, 0x177, 0x17A, 0x17C, 0x17E, 0x192, 0x3B1, 0x3B2, 0x3B3, 0x3B4, 0x3B5, 0x3B6, 0x3B7, 0x3B8, 0x3B9, 0x3BA, 0x3BB, 0x3BC, 0x3BD, 0x3BE, 0x3BF, 0x3C0, 0x3C1, 0x3C3, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 0x3C8, 0x3C9, 0x3CA, 0x430, 0x431, 0x432, 0x433, 0x434, 0x435, 0x436, 0x437, 0x438, 0x439, 0x43A, 0x43B, 0x43C, 0x43D, 0x43E, 0x43F, 0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44A, 0x44B, 0x44C, 0x44D, 0x44E, 0x44F, 0x451, 0x452, 0x453, 0x454, 0x455, 0x456, 0x457, 0x458, 0x459, 0x45A, 0x45B, 0x45C, 0x45E, 0x45F, 0x2170, 0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177, 0x2178, 0x2179, 0x217A, 0x217B, 0x217C, 0x217D, 0x217E, 0x217F, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A, 0 };
	static const WCHAR tbl_upper[] = { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x21, 0xFFE0, 0xFFE1, 0xFFE5, 0xFFE2, 0xFFE3, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0x178, 0x100, 0x102, 0x104, 0x106, 0x108, 0x10A, 0x10C, 0x10E, 0x110, 0x112, 0x114, 0x116, 0x118, 0x11A, 0x11C, 0x11E, 0x120, 0x122, 0x124, 0x126, 0x128, 0x12A, 0x12C, 0x12E, 0x130, 0x132, 0x134, 0x136, 0x139, 0x13B, 0x13D, 0x13F, 0x141, 0x143, 0x145, 0x147, 0x14A, 0x14C, 0x14E, 0x150, 0x152, 0x154, 0x156, 0x158, 0x15A, 0x15C, 0x15E, 0x160, 0x162, 0x164, 0x166, 0x168, 0x16A, 0x16C, 0x16E, 0x170, 0x172, 0x174, 0x176, 0x179, 0x17B, 0x17D, 0x191, 0x391, 0x392, 0x393, 0x394, 0x395, 0x396, 0x397, 0x398, 0x399, 0x39A, 0x39B, 0x39C, 0x39D, 0x39E, 0x39F, 0x3A0, 0x3A1, 0x3A3, 0x3A4, 0x3A5, 0x3A6, 0x3A7, 0x3A8, 0x3A9, 0x3AA, 0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416, 0x417, 0x418, 0x419, 0x41A, 0x41B, 0x41C, 0x41D, 0x41E, 0x41F, 0x420, 0x421, 0x422, 0x423, 0x424, 0x425, 0x426, 0x427, 0x428, 0x429, 0x42A, 0x42B, 0x42C, 0x42D, 0x42E, 0x42F, 0x401, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40A, 0x40B, 0x40C, 0x40E, 0x40F, 0x2160, 0x2161, 0x2162, 0x2163, 0x2164, 0x2165, 0x2166, 0x2167, 0x2168, 0x2169, 0x216A, 0x216B, 0x216C, 0x216D, 0x216E, 0x216F, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F, 0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A, 0 };
	int i;


	for (i = 0; tbl_lower[i] && chr != tbl_lower[i]; i++) ;

	return tbl_lower[i] ? tbl_upper[i] : chr;
}
#endif
# 1 "diskio.cpp"
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


/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN, 2010           */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), sdPoll10mS() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/

#include "OpenAVRc.h"
#include "thirdparty/FatFs/diskio.h"
#include "spi.h"

/* Definitions for MMC/SDC command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK	0x08		/* Block addressing */


/*--------------------------------------------------------------------------

 Module Private Functions

 ---------------------------------------------------------------------------*/
#define spi_tx(value)  master_spi_xfer(value)
#define spi_rx()  master_spi_xfer(0xFF) //-> SD bug without 0xFF.


static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

volatile BYTE Timer1, Timer2;	/* 100Hz decrement timer */

static
BYTE CardType;			/* Card type flags */

void checkMixer()
{
  // TODO duplicated code ...
  uint16_t t0 = getTmr16KHz();
  int16_t delta = (nextMixerEndTime - lastMixerDuration) - t0;
  if ((delta > 0 && delta < (int16_t)US_TO_16KHZ_TICK(MAX_MIXER_DELTA_US)) || (!s_mixer_first_run_done)) return;

  nextMixerEndTime = t0 + US_TO_16KHZ_TICK(MAX_MIXER_DELTA_US);
  doMixerCalculations();

  t0 = getTmr16KHz() - t0;
  lastMixerDuration = t0;
  if (t0 > maxMixerDuration) maxMixerDuration = t0;
}

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

void xmit_spi(uint8_t dat)
{
  checkMixer();
  MYWDT_RESET();
  spi_tx(dat);
}


/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static
BYTE rcvr_spi ()
{
  checkMixer();
  MYWDT_RESET();
  return spi_rx();
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
uint8_t wait_ready ()	/* 1:OK, 0:Timeout */
{
  Timer2 = 5;	/* Wait for ready in timeout of 50ms */
  rcvr_spi();
  do
    if (rcvr_spi() == 0xFF) return 1;
  while (Timer2);

  return 0;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void deselect ()
{
  SDCARD_CS_N_INACTIVE();
  rcvr_spi();
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static
uint8_t select ()	/* 1:Successful, 0:Timeout */
{
  SDCARD_CS_N_ACTIVE();
  if (!wait_ready()) {
    deselect();
    return 0;
  }
  return 1;
}



/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static
uint8_t power_status()		/* Socket power state: 0=off, 1=on */
{
  return 1;
}


static
void power_on ()
{
  for (Timer1 = 2; Timer1; );	// Wait for 20ms
  enable_spi_master_mode();
}



void SD_spi_power_off ()
{
  SDCARD_CS_N_INACTIVE();
  master_spi_disable();
  Stat |= STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
uint8_t rcvr_datablock (
  BYTE *buff,	/* Data buffer to store received data */
  UINT btr	/* Byte count (must be multiple of 4) */
)
{
  BYTE token;


  Timer1 = 2;
  do {				/* Wait for data packet in timeout of 20ms */
    token = rcvr_spi();
  } while ((token == 0xFF) && Timer1);
  if(token != 0xFE) return 0;		/* If not valid data token, retutn with error */

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    do {							/* Receive the data block into buffer */
      //SDCARD_CS_N_ACTIVE();
      *buff++ = spi_rx();
      *buff++ = spi_rx();
      *buff++ = spi_rx();
      *buff++ = spi_rx();
    } while (btr -= 4);
    rcvr_spi();						/* Discard CRC */
    rcvr_spi();
    //SDCARD_CS_N_INACTIVE();
  }
  return 1;						/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

static
uint8_t xmit_datablock (
  const BYTE *buff,	/* 512 byte data block to be transmitted */
  BYTE token		/* Data/Stop token */
)
{

  if (!wait_ready()) return 0;

  xmit_spi(token);	/* Xmit data token */
  if (token != 0xFD) {	/* Is data token */
    BYTE wc = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      do {		/* Xmit the 512 byte data block to MMC */
        xmit_spi(*buff++);
        xmit_spi(*buff++);
      } while (--wc);
      xmit_spi(0xFF);			/* CRC (Dummy) */
      xmit_spi(0xFF);
      BYTE resp = rcvr_spi();		/* Reveive data response */
      if ((resp & 0x1F) != 0x05)	/* If not accepted, return with error */
        return 0;
    }
  }

  return 1;
}



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (		/* Returns R1 resp (bit7==1:Send failed) */
  BYTE cmd,		/* Command index */
  DWORD arg		/* Argument */
)
{
  BYTE n, res;


  if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
    cmd &= 0x7F;
    res = send_cmd(CMD55, 0);
    if (res > 1) return res;
  }

  /* Select the card and wait for ready */
  deselect();
  if (!select()) return 0xFF;

  /* Send command packet */
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    xmit_spi(0x40 | cmd);			/* Start + Command index */
    xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
    xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
    xmit_spi((BYTE)(arg >> 8));		/* Argument[15..8] */
    xmit_spi((BYTE)arg);			/* Argument[7..0] */
    n = 0x01;				/* Dummy CRC + Stop */
    if (cmd == CMD0) n = 0x95;		/* Valid CRC for CMD0(0) */
    if (cmd == CMD8) n = 0x87;		/* Valid CRC for CMD8(0x1AA) */
    xmit_spi(n);
  }

  /* Receive command response */
  if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */
  n = 10;					/* Wait for a valid response in timeout of 10 attempts */
  do
    res = rcvr_spi();
  while ((res & 0x80) && --n);

  return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

 Public Functions

 ---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
  BYTE drv		/* Physical drive nmuber (0) */
)
{
  BYTE n, cmd, ty, ocr[4];


  if (drv) return STA_NOINIT;		/* Supports only single drive */
  if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

  power_on();				/* Force socket power on */
  SPI_250K();
  for (n = 10; n; --n) rcvr_spi();	/* 80 dummy clocks */

  ty = 0;
  if (send_cmd(CMD0, 0) == 1) {		/* Enter Idle state */
    Timer1 = 100;			/* Initialization timeout of 1000 msec */
    if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
      for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();  /* Get trailing return value of R7 resp */
      if (ocr[2] == 0x01 && ocr[3] == 0xAA) {	      /* The card can work at vdd range of 2.7-3.6V */
        while (Timer1 && send_cmd(ACMD41, 1UL << 30));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
        if (Timer1 && send_cmd(CMD58, 0) == 0) {  /* Check CCS bit in the OCR */
          for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
          ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
        }
      }
    } else {							/* SDv1 or MMCv3 */
      if (send_cmd(ACMD41, 0) <= 1) 	{
        ty = CT_SD1;
        cmd = ACMD41;	/* SDv1 */
      } else {
        ty = CT_MMC;
        cmd = CMD1;	/* MMCv3 */
      }
      while (Timer1 && send_cmd(cmd, 0));	/* Wait for leaving idle state */
      if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
        ty = 0;
    }
  }
  CardType = ty;
  deselect();

  if (ty) {			/* Initialization succeded */
    Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT */
    SPI_8M();
  } else {			/* Initialization failed */
    SD_spi_power_off();
  }

  return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
  BYTE drv		/* Physical drive number (0) */
)
{
  if (drv) return STA_NOINIT;		/* Supports only single drive */
  return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
  BYTE drv,			/* Physical drive nmuber (0) */
  BYTE *buff,			/* Pointer to the data buffer to store read data */
  DWORD sector,		/* Start sector number (LBA) */
  UINT count			/* Sector count (1..255) */
)
{
  if (drv || !count) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;

  if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

  if (count == 1) {	/* Single block read */
    if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
        && rcvr_datablock(buff, 512))
      count = 0;
  } else {				/* Multiple block read */
    if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
      do {
        if (!rcvr_datablock(buff, 512)) break;
        buff += 512;
      } while (--count);
      send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
    }
  }
  deselect();

  return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
  BYTE drv,			/* Physical drive nmuber (0) */
  const BYTE *buff,	/* Pointer to the data to be written */
  DWORD sector,		/* Start sector number (LBA) */
  UINT count			/* Sector count (1..255) */
)
{
  if (drv || !count) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  if (Stat & STA_PROTECT) return RES_WRPRT;

  if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

  if (count == 1) {	/* Single block write */
    if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
        && xmit_datablock(buff, 0xFE))
      count = 0;
  } else {				/* Multiple block write */
    if (CardType & CT_SDC) send_cmd(ACMD23, count);
    if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
      do {
        if (!xmit_datablock(buff, 0xFC)) break;
        buff += 512;
      } while (--count);
      if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
        count = 1;
    }
  }
  deselect();

  return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
  BYTE drv,		/* Physical drive nmuber (0) */
  BYTE ctrl,		/* Control code */
  void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT res;
  BYTE n, csd[16], *ptr = (BYTE*)buff;
  WORD csize;


  if (drv) return RES_PARERR;

  res = RES_ERROR;

  if (ctrl == CTRL_POWER) {
    switch (ptr[0]) {
    case 0:		/* Sub control code (POWER_OFF) */
      SD_spi_power_off();		/* Power off */
      res = RES_OK;
      break;
    case 1:		/* Sub control code (POWER_GET) */
      ptr[1] = (BYTE)power_status();
      res = RES_OK;
      break;
    default :
      res = RES_PARERR;
    }
  } else {
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    switch (ctrl) {
    case CTRL_SYNC :		/* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
      if (select()) {
        deselect();
        res = RES_OK;
      }
      break;

    case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
      if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
        if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
          csize = csd[9] + ((WORD)csd[8] << 8) + 1;
          *(DWORD*)buff = (DWORD)csize << 10;
        } else {					/* SDC ver 1.XX or MMC*/
          n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
          csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
          *(DWORD*)buff = (DWORD)csize << (n - 9);
        }
        res = RES_OK;
      }
      break;

    case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
      *(WORD*)buff = 512;
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
      if (CardType & CT_SD2) {	/* SDv2? */
        if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
          rcvr_spi();
          if (rcvr_datablock(csd, 16)) {				/* Read partial block */
            for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
            *(DWORD*)buff = 16UL << (csd[10] >> 4);
            res = RES_OK;
          }
        }
      } else {					/* SDv1 or MMCv3 */
        if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
          if (CardType & CT_SD1) {	/* SDv1 */
            *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
          } else {					/* MMCv3 */
            *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
          }
          res = RES_OK;
        }
      }
      break;

    case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
      *ptr = CardType;
      res = RES_OK;
      break;

    case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
      if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
          && rcvr_datablock(ptr, 16))
        res = RES_OK;
      break;

    case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
      if (send_cmd(CMD10, 0) == 0		/* READ_CID */
          && rcvr_datablock(ptr, 16))
        res = RES_OK;
      break;

    case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
      if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
        for (n = 4; n; n--) *ptr++ = rcvr_spi();
        res = RES_OK;
      }
      break;

    case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
      if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
        rcvr_spi();
        if (rcvr_datablock(ptr, 64))
          res = RES_OK;
      }
      break;

    default:
      res = RES_PARERR;
    }

    deselect();
  }

  return res;
}



/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure                                      */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void sdPoll10ms()
{
  BYTE s;

  if (Timer1) --Timer1;
  if (Timer2) --Timer2;

  s = Stat;
  s &= ~STA_NODISK;
  s &= ~STA_PROTECT;
  Stat = s;
}

bool sdMounted()
{
  return (g_FATFS_Obj.fs_type != 0);
}

void sdMountPoll()
{
  uint8_t mountTimer = 100;
  do {
    if (!sdMounted()) {
      f_mount(&g_FATFS_Obj, "", 1);
      f_chdir(ROOT_PATH);
    } else break;
  } while (--mountTimer);

  if (!sdMounted()) {
    SD_spi_power_off();
    POPUP_WARNING(STR_SDCARD_ERROR);
  }
}

FATFS g_FATFS_Obj;
