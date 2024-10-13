/*
 * Authors (alphabetical order)
 *
 * - Rich Kilvington (2023).
 *
 */

/*
 *****************************************************************
 * Driver for a SSD1309 OLED Display.
 * Written for Xmega with DMA.
 * No Reset signal as that is derived from RTC chip although unused
 * RXD1 pin could be utilised.
 *****************************************************************
 */

/*
 *****************************************************************
 * LCD driver for a 132 * 64 LCD as fitted in the EVO transmitter.
 * All information was derived by tracing the SPI messages.
 * LCD looks to be ST7565R compatible with a CS+ chip select.
 * Written for Xmega SPI and Bit-Bang.
 * Adapted from USART MSPI code.
 * As this is a uni-directional driver, choose Bit-Bang to reclaim
 * the unused MISO pin.
 * USART MSPI would be better and can do DMA
 * too unlike the SPI port.
 * No Reset signal as that is derived from RTC chip.
 *****************************************************************
*/

void lcdClearRAM(void);

#define LCD_SSD1309
//#define LCD_EVO
//#define LCD_ST7567

#if defined(LCD_EVO)
// #define CONTRAST_MIN 10
 #define LCD_SIZE_132X64
 #define POSITIVE_CHIP_SELECT
#endif
#if defined(LCD_ST7567)
// #define CONTRAST_MIN 35
 #define LCD_SIZE_132X64
 #define POSITIVE_CHIP_SELECT
#endif
#if defined(LCD_SSD1309)
// #define CONTRAST_MIN 30
#endif


#define SSD1309_CMD_SETCMDLOCK            0xFD
#define SSD1309_CMD_DISPLAYSLEEP          0xAE
#define SSD1309_CMD_DISPLAYWAKEUP         0xAF
#define SSD1309_CMD_SETDISPLAYCLOCKDIV    0xD5 // ?
#define SSD1309_CMD_SETMULTIPLEX          0xA8
#define SSD1309_CMD_SETDISPLAYOFFSET      0xD3
#define SSD1309_CMD_SETMEMORYADDRESSMODE  0x20
#define SSD1309_CMD_SET_COLUMN_START_AND_END_ADDRESS  0x21
#define SSD1309_CMD_SETSEGMENTREMAPNORM   0xA0
#define SSD1309_CMD_SETSEGMENTREMAPREV    0xA1
#define SSD1309_CMD_SETCOMOUTPUTDIRNORM   0xC0
#define SSD1309_CMD_SETCOMOUTPUTDIRREV    0xC8
#define SSD1309_CMD_SETCOMPINSHARDCONF    0xDA
#define SSD1309_CMD_DISPLAYALLON          0xA5 // All dots on.
#define SSD1309_CMD_DISPLAYALLON_RESUME   0xA4 // Display RAM contents.
#define SSD1309_CMD_SETCONTRAST           0x81
#define SSD1309_CMD_NORMALDISPLAY         0xA6
#define SSD1309_CMD_INVERTDISPLAY         0xA7
#define SSD1309_CMD_SETVCOMDETECT         0xDB
#define SSD1309_CMD_SETPRECHARGE          0xD9
#define SSD1309_CMD_SETSTARTLINE( line )               (0x40 | (line))
#define SSD1309_CMD_PAGE_ADDRESS_SET( page )           (0xB0 | (page))
#define SSD1309_CMD_COLUMN_ADDRESS_SET_LSB( column )   (0x00 | (column))
#define SSD1309_CMD_COLUMN_ADDRESS_SET_MSB( column )   (0x10 | (column))

#define ST756n_CMD_RESET                       0xE2
#define ST756n_CMD_DISPLAY_ALL_POINTS_OFF      0xA4
#define ST756n_CMD_DISPLAY_ALL_POINTS_ON       0xA5
#define ST756n_CMD_NORMAL_COM_DIRECTION        0xC0
#define ST756n_CMD_REVERSE_COM_DIRECTION       0xC8
#define ST756n_CMD_NORMAL_SEG_DIRECTION        0xA0
#define ST756n_CMD_REVERSE_SEG_DIRECTION       0xA1
#define ST756n_CMD_BIAS_SELECT_7TH             0xA3
#define ST756n_CMD_BIAS_SELECT_9TH             0xA2
#define ST756n_CMD_CONTRAST_SET                0x81
#define ST756n_CMD_VOLTAGE_RESISTOR_RATIO_5dec5    0x25
#define ST756n_CMD_VOLTAGE_RESISTOR_RATIO_6dec0    0x26
#define ST756n_CMD_POWER_CTRL                  0x28
#define ST756n_CMD_DISPLAY_OFF                 0xAE
#define ST756n_CMD_DISPLAY_ON                  0xAF
#define ST756n_CMD_START_LINE_SET( line )             (0x40 | (line))
#define ST756n_CMD_PAGE_ADDRESS_SET( page )           (0xB0 | (page))
#define ST756n_CMD_COLUMN_ADDRESS_SET_LSB( column )   (0x00 | (column))
#define ST756n_CMD_COLUMN_ADDRESS_SET_MSB( column )   (0x10 | (column))


