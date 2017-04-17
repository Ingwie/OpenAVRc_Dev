 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
 *                                                                        *
 *   OpenAVRc is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *   OpenAVRc is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
 *                                                                        *
 **************************************************************************
*/


/*
 *****************************************************************
 * LCD driver for a 132 * 64 LCD as fitted in the Evo/Royal/SX TX.
 * Driver uses a USART in MSPI mode.
 * All information was derived by tracing the SPI messages.
 * LCD looks to be ST7565 compatible.
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


#define XCK2_PORT               PORTH
#define XCK2_DDR                DDRH
#define XCK2_PIN                PIN2_bm // port bit PH2

#define WAIT_TX_FIN()           while(! (UCSR2A & (1 << TXC2)) )

#define PORT_H_LCD_RES_N        PORTH
#define OUT_H_LCD_RES_N         PIN0_bm
#define LCD_RES_N_ACTIVE()      PORTH &= ~OUT_H_LCD_RES_N
#define LCD_RES_N_INACTIVE()    PORTH |=  OUT_H_LCD_RES_N

#define PORT_C_LCD_CS_P         PORTC
#define OUT_C_LCD_CS_P          PIN0_bm
#define LCD_CS_P_ACTIVE()       PORTC |=  OUT_C_LCD_CS_P
#define LCD_CS_P_INACTIVE()     PORTC &= ~OUT_C_LCD_CS_P

#define PORT_C_LCD_A0           PORTC
#define OUT_C_LCD_A0            PIN1_bm
#define LCD_A0_HIGH()           PORTC |=  OUT_C_LCD_A0
#define LCD_A0_LOW()            PORTC &= ~OUT_C_LCD_A0

void lcd_mspi(char c);

#define NUMITERATIONFULLREFRESH  2

void lcd_mspi(char c)
{
  while(! (UCSR2A & (1 << UDRE2)) ); // Wait for USART buffer (UDRE) to empty.
/*
********************************************************************************************
The following code was made atomic as the USART logic does not clear the
USART transmit complete flag TXC automatically when data is written to the UDR.
For code using polling we clear the TXC flag after writing to the UDR.
********************************************************************************************
*/

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    UDR2 = c;
    UCSR2A |= (1<<TXC2); // Clear USART transmit complete flag.
  }
}


void lcdSendCtl(uint8_t val)
{
  LCD_A0_LOW();
  LCD_CS_P_ACTIVE();
  lcd_mspi(val); // USART MSPI
  WAIT_TX_FIN(); // Wait for USART transmit shift register to empty.
  LCD_CS_P_INACTIVE();
}


void lcdSetRefVolt(uint8_t val)
{
  lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET);// Electronic volume mode set ? Range = 0x09 to 0x19 (-8 to +8 on Evo)
  lcdSendCtl(val>>1);
}


inline void lcdInit()
{
  // Setup pin states and USART in MSPI mode.
  DDRH |= OUT_H_LCD_RES_N;
  LCD_RES_N_ACTIVE();

  DDRC |= OUT_C_LCD_CS_P | OUT_C_LCD_A0;
  LCD_CS_P_INACTIVE();
  LCD_A0_LOW();

  XCK2_DDR |= XCK2_PIN;
  XCK2_PORT &= ~XCK2_PIN;

  _delay_ms(250);

  // Initialisation of USART.
  UBRR2 = 0; // Reset is part of initialisation sequence.

  UCSR2C = 0xC3; // UMSEL21:0 = 3 DORD2=0 CPHA2=1 CPOL2=1  USART in Master SPI mode, MSB first, Clock phase=1 Polarity=1.
  UCSR2B = (0 << RXEN2) | (1 << TXEN2); // Transmit only, use RX pin as Reset.
  UBRR2 = 3; // 2.0MHz clock ... 16MHz/(2*(UBRR+1))

  LCD_RES_N_INACTIVE();
  _delay_us(300);

  lcdSendCtl(ST7565R_CMD_RESET);
  _delay_us(300);
  lcdSendCtl(0x2C);// Power controller set, booster cct on, V reg off, V follower off
  _delay_us(300);
  lcdSendCtl(0x2E);// Power controller set, booster cct on, V reg on, V follower off
  _delay_us(300);
  lcdSendCtl(0x2F);// Power controller set, booster cct on, V reg on, V follower on
  _delay_us(300);
  lcdSendCtl(ST7565R_CMD_VOLTAGE_RESISTOR_RATIO_5);

  lcdSendCtl(ST7565R_CMD_DISPLAY_OFF);
  // Turn display on in lcdRefresh() ... prevents displaying garbage at startup, which seems to happen more when sdcards are attached.

  lcdSendCtl(ST7565R_CMD_ELECTRONIC_VOLUME_MODE_SET);// Electronic volume mode set ? Range = 0x09 to 0x19 (-8 to +8 on Evo)
  lcdSendCtl(0x09);// See above ... 0 = 0x11
  lcdSendCtl(ST7565R_CMD_REVERSE_SCAN_DIRECTION);
  lcdSendCtl(ST7565R_CMD_DISPLAY_ALL_POINTS_OFF);
  // lcdSendCtl(0x40); // Set display start line to 0 *** Added due to random display scrolling ***

  g_eeGeneral.contrast = 0x09;
}


void lcdRefreshFast()
{
#if defined(SHOWDURATION)
  lcdDrawNumberAttUnit(16*FW, 1, DURATION_MS_PREC2(DurationValue), PREC2);
#endif
  SHOWDURATIONLCD1

  static uint8_t offset = 0;
  uint8_t * p=displayBuf;
  if(offset) p += 512;

  lcdSendCtl(ST7565R_CMD_DISPLAY_ON);
//  lcdSendCtl(0x40); // Set display start line to 0 *** Added due to random display scrolling ***

  for(uint8_t page=0; page<4; page++) {
    lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_LSB(0)); // Set LS nibble column RAM address 0
    lcdSendCtl(ST7565R_CMD_COLUMN_ADDRESS_SET_MSB(0)); // Set MS nibble column RAM address 0
    lcdSendCtl(ST7565R_CMD_PAGE_ADDRESS_SET(page + offset));

    LCD_A0_HIGH();
    LCD_CS_P_ACTIVE();
    lcd_mspi(0x00); // Pad out the four unused columns.
    lcd_mspi(0x00);
      for(unsigned char x=LCD_W; x>0; --x) {
      lcd_mspi( *p++ );
      }
    lcd_mspi(0x00);
    lcd_mspi(0x00);
    WAIT_TX_FIN();
    LCD_CS_P_INACTIVE();
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
