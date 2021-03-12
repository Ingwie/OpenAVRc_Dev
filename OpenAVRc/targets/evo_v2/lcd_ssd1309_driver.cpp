/*
 * Authors (alphabetical order)
 *
 * - Rich Kilvington (2019).
 *
 */

/*
 *****************************************************************
 * Driver for a SSD1309 OLED Display.
 * Written for Xmega SPI and Bit-Bang.
 * Adapted from USART MSPI code.
 * As this is a uni-directional driver, choose Bit-Bang to reclaim
 * the unused MISO pin.
 * 8 bit Bit-Bang (4 wire SPI mode) tested.
 * 9 bit Bit-Bang (3-wire SPI mode) cannot be tested due to
 * BS0 connection being inaccessible on the module.
 * USART MSPI would be better and can do DMA
 * too unlike the SPI port.
 * No Reset signal as that is derived from RTC chip.
 *****************************************************************
 */

//#define BITBANGSPI
#define SPI8BIT

#if !defined (BITBANGSPI)
#include "spi.h"
#endif

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

#if !defined (BITBANGSPI)
#define LCD_SPI    SPID
#define WAIT_LCD_BUFFER_EMPTY  while (! (LCD_SPI.STATUS & SPI_IF_bm)) // Same as below due to reused USART MPSPI code.
#define WAIT_LCD_TX_FIN        while (! (LCD_SPI.STATUS & SPI_IF_bm)) // Wait for USART transmit shift register to empty.
#else
#define LCD_MOSI_HI    LCD_PORT.OUTSET = O_LCD_MOSI
#define LCD_MOSI_LO    LCD_PORT.OUTCLR = O_LCD_MOSI
#define WAIT_LCD_TX_FIN
#endif

#define O_LCD_CS_N           PIN4_bm
#define LCD_CS_N_INACTIVE    LCD_PORT.OUTSET = O_LCD_CS_N
#define LCD_CS_N_ACTIVE      LCD_PORT.OUTCLR = O_LCD_CS_N

#if defined (SPI8BIT)
#define O_LCD_CMD_DATA             PIN1_bm
#define LCD_CMD_DATA_HI            LCD_PORT.OUTSET = O_LCD_CMD_DATA
#define LCD_CMD_DATA_LO            LCD_PORT.OUTCLR = O_LCD_CMD_DATA
#endif

#define NUMITERATIONFULLREFRESH  2



void lcd_spi_tx(uint8_t c)
#if defined (BITBANGSPI)
{
  LCD_CLK_LO;

#if !defined (SPI8BIT)
  // Send CMD_DATA bit first then b7 .. b0.
  // Clock line inactive = low. Clock rising = sample.
  LCD_MOSI_HI; // Data.
  LCD_CLK_HI; // Latch MISO.
#endif

  uint8_t n = 8;

    do
    {
      LCD_CLK_LO;

      if (c & 0x80)
        LCD_MOSI_HI;
      else
        LCD_MOSI_LO;

      c = c << 1;
      LCD_CLK_HI; // Latch MISO.
    }
    while(--n);

  LCD_CLK_LO;
}
#else
{
  WAIT_LCD_BUFFER_EMPTY;
  LCD_SPI.DATA = c;
}
#endif



void lcdSendCmd(uint8_t c)
#if defined (BITBANGSPI)
{
  LCD_CLK_LO;
  LCD_CS_N_ACTIVE;

#if defined (SPI8BIT)
  LCD_CMD_DATA_LO;
#else
  // Send CMD_DATA bit first then b7 .. b0.
  // Clock line inactive = low. Clock rising = sample.
  LCD_MOSI_LO; // Command.
  LCD_CLK_HI; // Latch MISO.
#endif

  uint8_t n = 8;

    do
    {
      LCD_CLK_LO;

      if (c & 0x80)
        LCD_MOSI_HI;
      else
        LCD_MOSI_LO;

      c = c << 1;
      LCD_CLK_HI; // Latch MISO.
    }
    while(--n);

  LCD_CLK_LO;
  LCD_CS_N_INACTIVE;
}
#else
{
  LCD_CMD_DATA_LO;
  LCD_CS_N_ACTIVE;
  lcd_spi_tx(c);
  WAIT_LCD_TX_FIN;
  LCD_CS_N_INACTIVE;
}
#endif


void lcdSetRefVolt(uint8_t val)
{
  lcdSendCmd(SSD1309_CMD_SETCONTRAST);
//  lcdSendCmd(g_eeGeneral.blOnBright << 4);
  lcdSendCmd(g_eeGeneral.contrast << 1);
}