#define LCD_PORT       PORTD
#define O_LCD_MOSI     PIN7_bm
#define O_LCD_SCK_P    PIN5_bm

#define LCD_CLK_HI     LCD_PORT.OUTSET = O_LCD_SCK_P
#define LCD_CLK_LO     LCD_PORT.OUTCLR = O_LCD_SCK_P

#define LCD_USART      USARTD1

#define O_LCD_CS       PIN4_bm

#if defined (POSITIVE_CHIP_SELECT)
#define LCD_CS_ACTIVE      LCD_PORT.OUTSET = O_LCD_CS
#define LCD_CS_INACTIVE    LCD_PORT.OUTCLR = O_LCD_CS
#else
// Negative Chip select.
#define LCD_CS_INACTIVE    LCD_PORT.OUTSET = O_LCD_CS
#define LCD_CS_ACTIVE      LCD_PORT.OUTCLR = O_LCD_CS
#endif

#define O_LCD_A0           PIN1_bm
#define LCD_A0_HI          LCD_PORT.OUTSET = O_LCD_A0
#define LCD_A0_LO          LCD_PORT.OUTCLR = O_LCD_A0

#define NUMITERATIONFULLREFRESH  8


void lcd_spi_tx(char c)
{
  WAIT_USART_BUFFER_EMPTY(LCD_USART);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    LCD_USART.DATA = c;
    LCD_USART.STATUS |= USART_TXCIF_bm; // Clear USART transmit flag.
  }
  WAIT_USART_TX_FIN(LCD_USART);
}


void lcdSendCtl(uint8_t c)
{
  LCD_A0_LO;
  LCD_CS_ACTIVE;
  lcd_spi_tx(c);
  WAIT_USART_TX_FIN (LCD_USART);
  LCD_CS_INACTIVE;
}


void lcdSetRefVolt(uint8_t val)
{
  return; // Patched out due to DMA, see lcdRefresh().
}


