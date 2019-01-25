/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2018).
 *
 */


#ifndef xmega_board_h
#define xmega_board_h

#include "../../OpenAVRc.h"

#define GETADC_COUNT 1  // Used in switches.cpp

// USART General defines.
#define USART_SET_BAUD_9K6(usartx)   _USART_SET_BAUD(usartx, 12, +4)
#define USART_SET_BAUD_57K6(usartx)  _USART_SET_BAUD(usartx, 135,-2)
#define USART_SET_BAUD_100K(usartx)  _USART_SET_BAUD(usartx, 159,-3)
#define USART_SET_BAUD_125K(usartx)  _USART_SET_BAUD(usartx, 127,-3)

#define _USART_SET_BAUD(usartx, bsel, bscale) \
  { usartx.BAUDCTRLA = bsel & 0xFF; usartx.BAUDCTRLB = ((int8_t) bscale << USART_BSCALE_gp) | (bsel >> 8); }
#define USART_SET_MODE_8N1(usartx) \
  { usartx.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | (0 << USART_SBMODE_bp) | USART_CHSIZE_8BIT_gc; } // 8N1
#define USART_SET_MODE_8E2(usartx) \
  { usartx.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_EVEN_gc | (1 << USART_SBMODE_bp) | USART_CHSIZE_8BIT_gc; } // 8E2

#define USART_ENABLE_TX(usartx)  usartx.CTRLB |= USART_TXEN_bm;
#define USART_ENABLE_RX(usartx)  usartx.CTRLB |= USART_RXEN_bm;

#define USART_XCK_PIN  PIN1_bm
#define USART_RXD_PIN  PIN2_bm
#define USART_TXD_PIN  PIN3_bm

#if defined (MULTIMODULE)
#define PROTO_HAS_MULTISUPIIIK
#endif
// Using USARTD0 which is the Telemetry / Bootloader USART.
// Should be on USARTE0 which is the USART used for MSPI of SPIMODULES.
// All communication to RF module(s) will use the same pin ... e.g. USART MSPI, USART Asynch and PPM.
#define TLM_USART           MULTI_USART
#define MULTI_USART         USARTD0
#define TLM_USART_PORT      PORTD
#define TLM_USART_RXC_VECT  USARTD0_RXC_vect
#define TLM_USART_DRE_VECT  USARTD0_DRE_vect


// RF Module Timer counter.
#define RF_TC                      TCE0
#define RF_PORT                    PORTE
#define RF_TIMER_COMPA_VECT        TCE0_CCA_vect
#define RF_TIMER_COMPA_REG         RF_TC.CCA
#define RF_TIMER_PAUSE_INTERRUPT   RF_TC.INTCTRLB &= ~TC0_CCAINTLVL_gm;
#define RF_TIMER_RESUME_INTERRUPT  RF_TC.INTCTRLB |=  (0b11 << TC0_CCAINTLVL_gp); // Level 3 - High Priority.
#define RF_TIMER_CLEAR_COMPA_FLAG  RF_TC.INTFLAGS = TC1_CCAIF_bm; // clear ccaif.


//#define HALF_MICRO_SEC_COUNTS(half_us) (half_us)
//#define HALF_MICRO_SEC_COUNTS(half_us) (((F_CPU/800)*(half_us))/20000)


#define CALCULATE_LAT_JIT()  dt = (RF_TC.CNT - RF_TIMER_COMPA_REG ) // Calculate latency and jitter.

#if defined(SPIMODULES) // PORTE 0
char rf_usart_mspi_xfer(char c);
#define RF_SPI_xfer  rf_usart_mspi_xfer
void rf_usart_mspi_init(void);

#define RF_USART   USARTE0
#define RF_CS_PIN  PIN0_bm

#define RF_CS_CC2500_ACTIVE()     PORTE.OUTCLR = RF_CS_PIN
#define RF_CS_CC2500_INACTIVE()   PORTE.OUTSET = RF_CS_PIN
#define RF_CS_CYRF6936_ACTIVE()   PORTE.OUTCLR = RF_CS_PIN
#define RF_CS_CYRF6936_INACTIVE() PORTE.OUTSET = RF_CS_PIN
#define RF_CS_NRF24L01_ACTIVE()   PORTE.OUTCLR = RF_CS_PIN
#define RF_CS_NRF24L01_INACTIVE() PORTE.OUTSET = RF_CS_PIN
#define RF_CS_A7105_ACTIVE()      PORTE.OUTCLR = RF_CS_PIN
#define RF_CS_A7105_INACTIVE()    PORTE.OUTSET = RF_CS_PIN

