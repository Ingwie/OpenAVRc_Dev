/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2018).
 *
 */


#include "../../i2c_master.h"


void adcInit(void);
void read_sws_1(void);
void read_sws_2(void);
void read_sws_3(void);
void read_sws_4(void);
void read_keyboard(void);


inline void boardInit()
{
// Setup Clock System.
// Using 4MHz External Crystal.

#define OSC_XOSCPWR_bm  0x10  /* 16 MHz Crystal Oscillator High Power mode bit mask. */
#define OSC_XOSCPWR_bp  4  /* 16 MHz Crystal Oscillator High Power mode bit position. */

  OSC.XOSCCTRL = OSC_FRQRANGE_2TO9_gc; // Select XOSC 2-9MHz.
  //OSC.XOSCCTRL = OSC_XOSCSEL_32KHz_gc; // Select 32768Hz TOSC.
  OSC.XOSCCTRL |= OSC_XOSCSEL_XTAL_16KCLK_gc; // XTAL Startup time.
  OSC.XOSCCTRL |= (0<<OSC_X32KLPM_bp) | (0<<OSC_XOSCPWR_bp); // High drive on TOSC, Low drive on XOSC.

  OSC.CTRL |= OSC_XOSCEN_bm; // Enable the XOSC (TOSC).
  while(!(OSC.STATUS & OSC_XOSCRDY_bm)); // Wait for XOSC (TOSC) to stabilise.

//OSC.DFLLCTRL = OSC_RC32MCREF_XOSC32K_gc | ( 1<<OSC_RC2MCREF_bp ); // Select TOSC as reference for RC32M & RC2M.
//DFLLRC32M.CTRL = DFLL_ENABLE_bm; // Enable DFLL.
//DFLLRC2M.CTRL = DFLL_ENABLE_bm; // Enable DFLL.

//while(!(OSC.STATUS & OSC_RC2MRDY_bm)); // Wait for 2MHz oscillator to stabilise.

//OSC.CTRL |= OSC_RC32MEN_bm; // Enable the internal 32MHz oscillator.
//while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // Wait for 32MHz oscillator to stabilise.

  OSC.PLLCTRL = (OSC_PLLSRC_XOSC_gc) | (8<<OSC_PLLFAC_gp); // Select PLL source, PLL*8 ..32MHz.

  OSC.CTRL |= OSC_PLLEN_bm;  // Enable PLL.
  while(!(OSC.STATUS & OSC_PLLRDY_bm));

  _PROTECTED_WRITE(CLK.PSCTRL, 0); // No prescaling.

  _PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_PLL_gc); // Select PLL as source.

  OSC.CTRL &= ~OSC_RC2MEN_bm; // Disable 2MHz oscillator which was enabled after reset.

  // Lock the clock configuration.
  _PROTECTED_WRITE(CLK.LOCK, (CLK.LOCK |= CLK_LOCK_bm));

#if 0
  OSC.CTRL |= OSC_RC32MEN_bm; // Enable the internal 32MHz oscillator.
  while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // Wait for 32MHz oscillator to stabilise.

  _PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_RC32M_gc); // Select RC32MHz as source.
#endif

//#define CLOCK_OUT
#if defined (CLOCK_OUT)
  // Output ClkPER on PC7.
  PORTCFG.CLKEVOUT = PORTCFG_CLKOUT_PC7_gc;
  PORTC.DIRSET = PIN7_bm;
  while (1);
#endif

  EEPROM_EnableMapping();

  // SETUP PINS
  // *** SAFER to use DIRSET DIRCLR OUTSET OUTCLR ***

#if defined(VOICE)
  VOICE_BUSY_PORT.DIRCLR = 1 << VOICE_BUSY_PIN;
  VOICE_BUSY_PIN_CTRL_REG = PORT_OPC_PULLUP_gc; // Pullup on Busy pin.
  VOICE_USART_PORT.OUTSET = USART_TXD_PIN_bm; // Marking state.
  VOICE_USART_PORT.DIRSET = USART_TXD_PIN_bm;
