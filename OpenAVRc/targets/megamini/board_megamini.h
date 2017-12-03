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


#ifndef _BOARD_MEGAMINI_H_
#define _BOARD_MEGAMINI_H_

#include "../../board_avr.h"

#define CALCULATE_LAT_JIT()  dt = TCNT1 - OCR1A // Calculate latency and jitter.

//Mods
#define FATFSTINY // Reduce SDdriver buffer size

// Keys
void readKeysAndTrims();
#define GPIO_KEYS                 PINL
#define KEYS_GPIO_PIN_MENU        PIN4_bm
#define KEYS_GPIO_PIN_EXIT        PIN5_bm
#define KEYS_GPIO_PIN_RIGHT       PIN2_bm
#define KEYS_GPIO_PIN_LEFT        PIN3_bm
#define KEYS_GPIO_PIN_UP          PIN1_bm
#define KEYS_GPIO_PIN_DOWN        PIN0_bm
// DBLKeys driver
#define KEYS_PRESSED()            (~GPIO_KEYS)

// Trims
#define GPIO_TRIMS                PINF
#define TRIMS_GPIO_PIN_LHL        PIN7_bm
#define TRIMS_GPIO_PIN_LVD        PIN5_bm
#define TRIMS_GPIO_PIN_RVU        PIN2_bm
#define TRIMS_GPIO_PIN_RHL        PIN1_bm
#define TRIMS_GPIO_PIN_LHR        PIN6_bm
#define TRIMS_GPIO_PIN_LVU        PIN4_bm
#define TRIMS_GPIO_PIN_RVD        PIN3_bm
#define TRIMS_GPIO_PIN_RHR        PIN0_bm
#define TRIMS_PRESSED()           (~GPIO_TRIMS)

#define TIMER_16KHZ_VECT          TIMER2_OVF_vect
#define COUNTER_16KHZ             TCNT2
#define TIMER_10MS_VECT           TIMER2_COMPA_vect
#define TIMER_10MS_COMPVAL        OCR2A

// Backlight driver
#define backlightEnable()         PORTC |= (1<<OUT_C_LIGHT)
#define backlightDisable()        PORTC &= ~(1<<OUT_C_LIGHT)
#define isBacklightEnable()       PORTC & (1<<OUT_C_LIGHT)

// SD driver
#define SDCARD_CS_N_ACTIVE()        PORTB &= ~PIN0_bm // MMC CS = L
#define SDCARD_CS_N_INACTIVE()      PORTB |= PIN0_bm // MMC CS = H
#define SDCARD_CS_N_IS_INACTIVE()   (PINB & PIN0_bm)
#define SPI_250K() { SPSR = _BV(SPI2X); SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0); }
#define SPI_8M()   { SPSR = _BV(SPI2X); SPCR = _BV(SPE) | _BV(MSTR); }

// Switchs driver
#define INP_C_ID2                 PIN1_bm
#define INP_C_ID1                 PIN0_bm
#define INP_D_AileDR              PIN7_bm
#define INP_G_ThrCt               PIN2_bm
#define INP_G_Gear                PIN1_bm
#define INP_G_RuddDR              PIN0_bm
#define INP_L_ElevDR              PIN6_bm
#define INP_L_Trainer             PIN7_bm

// Servitudes driver
#define INP_D_PPM_IN              4    // ICP1
#define OUT_B_PPM                 6    // Master_PPM_out OC1A
#define OUT_B_PPM16_SIM           5    // OC1B
#define I_O_B_UNUSED              4    // unused was Buzzer
#define INP_D_I2C_SCL             1
#define INP_D_I2C_SDA             0
#define INP_E_TELEM_RX            1
#define OUT_E_TELEM_TX            0
//#define INP_H_RF_Activated        6
//#define INP_H_DSC_Activated       5    //not used, reserved for pwrCheck()
#define INP_H_Hold_Power          PIN4_bm
#define OUT_H_SpeakerBuzzer       3
#define OUT_H_PPM16_SIM_CTL       6
#define OUT_J_JQ_SERIAL           1    //JQ6500
#define INP_J_JQ_BUSY             0    //JQ6500
#define OUT_H_HAPTIC              5

//Puppy Master Mode
#define ACTIVE_PPM_OUT()            PORTH &= ~(1<<OUT_H_PPM16_SIM_CTL)
#define ACTIVE_PPM_IN()             PORTH |= (1<<OUT_H_PPM16_SIM_CTL)
#define ENABLE_TRAINER_INTERRUPT()  TIMSK1 |= (1<<ICIE1);  // Enable ICP Interrupt.
#define DISABLE_TRAINER_INTERRUPT() TIMSK1 &= ~(1<<ICIE1); // Disable ICP Interrupt.
#define WAIT_PUPIL()                ENABLE_TRAINER_INTERRUPT(); ACTIVE_PPM_IN()
#define PPM16_CONF()                DISABLE_TRAINER_INTERRUPT(); ACTIVE_PPM_OUT()
#define IS_WAIT_PUPIL_STATE()       ((g_model.rfProtocol == (PROTOCOL_PPM16-1)) || (g_model.rfProtocol == (PROTOCOL_PPMSIM-1)))

