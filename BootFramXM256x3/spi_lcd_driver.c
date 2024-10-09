/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2020).
 *
 */

/*
 *****************************************************************
 * SPI LCD Driver.
 * Supports the following LCD panels.
 * SSD1309 OLED 128x64
 * ST7567 132x65 +CS signal
 * Original Multiplex EVO Transmitter LCD (ST7565? COF) 132x64 +CS signal
 * Written for Xmega SPI and Bit-Bang.
 * Adapted from USART MSPI code.
 * As this is a uni-directional driver, choose Bit-Bang to reclaim
 * the unused MISO pin.
 * USART MSPI would be better and can do DMA
 * too unlike the SPI port.
 * No Reset signal as that is derived from RTC chip / FRAM daughter board.
 *****************************************************************
*/

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "lcd.h"

#if !defined (BITBANGSPI)
 #include "spi.h"
#endif

const uint8_t desktop_icon[] PROGMEM = {
#include "desktop.lbm"
};

uint8_t displayBuf[DISPLAY_BUFER_SIZE];

void lcd_imgfar(coord_t x, coord_t y,  uint_farptr_t img, uint8_t idx, LcdFlags att) // progmem "far"
{
  uint_farptr_t q = img;
  uint8_t w    = pgm_read_byte_far(q++);
  uint8_t hb   = (pgm_read_byte_far(q++)+7)/8;

  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (coord_t i=0; i<w; i++) {
      uint8_t b = pgm_read_byte_far(q);
      q++;
      ASSERT_IN_DISPLAY(p);
      *p++ = b;
    }
  }
}


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
#define SSD1309_CMD_COLUMN_ADDRESS_SET_LSB( column )   (0x00 | (column))
#define SSD1309_CMD_COLUMN_ADDRESS_SET_MSB( column )   (0x10 | (column))
#define SSD1309_CMD_PAGE_ADDRESS_SET( page )           (0xB0 | (page))
#define SSD1309_CMD_SETCONTRAST           0x81
#define SSD1309_CMD_NORMALDISPLAY         0xA6
#define SSD1309_CMD_INVERTDISPLAY         0xA7
#define SSD1309_CMD_SETVCOMDETECT         0xDB
#define SSD1309_CMD_SETPRECHARGE          0xD9
#define SSD1309_CMD_SETSTARTLINE( line )               (0x40 | (line))

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

#if !defined (BITBANGSPI)
#define LCD_SPI    SPID
#define WAIT_LCD_BUFFER_EMPTY  while (! (LCD_SPI.STATUS & SPI_IF_bm)) // Same as below due to reused USART MPSPI code.
#define WAIT_LCD_TX_FIN        while (! (LCD_SPI.STATUS & SPI_IF_bm)) // Wait for USART transmit shift register to empty.
#else
#define LCD_MOSI_HI    LCD_PORT.OUTSET = O_LCD_MOSI
#define LCD_MOSI_LO    LCD_PORT.OUTCLR = O_LCD_MOSI
#define WAIT_LCD_TX_FIN
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


#define O_LCD_A0             PIN1_bm
#define LCD_A0_HI            LCD_PORT.OUTSET = O_LCD_A0
#define LCD_A0_LO            LCD_PORT.OUTCLR = O_LCD_A0

#define NUMITERATIONFULLREFRESH  2

#if !defined (BITBANGSPI)
void lcd_spi_tx(char c)
{
  WAIT_LCD_BUFFER_EMPTY;
  LCD_SPI.DATA = c;
}
#else
void lcd_spi_tx(uint8_t c) {
// MSB first. Clock line inactive = high. SPI Mode 3.
// Clock leading edge falling = data setup. Clock trailing edge rising = sample.
  uint8_t n = 8;

  do {
    if (c & 0x80)
      LCD_MOSI_HI;
    else
      LCD_MOSI_LO;
    LCD_CLK_LO; //
    c = c << 1;
    LCD_CLK_HI; // Latch MISO.
  } while (--n);
}
#endif

void lcdSendCtl(uint8_t c)
{
  LCD_A0_LO;
  LCD_CS_ACTIVE;
  lcd_spi_tx(c);
  WAIT_LCD_TX_FIN;
  LCD_CS_INACTIVE;
}

void lcdSetRefVolt(uint8_t val)
{
  lcdSendCtl(ST756n_CMD_CONTRAST_SET);
  lcdSendCtl(CONTRAST_MIN);
}


void lcdInit()
{
  memset(displayBuf, 0x00, DISPLAY_BUFER_SIZE);

  lcd_imgfar(32, 0, (pgm_get_far_address(desktop_icon)), 0, 0);

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

  LCD_PORT.DIRSET = O_LCD_CS | O_LCD_A0 | O_LCD_SCK_P | O_LCD_MOSI;
  LCD_CS_INACTIVE;
  LCD_A0_LO;
  LCD_CLK_HI;
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

#else

  lcdSendCtl(ST756n_CMD_RESET);
  _delay_us(300);

#if defined (LCD_ST7567)
  lcdSendCtl(ST756n_CMD_BIAS_SELECT_7TH); // Select bias 1/7 (at 1/65 duty) not 1/9.
#endif

#if !defined (FLIP_DISPLAY)
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

  lcdSendCtl(ST756n_CMD_DISPLAY_OFF); // display off
  lcdSendCtl(0x40 ); // start line 0
#endif
  lcdSetRefVolt(CONTRAST_MIN);
}


void lcdRefreshFast()
{
#if defined(SHOWDURATION)
  lcdDrawNumberAttUnit(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
  SHOWDURATIONLCD1

  static uint8_t offset = 0;
  uint8_t * p = displayBuf;
  if(offset) p += 512;

  for(uint8_t page=0; page<4; page++) {
    lcdSendCtl(ST756n_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
    lcdSendCtl(ST756n_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
    lcdSendCtl(ST756n_CMD_PAGE_ADDRESS_SET(page + offset));

    LCD_A0_HI;
    LCD_CS_ACTIVE;
    #if defined (LCD_SIZE_132X64)
    lcd_spi_tx(0x00); // Pad out the four unused columns.
    lcd_spi_tx(0x00);
    #endif
      for(unsigned char x=LCD_W; x>0; --x) {
      lcd_spi_tx( *p++ );
      }
    #if defined (LCD_SIZE_132X64)
    lcd_spi_tx(0x00);
    lcd_spi_tx(0x00);
    #endif
    WAIT_LCD_TX_FIN;
    LCD_CS_INACTIVE;
  }
  offset ^=4;
  SHOWDURATIONLCD2
}


void lcdRefresh()
{
  for(uint8_t i=NUMITERATIONFULLREFRESH; i>0; i--) {
    lcdRefreshFast();
  }
#if defined (LCD_SSD1309)
  lcdSendCtl(SSD1309_CMD_DISPLAYALLON_RESUME); // Turn on OLED panel.
#else
  lcdSendCtl(ST756n_CMD_DISPLAY_ON);
#endif
}

