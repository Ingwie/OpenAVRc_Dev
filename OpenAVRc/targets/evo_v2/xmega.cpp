/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2018).
 *
 */


#include "../../i2c_master.h"


bool read_keyb_matrix(void);
void adcInit(void);
void read_A8_A10(void);
void read_A9_A11(void);


inline void boardInit()
{
// Setup Clock System.
// Using 4MHz External Crystal.
#define RTCOUT  PIN6_bm
#define CLKOUT  PIN7_bm
#define OSC_XOSCPWR_bm  0x10  /* 16 MHz Crystal Oscillator High Power mode bit mask. */
#define OSC_XOSCPWR_bp  4  /* 16 MHz Crystal Oscillator High Power mode bit position. */

  // Output RTC clock on PC6, Output ClkPER on PC7.
  // PORTCFG.CLKEVOUT = RTCOUT | (0b00<<2) | PORTCFG_CLKOUT_PC7_gc;
  // PORTC.DIRSET = RTCOUT | CLKOUT;

  OSC.XOSCCTRL = OSC_FRQRANGE_2TO9_gc; //Select XOSC 2-9MHz.
  //OSC.XOSCCTRL = OSC_XOSCSEL_32KHz_gc; // Select 32768Hz TOSC.
  OSC.XOSCCTRL |= OSC_XOSCSEL_XTAL_16KCLK_gc; // XTAL Startup time.
  OSC.XOSCCTRL |= (0<<OSC_X32KLPM_bp) | (0<<OSC_XOSCPWR_bp); // high drive on TOSC, low drive on XOSC.

  OSC.CTRL |= OSC_XOSCEN_bm; // Enable the XOSC (TOSC).
  while(!(OSC.STATUS & OSC_XOSCRDY_bm)); // Wait for XOSC (TOSC) to stabilise.

//OSC.DFLLCTRL = OSC_RC32MCREF_XOSC32K_gc | ( 1<<OSC_RC2MCREF_bp ); // Select TOSC as reference for RC32M & RC2M.
//DFLLRC32M.CTRL = DFLL_ENABLE_bm; // Enable DFLL.
//DFLLRC2M.CTRL = DFLL_ENABLE_bm; // Enable DFLL.

//while(!(OSC.STATUS & OSC_RC2MRDY_bm)); // Wait for 2MHz oscillator to stabilise.

//OSC.CTRL |= OSC_RC32MEN_bm; // Enable the internal 32MHz oscillator.
//while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // Wait for 32MHz oscillator to stabilise.

  OSC.PLLCTRL = (OSC_PLLSRC_XOSC_gc) | (8<<OSC_PLLFAC_gp); // Select PLL source, PLL*8 ..32MHz.
  while(!(OSC.STATUS & OSC_PLLRDY_bm))
  OSC.CTRL |= OSC_PLLEN_bm;  // Enable PLL.

//  CCP = CCP_IOREG_gc; // Disable register security for clock update (0xD8).
_PROTECTED_WRITE(CLK.PSCTRL, 0); // No prescaling.
//  CLK.PSCTRL = 0; // No prescaling.
// TODO CCP function.
//  CCP = CCP_IOREG_gc; // Disable register security for clock update (0xD8).
_PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_PLL_gc); // Select PLL source.
//  CLK.CTRL = CLK_SCLKSEL_PLL_gc; // Select PLL source.

  OSC.CTRL &= ~OSC_RC2MEN_bm; // Disable 2MHz oscillator which was enabled after reset.

  // Lock the clock configuration.
  // CCP = CCP_IOREG_gc; // Disable register security for clock update (0xD8).
  //TODO Check this