#endif

  //  Trims Button  Matrix.
  PORTE.DIRCLR = IE_TRIM_COL_A_bm | IE_TRIM_COL_B_bm | IE_TRIM_COL_C_bm | IE_TRIM_COL_D_bm;
  PORTE.PIN4CTRL = PORT_OPC_PULLUP_gc; // I_E_TRIM_COL_A
  PORTE.PIN5CTRL = PORT_OPC_PULLUP_gc; // I_E_TRIM_COL_B
  PORTE.PIN6CTRL = PORT_OPC_PULLUP_gc; // I_E_TRIM_COL_C
  PORTE.PIN7CTRL = PORT_OPC_PULLUP_gc; // I_E_TRIM_COL_D

  PORTF.DIRSET = OF_TRIM_ROW_A_bm;
  PORTF.OUTCLR = OF_TRIM_ROW_A_bm;

  // NOTE : In WIREDAND with optional PULLUP.
  // OUT DIR
  //  0   0   output driven low.
  //  1   0   low output disconnected (optionally pulled up).

  adcInit();
  getADC(); // Best to get some values before we start.
  read_sws_1();
  read_sws_2();
  read_sws_3();
  read_sws_4();

  lcdInit();
  lcdClear();

// Setup Event System to generate 64us pulses for compatibility with Mega2560.
  EVSYS.CH0MUX = 0x80 + 11;  // ClkPER / (2^11) ... /2048.

// Setup TCC1 for 64us / 10ms.
  MS064_TC.CTRLB = 0b000 << TC0_WGMODE_gp; // Mode = NORMAL.
  MS064_TC.CTRLC = 0;
  MS064_TC.CTRLD = 0;
  MS064_TC.CTRLE = 0;
  MS064_TC.CNT = 0;
  MS064_TC.CCA = 157; // 156 or 157.
  MS064_TC_CLEAR_CCAIF_FLAG;
  RESUME_10MS_INTERRUPT;
  MS064_TC.CTRLA = 8 + 0 ; // Event channel 0 (prescaler of 2048).

// Setup Event System to generate 2MHz pulses for compatibility with Mega2560.
  EVSYS.CH1MUX = 0x80 + 4;  // ClkPER / (2^4) ... /16.

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
  PORTD.DIRSET = OD_AUDIO_bm;
#endif

//#if defined(ROTARY_ENCODERS)
// Rotary encoder interrupt setup for Evo digi-adjusters.

  // Setup pins.
  // Encoder REa LH
  ROT_ENC_LH_CLK_CTRL_REG |= PORT_ISC_FALLING_gc;
  // Encoder REb RH
  ROT_ENC_RH_CLK_CTRL_REG |= PORT_ISC_FALLING_gc;

  ROT_ENC_PORT.INTFLAGS = PORT_INT1IF_bm | PORT_INT0IF_bm; // Clear flags.

  // Low priority.
  ROT_ENC_PORT.INTCTRL = PORT_INT1LVL_LO_gc | PORT_INT0LVL_LO_gc;

  ROT_ENC_PORT.INT0MASK = IF_ROT_ENC_LH_CLK_bm;
  ROT_ENC_PORT.INT1MASK = IF_ROT_ENC_RH_CLK_bm;
//#endif

#if defined(SDCARD)
  // Remap SPIC MOSI and SCK pins.
  #if !defined(PORTC_REMAP)
  #define PORTC_REMAP  _SFR_MEM8(0x064E) //
  #endif
  #if !defined(PORT_SPI_bm)
  #define PORT_SPI_bm  0x20  /* SPI bit mask. */
  #endif

  SDCARD_PORT.REMAP |= PORT_SPI_bm; // Swap MOSI and SCK.
  SDCARD_PORT.OUTSET = O_SDCARD_CSN_bm;
  SDCARD_PORT.DIRSET = O_SDCARD_CSN_bm | PIN5_bm | PIN7_bm; // CS SCK MOSI as output.
#endif

#if defined(RTCLOCK) || defined(EXTERNALEEPROM)
  // Init TWI Hardware.
  i2c_init();
#endif

#if defined(_AVR_ATXMEGA256A3U_H_INCLUDED)
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
  TCC2.CTRLA = TC2_CLKSEL_DIV1_gc; // ClkPER/64 32MHz/64.