void lcdInit()
{
  // Setup pin states and MSPI mode.
  // LCD Reset pin is now controlled by RTC reset signal.
  // Remap SPIC MOSI and SCK pins.

  LCD_CS_INACTIVE;
  LCD_A0_LO;
  LCD_CLK_LO;

  LCD_PORT.DIRSET = O_LCD_CS | O_LCD_A0 | O_LCD_SCK_P | O_LCD_MOSI;
  // Setup pin states and USART in MSPI mode.

  _delay_ms(250);

  // Initialisation of USART in MSPI mode.

  // fBAUD = ClkPER / (2 * (BSEL + 1) )
  LCD_USART.BAUDCTRLA = 7; // 2MHz @ ClkPER = 32MHz
  LCD_USART.BAUDCTRLB = 0; // BSCALE=0

  // USART in Master SPI mode, MSB first, Clock rising = Sample.
  LCD_USART.CTRLA = 0; // Disable interrupts.
  LCD_USART.CTRLC = (USART_CMODE_MSPI_gc) | (0b11 < 1);
  LCD_USART.CTRLB = USART_TXEN_bm; // Transmit only.

  _delay_us(300);

#if defined (LCD_SSD1309)
lcdSendCtl(SSD1309_CMD_SETCMDLOCK);  // Set Command Lock
lcdSendCtl(0x12);  // Unlock
//     0x12 => Driver IC interface is unlocked from entering command.
//     0x16 => All Commands are locked except 0xFD.

lcdSendCtl(SSD1309_CMD_DISPLAYSLEEP);            //--turn off oled panel

lcdSendCtl(SSD1309_CMD_SETDISPLAYCLOCKDIV); // Set Display Clock Divide Ratio / Oscillator Frequency
lcdSendCtl(0x70);      //   Default => 0x70
//     D[3:0] => Display Clock Divider
//     D[7:4] => Oscillator Frequency

lcdSendCtl(SSD1309_CMD_SETMULTIPLEX);         //--set multiplex ratio(1 to 64)
lcdSendCtl(0x3f);            //--1/64 duty

lcdSendCtl(SSD1309_CMD_SETDISPLAYOFFSET); //Set Display Offset
lcdSendCtl(0x00);

lcdSendCtl(SSD1309_CMD_SETSTARTLINE(0));      // Set Display Start Line

lcdSendCtl(SSD1309_CMD_SETMEMORYADDRESSMODE);    // Set Memory Addressing Mode
lcdSendCtl(0x02);      //   Default => 0x02
//     0x00 => Horizontal Addressing Mode
//     0x01 => Vertical Addressing Mode
//     0x02 => Page Addressing Mode  .. no increment between pages.

lcdSendCtl(SSD1309_CMD_SETSEGMENTREMAPREV); //
lcdSendCtl(SSD1309_CMD_SETCOMOUTPUTDIRREV); //

lcdSendCtl(SSD1309_CMD_SETCOMPINSHARDCONF); //--Set COM Pins Hardware Configuration
lcdSendCtl(0x12); //     Disable COM Left/Right Re-Map   Alternative COM Pin Configuration

lcdSendCtl(SSD1309_CMD_SETPRECHARGE);      // Set Pre-Charge Period
lcdSendCtl(0xAA); //   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
//     D[3:0] => Phase 1 Period (discharge) in 1~15 Display Clocks
//     D[7:4] => Phase 2 Period (charge) in 1~15 Display Clocks

lcdSendCtl(SSD1309_CMD_SETVCOMDETECT);      // Set VCOMH Deselect Level
lcdSendCtl(0x00);      // 34  Default => 0x34 (0.78*VCC)

lcdSendCtl(SSD1309_CMD_NORMALDISPLAY); //--set normal display
lcdSendCtl(SSD1309_CMD_DISPLAYALLON);  //Disable Entire Display On
lcdSendCtl(SSD1309_CMD_DISPLAYWAKEUP); //--turn on oled panel
lcdSendCtl(SSD1309_CMD_DISPLAYALLON_RESUME); // Turn on OLED panel
#else

lcdSendCtl(ST756n_CMD_RESET);
_delay_us(300);

#if defined (LCD_ST7567)
lcdSendCtl(ST756n_CMD_BIAS_SELECT_7TH); // Select bias 1/7 (at 1/65 duty) not 1/9.
#endif

#if !defined (REVERSE_DISPLAY)
lcdSendCtl(ST756n_CMD_NORMAL_SEG_DIRECTION);
lcdSendCtl(ST756n_CMD_REVERSE_COM_DIRECTION);
#else
lcdSendCtl(ST756n_CMD_REVERSE_SEG_DIRECTION);
lcdSendCtl(ST756n_CMD_NORMAL_COM_DIRECTION);
#endif

#if defined (LCD_ST7567)
lcdSendCtl(ST756n_CMD_VOLTAGE_RESISTOR_RATIO_6dec0);
#endif

#if defined (LCD_EVO)
lcdSendCtl(ST756n_CMD_VOLTAGE_RESISTOR_RATIO_5dec5);
#endif

lcdSendCtl(ST756n_CMD_POWER_CTRL | 4);
_delay_us(300);
lcdSendCtl(ST756n_CMD_POWER_CTRL | 6);
_delay_us(300);
lcdSendCtl(ST756n_CMD_POWER_CTRL | 7);
_delay_us(300);

lcdClearRAM();
lcdSendCtl(ST756n_CMD_DISPLAY_ON); // Display on.
#endif
}


