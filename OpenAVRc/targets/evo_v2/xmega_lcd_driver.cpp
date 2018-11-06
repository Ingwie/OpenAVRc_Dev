/*
 * Authors (alphabetical order)
 *
 * - Rick Kilvington (2018).
 *
 */


/*
 *****************************************************************
 * LCD driver for a 132 * 64 LCD as fitted in the EVO transmitter.
 * Driver uses XMEGA SPI in Master SPI mode.
 * All information was derived by tracing the SPI messages.
 * LCD looks to be ST7565R compatible.
 *****************************************************************
*/

#include "spi.h"

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


#define LCD_SPI                SPID
#define LCD_PORT               PORTD

#define WAIT_LCD_BUFFER_EMPTY  while (! (LCD_SPI.STATUS & SPI_IF_bm)) // Same as below due to reused USART MPSPI code.
#define WAIT_LCD_TX_FIN        while (! (LCD_SPI.STATUS & SPI_IF_bm)) // Wait for USART transmit shift register to empty.

#define O_LCD_CS_P           PIN4_bm
#define LCD_CS_P_ACTIVE      LCD_PORT.OUTSET = O_LCD_CS_P
#define LCD_CS_P_INACTIVE    LCD_PORT.OUTCLR = O_LCD_CS_P

#define O_LCD_A0             PIN1_bm
#define LCD_A0_HIGH          LCD_PORT.OUTSET = O_LCD_A0
#define LCD_A0_LOW           LCD_PORT.OUTCLR = O_LCD_A0

void lcd_mspi(char c);

#define NUMITERATIONFULLREFRESH  2

void lcd_mspi(char c)
{
  WAIT_LCD_BUFFER_EMPTY;
  LCD_SPI.DATA = c;
}


void lcdSendCtl(uint8_t c)
{
  LCD_A0_LOW;
  LCD_CS_P_ACTIVE;
  lcd_mspi(c); // Master SPI.
  WAIT_LCD_TX_FIN;
  LCD_CS_P_INACTIVE;
}


void lcdSetRefVolt(uint8_t val)
{
  lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET); // Range = 0x09 to 0x19 (-8 to +8 on Evo setup screen)
  lcdSendCtl(val>>1);
}


void lcdInit()
{
  // Setup pin states and MSPI mode.
  // LCD Reset pin is now controlled by RTC reset signal.
  // Remap SPIC MOSI and SCK pins.
#if 0
  #if !defined(PORTD_REMAP)
  #define PORTD_REMAP  _SFR_MEM8(0x066E)
  #endif
  #if !defined(PORT_SPI_bm)
  #define PORT_SPI_bm  0x20  /* SPI bit mask. */
  #endif
  LCD_PORT.REMAP |= PORT_SPI_bm; // Swap MOSI and SCK.
#endif

  LCD_PORT.DIRSET = O_LCD_CS_P | O_LCD_A0 | PIN5_bm | PIN7_bm;
  LCD_CS_P_INACTIVE;
  LCD_A0_LOW;
  _delay_ms(250);

  // Initialisation of MSPI.
  // Enable SPI as Master, MSB first, SPI Mode 3.
  // Clock Leading edge falling = data setup. Clock trailing edge rising = sample.
  LCD_SPI.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | (0b11<<SPI_MODE_gp);
  LCD_SPI.CTRL &= ~SPI_DORD_bm;
  // Note : Make sure Slave Select pin is output or input pullup.

  MSPI_2M(LCD_SPI);
  // Set SPI_IF Flag for first time.
  LCD_SPI.DATA =0;

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

 g_eeGeneral.contrast = 0x09;
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

  lcdSendCtl(ST7565R_CMD_DISPLAY_ON);

  for(uint8_t page=0; page<4; page++) {
    lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
    lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
    lcdSendCtl(ST7565R_CMD_PAGE_ADDRESS_SET(page + offset));

    LCD_A0_HIGH;
    LCD_CS_P_ACTIVE;
    lcd_mspi(0x00); // Pad out the four unused columns.
    lcd_mspi(0x00);
      for(unsigned char x=LCD_W; x>0; --x) {
      lcd_mspi( *p++ );
      }
    lcd_mspi(0x00);
    lcd_mspi(0x00);
    WAIT_LCD_TX_FIN;
    LCD_CS_P_INACTIVE;
  }
  offset ^=4;
  SHOWDURATIONLCD2
}


void lcdRefresh()
{
  for(uint8_t i=NUMITERATIONFULLREFRESH; i>0; i--) {
    lcdRefreshFast();
  }
}