#endif
#endif


  PMIC.CTRL |= PMIC_LOLVLEN_bm;  // Enable Low Priority Interrupts. e.g Rotary encoders, 10ms Counter.
  PMIC.CTRL |= PMIC_MEDLVLEN_bm; // Enable Medium Priority Interrupts. e.g.
  PMIC.CTRL |= PMIC_HILVLEN_bm;  // Enable High Priority Interrupts. e.g. RF Pulses.


  PWR_HOLD_PORT.DIRSET = OB_PWR_HOLD_bm; // Output.
  PWR_HOLD_PORT.OUTSET = OB_PWR_HOLD_bm; // Hold power on.
//  PWR_STATUS_PORT.PIN7CTRL = PORT_OPC_PULLUP_gc; // I_B_PWR_STATUS.

  PORTC.DIRSET = OC_PWR_LED_bm;
  PORTC.OUTSET = OC_PWR_LED_bm;

  setup_trainer_tc();

  DISPLAY_LOADING_MESSAGE();
  lcdRefresh();

#if defined(VOICE)
  InitVoiceUartTx();
#endif

//  PORTB.DIRSET = PIN6_bm; // Test
}



void xmega_wdt_enable_512ms(void)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _PROTECTED_WRITE(WDT.CTRL, WDT_PER_512CLK_gc | WDT_ENABLE_bm | WDT_CEN_bm);
     while (WDT.STATUS & WDT_SYNCBUSY_bm); // wait
     // We don't want a windowed watchdog.
    _PROTECTED_WRITE(WDT.WINCTRL, WDT_WCEN_bm);
    while (WDT.STATUS & WDT_SYNCBUSY_bm); // wait
  }
}


void xmega_wdt_disable(void)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _PROTECTED_WRITE(WDT.CTRL, WDT_PER_8KCLK_gc | WDT_CEN_bm);
//    while (WDT.STATUS & WDT_SYNCBUSY_bm); // wait
  }
}


void setup_rf_tc()
{
// Setup TCx0 for RF Module use.
  PROTO_TC.CTRLA &= ~TC0_CLKSEL_gm; // Stop timer = OFF.
  PROTO_TC.CTRLFSET = TC_CMD_RESET_gc;
  PROTO_TC.CTRLB = 0b000 << TC0_WGMODE_gp; // Mode = NORMAL.
  PROTO_TC.CTRLA = 8 + 1; // Event channel 1 (prescaler of 16).
}


void setup_trainer_tc()
{
// Setup TCx0 for Trainer input pulses.
  PULSES_IN_TC.CTRLA &= ~TC0_CLKSEL_gm; // Stop timer = OFF.
  PULSES_IN_TC.CTRLFSET = TC_CMD_RESET_gc;

// Setup pin for input capture.
  TRN_PULSES_IN_PIN_CTRL_REG = PORT_ISC_FALLING_gc | PORT_OPC_WIREDANDPULL_gc;
  TRAINER_PORT.DIRCLR = 1 << TRN_PULSES_IN_PIN; // Input.

  EVSYS.CH2MUX = 0x70 | 0x08 | TRN_PULSES_IN_PIN; // Port F pin event select. Maps onto CCA.
  EVSYS.CH2CTRL = EVSYS_DIGFILT_8SAMPLES_gc; // 8 samples;

  PULSES_IN_TC.CTRLD = TC_EVACT_CAPT_gc | TC_EVSEL_CH2_gc;

  WAIT_PUPIL();
  PULSES_IN_TC.CTRLB = TC0_CCAEN_bm | 0b000 << TC0_WGMODE_gp; // Mode = NORMAL.
  PULSES_IN_TC.CTRLA = 8 + 1; // Event channel 1 (prescaler of 16).
}


#if defined(SPIMODULES)

void rf_usart_mspi_init()
{
  // Setup pin states and USART in MSPI mode.

  RF_CS_CC2500_INACTIVE();
  RF_CS_CYRF6936_INACTIVE();
  RF_CS_NRF24L01_INACTIVE();
  RF_CS_A7105_INACTIVE();

  RF_PORT.DIRSET = USART_TXD_PIN_bm | RF_CS_PIN_bm | USART_XCK_PIN_bm;
  RF_PORT.DIRCLR = USART_RXD_PIN_bm;
// Configure Virtual Port 0 as Port E for fast bit-bang spi.
  PORTCFG_VPCTRLA = PORTCFG_VP02MAP_PORTE_gc;
#if 0
  // Initialisation of USART in MSPI mode.

  // fBAUD = ClkPER / (2 * (BSEL + 1) )
  RF_USART.BAUDCTRLA = 3; // 4MHz @ ClkPER = 32MHz
  RF_USART.BAUDCTRLB = 0; // BSCALE=0

  // USART in Master SPI mode, MSB first, Clock rising = Sample. Clock falling = Setup.
  RF_USART.CTRLA = 0; // Disable interrupts.
  RF_USART.CTRLC = (USART_CMODE_MSPI_gc) | (0b00 << 1); // MSB first, UCPHA = 0. Check INVEN = 0.
  RF_USART.CTRLB = USART_TXEN_bm | USART_RXEN_bm; // Transmit and Receive.
#endif
}