#define WAIT_RF_BUFFER_EMPTY()    while(! (RF_USART.STATUS & USART_DREIF_bm) )
#define WAIT_RF_TX_FIN()          while(! (RF_USART.STATUS & USART_TXCIF_bm) )
#define WAIT_RF_RX_FIN()          while(! (RF_USART.STATUS & USART_RXCIF_bm) )
#endif // SPIMODULES


// Trainer Pulses
#define TRAINER_TC         TCF0
#define TRAINER_TC_VECT    TCF0_CCC_vect
#define TRAINER_TC_REG     TRAINER_TC.CCC

#define OUT_B_SIM_CTL
#define JACK_PPM_OUT()
#define JACK_PPM_IN()

#define OUT_B_PPM
#define OUT_L_CPPM_OC5A
#define OUT_L_CPPM_TRN
#define INP_L_PPM_IN

#define SLAVE_MODE()	(false)
#define ENABLE_TRAINER_INTERRUPT()   (false) // Enable ICP Interrupt.
#define DISABLE_TRAINER_INTERRUPT()  (false) // Disable ICP Interrupt.
#define IS_WAIT_PUPIL_STATE()       ((g_model.rfProtocol == (PROTOCOL_PPM16)) || (g_model.rfProtocol == (PROTOCOL_PPMSIM)))

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


// MPX / Trainer DIN
#define MPX_USART       USARTE0
#define MPX_PORT        PORTE
#define IO_E_MPX_RF_EN  PIN1_bm

// Buzzer driver
//#if defined(BUZZER)
#define buzzerOn()
#define buzzerOff()
//#endif

// Audio - Piezo Sounder driver
#define AUDIO_TC             TCD0
#define AUDIO_COMPARE_REG    AUDIO_TC.CCABUF
#define speakerOn()   AUDIO_TC.CTRLB |= TC0_CCAEN_bm
#define speakerOff()  AUDIO_TC.CTRLB &= ~TC0_CCAEN_bm
#define O_D_AUDIO            PIN0_bm

#if defined(VOICE_JQ6500)
#define VOICE_USART       USARTC0
#define VOICE_USART_PORT  PORTC
#define VOICE_BUSY_PIN    PIN7_bm
#define JQ6500_BUSY       (0) //(PORTB.IN & VOICE_BUSY_PIN)
#define VOICE_DRE_VECT    USARTC0_DRE_vect
extern void InitJQ6500UartTx();
#endif

// Trims Button  Matrix.
#define I_E_TRIM_COL_A  PIN4_bm
#define I_E_TRIM_COL_B  PIN5_bm
#define I_E_TRIM_COL_C  PIN6_bm
#define I_E_TRIM_COL_D  PIN7_bm
#define O_F_TRIM_ROW_A  PIN0_bm

// Serial Telemetry on USART0
//#define INP_E_TELEM_RX	0
//#define OUT_E_TELEM_TX	1

// Trim Buttons
void read_trim_matrix(void);
#define TRIMS_PRESSED() false

#if 0
// Keys driver
#define KEYS_PRESSED()  (~PINL)
#define DBLKEYS_PRESSED_RGT_LFT(i)  (false)
#define DBLKEYS_PRESSED_UP_DWN(i)   (false)
#define DBLKEYS_PRESSED_RGT_UP(i)   (false)
#define DBLKEYS_PRESSED_LFT_DWN(i)  (false)
#endif


#define MS064_TC                   TCC1
#define COUNTER_64uS               MS064_TC.CNT
//#define TIMER_10MS_VECT            TCC1_CCA_vect
#define TIMER_10MS_COMPVAL         MS064_TC.CCA
#define PAUSE_10MS_INTERRUPT       MS064_TC.INTCTRLB &= ~TC1_CCAINTLVL_gm;
#define RESUME_10MS_INTERRUPT      MS064_TC.INTCTRLB |=  (0b01 << TC1_CCAINTLVL_gp); // Level 1 - Low Priority.
#define MS064_TC_CLEAR_CCAIF_FLAG  MS064_TC.INTFLAGS = TC1_CCAIF_bm; // clear ccaif.


// SD Card driver
#define sdDone()
#define SD_IS_HC()              (0)
#define SD_GET_SPEED()          (0)
#define SDCARD_SPI              SPIC
#define SDCARD_PORT             PORTC
#define O_SDCARD_CS_N           PIN4_bm
#define select_card()           SDCARD_PORT.OUTCLR = O_SDCARD_CS_N
#define SDCARD_CS_N_ACTIVE()    SDCARD_PORT.OUTCLR = O_SDCARD_CS_N
#define unselect_card()         SDCARD_PORT.OUTSET = O_SDCARD_CS_N
#define SDCARD_CS_N_INACTIVE()  SDCARD_PORT.OUTSET = O_SDCARD_CS_N
#define SPI_8M()                MSPI_16M(SDCARD_SPI)
#define SPI_250K()              MSPI_250K(SDCARD_SPI)
#define SPI_START_SPEED()       SPI_250K()
#define LEDON()
#define LEDOFF()