void lcdRefreshFast()
{
//#define SHOWDURATION
#if defined(SHOWDURATION)
  lcdDrawNumberNAtt(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
// Non blocking function. Just DMA and go. DMA ISR finishes DMA communication to LCD.
  if (DMA.CH2.CTRLA & DMA_ENABLE_bm) return;

  SHOWDURATIONLCD1

  static uint8_t page = 0;
  static volatile uint8_t *p = displayBuf;

#if defined (LCD_EVO)
  lcdSendCtl(ST756n_CMD_CONTRAST_SET); lcdSendCtl(g_eeGeneral.contrast);
#endif
#if defined(LCD_ST7567)
  lcdSendCtl(ST756n_CMD_CONTRAST_SET); lcdSendCtl(g_eeGeneral.contrast + 25);
#endif
#if defined(LCD_SSD1309)
  lcdSendCtl(ST756n_CMD_CONTRAST_SET); lcdSendCtl(g_eeGeneral.contrast + 20);
#endif

#if defined (LCD_SIZE_132X64)
  lcdSendCtl(ST756n_CMD_COLUMN_ADDRESS_SET_LSB(2)); // Set LS nibble column RAM address 2
#else
  lcdSendCtl(ST756n_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
#endif
  lcdSendCtl(ST756n_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
  lcdSendCtl(ST756n_CMD_PAGE_ADDRESS_SET(page ++));

  LCD_A0_HI;
  LCD_CS_ACTIVE;
  // Burst transfer of 1 byte, block size is 128 bytes (One LCD ROW).
  DMA.CTRL |= DMA_ENABLE_bm;
  DMA.CTRL |= DMA_PRIMODE_CH0123_gc;
  DMA.CH2.CTRLA = DMA_CH_RESET_bm;
  DMA.CH2.ADDRCTRL = DMA_CH_SRCRELOAD_NONE_gc | DMA_CH_SRCDIR_INC_gc
      | DMA_CH_DESTDIR_FIXED_gc | DMA_CH_DESTRELOAD_NONE_gc;

  /*
   DMA_CH_SRCRELOAD_NONE_gc = (0x00<<6), // No reload
   DMA_CH_SRCRELOAD_BLOCK_gc = (0x01<<6), // Reload at end of block
   DMA_CH_SRCRELOAD_BURST_gc = (0x02<<6), // Reload at end of burst
   DMA_CH_SRCRELOAD_TRANSACTION_gc = (0x03<<6), // Reload at end of transaction
   */
  DMA.CH2.TRIGSRC = DMA_CH_TRIGSRC_USARTD1_DRE_gc;
  DMA.CH2.TRFCNT = LCD_W;
  DMA.CH2.SRCADDR0 = ((uint16_t) p >> 0 * 8) & 0xFF;
  DMA.CH2.SRCADDR1 = ((uint16_t) p >> 1 * 8) & 0xFF;
  DMA.CH2.SRCADDR2 = 0;
  DMA.CH2.DESTADDR0 = (((uint16_t) & LCD_USART.DATA) >> 0 * 8) & 0xFF;
  DMA.CH2.DESTADDR1 = (((uint16_t) & LCD_USART.DATA) >> 1 * 8) & 0xFF;
  DMA.CH2.DESTADDR2 = 0;

  DMA.CH2.CTRLB = DMA_CH_TRNINTLVL_LO_gc; // Transfer complete interrupt enable.
  DMA.CH2.CTRLA = DMA_ENABLE_bm | DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_1BYTE_gc;

  p += LCD_W;

  if (page > 7)
  {
    page = 0;
    p = displayBuf;
  }
  SHOWDURATIONLCD2
}


ISR(DMA_CH2_vect)
{
// Shared Vector for Error Interrupt Flag (ERRIF) and Channel N Transfer Complete (TRNIF).

  while(DMA.CH2.CTRLA & DMA_ENABLE_bm); // Channel should auto disable.

  DMA.CH2.CTRLB |=  DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm; // Clear shared flags.

  _delay_us(20); // The USART is double buffered ... DMA transaction has ended, but USART is still running.
  LCD_CS_INACTIVE;
}


void lcdRefresh()
{
  /* Although this uses DMA, it is a blocking process so that it does not upset the timing.
   * e.g. During startup a whole screen needs to be displayed before continuing.
   */
  while (DMA.CH2.CTRLA & DMA_ENABLE_bm);

  for(uint8_t i=NUMITERATIONFULLREFRESH; i>0; i--) {
    lcdRefreshFast();
    while (DMA.CH2.CTRLA & DMA_ENABLE_bm);
  }
}


void lcdClearRAM(void)
{
  for(uint8_t page = 0; page < 8; page ++)
    {
      lcdSendCtl(ST756n_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
      lcdSendCtl(ST756n_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
      lcdSendCtl(ST756n_CMD_PAGE_ADDRESS_SET(page));

      LCD_A0_HI;
      LCD_CS_ACTIVE;

#if defined (LCD_SIZE_132X64)
      uint8_t column = 132;
#else
      uint8_t column = LCD_W; // 128
#endif

      while(column--)  lcd_spi_tx(0x00);

      WAIT_USART_TX_FIN(LCD_USART);
      LCD_CS_INACTIVE;
    }
}


