/*
 * Authors (alphabetical order)
 *
 * - Rich Kilvington (2021).
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

#define SSD1309_CMD_SETCMDLOCK            0xFD
#define SSD1309_CMD_DISPLAYSLEEP          0xAE
#define SSD1309_CMD_DISPLAYWAKEUP         0xAF
#define SSD1309_CMD_SETDISPLAYCLOCKDIV    0xD5 // ?
#define SSD1309_CMD_SETMULTIPLEX          0xA8
#define SSD1309_CMD_SETDISPLAYOFFSET      0xD3
#define SSD1309_CMD_SETMEMORYADDRESSMODE  0x20
#define SSD1309_CMD_SETSEGMENTREMAPNORM   0xA0
#define SSD1309_CMD_SETSEGMENTREMAPREV    0xA1
#define SSD1309_CMD_SETCOMOUTPUTDIRNORM   0xC0
#define SSD1309_CMD_SETCOMOUTPUTDIRREV    0xC8
#define SSD1309_CMD_SETCOMPINSHARDCONF    0xDA
#define SSD1309_CMD_DISPLAYALLON          0xA5 // All dots on.
#define SSD1309_CMD_DISPLAYALLON_RESUME   0xA4 // Display RAM contents.

#define SSD1309_CMD_SET_COLUMN_ADDRESS_LSB( column )   (0x00 | (column))
#define SSD1309_CMD_SET_COLUMN_ADDRESS_MSB( column )   (0x10 | (column))
#define SSD1309_CMD_SET_PAGE_ADDRESS( page )           (0xB0 | (page))
#define SSD1309_CMD_SET_COLUMN_START_AND_END_ADDRESS  0x21

#define SSD1309_CMD_SETCONTRAST         0x81
#define SSD1309_CMD_NORMALDISPLAY       0xA6
#define SSD1309_CMD_INVERTDISPLAY       0xA7
#define SSD1309_CMD_SETVCOMDETECT       0xDB
#define SSD1309_CMD_SETPRECHARGE        0xD9
#define SSD1309_CMD_SETSTARTLINE( line )             (0x40 | (line))

#define LCD_PORT       PORTD
#define O_LCD_MOSI     PIN7_bm
#define O_LCD_SCK_P    PIN5_bm

#define LCD_CLK_HI     LCD_PORT.OUTSET = O_LCD_SCK_P
#define LCD_CLK_LO     LCD_PORT.OUTCLR = O_LCD_SCK_P

#define LCD_USART      USARTD1

#define O_LCD_CS_N           PIN4_bm
#define LCD_CS_N_INACTIVE    LCD_PORT.OUTSET = O_LCD_CS_N
#define LCD_CS_N_ACTIVE      LCD_PORT.OUTCLR = O_LCD_CS_N

#define O_LCD_A0             PIN1_bm
#define LCD_A0_HI            LCD_PORT.OUTSET = O_LCD_A0
#define LCD_A0_LO            LCD_PORT.OUTCLR = O_LCD_A0

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
  LCD_CS_N_ACTIVE;
  lcd_spi_tx(c);
  WAIT_USART_TX_FIN (LCD_USART);
  LCD_CS_N_INACTIVE;
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

  LCD_PORT.DIRSET = O_LCD_CS_N | O_LCD_A0 | O_LCD_SCK_P | O_LCD_MOSI;
  // Setup pin states and USART in MSPI mode.

  LCD_CS_N_INACTIVE;
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

  //lcdSendCtl(SSD1309_CMD_SETCONTRAST);//--set contrast control register
  //lcdSendCtl(0x32);

  lcdSendCtl(SSD1309_CMD_SETPRECHARGE);      // Set Pre-Charge Period
  lcdSendCtl(0xAA); //   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
  //     D[3:0] => Phase 1 Period (discharge) in 1~15 Display Clocks
  //     D[7:4] => Phase 2 Period (charge) in 1~15 Display Clocks

  lcdSendCtl(SSD1309_CMD_SETVCOMDETECT);      // Set VCOMH Deselect Level
  lcdSendCtl(0x00);      // 34  Default => 0x34 (0.78*VCC)

  //lcdSendCtl(0xad);//--Set Master Configuration
  //lcdSendCtl(0x8e);//--
  //cdSendCtl(0xd8);//--Set Area Color Mode On/Off & Low Power Display Mode
  //lcdSendCtl(0x05);//

  lcdSendCtl(SSD1309_CMD_NORMALDISPLAY); //--set normal display
  lcdSendCtl(SSD1309_CMD_DISPLAYALLON);  //Disable Entire Display On
  lcdSendCtl(SSD1309_CMD_DISPLAYWAKEUP); //--turn on oled panel
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

  static uint8_t
  page = 0;
  static volatile uint8_t *p = displayBuf;

  lcdSendCtl(SSD1309_CMD_SETMEMORYADDRESSMODE);    // Set Memory Addressing Mode
  lcdSendCtl(0x02);      //   Default => 0x02
  //     0x00 => Horizontal Addressing Mode
  //     0x01 => Vertical Addressing Mode
  //     0x02 => Page Addressing Mode  ... no increment between pages.

  lcdSendCtl(SSD1309_CMD_SET_COLUMN_ADDRESS_LSB(0)); // Set LS nibble column RAM address 0
  lcdSendCtl(SSD1309_CMD_SET_COLUMN_ADDRESS_MSB(0)); // Set MS nibble column RAM address 0
  lcdSendCtl(SSD1309_CMD_SET_PAGE_ADDRESS(page++));

  LCD_A0_HI;
  LCD_CS_N_ACTIVE;

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
  LCD_CS_N_INACTIVE;
}


void lcdRefresh()
{
  /* Although this uses DMA, it is a blocking process so that it does not upset the timing.
   * e.g. During startup a whole screen needs to be displayed before continuing.
   */
  while (DMA.CH2.CTRLA & DMA_ENABLE_bm);

  lcdSendCtl(SSD1309_CMD_SETMEMORYADDRESSMODE); // Set Memory Addressing Mode
  lcdSendCtl(0x00);      //   Default => 0x02
  //     0x00 => Horizontal Addressing Mode
  //     0x01 => Vertical Addressing Mode
  //     0x02 => Page Addressing Mode  .. no increment between pages.

  lcdSendCtl(SSD1309_CMD_SET_COLUMN_START_AND_END_ADDRESS);
  lcdSendCtl(0); // Start
  lcdSendCtl(127); // End
  lcdSendCtl(SSD1309_CMD_SET_PAGE_ADDRESS(0));

  lcdSendCtl(SSD1309_CMD_SETCONTRAST);
  //  lcdSendCtl(g_eeGeneral.blOnBright << 4);
  lcdSendCtl(g_eeGeneral.contrast << 1);
  lcdSendCtl(SSD1309_CMD_DISPLAYALLON_RESUME); // Turn on OLED panel

  LCD_A0_HI;
  LCD_CS_N_ACTIVE;
  // The DMA transaction comprises, burst transfer of 1 byte, block size is 128 bytes, repeat count is 8.
  DMA.CTRL |= DMA_ENABLE_bm;
  DMA.CTRL |= DMA_PRIMODE_CH0123_gc;
  DMA.CH2.CTRLA = DMA_CH_RESET_bm;
  DMA.CH2.ADDRCTRL = DMA_CH_SRCRELOAD_NONE_gc | DMA_CH_SRCDIR_INC_gc
      | DMA_CH_DESTDIR_FIXED_gc | DMA_CH_DESTRELOAD_NONE_gc;
  DMA.CH2.TRIGSRC = DMA_CH_TRIGSRC_USARTD1_DRE_gc;
  DMA.CH2.TRFCNT = LCD_W;
  DMA.CH2.REPCNT = 8;
  DMA.CH2.SRCADDR0 = (((uint16_t) & displayBuf) >> 0 * 8) & 0xFF;
  DMA.CH2.SRCADDR1 = (((uint16_t) & displayBuf) >> 1 * 8) & 0xFF;
  DMA.CH2.SRCADDR2 = 0;
  DMA.CH2.DESTADDR0 = (((uint16_t) & LCD_USART.DATA) >> 0 * 8) & 0xFF;
  DMA.CH2.DESTADDR1 = (((uint16_t) & LCD_USART.DATA) >> 1 * 8) & 0xFF;
  DMA.CH2.DESTADDR2 = 0;

  DMA.CH2.CTRLB |= DMA_CH_TRNIF_bm;
  DMA.CH2.CTRLA = DMA_ENABLE_bm | DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_1BYTE_gc
      | DMA_CH_REPEAT_bm;

  while(DMA.CH2.CTRLA & DMA_ENABLE_bm); // Channel should auto disable.

  DMA.CH2.CTRLB = DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm; // Clear flags.

  _delay_us(20); // The USART is double buffered ... DMA transaction has ended, but USART is still running.
  LCD_CS_N_INACTIVE;
}


