/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


#ifndef evo_mega2560_h
#define evo_mega2560_h

#include "../../OpenAVRc.h"

#define lcdRefreshFast() lcdRefresh()

#ifndef PIN0_bm
#define PIN0_bm  0x01
#endif
#ifndef PIN1_bm
#define PIN1_bm  0x02
#endif
#ifndef PIN2_bm
#define PIN2_bm  0x04
#endif
#ifndef PIN3_bm
#define PIN3_bm  0x08
#endif
#ifndef PIN4_bm
#define PIN4_bm  0x10
#endif
#ifndef PIN5_bm
#define PIN5_bm  0x20
#endif
#ifndef PIN6_bm
#define PIN6_bm  0x40
#endif
#ifndef PIN7_bm
#define PIN7_bm  0x80
#endif

#if 0
// For Evo Tx
enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
  POT1, // SLIDER1,
  POT2, // SLIDER2,
  POT_LAST = POT2,
  TX_VOLTAGE,
  ANALOG_A7,
  ANALOG_A8,
  ANALOG_A9,
  ANALOG_A10,
  NUMBER_ANALOG
};
#endif

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


#define OUT_B_PPM                  6
#define OUT_L_CPPM_OC5A            PIN3_bm // OC5A
#define OUT_L_CPPM_TRN             PIN4_bm // OC5B
#define INP_L_PPM_IN               PIN1_bm // ICP5

// Trainer
#define OUT_B_SIM_CTL              PIN5_bm // done
#define JACK_PPM_OUT()             PORTB |= (OUT_B_SIM_CTL)
#define JACK_PPM_IN()              PORTB &= ~(OUT_B_SIM_CTL)

#define IO_J_MPX_RF_EN             PIN2_bm
bool check_slave_mode(void);
#define SLAVE_MODE()               check_slave_mode()

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
#define PAUSE_PPMIN_INTERRUPT()    TIMSK5 &= ~(1<<ICIE5) // Timer 5
#define RESUME_PPMIN_INTERRUPT()   TIMSK5 |= (1<<ICIE5) // Timer 5

// SD Card driver
#define sdDone()
#define SD_IS_HC()                 (0)
#define SD_GET_SPEED()             (0)
#if !defined(SIMU)
bool sdMounted();
void sdMountPoll();
void sdPoll10ms();
#endif

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
#define OUT_C_PWR_LED              PIN4_bm // Used for debug.

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
#define eepromReadBlock(a, b, c)   eeprom_read_block(a, (const void *)b, c)
#endif

#endif // evo_mega2560_h