_PROTECTED_WRITE(CLK.LOCK, (CLK.LOCK |= CLK_LOCK_bm));
  // CLK.LOCK |= CLK_LOCK_bm;

  EEPROM_EnableMapping();

  // SETUP PINS
  // *** SAFER to use DIRSET DIRCLR OUTSET OUTCLR ***

  //  Trims Button  Matrix.
  PORTE.DIRCLR = I_E_TRIM_COL_A | I_E_TRIM_COL_B | I_E_TRIM_COL_C | I_E_TRIM_COL_D;
  PORTE.PIN4CTRL = PORT_OPC_PULLUP_gc; // I_E_TRIM_COL_A
  PORTE.PIN5CTRL = PORT_OPC_PULLUP_gc; // I_E_TRIM_COL_B
  PORTE.PIN6CTRL = PORT_OPC_PULLUP_gc; // I_E_TRIM_COL_C
  PORTE.PIN7CTRL = PORT_OPC_PULLUP_gc; // I_E_TRIM_COL_D

  PORTF.DIRSET = O_F_TRIM_ROW_A;
  PORTF.OUTCLR = O_F_TRIM_ROW_A;

  // NOTE : In WIREDAND with optional PULLUP.
  // OUT DIR
  //  0   0   output driven low.
  //  1   0   low output disconnected (optionally pulled up).

  adcInit();
  getADC(); // Best to get some values before we start.

//Setup TCC1
//  COUNTER_31250HZ
  COUNTER_31250HZ.CTRLB = 0b000 << TC0_WGMODE_gp; // Mode = NORMAL.
  COUNTER_31250HZ.CTRLC = 0;
  COUNTER_31250HZ.CTRLD = 0;
  COUNTER_31250HZ.CTRLE = 0;
  COUNTER_31250HZ.CNT = 0;
  COUNTER_31250HZ.CCA = 312; // 312 or 313.
  COUNTER_31250HZ_CLEAR_CCAIF_FLAG;
  RESUME_10MS_INTERRUPT;
  COUNTER_31250HZ.CTRLA = 0b0111 << TC0_CLKSEL_gp; // ClkPER/1024 32MHz/1024.

//Setup TCx0 for RF Module use.
  RF_TC.CTRLB = 0b000 << TC0_WGMODE_gp; // Mode = NORMAL.
  RF_TC.CTRLC = 0;
  RF_TC.CTRLD = 0;
  RF_TC.CTRLE = 0;
  RF_TC.CNT = 0;
  RF_TIMER_CLEAR_CCAIF_FLAG;
  RF_TC.CTRLA = 0b0100 << TC0_CLKSEL_gp; // ClkPER/8 32MHz/8.

//Setup TCx0 for Trainer pulses.
  TRAINER_TC.CTRLB = 0b000 << TC0_WGMODE_gp; // Mode = NORMAL.
  TRAINER_TC.CTRLC = 0;
  TRAINER_TC.CTRLD = 0;
  TRAINER_TC.CTRLE = 0;
  TRAINER_TC.CNT = 0;
  TRAINER_TC.CTRLA = 0b0100 << TC0_CLKSEL_gp; // ClkPER/8 32MHz/8.

#if defined(AUDIO)
/*
 * Setup TCx0 for audio tone generation.
 * Peripheral clock = 32MHz.
 * Use FRQ waveform generation mode.
 */
  AUDIO_TC.CTRLB = 0b001 << TC0_WGMODE_gp; // Mode = FRQ.
  AUDIO_TC.CTRLC = 0;
  AUDIO_TC.CTRLD = 0;
  AUDIO_TC.CTRLE = 0;
  AUDIO_TC.CNT = 0;
  AUDIO_TC.CCA = 1;
  AUDIO_TC.CTRLA = 0b0101 << TC0_CLKSEL_gp; // ClkPER/1 32MHz/1.
  PORTD.DIRSET = O_D_AUDIO;
#endif

#if defined(ROTARY_ENCODERS)
// Rotary encoder interrupt setup for Evo digi-adjusters.
// TODO Disable ALL external interrupts ?

  // Setup pins.
  PORTF.PIN4CTRL = PORT_OPC_PULLUP_gc;
  PORTF.PIN5CTRL = PORT_OPC_PULLUP_gc;
  PORTF.PIN6CTRL = PORT_OPC_PULLUP_gc;
  PORTF.PIN7CTRL = PORT_OPC_PULLUP_gc;
  // Encoder REa RH
  PORTF.PIN4CTRL |= PORT_ISC_FALLING_gc;
  PORTF.PIN5CTRL |= PORT_ISC_BOTHEDGES_gc;
  // Encoder REb LH
  PORTF.PIN6CTRL |= PORT_ISC_FALLING_gc;
  PORTF.PIN7CTRL |= PORT_ISC_BOTHEDGES_gc;

  PORTF.INTFLAGS = 0x3; // Clear flags.

  // Low priority.
  PORTF.INTCTRL = PORT_INT1LVL_LO_gc | PORT_INT0LVL_LO_gc;

  PORTF.INT0MASK = I_F_ROT_ENC_LH_CLK;
  PORTF.INT1MASK = I_F_ROT_ENC_LH_DIR;