void lcdInit()
{
  // Setup pin states and MSPI mode.
  // LCD Reset pin is now controlled by RTC reset signal.
  // Remap SPIC MOSI and SCK pins.

  LCD_PORT.DIRSET = O_LCD_CS_N | O_LCD_SCK_P | O_LCD_MOSI;

#if !defined (BITBANGSPI)
  PORTD_REMAP |= PORT_SPI_bm; // Swap MOSI and SCK.
  LCD_PORT.DIRSET |= O_LCD_CMD_DATA;
  LCD_CMD_DATA_LO;
#endif
#if defined (SPI8BIT)
  LCD_PORT.DIRSET |= O_LCD_CMD_DATA;
  LCD_CMD_DATA_LO;
#endif

  LCD_CS_N_INACTIVE;
  LCD_CLK_LO;
  _delay_ms(100);

#if !defined (BITBANGSPI)
  // Initialisation of MSPI.
  // Enable SPI as Master, MSB first, SPI Mode 0.
  // Clock leading edge falling = data setup. Clock trailing edge rising = sample.
  LCD_SPI.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | (0b00<<SPI_MODE_gp);
  LCD_SPI.CTRL &= ~SPI_DORD_bm;
  // Note : Make sure Slave Select pin is output or input pullup.

  MSPI_4M(LCD_SPI);
  // Set SPI_IF Flag for first time.
  LCD_SPI.DATA = 0;
#endif

  _delay_us(300);

  lcdSendCmd(SSD1309_CMD_SETCMDLOCK);  // Set Command Lock
  lcdSendCmd(0x12);  // Unlock
  //     0x12 => Driver IC interface is unlocked from entering command.
  //     0x16 => All Commands are locked except 0xFD.

  lcdSendCmd(SSD1309_CMD_DISPLAYSLEEP);            //--turn off oled panel

  lcdSendCmd(SSD1309_CMD_SETDISPLAYCLOCKDIV); // Set Display Clock Divide Ratio / Oscillator Frequency
  lcdSendCmd(0x70);      //   Default => 0x70
  //     D[3:0] => Display Clock Divider
  //     D[7:4] => Oscillator Frequency

  lcdSendCmd(SSD1309_CMD_SETMULTIPLEX);         //--set multiplex ratio(1 to 64)
  lcdSendCmd(0x3f);            //--1/64 duty

  lcdSendCmd(SSD1309_CMD_SETDISPLAYOFFSET); //Set Display Offset
  lcdSendCmd(0x00);

  lcdSendCmd(SSD1309_CMD_SETSTARTLINE(0));      // Set Display Start Line

  lcdSendCmd(SSD1309_CMD_SETMEMORYADDRESSMODE);    // Set Memory Addressing Mode
  lcdSendCmd(0x02);      //   Default => 0x02
  //     0x00 => Horizontal Addressing Mode
  //     0x01 => Vertical Addressing Mode
  //     0x02 => Page Addressing Mode  .. no increment between pages.

  lcdSendCmd(SSD1309_CMD_SETSEGMENTREMAPREV); //
  lcdSendCmd(SSD1309_CMD_SETCOMOUTPUTDIRREV); //

  lcdSendCmd(SSD1309_CMD_SETCOMPINSHARDCONF); //--Set COM Pins Hardware Configuration
  lcdSendCmd(0x12); //     Disable COM Left/Right Re-Map   Alternative COM Pin Configuration

  //lcdSendCmd(SSD1309_CMD_SETCONTRAST);//--set contrast control register
  //lcdSendCmd(0x32);

  lcdSendCmd(SSD1309_CMD_SETPRECHARGE);      // Set Pre-Charge Period
  lcdSendCmd(0xAA); //   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
  //     D[3:0] => Phase 1 Period (discharge) in 1~15 Display Clocks
  //     D[7:4] => Phase 2 Period (charge) in 1~15 Display Clocks

  lcdSendCmd(SSD1309_CMD_SETVCOMDETECT);      // Set VCOMH Deselect Level
  lcdSendCmd(0x00);      // 34  Default => 0x34 (0.78*VCC)

  lcdSendCmd(SSD1309_CMD_NORMALDISPLAY); //--set normal display
  lcdSendCmd(SSD1309_CMD_DISPLAYALLON);  //Disable Entire Display On
  lcdSendCmd(SSD1309_CMD_DISPLAYWAKEUP); //--turn on oled panel

  //g_eeGeneral.contrast = 0x09;
}


void lcdRefreshFast()
{
  SHOWDURATIONLCD1

  static uint8_t  offset = 0;
  uint8_t * p = displayBuf;
  if (offset)
    p += 512;

  lcdSendCmd(SSD1309_CMD_DISPLAYALLON_RESUME); //--turn on oled panel

  for (uint8_t page = 0; page < 4; page++) {

    lcdSendCmd(SSD1309_CMD_SET_PAGE_ADDRESS(offset + page));
    lcdSendCmd(SSD1309_CMD_SET_COLUMN_ADDRESS_LSB(0)); // Set LS nibble column RAM address 0
    lcdSendCmd(SSD1309_CMD_SET_COLUMN_ADDRESS_MSB(0)); // Set MS nibble column RAM address 0

#if defined (SPI8BIT)
    LCD_CMD_DATA_HI;
#endif

    LCD_CS_N_ACTIVE;

    for (unsigned char x = LCD_W; x > 0; --x) {
      lcd_spi_tx(*p++);
    }

#if !defined (BITBANGSPI)
    WAIT_LCD_TX_FIN;
#endif

    LCD_CS_N_INACTIVE;
  }
  offset ^= 4;
  SHOWDURATIONLCD2
}


void lcdRefresh()
{
  for (uint8_t i = NUMITERATIONFULLREFRESH; i > 0; i--) {
    lcdRefreshFast();
  }
}


