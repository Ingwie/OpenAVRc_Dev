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

#define ST7565R_CMD_RESET                       0xE2
#define ST7565R_CMD_DISPLAY_OFF                 0xAE
#define ST7565R_CMD_DISPLAY_ALL_POINTS_OFF      0xA4
#define ST7565R_CMD_REVERSE_SCAN_DIRECTION      0xC8
#define ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET  0x81
#define ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_5    0x25
#define ST7565R_CMD_POWER_CTRL_ALL_ON           0x2F
#define ST7565R_CMD_DISPLAY_ON                  0xAF
#define ST7565R_CMD_START_LINE_SET( line )             (0x40 | (line))
#define ST7565R_CMD_PAGE_ADDRESS_SET( page )           (0xB0 | (page))
#define ST7565R_CMD_COLUMN_ADDRESS_SET_MSB( column )   (0x10 | (column))
#define ST7565R_CMD_COLUMN_ADDRESS_SET_LSB( column )   (0x00 | (column))


#define LCD_PORT       PORTD
#define O_LCD_MOSI     PIN7_bm
#define O_LCD_SCK_P    PIN5_bm

#define LCD_CLK_HI     LCD_PORT.OUTSET = O_LCD_SCK_P
#define LCD_CLK_LO     LCD_PORT.OUTCLR = O_LCD_SCK_P

#define LCD_USART      USARTD1

#define O_LCD_CS       PIN4_bm

//#define POSITIVE_CHIP_SELECT
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

  LCD_PORT.DIRSET = O_LCD_CS | O_LCD_A0 | O_LCD_SCK_P | O_LCD_MOSI;
  // Setup pin states and USART in MSPI mode.

  LCD_CS_INACTIVE;
  LCD_A0_LO;
  LCD_CLK_LO;
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

  lcdSendCtl(ST7565R_CMD_RESET);
  _delay_us(300);
  lcdSendCtl(0x2C);// Power controller set, booster cct on, V reg off, V follower off
  _delay_us(300);
  lcdSendCtl(0x2E);// Power controller set, booster cct on, V reg on, V follower off
  _delay_us(300);
  lcdSendCtl(ST7565R_CMD_POWER_CTRL_ALL_ON);// Power controller set, booster cct on, V reg on, V follower on
  _delay_us(300);
  lcdSendCtl(ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_5);
   lcdSendCtl(ST7565R_CMD_DISPLAY_OFF);

   // Turn display on in lcdRefresh() ... prevents displaying garbage at startup, which seems to happen more when sdcards are attached.
   //lcdSendCtl(ST7565R_CMD_DISPLAY_ON);

   lcdSendCtl(ST7565R_CMD_REVERSE_SCAN_DIRECTION);// Common output mode select
   lcdSendCtl(ST7565R_CMD_DISPLAY_ALL_POINTS_OFF);// All points off (normal)

   lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET);// Electronic volume mode set ? Range = 0x09 to 0x19 (-8 to +8 on Evo)
   lcdSendCtl(0x09);// See above ... 0 = 0x11
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

  lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET); // Range = 0x09 to 0x19 (-8 to +8 on Evo setup screen)
  lcdSendCtl(g_eeGeneral.contrast - CONTRAST_MIN);

  lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
  lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
  lcdSendCtl(ST7565R_CMD_PAGE_ADDRESS_SET(page ++));

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


void lcdRefresh()
{
  /* Although this uses DMA, it is a blocking process so that it does not upset the timing.
   * e.g. During startup a whole screen needs to be displayed before continuing.
   */
  while (DMA.CH2.CTRLA & DMA_ENABLE_bm);

  lcdSendCtl(ST7565R_CMD_DISPLAY_ON);

  for(uint8_t i=NUMITERATIONFULLREFRESH; i>0; i--) {
    lcdRefreshFast();
    while (DMA.CH2.CTRLA & DMA_ENABLE_bm);
  }
}


ISR(DMA_CH2_vect)
{
// Shared Vector for Error Interrupt Flag (ERRIF) and Channel N Transfer Complete (TRNIF).

  while(DMA.CH2.CTRLA & DMA_ENABLE_bm); // Channel should auto disable.

  DMA.CH2.CTRLB |=  DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm; // Clear shared flags.

  _delay_us(20); // The USART is double buffered ... DMA transaction has ended, but USART is still running.
  LCD_CS_INACTIVE;
}