//  PORTF.INT0MASK = I_F_ROT_ENC_RH_CLK;
//  PORTF.INT1MASK = I_F_ROT_ENC_RH_DIR;

#endif

#if defined(SDCARD)
  // Remap SPIC MOSI and SCK pins.
  #if !defined(PORTC_REMAP)
  #define PORTC_REMAP  _SFR_MEM8(0x064E) //
  #endif
  #if !defined(PORT_SPI_bm)
  #define PORT_SPI_bm  0x20  /* SPI bit mask. */
  #endif

  SDCARD_PORT.REMAP |= PORT_SPI_bm; // Swap MOSI and SCK.
  SDCARD_PORT.OUTSET = O_SDCARD_CS_N;
  SDCARD_PORT.DIRSET = O_SDCARD_CS_N | PIN5_bm | PIN7_bm; // CS SCK MOSI as output.
#endif

#if defined(RTCLOCK) || defined(EXTERNALEEPROM)
  // Init TWI Hardware.
  i2c_init();
#endif

#if defined(PWM_BACKLIGHT)
// PIN C2.
  /*
   * Setup TCC2 for PWM Backlight.
   * Peripheral clock = 32MHz.
   * Uses single slope PWM; Down counting.
   */

  TCC2.CTRLE = TC_BYTEM_SPLITMODE_gc; // Two 8 bit timer counters.
  TCC2.CTRLB = PIN2_bm;
  TCC2.LPER = 255;
  TCC2.LCMPC = 127;
  TCC2.CTRLA = TC2_CLKSEL_DIV1_gc; // << TC0_CLKSEL_gp; // ClkPER/64 32MHz/64.
#endif

#if defined(VOICE_JQ6500)
  PORTB.DIRCLR = VOICE_BUSY_PIN;
  PORTB.PIN7CTRL = PORT_OPC_PULLUP_gc; // Pullup on Busy pin.

  PORTC.OUTSET = VOICE_TX_PIN; // Marking state.
  PORTC.DIRSET = VOICE_TX_PIN;

  InitJQ6500UartTx();
 #endif

#if defined(SPIMODULES)
// Setup USARTxn for MSPI.
  rf_usart_mspi_init();
  protoMode = NORMAL_MODE;
#endif

  PMIC.CTRL |= PMIC_LOLVLEN_bm;  // Enable Low Priority Interrupts.
  PMIC.CTRL |= PMIC_MEDLVLEN_bm; // Enable Medium Priority Interrupts.
  PMIC.CTRL |= PMIC_HILVLEN_bm;  // Enable High Priority Interrupts. e.g. RF Pulses.


  PWR_HOLD_PORT.DIRSET = O_B_PWR_HOLD; // Output.
  PWR_HOLD_PORT.OUTSET = O_B_PWR_HOLD; // Hold power on.
  PWR_HOLD_PORT.PIN6CTRL = PORT_OPC_PULLUP_gc; // I_B_PWR_STATUS //TODO check

  PORTC.DIRSET = O_C_PWR_LED;
  PORTC.OUTSET = O_C_PWR_LED;
}



#if defined(SPIMODULES)
char rf_usart_mspi_xfer(char c)
{
  WAIT_RF_BUFFER_EMPTY();
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      RF_USART.DATA = c;
      RF_USART.STATUS = USART_TXCIF_bm | USART_RXCIF_bm; // Clear USART transmit complete flag.
    }
  WAIT_RF_TX_FIN();
return RF_USART.DATA;
}