char rf_usart_mspi_xfer(char c)
{
  WAIT_USART_BUFFER_EMPTY(RF_USART);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      RF_USART.DATA = c;
      RF_USART.STATUS = USART_TXCIF_bm | USART_RXCIF_bm; // Clear USART transmit and receive complete flag.
    }
  WAIT_USART_RX_FIN(RF_USART);
  return RF_USART.DATA;
}


uint8_t spi_xfer_4wires_bb(uint8_t data) // Bitbang write / read.
{
  uint8_t result = 0;
  uint8_t mask = 0x80;
// Unroll loop for optimisation.
 //1
  if(data & mask) RF_MOSI_ON();
  else RF_MOSI_OFF();
  RF_XCK_ON();
  if(IS_RF_MISO_ON) result |= mask;
  mask>>=1;
  RF_XCK_OFF();
//2
  if(data & mask) RF_MOSI_ON();
  else RF_MOSI_OFF();
  RF_XCK_ON();
  if(IS_RF_MISO_ON) result |= mask;
  mask>>=1;
  RF_XCK_OFF();
//3
  if(data & mask) RF_MOSI_ON();
  else RF_MOSI_OFF();
  RF_XCK_ON();
  if(IS_RF_MISO_ON) result |= mask;
  mask>>=1;
  RF_XCK_OFF();
//4
  if(data & mask) RF_MOSI_ON();
  else RF_MOSI_OFF();
  RF_XCK_ON();
  if(IS_RF_MISO_ON) result |= mask;
  mask>>=1;
  RF_XCK_OFF();
//5
  if(data & mask) RF_MOSI_ON();
  else RF_MOSI_OFF();
  RF_XCK_ON();
  if(IS_RF_MISO_ON) result |= mask;
  mask>>=1;
  RF_XCK_OFF();
//6
  if(data & mask) RF_MOSI_ON();
  else RF_MOSI_OFF();
  RF_XCK_ON();
  if(IS_RF_MISO_ON) result |= mask;
  mask>>=1;
  RF_XCK_OFF();
//7
  if(data & mask) RF_MOSI_ON();
  else RF_MOSI_OFF();
  RF_XCK_ON();
  if(IS_RF_MISO_ON) result |= mask;
  mask>>=1;
  RF_XCK_OFF();
//8
  if(data & mask) RF_MOSI_ON();
  else RF_MOSI_OFF();
  RF_XCK_ON();
  if(IS_RF_MISO_ON) result |= mask;
  mask>>=1;
  RF_XCK_OFF();

  return result;
}


void spi_write_3wires_bb(uint8_t data) // Bitbang write.
{
  uint8_t mask = 0x80;
  do
  {
   if(data & mask) RF_MOSI_ON();
   else RF_MOSI_OFF();

   RF_XCK_ON();
   mask>>=1;
   RF_XCK_OFF();
  }
  while(mask);
}


uint8_t spi_read_3wires_bb() // Bitbang read.
{
  uint8_t result = 0;
  uint8_t mask = 0x80;
  do
  {
    RF_XCK_ON();

    if(IS_RF_MOSI_ON)
      result |= mask;

    mask>>=1;
    RF_XCK_OFF();
  }
  while(mask);

  return result;
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
  PWR_HOLD_PORT.OUTCLR = OB_PWR_HOLD_bm;
}

