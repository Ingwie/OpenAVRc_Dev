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


#ifndef evo_mega2560_h
#define evo_mega2560_h

#include "../../OpenAVRc.h"

#define FATFSTINY // Reduce SDdriver buffer size

//Mixer
  #if   F_CPU == 16000000UL
  #define HALF_MICRO_SEC_COUNTS(half_us) (half_us)
  #elif F_CPU == 32000000UL
  #define HALF_MICRO_SEC_COUNTS(half_us) (half_us *2)
  #else
  #define HALF_MICRO_SEC_COUNTS(half_us) (((F_CPU/800)*(half_us))/20000)
  #endif


//Xmitter
#define SPIMODULES // This needs to be in the makefile based upon a build option e.g. SPI_XMITTER ?
#define PROTO_HAS_CC2500 // This needs to be in the makefile based upon a build option e.g. SPI_XMITTER ?
#define PROTO_HAS_CYRF6936


//SUPIIIK FILE
#define MULTIMODULE
#if defined (MULTIMODULE)
#define PROTO_HAS_MULTISUPIIIK
#endif
//SUPIIIK FILE

#define CALCULATE_LAT_JIT()  dt = TCNT1 - OCR1A // Calculate latency and jitter.

#if defined(SPIMODULES)
  #define OUT_C_RF_CS_N       PIN4_bm
  #define RF_CS_CC2500_ACTIVE()     PORTC &= ~(OUT_C_RF_CS_N)
  #define RF_CS_CC2500_INACTIVE()   PORTC |=  (OUT_C_RF_CS_N)
  #define RF_CS_CYRF6936_ACTIVE()   PORTC &= ~(OUT_C_RF_CS_N)
  #define RF_CS_CYRF6936_INACTIVE() PORTC |=  (OUT_C_RF_CS_N)

  uint8_t USART3_mspi_xfer(uint8_t data);
  #define RF_SPI_xfer  USART3_mspi_xfer

#endif // SPIMODULES

//#define GPIO_SET(sfr, bit) (sfr) |=  (1<<(bit))
//#define GPIO_CLR(sfr, bit) (sfr) &= ~(1<<(bit))
//#define GPIO_GET(sfr, bit) (sfr) &   (1<<(bit))

// Switch cross mapping
#define SW_ID0 SW_Jup
#define SW_ID1 SW_Jmi
#define SW_ID2 SW_Jdn

#define SW_GEA SW_Gdn
#define SW_RUD SW_Ldn
#define SW_ELE SW_Odn

#define SW_THR SW_N
#define SW_AIL SW_I
#define SW_TRN SW_M

#define GETADC_COUNT 1  // Used in switches.cpp
//#define MUX_MAX 1 // Used in switches.cpp

// USB fake driver
//#define usbPlugged()             false
//#define INP_E_USB_D_PLS          4
//#define INP_E_USB_D_NEG          2

// Buzzer driver
#define OUT_B_BUZZER               PIN4_bm
#define buzzerOn()                 PORTB |=  (OUT_B_BUZZER)
#define buzzerOff()                PORTB &= ~(OUT_B_BUZZER)

// Audio - Piezo Sounder driver
#define OUT_H_Speaker              PIN3_bm
#define speakerOn()                TCCR4A |=  (0b01 << COM4A0)
#define speakerOff()               TCCR4A &= ~(0b01 << COM4A0)

#define OUT_B_PPMSIM               PIN5_bm // OC1A
#define OUT_B_PPM_B                PIN6_bm // OC1B
#define INP_D_PPM_IN               PIN4_bm // ICP1

// Trainer
#define OUT_G_SIM_CTL                PIN0_bm
#define ACTIVE_PPM_OUT()             PORTG |= (OUT_G_SIM_CTL)  // Pupil
#define ACTIVE_PPM_IN()              PORTG &= ~(OUT_G_SIM_CTL) // Master
#define ENABLE_TRAINER_INTERRUPT()   TIMSK1 |= (1<<ICIE1);   // Enable ICP Interrupt.
#define DISABLE_TRAINER_INTERRUPT()  TIMSK1 &= ~(1<<ICIE1);  // Disable ICP Interrupt.
#define WAIT_PUPIL()                ENABLE_TRAINER_INTERRUPT(); ACTIVE_PPM_IN()
#define PPM16_CONF()                DISABLE_TRAINER_INTERRUPT(); ACTIVE_PPM_OUT()
#define IS_WAIT_PUPIL_STATE()       ((g_model.rfProtocol == (PROTOCOL_PPM16-1)) || (g_model.rfProtocol == (PROTOCOL_PPMSIM-1)))


#define IO_J_MPX_RF_EN             PIN2_bm

// Backlight driver
#define OUT_C_LIGHT                PIN5_bm
#define backlightEnable()          PORTC |= (OUT_C_LIGHT)
#define backlightDisable()         PORTC &= ~(OUT_C_LIGHT)
#define isBacklightEnable()        PORTC & (OUT_C_LIGHT)


#if defined(VOICE)
#define OUT_H_14DRESET             3
#define OUT_H_14DCLK               4
#define OUT_H_14DDATA              5
#define INP_H_14DBUSY              6
#endif

// Keyboard Matrix.
#define DDR_KEYB_MATRIX_ROW        DDRJ // Use as open drain
#define OUT_J_KEYB_ROW_A           PIN4_bm
#define OUT_J_KEYB_ROW_B           PIN5_bm
#define OUT_J_KEYB_ROW_C           PIN6_bm
#define OUT_J_KEYB_ROW_D           PIN7_bm
#define GPIO_KEYB_MATRIX_COL       PINK // Use as input pullup
#define INP_K_KEYB_COL_A           PIN5_bm
#define INP_K_KEYB_COL_B           PIN6_bm
#define INP_K_KEYB_COL_C           PIN7_bm