// SPI general.
#define MSPI_250K(spix)  { spix.CTRL &= ~(SPI_CLK2X_bm); spix.CTRL |= 0b11 << SPI_PRESCALER_gp; }
#define MSPI_500K(spix)  { spix.CTRL |=  (SPI_CLK2X_bm); spix.CTRL |= 0b11 << SPI_PRESCALER_gp; }
#define MSPI_2M(spix)    { spix.CTRL &= ~(SPI_CLK2X_bm | SPI_PRESCALER1_bm); spix.CTRL |= SPI_PRESCALER0_bm; }
#define MSPI_4M(spix)    { spix.CTRL |=  (SPI_CLK2X_bm | SPI_PRESCALER0_bm); spix.CTRL &= ~SPI_PRESCALER1_bm; }
#define MSPI_8M(spix)    { spix.CTRL &= ~(SPI_CLK2X_bm); spix.CTRL &= ~(0b11 << SPI_PRESCALER_gp); }
#define MSPI_16M(spix)   { spix.CTRL |=  (SPI_CLK2X_bm); spix.CTRL &= ~(0b11 << SPI_PRESCALER_gp); }

// Power driver
#define PWR_HOLD_PORT      PORTB
#define O_B_PWR_HOLD       PIN5_bm
#define PWR_STATUS_PORT    PORTB.IN
#define I_B_PWR_STATUS     PIN6_bm
#define PWR_LED_PORT       PORTC
#define O_C_PWR_LED        PIN2_bm

// Power LED / Backlight driver.
#define BACKLIGHT_PORT     PORTC
#define O_C_BACKLIGHT      PIN2_bm
#if !defined(PWM_BACKLIGHT)
#define BACKLIGHT_ON()  (BACKLIGHT_PORT.OUTSET = O_C_BACKLIGHT)
#define BACKLIGHT_OFF() (BACKLIGHT_PORT.OUTCLR = O_C_BACKLIGHT)
#else
#define BACKLIGHT_ON()  (TCC2.LCMPC = ( (uint8_t) g_eeGeneral.blOnBright)  <<4)
#define BACKLIGHT_OFF() (TCC2.LCMPC = ( (uint8_t) g_eeGeneral.blOffBright) <<4)
#endif

#if defined(PWRMANAGE)
#define UNEXPECTED_SHUTDOWN()    ((mcusr & (RST_WDRF_bm)) || g_eeGeneral.unexpectedShutdown)
#else
#define UNEXPECTED_SHUTDOWN()    (mcusr & (RST_WDRF_bm))
#endif

// Rotary encoder driver.
#define I_F_ROT_ENC_LH_CLK       PIN4_bm
#define I_F_ROT_ENC_LH_DIR       PIN5_bm
#define I_F_ROT_ENC_RH_CLK       PIN6_bm
#define I_F_ROT_ENC_RH_DIR       PIN7_bm

#define REA_DOWN()	switchState((EnumKeys)BTN_REa)
#define REB_DOWN()	switchState((EnumKeys)BTN_REb)
#define ROTENC_DOWN() (REA_DOWN() || REB_DOWN())

#define ENABLEROTENCAISR()   {}
#define ENABLEROTENCBISR()   {}
#define DISABLEROTENCAISR()  {}
#define DISABLEROTENCBISR()  {}

// TWI / I2C
#define FRAM_RTC_TWI            TWIC
#define FRAM_RTC_TWI_TWIM_VECT  TWIC_TWIM_vect

// EEPROM driver
#define EEPROM_EnableMapping()   NVM.CTRLB |= NVM_EEMAPEN_bm
#define EEPROM_DisableMapping()  NVM.CTRLB &= ~NVM_EEMAPEN_bm
#define NVM_EXEC()              _PROTECTED_WRITE(NVM.CTRLA, 0b1);
void EEPROM_WaitForNVM(void);
void EEPROM_FlushBuffer(void);



#if !defined(SIMU)
#if defined(EXTERNALEEPROM)
#define ADDRESS_EXTERN_EEPROM  (0x50 << 1) // 0x50 with all strap closed on ZS042 module (or alone), EEPROM FM24W256, see datasheet
#define EEPROMREADBLOCK(a, b, c)   Ext_eeprom_read_block(a, b, c) //External EEPROM
#else
#define EEPROMREADBLOCK(a, b, c) \
  memcpy(a, (const void *) (b + MAPPED_EEPROM_START), c) // Xmega EEPROM is memory mapped for read access.
#endif

#endif // ! SIMU

#endif // xmega_board_h