#if 0
void Check_PWR_Switch(void)
{
  static tmr10ms_t last10ms;

  if( ! (PWR_STATUS_PORT.IN & IB_PWR_STATUS_bm ) ) {
  // PWR Switch is on.
  last10ms = getTmr10ms();
  pwrCheck = true;
  return;
  }
  // PWR Switch is off.
  if( getTmr10ms() - last10ms > 200 ) {
    pwrCheck = false; // > 2 seconds.
  }
  else pwrCheck = true;
}
#endif

uint8_t switches[NUM_SWS - SW_BASE] = {0}; // Switches via R-2R ladder DAC.


uint8_t switchState(enum EnumKeys enuk)
{
// Returns the state of the enum parameter (keys, trims, rotary encoder buttons and switches).
  if(enuk < NUM_KEYS)
    return keys[enuk].state() ? 1 : 0; // Keys, trims and rotary encoder buttons.

  else if(enuk < NUM_SWS)
    return switches[enuk - SW_BASE]; // Switches.

  else
    return 0;
}


uint8_t keyDown()
{
// Returns 0 for no key pressed. Non-zero value is not checked elsewhere.
  return keys[KEY_ENTER].state(); // Patched
}


// Trim switches.
// Expected sequence is defined in keys.h
#if defined(TOGGLETRIM) //Toggle trim usage -> Left trim for right stick and right trim for left stick
static const pm_uchar crossTrim[] PROGMEM = { // ToDo
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LH_DWN,
  TRM_LV_UP,
  TRM_RH_UP,
  TRM_RV_DWN,
  TRM_RH_DWN,
  TRM_RV_UP
};
#else
static const pm_uchar crossTrim[] PROGMEM = { // Done
  TRM_RH_UP,
  TRM_RV_DWN,
  TRM_RH_DWN,
  TRM_RV_UP,
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LH_DWN,
  TRM_LV_UP
};
#endif


uint8_t trimDown(uint8_t idx)
{
// Return value is only interpreted as bool.
//  return keys[TRM_BASE + idx].state();
return 0;
}


void read_trim_matrix()
{
  uint8_t in = ~PORTE.IN;

  if(PORTF.OUT & OF_TRIM_ROW_A_bm)
  {
    keys[pgm_read_byte_far(crossTrim+0)].input( in & IE_TRIM_COL_A_bm);
    keys[pgm_read_byte_far(crossTrim+1)].input( in & IE_TRIM_COL_B_bm);
    keys[pgm_read_byte_far(crossTrim+2)].input( in & IE_TRIM_COL_C_bm);
    keys[pgm_read_byte_far(crossTrim+3)].input( in & IE_TRIM_COL_D_bm);
  }
  else
  {
    keys[pgm_read_byte_far(crossTrim+4)].input( in & IE_TRIM_COL_A_bm);
    keys[pgm_read_byte_far(crossTrim+5)].input( in & IE_TRIM_COL_B_bm);
    keys[pgm_read_byte_far(crossTrim+6)].input( in & IE_TRIM_COL_C_bm);
    keys[pgm_read_byte_far(crossTrim+7)].input( in & IE_TRIM_COL_D_bm);
  }

  PORTF.OUTTGL = OF_TRIM_ROW_A_bm;
}


void readKeysAndTrims()
{
  // Keyboard via ADC.
  read_keyboard();
  // Multiplexed trim buttons.
  read_trim_matrix();
}

enum MpxButtons {
  BTN_NONE,
  BTN_DIGI,
  BTN_REVCLR,
  BTN_ENTER,
  BTN_UP,
  BTN_DOWN,
  BTN_MEMORY,
  BTN_TIMER,
  BTN_SERVO,
  BTN_MIXER,
  BTN_CONTROL,
  BTN_SETUP,
  NUM_BTNS,
};


#define BTN_TOL 0x27

static const uint16_t AnalKeyboardVal[NUM_BTNS] PROGMEM = {
  // AD reading, Multiplex Key Name.
  // Quiescent reading = 0x7ff.
  // Min delta is 157 so BTN_TOL = 0x27

  0x7ff + BTN_TOL, // No Key pressed

  0x38e + BTN_TOL, // Digi
  0x4cc + BTN_TOL, // Rev/Clr --> exit
  0x5d1 + BTN_TOL, // Enter --> menu
  0x6aa + BTN_TOL, // Up --> up
  0x762 + BTN_TOL, // Down --> down
  0x89d + BTN_TOL, // Memory --> right
  0x954 + BTN_TOL, // Timer --> left
  0xa2d + BTN_TOL, // Servo
  0xb32 + BTN_TOL, // Mixer
  0xc71 + BTN_TOL, // Control
  0xdff + BTN_TOL, // Setup
};