// Serial Telemetry on USART0 (also serial bootloader) - done
#define INP_E_TELEM_RX             0
#define OUT_E_TELEM_TX             1

// Trim Buttons
#define GPIO_TRIMS                 PINA
#define TRIMS_GPIO_PIN_LH_PO       PIN0_bm //left hand -port
#define TRIMS_GPIO_PIN_LH_DN       PIN1_bm //-down
#define TRIMS_GPIO_PIN_LH_UP       PIN2_bm //-up
#define TRIMS_GPIO_PIN_LH_ST       PIN3_bm //-starboard
#define TRIMS_GPIO_PIN_RH_DN       PIN4_bm //right hand -down
#define TRIMS_GPIO_PIN_RH_PO       PIN5_bm
#define TRIMS_GPIO_PIN_RH_ST       PIN6_bm
#define TRIMS_GPIO_PIN_RH_UP       PIN7_bm
#define TRIMS_PRESSED()            (~GPIO_TRIMS)


#define TIMER_16KHZ_VECT           TIMER2_OVF_vect
#define COUNTER_16KHZ              TCNT2
#define TIMER_10MS_VECT            TIMER2_COMPA_vect
#define TIMER_10MS_COMPVAL         OCR2A
#define PAUSE_10MS_INTERRUPT()     TIMSK2 &= ~(1<<OCIE2A)
#define RESUME_10MS_INTERRUPT()    TIMSK2 |= (1<<OCIE2A)


// SD Card driver
#define sdDone()
#define SD_IS_HC()                (0)
#define SD_GET_SPEED()            (0)
#if defined(CPUM2560)
// SPI Speeds.
#define SPI_250K() { SPSR &= ~(1<<SPI2X); SPCR = (SPCR | 0b10) & 0b11111110; }
#define SPI_500K() { SPSR |=  (1<<SPI2X); SPCR = (SPCR | 0b10) & 0b11111110; }
#define SPI_4M() { SPSR &= ~(1<<SPI2X); SPCR &= 0b11111100; }
#define SPI_8M() { SPSR |=  (1<<SPI2X); SPCR &= 0b11111100; }
#endif
#define SDCARD_CS_N_ACTIVE()        PORTB &= ~PIN0_bm // MMC CS = L
#define SDCARD_CS_N_INACTIVE()      PORTB |= PIN0_bm // MMC CS = H
#define SDCARD_CS_N_IS_INACTIVE()   (PINB & PIN0_bm)
bool sdMounted();
void sdMountPoll();
void sdPoll10ms();
#define SPI_SLOW()  SPI_250K()
#define SPI_FAST()  SPI_4M()

#if 0
// Keys driver
#define KEYS_PRESSED()             (~PINL) // used only for DBLKEYS code.
#define DBLKEYS_PRESSED_RGT_LFT(i) (false)
#define DBLKEYS_PRESSED_UP_DWN(i)  (false)
#define DBLKEYS_PRESSED_RGT_UP(i)  (false)
#define DBLKEYS_PRESSED_LFT_DWN(i) (false)
#endif


// Power driver - done
#define GPIO_C_PWR_HOLD            PORTC
#define OUT_C_PWR_HOLD             PIN2_bm
#define GPIO_C_PWR_STATUS          PINC
#define INP_C_PWR_STATUS           PIN3_bm
#define GPIO_C_PWR_LED             PORTC
#define OUT_C_PWR_LED              PIN4_bm // Used for CC2500 chip select.

void pwrOff();
#if defined(PWRMANAGE)
  #define UNEXPECTED_SHUTDOWN()    ((mcusr & (1 << WDRF)) || g_eeGeneral.unexpectedShutdown)
#else
  #define UNEXPECTED_SHUTDOWN()    (mcusr & (1 << WDRF))
#endif

// Haptic driver // todo
#define OUT_D_HAPTIC               7
#define hapticOff()
#define HAPTIC_ON()
#define HAPTIC_OFF()

// Rotary encoder driver - done
#define GPIO_ROT_ENCS              PINE
#define INP_E_ROT_ENC_RH_INT       PIN4_bm
#define INP_E_ROT_ENC_RH_DIR       PIN5_bm
#define INP_E_ROT_ENC_LH_INT       PIN6_bm
#define INP_E_ROT_ENC_LH_DIR       PIN7_bm

#define REA_DOWN()                 switchState((EnumKeys)BTN_REa)
#define REB_DOWN()                 switchState((EnumKeys)BTN_REb)
#define ROTENC_DOWN()              (REA_DOWN() || REB_DOWN())

// I2C - done
#define INP_D_I2C_SCL              0
#define INP_D_I2C_SDA              1

// EEPROM driver
#if !defined(SIMU)
#if defined(EXTERNALEEPROM)
#define ADDRESS_EXTERN_EEPROM  (0x50 << 1) //0x50 with all strap closed on ZS042 module (or alone), EEPROM FM24W256, see datasheet
#define eepromReadBlock(a, b, c)   Ext_eeprom_read_block(a, b, c) //External EEPROM
#else
#define eepromReadBlock(a, b, c)   eeprom_read_block(a, (const void *)b, c) //Internal EEPROM
#endif
#endif // evo_mega2560_h

#endif // evo_mega2560_h
