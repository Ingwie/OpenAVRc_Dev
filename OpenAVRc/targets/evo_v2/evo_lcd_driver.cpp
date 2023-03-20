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

//#define BITBANGSPI

#if !defined (BITBANGSPI)
#include "spi.h"
#endif


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

//#define POSITIVE_CHIP_SELECT
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
  lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET); // Range = 0x09 to 0x19 (-8 to +8 on Evo setup screen)
  lcdSendCtl(val - CONTRAST_MIN);
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

 lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET);// Electronic volume mode set ? Range = 0x09 to 0x19 (-8 to +8 on Evo)
 lcdSendCtl(0x09);// See above ... 0 = 0x11

 lcdSendCtl(ST7565R_CMD_REVERSE_SCAN_DIRECTION);// Common output mode select
 lcdSendCtl(ST7565R_CMD_DISPLAY_ALL_POINTS_OFF);// All points off (normal)
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
    lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
    lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
    lcdSendCtl(ST7565R_CMD_PAGE_ADDRESS_SET(page + offset));

    LCD_A0_HI;
    LCD_CS_ACTIVE;
    lcd_spi_tx(0x00); // Pad out the four unused columns.
    lcd_spi_tx(0x00);
      for(unsigned char x=LCD_W; x>0; --x) {
      lcd_spi_tx( *p++ );
      }
    lcd_spi_tx(0x00);
    lcd_spi_tx(0x00);
    WAIT_LCD_TX_FIN;
    LCD_CS_INACTIVE;
  }
  offset ^=4;
  SHOWDURATIONLCD2
}


void lcdRefresh()
{
  lcdSendCtl(ST7565R_CMD_DISPLAY_ON);

  for(uint8_t i=NUMITERATIONFULLREFRESH; i>0; i--) {
    lcdRefreshFast();
  }
}