void read_keyboard()
{
  uint16_t AD_value = s_anaFilt[KEYBOARD];

  for(uint8_t x=BTN_NONE; x<NUM_BTNS; x++) {

    uint16_t  upper = pgm_read_word_far(&AnalKeyboardVal[x]);
    uint16_t  lower = upper - (BTN_TOL *2);
    if(lower > upper) lower = 0;

    if(AD_value > lower && AD_value < upper){
      keys[KEY_EXIT].input(  (x==BTN_REVCLR)  ? 1 :0);
      keys[KEY_MENU].input(  (x==BTN_ENTER)  ? 1 :0);
      keys[KEY_UP].input(    (x==BTN_UP)  ? 1 :0);
      keys[KEY_DOWN].input(  (x==BTN_DOWN)  ? 1 :0);
      keys[KEY_RIGHT].input( (x==BTN_MEMORY)  ? 1 :0);
      keys[KEY_LEFT].input(  (x==BTN_TIMER)  ? 1 :0);
      break;
    }
  }
}


#define ANA_TOL 0x28

/*
The Evo has two switch modules which convert the various switch states into analogue voltages.
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



static const uint16_t AnalSwitchVal[16] PROGMEM = {
  // AD reading, switch state bitmap.
  //        ENC 2  Position  3   pos
  //        R/L K/P H/M I/N  O/J L/G
  0x000 + ANA_TOL, // 1   1   1   0
  0x0b2 + ANA_TOL, // 1   1   1   1
  0x1ca + ANA_TOL, // 1   1   0   0
  0x286 + ANA_TOL, // 1   1   0   1
  0x3ce + ANA_TOL, // 1   0   1   0
  0x48f + ANA_TOL, // 1   0   1   1   DN  DN
  0x5b0 + ANA_TOL, // 1   0   0   0   DN  UP
  0x670 + ANA_TOL, // 1   0   0   1   DN  MI
  0x7d4 + ANA_TOL, // 0   1   1   0
  0x898 + ANA_TOL, // 0   1   1   1   UP  DN
  0x9ba + ANA_TOL, // 0   1   0   0   UP  UP
  0xa7b + ANA_TOL, // 0   1   0   1   UP  MI
  0xbca + ANA_TOL, // 0   0   1   0
  0xc95 + ANA_TOL, // 0   0   1   1   MI  DN
  0xdbe + ANA_TOL, // 0   0   0   0   MI  UP
  0xe82 + ANA_TOL, // 0   0   0   1   MI  MI
                   // 8   4   2   1
};


void read_sws_1(void)
{
  uint16_t AD_value;
  uint16_t upper;
  uint16_t lower;

  AD_value = s_anaFilt[ANAL_SWS_1];

  for(uint8_t x=5; x<16; x++) {

    upper = pgm_read_word_far(&AnalSwitchVal[x]);
    lower = upper - (ANA_TOL *2);
    if (lower > upper) lower = 0;

    if(AD_value > lower && AD_value < upper) {
      switches[SW_Jdn - SW_BASE] = ( (x==5 || x==6 || x==7) ? 1 : 0); // J Down
      switches[SW_Jup - SW_BASE] = ( (x==9 || x==10 || x==11) ? 1 : 0); // J Up
      switches[SW_Jmi - SW_BASE] = ( (x==13 || x==14 || x==15) ? 1 : 0); // J Middle

      switches[SW_Gdn - SW_BASE] = ( (x==5 || x==9 || x==13) ? 1 : 0); // G Down
      switches[SW_Gup - SW_BASE] = ( (x==6 || x==10 || x==14) ? 1 : 0); // G Up
      switches[SW_Gmi - SW_BASE] = ( (x==7 || x==11 || x==15) ? 1 : 0); // G Middle
    break;
    }
  }
}


void read_sws_2(void)
{
  uint16_t AD_value;
  uint16_t upper;
  uint16_t lower;

  AD_value = s_anaFilt[ANAL_SWS_2];

  for(uint8_t x=0; x<16; x++) {

    upper = pgm_read_word_far(&AnalSwitchVal[x]);
    lower = upper - (ANA_TOL *2);
    if (lower > upper) lower = 0;

    if(AD_value > lower && AD_value < upper) {
      switches[SW_H - SW_BASE] = ( (x & 2) ? 0 : 1); // H
      switches[SW_I - SW_BASE] = ( (x & 1) ? 1 : 0); // I
      switches[SW_K - SW_BASE] = ( (x & 4) ? 1 : 0); // K
      keys[BTN_REb].input( (x & 8) ? 0 : 1); // EncRight
    break;
    }
  }
}


void read_sws_3(void)
{
  uint16_t AD_value;
  uint16_t upper;
  uint16_t lower;

  AD_value = s_anaFilt[ANAL_SWS_3];

  for(uint8_t x=5; x<16; x++) {

    upper = pgm_read_word_far(&AnalSwitchVal[x]);
    lower = upper - (ANA_TOL *2);
    if (lower > upper) lower = 0;

    if(AD_value > lower && AD_value < upper) {
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


void read_sws_4(void)
{
  uint16_t AD_value;
  uint16_t upper;
  uint16_t lower;

  AD_value = s_anaFilt[ANAL_SWS_4];

  for(uint8_t x=0; x<16; x++) {

    upper = pgm_read_word_far(&AnalSwitchVal[x]);
    lower = upper - (ANA_TOL *2);
    if (lower > upper) lower = 0;

    if(AD_value > lower && AD_value < upper) {
      switches[SW_M - SW_BASE] = ( (x & 2) ? 0 : 1); // M
      switches[SW_N - SW_BASE] = ( (x & 1) ? 1 : 0); // N
      switches[SW_P - SW_BASE] = ( (x & 4) ? 1 : 0); // P
      keys[BTN_REa].input( (x & 8) ? 0 : 1); // EncLeft
    break;
    }
  }
}


bool check_slave_mode(void)
{
  return 1;
}


/*
The Rotary Encoder (Digi-adjuster) ISR's.
Rotary Encoder type is Alps 321V.
15 Detents.
There is hardware filtering of the signals.
*/