// Rotary encoders driver
#define INP_E_ROT_ENC_1_A         PIN4_bm
#define INP_E_ROT_ENC_1_B         PIN5_bm
#define ROT_ENC_1_MASK            (INP_E_ROT_ENC_1_A | INP_E_ROT_ENC_1_B)
#define INP_E_ROT_ENC_2_A         PIN7_bm
#define INP_E_ROT_ENC_2_B         PIN6_bm
#define ROT_ENC_2_MASK            (INP_E_ROT_ENC_2_A | INP_E_ROT_ENC_2_B)
#define INP_G_ROT_ENC_1_PUSH      PIN5_bm
#define INP_E_ROT_ENC_2_PUSH      PIN3_bm
#define REA_DOWN()                (~PING & INP_G_ROT_ENC_1_PUSH)
#define REB_DOWN()                (~PINE & INP_E_ROT_ENC_2_PUSH)
#define ROTENC_DOWN()             (REA_DOWN() || REB_DOWN())
#define ENABLEROTENCAISR()        EIMSK |= (1<<INT5) | (1<<INT4) // enable the rot. enc.A ext. int.
#define ENABLEROTENCBISR()        EIMSK |= (1<<INT7) | (1<<INT6) // enable the rot. enc.B ext. int.
#define DISABLEROTENCAISR()       EIMSK &= (~((1<<INT5) | (1<<INT4))) // disable the rot.A enc. ext. int.
#define DISABLEROTENCBISR()       EIMSK &= (~((1<<INT7) | (1<<INT6))) // disable the rot.B enc. ext. int.

// LCD driver
#define PORTA_LCD_DAT            PORTA
#define PORTC_LCD_CTRL           PORTC
#if defined(LCD_KS108)              // (For KS108 LCD only) MEGA R/W pin always at 0 state in OpenAVRc then
  #define OUT_C_LCD_CS2          6  // use this pin to control second KS108 (CS2)
#else                               // and connect KS108 R/W pin to ground via a 1k resistor
  #define OUT_C_LCD_RnW          6
#endif
#define OUT_C_LCD_E              7
#define OUT_C_LCD_A0             5
#define OUT_C_LCD_RES            4
#define OUT_C_LCD_CS1            3
#define OUT_C_LIGHT              2

// Power driver
void boardOff();
#if defined(PWRMANAGE)
  #define UNEXPECTED_SHUTDOWN()   ((mcusr & (1 << WDRF)) || g_eeGeneral.unexpectedShutdown)
#else
  #define UNEXPECTED_SHUTDOWN()   (mcusr & (1 << WDRF))
#endif

// Haptic driver
#define hapticOn()                PORTH |=  (1 << OUT_H_HAPTIC)
#define hapticOff()               PORTH &= ~(1 << OUT_H_HAPTIC)

// Buzzer driver
#define buzzerOn()                PORTH |=  (1 << OUT_H_SpeakerBuzzer)
#define buzzerOff()               PORTH &= ~(1 << OUT_H_SpeakerBuzzer)

// Speaker driver
#if defined(AUDIO) && !defined(SIMU)
  #define speakerOff()              TCCR4A &= ~(1 << COM4A0)
  #define speakerOn()               TCCR4A |=  (1 << COM4A0)
#endif

// Voice driver

//JQ6500
#define JQ6500_BUSY                   (PINJ & (1<<INP_J_JQ_BUSY))
#if defined(VOICE_JQ6500)
  extern void InitJQ6500UartTx();
#endif

// EEPROM driver
#if !defined(SIMU)

  #if defined(EXTERNALEEPROM)
    #define ADDRESS_EXTERN_EEPROM  (0x50 << 1) //0x50 with all strap closed on ZS042 module (or alone), EEPROM FM24W256, see datasheet
    #define eepromReadBlock(a, b, c)   Ext_eeprom_read_block(a, b, c) //External EEPROM
  #else
    #define eepromReadBlock(a, b, c)   eeprom_read_block(a, (const void *)b, c) //Internal EEPROM
  #endif
#else
  extern void boardInit();
  extern ISR(INT7_vect);
  extern ISR(INT6_vect);
  extern ISR(INT4_vect);
  extern ISR(INT5_vect);
#endif

//Mixer
#define HALF_MICRO_SEC_COUNTS(half_us) (half_us)

//SUPIIIK FILE
#define MULTIMODULE
#if defined (MULTIMODULE)
  #define PROTO_HAS_MULTISUPIIIK
#endif
//SUPIIIK FILE

//Xmitter
#define SPIMODULES

#if defined(SPIMODULES)

  #define PROTO_HAS_CC2500 // This needs to be in the makefile based upon a build option e.g. SPI_XMITTER ?
  #define PROTO_HAS_CYRF6936
  #define TX_FIXED_ID 0x1a2b3c4d // Loaded in SpiRFModule.fixed_id

  uint8_t USART2_mspi_xfer(uint8_t data);
  #define RF_SPI_xfer  USART2_mspi_xfer
  #define OUT_J_CC2500_CS_N       PIN3_bm
  #define OUT_J_CYRF6936_CS_N     PIN4_bm
  #define OUT_J_NRF24L01_CS_N     PIN5_bm
  #define OUT_J_A7105_CS_N        PIN6_bm

  #define RF_CS_CC2500_ACTIVE() PORTJ &= ~(OUT_J_CC2500_CS_N)
  #define RF_CS_CC2500_INACTIVE() PORTJ |= (OUT_J_CC2500_CS_N)
  #define RF_CS_CYRF6936_ACTIVE() PORTJ &= ~(OUT_J_CYRF6936_CS_N)
  #define RF_CS_CYRF6936_INACTIVE() PORTJ |= (OUT_J_CYRF6936_CS_N)
  #define RF_CS_NRF24L01_ACTIVE() PORTJ &= ~(OUT_J_NRF24L01_CS_N)
  #define RF_CS_NRF24L01_INACTIVE() PORTJ |= (OUT_J_NRF24L01_CS_N)
  #define RF_CS_A7105_ACTIVE() PORTJ &= ~(OUT_J_A7105_CS_N)
  #define RF_CS_A7105_INACTIVE() PORTJ |= (OUT_J_A7105_CS_N)

#endif // SPIMODULES

#endif // _BOARD_MEGAMINI_H_
