/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2020).
 *
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
//#define BITBANGSPI
//#define SPI9BIT // Bitbang only !

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
#define ST756n_CMD_COLUMN_ADDRESS_SET_MSB( column )   (0x10 | (column))
#define ST756n_CMD_COLUMN_ADDRESS_SET_LSB( column )   (0x00 | (column))


#define LCD_PORT       PORTD
#define O_LCD_MOSI     PIN7_bm
#define O_LCD_SCK_P    PIN5_bm

#define LCD_CLK_HI     LCD_PORT.OUTSET = O_LCD_SCK_P
#define LCD_CLK_LO     LCD_PORT.OUTCLR = O_LCD_SCK_P

#if defined (BITBANGSPI)
#define LCD_MOSI_HI    LCD_PORT.OUTSET = O_LCD_MOSI
#define LCD_MOSI_LO    LCD_PORT.OUTCLR = O_LCD_MOSI
#define WAIT_LCD_TX_FIN
#else
#define LCD_SPI    SPID
#define WAIT_LCD_BUFFER_EMPTY  while (! (LCD_SPI.STATUS & SPI_IF_bm)) // Same as below due to reused USART MPSPI code.
#define WAIT_LCD_TX_FIN        while (! (LCD_SPI.STATUS & SPI_IF_bm)) // Wait for USART transmit shift register to empty.
#endif

#define O_LCD_CS           PIN4_bm

#if defined (POSITIVE_CHIP_SELECT)
#define LCD_CS_ACTIVE      LCD_PORT.OUTSET = O_LCD_CS
#define LCD_CS_INACTIVE    LCD_PORT.OUTCLR = O_LCD_CS
#else
// Negative Chip select.
#define LCD_CS_INACTIVE    LCD_PORT.OUTSET = O_LCD_CS
#define LCD_CS_ACTIVE      LCD_PORT.OUTCLR = O_LCD_CS
#endif

#if !defined (SPI9BIT) // 8 bit spi and bitbang.
#define O_LCD_CMD_DATA             PIN1_bm
#define LCD_CMD_DATA_HI            LCD_PORT.OUTSET = O_LCD_CMD_DATA
#define LCD_CMD_DATA_LO            LCD_PORT.OUTCLR = O_LCD_CMD_DATA
#endif

#define NUMITERATIONFULLREFRESH  8

void lcd_spi_tx(uint8_t c)
{
#if defined (BITBANGSPI)
#if defined (SPI9BIT)
  // Send CMD_DATA bit first then b7 .. b0.
  // Clock line inactive = low. Clock rising = sample.
  LCD_MOSI_HI; // Data not command.
  LCD_CLK_LO;
  LCD_CLK_HI; // Latch MISO.
#endif

  uint8_t n = 8;

  do {
    if (c & 0x80)
      LCD_MOSI_HI;
    else
      LCD_MOSI_LO;
    LCD_CLK_LO; //
    c = c << 1;
//    _NOP();
    LCD_CLK_HI; // Latch MISO.
  } while (--n);
#else
  WAIT_LCD_BUFFER_EMPTY;
  LCD_SPI.DATA = c;
#endif
}

void lcdSendCmd(uint8_t c)
{
#if defined (BITBANGSPI)
  LCD_CS_ACTIVE;

#if defined (SPI9BIT)
  // Send CMD_DATA bit first then b7 .. b0.
  // Clock line inactive = low. Clock rising = sample.
  LCD_MOSI_LO; // Command not data.
  LCD_CLK_LO;
  LCD_CLK_HI; // Latch MISO.
#else
  LCD_CMD_DATA_LO;
#endif
  uint8_t n = 8;

  do {
    if (c & 0x80)
      LCD_MOSI_HI;
    else
      LCD_MOSI_LO;
    LCD_CLK_LO; //
    c = c << 1;
//    _NOP();
    LCD_CLK_HI; // Latch MISO.
  } while (--n);

  LCD_CS_INACTIVE;
#else
  WAIT_LCD_BUFFER_EMPTY;
  LCD_CMD_DATA_LO;
  LCD_CS_ACTIVE;
  lcd_spi_tx(c);
  WAIT_LCD_TX_FIN;
  LCD_CS_INACTIVE;
#endif
}