ISR(PORTF_INT0_vect) // REa (LH).
{
  static uint8_t rea_neg_state =0;
  static uint8_t rea_pos_state =0;

  if(PORT_ISC_FALLING_gc == (ROT_ENC_LH_CLK_CTRL_REG & PORT_ISC_FALLING_gc)) {
    rea_neg_state = ROT_ENC_PORT.IN & IF_ROT_ENC_LH_DIR_bm;

    ROT_ENC_LH_CLK_CTRL_REG = PORT_ISC_RISING_gc;
  }
  else if(PORT_ISC_RISING_gc == (ROT_ENC_LH_CLK_CTRL_REG & PORT_ISC_RISING_gc)) {
    rea_pos_state = ROT_ENC_PORT.IN & IF_ROT_ENC_LH_DIR_bm;

    if(rea_neg_state != rea_pos_state) {
      if(rea_pos_state) incRotaryEncoder(0, -1);
      else incRotaryEncoder(0, +1);
    }

    ROT_ENC_LH_CLK_CTRL_REG = PORT_ISC_FALLING_gc;
  }
}


ISR(PORTF_INT1_vect) // REb (RH).
{
  static uint8_t reb_neg_state =0;
  static uint8_t reb_pos_state =0;

  if(PORT_ISC_FALLING_gc == (ROT_ENC_RH_CLK_CTRL_REG & PORT_ISC_FALLING_gc)) {
    reb_neg_state = ROT_ENC_PORT.IN & IF_ROT_ENC_RH_DIR_bm;

    ROT_ENC_RH_CLK_CTRL_REG = PORT_ISC_RISING_gc;
  }
  else if(PORT_ISC_RISING_gc == (ROT_ENC_RH_CLK_CTRL_REG & PORT_ISC_RISING_gc)) {
    reb_pos_state = ROT_ENC_PORT.IN & IF_ROT_ENC_RH_DIR_bm;

    if(reb_neg_state != reb_pos_state) {
      if(reb_pos_state) incRotaryEncoder(1, -1);
      else incRotaryEncoder(1, +1);
    }

    ROT_ENC_RH_CLK_CTRL_REG = PORT_ISC_FALLING_gc;
  }
}