void rf_usart_mspi_init()
{
  // Setup pin states and USART in MSPI mode.

  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();
  RF_CS_NRF24L01_INACTIVE();
  RF_CS_A7105_INACTIVE();

  RF_PORT.DIRSET = RF_TX_PIN | RF_CS_PIN | RF_CK_PIN;
  RF_PORT.DIRCLR = RF_RX_PIN;

  // Initialisation of USART in MSPI mode.

  // fBAUD = ClkPER / (2 * (BSEL + 1) )
  RF_USART.BAUDCTRLA = 4; // 3.2MHz @ ClkPER = 32MHz
  RF_USART.BAUDCTRLB = 0; // BSCALE=0

  // USART in Master SPI mode, MSB first, Clock rising = Sample. Clock falling = Setup.
  RF_USART.CTRLA = 0; // Disable interrupts.
  RF_USART.CTRLC = (USART_CMODE_MSPI_gc) | (0b00 << 1); // MSB first, UCPHA = 0. Check INVEN = 0.
  RF_USART.CTRLB = USART_TXEN_bm; // Transmit and Receive.todo
}
#endif




void backlightFade(void)
{
// ToDo ... We can decrement the compare register until the target is reached.
// At the moment we just have a backlight brightness of two levels.
  // TCC2.LCMPC = TCC2.LCMPC -1;
}


void boardOff(void)
{
  PWR_HOLD_PORT.OUTCLR = O_B_PWR_HOLD;
}


//extern bool pwrCheck;

void Check_PWR_Switch(void)
{
  static tmr10ms_t last10ms;

  if( ! (PWR_STATUS_PORT & I_B_PWR_STATUS ) ) {
  // PWR Switch is on.
  last10ms = get_tmr10ms();
  }
  // PWR Switch is off.
  else if( get_tmr10ms() - last10ms > 200 ) {
    pwrCheck = 0; // 2 seconds.
  }
  else;
}


bool switches[NUM_SW - SW_BASE]; // Switches via R-2R ladder DAC.


bool switchState(enum EnumKeys enuk)
{
// Returns the state of the enum parameter (keys, trims, rotary encoder buttons and switches).
  if(enuk < NUM_KEYS)
    return keys[enuk].state() ? 1 : 0; // Keys, trims and rotary encoder buttons.

  else if(enuk < NUM_SW)
    return switches[enuk - SW_BASE]; // Switches.

  else
    return 0;
}


uint8_t keyDown()
{
// Returns 0 for no key pressed. Non-zero value is not checked elsewhere.
return trimDown((uint8_t) 0); // ToDo PATCH.
//return 0;
}

enum {
  TRIMS_LHD,
  TRIMS_LHU,
  TRIMS_LVD,
  TRIMS_LVU,
  TRIMS_RVD,
  TRIMS_RVU,
  TRIMS_RHD,
  TRIMS_RHU
};

// Trim switches
#if defined(TOGGLETRIM) //Toggle trim usage -> Left trim for right stick and right trim for left stick
static const pm_uchar crossTrim[] PROGMEM = {
  TRIMS_RHL, //TODO
  TRIMS_RHR,
  TRIMS_RVD,
  TRIMS_RVU,
  TRIMS_LVD,
  TRIMS_LVU,
  TRIMS_LHL,
  TRIMS_LHR
};
#else
static const pm_uchar crossTrim[] PROGMEM = { // Done
  TRIMS_RHU,
  TRIMS_RVD,
  TRIMS_RHD,
  TRIMS_RVU,
  TRIMS_LHU,
  TRIMS_LVD,
  TRIMS_LHD,
  TRIMS_LVU
};
#endif


uint8_t trimDown(uint8_t idx) // Could return bool.
{
// Return value is only interpreted as bool.
//  return keys[TRM_BASE + pgm_read_byte_near(crossTrim+idx)].state() ? 1 : 0;
  return keys[TRM_BASE + idx].state() ? 1 : 0;
}