void lcdSetRefVolt(uint8_t val)
{
#if defined(BITBANG)
  #if defined (LCD_EVO)
  lcdSendCmd(ST756n_CMD_CONTRAST_SET); lcdSendCmd(val + 0);
#endif
#if defined (LCD_ST7567)
  lcdSendCmd(ST756n_CMD_CONTRAST_SET); lcdSendCmd(val + 25);
#endif
#if defined (LCD_SSD1309)
  lcdSendCmd(ST756n_CMD_CONTRAST_SET); lcdSendCmd(val + 20);
#endif
#else
  // Interrupt driven SPI uses lcdRefreshFast().
#endif
}


void lcdInit()
{
  // Setup pin states and MSPI mode.
  // LCD Reset pin is now controlled by RTC reset signal.
  // Remap SPIC MOSI and SCK pins.

#if !defined(PORTD_REMAP)
#define PORTD_REMAP  _SFR_MEM8(0x066E)
#endif

#if !defined(PORT_SPI_bm)
#define PORT_SPI_bm  0x20  /* SPI bit mask. */
#endif

  PORTD_REMAP |= PORT_SPI_bm; // Swap MOSI and SCK.

  LCD_CS_INACTIVE;
  LCD_CLK_HI;

  LCD_PORT.DIRSET = O_LCD_CS | O_LCD_SCK_P | O_LCD_MOSI;

  #if !defined (SPI9BIT) // 8 bit spi and bitbang.
  LCD_PORT.DIRSET = O_LCD_CMD_DATA;
#endif

  _delay_ms(250);

#if !defined (BITBANGSPI)
  // Initialisation of MSPI.
  // Enable SPI as Master, MSB first, SPI Mode 3.
  // Clock leading edge falling = data setup. Clock trailing edge rising = sample.
  LCD_SPI.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | (0b11<<SPI_MODE_gp);
  LCD_SPI.CTRL &= ~SPI_DORD_bm;
  // Note : Make sure Slave Select pin is output or input pullup.

  MSPI_2M(LCD_SPI);
  // Set SPI_IF Flag for first time.
  LCD_SPI.DATA = 0;

#endif

  _delay_us(300);

#if defined (LCD_SSD1309)
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

lcdSendCmd(SSD1309_CMD_SETPRECHARGE);      // Set Pre-Charge Period
lcdSendCmd(0xAA); //   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
//     D[3:0] => Phase 1 Period (discharge) in 1~15 Display Clocks
//     D[7:4] => Phase 2 Period (charge) in 1~15 Display Clocks

lcdSendCmd(SSD1309_CMD_SETVCOMDETECT);      // Set VCOMH Deselect Level
lcdSendCmd(0x00);      // 34  Default => 0x34 (0.78*VCC)

lcdSendCmd(SSD1309_CMD_NORMALDISPLAY); //--set normal display
lcdSendCmd(SSD1309_CMD_DISPLAYALLON);  //Disable Entire Display On
lcdSendCmd(SSD1309_CMD_DISPLAYWAKEUP); //--turn on oled panel
lcdSendCmd(SSD1309_CMD_DISPLAYALLON_RESUME); // Turn on OLED panel
#else

lcdSendCmd(ST756n_CMD_RESET);
_delay_us(300);

#if defined (LCD_ST7567)
lcdSendCmd(ST756n_CMD_BIAS_SELECT_7TH); // Select bias 1/7 (at 1/65 duty) not 1/9.
#endif

#if !defined (REVERSE_DISPLAY)
lcdSendCmd(ST756n_CMD_NORMAL_SEG_DIRECTION);
lcdSendCmd(ST756n_CMD_REVERSE_COM_DIRECTION);
#else
lcdSendCmd(ST756n_CMD_REVERSE_SEG_DIRECTION);
lcdSendCmd(ST756n_CMD_NORMAL_COM_DIRECTION);
#endif

#if defined (LCD_ST7567)
lcdSendCmd(ST756n_CMD_VOLTAGE_RESISTOR_RATIO_6dec0);
#endif

#if defined (LCD_EVO)
lcdSendCmd(ST756n_CMD_VOLTAGE_RESISTOR_RATIO_5dec5);
#endif

lcdSendCmd(ST756n_CMD_POWER_CTRL | 4);
_delay_us(300);
lcdSendCmd(ST756n_CMD_POWER_CTRL | 6);
_delay_us(300);
lcdSendCmd(ST756n_CMD_POWER_CTRL | 7);
_delay_us(300);

lcdClearRAM();
lcdSendCmd(ST756n_CMD_DISPLAY_ON); // Display on.
#endif
}


volatile uint8_t * lcd_p = displayBuf;
volatile uint8_t lcd_x_pos;
uint8_t lcd_page = 0;


void lcdRefreshFast()
{
#if defined(SHOWDURATION)
  lcdDrawNumberAttUnit(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif

#if !defined(BITBANGSPI)
  if (LCD_SPI.INTCTRL & SPI_INTLVL_gm) return;
#endif
  SHOWDURATIONLCD1

#if !defined(BITBANG)
#if defined (LCD_EVO)
  lcdSendCmd(ST756n_CMD_CONTRAST_SET); lcdSendCmd(g_eeGeneral.contrast);
#endif
#if defined(LCD_ST7567)
  lcdSendCmd(ST756n_CMD_CONTRAST_SET); lcdSendCmd(g_eeGeneral.contrast + 25);
#endif
#if defined(LCD_SSD1309)
  lcdSendCmd(ST756n_CMD_CONTRAST_SET); lcdSendCmd(g_eeGeneral.contrast + 20);
#endif
#endif

#if defined (LCD_SIZE_132X64)
    lcdSendCmd(ST756n_CMD_COLUMN_ADDRESS_SET_LSB(2)); // Set LS nibble column RAM address 2
#else
    lcdSendCmd(ST756n_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
#endif
    lcdSendCmd(ST756n_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
    lcdSendCmd(ST756n_CMD_PAGE_ADDRESS_SET(lcd_page ++));

#if !defined (SPI9BIT)
    LCD_CMD_DATA_HI;
#endif

#if defined(BITBANGSPI)
    LCD_CS_ACTIVE;

    for(lcd_x_pos = LCD_W; lcd_x_pos > 0; lcd_x_pos--) {
      lcd_spi_tx( *lcd_p++ );
    }

    WAIT_LCD_TX_FIN;
    LCD_CS_INACTIVE;

    if (lcd_page > 7)
    {
      lcd_page = 0;
      lcd_p = displayBuf;
    }
#else
    LCD_CS_ACTIVE;
    lcd_x_pos = LCD_W;
    LCD_SPI.INTCTRL |= SPI_INTLVL_LO_gc; // Start interrupt driven SPI.
#endif

  SHOWDURATIONLCD2
}


void lcdRefresh()
{
#if !defined(BITBANGSPI)
  while(LCD_SPI.INTCTRL & SPI_INTLVL_gm);
#endif

  for(uint8_t i=NUMITERATIONFULLREFRESH; i>0; i--)
  {
    lcdRefreshFast();

#if !defined(BITBANGSPI)
    while(LCD_SPI.INTCTRL & SPI_INTLVL_gm);
#endif
  }
}


#if !defined(BITBANGSPI)
ISR(SPID_INT_vect)
{
  if (lcd_x_pos --)
  {
    LCD_SPI.DATA = *lcd_p++;
  }
  else
  {
    LCD_SPI.INTCTRL &= ~SPI_INTLVL_gm;
    LCD_CS_INACTIVE;
    LCD_SPI.DATA = 0x00; // Set SPI_IF Flag.

    if (lcd_page > 7)
    {
      lcd_page = 0;
      lcd_p = displayBuf;
    }
  }
}
#endif


void lcdClearRAM(void)
{
  for(uint8_t page = 0; page < 8; page ++)
    {
      lcdSendCmd(ST756n_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
      lcdSendCmd(ST756n_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
      lcdSendCmd(ST756n_CMD_PAGE_ADDRESS_SET(page));

#if !defined (SPI9BIT)
    LCD_CMD_DATA_HI;
#endif

    LCD_CS_ACTIVE;

#if defined (LCD_SIZE_132X64)
      uint8_t column = 132;
#else
      uint8_t column = LCD_W; // 128.
#endif

      while(column--)  lcd_spi_tx(0x00);

      WAIT_LCD_TX_FIN;
      LCD_CS_INACTIVE;
    }
}