void read_trim_matrix()
{
  if(PORTF.OUT & O_F_TRIM_ROW_A) {
    keys[KEY_RIGHT].input( (PORTE.IN & I_E_TRIM_COL_A)  ? 0 :1);
    keys[KEY_DOWN].input( (PORTE.IN & I_E_TRIM_COL_B)  ? 0 :1);
    keys[KEY_LEFT].input( (PORTE.IN & I_E_TRIM_COL_C)  ? 0 :1);
    keys[KEY_UP].input( (PORTE.IN & I_E_TRIM_COL_D)  ? 0 :1);

//keys[TRM_BASE + pgm_read_byte_far(crossTrim+0)].input( (PORTE.IN & I_E_TRIM_COL_A)  ? 0 :1);
//keys[TRM_BASE + pgm_read_byte_far(crossTrim+1)].input( (PORTE.IN & I_E_TRIM_COL_B)  ? 0 :1);
//keys[TRM_BASE + pgm_read_byte_far(crossTrim+2)].input( (PORTE.IN & I_E_TRIM_COL_C)  ? 0 :1);
//keys[TRM_BASE + pgm_read_byte_far(crossTrim+3)].input( (PORTE.IN & I_E_TRIM_COL_D)  ? 0 :1);

  } else {
    keys[KEY_MENU].input( (PORTE.IN & I_E_TRIM_COL_B)  ? 0 :1);
    keys[KEY_EXIT].input( (PORTE.IN & I_E_TRIM_COL_D)  ? 0 :1);

keys[TRM_BASE + pgm_read_byte_far(crossTrim+4)].input( (PORTE.IN & I_E_TRIM_COL_A)  ? 0 :1);
//keys[TRM_BASE + pgm_read_byte_far(crossTrim+5)].input( (PORTE.IN & I_E_TRIM_COL_B)  ? 0 :1);
keys[TRM_BASE + pgm_read_byte_far(crossTrim+6)].input( (PORTE.IN & I_E_TRIM_COL_C)  ? 0 :1);
//keys[TRM_BASE + pgm_read_byte_far(crossTrim+7)].input( (PORTE.IN & I_E_TRIM_COL_D)  ? 0 :1);
  }

  PORTF.OUTTGL = O_F_TRIM_ROW_A;
}


void readKeysAndTrims()
{
// Encoder Buttons and switches.
  getADC(); // See if this can be removed.
  read_A8_A10();
  read_A9_A11();
// Keyboard ToDo
// Multiplexed trim buttons.
  read_trim_matrix();
}


/*
The Evo has two switch modules which convert the various switch states into an analogue voltages.
Each module has 2 * Push button switches, 2 * Two position switches and 2 * Three position switches.
Rather than reverse engineer the circuits, the four analogue voltages were fed into the ADC and a state table
was created. The switch circuits appear to be R-2R DAC's.
The array below details the nominal voltage thresholds from the switch modules.
The #define ANA_TOL provides a transition zone around each voltage.

Switches H I K RHencoder feed A9
Switches G J feed A8
Switches M N P LHencoder feed A11
Switches L O feed A10
*/

#define ANA_TOL 40

static const uint16_t AnalSwitchVal[16] PROGMEM = {
  // AD reading, switch state bitmap.
  //        ENC 2  Position  3   pos
  //        R/L K/P H/M I/N  O/J L/G
  0x398c, // 1   1   1   0
  0x3a1e, // 1   1   1   1
  0x3b03, // 1   1   0   0
  0x3b93, // 1   1   0   1
  0x3c9c, // 1   0   1   0
  0x3d32, // 1   0   1   1   DN  DN
  0x3e19, // 1   0   0   0   DN  UP
  0x3eb3, // 1   0   0   1   DN  MI
  0x3fd7, // 0   1   1   0
  0x406e, // 0   1   1   1   UP  DN
  0x4154, // 0   1   0   0   UP  UP
  0x41ef, // 0   1   0   1   UP  MI
  0x4303, // 0   0   1   0
  0x439e, // 0   0   1   1   MI  DN
  0x448b, // 0   0   0   0   MI  UP
  0x4527, // 0   0   0   1   MI  MI
          // 8   4   2   1
};

void read_A9_A11(void)
{
  uint16_t AD_value;
  uint16_t upper;
  uint16_t lower;

  AD_value = s_anaFilt[ANAL_SWS_2];

  if((unsigned)AD_value + ANA_TOL < AD_value)
    upper = 0x47FF;
  else upper = AD_value + ANA_TOL;

  if((unsigned)AD_value - ANA_TOL > AD_value)
    lower = 0x3800;
  else lower = AD_value - ANA_TOL;

  for(uint8_t x=0; x<16; x++) {
    if( pgm_read_word_far(&AnalSwitchVal[x]) < upper && pgm_read_word_far(&AnalSwitchVal[x]) > lower ) {
      switches[SW_H - SW_BASE] = ( (x & 2) ? 0 : 1); // H
      switches[SW_I - SW_BASE] = ( (x & 1) ? 1 : 0); // I
      switches[SW_K - SW_BASE] = ( (x & 4) ? 1 : 0); // K
      keys[BTN_REb].input( (x & 8) ? 0 : 1); // EncRight
    break;
    }
  }

  AD_value = s_anaFilt[ANAL_SWS_4];

  if((unsigned)AD_value + ANA_TOL < AD_value)
    upper = 0x47FF;
  else upper = AD_value + ANA_TOL;

  if((unsigned)AD_value - ANA_TOL > AD_value)
    lower = 0x3800;
  else lower = AD_value - ANA_TOL;

  for(uint8_t x=0; x<16; x++) {
    if( pgm_read_word_far(&AnalSwitchVal[x]) < upper && pgm_read_word_far(&AnalSwitchVal[x]) > lower ) {
      switches[SW_M - SW_BASE] = ( (x & 2) ? 0 : 1); // M
      switches[SW_N - SW_BASE] = ( (x & 1) ? 1 : 0); // N
      switches[SW_P - SW_BASE] = ( (x & 4) ? 1 : 0); // P
      keys[BTN_REa].input( (x & 8) ? 0 : 1); // EncLeft
    break;
    }
  }
}


void read_A8_A10(void)
{
  uint16_t AD_value;
  uint16_t upper;
  uint16_t lower;

  AD_value = s_anaFilt[ANAL_SWS_1];

  if((unsigned)AD_value + ANA_TOL < AD_value)
    upper = 0x47FF;
  else upper = AD_value + ANA_TOL;

  if((unsigned)AD_value - ANA_TOL > AD_value)
    lower = 0x3800;
  else lower = AD_value - ANA_TOL;

  for(uint8_t x=5; x<16; x++) {
    if( pgm_read_word_far(&AnalSwitchVal[x]) < upper && pgm_read_word_far(&AnalSwitchVal[x]) > lower ) {
      switches[SW_Jdn - SW_BASE] = ( (x==5 || x==6 || x==7) ? 1 : 0); // J Down
      switches[SW_Jup - SW_BASE] = ( (x==9 || x==10 || x==11) ? 1 : 0); // J Up
      switches[SW_Jmi - SW_BASE] = ( (x==13 || x==14 || x==15) ? 1 : 0); // J Middle

      switches[SW_Gdn - SW_BASE] = ( (x==5 || x==9 || x==13) ? 1 : 0); // G Down
      switches[SW_Gup - SW_BASE] = ( (x==6 || x==10 || x==14) ? 1 : 0); // G Up
      switches[SW_Gmi - SW_BASE] = ( (x==7 || x==11 || x==15) ? 1 : 0); // G Middle
    break;
    }
  }

  AD_value = s_anaFilt[ANAL_SWS_3];

  if((unsigned)AD_value + ANA_TOL < AD_value)
    upper = 0x04FF;
  else upper = AD_value + ANA_TOL;

  if((unsigned)AD_value - ANA_TOL > AD_value)
    lower = 0X3800;
  else lower = AD_value - ANA_TOL;

  for(uint8_t x=0; x<16; x++) {
    if( pgm_read_word_far(&AnalSwitchVal[x]) < upper && pgm_read_word_far(&AnalSwitchVal[x]) > lower ) {
      switches[SW_Odn - SW_BASE] = ( (x==5 || x==6 || x==7) ? 1 : 0); // O Down
      switches[SW_Oup - SW_BASE] = ( (x==9 || x==10 || x==11) ? 1 : 0); // O Up
      switches[SW_Omi - SW_BASE] = ( (x==13 || x==14 || x==15) ? 1 : 0); // O Middle

      switches[SW_Ldn - SW_BASE] = ( (x==5 || x==9 || x==13) ? 1 : 0); // L Down
      switches[SW_Lup - SW_BASE] = ( (x==6 || x==10 || x==14) ? 1 : 0); // L Up
      switches[SW_Lmi - SW_BASE] = ( (x==7 || x==11 || x==15) ? 1 : 0); // L Middle
    break;
    }
  }
}



ISR(TIMER_10MS_VECT, ISR_NOBLOCK)
{
#if defined(SIMU)
ISR10msLoop_is_runing = true;
#endif


static uint8_t accuracyWarble = 0;

TIMER_10MS_COMPVAL += (accuracyWarble++ & 0b01) ? 313 : 312;

#if defined(AUDIO)
  AUDIO_HEARTBEAT();
#endif

#if defined(BUZZER)
  BUZZER_HEARTBEAT();
#endif

#if defined(HAPTIC)
  HAPTIC_HEARTBEAT();
#endif

  Check_PWR_Switch();

  SIMU_PROCESSEVENTS;

  per10ms();

#if defined(SIMU)
ISR10msLoop_is_runing = false;
#endif
}


uint16_t getTmr31250Hz()
{
  return COUNTER_31250HZ.CNT;
}


bool check_slave_mode(void)
{
#if 0
  /*
   * This quirky function reads the mpx_rf_en signal which goes
   * to the mpx module and the trainer DIN.
   * If it ever reads a low, it stores the low in the port register.
   * All further reads are low because the pin is then configured as an output.
  */
  if(MPX_PORT.IN & IO_E_MPX_RF_EN) {
    return 0;
  }
  else {
    MPX_PORT.OUT &= ~IO_E_MPX_RF_EN; // Pin is low.
    MPX_PORT.DIR |= IO_E_MPX_RF_EN; // Pin is output.
  }
#endif
    return 1;
}


/*
The Rotary Encoder (Digi-adjuster) ISR's.
There is hardware filtering of the signals,
however the ISR's debounce the encoder signals by working alternately.
e.g.
INT4 & 5 work as a pair for Encoder B.
INT6 & 7 work as a pair for Encoder A.

EICRB  = (0b10 << ISC60) | (0b10 << ISC40); // 10 = interrupt on -ve edge.
EICRB  = (0b11 << ISC60) | (0b11 << ISC40); // 11 = interrupt on +ve edge.
*/
#if 0
ISR(PORTE_INT0_vect) // REb (LH)
{
  if(PORTE.PIN4CTRL & PORT_ISC_RISING_gc) { // If +ve edge triggered change to -ve.
    PORTE.PIN4CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
    if(PORTE.IN & I_E_ROT_ENC_LH_DIR) incRotaryEncoder(1, -1);
    else incRotaryEncoder(1, +1);
  }
  else PORTE.PIN4CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;
  // Disables itself and enables the other.
  PORTE.INTCTRL = PORT_INT1LVL_LO_gc | PORT_INT0LVL_OFF_gc;
  PORTE.INTFLAGS = 0x3; // Clear flags.
}

ISR(PORTE_INT1_vect) // REb (LH) Level change ISR.
{
  // Disables itself and enables the other.
  PORTE.INTCTRL = PORT_INT1LVL_OFF_gc | PORT_INT0LVL_LO_gc;
  PORTE.INTFLAGS = 0x3; // Clear flags.
}
#endif

ISR(PORTF_INT0_vect) // REa (RH)
{
  if(PORTF.PIN6CTRL & PORT_ISC_RISING_gc) {
    PORTF.PIN6CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
    if(PORTF.IN & I_F_ROT_ENC_RH_DIR) incRotaryEncoder(0, -1);
    else incRotaryEncoder(0, +1);
  }
  else PORTF.PIN6CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;
  // Disables itself and enables the other.
  PORTF.INTCTRL = PORT_INT1LVL_LO_gc | PORT_INT0LVL_OFF_gc;
  PORTF.INTFLAGS = 0x3; // Clear flags.
}

ISR(PORTF_INT1_vect) // REa (RH) Level change ISR.
{
  // Disables itself and enables the other.
  PORTF.INTCTRL = PORT_INT1LVL_OFF_gc | PORT_INT0LVL_LO_gc;
  PORTF.INTFLAGS = 0x3; // Clear flags.
}


#if defined(VOICE)
//ToDo
#endif

